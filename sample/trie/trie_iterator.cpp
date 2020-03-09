// sample/trie/trie_iterator.cpp

#include <boost/test/unit_test.hpp>

#include <wordring/trie/trie.hpp>
#include <wordring/trie/trie_iterator.hpp>

#include <cassert>
#include <sstream>
#include <string>

BOOST_AUTO_TEST_SUITE(trie_iterator__sample)

/*
ラベルで遷移できる子を返す

const_trie_iterator operator[](value_type label) const
*/
BOOST_AUTO_TEST_CASE(const_trie_iterator__at__1)
{
	using namespace wordring;

	// Trie木を作成
	std::vector<std::u32string> v{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
	auto t = trie<char32_t>(v.begin(), v.end());

	// 根を指すイテレータを取得する
	auto it = t.begin();

	// ラベル「う」で遷移する子を取得する
	auto it1 = it[U'う'];

	// 検証
	assert(*it1 == U'う');

	// ワーニング回避
	*it1;
}

/*
根からイテレータが指すノードまでのラベル列を返す

void string(String& result) const
*/
BOOST_AUTO_TEST_CASE(const_trie_iterator__string__1)
{
	using namespace wordring;

	// Trie木を作成
	std::vector<std::u32string> v{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
	auto t = trie<char32_t>(v.begin(), v.end());

	// ラベル列「うあ」で遷移したノードを指すイテレータを取得する
	auto it = t.search(std::u32string(U"うあ"));

	// 根からイテレータ「it」迄のラベル列を取得する
	std::u32string s;
	it.string(s);

	// 検証
	assert(s == U"うあ");
}

BOOST_AUTO_TEST_CASE(const_trie_iterator__parent__1)
{
	using namespace wordring;

	// Trie木を作成
	std::vector<std::u32string> v{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
	auto t = trie<char32_t>(v.begin(), v.end());

	// ラベル列「うあ」で遷移したノードを指すイテレータを取得する
	auto it = t.search(std::u32string(U"うあ"));

	// イテレータ「it」の親を取得する
	auto it1 = it.parent();

	// 検証
	assert(*it1 == U'う');

	// ワーニング回避
	*it1;
}

BOOST_AUTO_TEST_SUITE_END()
