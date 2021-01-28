// test/tag_tree/tree_iterator.cpp

#include <boost/test/unit_test.hpp>

#include <wordring/tag_tree/tag_tree.hpp>
#include <wordring/tree/tree_iterator.hpp>

#include <wordring/html/simple_html.hpp>
#include <wordring/html/simple_node.hpp>

#include <iterator>
#include <string>
#include <string_view>
#include <vector>

BOOST_AUTO_TEST_SUITE(tag_tree_tree_iterator_test)

BOOST_AUTO_TEST_CASE(tree_iterator_1)
{
	using namespace wordring;
	using namespace wordring::html;

	u8simple_tree t1;
	t1.insert(t1.end(), simple_element<std::u8string>{});

	tree_iterator<u8simple_tree::iterator> it1(t1.begin()), it2;

	int i = 0;
	while (it1 != it2)
	{
		++i;
		++it1;
	}

	BOOST_CHECK(i == 1);
}

BOOST_AUTO_TEST_CASE(tree_iterator_2)
{
	using namespace wordring;
	using namespace wordring::html;

	u8simple_tree t1;
	auto it1 = t1.insert(t1.end(), simple_element<std::u8string>{});
	auto it2 = t1.insert(it1.end(), simple_element<std::u8string>{});
	t1.insert(it2.end(), simple_element<std::u8string>{});

	tree_iterator<u8simple_tree::iterator> first(t1.begin()), last;

	int i = 0;
	while (first != last)
	{
		++i;
		++first;
	}

	BOOST_CHECK(i == 3);
}

BOOST_AUTO_TEST_CASE(tree_iterator_3)
{
	using namespace wordring;
	using namespace wordring::html;

	u8simple_tree t1;
	auto it1 = t1.insert(t1.end(), simple_element<std::u8string>{});
	t1.insert(it1.end(), simple_element<std::u8string>{});
	t1.insert(it1.end(), simple_element<std::u8string>{});

	tree_iterator<u8simple_tree::iterator> first(t1.begin()), last;

	int i = 0;
	while (first != last)
	{
		++i;
		++first;
	}

	BOOST_CHECK(i == 3);
}

BOOST_AUTO_TEST_CASE(tree_iterator_4)
{
	using namespace wordring;
	using namespace wordring::html;

	u8simple_tree t1;
	auto it1 = t1.insert(t1.end(), simple_element<std::u8string>{});
	t1.insert(it1.end(), simple_text<std::u8string>{});

	tree_iterator<u8simple_tree::iterator> first(t1.begin()), last;

	int i = 0;
	while (first != last)
	{
		++i;
		++first;
	}

	BOOST_CHECK(i == 2);
}

BOOST_AUTO_TEST_CASE(tree_iterator_5)
{
	using namespace wordring;
	using namespace wordring::html;

	u8simple_tree t1;
	t1.insert(t1.end(), simple_text<std::u8string>{});

	tree_iterator<u8simple_tree::iterator> first(t1.begin()), last;

	int i = 0;
	while (first != last)
	{
		++i;
		++first;
	}

	BOOST_CHECK(i == 1);
}

BOOST_AUTO_TEST_CASE(tree_iterator_6)
{
	using namespace wordring;
	using namespace wordring::html;

	u8simple_tree t1;
	t1.insert(t1.end(), simple_text<std::u8string>{});

	tree_iterator<u8simple_tree::iterator> first(t1.begin()), last;

	int i = 0;
	while (first != last)
	{
		++i;
		++first;
	}

	BOOST_CHECK(i == 1);
}

BOOST_AUTO_TEST_SUITE_END()
