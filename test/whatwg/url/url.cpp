#include <boost/test/unit_test.hpp>

#include <algorithm>
#include <deque>
#include <string>
#include <string_view>

#include <wordring/compatibility.hpp>
#include <wordring/whatwg/url/url.hpp>

BOOST_AUTO_TEST_SUITE(whatwg_url_test)

BOOST_AUTO_TEST_CASE(url_is_special_scheme_1)
{
	using namespace wordring::whatwg;
	BOOST_CHECK(is_special_scheme(U"file"));
}

BOOST_AUTO_TEST_CASE(url_default_port_1)
{
	using namespace wordring::whatwg;
	BOOST_CHECK(*default_port(U"http") == 80);
}

BOOST_AUTO_TEST_CASE(url_default_port_2)
{
	using namespace wordring::whatwg;
	BOOST_CHECK(!default_port(U"file"));
}

BOOST_AUTO_TEST_CASE(url_is_special_1)
{
	using namespace wordring::whatwg;
	url_record_base url;
	url.m_scheme = U"http";
	BOOST_CHECK(is_special(url));
}

BOOST_AUTO_TEST_CASE(url_includes_credentials_1)
{
	using namespace wordring::whatwg;
	url_record_base url;
	url.m_username = U"user";
	BOOST_CHECK(includes_credentials(url));
}

BOOST_AUTO_TEST_CASE(url_cannot_have_username_or_password_or_port_1)
{
	using namespace wordring::whatwg;
	url_record_base url;
	BOOST_CHECK(cannot_have_username_or_password_or_port(url));
}

BOOST_AUTO_TEST_CASE(url_is_windows_drive_letter_1)
{
	using namespace wordring::whatwg;
	BOOST_CHECK(is_windows_drive_letter(U'A', U'|'));
}

BOOST_AUTO_TEST_CASE(url_is_windows_drive_letter_2)
{
	using namespace wordring::whatwg;
	BOOST_CHECK(is_windows_drive_letter(std::u32string{ U'A', U'|' }));
}

BOOST_AUTO_TEST_CASE(url_is_normalized_windows_drive_letter_1)
{
	using namespace wordring::whatwg;
	BOOST_CHECK(is_windows_drive_letter(U'A', U':'));
}

BOOST_AUTO_TEST_CASE(url_is_normalized_windows_drive_letter_2)
{
	using namespace wordring::whatwg;
	BOOST_CHECK(is_windows_drive_letter(std::u32string{ U'A', U':' }));
}

BOOST_AUTO_TEST_CASE(url_starts_with_windows_drive_letter_1)
{
	using namespace wordring::whatwg;
	std::u32string s = U"A:/start with ...";
	BOOST_CHECK(starts_with_windows_drive_letter(s.data(), s.data() + s.size()));
}

BOOST_AUTO_TEST_CASE(url_shorten_urls_path_1)
{
	using namespace wordring::whatwg;
	url_record_base url;
	url.m_path.push_back(U"A:");
	url.m_path.push_back(U"path");
	shorten_urls_path(url);
	BOOST_CHECK(url.m_path.size() == 1);
}

BOOST_AUTO_TEST_CASE(url_basic_url_parser_construct_1)
{
	using namespace wordring::whatwg;
	std::error_code ec;
	basic_url_parser p(U"    ", ec);
	p.run();
	BOOST_CHECK(ec.value() == static_cast<int>(url_error_name::ValidationError));
	BOOST_CHECK(p.m_input.empty());
}

BOOST_AUTO_TEST_CASE(url_basic_url_parser_construct_2)
{
	using namespace wordring::whatwg;
	std::error_code ec;
	basic_url_parser p(U"  abc  ", ec);
	BOOST_CHECK(ec.value() == static_cast<int>(url_error_name::ValidationError));
	BOOST_CHECK(p.m_input == U"abc");
}

BOOST_AUTO_TEST_CASE(url_basic_url_parser_construct_3)
{
	using namespace wordring::whatwg;
	std::error_code ec;
	basic_url_parser p(U"  a\tb\nc  ", ec);
	BOOST_CHECK(ec.value() == static_cast<int>(url_error_name::ValidationError));
	BOOST_CHECK(p.m_input == U"abc");
}

BOOST_AUTO_TEST_CASE(url_basic_url_parser_construct_4)
{
	using namespace wordring::whatwg;
	std::error_code ec;
	basic_url_parser p(U"", ec, nullptr, encoding_name::UTF_16BE);
	BOOST_CHECK(p.m_encoding_name == encoding_name::UTF_8);
}

