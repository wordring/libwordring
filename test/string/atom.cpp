// test/string/atom.cpp

#include <boost/test/unit_test.hpp>

#include <wordring/string/atom.hpp>

#include <algorithm>
#include <iterator>
#include <memory>
#include <string>

BOOST_AUTO_TEST_SUITE(atom__test)

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

	std::vector<std::u32string> v{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
	auto as = basic_atom_set<std::u32string>(v.begin(), v.end());

	auto a = as.at(U"い");
	std::uint32_t i = a;

	BOOST_CHECK(i != 0);
}

BOOST_AUTO_TEST_CASE(basic_atom__id__2)
{
	using namespace wordring;
	using namespace wordring::detail;

	std::vector<std::u32string> v{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
	auto as = basic_atom_set<std::u32string>(v.begin(), v.end());

	auto a = as.at(U"");
	std::uint32_t i = a;

	BOOST_CHECK(i == 0);
}

/*
アトムを文字列に変換する

operator string_type() const
*/
BOOST_AUTO_TEST_CASE(basic_atom__string__1)
{
	using namespace wordring;
	using namespace wordring::detail;

	std::vector<std::u32string> v{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
	auto as = basic_atom_set<std::u32string>(v.begin(), v.end());

	auto a = as.at(U"い");
	std::u32string s = a;

	BOOST_CHECK(s == U"い");
}

BOOST_AUTO_TEST_CASE(basic_atom__string__2)
{
	using namespace wordring;
	using namespace wordring::detail;

	std::vector<std::u32string> v{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
	auto as = basic_atom_set<std::u32string>(v.begin(), v.end());

	auto a = as.at(U"");
	std::u32string s = a;

	BOOST_CHECK(s == U"");
}

/*
アトムを文字列に変換する

string_type& string(string_type& result) const
*/
BOOST_AUTO_TEST_CASE(basic_atom__string__3)
{
	using namespace wordring;
	using namespace wordring::detail;

	std::vector<std::u32string> v{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
	auto as = basic_atom_set<std::u32string>(v.begin(), v.end());

	auto a = as.at(U"い");
	std::u32string s;
	a.string(s);

	BOOST_CHECK(s == U"い");
}

BOOST_AUTO_TEST_CASE(basic_atom__string__4)
{
	using namespace wordring;
	using namespace wordring::detail;

	std::vector<std::u32string> v{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
	auto as = basic_atom_set<std::u32string>(v.begin(), v.end());

	auto a = as.at(U"");
	std::u32string s;
	a.string(s);

	BOOST_CHECK(s == U"");
}

/*
二つのアトムが等しいか調べる

inline bool operator==(basic_atom<String1, Allocator1> const& lhs, basic_atom<String1, Allocator1> const& rhs)
*/
BOOST_AUTO_TEST_CASE(basic_atom__equal__1)
{
	using namespace wordring;
	using namespace wordring::detail;

	std::vector<std::u32string> v{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
	auto as = basic_atom_set<std::u32string>(v.begin(), v.end());

	auto a1 = as.at(U"あ");
	auto a2 = as.at(U"あ");

	BOOST_CHECK(a1 == a2);
}

/*
二つのアトムが等しくないか調べる

inline bool operator!=(basic_atom<String1, Allocator1> const& lhs, basic_atom<String1, Allocator1> const& rhs)
*/
BOOST_AUTO_TEST_CASE(basic_atom__not_equal__1)
{
	using namespace wordring;
	using namespace wordring::detail;

	std::vector<std::u32string> v{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
	auto as = basic_atom_set<std::u32string>(v.begin(), v.end());

	auto a1 = as.at(U"あ");
	auto a2 = as.at(U"あう");

	BOOST_CHECK(a1 != a2);
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

	basic_atom_set<std::string> as;
}

/*
アロケータを指定して空のコンテナを構築する

explicit basic_atom_set(allocator_type const& alloc)
*/
BOOST_AUTO_TEST_CASE(basic_atom_set__construct__2)
{
	using namespace wordring;

	basic_atom_set<std::string> as(std::allocator<detail::trie_node>{});
}

/*
直列化データからの構築

basic_atom_set(InputIterator first, InputIterator last, allocator_type const& alloc = allocator_type())
*/
BOOST_AUTO_TEST_CASE(basic_atom_set__construct__3)
{
	using namespace wordring;

	std::vector<std::u32string> v{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
	basic_atom_set<std::u32string> as1(v.begin(), v.end());

	basic_atom_set<std::u32string> as2(as1.ibegin(), as1.iend());

	BOOST_CHECK(as2.contains(std::u32string(U"あ")));
	BOOST_CHECK(as2.contains(std::u32string(U"あう")));
	BOOST_CHECK(as2.contains(std::u32string(U"い")));
	BOOST_CHECK(as2.contains(std::u32string(U"うあい")));
	BOOST_CHECK(as2.contains(std::u32string(U"うえ")));
}

/*
文字列リストからの構築

basic_atom_set(InputIterator first, InputIterator last, allocator_type const& alloc = allocator_type())
*/
BOOST_AUTO_TEST_CASE(basic_atom_set__construct__4)
{
	using namespace wordring;

	std::vector<std::u32string> v{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
	basic_atom_set<std::u32string> as(v.begin(), v.end());

	BOOST_CHECK(as.contains(std::u32string(U"あ")));
	BOOST_CHECK(as.contains(std::u32string(U"あう")));
	BOOST_CHECK(as.contains(std::u32string(U"い")));
	BOOST_CHECK(as.contains(std::u32string(U"うあい")));
	BOOST_CHECK(as.contains(std::u32string(U"うえ")));
}

/*
直列化データからの割り当て

void assign(InputIterator first, InputIterator last)
*/
BOOST_AUTO_TEST_CASE(basic_atom_set__assign__1)
{
	using namespace wordring;

	std::vector<std::u32string> v1{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
	auto as1 = basic_atom_set<std::u32string>(v1.begin(), v1.end());
	auto v2 = std::vector<std::int32_t>(as1.ibegin(), as1.iend());

	auto as2 = basic_atom_set<std::u32string>(v2.begin(), v2.end());

	BOOST_CHECK(as2.contains(std::u32string(U"あ")));
	BOOST_CHECK(as2.contains(std::u32string(U"あう")));
	BOOST_CHECK(as2.contains(std::u32string(U"い")));
	BOOST_CHECK(as2.contains(std::u32string(U"うあい")));
	BOOST_CHECK(as2.contains(std::u32string(U"うえ")));

	BOOST_CHECK(as2.contains(std::u32string(U"え")) == false);
}

/*
文字列リストからの割り当て

void assign(InputIterator first, InputIterator last)
*/
BOOST_AUTO_TEST_CASE(basic_atom_set__assign__2)
{
	using namespace wordring;

	std::vector<std::u32string> v{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
	basic_atom_set<std::u32string> as;
	
	as.assign(v.begin(), v.end());

	BOOST_CHECK(as.contains(std::u32string(U"あ")));
	BOOST_CHECK(as.contains(std::u32string(U"あう")));
	BOOST_CHECK(as.contains(std::u32string(U"い")));
	BOOST_CHECK(as.contains(std::u32string(U"うあい")));
	BOOST_CHECK(as.contains(std::u32string(U"うえ")));

	BOOST_CHECK(as.contains(std::u32string(U"え")) == false);
}

// 要素アクセス ----------------------------------------------------------------

/*
IDからアトムを返す

value_type at(std::uint32_t id) const
*/
BOOST_AUTO_TEST_CASE(basic_atom_set__at__1)
{
	using namespace wordring;

	std::vector<std::u32string> v1{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
	auto as = basic_atom_set<std::u32string>(v1.begin(), v1.end());

	auto a = as.at(U"あう");
	std::uint32_t i = a;

	BOOST_CHECK(a);
	BOOST_CHECK(static_cast<std::u32string>(as.at(i)) == std::u32string(U"あう"));
}

BOOST_AUTO_TEST_CASE(basic_atom_set__at__2)
{
	using namespace wordring;

	std::vector<std::u32string> v1{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
	auto as = basic_atom_set<std::u32string>(v1.begin(), v1.end());

	auto a = as.at(U"え");
	std::uint32_t i = a;

	BOOST_CHECK(!a);
	BOOST_CHECK(i == 0);
}

BOOST_AUTO_TEST_CASE(basic_atom_set__at__3)
{
	using namespace wordring;

	std::vector<std::u32string> v1{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
	auto as = basic_atom_set<std::u32string>(v1.begin(), v1.end());

	auto a = as.at(U"");
	std::uint32_t i = a;

	BOOST_CHECK(!a);
	BOOST_CHECK(i == 0);
}

BOOST_AUTO_TEST_CASE(basic_atom_set__at__4)
{
	using namespace wordring;

	basic_atom_set<std::u32string> as;

	auto a = as.at(U"え");
	std::uint32_t i = a;

	BOOST_CHECK(!a);
	BOOST_CHECK(i == 0);
}

BOOST_AUTO_TEST_CASE(basic_atom_set__at__5)
{
	using namespace wordring;

	basic_atom_set<std::u32string> as;

	auto a = as.at(U"");
	std::uint32_t i = a;

	BOOST_CHECK(!a);
	BOOST_CHECK(i == 0);
}

/*
文字列からアトムを返す

value_type at(InputIterator first, InputIterator last) const
*/
BOOST_AUTO_TEST_CASE(basic_atom_set__at__6)
{
	using namespace wordring;

	std::vector<std::u32string> v1{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
	auto as = basic_atom_set<std::u32string>(v1.begin(), v1.end());
	
	auto s = std::u32string(U"あう");
	auto a = as.at(s.begin(), s.end());

	BOOST_CHECK(a);
	BOOST_CHECK(static_cast<std::u32string>(s) == U"あう");
}

BOOST_AUTO_TEST_CASE(basic_atom_set__at__7)
{
	using namespace wordring;

	std::vector<std::u32string> v1{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
	auto as = basic_atom_set<std::u32string>(v1.begin(), v1.end());
	
	auto s = std::u32string(U"え");
	auto a = as.at(s.begin(), s.end());

	BOOST_CHECK(!a);
}

BOOST_AUTO_TEST_CASE(basic_atom_set__at__8)
{
	using namespace wordring;

	std::vector<std::u32string> v1{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
	auto as = basic_atom_set<std::u32string>(v1.begin(), v1.end());

	auto s = std::u32string(U"");
	auto a = as.at(s.begin(), s.end());

	BOOST_CHECK(!a);
}

BOOST_AUTO_TEST_CASE(basic_atom_set__at__9)
{
	using namespace wordring;

	basic_atom_set<std::u32string> as;

	auto s = std::u32string(U"え");
	auto a = as.at(s.begin(), s.end());

	BOOST_CHECK(!a);
}

BOOST_AUTO_TEST_CASE(basic_atom_set__at__10)
{
	using namespace wordring;

	basic_atom_set<std::u32string> as;

	auto s = std::u32string(U"");
	auto a = as.at(s.begin(), s.end());

	BOOST_CHECK(!a);
}

/*
文字列からアトムを返す

value_type at(std::basic_string_view<label_type> sv) const
*/
BOOST_AUTO_TEST_CASE(basic_atom_set__at__11)
{
	using namespace wordring;

	std::vector<std::u32string> v1{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
	auto as = basic_atom_set<std::u32string>(v1.begin(), v1.end());

	auto a = as.at(U"あう");

	BOOST_CHECK(static_cast<std::u32string>(a) == U"あう");
}

BOOST_AUTO_TEST_CASE(basic_atom_set__at__12)
{
	using namespace wordring;

	std::vector<std::u32string> v1{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
	auto as = basic_atom_set<std::u32string>(v1.begin(), v1.end());

	auto a = as.at(U"え");

	BOOST_CHECK(!a);
}

BOOST_AUTO_TEST_CASE(basic_atom_set__at__13)
{
	using namespace wordring;

	std::vector<std::u32string> v1{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
	auto as = basic_atom_set<std::u32string>(v1.begin(), v1.end());

	auto a = as.at(U"");

	BOOST_CHECK(!a);
}

BOOST_AUTO_TEST_CASE(basic_atom_set__at__14)
{
	using namespace wordring;

	basic_atom_set<std::u32string> as;

	auto a = as.at(U"え");

	BOOST_CHECK(!a);
}

BOOST_AUTO_TEST_CASE(basic_atom_set__at__15)
{
	using namespace wordring;

	basic_atom_set<std::u32string> as;

	auto a = as.at(U"");

	BOOST_CHECK(!a);
}

/*
文字列からアトムを返す

value_type operator[](std::basic_string_view<label_type> sv)
*/
BOOST_AUTO_TEST_CASE(basic_atom_set__at__16)
{
	using namespace wordring;

	std::vector<std::u32string> v1{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
	auto as = basic_atom_set<std::u32string>(v1.begin(), v1.end());

	std::u32string s{ U'え', U'\0', U'い' };
	BOOST_CHECK(s.size() == 3);
	auto a = as[s];

	BOOST_CHECK(a);
	BOOST_CHECK(static_cast<std::u32string>(a) == s);
}

BOOST_AUTO_TEST_CASE(basic_atom_set__at__17)
{
	using namespace wordring;

	std::vector<std::u32string> v1{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
	auto as = basic_atom_set<std::u32string>(v1.begin(), v1.end());

	auto a = as[U""];

	BOOST_CHECK(!a);
}

BOOST_AUTO_TEST_CASE(basic_atom_set__at__18)
{
	using namespace wordring;

	basic_atom_set<std::u32string> as;

	auto a = as[U""];

	BOOST_CHECK(!a);
}

// 変更 -----------------------------------------------------------------------

/*
文字列を挿入する

value_type insert(InputIterator first, InputIterator last)
*/
BOOST_AUTO_TEST_CASE(basic_atom_set__insert__1)
{
	using namespace wordring;

	basic_atom_set<std::u32string> as;

	std::u32string s(U"あ");
	auto a = as.insert(s.begin(), s.end());

	BOOST_CHECK(static_cast<std::u32string>(a) == U"あ");
	BOOST_CHECK(as.size() == 1);
}

BOOST_AUTO_TEST_CASE(basic_atom_set__insert__2)
{
	using namespace wordring;

	basic_atom_set<std::u32string> as;

	std::u32string s(U"");
	auto a = as.insert(s.begin(), s.end());

	BOOST_CHECK(!a);
	BOOST_CHECK(as.empty());
}

/*
文字列を挿入する

value_type insert(std::basic_string_view<label_type> sv)
*/
BOOST_AUTO_TEST_CASE(basic_atom_set__insert__3)
{
	using namespace wordring;

	basic_atom_set<std::u32string> as;

	auto a = as.insert(U"あ");

	BOOST_CHECK(static_cast<std::u32string>(a) == U"あ");
	BOOST_CHECK(as.size() == 1);
}

BOOST_AUTO_TEST_CASE(basic_atom_set__insert__4)
{
	using namespace wordring;

	basic_atom_set<std::u32string> as;

	auto a = as.insert(U"");

	BOOST_CHECK(!a);
	BOOST_CHECK(as.empty());
}

/*
アトムを削除する

void erase(std::uint32_t id)
*/
BOOST_AUTO_TEST_CASE(basic_atom_set__erase__1)
{
	using namespace wordring;

	std::vector<std::u32string> v1{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
	auto as = basic_atom_set<std::u32string>(v1.begin(), v1.end());
	auto a = as.insert(U"え");

	as.erase(static_cast<std::uint32_t>(a));

	BOOST_CHECK(as.contains(U"え") == false);
	BOOST_CHECK(as.size() == 5);
}

BOOST_AUTO_TEST_CASE(basic_atom_set__erase__2)
{
	using namespace wordring;

	basic_atom_set<std::u32string> as;
	auto a = as.insert(U"");

	as.erase(static_cast<std::uint32_t>(a));

	BOOST_CHECK(as.contains(U"") == false);
	BOOST_CHECK(as.size() == 0);
}

/*
アトムを削除する

void erase(InputIterator first, InputIterator last)
*/
BOOST_AUTO_TEST_CASE(basic_atom_set__erase__3)
{
	using namespace wordring;

	std::vector<std::u32string> v1{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
	auto as = basic_atom_set<std::u32string>(v1.begin(), v1.end());

	auto s = std::u32string(U"うあい");
	as.erase(s.begin(), s.end());

	BOOST_CHECK(as.contains(U"うあい") == false);
	BOOST_CHECK(as.size() == 4);
}

BOOST_AUTO_TEST_CASE(basic_atom_set__erase__4)
{
	using namespace wordring;

	basic_atom_set<std::u32string> as;

	auto s = std::u32string(U"");
	as.erase(s.begin(), s.end());

	BOOST_CHECK(as.empty());
}

/*
アトムを削除する

void erase(std::basic_string_view<label_type> sv)
*/
BOOST_AUTO_TEST_CASE(basic_atom_set__erase__5)
{
	using namespace wordring;

	std::vector<std::u32string> v1{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
	auto as = basic_atom_set<std::u32string>(v1.begin(), v1.end());

	as.erase(U"うあい");

	BOOST_CHECK(as.contains(U"うあい") == false);
	BOOST_CHECK(as.size() == 4);
}

BOOST_AUTO_TEST_CASE(basic_atom_set__erase__6)
{
	using namespace wordring;

	basic_atom_set<std::u32string> as;

	as.erase(U"");

	BOOST_CHECK(as.empty());
}

BOOST_AUTO_TEST_CASE(basic_atom_set__erase__7)
{
	using namespace wordring;

	std::vector<std::u32string> v1{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
	auto as = basic_atom_set<std::u32string>(v1.begin(), v1.end());

	as.erase(U"え");

	BOOST_CHECK(as.size() == 5);
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
	auto as = basic_atom_set<std::u32string>(v1.begin(), v1.end());

	BOOST_CHECK(as.contains(U"あ"));
	BOOST_CHECK(as.contains(U"あう"));
	BOOST_CHECK(as.contains(U"い"));
	BOOST_CHECK(as.contains(U"うあい"));
	BOOST_CHECK(as.contains(U"うえ"));

	BOOST_CHECK(as.contains(U"え") == false);
}

BOOST_AUTO_TEST_CASE(basic_atom_set__contains__2)
{
	using namespace wordring;

	std::vector<std::u32string> v1{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
	auto as = basic_atom_set<std::u32string>(v1.begin(), v1.end());

	BOOST_CHECK(as.contains(U"") == false);
}

BOOST_AUTO_TEST_CASE(basic_atom_set__contains__3)
{
	using namespace wordring;

	basic_atom_set<std::u32string> as;

	BOOST_CHECK(as.contains(U"") == false);
}

// 直列化 ---------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(basic_atom_set__serialize__1)
{
	using namespace wordring;

	std::vector<std::u32string> v1{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
	auto as1 = basic_atom_set<std::u32string>(v1.begin(), v1.end());

	auto v2 = std::vector<std::uint32_t>(as1.ibegin(), as1.iend());

	auto as2 = basic_atom_set<std::u32string>(v2.begin(), v2.end());

	BOOST_CHECK(as2.size() == 5);
}

BOOST_AUTO_TEST_SUITE_END()
