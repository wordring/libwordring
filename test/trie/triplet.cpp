// test/trie/triplet.cpp

#include <algorithm>
#include <iterator>
#include <list>
#include <string>
#include <vector>

#include <iostream>

#include <boost/test/unit_test.hpp>

#include <wordring/trie/triplet.hpp>

BOOST_AUTO_TEST_SUITE(triplet_trie__test)

// フルチェック
BOOST_AUTO_TEST_CASE(triplet_trie__children__1)
{
	using namespace wordring;

	std::cout << "sizeof(wordring::triplet_trie::node): " << sizeof(triplet_trie< std::vector<std::string>>::node) << std::endl;
	std::vector<std::string> list{ "a", "ac", "b", "cab", "cd" };
	std::sort(list.begin(), list.end());
	auto triplet = triplet_trie(list);

	// 0層目
	auto root = triplet.root();
	BOOST_CHECK(std::distance(root.first, root.last) == 5);

	// 1層目
	auto children = triplet.children(root);
	BOOST_CHECK(children.size() == 3);
	auto c0 = children.at(0);
	auto c1 = children.at(1);
	auto c2 = children.at(2);
	BOOST_CHECK(std::distance(c0.first, c0.last) == 2);
	BOOST_CHECK(std::distance(c1.first, c1.last) == 1);
	BOOST_CHECK(std::distance(c2.first, c2.last) == 2);
	BOOST_CHECK(c0.first->substr(c0.position) == "a");
	BOOST_CHECK((c0.first + 1)->substr(c0.position) == "ac");
	BOOST_CHECK(c1.first->substr(c1.position) == "b");
	BOOST_CHECK(c2.first->substr(c2.position) == "cab");
	BOOST_CHECK((c2.first + 1)->substr(c2.position) == "cd");

	// 2層目
	children = triplet.children(c0);
	BOOST_CHECK(children.size() == 1);
	auto c3 = children.at(0);
	BOOST_CHECK(c3.first->substr(c3.position) == "c");

	children = triplet.children(c1);
	BOOST_CHECK(children.size() == 0);

	children = triplet.children(c2);
	BOOST_CHECK(children.size() == 2);
	auto c4 = children.at(0);
	auto c5 = children.at(1);
	BOOST_CHECK(c4.first->substr(c4.position) == "ab");
	BOOST_CHECK(c5.first->substr(c5.position) == "d");

	// 3層目
	children = triplet.children(c3);
	BOOST_CHECK(children.size() == 0);

	children = triplet.children(c4);
	BOOST_CHECK(children.size() == 1);
	auto c6 = children.at(0);
	BOOST_CHECK(c6.first->substr(c6.position) == "b");

	children = triplet.children(c6);
	BOOST_CHECK(children.size() == 0);

	// 4層目
	children = triplet.children(c6);
	BOOST_CHECK(children.size() == 0);
}

// コンテナの型を変更してテスト
BOOST_AUTO_TEST_CASE(triplet_trie__children__2)
{
	using namespace wordring;

	std::list<std::vector<char>> list{ { 'a' }, { 'a', 'c' }, { 'b' }, { 'c', 'a', 'b' }, { 'c', 'd' } };
	auto triplet = triplet_trie(list);

	// 0層目
	auto root = triplet.root();
	BOOST_CHECK(std::distance(root.first, root.last) == 5);
	// 1層目
	auto children = triplet.children(root);
	BOOST_CHECK(children.size() == 3);
}

BOOST_AUTO_TEST_SUITE_END()
