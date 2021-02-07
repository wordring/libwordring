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
	BOOST_CHECK(starts_with_windows_drive_letter(s.begin(), s.end()));
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

BOOST_AUTO_TEST_CASE(url_basic_url_parser_on_scheme_start_state_4)
{
	using namespace wordring::whatwg;
	std::error_code ec;
	basic_url_parser p(U"A", ec, nullptr, encoding_name::UTF_16BE);
	auto url = p.run();
	BOOST_CHECK(p.buffer == U"a");
}



BOOST_AUTO_TEST_SUITE_END()
