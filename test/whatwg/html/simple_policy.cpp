// test/whatwg/html/simple_policy.cpp

#include <boost/test/unit_test.hpp>

#include <wordring/tree/tree.hpp>

#include <wordring/whatwg/html/simple_node.hpp>
#include <wordring/whatwg/html/simple_policy.hpp>

namespace
{
	using node_type = wordring::whatwg::html::simple_node<std::u32string>;

	using tree = wordring::tree<node_type>;

	using policy = wordring::whatwg::html::simple_policy<std::u32string, tree>;

	struct test_policy : policy
	{

	};
}

BOOST_AUTO_TEST_SUITE(whatwg_html__simple_policy__test)

BOOST_AUTO_TEST_CASE(simple_policy__construct__1)
{
	//using namespace wordring::whatwg::html::parsing;
	test_policy tp;
	BOOST_CHECK(true);
}

/*
BOOST_AUTO_TEST_CASE(simple_policy__in_html_namespace__1)
{
	auto elm = policy::element_node_type();
	elm.namespace_uri(U"http://www.w3.org/1999/xhtml");
	auto t = tree();
	t.insert(t.end(), elm);

	BOOST_CHECK(policy::in_html_namespace(t.begin()));
}

BOOST_AUTO_TEST_CASE(simple_policy__in_html_namespace__2)
{
	auto elm = policy::element_node_type();
	elm.namespace_uri(U"http://www.w3.org/2000/svg");
	auto t = tree();
	t.insert(t.end(), elm);

	BOOST_CHECK(!policy::in_html_namespace(t.begin()));
}
*/

BOOST_AUTO_TEST_SUITE_END()
