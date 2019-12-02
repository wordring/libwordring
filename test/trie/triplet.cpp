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

// 一つの8ビット文字列をinsertし、8ビット記号列として検索出来ることを確認する。
BOOST_AUTO_TEST_CASE(triplet_trie__insert__1)
{
	using namespace wordring;
	using node = triplet_trie::node;

	triplet_trie t{};
	t.insert(t.begin(), "ABCDE");
	std::vector<uint8_t> v1{ 0x41u, 0x42u, 0x43u, 0x44u, 0x45u };
	std::vector<uint8_t> v2{};

	for (auto n = node(t, t.root().children().at(0)); n.count() != 0;)
	{
		v2.push_back(static_cast<uint8_t>(n.symbol()));
		n = (n.children().size()) ? node(t, n.children().at(0)) : node();
	}

	BOOST_CHECK(v1 == v2);
}

// 一つの16ビット文字列をinsertし、8ビット記号列として検索出来ることを確認する。
BOOST_AUTO_TEST_CASE(triplet_trie__insert__2)
{
	using namespace wordring;
	using node = triplet_trie::node;

	triplet_trie t{};
	t.insert(t.begin(), u"あいうえお");
	std::vector<uint8_t> v1{ 0x30u, 0x42u, 0x30u, 0x44u, 0x30u, 0x46u, 0x30u, 0x48u, 0x30u, 0x4Au };
	std::vector<uint8_t> v2{};

	for (auto n = node(t, t.root().children().at(0)); n.count() != 0;)
	{
		v2.push_back(static_cast<uint8_t>(n.symbol()));
		n = (n.children().size()) ? node(t, n.children().at(0)) : node();
	}

	BOOST_CHECK(v1 == v2);
}

// 標準コンテナにinsertすると、引数として与えた挿入位置と同一の位置が戻り値として返ることを確認。
// ※挿入するとイテレータは無効になるため、同じ位置を指す有効なイテレータが返る。
BOOST_AUTO_TEST_CASE(triplet_trie__insert__3)
{
	using namespace wordring;
	using node = triplet_trie::node;

	std::vector<uint8_t> v1{};
	auto it1 = v1.insert(v1.begin(), 0x30u);
	BOOST_CHECK(it1 == v1.begin());

	std::vector<uint8_t> v2{};
	std::vector<uint8_t> v3{ 0x30u, 0x42u };
	auto it2 = v2.insert(v2.begin(), v3.begin(), v3.end());
	BOOST_CHECK(it2 == v2.begin());
}

// 記号列のリストをinsertし、挿入されていることを確認する。
BOOST_AUTO_TEST_CASE(triplet_trie__insert__4)
{
	using namespace wordring;
	using node = triplet_trie::node;

	triplet_trie t{};
	std::vector<std::u16string> vs1{ u"一あいうえお", u"企かきくけこ" };
	auto it1 = t.insert(t.begin(), vs1.begin(), vs1.end());
	BOOST_CHECK(t.root().count() == 2);

	std::vector<uint8_t> v1{ 0x4Eu, 0x00u, 0x30u, 0x42u, 0x30u, 0x44u, 0x30u, 0x46u, 0x30u, 0x48u, 0x30u, 0x4Au };
	std::vector<uint8_t> v2{};
	for (auto n = node(t, t.root().children().at(0)); n.count() != 0;)
	{
		v2.push_back(static_cast<uint8_t>(n.symbol()));
		n = (n.children().size()) ? node(t, n.children().at(0)) : node();
	}
	BOOST_CHECK(v1 == v2);

	std::vector<uint8_t> v3{ 0x4Fu, 0x01u, 0x30u, 0x4Bu, 0x30u, 0x4Du, 0x30u, 0x4Fu, 0x30u, 0x51u, 0x30u, 0x53u };
	std::vector<uint8_t> v4{};
	for (auto n = node(t, t.root().children().at(1)); n.count() != 0;)
	{
		v4.push_back(static_cast<uint8_t>(n.symbol()));
		n = (n.children().size()) ? node(t, n.children().at(0)) : node();
	}
	BOOST_CHECK(v3 == v4);
}

// 一つの16ビット文字列をpush_backし、8ビット記号列として挿入されていることを確認する。
BOOST_AUTO_TEST_CASE(triplet_trie__push_back__1)
{
	using namespace wordring;
	using node = triplet_trie::node;

	triplet_trie t{};
	t.push_back(u"あいうえお");
	std::vector<uint8_t> v1{ 0x30u, 0x42u, 0x30u, 0x44u, 0x30u, 0x46u, 0x30u, 0x48u, 0x30u, 0x4Au };
	std::vector<uint8_t> v2{};

	for (auto n = node(t, t.root().children().at(0)); n.count() != 0;)
	{
		v2.push_back(static_cast<uint8_t>(n.symbol()));
		n = (n.children().size()) ? node(t, n.children().at(0)) : node();
	}
	BOOST_CHECK(v1 == v2);
}

