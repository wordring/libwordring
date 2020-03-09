// sample/trie/trie.cpp

#include <boost/test/unit_test.hpp>

#include <wordring/tree/tree_iterator.hpp>
#include <wordring/trie/trie.hpp>

#include <cassert>
#include <sstream>
#include <string>

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
	using namespace wordring;

	// 初期化子リストから構築
	auto t1 = trie<char32_t>({ { 1, 2 }, { 3, 4 }, { 5, 6 } });
}

/*
直列化データから割り当てる

void assign(InputIterator first, InputIterator last)
*/
BOOST_AUTO_TEST_CASE(trie__assign__1)
{
	using namespace wordring;

	// 元となるtrieを作成
	std::vector<std::u32string> v1{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
	auto t1 = trie<char32_t>(v1.begin(), v1.end());

	// 直列化データを作成
	auto v2 = std::vector<std::int32_t>(t1.ibegin(), t1.iend());

	// 直列化データから割り当てる
	trie<char32_t> t2;
	t2.assign(v2.begin(), v2.end());
}

/*
文字列リストから割り当てる

void assign(ForwardIterator first, ForwardIterator last)
*/
BOOST_AUTO_TEST_CASE(trie__assign__2)
{
	using namespace wordring;

	// 文字列リスト
	std::vector<std::u32string> v{ U"あ", U"あう", U"い", U"うあい", U"うえ" };

	// 文字列リストから割り当てる
	trie<char32_t> t;
	t.assign(v.begin(), v.end());
}

/*
葉の値への参照を取得する

reference at(const_iterator pos)
*/
BOOST_AUTO_TEST_CASE(trie__at__1)
{
	using namespace wordring;

	// Trie木を作成
	std::vector<std::u32string> v{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
	auto t = trie<char32_t>(v.begin(), v.end());

	// 検索
	auto pos = t.find(std::u32string(U"あう"));

	// 値への参照を取得
	auto val = t.at(pos);
	// 値を設定
	val = 100;

	// 検証
	assert(t.at(std::u32string(U"あう")) == 100);
}

/*
葉の値への参照を取得する

reference at(InputIterator first, InputIterator last)
*/
BOOST_AUTO_TEST_CASE(trie__at__2)
{
	using namespace wordring;

	// Trie木を作成
	std::vector<std::u32string> v{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
	auto t = trie<char32_t>(v.begin(), v.end());

	// 値への参照を取得
	std::u32string key{ U"あう" };
	auto val = t.at(key.begin(), key.end());
	// 値を設定
	val = 100;

	// 検証
	assert(t.at(std::u32string(U"あう")) == 100);
}

/*
葉の値への参照を取得する

reference at(Key const& key)
*/
BOOST_AUTO_TEST_CASE(trie__at__3)
{
	using namespace wordring;

	// Trie木を作成
	std::vector<std::u32string> v{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
	auto t = trie<char32_t>(v.begin(), v.end());

	// 値への参照を取得
	auto val = t.at(std::u32string(U"あう"));
	// 値を設定
	val = 100;

	// 検証
	assert(t.at(std::u32string(U"あう")) == 100);
}

/*
葉の値への参照を取得する

reference operator[](Key const& key)
*/
BOOST_AUTO_TEST_CASE(trie__at__4)
{
	using namespace wordring;

	// Trie木を作成
	std::vector<std::u32string> v{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
	auto t = trie<char32_t>(v.begin(), v.end());

	// 値への参照を取得して、値を設定
	t[std::u32string(U"あう")] = 100;

	// 検証
	assert(t.at(std::u32string(U"あう")) == 100);
}

/*
キー文字列を挿入する

const_iterator insert(InputIterator first, InputIterator last, value_type value = 0)
*/
BOOST_AUTO_TEST_CASE(trie__insert__1)
{
	using namespace wordring;

	trie<char32_t> t;

	// キー文字列と値100を挿入する
	auto key = std::u32string(U"あう");
	auto it = t.insert(key.begin(), key.end(), 100);

	// 挿入された最後の文字に対応するイテレータが返されている
	assert(*it == U'う');
	// 値100が設定されている
	assert(t.at(it) == 100);
	// 挿入したキー文字列が格納されている
	assert(t.contains(std::u32string(U"あう")));

	// warning 回避用
	*it;
}

/*
キー文字列を挿入する

const_iterator insert(Key const& key, value_type value = 0)
*/
BOOST_AUTO_TEST_CASE(trie__insert__2)
{
	using namespace wordring;

	trie<char32_t> t;

	// キー文字列と値100を挿入する
	auto it = t.insert(std::u32string(U"あう"), 100);

	// 挿入された最後の文字に対応するイテレータが返されている
	assert(*it == U'う');
	// 値100が設定されている
	assert(t.at(it) == 100);
	// 挿入したキー文字列が格納されている
	assert(t.contains(std::u32string(U"あう")));

	// warning 回避用
	*it;
}

/*
キー文字列を削除する

void erase(const_iterator pos)
*/
BOOST_AUTO_TEST_CASE(trie__erase__1)
{
	using namespace wordring;

	// Trie木を作成
	std::vector<std::u32string> v{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
	auto t = trie<char32_t>(v.begin(), v.end());

	// キー文字列を検索する
	auto it = t.find(std::u32string(U"あう"));

	// キーを削除する
	t.erase(it);

	// 検証
	assert(! t.contains(std::u32string(U"あう")));
}

/*
キー文字列を削除する

void erase(InputIterator first, InputIterator last)
*/
BOOST_AUTO_TEST_CASE(trie__erase__2)
{
	using namespace wordring;

	// Trie木を作成
	std::vector<std::u32string> v{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
	auto t = trie<char32_t>(v.begin(), v.end());

	// キー文字列を削除する
	auto key = std::u32string(U"あう");
	t.erase(key.begin(), key.end());

	// 検証
	assert(! t.contains(std::u32string(U"あう")));
}

/*
キー文字列を削除する

void erase(InputIterator first, InputIterator last)
*/
BOOST_AUTO_TEST_CASE(trie__erase__3)
{
	using namespace wordring;

	// Trie木を作成
	std::vector<std::u32string> v{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
	auto t = trie<char32_t>(v.begin(), v.end());

	// キー文字列を削除する
	t.erase(std::u32string(U"あう"));

	// 検証
	assert(! t.contains(std::u32string(U"あう")));
}

/*
部分一致検索

auto lookup(InputIterator first, InputIterator last) const
*/
BOOST_AUTO_TEST_CASE(trie__lookup__1)
{
	using namespace wordring;

	// Trie木を作成
	std::vector<std::u32string> v{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
	auto t = trie<char32_t>(v.begin(), v.end());

	// キー文字列を部分一致検索する
	std::u32string s{ U"うい" };
	auto pair = t.lookup(s.begin(), s.end());

	// 一致した最後のノードと
	assert(*pair.first == U'う');
	// 一致した最後のキー文字の次を返す
	assert(*pair.second == U'い');

	// warning 回避用
	*pair.first;
}

/*
前方一致検索

const_iterator search(Key const& key) const
*/
BOOST_AUTO_TEST_CASE(trie__search__1)
{
	using namespace wordring;

	// Trie木を作成
	std::vector<std::u32string> v{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
	auto t = trie<char32_t>(v.begin(), v.end());

	// キー文字列を前方一致検索する
	auto it = t.search(std::u32string(U"うあ"));

	// 葉以外のノードにも一致する
	assert(!it);
	// 検索文字列全体のみに一致する
	assert(*it == U'あ');

	// warning 回避用
	*it;
}

/*
完全一致検索

const_iterator find(Key const& key) const
*/
BOOST_AUTO_TEST_CASE(trie__find__1)
{
	using namespace wordring;

	// Trie木を作成
	std::vector<std::u32string> v{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
	auto t = trie<char32_t>(v.begin(), v.end());

	// キー文字列を完全一致検索する
	auto it = t.find(std::u32string(U"あ"));

	// 葉のみに一致する
	BOOST_CHECK(it);
	// 検索文字列全体のみに一致する
	BOOST_CHECK(*it == U'あ');
}

/*
キー文字列が格納されているか調べる

bool contains(Key const& key) const
*/
BOOST_AUTO_TEST_CASE(trie__contains__1)
{
	using namespace wordring;

	// Trie木を作成
	std::vector<std::u32string> v{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
	auto t = trie<char32_t>(v.begin(), v.end());

	// キー文字列「うあい」は格納されている
	assert(t.contains(std::u32string(U"うあい")));
	// キー文字列「え」は格納されていない
	assert(t.contains(std::u32string(U"え")) == false);
}

/*!
ストリーム入出力

inline std::ostream& operator<<(std::ostream& os, basic_trie<Label1, Base1> const& trie)
inline std::istream& operator>>(std::istream& is, basic_trie<Label1, Base1>& trie)
*/
BOOST_AUTO_TEST_CASE(trie__stream__1)
{
	using namespace wordring;

	// Trie木を作成
	std::vector<std::u32string> v{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
	auto t1 = trie<char32_t>(v.begin(), v.end());

	// ストリームへ出力
	std::stringstream ss;
	ss << t1;

	// ストリームから入力
	trie<char32_t> t2;
	ss >> t2;

	// 検証
	assert(t1.size() == t2.size());
}

BOOST_AUTO_TEST_SUITE_END()
