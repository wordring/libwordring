#include <boost/test/unit_test.hpp>

#include <algorithm>
#include <deque>
#include <string>
#include <string_view>

#include <wordring/compatibility.hpp>
#include <wordring/whatwg/url/url.hpp>

BOOST_AUTO_TEST_SUITE(whatwg_url_test)

BOOST_AUTO_TEST_CASE(whatwg_url__serialize)
{
	std::u8string result{};

	wordring::whatwg::serialize(12345, std::back_inserter(result));
	BOOST_CHECK(true);
}

/*
BOOST_AUTO_TEST_CASE(whatwg_url__)
{
	BOOST_CHECK(wordring::whatwg::() == true);
}
*/

BOOST_AUTO_TEST_SUITE_END()
