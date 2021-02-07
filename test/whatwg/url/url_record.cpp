// test/whatwg/url/url_record.cpp

#include <boost/test/unit_test.hpp>

#include <algorithm>
#include <string>
#include <string_view>

#include <wordring/compatibility.hpp>
#include <wordring/whatwg/url/url_record.hpp>

BOOST_AUTO_TEST_SUITE(whatwg__url_record__test)


BOOST_AUTO_TEST_CASE(url_record__construct__1)
{
	using namespace wordring::whatwg;

	//url_record ur;
}

BOOST_AUTO_TEST_CASE(is_forbidden_host_code_point__1)
{
	using namespace wordring::whatwg;

	BOOST_CHECK(is_forbidden_host_code_point(0));
	BOOST_CHECK(is_forbidden_host_code_point(0x9u));
	BOOST_CHECK(is_forbidden_host_code_point(0xAu));
	BOOST_CHECK(is_forbidden_host_code_point(0xDu));
	BOOST_CHECK(is_forbidden_host_code_point(0x20u));
	BOOST_CHECK(is_forbidden_host_code_point(0x23u));
	BOOST_CHECK(is_forbidden_host_code_point(0x25u));
	BOOST_CHECK(is_forbidden_host_code_point(0x2Fu));
	BOOST_CHECK(is_forbidden_host_code_point(0x3Au));
	BOOST_CHECK(is_forbidden_host_code_point(0x3Fu));
	BOOST_CHECK(is_forbidden_host_code_point(0x40u));
	BOOST_CHECK(is_forbidden_host_code_point(0x5Bu));
	BOOST_CHECK(is_forbidden_host_code_point(0x5Cu));
	BOOST_CHECK(is_forbidden_host_code_point(0x5Du));
}


BOOST_AUTO_TEST_SUITE_END()
