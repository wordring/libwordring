// sample/trie/trie_heap.cpp

#include <boost/test/unit_test.hpp>

#include <wordring/serialize/serialize_iterator.hpp>
#include <wordring/trie/trie.hpp>
#include <wordring/trie/trie_heap.hpp>

#include <vector>

BOOST_AUTO_TEST_SUITE(trie_heap__sample)

/*
直列化データから構築する

void assign(InputIterator first, InputIterator last)

※trie_heapは単独で構築できないため、サンプルはtrieを使用。
*/
BOOST_AUTO_TEST_CASE(trie_heap__assign__1)
{
	using namespace wordring;

	// 元となるTrieを作成
	std::vector<std::string> v1{ "a", "ac", "b", "cab", "cd" };
	auto t1 = trie<char>(v1.begin(), v1.end());

	// 直列化データを作成
	std::vector<std::int32_t> v2{ t1.ibegin(), t1.iend() };

	// 直列化データから割り当て
	trie<char> t2;
	t2.assign(v2.begin(), v2.end());
}

/*
直列化用のイテレータを返す

serialize_iterator ibegin() const

直列化イテレータを使用するsample。

※trie_heapは単独で構築できないため、サンプルはtrieを使用。
*/
BOOST_AUTO_TEST_CASE(trie_heap__ibegin__1)
{
	using namespace wordring;

	// 元となるTrieを作成
	std::vector<std::string> v1{ "a", "ac", "b", "cab", "cd" };
	auto t1 = trie<char>(v1.begin(), v1.end());

	// 直列化イテレータを作成
	auto it1 = serialize_iterator(t1.ibegin());
	auto it2 = serialize_iterator(t1.iend());

	// 直列化イテレータから構築
	auto t2 = trie<char>(it1, it2);
}

/*
直列化用のイテレータを返す

serialize_iterator ibegin() const

バイト列に保存するsample。

※trie_heapは単独で構築できないため、サンプルはtrieを使用。
*/
BOOST_AUTO_TEST_CASE(trie_heap__ibegin__2)
{
	using namespace wordring;

	// 元となるTrieを作成
	std::vector<std::string> v1{ "a", "ac", "b", "cab", "cd" };
	auto t1 = trie<char>(v1.begin(), v1.end());

	// 直列化イテレータを作成
	auto it1 = serialize_iterator(t1.ibegin());
	auto it2 = serialize_iterator(t1.iend());

	// バイト列に保存
	auto v2 = std::vector<char>(it1, it2);

	// バイト列から構築
	auto t2 = trie<char>(v2.begin(), v2.end());
}

BOOST_AUTO_TEST_SUITE_END()