// フルチェック
BOOST_AUTO_TEST_CASE(triplet_trie__node__children__1)
{
	using namespace wordring;
	using node = triplet_trie::node;

	std::cout << "sizeof(wordring::triplet_trie::node): " << sizeof(triplet_trie::node) << std::endl;
	std::cout << "sizeof(int16_t): " << sizeof(int16_t) << std::endl;

	std::vector<std::string> list{ "a", "ac", "b", "cab", "cd" };
	std::sort(list.begin(), list.end());
	auto t = triplet_trie(list.begin(), list.end());

	// 0層目
	auto root = t.root();
	BOOST_CHECK(root.count() == 5);

	// 1層目
	auto c = root.children();
	BOOST_CHECK(c.size() == 3);

	auto n0 = node(t, c.at(0)); // [a], [a]c
	auto n1 = node(t, c.at(1)); // [b]
	auto n2 = node(t, c.at(2)); // [c]ab, [c]d

	BOOST_CHECK(n0.count() == 2);
	BOOST_CHECK(n1.count() == 1);
	BOOST_CHECK(n2.count() == 2);
	BOOST_CHECK(list.at(n0.first()).substr(n0.position()) == "a");
	BOOST_CHECK(list.at(n0.first() + 1).substr(n0.position()) == "ac");
	BOOST_CHECK(list.at(n1.first()).substr(n1.position()) == "b");
	BOOST_CHECK(list.at(n2.first()).substr(n2.position()) == "cab");
	BOOST_CHECK(list.at(n2.first() + 1).substr(n2.position()) == "cd");

	// 2層目
	c = n0.children();
	BOOST_CHECK(c.size() == 1);

	auto n3 = node(t, c.at(0)); // a[c]
	BOOST_CHECK(list.at(n3.first()).substr(n3.position()) == "c");

	c = n1.children();
	BOOST_CHECK(c.size() == 0);

	c = n2.children();
	BOOST_CHECK(c.size() == 2);

	auto n4 = node(t, c.at(0)); // c[a]b
	auto n5 = node(t, c.at(1)); // c[d]
	BOOST_CHECK(list.at(n4.first()).substr(n4.position()) == "ab");
	BOOST_CHECK(list.at(n5.first()).substr(n5.position()) == "d");

	// 3層目
	c = n3.children();
	BOOST_CHECK(c.size() == 0);

	c = n4.children();
	BOOST_CHECK(c.size() == 1);

	auto n6 = node(t, c.at(0)); // ca[b]
	BOOST_CHECK(list.at(n6.first()).substr(n6.position()) == "b");

	// 4層目
	c = n6.children();
	BOOST_CHECK(c.size() == 0);
}

// コンテナの型を変更してテスト
BOOST_AUTO_TEST_CASE(triplet_trie__node__children__2)
{
	using namespace wordring;

	std::list<std::vector<char>> list{ { 'a' }, { 'a', 'c' }, { 'b' }, { 'c', 'a', 'b' }, { 'c', 'd' } };
	auto t = triplet_trie(list.begin(), list.end());

	// 0層目
	auto root = t.root();
	BOOST_CHECK(root.count() == 5);
	// 1層目
	auto c = root.children();
	BOOST_CHECK(c.size() == 3);
}

BOOST_AUTO_TEST_CASE(triplet_trie__node__symbol__1)
{
	using namespace wordring;
	using node = triplet_trie::node;

	std::vector<std::string> list{ "a", "ac", "b", "cab", "cd" };
	std::sort(list.begin(), list.end());
	auto t = triplet_trie(list.begin(), list.end());

	// 0層目
	auto root = t.root();
	// 1層目
	auto c = root.children();
	auto n0 = node(t, c.at(0));
	auto n1 = node(t, c.at(1));
	auto n2 = node(t, c.at(2));
	BOOST_CHECK(n0.symbol() == 'a'); // [a], [a]c
	BOOST_CHECK(n1.symbol() == 'b'); // [b]
	BOOST_CHECK(n2.symbol() == 'c'); // [c]ab, [c]d

	// 2層目
	c = n0.children();
	auto n3 = node(t, c.at(0));
	BOOST_CHECK(n3.symbol() == 'c'); // a[c]

	c = n2.children();
	auto n4 = node(t, c.at(0));
	auto n5 = node(t, c.at(1));
	BOOST_CHECK(n4.symbol() == 'a'); // c[a]b
	BOOST_CHECK(n5.symbol() == 'd'); // c[d]
						
	// 3層目
	c = n4.children();
	auto n6 = node(t, c.at(0));
	BOOST_CHECK(n6.symbol() == 'b'); // ca[b]
}

BOOST_AUTO_TEST_CASE(triplet_trie__node__is_terminal__1)
{
	using namespace wordring;
	using node = triplet_trie::node;

	std::vector<std::string> list{ "a", "ac", "b", "cab", "cd" };
	std::sort(list.begin(), list.end());
	auto t = triplet_trie(list.begin(), list.end());

	// 0層目
	auto root = t.root();

	// 1層目
	auto c = root.children();
	auto n0 = node(t, c.at(0));
	auto n1 = node(t, c.at(1));
	auto n2 = node(t, c.at(2));
	BOOST_CHECK(n0.is_terminal() == true);  // [a], [a]c
	BOOST_CHECK(n1.is_terminal() == true);  // [b]
	BOOST_CHECK(n2.is_terminal() == false); // [c]ab, [c]d

	// 2層目
	c = n0.children();
	auto n3 = node(t, c.at(0));
	BOOST_CHECK(n3.is_terminal()); // a[c]
	c = n2.children();
	auto n4 = node(t, c.at(0));
	auto n5 = node(t, c.at(1));
	BOOST_CHECK(n4.is_terminal() == false); // c[a]b
	BOOST_CHECK(n5.is_terminal() == true);  // c[d]

	// 3層目
	c = n4.children();
	auto n6 = node(t, c.at(0));
	BOOST_CHECK(n5.is_terminal() == true); // ca[b]
}


BOOST_AUTO_TEST_CASE(triplet_trie__indexes__1)
{
	using namespace wordring;
	using node = triplet_trie::node;

	std::vector<std::string> list{ "a", "ac", "b", "cab", "cd" };
	std::sort(list.begin(), list.end());
	auto t = triplet_trie(list.begin(), list.end());

	std::bitset<256> b{};
	b.set(0x61).set(0x62).set(0x63);
	BOOST_CHECK(t.root().indexes() == b);
}

BOOST_AUTO_TEST_SUITE_END()
