// test/trie/trie_construct_iterator.cpp

#include <boost/test/unit_test.hpp>

#include <wordring/tree/tree_iterator.hpp>
#include <wordring/trie/list_trie_iterator.hpp>
#include <wordring/trie/trie_construct_iterator.hpp>

#include <algorithm>
#include <iterator>
#include <string>
#include <vector>


BOOST_AUTO_TEST_SUITE(trie_construct_iterator__test)

// trie_construct_iterator(iterator_type root)
BOOST_AUTO_TEST_CASE(trie_construct_iterator__construct__1)
{
	using namespace wordring;

	using list_iterator = const_list_trie_iterator<std::vector<std::string>::const_iterator>;
	using construct_iterator = trie_construct_iterator<list_iterator>;
	
	std::vector<std::string> words{ "a", "ac", "b", "cab", "cd" };
	auto li = list_iterator(words.begin(), words.end());
	auto it = construct_iterator(li);

	BOOST_CHECK(it.children()[0] == 'a');
	BOOST_CHECK(it.children()[1] == 'b');
	BOOST_CHECK(it.children()[2] == 'c');
}

BOOST_AUTO_TEST_CASE(trie_construct_iterator__base__1)
{
}

// result_range<typename string_type::const_iterator> string() const
BOOST_AUTO_TEST_CASE(trie_construct_iterator__string__1)
{
}

BOOST_AUTO_TEST_CASE(trie_construct_iterator__string__2)
{
}

BOOST_AUTO_TEST_CASE(trie_construct_iterator__string__3)
{
}

BOOST_AUTO_TEST_CASE(trie_construct_iterator__string__4)
{
}

// value_type operator*() const
BOOST_AUTO_TEST_CASE(trie_construct_iterator__reference__1)
{
}

BOOST_AUTO_TEST_CASE(trie_construct_iterator__reference__2)
{
}

// trie_construct_iterator& operator++()
BOOST_AUTO_TEST_CASE(trie_construct_iterator__increment__1)
{
}

BOOST_AUTO_TEST_CASE(trie_construct_iterator__increment__2)
{
}

BOOST_AUTO_TEST_CASE(trie_construct_iterator__increment__3)
{
}

BOOST_AUTO_TEST_CASE(trie_construct_iterator__increment__4)
{
}

BOOST_AUTO_TEST_CASE(trie_construct_iterator__increment__5)
{
}

BOOST_AUTO_TEST_CASE(trie_construct_iterator__increment__6)
{
}

BOOST_AUTO_TEST_CASE(trie_construct_iterator__increment__7)
{
}

// trie_construct_iterator operator++(int)
BOOST_AUTO_TEST_CASE(trie_construct_iterator__increment__8)
{
}

// trie_construct_iterator begin() const
BOOST_AUTO_TEST_CASE(trie_construct_iterator__begin__1)
{
}

BOOST_AUTO_TEST_CASE(trie_construct_iterator__begin__2)
{
}

BOOST_AUTO_TEST_CASE(trie_construct_iterator__parent__1)
{
	using namespace wordring;

	using list_iterator = const_list_trie_iterator<std::vector<std::string>::const_iterator>;
	using construct_iterator = trie_construct_iterator<list_iterator>;

	std::vector<std::string> words{ "", "a", "ac", "b", "cab", "cd" };
	auto li = list_iterator(words.begin(), words.end());
	auto it = construct_iterator(li);

	auto view = it.parent();
	auto s = std::string(view.begin(), view.end());
	BOOST_CHECK(s == "");

	++it;
	view = it.parent();
	s = std::string(view.begin(), view.end());
	BOOST_CHECK(s == "a");

	++it;
	view = it.parent();
	s = std::string(view.begin(), view.end());
	BOOST_CHECK(s == "c");

	++it;
	view = it.parent();
	s = std::string(view.begin(), view.end());
	BOOST_CHECK(s == "ca");
}

// void children(lavel_vector& labels)
BOOST_AUTO_TEST_CASE(trie_construct_iterator__children__1)
{
	using namespace wordring;

	using list_iterator = const_list_trie_iterator<std::vector<std::string>::const_iterator>;
	using construct_iterator = trie_construct_iterator<list_iterator>;

	std::vector<std::string> words{ "", "a", "ac", "b", "cab", "cd" };
	auto li = list_iterator(words.begin(), words.end());
	auto it = construct_iterator(li);

	BOOST_CHECK(it.children().size() == 3);
	BOOST_CHECK(it.children()[0] == 'a');
	BOOST_CHECK(it.children()[1] == 'b');
	BOOST_CHECK(it.children()[2] == 'c');

	++it;
	BOOST_CHECK(it.children().size() == 2);
	BOOST_CHECK(it.children()[0] == 'c');
	BOOST_CHECK(it.children()[1] == 256u);

	++it;
	BOOST_CHECK(it.children().size() == 2);
	BOOST_CHECK(it.children()[0] == 'a');
	BOOST_CHECK(it.children()[1] == 'd');

	++it;
	BOOST_CHECK(it.children().size() == 1);
	BOOST_CHECK(it.children()[0] == 'b');

	++it;
	BOOST_CHECK(it.children().size() == 0);
	BOOST_CHECK(it.empty());
}

BOOST_AUTO_TEST_CASE(trie_construct_iterator__children__2)
{
	using namespace wordring;

	using list_iterator = const_list_trie_iterator<std::vector<std::string>::const_iterator>;
	using construct_iterator = trie_construct_iterator<list_iterator>;

	std::vector<std::string> words{ "", "a", "ac", "b", "cab", "cd" };
	auto li = list_iterator(words.begin(), words.end());
	auto it = construct_iterator(li);

	BOOST_CHECK(it.children().size() == 3);
	BOOST_CHECK(it.children()[0] == 'a');
	BOOST_CHECK(it.children()[1] == 'b');
	BOOST_CHECK(it.children()[2] == 'c');

	++it;
	BOOST_CHECK(it.children().size() == 2);
	BOOST_CHECK(it.children()[0] == 'c');
	BOOST_CHECK(it.children()[1] == 256u);

	++it;
	BOOST_CHECK(it.children().size() == 2);
	BOOST_CHECK(it.children()[0] == 'a');
	BOOST_CHECK(it.children()[1] == 'd');

	++it;
	BOOST_CHECK(it.children().size() == 1);
	BOOST_CHECK(it.children()[0] == 'b');

	++it;
	BOOST_CHECK(it.children().size() == 0);
	BOOST_CHECK(it.empty());
}

BOOST_AUTO_TEST_SUITE_END()