BOOST_AUTO_TEST_CASE(url_basic_url_parser_run_1)
{
	using namespace wordring::whatwg;
	std::error_code ec;
	basic_url_parser p(U"ftp://ftp.is.co.za/rfc/rfc1808.txt", ec);
	std::optional<url_record_base> u = p.run();

	BOOST_CHECK(u->m_scheme == U"ftp");
	BOOST_CHECK(u->m_host->type() == host_type_name::Domain);
	BOOST_CHECK(*u->m_host == std::u32string(U"ftp.is.co.za"));
	BOOST_CHECK(u->m_path.size() == 2);
}

BOOST_AUTO_TEST_CASE(url_basic_url_parser_run_2)
{
	using namespace wordring::whatwg;
	std::error_code ec;
	basic_url_parser p(U"http://www.ietf.org/rfc/rfc2396.txt", ec);
	std::optional<url_record_base> u = p.run();

	BOOST_CHECK(u->m_scheme == U"http");
	BOOST_CHECK(u->m_host->type() == host_type_name::Domain);
	BOOST_CHECK(*u->m_host == std::u32string(U"www.ietf.org"));
	BOOST_CHECK(u->m_path.size() == 2);
}

BOOST_AUTO_TEST_CASE(url_basic_url_parser_run_3)
{
	using namespace wordring::whatwg;
	std::error_code ec;
	basic_url_parser p(U"ldap://[2001:db8::7]/c=GB?objectClass?one", ec);
	std::optional<url_record_base> u = p.run();

	BOOST_CHECK(u->m_scheme == U"ldap");
	BOOST_CHECK(u->m_host->type() == host_type_name::Ipv6Address);
	BOOST_CHECK(u->m_path.size() == 1);
	BOOST_CHECK(*u->m_query == U"objectClass?one");
}

BOOST_AUTO_TEST_CASE(url_basic_url_parser_run_4)
{
	using namespace wordring::whatwg;
	std::error_code ec;
	basic_url_parser p(U"news:comp.infosystems.www.servers.unix", ec);
	std::optional<url_record_base> u = p.run();

	BOOST_CHECK(u->m_scheme == U"news");
	BOOST_CHECK(!u->m_host);
	BOOST_CHECK(u->m_path.size() == 1);
	BOOST_CHECK(u->m_path.front() == U"comp.infosystems.www.servers.unix");
}

BOOST_AUTO_TEST_CASE(url_basic_url_parser_run_5)
{
	using namespace wordring::whatwg;
	std::error_code ec;
	basic_url_parser p(U"tel:+1-816-555-1212", ec);
	std::optional<url_record_base> u = p.run();

	BOOST_CHECK(u->m_scheme == U"tel");
	BOOST_CHECK(!u->m_host);
	BOOST_CHECK(u->m_path.size() == 1);
	BOOST_CHECK(u->m_path.front() == U"+1-816-555-1212");
}

BOOST_AUTO_TEST_CASE(url_basic_url_parser_run_6)
{
	using namespace wordring::whatwg;
	std::error_code ec;
	basic_url_parser p(U"telnet://192.0.2.16:80/", ec);
	std::optional<url_record_base> u = p.run();

	BOOST_CHECK(u->m_scheme == U"telnet");
	BOOST_CHECK(u->m_host->type() == host_type_name::OpaqueHost); // スキームが特別ではないので、IPv4と解釈されない。
	BOOST_CHECK(*u->m_port == 80u);
	BOOST_CHECK(u->m_path.size() == 1);
	BOOST_CHECK(u->m_path.front() == U"");
}

BOOST_AUTO_TEST_CASE(url_basic_url_parser_run_7)
{
	using namespace wordring::whatwg;
	std::error_code ec;
	basic_url_parser p(U"urn:oasis:names:specification:docbook:dtd:xml:4.1.2", ec);
	std::optional<url_record_base> u = p.run();

	BOOST_CHECK(u->m_scheme == U"urn");
	BOOST_CHECK(!u->m_host);
	BOOST_CHECK(u->m_path.size() == 1);
	BOOST_CHECK(u->m_path.front() == U"oasis:names:specification:docbook:dtd:xml:4.1.2");
}

BOOST_AUTO_TEST_CASE(url_basic_url_parser_run_8)
{
	using namespace wordring::whatwg;
	std::error_code ec;
	basic_url_parser p(U"http://www.google.com", ec);
	std::optional<url_record_base> u = p.run();

	BOOST_CHECK(u->m_scheme == U"http");
	BOOST_CHECK(u->m_host->type() == host_type_name::Domain);
	BOOST_CHECK(*u->m_host == std::u32string(U"www.google.com"));
	BOOST_CHECK(u->m_path.size() == 1);
}

