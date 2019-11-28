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
	std::cout << "sizeof(int16_t): " << sizeof(int16_t) << std::endl;

	std::vector<std::string> list{ "a", "ac", "b", "cab", "cd" };
	std::sort(list.begin(), list.end());
	auto triplet = triplet_trie(list);

	// 0層目
	auto root = triplet.root();
	BOOST_CHECK(root.last - root.first == 5);

	// 1層目
	auto children = triplet.children(root);
	BOOST_CHECK(children.size() == 3);
	auto n0 = children.at(0);
	auto n1 = children.at(1);
	auto n2 = children.at(2);
	BOOST_CHECK(n0.size() == 2);
	BOOST_CHECK(n1.size() == 1);
	BOOST_CHECK(n2.size() == 2);
	BOOST_CHECK(list.at(n0.first).substr(n0.position) == "a");
	BOOST_CHECK(list.at(n0.first + 1).substr(n0.position) == "ac");
	BOOST_CHECK(list.at(n1.first).substr(n1.position) == "b");
	BOOST_CHECK(list.at(n2.first).substr(n2.position) == "cab");
	BOOST_CHECK(list.at(n2.first + 1).substr(n2.position) == "cd");

	// 2層目
	children = triplet.children(n0);
	BOOST_CHECK(children.size() == 1);
	auto n3 = children.at(0);
	BOOST_CHECK(list.at(n3.first).substr(n3.position) == "c");

	children = triplet.children(n1);
	BOOST_CHECK(children.size() == 0);

	children = triplet.children(n2);
	BOOST_CHECK(children.size() == 2);
	auto n4 = children.at(0);
	auto n5 = children.at(1);
	BOOST_CHECK(list.at(n4.first).substr(n4.position) == "ab");
	BOOST_CHECK(list.at(n5.first).substr(n5.position) == "d");

	// 3層目
	children = triplet.children(n3);
	BOOST_CHECK(children.size() == 0);

	children = triplet.children(n4);
	BOOST_CHECK(children.size() == 1);
	auto n6 = children.at(0);
	BOOST_CHECK(list.at(n6.first).substr(n6.position) == "b");

	children = triplet.children(n6);
	BOOST_CHECK(children.size() == 0);

	// 4層目
	children = triplet.children(n6);
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
	BOOST_CHECK(root.size() == 5);
	// 1層目
	auto children = triplet.children(root);
	BOOST_CHECK(children.size() == 3);
}

BOOST_AUTO_TEST_CASE(triplet_trie__index__1)
{
	using namespace wordring;

	std::vector<std::string> list{ "a", "ac", "b", "cab", "cd" };
	std::sort(list.begin(), list.end());
	auto triplet = triplet_trie(list);

	// 0層目
	auto root = triplet.root();
	// 1層目
	auto children = triplet.children(root);
	auto n0 = children.at(0);
	auto n1 = children.at(1);
	auto n2 = children.at(2);
	BOOST_CHECK(triplet.to_symbol(n0) == 'a'); // [a], [a]c
	BOOST_CHECK(triplet.to_symbol(n1) == 'b'); // [b]
	BOOST_CHECK(triplet.to_symbol(n2) == 'c'); // [c]ab, [c]d
	// 2層目
	children = triplet.children(n0);
	auto n3 = children.at(0);
	BOOST_CHECK(triplet.to_symbol(n3) == 'c'); // a[c]
	children = triplet.children(n2);
	auto n4 = children.at(0);
	auto n5 = children.at(1);
	BOOST_CHECK(triplet.to_symbol(n4) == 'a'); // c[a]b
	BOOST_CHECK(triplet.to_symbol(n5) == 'd'); // c[d]
	// 3層目
	children = triplet.children(n4);
	auto n6 = children.at(0);
	BOOST_CHECK(triplet.to_symbol(n6) == 'b'); // ca[b]
}

BOOST_AUTO_TEST_CASE(triplet_trie__terminal__1)
{
	using namespace wordring;

	std::vector<std::string> list{ "a", "ac", "b", "cab", "cd" };
	std::sort(list.begin(), list.end());
	auto triplet = triplet_trie(list);

	// 0層目
	auto root = triplet.root();
	// 1層目
	auto children = triplet.children(root);
	auto n0 = children.at(0);
	auto n1 = children.at(1);
	auto n2 = children.at(2);
	BOOST_CHECK(triplet.is_accepted_state(n0)); // [a], [a]c
	BOOST_CHECK(triplet.is_accepted_state(n1)); // [b]
	BOOST_CHECK(triplet.is_accepted_state(n2) == false); // [c]ab, [c]d
	// 2層目
	children = triplet.children(n0);
	auto n3 = children.at(0);
	BOOST_CHECK(triplet.is_accepted_state(n3)); // a[c]
	children = triplet.children(n2);
	auto n4 = children.at(0);
	auto n5 = children.at(1);
	BOOST_CHECK(triplet.is_accepted_state(n4) == false); // c[a]b
	BOOST_CHECK(triplet.is_accepted_state(n5)); // c[d]
	// 3層目
	children = triplet.children(n4);
	auto n6 = children.at(0);
	BOOST_CHECK(triplet.is_accepted_state(n5)); // ca[b]
}

BOOST_AUTO_TEST_SUITE_END()
