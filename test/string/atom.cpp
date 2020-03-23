// test/string/atom.cpp

#include <boost/test/unit_test.hpp>

#include <wordring/string/atom.hpp>

#include <algorithm>
#include <iterator>
#include <iomanip>
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
初期化子リストから構築する

basic_atom_set(std::initializer_list<detail::trie_node> il, allocator_type const& alloc = allocator_type())
*/
BOOST_AUTO_TEST_CASE(basic_atom_set__construct__5)
{
	using namespace wordring;

	/*
	std::vector<std::u32string> v{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
	basic_atom_set<std::u32string> as1(v.begin(), v.end());

	auto it1 = as1.ibegin();
	auto it2 = as1.iend();
	for (std::uint32_t i= 0; it1 != it2; ++i)
	{
		std::int32_t j = *it1++;
		if (i % 2 == 0) std::cout << "{ " << j << ", ";
		else std::cout << j << " }, ";
		if (1 < i && (i - 1) % 16 == 0) std::cout << std::endl;
	}
	*/
	basic_atom_set<std::u32string> as2 = {
		{ 5, -10 }, { 2, 0 }, { 3, 1 }, { 1, 2 }, { 5, 68 }, { 2, 4 }, { 7, 72 }, { 3, 6 }, { 9, 67 },
		{ 4, 8 }, { 0, -11 }, { 0, -12 }, { 0, -13 }, { 0, -14 }, { 0, -15 }, { 0, -16 }, { 0, -17 },
		{ 0, -18 }, { 0, -19 }, { 0, -20 }, { 0, -21 }, { 0, -22 }, { 0, -23 }, { 0, -24 }, { 0, -25 },
		{ 0, -26 }, { 0, -27 }, { 0, -28 }, { 0, -29 }, { 0, -30 }, { 0, -31 }, { 0, -32 }, { 0, -33 },
		{ 0, -34 }, { 0, -35 }, { 0, -36 }, { 0, -37 }, { 0, -38 }, { 0, -39 }, { 0, -40 }, { 0, -41 },
		{ 0, -42 }, { 0, -43 }, { 0, -44 }, { 0, -45 }, { 0, -46 }, { 0, -47 }, { 0, -48 }, { 0, -53 },
		{ 2, 3 }, { 1, 5 }, { 1, 7 }, { 1, 9 }, { 0, -54 }, { 0, -55 }, { 0, -56 }, { 0, -57 },
		{ 0, -58 }, { 0, -59 }, { 0, -60 }, { 0, -61 }, { 0, -62 }, { 0, -63 }, { 0, -64 }, { 0, -65 },
		{ 0, -66 }, { 0, -74 }, { 8, 51 }, { 4, 49 }, { 3, 52 }, { 2, 49 }, { 1, 50 }, { 6, 49 },
		{ 5, 51 }, { 0, -75 }, { 0, -76 }, { 0, -77 }, { 0, -78 }, { 0, -79 }, { 0, -80 }, { 0, -81 },
		{ 0, -82 }, { 0, -83 }, { 0, -84 }, { 0, -85 }, { 0, -86 }, { 0, -87 }, { 0, -88 }, { 0, -89 },
		{ 0, -90 }, { 0, -91 }, { 0, -92 }, { 0, -93 }, { 0, -94 }, { 0, -95 }, { 0, -96 }, { 0, -97 },
		{ 0, -98 }, { 0, -99 }, { 0, -100 }, { 0, -101 }, { 0, -102 }, { 0, -103 }, { 0, -104 }, { 0, -105 },
		{ 0, -106 }, { 0, -107 }, { 0, -108 }, { 0, -109 }, { 0, -110 }, { 0, -111 }, { 0, -112 }, { 0, -113 },
		{ 0, -114 }, { 0, -115 }, { 0, -116 }, { 0, -117 }, { 0, -118 }, { 0, -119 }, { 0, -120 }, { 0, -121 },
		{ 0, -122 }, { 0, -123 }, { 0, -124 }, { 0, -125 }, { 0, -126 }, { 0, -127 }, { 0, -128 }, { 0, -129 },
		{ 0, -130 }, { 0, -131 }, { 0, -132 }, { 0, -133 }, { 0, -134 }, { 0, -135 }, { 0, -136 }, { 0, -137 },
		{ 0, -138 }, { 0, -139 }, { 0, -140 }, { 0, -141 }, { 0, -142 }, { 0, -143 }, { 0, -144 }, { 0, -145 },
		{ 0, -146 }, { 0, -147 }, { 0, -148 }, { 0, -149 }, { 0, -150 }, { 0, -151 }, { 0, -152 }, { 0, -153 },
		{ 0, -154 }, { 0, -155 }, { 0, -156 }, { 0, -157 }, { 0, -158 }, { 0, -159 }, { 0, -160 }, { 0, -161 },
		{ 0, -162 }, { 0, -163 }, { 0, -164 }, { 0, -165 }, { 0, -166 }, { 0, -167 }, { 0, -168 }, { 0, -169 },
		{ 0, -170 }, { 0, -171 }, { 0, -172 }, { 0, -173 }, { 0, -174 }, { 0, -175 }, { 0, -176 }, { 0, -177 },
		{ 0, -178 }, { 0, -179 }, { 0, -180 }, { 0, -181 }, { 0, -182 }, { 0, -183 }, { 0, -184 }, { 0, -185 },
		{ 0, -186 }, { 0, -187 }, { 0, -188 }, { 0, -189 }, { 0, -190 }, { 0, -191 }, { 0, -192 }, { 0, -193 },
		{ 0, -194 }, { 0, -195 }, { 0, -196 }, { 0, -197 }, { 0, -198 }, { 0, -199 }, { 0, -200 }, { 0, -201 },
		{ 0, -202 }, { 0, -203 }, { 0, -204 }, { 0, -205 }, { 0, -206 }, { 0, -207 }, { 0, -208 }, { 0, -209 },
		{ 0, -210 }, { 0, -211 }, { 0, -212 }, { 0, -213 }, { 0, -214 }, { 0, -215 }, { 0, -216 }, { 0, -217 },
		{ 0, -218 }, { 0, -219 }, { 0, -220 }, { 0, -221 }, { 0, -222 }, { 0, -223 }, { 0, -224 }, { 0, -225 },
		{ 0, -226 }, { 0, -227 }, { 0, -228 }, { 0, -229 }, { 0, -230 }, { 0, -231 }, { 0, -232 }, { 0, -233 },
		{ 0, -234 }, { 0, -235 }, { 0, -236 }, { 0, -237 }, { 0, -238 }, { 0, -239 }, { 0, -240 }, { 0, -241 },
		{ 0, -242 }, { 0, -243 }, { 0, -244 }, { 0, -245 }, { 0, -246 }, { 0, -247 }, { 0, -248 }, { 0, -249 },
		{ 0, -250 }, { 0, -251 }, { 0, -252 }, { 0, -253 }, { 0, -254 }, { 0, -255 }, { 0, -256 }, { 0, 0 },
		{ -257, 71 }, { -258, 70 }, { -259, 69 }, { -257, 68 }, { -261, 73 }
	};

	BOOST_CHECK(as2.contains(std::u32string(U"あ")));
	BOOST_CHECK(as2.contains(std::u32string(U"あう")));
	BOOST_CHECK(as2.contains(std::u32string(U"い")));
	BOOST_CHECK(as2.contains(std::u32string(U"うあい")));
	BOOST_CHECK(as2.contains(std::u32string(U"うえ")));
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