BOOST_AUTO_TEST_CASE(url_basic_url_parser_run_9)
{
	using namespace wordring::whatwg;
	std::error_code ec;
	basic_url_parser p(U"http://foo:bar@w1.superman.com/very/long/path.html?p1=v1&p2=v2#more-details", ec);
	std::optional<url_record_base> u = p.run();

	BOOST_CHECK(u->m_scheme == U"http");
	BOOST_CHECK(u->m_username == U"foo");
	BOOST_CHECK(u->m_password == U"bar");
	BOOST_CHECK(u->m_host->type() == host_type_name::Domain);
	BOOST_CHECK(*u->m_host == std::u32string(U"w1.superman.com"));
	BOOST_CHECK(u->m_path.size() == 3);
	BOOST_CHECK(*u->m_query == U"p1=v1&p2=v2");
	BOOST_CHECK(*u->m_fragment == U"more-details");
}

BOOST_AUTO_TEST_CASE(url_basic_url_parser_run_10)
{
	using namespace wordring::whatwg;
	std::error_code ec;
	basic_url_parser p(U"https://secured.com:443", ec);
	std::optional<url_record_base> u = p.run();

	BOOST_CHECK(u->m_scheme == U"https");
	BOOST_CHECK(u->m_host->type() == host_type_name::Domain);
	BOOST_CHECK(*u->m_host == std::u32string(U"secured.com"));
	BOOST_CHECK(!u->m_port); // 規定ポートは、ポート番号を消される。
	BOOST_CHECK(u->m_path.size() == 1); // ポート番号があると空のパスを追加するらしい。
}

BOOST_AUTO_TEST_CASE(url_basic_url_parser_run_11)
{
	using namespace wordring::whatwg;
	std::error_code ec;
	basic_url_parser p(U"ftp://ftp.bogus.com/~some/path/to/a/file.txt", ec);
	std::optional<url_record_base> u = p.run();

	BOOST_CHECK(u->m_scheme == U"ftp");
	BOOST_CHECK(u->m_host->type() == host_type_name::Domain);
	BOOST_CHECK(*u->m_host == std::u32string(U"ftp.bogus.com"));
	BOOST_CHECK(!u->m_port);
	BOOST_CHECK(u->m_path.size() == 5);
}

BOOST_AUTO_TEST_CASE(url_basic_url_parser_run_12)
{
	using namespace wordring::whatwg;
	std::error_code ec;
	basic_url_parser p(U"https://example.com/", ec);
	std::optional<url_record_base> u = p.run();

	BOOST_CHECK(u->m_scheme == U"https");
	BOOST_CHECK(u->m_host->type() == host_type_name::Domain);
	BOOST_CHECK(*u->m_host == std::u32string(U"example.com"));
	BOOST_CHECK(!u->m_port);
	BOOST_CHECK(u->m_path.size() == 1);
	BOOST_CHECK(u->m_path.front().empty());
	BOOST_CHECK(!u->m_query);
	BOOST_CHECK(!u->m_fragment);
	BOOST_CHECK(u->m_cannot_be_base_url == false);
}

BOOST_AUTO_TEST_CASE(url_basic_url_parser_run_13)
{
	using namespace wordring::whatwg;
	std::error_code ec;
	basic_url_parser p(U"https://localhost:8000/search?q=text#hello", ec);
	std::optional<url_record_base> u = p.run();

	BOOST_CHECK(u->m_scheme == U"https");
	BOOST_CHECK(u->m_host->type() == host_type_name::Domain);
	BOOST_CHECK(*u->m_host == std::u32string(U"localhost"));
	BOOST_CHECK(*u->m_port == 8000u);
	BOOST_CHECK(u->m_path.size() == 1);
	BOOST_CHECK(u->m_path.front() == U"search");
	BOOST_CHECK(*u->m_query == U"q=text");
	BOOST_CHECK(*u->m_fragment == U"hello");
	BOOST_CHECK(u->m_cannot_be_base_url == false);
}

BOOST_AUTO_TEST_CASE(url_basic_url_parser_run_14)
{
	using namespace wordring::whatwg;
	std::error_code ec;
	basic_url_parser p(U"urn:isbn:9780307476463", ec);
	std::optional<url_record_base> u = p.run();

	BOOST_CHECK(u->m_scheme == U"urn");
	BOOST_CHECK(!u->m_host);
	BOOST_CHECK(!u->m_port);
	BOOST_CHECK(u->m_path.size() == 1);
	BOOST_CHECK(u->m_path.front() == U"isbn:9780307476463");
	BOOST_CHECK(!u->m_query);
	BOOST_CHECK(!u->m_fragment);
	BOOST_CHECK(u->m_cannot_be_base_url == true);
}

