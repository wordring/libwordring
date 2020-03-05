// sample/trie/trie.cpp

#include <boost/test/unit_test.hpp>

#include <wordring/tree/tree_iterator.hpp>
#include <wordring/trie/trie.hpp>


BOOST_AUTO_TEST_SUITE(trie__sample)

/*
空のコンテナを構築する

basic_trie()
*/
BOOST_AUTO_TEST_CASE(trie__construct__1)
{
	using namespace wordring;

	auto t = trie<char32_t>();
}

/*
アロケータを指定して空のコンテナを構築する

explicit basic_trie(allocator_type const& alloc)
*/
BOOST_AUTO_TEST_CASE(trie__construct__2)
{
	using namespace wordring;

	auto t = trie<char32_t>(std::allocator<detail::trie_node>());
}

/*
直列化データから構築する

basic_trie(InputIterator first, InputIterator last, allocator_type const& alloc)
*/
BOOST_AUTO_TEST_CASE(trie__construct__3)
{
	using namespace wordring;

	// 直列化データを作成
	std::vector<std::u32string> v1{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
	auto t1 = trie<char32_t>(v1.begin(), v1.end());
	std::vector<std::int32_t> v2{ t1.ibegin(), t1.iend() };

	// 直列化データから構築
	auto t2 = trie<char32_t>(v2.begin(), v2.end());
}

/*
文字列のリストから構築する

basic_trie(ForwardIterator first, ForwardIterator last)
*/
BOOST_AUTO_TEST_CASE(trie__construct__4)
{
	using namespace wordring;

	// 文字列のリスト
	std::vector<std::u32string> v1{ U"あ", U"あう", U"い", U"うあい", U"うえ" };

	// 文字列のリストから構築
	auto t1 = trie<char32_t>(v1.begin(), v1.end());
}

/*
初期化子リストから構築する

basic_trie(std::initializer_list<detail::trie_node> il, allocator_type const& alloc)
*/

BOOST_AUTO_TEST_CASE(trie__construct__5)
{
}

BOOST_AUTO_TEST_SUITE_END()
