// sample/string/atom.cpp

#include <boost/test/unit_test.hpp>

#include <wordring/string/atom.hpp>

#include <algorithm>
#include <iterator>
#include <memory>
#include <string>

BOOST_AUTO_TEST_SUITE(atom__sample)

// ----------------------------------------------------------------------------
// basic_atom
// ----------------------------------------------------------------------------

/*
空のアトムを構築する

basic_atom()
*/
BOOST_AUTO_TEST_CASE(basic_atom__construct__1)
{
	using namespace wordring::detail;

	// 空のアトムを構築する
	basic_atom<std::string> a;
}

/*
アトムをIDに変換する

operator std::uint32_t() const
*/
BOOST_AUTO_TEST_CASE(basic_atom__id__1)
{
	using namespace wordring;
	using namespace wordring::detail;

	// コンテナを構築する
	std::vector<std::u32string> v{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
	auto as = basic_atom_set<std::u32string>(v.begin(), v.end());

	// コンテナからアトムを取得する
	auto a = as.at(U"い");

	// アトムをIDに変換する
	auto i = static_cast<std::uint32_t>(a);

	// 検証
	assert(i != 0);

	// ワーニング回避
	++i;
}

/*
アトムを文字列に変換する

operator string_type() const
*/
BOOST_AUTO_TEST_CASE(basic_atom__string__1)
{
	using namespace wordring;
	using namespace wordring::detail;

	// コンテナを構築する
	std::vector<std::u32string> v{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
	auto as = basic_atom_set<std::u32string>(v.begin(), v.end());

	// コンテナからアトムを取得する
	auto a = as.at(U"い");

	// アトムを文字列に変換する
	auto s = static_cast<std::u32string>(a);

	// 検証
	assert(s == U"い");

	// ワーニング回避
	s = U"";
}

/*
アトムを文字列に変換する

string_type& string(string_type& result) const
*/
BOOST_AUTO_TEST_CASE(basic_atom__string__2)
{
	using namespace wordring;
	using namespace wordring::detail;

	// コンテナを構築する
	std::vector<std::u32string> v{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
	auto as = basic_atom_set<std::u32string>(v.begin(), v.end());

	// コンテナからアトムを取得する
	auto a = as.at(U"い");

	std::u32string s;
	// アトムを文字列に変換する
	a.string(s);

	// 検証
	assert(s == U"い");

	// ワーニング回避
	s = U"";
}

/*
二つのアトムが等しいか調べる

inline bool operator==(basic_atom<String1, Allocator1> const& lhs, basic_atom<String1, Allocator1> const& rhs)
*/
BOOST_AUTO_TEST_CASE(basic_atom__equal__1)
{
	using namespace wordring;
	using namespace wordring::detail;

	// アトム・コンテナを構築する
	std::vector<std::u32string> v{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
	auto as = basic_atom_set<std::u32string>(v.begin(), v.end());

	// コンテナからアトムを取得する
	auto a1 = as.at(U"あ");
	auto a2 = as.at(U"あ");

	// 二つのアトムが等しいか調べる
	assert(a1 == a2);

	// ワーニング回避
	std::uint32_t i = a1;
	i = a2;
	++i;
}

/*
二つのアトムが等しくないか調べる

inline bool operator!=(basic_atom<String1, Allocator1> const& lhs, basic_atom<String1, Allocator1> const& rhs)
*/
BOOST_AUTO_TEST_CASE(basic_atom__not_equal__1)
{
	using namespace wordring;
	using namespace wordring::detail;

	// アトム・コンテナを構築する
	std::vector<std::u32string> v{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
	auto as = basic_atom_set<std::u32string>(v.begin(), v.end());

	// コンテナからアトムを取得する
	auto a1 = as.at(U"あ");
	auto a2 = as.at(U"あう");

	// 二つのアトムが等しくないか調べる
	assert(a1 != a2);

	// ワーニング回避
	std::uint32_t i = a1;
	i = a2;
	++i;
}

// ----------------------------------------------------------------------------
// basic_atom_set
// ----------------------------------------------------------------------------

/*
空のコンテナを構築する

basic_atom_set()
*/
BOOST_AUTO_TEST_CASE(basic_atom_set__construct__1)
{
	using namespace wordring;

	// 空のコンテナを構築する
	basic_atom_set<std::string> as;
}

/*
アロケータを指定して空のコンテナを構築する

explicit basic_atom_set(allocator_type const& alloc)
*/
BOOST_AUTO_TEST_CASE(basic_atom_set__construct__2)
{
	using namespace wordring;

	// アロケータを指定して空のコンテナを構築する
	basic_atom_set<std::string> as(std::allocator<detail::trie_node>{});
}

/*
直列化データからの構築

basic_atom_set(InputIterator first, InputIterator last, allocator_type const& alloc = allocator_type())
*/
BOOST_AUTO_TEST_CASE(basic_atom_set__construct__3)
{
	using namespace wordring;

	// 元となるコンテナを作成
	std::vector<std::u32string> v1{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
	basic_atom_set<std::u32string> as1(v1.begin(), v1.end());

	//直列化データを作成
	auto v2 = std::vector<std::uint32_t>(as1.ibegin(), as1.iend());

	// 直列化データから構築
	auto as2 = basic_atom_set<std::u32string>(v2.begin(), v2.end());

	// 検証
	assert(as2.size() == 5);
}

/*
文字列リストからの構築

basic_atom_set(InputIterator first, InputIterator last, allocator_type const& alloc = allocator_type())
*/
BOOST_AUTO_TEST_CASE(basic_atom_set__construct__4)
{
	using namespace wordring;

	// 文字列リスト
	std::vector<std::u32string> v{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
	
	// 文字列リストから構築
	auto as = basic_atom_set<std::u32string>(v.begin(), v.end());

	// 検証
	assert(as.size() == 5);
}

/*
直列化データからの割り当て

void assign(InputIterator first, InputIterator last)
*/
BOOST_AUTO_TEST_CASE(basic_atom_set__assign__1)
{
	using namespace wordring;

	// 元となるコンテナを作成
	std::vector<std::u32string> v1{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
	basic_atom_set<std::u32string> as1(v1.begin(), v1.end());

	//直列化データを作成
	auto v2 = std::vector<std::uint32_t>(as1.ibegin(), as1.iend());

	// 直列化データから割り当て
	auto as2 = basic_atom_set<std::u32string>(v2.begin(), v2.end());

	// 検証
	assert(as2.size() == 5);
}

/*
文字列リストからの割り当て

void assign(InputIterator first, InputIterator last)
*/
BOOST_AUTO_TEST_CASE(basic_atom_set__assign__2)
{
	using namespace wordring;

	// 文字列リスト
	std::vector<std::u32string> v{ U"あ", U"あう", U"い", U"うあい", U"うえ" };

	basic_atom_set<std::u32string> as;
	// 文字列リストから割り当て
	as.assign(v.begin(), v.end());

	// 検証
	assert(as.size() == 5);
}

// 要素アクセス ----------------------------------------------------------------

/*
IDからアトムを返す

value_type at(std::uint32_t id) const
*/
BOOST_AUTO_TEST_CASE(basic_atom_set__at__1)
{
	using namespace wordring;

	// コンテナを作成
	std::vector<std::u32string> v1{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
	auto as = basic_atom_set<std::u32string>(v1.begin(), v1.end());

	// アトムIDを取得
	std::uint32_t id = as.at(std::u32string(U"あう"));

	// IDからアトムを取得する
	auto a = as.at(id);

	// 検証
	assert(static_cast<std::u32string>(a) == U"あう");

	// ワーニング回避
	std::uint32_t i = a;
	++i;
}

/*
文字列からアトムを返す

value_type at(InputIterator first, InputIterator last) const
*/
BOOST_AUTO_TEST_CASE(basic_atom_set__at__2)
{
	using namespace wordring;

	// コンテナを作成
	std::vector<std::u32string> v1{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
	auto as = basic_atom_set<std::u32string>(v1.begin(), v1.end());

	auto s = std::u32string(U"あう");
	// 文字列からアトムを取得する
	auto a = as.at(s.begin(), s.end());

	// 検証
	assert(static_cast<std::u32string>(a) == U"あう");

	// ワーニング回避
	std::uint32_t i = a;
	++i;
}

/*
文字列からアトムを返す

value_type at(std::basic_string_view<label_type> sv) const
*/
BOOST_AUTO_TEST_CASE(basic_atom_set__at__3)
{
	using namespace wordring;

	// コンテナを作成
	std::vector<std::u32string> v1{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
	auto as = basic_atom_set<std::u32string>(v1.begin(), v1.end());

	// 文字列からアトムを取得する
	auto a = as.at(U"あう");

	// 検証
	assert(static_cast<std::u32string>(a) == U"あう");

	// ワーニング回避
	std::uint32_t i = a;
	++i;
}

/*
文字列からアトムを返す

value_type operator[](std::basic_string_view<label_type> sv)
*/
BOOST_AUTO_TEST_CASE(basic_atom_set__at__4)
{
	using namespace wordring;

	// コンテナを作成
	std::vector<std::u32string> v1{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
	auto as = basic_atom_set<std::u32string>(v1.begin(), v1.end());

	// 文字列からアトムを取得する
	auto a = as[U"え"];

	// 検証
	assert(static_cast<std::u32string>(a) == U"え");

	// ワーニング回避
	std::uint32_t i = a;
	++i;
}

// 変更 -----------------------------------------------------------------------

/*
文字列を挿入する

value_type insert(InputIterator first, InputIterator last)
*/
BOOST_AUTO_TEST_CASE(basic_atom_set__insert__1)
{
	using namespace wordring;

	// 空のコンテナを作成
	basic_atom_set<std::u32string> as;

	std::u32string s(U"あ");
	// 文字列を挿入する
	auto a = as.insert(s.begin(), s.end());

	// 検証
	assert(static_cast<std::u32string>(a) == U"あ");
	assert(as.size() == 1);

	// ワーニング回避
	std::uint32_t i = a;
	++i;
}

/*
文字列を挿入する

value_type insert(std::basic_string_view<label_type> sv)
*/
BOOST_AUTO_TEST_CASE(basic_atom_set__insert__2)
{
	using namespace wordring;

	// 空のコンテナを作成
	basic_atom_set<std::u32string> as;

	// 文字列を挿入する
	auto a = as.insert(U"あ");

	// 検証
	assert(static_cast<std::u32string>(a) == U"あ");
	assert(as.size() == 1);

	// ワーニング回避
	std::uint32_t i = a;
	++i;
}

/*
アトムを削除する

void erase(std::uint32_t id)
*/
BOOST_AUTO_TEST_CASE(basic_atom_set__erase__1)
{
	using namespace wordring;

	std::vector<std::u32string> v1{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
	// コンテナを構築する
	auto as = basic_atom_set<std::u32string>(v1.begin(), v1.end());

	// IDを取得する
	std::uint32_t id = as.at(U"い");

	// アトムを削除する
	as.erase(id);

	// 検証
	assert(as.contains(U"い") == false);
	assert(as.size() == 4);
}

/*
アトムを削除する

void erase(InputIterator first, InputIterator last)
*/
BOOST_AUTO_TEST_CASE(basic_atom_set__erase__3)
{
	using namespace wordring;

	std::vector<std::u32string> v1{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
	// コンテナを構築する
	auto as = basic_atom_set<std::u32string>(v1.begin(), v1.end());

	auto s = std::u32string(U"うあい");
	// アトムを削除する
	as.erase(s.begin(), s.end());

	// 検証
	assert(as.contains(U"うあい") == false);
	assert(as.size() == 4);
}

/*
アトムを削除する

void erase(std::basic_string_view<label_type> sv)
*/
BOOST_AUTO_TEST_CASE(basic_atom_set__erase__4)
{
	using namespace wordring;

	std::vector<std::u32string> v1{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
	// コンテナを構築する
	auto as = basic_atom_set<std::u32string>(v1.begin(), v1.end());

	// アトムを削除する
	as.erase(U"うあい");

	// 検証
	assert(as.contains(U"うあい") == false);
	assert(as.size() == 4);
}

// 検索 -----------------------------------------------------------------------

/*
文字列が格納されているか調べる

bool contains(InputIterator first, InputIterator last) const
bool contains(std::basic_string_view<label_type> sv) const
*/
BOOST_AUTO_TEST_CASE(basic_atom_set__contains__1)
{
	using namespace wordring;

	std::vector<std::u32string> v1{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
	// コンテナを構築する
	auto as = basic_atom_set<std::u32string>(v1.begin(), v1.end());

	// 文字列が格納されているか調べる
	bool b = as.contains(U"あ");

	// 検証
	assert(b == true);

	// ワーニング回避
	b = false;
}

BOOST_AUTO_TEST_SUITE_END()
