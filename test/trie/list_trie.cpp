// test/trie/list_trie.cpp

#include <boost/test/unit_test.hpp>

#include <wordring/trie/list_trie.hpp>

#include <algorithm>
#include <iterator>
#include <string>
#include <vector>


BOOST_AUTO_TEST_SUITE(list_trie__test)

// list_trie_iterator(iterator_type first, iterator_type last)
BOOST_AUTO_TEST_CASE(list_trie__construct__1)
{
	using namespace wordring;

	std::vector<std::string> list{ "abc", "abd", "bbc", "bbd" };
	list_trie_iterator it{ list.begin(), list.end() };

	BOOST_CHECK(it.begin() != it.end());
}

BOOST_AUTO_TEST_CASE(list_trie__construct__2)
{
	using namespace wordring;

	std::vector<std::string> list{ };
	list_trie_iterator it{ list.begin(), list.end() };

	BOOST_CHECK(it.begin() == it.end());
}

// result_pair string() const
BOOST_AUTO_TEST_CASE(list_trie__string__1)
{
	using namespace wordring;

	std::vector<std::string> list{ "abc", "abd", "bbc", "bbd" };
	list_trie_iterator it{ list.begin(), list.end() };
	
	auto pair = (++it.begin().begin().begin()).string();
	std::string s{ pair.begin(), pair.end() };
	BOOST_CHECK(s == "abd");
}

// value_type operator*() const
BOOST_AUTO_TEST_CASE(list_trie__reference__1)
{
	using namespace wordring;

	std::vector<std::string> list{ "abc", "abd", "bbc", "bbd" };
	list_trie_iterator it1{ list.begin(), list.end() };

	list_trie_iterator it2 = it1.begin();
	BOOST_CHECK(*it2 == 'a');
	BOOST_CHECK(*++it2 == 'b');
	BOOST_CHECK(++it2 == it1.end());
}

// list_trie_iterator& operator++()
BOOST_AUTO_TEST_CASE(list_trie__increment__1)
{
	using namespace wordring;

	std::vector<std::string> list{ "abc", "abd", "bbc", "bbd" };
	list_trie_iterator it{ list.begin(), list.end() };

	auto it1 = it.begin();
	++it1;
	++it1;
	BOOST_CHECK(it1 == it.end());
}

BOOST_AUTO_TEST_CASE(list_trie__increment__2)
{
	using namespace wordring;

	std::vector<std::string> list{ "abc", "abd", "bbc", "bbd" };
	list_trie_iterator it{ list.begin(), list.end() };

	std::string s;
	auto it1 = it.begin();
	auto it2 = it.end();
	while (it1 != it2)
	{
		s.push_back(*it1);
		++it1;
	}

	BOOST_CHECK(s == "ab");
}

// list_trie_iterator operator++(int)
BOOST_AUTO_TEST_CASE(list_trie__increment__3)
{
	using namespace wordring;

	std::vector<std::string> list{ "abc", "abd", "bbc", "bbd" };
	list_trie_iterator it{ list.begin(), list.end() };

	auto it1 = it.begin();
	BOOST_CHECK(*it1++ == 'a');
	BOOST_CHECK(*it1++ == 'b');
	BOOST_CHECK(it1 == it.end());
}

BOOST_AUTO_TEST_SUITE_END()
