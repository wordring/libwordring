// test/whatwg/html/simple_node.cpp

#include <boost/test/unit_test.hpp>

#include <wordring/whatwg/html/simple_node.hpp>

BOOST_AUTO_TEST_SUITE(whatwg_html__simple_node__test)

BOOST_AUTO_TEST_CASE(simple_node__construct__1)
{
	using namespace wordring::whatwg::html::simple;

	BOOST_CHECK(true);

	basic_element<std::string> be;
}

BOOST_AUTO_TEST_SUITE_END()