BOOST_AUTO_TEST_CASE(url_basic_url_parser_run_15)
{
	using namespace wordring::whatwg;
	std::error_code ec;
	basic_url_parser p(U"file:///ada/Analytical%20Engine/README.md", ec);
	std::optional<url_record_base> u = p.run();

	BOOST_CHECK(u->m_scheme == U"file");
	// 規格のアルゴリズム通りに進行するとホストは空文字列であり、nullにならない？
	// BOOST_CHECK(!u->m_host);
	BOOST_CHECK(!u->m_port);
	BOOST_CHECK(u->m_path.size() == 3);
	BOOST_CHECK(u->m_path[0] == U"ada");
	BOOST_CHECK(u->m_path[1] == U"Analytical%20Engine");
	BOOST_CHECK(u->m_path[2] == U"README.md");
	BOOST_CHECK(!u->m_query);
	BOOST_CHECK(!u->m_fragment);
	BOOST_CHECK(u->m_cannot_be_base_url == false);
}

BOOST_AUTO_TEST_CASE(url_serialize_url_1)
{
	using namespace wordring::whatwg;
	std::error_code ec;
	basic_url_parser p(U"https:example.org", ec);
	std::optional<url_record_base> u = p.run();
	BOOST_CHECK(ec);
	std::u32string s = serialize_url(*u);
	BOOST_CHECK(s == U"https://example.org/");
}

BOOST_AUTO_TEST_CASE(url_serialize_url_2)
{
	using namespace wordring::whatwg;
	std::error_code ec;
	basic_url_parser p(U"https://////example.com///", ec);
	std::optional<url_record_base> u = p.run();
	BOOST_CHECK(ec);
	std::u32string s = serialize_url(*u);
	BOOST_CHECK(s == U"https://example.com///");
}

BOOST_AUTO_TEST_CASE(url_serialize_url_3)
{
	using namespace wordring::whatwg;
	std::error_code ec;
	basic_url_parser p(U"https://example.com/././foo", ec);
	std::optional<url_record_base> u = p.run();
	BOOST_CHECK(!ec);
	std::u32string s = serialize_url(*u);
	BOOST_CHECK(s == U"https://example.com/foo");
}

BOOST_AUTO_TEST_CASE(url_serialize_url_4)
{
	using namespace wordring::whatwg;

	std::error_code ec1;
	basic_url_parser p1(U"https://example.com/", ec1);
	std::optional<url_record_base> base = p1.run();

	std::error_code ec2;
	basic_url_parser p2(U"hello:world", ec2, &*base);
	std::optional<url_record_base> u = p2.run();
	BOOST_CHECK(!ec2);
	std::u32string s = serialize_url(*u);
	BOOST_CHECK(s == U"hello:world");
}

BOOST_AUTO_TEST_CASE(url_serialize_url_5)
{
	using namespace wordring::whatwg;

	std::error_code ec1;
	basic_url_parser p1(U"https://example.com/", ec1);
	std::optional<url_record_base> base = p1.run();

	std::error_code ec2;
	basic_url_parser p2(U"https:example.org", ec2, &*base);
	std::optional<url_record_base> u = p2.run();
	BOOST_CHECK(ec2);
	std::u32string s = serialize_url(*u);
	BOOST_CHECK(s == U"https://example.com/example.org");
}

BOOST_AUTO_TEST_CASE(url_serialize_url_6)
{
	using namespace wordring::whatwg;

	std::error_code ec1;
	basic_url_parser p1(U"https://example.com/", ec1);
	std::optional<url_record_base> base = p1.run();

	std::error_code ec2;
	basic_url_parser p2(U"\\example\\..\\demo/.\\", ec2, &*base);
	std::optional<url_record_base> u = p2.run();
	BOOST_CHECK(ec2);
	std::u32string s = serialize_url(*u);
	BOOST_CHECK(s == U"https://example.com/demo/");
}

BOOST_AUTO_TEST_CASE(url_serialize_url_7)
{
	using namespace wordring::whatwg;

	std::error_code ec1;
	basic_url_parser p1(U"https://example.com/demo", ec1);
	std::optional<url_record_base> base = p1.run();

	std::error_code ec2;
	basic_url_parser p2(U"example", ec2, &*base);
	std::optional<url_record_base> u = p2.run();
	BOOST_CHECK(!ec2);
	std::u32string s = serialize_url(*u);
	BOOST_CHECK(s == U"https://example.com/example");
}

