// test/whatwg/html/simple_traits.cpp

#include <boost/test/unit_test.hpp>

#include <wordring/tree/tree.hpp>

#include <wordring/html/simple_node.hpp>
#include <wordring/html/simple_traits.hpp>

BOOST_AUTO_TEST_SUITE(whatwg_html_simple_traits_test)

BOOST_AUTO_TEST_CASE(simple_adapter__construct__1)
{
}

/*
BOOST_AUTO_TEST_CASE(simple_adapter__in_html_namespace__1)
{
	auto elm = adapter::element_node_type();
	elm.namespace_uri(U"http://www.w3.org/1999/xhtml");
	auto t = tree();
	t.insert(t.end(), elm);

	BOOST_CHECK(adapter::in_html_namespace(t.begin()));
}

BOOST_AUTO_TEST_CASE(simple_adapter__in_html_namespace__2)
{
	auto elm = adapter::element_node_type();
	elm.namespace_uri(U"http://www.w3.org/2000/svg");
	auto t = tree();
	t.insert(t.end(), elm);

	BOOST_CHECK(!adapter::in_html_namespace(t.begin()));
}
*/

BOOST_AUTO_TEST_SUITE_END()
