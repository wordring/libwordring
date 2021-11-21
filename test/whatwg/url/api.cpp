#include <boost/test/unit_test.hpp>

#include <wordring/whatwg/url/api.hpp>

#include <wordring/compatibility.hpp>

#include <iterator>
#include <string>

BOOST_AUTO_TEST_SUITE(whatwg_url_api_test)

BOOST_AUTO_TEST_CASE(api_basic_url_construct_1)
{
	wordring::whatwg::basic_url<std::u8string> url;
	BOOST_CHECK(url.host() == u8"");
}

BOOST_AUTO_TEST_CASE(api_basic_url_construct_2)
{
	wordring::whatwg::basic_url<std::u8string> url(u8"../", u8"http://wordring.net/path1");
	BOOST_CHECK(url.pathname() == u8"/");
}

BOOST_AUTO_TEST_CASE(api_basic_url_copy_1)
{
	wordring::whatwg::basic_url<std::u8string> u1(u8"../", u8"http://wordring.net/path1");

	auto u2 = u1;
	BOOST_CHECK(to_string(u2) == u8"http://wordring.net/"); // ADL
}

BOOST_AUTO_TEST_CASE(api_basic_url_move_1)
{
	wordring::whatwg::basic_url<std::u8string> u1(u8"../", u8"http://wordring.net/path1");

	auto u2 = std::move(u1);
	BOOST_CHECK(to_string(u2) == u8"http://wordring.net/"); // ADL
}

BOOST_AUTO_TEST_CASE(api_basic_url_href_1)
{
	std::u8string s = u8"http://wordring.net/";

	wordring::whatwg::basic_url<std::u8string> url;
	url.href(s);

	BOOST_CHECK(url.href() == s);
}

BOOST_AUTO_TEST_CASE(api_basic_url_protocol_1)
{
	std::u8string s = u8"http://wordring.net/";

	wordring::whatwg::basic_url<std::u8string> url;
	url.href(s);

	url.protocol(u8"ftp");
	BOOST_CHECK(url.protocol() == u8"ftp:");
}

BOOST_AUTO_TEST_CASE(api_basic_url_username_1)
{
	std::u8string s = u8"http://wordring.net/";

	wordring::whatwg::basic_url<std::u8string> url;
	url.href(s);

	url.username(u8"user");
	BOOST_CHECK(url.username() == u8"user");
}

BOOST_AUTO_TEST_CASE(api_basic_url_password_1)
{
	std::u8string s = u8"http://wordring.net/";

	wordring::whatwg::basic_url<std::u8string> url;
	url.href(s);

	url.password(u8"password");
	BOOST_CHECK(url.password() == u8"password");
}

BOOST_AUTO_TEST_CASE(api_basic_url_host_1)
{
	std::u8string s = u8"http://wordring.net/";

	wordring::whatwg::basic_url<std::u8string> url;
	url.href(s);

	url.host(u8"www.example.net:8080");
	BOOST_CHECK(url.host() == u8"www.example.net:8080");
}

BOOST_AUTO_TEST_CASE(api_basic_url_hostname_1)
{
	std::u8string s = u8"http://wordring.net/";

	wordring::whatwg::basic_url<std::u8string> url;
	url.href(s);

	url.hostname(u8"www.example.net");
	BOOST_CHECK(url.hostname() == u8"www.example.net");
}

BOOST_AUTO_TEST_CASE(api_basic_url_port_1)
{
	std::u8string s = u8"http://wordring.net/";

	wordring::whatwg::basic_url<std::u8string> url;
	url.href(s);

	url.port(u8"8080");
	BOOST_CHECK(url.port() == u8"8080");
}

BOOST_AUTO_TEST_CASE(api_basic_url_pathname_1)
{
	std::u8string s = u8"http://wordring.net/";

	wordring::whatwg::basic_url<std::u8string> url;
	url.href(s);

	url.pathname(u8"path1/path2");
	BOOST_CHECK(url.pathname() == u8"/path1/path2");
}

BOOST_AUTO_TEST_CASE(api_basic_url_search_1)
{
	std::u8string s = u8"http://wordring.net/";

	wordring::whatwg::basic_url<std::u8string> url;
	url.href(s);

	url.search(u8"?q=a,");
	BOOST_CHECK(url.search() == u8"?q=a,");
}

BOOST_AUTO_TEST_CASE(api_basic_url_hash_1)
{
	std::u8string s = u8"http://wordring.net/";

	wordring::whatwg::basic_url<std::u8string> url;
	url.href(s);

	url.hash(u8"#hash");
	BOOST_CHECK(url.hash() == u8"#hash");
}

BOOST_AUTO_TEST_CASE(api_basic_url_to_string_1)
{
	std::u8string s = u8"http://wordring.net/";

	wordring::whatwg::basic_url<std::u8string> url;
	url.href(s);
	url.hash(u8"#hash");

	BOOST_CHECK(wordring::whatwg::to_string(url) == u8"http://wordring.net/#hash");
}

BOOST_AUTO_TEST_SUITE_END()
