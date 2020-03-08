// test/trie/stable_trie_iterator.cpp

#include <boost/test/unit_test.hpp>

#include <wordring/trie/trie.hpp>
#include <wordring/trie/trie_iterator.hpp>

#include <memory>
#include <string>

namespace
{
	using wordring::detail::trie_node;

	template <typename Label>
	using test_iterator = typename wordring::stable_trie<Label>::const_iterator;

	template <typename Label>
	using test_trie = wordring::stable_trie<Label>;
}

BOOST_AUTO_TEST_SUITE(stable_trie_iterator__test)

// ----------------------------------------------------------------------------
// const_trie_iterator
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(const_stable_trie_iterator__constrcut__1)
{
	test_iterator<char32_t> it1{}, it2;

	BOOST_CHECK(it1 == it2);
}

BOOST_AUTO_TEST_CASE(const_stable_trie_iterator__constrcut__2)
{
	test_iterator<char16_t> it1{}, it2;

	BOOST_CHECK(it1 == it2);
}

BOOST_AUTO_TEST_CASE(const_stable_trie_iterator__constrcut__3)
{
	test_iterator<char> it1{}, it2;

	BOOST_CHECK(it1 == it2);
}

// value_type operator*() const
BOOST_AUTO_TEST_CASE(const_stable_trie_iterator__reference__1)
{
	std::vector<std::u32string> v{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
	test_trie<char32_t> trie;
	trie.assign(v.begin(), v.end());

	auto it1 = trie.find(std::u32string(U"あ"));
	auto it2 = trie.find(std::u32string(U"あう"));
	auto it3 = trie.find(std::u32string(U"い"));
	auto it4 = trie.find(std::u32string(U"うあい"));
	auto it5 = trie.find(std::u32string(U"うえ"));

	BOOST_CHECK(*it1 == U'あ');
	BOOST_CHECK(*it2 == U'う');
	BOOST_CHECK(*it3 == U'い');
	BOOST_CHECK(*it4 == U'い');
	BOOST_CHECK(*it5 == U'え');
}

BOOST_AUTO_TEST_CASE(const_stable_trie_iterator__reference__2)
{
	std::vector<std::u16string> v{ u"あ", u"あう", u"い", u"うあい", u"うえ" };
	test_trie<char16_t> trie;
	trie.assign(v.begin(), v.end());

	auto it1 = trie.find(std::u16string(u"あ"));
	auto it2 = trie.find(std::u16string(u"あう"));
	auto it3 = trie.find(std::u16string(u"い"));
	auto it4 = trie.find(std::u16string(u"うあい"));
	auto it5 = trie.find(std::u16string(u"うえ"));

	BOOST_CHECK(*it1 == u'あ');
	BOOST_CHECK(*it2 == u'う');
	BOOST_CHECK(*it3 == u'い');
	BOOST_CHECK(*it4 == u'い');
	BOOST_CHECK(*it5 == u'え');
}

BOOST_AUTO_TEST_CASE(const_stable_trie_iterator__reference__3)
{
	std::vector<std::string> v{ u8"あ", u8"あう", u8"い", u8"うあい", u8"うえ" };
	test_trie<char> trie;
	trie.assign(v.begin(), v.end());

	auto it1 = trie.find(std::string(u8"あ"));
	auto it2 = trie.find(std::string(u8"あう"));
	auto it3 = trie.find(std::string(u8"い"));
	auto it4 = trie.find(std::string(u8"うあい"));
	auto it5 = trie.find(std::string(u8"うえ"));

	BOOST_CHECK(*it1 == '\x82');
	BOOST_CHECK(*it2 == '\x86');
	BOOST_CHECK(*it3 == '\x84');
	BOOST_CHECK(*it4 == '\x84');
	BOOST_CHECK(*it5 == '\x88');
}

// const_trie_iterator operator[](value_type label) const
BOOST_AUTO_TEST_CASE(const_stable_trie_iterator__at__1)
{
	std::vector<std::u32string> v{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
	test_trie<char32_t> trie;
	trie.assign(v.begin(), v.end());

	auto it = trie.begin();

	auto it1 = it[U'あ'];
	BOOST_CHECK(*it1 == U'あ');
	auto it2 = it1[U'う'];
	BOOST_CHECK(*it2 == U'う');
	auto it3 = it[U'い'];
	BOOST_CHECK(*it3 == U'い');
	auto it4 = it[U'う'];
	BOOST_CHECK(*it4 == U'う');
	auto it5 = it4[U'あ'];
	BOOST_CHECK(*it5 == U'あ');
	auto it6 = it5[U'い'];
	BOOST_CHECK(*it6 == U'い');
	auto it7 = it4[U'え'];
	BOOST_CHECK(*it7 == U'え');

	BOOST_CHECK(it[U'え'] == it.end());
}

BOOST_AUTO_TEST_CASE(const_stable_trie_iterator__at__2)
{
	std::vector<std::u16string> v{ u"あ", u"あう", u"い", u"うあい", u"うえ" };
	test_trie<char16_t> trie;
	trie.assign(v.begin(), v.end());

	auto it = trie.begin();

	auto it1 = it[u'あ'];
	BOOST_CHECK(*it1 == u'あ');
	auto it2 = it1[u'う'];
	BOOST_CHECK(*it2 == u'う');
	auto it3 = it[u'い'];
	BOOST_CHECK(*it3 == u'い');
	auto it4 = it[u'う'];
	BOOST_CHECK(*it4 == u'う');
	auto it5 = it4[u'あ'];
	BOOST_CHECK(*it5 == u'あ');
	auto it6 = it5[u'い'];
	BOOST_CHECK(*it6 == u'い');
	auto it7 = it4[u'え'];
	BOOST_CHECK(*it7 == u'え');

	BOOST_CHECK(it[u'え'] == it.end());
}

BOOST_AUTO_TEST_CASE(const_stable_trie_iterator__at__3)
{
	std::vector<std::string> v{ u8"あ", u8"あう", u8"い", u8"うあい", u8"うえ" };
	test_trie<char> trie;
	trie.assign(v.begin(), v.end());

	auto it = trie.begin();

	auto it1 = it['\xE3'];
	BOOST_CHECK(*it1 == '\xE3');
	auto it2 = it1['\x81'];
	BOOST_CHECK(*it2 == '\x81');
	auto it3 = it2['\x82'];
	BOOST_CHECK(*it3 == '\x82'); // あ

	auto it4 = it3['\xE3'];
	BOOST_CHECK(*it4 == '\xE3');
	auto it5 = it4['\x81'];
	BOOST_CHECK(*it5 == '\x81');
	auto it6 = it5['\x86'];
	BOOST_CHECK(*it6 == '\x86'); // あう

	BOOST_CHECK(it6[0] == it.end());
}

// const_trie_iterator& operator++()
BOOST_AUTO_TEST_CASE(const_stable_trie_iterator__increment__1)
{
	std::vector<std::u32string> v{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
	test_trie<char32_t> trie;
	trie.assign(v.begin(), v.end());

	auto it = trie.begin();
	auto it1 = it[U'あ'];
	++it1;
	BOOST_CHECK(*it1 == U'い');
	++it1;
	BOOST_CHECK(*it1 == U'う');
	++it1;
	BOOST_CHECK(it1 == it.end());
}

BOOST_AUTO_TEST_CASE(const_stable_trie_iterator__increment__2)
{
	std::vector<std::u16string> v{ u"あ", u"あう", u"い", u"うあい", u"うえ" };
	test_trie<char16_t> trie;
	trie.assign(v.begin(), v.end());

	auto it = trie.begin();
	auto it1 = it[u'あ'];
	++it1;
	BOOST_CHECK(*it1 == u'い');
	++it1;
	BOOST_CHECK(*it1 == u'う');
	++it1;
	BOOST_CHECK(it1 == it.end());
}

BOOST_AUTO_TEST_CASE(const_stable_trie_iterator__increment__3)
{
	std::vector<std::string> v{ "a", "ac", "b", "cab", "cd" };
	test_trie<char> trie;
	trie.assign(v.begin(), v.end());

	auto it = trie.begin();
	auto it1 = it['a'];
	++it1;
	BOOST_CHECK(*it1 == 'b');
	++it1;
	BOOST_CHECK(*it1 == 'c');
	++it1;
	BOOST_CHECK(it1 == it.end());
}

// const_trie_iterator operator++(int)
BOOST_AUTO_TEST_CASE(const_stable_trie_iterator__increment__4)
{
	std::vector<std::u32string> v{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
	test_trie<char32_t> trie;
	trie.assign(v.begin(), v.end());

	auto it = trie.begin();
	auto it1 = it[U'あ'];
	BOOST_CHECK(*it1++ == U'あ');
	BOOST_CHECK(*it1++ == U'い');
	BOOST_CHECK(*it1++ == U'う');
	BOOST_CHECK(it1 == it.end());
}

BOOST_AUTO_TEST_CASE(const_stable_trie_iterator__increment__5)
{
	std::vector<std::u16string> v{ u"あ", u"あう", u"い", u"うあい", u"うえ" };
	test_trie<char16_t> trie;
	trie.assign(v.begin(), v.end());

	auto it = trie.begin();
	auto it1 = it[u'あ'];
	BOOST_CHECK(*it1++ == u'あ');
	BOOST_CHECK(*it1++ == u'い');
	BOOST_CHECK(*it1++ == u'う');
	BOOST_CHECK(it1 == it.end());
}

BOOST_AUTO_TEST_CASE(const_stable_trie_iterator__increment__6)
{
	std::vector<std::string> v{ "a", "ac", "b", "cab", "cd" };
	test_trie<char> trie;
	trie.assign(v.begin(), v.end());

	auto it = trie.begin();
	auto it1 = it['a'];
	BOOST_CHECK(*it1++ == 'a');
	BOOST_CHECK(*it1++ == 'b');
	BOOST_CHECK(*it1++ == 'c');
	BOOST_CHECK(it1 == it.end());
}

// void string(String& result) const
BOOST_AUTO_TEST_CASE(const_stable_trie_iterator__string__1)
{
	std::vector<std::u32string> v{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
	test_trie<char32_t> trie;
	trie.assign(v.begin(), v.end());

	auto it1 = trie.find(std::u32string(U"あ"));
	auto it2 = trie.find(std::u32string(U"あう"));
	auto it3 = trie.find(std::u32string(U"い"));
	auto it4 = trie.find(std::u32string(U"うあい"));
	auto it5 = trie.find(std::u32string(U"うえ"));

	std::u32string s;

	it1.string(s);
	BOOST_CHECK(s == U"あ");
	it2.string(s);
	BOOST_CHECK(s == U"あう");
	it3.string(s);
	BOOST_CHECK(s == U"い");
	it4.string(s);
	BOOST_CHECK(s == U"うあい");
	it5.string(s);
	BOOST_CHECK(s == U"うえ");
}

BOOST_AUTO_TEST_CASE(const_stable_trie_iterator__string__2)
{
	std::vector<std::u16string> v{ u"あ", u"あう", u"い", u"うあい", u"うえ" };
	test_trie<char16_t> trie;
	trie.assign(v.begin(), v.end());

	auto it1 = trie.find(std::u16string(u"あ"));
	auto it2 = trie.find(std::u16string(u"あう"));
	auto it3 = trie.find(std::u16string(u"い"));
	auto it4 = trie.find(std::u16string(u"うあい"));
	auto it5 = trie.find(std::u16string(u"うえ"));

	std::u16string s;

	it1.string(s);
	BOOST_CHECK(s == u"あ");
	it2.string(s);
	BOOST_CHECK(s == u"あう");
	it3.string(s);
	BOOST_CHECK(s == u"い");
	it4.string(s);
	BOOST_CHECK(s == u"うあい");
	it5.string(s);
	BOOST_CHECK(s == u"うえ");
}

BOOST_AUTO_TEST_CASE(const_stable_trie_iterator__string__3)
{
	std::vector<std::string> v{ u8"あ", u8"あう", u8"い", u8"うあい", u8"うえ" };
	test_trie<char> trie;
	trie.assign(v.begin(), v.end());

	auto it1 = trie.find(std::string(u8"あ"));
	auto it2 = trie.find(std::string(u8"あう"));
	auto it3 = trie.find(std::string(u8"い"));
	auto it4 = trie.find(std::string(u8"うあい"));
	auto it5 = trie.find(std::string(u8"うえ"));

	std::string s;

	it1.string(s);
	BOOST_CHECK(s == u8"あ");
	it2.string(s);
	BOOST_CHECK(s == u8"あう");
	it3.string(s);
	BOOST_CHECK(s == u8"い");
	it4.string(s);
	BOOST_CHECK(s == u8"うあい");
	it5.string(s);
	BOOST_CHECK(s == u8"うえ");
}

// const_trie_iterator parent() const
BOOST_AUTO_TEST_CASE(const_stable_trie_iterator__parent__1)
{
	std::vector<std::u32string> v{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
	test_trie<char32_t> trie;
	trie.assign(v.begin(), v.end());

	auto it1 = trie.find(std::u32string(U"あ"));
	auto it2 = trie.find(std::u32string(U"あう"));
	auto it3 = trie.find(std::u32string(U"い"));
	auto it4 = trie.find(std::u32string(U"うあい"));
	auto it5 = trie.find(std::u32string(U"うえ"));

	BOOST_CHECK(it1.parent() == trie.begin());
	BOOST_CHECK(*it2.parent() == U'あ');
	BOOST_CHECK(it3.parent() == trie.begin());
	BOOST_CHECK(*it4.parent() == U'あ');
	BOOST_CHECK(*it5.parent() == U'う');
}

BOOST_AUTO_TEST_CASE(const_stable_trie_iterator__parent__2)
{
	std::vector<std::u16string> v{ u"あ", u"あう", u"い", u"うあい", u"うえ" };
	test_trie<char16_t> trie;
	trie.assign(v.begin(), v.end());

	auto it1 = trie.find(std::u16string(u"あ"));
	auto it2 = trie.find(std::u16string(u"あう"));
	auto it3 = trie.find(std::u16string(u"い"));
	auto it4 = trie.find(std::u16string(u"うあい"));
	auto it5 = trie.find(std::u16string(u"うえ"));

	BOOST_CHECK(it1.parent() == trie.begin());
	BOOST_CHECK(*it2.parent() == u'あ');
	BOOST_CHECK(it3.parent() == trie.begin());
	BOOST_CHECK(*it4.parent() == u'あ');
	BOOST_CHECK(*it5.parent() == u'う');
}

BOOST_AUTO_TEST_CASE(const_stable_trie_iterator__parent__3)
{
	std::vector<std::string> v{ "a", "ac", "b", "cab", "cd" };
	test_trie<char> trie;
	trie.assign(v.begin(), v.end());

	auto it1 = trie.find(std::string("a"));
	auto it2 = trie.find(std::string("ac"));
	auto it3 = trie.find(std::string("b"));
	auto it4 = trie.find(std::string("cab"));
	auto it5 = trie.find(std::string("cd"));

	BOOST_CHECK(it1.parent() == trie.begin());
	BOOST_CHECK(*it2.parent() == 'a');
	BOOST_CHECK(it3.parent() == trie.begin());
	BOOST_CHECK(*it4.parent() == 'a');
	BOOST_CHECK(*it5.parent() == 'c');
}

// const_trie_iterator begin() const
BOOST_AUTO_TEST_CASE(const_stable_trie_iterator__begin__1)
{
	std::vector<std::u32string> v{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
	test_trie<char32_t> trie;
	trie.assign(v.begin(), v.end());

	auto it = trie.begin();

	auto it1 = it.begin();
	BOOST_CHECK(*it1 == U'あ');
	auto it2 = it1.begin();
	BOOST_CHECK(*it2 == U'う');
}

BOOST_AUTO_TEST_CASE(const_stable_trie_iterator__begin__2)
{
	std::vector<std::u16string> v{ u"あ", u"あう", u"い", u"うあい", u"うえ" };
	test_trie<char16_t> trie;
	trie.assign(v.begin(), v.end());

	auto it = trie.begin();

	auto it1 = it.begin();
	BOOST_CHECK(*it1 == u'あ');
	auto it2 = it1.begin();
	BOOST_CHECK(*it2 == u'う');
}

BOOST_AUTO_TEST_CASE(const_stable_trie_iterator__begin__3)
{
	std::vector<std::string> v{ "a", "ac", "b", "cab", "cd" };
	test_trie<char> trie;
	trie.assign(v.begin(), v.end());

	auto it = trie.begin();

	auto it1 = it.begin();
	BOOST_CHECK(*it1 == 'a');
	auto it2 = it1.begin();
	BOOST_CHECK(*it2 == 'c');
}

// const_trie_iterator end() const
BOOST_AUTO_TEST_CASE(const_stable_trie_iterator__end__1)
{
	test_trie<char32_t> trie;
	BOOST_CHECK(trie.begin() != trie.end());
}

BOOST_AUTO_TEST_CASE(const_stable_trie_iterator__end__2)
{
	test_trie<char16_t> trie;
	BOOST_CHECK(trie.begin() != trie.end());
}

BOOST_AUTO_TEST_CASE(const_stable_trie_iterator__end__3)
{
	test_trie<char> trie;
	BOOST_CHECK(trie.begin() != trie.end());
}

// inline bool operator==(const_trie_iterator<Label1, Base1> const& lhs, const_trie_iterator<Label1, Base1> const& rhs)
BOOST_AUTO_TEST_CASE(const_stable_trie_iterator__equal__1)
{
	std::vector<std::u32string> v{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
	test_trie<char32_t> trie;
	trie.assign(v.begin(), v.end());

	auto it = trie.begin();
	auto it1 = it[U'え'];
	BOOST_CHECK(it1 == it.end());
}

BOOST_AUTO_TEST_CASE(const_stable_trie_iterator__equal__2)
{
	std::vector<std::u16string> v{ u"あ", u"あう", u"い", u"うあい", u"うえ" };
	test_trie<char16_t> trie;
	trie.assign(v.begin(), v.end());

	auto it = trie.begin();
	auto it1 = it[u'え'];
	BOOST_CHECK(it1 == it.end());
}

BOOST_AUTO_TEST_CASE(const_stable_trie_iterator__equal__3)
{
	std::vector<std::string> v{ "a", "ac", "b", "cab", "cd" };
	test_trie<char> trie;
	trie.assign(v.begin(), v.end());

	auto it = trie.begin();
	auto it1 = it['d'];
	BOOST_CHECK(it1 == it.end());
}

// inline bool operator!=(const_trie_iterator<Label1, Base1> const& lhs, const_trie_iterator<Label1, Base1> const& rhs)
BOOST_AUTO_TEST_CASE(const_stable_trie_iterator__not_equal__1)
{
	test_trie<char32_t> trie;
	BOOST_CHECK(trie.begin() != trie.end());
}

BOOST_AUTO_TEST_CASE(const_stable_trie_iterator__not_equal__2)
{
	test_trie<char16_t> trie;
	BOOST_CHECK(trie.begin() != trie.end());
}

BOOST_AUTO_TEST_CASE(const_stable_trie_iterator__not_equal__3)
{
	test_trie<char> trie;
	BOOST_CHECK(trie.begin() != trie.end());
}

BOOST_AUTO_TEST_SUITE_END()