BOOST_AUTO_TEST_CASE(url_serialize_url_8)
{
	using namespace wordring::whatwg;
	std::error_code ec;
	basic_url_parser p(U"file:///C|/demo", ec);
	std::optional<url_record_base> u = p.run();
	BOOST_CHECK(ec);
	std::u32string s = serialize_url(*u);
	BOOST_CHECK(s == U"file:///C:/demo");
}

BOOST_AUTO_TEST_CASE(url_serialize_url_9)
{
	using namespace wordring::whatwg;

	std::error_code ec1;
	basic_url_parser p1(U"file:///C:/demo", ec1);
	std::optional<url_record_base> base = p1.run();

	std::error_code ec2;
	basic_url_parser p2(U"..", ec2, &*base);
	std::optional<url_record_base> u = p2.run();
	BOOST_CHECK(!ec2);
	std::u32string s = serialize_url(*u);
	BOOST_CHECK(s == U"file:///C:/");
}

BOOST_AUTO_TEST_CASE(url_serialize_url_10)
{
	using namespace wordring::whatwg;
	std::error_code ec;
	basic_url_parser p(U"file://loc%61lhost/", ec);
	std::optional<url_record_base> u = p.run();
	BOOST_CHECK(!ec);
	std::u32string s = serialize_url(*u);
	BOOST_CHECK(s == U"file:///");
}

BOOST_AUTO_TEST_CASE(url_serialize_url_11)
{
	using namespace wordring::whatwg;
	std::error_code ec;
	basic_url_parser p(U"https://user:password@example.org/", ec);
	std::optional<url_record_base> u = p.run();
	BOOST_CHECK(ec);
	std::u32string s = serialize_url(*u);
	BOOST_CHECK(s == U"https://user:password@example.org/");
}

BOOST_AUTO_TEST_CASE(url_serialize_url_12)
{
	using namespace wordring::whatwg;
	std::error_code ec;
	basic_url_parser p(U"https://example.org/foo bar", ec);
	std::optional<url_record_base> u = p.run();
	BOOST_CHECK(ec);
	std::u32string s = serialize_url(*u);
	BOOST_CHECK(s == U"https://example.org/foo%20bar");
}

BOOST_AUTO_TEST_CASE(url_serialize_url_13)
{
	using namespace wordring::whatwg;
	std::error_code ec;
	basic_url_parser p(U"https://EXAMPLE.com/../x", ec);
	std::optional<url_record_base> u = p.run();
	BOOST_CHECK(!ec);
	std::u32string s = serialize_url(*u);
	BOOST_CHECK(s == U"https://example.com/x");
}

BOOST_AUTO_TEST_CASE(url_serialize_url_14)
{
	using namespace wordring::whatwg;
	std::error_code ec;
	basic_url_parser p(U"https://ex ample.org/", ec);
	std::optional<url_record_base> u = p.run();
	BOOST_CHECK(ec);
	BOOST_CHECK(!u);
}

BOOST_AUTO_TEST_CASE(url_serialize_url_15)
{
	using namespace wordring::whatwg;
	std::error_code ec;
	basic_url_parser p(U"example", ec);
	std::optional<url_record_base> u = p.run();
	BOOST_CHECK(ec);
	BOOST_CHECK(!u);
}

BOOST_AUTO_TEST_CASE(url_serialize_url_16)
{
	using namespace wordring::whatwg;
	std::error_code ec;
	basic_url_parser p(U"https://example.com:demo", ec);
	std::optional<url_record_base> u = p.run();
	BOOST_CHECK(ec);
	BOOST_CHECK(!u);
}

BOOST_AUTO_TEST_CASE(url_serialize_url_17)
{
	using namespace wordring::whatwg;
	std::error_code ec;
	basic_url_parser p(U"http://[www.example.com]/", ec);
	std::optional<url_record_base> u = p.run();
	BOOST_CHECK(ec);
	BOOST_CHECK(!u);
}

BOOST_AUTO_TEST_CASE(url_serialize_url_18)
{
	using namespace wordring::whatwg;
	std::error_code ec;
	basic_url_parser p(U"https://example.org//", ec);
	std::optional<url_record_base> u = p.run();
	BOOST_CHECK(!ec);
	std::u32string s = serialize_url(*u);
	BOOST_CHECK(s == U"https://example.org//");
}


BOOST_AUTO_TEST_SUITE_END()
