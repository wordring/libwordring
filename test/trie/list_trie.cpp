﻿// test/trie/list_trie.cpp

#include <boost/test/unit_test.hpp>

#include <wordring/trie/list_trie.hpp>

#include <algorithm>
#include <iterator>
#include <string>
#include <vector>


BOOST_AUTO_TEST_SUITE(list_trie__test)

// const_list_trie_iterator(iterator_type first, iterator_type last)
BOOST_AUTO_TEST_CASE(const_list_trie_iterator__construct__1)
{
	using namespace wordring;

	std::vector<std::string> list{ "abc", "abd", "bbc", "bbd" };
	const_list_trie_iterator it{ list.begin(), list.end() };

	BOOST_CHECK(it.begin() != it.end());
}

BOOST_AUTO_TEST_CASE(const_list_trie_iterator__construct__2)
{
	using namespace wordring;

	std::vector<std::string> list{ };
	const_list_trie_iterator it{ list.begin(), list.end() };

	BOOST_CHECK(it.begin() == it.end());
}

// result_pair string() const
BOOST_AUTO_TEST_CASE(const_list_trie_iterator__string__1)
{
	using namespace wordring;

	std::vector<std::string> list{ "abc", "abd", "bbc", "bbd" };
	const_list_trie_iterator it{ list.begin(), list.end() };
	
	auto pair = (++it.begin().begin().begin()).string();
	std::string s{ pair.begin(), pair.end() };
	BOOST_CHECK(s == "abd");
}

// value_type operator*() const
BOOST_AUTO_TEST_CASE(const_list_trie_iterator__reference__1)
{
	using namespace wordring;

	std::vector<std::string> list{ "a", "ac", "b", "cab", "cd" };
	const_list_trie_iterator it1{ list.begin(), list.end() };

	BOOST_CHECK(*it1.begin() == 'a');
	BOOST_CHECK(*++it1.begin() == 'b');
	BOOST_CHECK(*++++it1.begin() == 'c');
}

BOOST_AUTO_TEST_CASE(const_list_trie_iterator__reference__2)
{
	using namespace wordring;

	std::vector<std::string> list{ "a", "ac", "b", "cab", "cd" };
	const_list_trie_iterator it{ list.begin(), list.end() };

	const_list_trie_iterator it1 = it.begin().begin();
	const_list_trie_iterator it2 = it.begin().end();

	BOOST_CHECK(*it1 == decltype(it)::null_value);
	BOOST_CHECK(*++it1 == 'c');
	BOOST_CHECK(++it1 == it2);
}

// const_list_trie_iterator& operator++()
BOOST_AUTO_TEST_CASE(const_list_trie_iterator__increment__1)
{
	using namespace wordring;

	std::vector<std::string> list{ "a", "ac", "b", "cab", "cd" };
	const_list_trie_iterator it{ list.begin(), list.end() };

	auto it1 = it.begin();
	auto it2 = it.end();

	BOOST_CHECK(*it1 == 'a');
	BOOST_CHECK(*++it1 == 'b');
	BOOST_CHECK(*++it1 == 'c');
	BOOST_CHECK(++it1 == it2);
}

BOOST_AUTO_TEST_CASE(const_list_trie_iterator__increment__2)
{
	using namespace wordring;

	std::vector<std::string> list{ "a", "ac", "b", "cab", "cd" };
	const_list_trie_iterator it{ list.begin(), list.end() };

	auto it1 = it.begin().begin(); // a[], a[c]
	auto it2 = it.begin().end();

	BOOST_CHECK(*it1 == decltype(it)::null_value);
	BOOST_CHECK(*++it1 == 'c');
	BOOST_CHECK(++it1 == it2);
}

BOOST_AUTO_TEST_CASE(const_list_trie_iterator__increment__3)
{
	using namespace wordring;

	std::vector<std::string> list{ "a", "ac", "b", "cab", "cd" };
	const_list_trie_iterator it{ list.begin(), list.end() };

	auto it1 = (++it.begin()).begin();
	auto it2 = (++it.begin()).end();

	BOOST_CHECK(++it1 == it2);
}

BOOST_AUTO_TEST_CASE(const_list_trie_iterator__increment__4)
{
	using namespace wordring;

	std::vector<std::string> list{ "a", "ac", "b", "cab", "cd" };
	const_list_trie_iterator it{ list.begin(), list.end() };

	auto it1 = (++++it.begin()).begin();
	auto it2 = (++++it.begin()).end();

	BOOST_CHECK(*it1 == 'a');
	BOOST_CHECK(*++it1 == 'd');
	BOOST_CHECK(++it1 == it2);
}

BOOST_AUTO_TEST_CASE(const_list_trie_iterator__increment__5)
{
	using namespace wordring;

	std::vector<std::string> list{ "a", "ac", "b", "cab", "cd" };
	const_list_trie_iterator it{ list.begin(), list.end() };

	auto it1 = (++++it.begin()).begin();
	auto it2 = (++++it.begin()).end();

	BOOST_CHECK(*it1 == 'a');   // c[a]b
	BOOST_CHECK(*++it1 == 'd'); // c[d]
	BOOST_CHECK(++it1 == it2);
}

BOOST_AUTO_TEST_CASE(const_list_trie_iterator__increment__6)
{
	using namespace wordring;

	std::vector<std::string> list{ "a", "ac", "b", "cab", "cd" };
	const_list_trie_iterator it{ list.begin(), list.end() };

	auto it1 = (++++it.begin()).begin().begin();
	auto it2 = (++++it.begin()).begin().end();

	BOOST_CHECK(*it1 == 'b');   // ca[b]
	BOOST_CHECK(++it1 == it2);
}

BOOST_AUTO_TEST_CASE(const_list_trie_iterator__increment__7)
{
	using namespace wordring;

	std::vector<std::string> list{ "a", "ac", "b", "cab", "cd" };
	const_list_trie_iterator it{ list.begin(), list.end() };

	auto it1 = ++(++++it.begin()).begin();
	auto it2 = (++++it.begin()).end();

	BOOST_CHECK(*it1 == 'd'); // c[d]

	auto it3 = it1.begin();
	auto it4 = it1.end();

	BOOST_CHECK(it3 == it4); // cd[]
}

// const_list_trie_iterator operator++(int)
BOOST_AUTO_TEST_CASE(const_list_trie_iterator__increment__8)
{
	using namespace wordring;

	std::vector<std::string> list{ "abc", "abd", "bbc", "bbd" };
	const_list_trie_iterator it{ list.begin(), list.end() };

	auto it1 = it.begin();
	BOOST_CHECK(*it1++ == 'a');
	BOOST_CHECK(*it1++ == 'b');
	BOOST_CHECK(it1 == it.end());
}

// const_list_trie_iterator begin() const
BOOST_AUTO_TEST_CASE(const_list_trie_iterator__begin__1)
{
	using namespace wordring;

	std::vector<std::string> list{ "a", "ac", "b", "cab", "cd" };
	const_list_trie_iterator it{ list.begin(), list.end() };

	auto it1 = it.begin().begin();

	auto it3 = it1.begin();
	auto it4 = it1.end();

	BOOST_CHECK(it3 == it4);
}

BOOST_AUTO_TEST_SUITE_END()