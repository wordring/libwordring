// test/tree/tree.cpp

#include <boost/test/unit_test.hpp>

#include <array>

#include <wordring/tree/tree.hpp>

BOOST_AUTO_TEST_SUITE(tree__test)

BOOST_AUTO_TEST_CASE(tree__construct__1)
{
	using namespace wordring;

	tree_node<int> tn{};

	tn.begin();
}


BOOST_AUTO_TEST_SUITE_END()
