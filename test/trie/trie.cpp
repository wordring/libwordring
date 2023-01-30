// test/trie/trie.cpp

#include <boost/test/unit_test.hpp>

#include <wordring/tree/tree_iterator.hpp>
#include <wordring/trie/trie.hpp>

#include <wordring/whatwg/infra/unicode.hpp>

#include <algorithm>
#include <chrono>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <random>
#include <sstream>
#include <string>
#include <vector>

#define STRING(str) #str
#define TO_STRING(str) STRING(str)

namespace
{
	using wordring::detail::trie_node;

	std::string const japanese_words_path{ TO_STRING(JAPANESE_WORDS_PATH) };

	template <typename Char>
	class test_trie : public wordring::trie<Char>
	{
	public:
		using base_type = wordring::trie<Char>;

		using base_type::begin;

		using base_type::m_c;

	public:
		/*! 保持する文字列の数を数える
		- size()と同じ数を返せば良好。
		*/
		std::size_t count() const
		{
			using namespace wordring;
			std::size_t n = 0;

			auto it1 = tree_iterator<decltype(begin())>(begin());
			auto it2 = tree_iterator<decltype(begin())>();

			while (it1 != it2)
			{
				if (it1.base()) ++n;
				++it1;
			}

			return n;
		}
	};
}

BOOST_AUTO_TEST_SUITE(trie_test)

// basic_trie()
BOOST_AUTO_TEST_CASE(trie_construct_1)
{
	using namespace wordring;
	auto t1 = trie<char32_t>();
}

BOOST_AUTO_TEST_CASE(trie_construct_2)
{
	using namespace wordring;
	trie<char16_t> t1;
}

BOOST_AUTO_TEST_CASE(trie_construct_3)
{
	using namespace wordring;
	trie<char> t1;
}

// explicit basic_trie(allocator_type const& alloc)
BOOST_AUTO_TEST_CASE(trie_construct_4)
{
	using namespace wordring;
	auto t = trie<char32_t>(std::allocator<trie_node>());
}

BOOST_AUTO_TEST_CASE(trie_construct_5)
{
	using namespace wordring;
	auto t = trie<char16_t>(std::allocator<trie_node>());
}

BOOST_AUTO_TEST_CASE(trie_construct_6)
{
	using namespace wordring;
	auto t = trie<char>(std::allocator<trie_node>());
}

// basic_trie(InputIterator first, InputIterator last, allocator_type const& alloc = allocator_type())
BOOST_AUTO_TEST_CASE(trie_construct_7)
{
	using namespace wordring;
	std::vector<std::u32string> v1{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
	trie<char32_t> t1{ v1.begin(), v1.end() };

	std::vector<std::int32_t> v2{ t1.ibegin(), t1.iend() };
	trie<char32_t> t2{ v2.begin(), v2.end(), std::allocator<trie_node>() };
}

BOOST_AUTO_TEST_CASE(trie_construct_8)
{
	using namespace wordring;
	std::vector<std::u16string> v1{ u"あ", u"あう", u"い", u"うあい", u"うえ" };
	trie<char16_t> t1{ v1.begin(), v1.end() };

	std::vector<std::int32_t> v2{ t1.ibegin(), t1.iend() };
	trie<char16_t> t2{ v2.begin(), v2.end(), std::allocator<trie_node>() };
}

BOOST_AUTO_TEST_CASE(trie_construct_9)
{
	using namespace wordring;
	std::vector<std::string> v1{ "a", "ac", "b", "cab", "cd" };
	trie<char> t1{ v1.begin(), v1.end() };

	std::vector<std::int32_t> v2{ t1.ibegin(), t1.iend() };
	trie<char> t2{ v2.begin(), v2.end(), std::allocator<trie_node>() };
}

// basic_trie(ForwardIterator first, ForwardIterator last, allocator_type const& alloc = allocator_type())
BOOST_AUTO_TEST_CASE(trie_construct_10)
{
	using namespace wordring;
	std::vector<std::u32string> v1{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
	trie<char32_t> t1{ v1.begin(), v1.end() };
}

BOOST_AUTO_TEST_CASE(trie_construct_11)
{
	using namespace wordring;
	std::vector<std::u16string> v1{ u"あ", u"あう", u"い", u"うあい", u"うえ" };
	trie<char16_t> t1{ v1.begin(), v1.end() };
}

BOOST_AUTO_TEST_CASE(trie_construct_12)
{
	using namespace wordring;
	std::vector<std::string> v1{ "a", "ac", "b", "cab", "cd" };
	trie<char> t1{ v1.begin(), v1.end() };
}

// basic_trie(std::initializer_list<trie_node> il, allocator_type const& alloc = allocator_type())
BOOST_AUTO_TEST_CASE(trie_construct_13)
{
	using namespace wordring;
	trie<char32_t> trie{ { { 0, 1 }, { 2, 3 } } };
}

BOOST_AUTO_TEST_CASE(trie_construct_14)
{
	using namespace wordring;
	trie<char16_t> trie{ { { 0, 1 }, { 2, 3 } } };
}

BOOST_AUTO_TEST_CASE(trie_construct_15)
{
	using namespace wordring;
	trie<char> trie{ { { 0, 1 }, { 2, 3 } } };
}

// void assign(InputIterator first, InputIterator last)
BOOST_AUTO_TEST_CASE(trie_assign_1)
{
	std::vector<std::u32string> v1{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
	test_trie<char32_t> t1;
	t1.assign(v1.begin(), v1.end());

	auto v2 = std::vector<std::int32_t>(t1.ibegin(), t1.iend());

	test_trie<char32_t> t2;
	t2.assign(v2.begin(), v2.end());

	BOOST_CHECK(t2.size() == 5);
	BOOST_CHECK(t2.size() == t2.count());
	BOOST_CHECK(t2.contains(std::u32string(U"あ")));
	BOOST_CHECK(t2.contains(std::u32string(U"あう")));
	BOOST_CHECK(t2.contains(std::u32string(U"い")));
	BOOST_CHECK(t2.contains(std::u32string(U"うあい")));
	BOOST_CHECK(t2.contains(std::u32string(U"うえ")));
}

BOOST_AUTO_TEST_CASE(trie_assign_2)
{
	std::vector<std::u16string> v1{ u"あ", u"あう", u"い", u"うあい", u"うえ" };
	test_trie<char16_t> t1;
	t1.assign(v1.begin(), v1.end());

	auto v2 = std::vector<std::int32_t>(t1.ibegin(), t1.iend());

	test_trie<char16_t> t2;
	t2.assign(v2.begin(), v2.end());

	BOOST_CHECK(t2.size() == 5);
	BOOST_CHECK(t2.size() == t2.count());
	BOOST_CHECK(t2.contains(std::u16string(u"あ")));
	BOOST_CHECK(t2.contains(std::u16string(u"あう")));
	BOOST_CHECK(t2.contains(std::u16string(u"い")));
	BOOST_CHECK(t2.contains(std::u16string(u"うあい")));
	BOOST_CHECK(t2.contains(std::u16string(u"うえ")));
}

BOOST_AUTO_TEST_CASE(trie_assign_3)
{
	std::vector<std::string> v1{ "a", "ac", "b", "cab", "cd" };
	test_trie<char> t1;
	t1.assign(v1.begin(), v1.end());

	auto v2 = std::vector<std::int32_t>(t1.ibegin(), t1.iend());

	test_trie<char> t2;
	t2.assign(v2.begin(), v2.end());

	BOOST_CHECK(t2.count() == 5);
	BOOST_CHECK(t2.size() == 5);
	BOOST_CHECK(t2.contains(std::string("a")));
	BOOST_CHECK(t2.contains(std::string("ac")));
	BOOST_CHECK(t2.contains(std::string("b")));
	BOOST_CHECK(t2.contains(std::string("cab")));
	BOOST_CHECK(t2.contains(std::string("cd")));
}

BOOST_AUTO_TEST_CASE(trie_assign_4)
{
	std::vector<std::u32string> v1{};
	test_trie<char32_t> t1;
	t1.assign(v1.begin(), v1.end());

	auto v2 = std::vector<std::int32_t>(t1.ibegin(), t1.iend());

	test_trie<char32_t> t2;
	t2.assign(v2.begin(), v2.end());

	BOOST_CHECK(t2.size() == 0);
	BOOST_CHECK(t2.size() == t2.count());
}

BOOST_AUTO_TEST_CASE(trie_assign_5)
{
	std::vector<std::u16string> v1{};
	test_trie<char16_t> t1;
	t1.assign(v1.begin(), v1.end());

	auto v2 = std::vector<std::int32_t>(t1.ibegin(), t1.iend());

	test_trie<char16_t> t2;
	t2.assign(v2.begin(), v2.end());

	BOOST_CHECK(t2.size() == 0);
	BOOST_CHECK(t2.size() == t2.count());
}

BOOST_AUTO_TEST_CASE(trie_assign_6)
{
	std::vector<std::string> v1{};
	test_trie<char> t1;
	t1.assign(v1.begin(), v1.end());

	auto v2 = std::vector<std::int32_t>(t1.ibegin(), t1.iend());

	test_trie<char> t2;
	t2.assign(v2.begin(), v2.end());

	BOOST_CHECK(t2.size() == 0);
	BOOST_CHECK(t2.size() == t2.count());
}

// void assign(ForwardIterator first, ForwardIterator last)
BOOST_AUTO_TEST_CASE(trie_assign_7)
{
	std::vector<std::u32string> v{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
	test_trie<char32_t> trie;
	trie.assign(v.begin(), v.end());

	BOOST_CHECK(trie.size() == 5);
	BOOST_CHECK(trie.size() == trie.count());
	BOOST_CHECK(trie.contains(std::u32string(U"あ")));
	BOOST_CHECK(trie.contains(std::u32string(U"あう")));
	BOOST_CHECK(trie.contains(std::u32string(U"い")));
	BOOST_CHECK(trie.contains(std::u32string(U"うあい")));
	BOOST_CHECK(trie.contains(std::u32string(U"うえ")));
}

BOOST_AUTO_TEST_CASE(trie_assign_8)
{
	std::vector<std::u16string> v{ u"あ", u"あう", u"い", u"うあい", u"うえ" };
	test_trie<char16_t> trie;
	trie.assign(v.begin(), v.end());

	BOOST_CHECK(trie.size() == 5);
	BOOST_CHECK(trie.size() == trie.count());
	BOOST_CHECK(trie.contains(std::u16string(u"あ")));
	BOOST_CHECK(trie.contains(std::u16string(u"あう")));
	BOOST_CHECK(trie.contains(std::u16string(u"い")));
	BOOST_CHECK(trie.contains(std::u16string(u"うあい")));
	BOOST_CHECK(trie.contains(std::u16string(u"うえ")));
}

BOOST_AUTO_TEST_CASE(trie_assign_9)
{
	std::vector<std::string> v{ "a", "ac", "b", "cab", "cd" };
	test_trie<char> trie;
	trie.assign(v.begin(), v.end());

	BOOST_CHECK(trie.size() == 5);
	BOOST_CHECK(trie.size() == trie.count());
	BOOST_CHECK(trie.contains(std::string("a")));
	BOOST_CHECK(trie.contains(std::string("ac")));
	BOOST_CHECK(trie.contains(std::string("b")));
	BOOST_CHECK(trie.contains(std::string("cab")));
	BOOST_CHECK(trie.contains(std::string("cd")));
}

// 要素アクセス ----------------------------------------------------------------

// reference at(const_iterator pos)
// reference at(InputIterator first, InputIterator last)
// reference at(Key const& key)
BOOST_AUTO_TEST_CASE(trie_at_1)
{
	std::vector<std::u32string> v{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
	test_trie<char32_t> trie;
	trie.assign(v.begin(), v.end());

	auto val = trie.at(std::u32string(U"あ"));
	val = 100;

	BOOST_CHECK(trie.at(std::u32string(U"あ")) == 100);
}

BOOST_AUTO_TEST_CASE(trie_at_2)
{
	std::vector<std::u32string> v{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
	test_trie<char32_t> trie;
	trie.assign(v.begin(), v.end());

	auto val = trie.at(std::u32string(U"あう"));
	val = 100;

	BOOST_CHECK(trie.at(std::u32string(U"あう")) == 100);
}

BOOST_AUTO_TEST_CASE(trie_at_3)
{
	std::vector<std::u32string> v{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
	test_trie<char32_t> trie;
	trie.assign(v.begin(), v.end());

	BOOST_CHECK_THROW(trie.at(std::u32string(U"え")), std::out_of_range);
}

BOOST_AUTO_TEST_CASE(trie_at_4)
{
	std::vector<std::u16string> v{ u"あ", u"あう", u"い", u"うあい", u"うえ" };
	test_trie<char16_t> trie;
	trie.assign(v.begin(), v.end());

	auto val = trie.at(std::u16string(u"い"));
	val = 100;

	BOOST_CHECK(trie.at(std::u16string(u"い")) == 100);
}

BOOST_AUTO_TEST_CASE(trie_at_5)
{
	std::vector<std::u16string> v{ u"あ", u"あう", u"い", u"うあい", u"うえ" };
	test_trie<char16_t> trie;
	trie.assign(v.begin(), v.end());

	BOOST_CHECK_THROW(trie.at(std::u16string(u"え")), std::out_of_range);
}

BOOST_AUTO_TEST_CASE(trie_at_6)
{
	std::vector<std::string> v{ "a", "ac", "b", "cab", "cd" };
	test_trie<char> trie;
	trie.assign(v.begin(), v.end());

	auto val = trie.at(std::string("cd"));
	val = 100;

	BOOST_CHECK(trie.at(std::string("cd")) == 100);
}

BOOST_AUTO_TEST_CASE(trie_at_7)
{
	std::vector<std::string> v{ "a", "ac", "b", "cab", "cd" };
	test_trie<char> trie;
	trie.assign(v.begin(), v.end());

	BOOST_CHECK_THROW(trie.at(std::string("d")), std::out_of_range);
}

// const_reference at(const_iterator pos) const
// const_reference at(InputIterator first, InputIterator last) const
// const_reference const at(Key const& key) const
BOOST_AUTO_TEST_CASE(trie_at_8)
{
	std::vector<std::u32string> v{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
	test_trie<char32_t> trie;
	trie.assign(v.begin(), v.end());

	trie.at(std::u32string(U"あ")) = 100;
	trie.at(std::u32string(U"うえ")) = 1000;

	test_trie<char32_t> const& t = trie;

	BOOST_CHECK(t.at(std::u32string(U"あ")) == 100);
	BOOST_CHECK(t.at(std::u32string(U"うえ")) == 1000);
}

// reference operator[](Key const& key)
BOOST_AUTO_TEST_CASE(trie_at_9)
{
	std::vector<std::u32string> v{};
	test_trie<char32_t> trie;
	trie.assign(v.begin(), v.end());

	trie[std::u32string(U"あ")] = 100;
	trie[std::u32string(U"うえ")] = 1000;

	BOOST_CHECK(trie.at(std::u32string(U"あ")) == 100);
	BOOST_CHECK(trie.at(std::u32string(U"うえ")) == 1000);
}

BOOST_AUTO_TEST_CASE(trie_at_10)
{
	std::vector<std::u16string> v{};
	test_trie<char16_t> trie;
	trie.assign(v.begin(), v.end());

	trie[std::u16string(u"あ")] = 100;
	trie[std::u16string(u"うえ")] = 1000;

	BOOST_CHECK(trie.at(std::u16string(u"あ")) == 100);
	BOOST_CHECK(trie.at(std::u16string(u"うえ")) == 1000);
}

BOOST_AUTO_TEST_CASE(trie_at_11)
{
	std::vector<std::string> v{};
	test_trie<char> trie;
	trie.assign(v.begin(), v.end());

	trie[std::string("a")] = 100;
	trie[std::string("ac")] = 1000;

	BOOST_CHECK(trie.at(std::string("a")) == 100);
	BOOST_CHECK(trie.at(std::string("ac")) == 1000);
}

// イテレータ ------------------------------------------------------------------

// const_iterator begin() const noexcept
BOOST_AUTO_TEST_CASE(trie_begin_1)
{
	test_trie<char32_t> trie{};
	BOOST_CHECK(trie.begin() != trie.end());
}

BOOST_AUTO_TEST_CASE(trie_begin_2)
{
	test_trie<char16_t> trie{};
	BOOST_CHECK(trie.begin() != trie.end());
}

BOOST_AUTO_TEST_CASE(trie_begin_3)
{
	test_trie<char> trie{};
	BOOST_CHECK(trie.begin() != trie.end());
}

// const_iterator cbegin() const noexcept
BOOST_AUTO_TEST_CASE(trie_cbegin_1)
{
	test_trie<char32_t> trie{};
	BOOST_CHECK(trie.cbegin() != trie.cend());
}

BOOST_AUTO_TEST_CASE(trie_cbegin_2)
{
	test_trie<char16_t> trie{};
	BOOST_CHECK(trie.cbegin() != trie.cend());
}

BOOST_AUTO_TEST_CASE(trie_cbegin_3)
{
	test_trie<char> trie{};
	BOOST_CHECK(trie.cbegin() != trie.cend());
}

// const_iterator end() const noexcept
BOOST_AUTO_TEST_CASE(trie_end_1)
{
	test_trie<char32_t> trie{};
	BOOST_CHECK(trie.begin() != trie.end());
}

BOOST_AUTO_TEST_CASE(trie_end_2)
{
	test_trie<char16_t> trie{};
	BOOST_CHECK(trie.begin() != trie.end());
}

BOOST_AUTO_TEST_CASE(trie_end_3)
{
	test_trie<char> trie{};
	BOOST_CHECK(trie.begin() != trie.end());
}

// const_iterator cend() const noexcept
BOOST_AUTO_TEST_CASE(trie_cend_1)
{
	test_trie<char32_t> trie{};
	BOOST_CHECK(trie.cbegin() != trie.cend());
}

BOOST_AUTO_TEST_CASE(trie_cend_2)
{
	test_trie<char16_t> trie{};
	BOOST_CHECK(trie.cbegin() != trie.cend());
}

BOOST_AUTO_TEST_CASE(trie_cend_3)
{
	test_trie<char> trie{};
	BOOST_CHECK(trie.cbegin() != trie.cend());
}

// 容量 -----------------------------------------------------------------------

// bool empty() const noexcept
BOOST_AUTO_TEST_CASE(trie_empty_1)
{
	test_trie<char32_t> trie{};
	BOOST_CHECK(trie.empty());
}

BOOST_AUTO_TEST_CASE(trie_empty_2)
{
	test_trie<char16_t> trie{};
	BOOST_CHECK(trie.empty());
}

BOOST_AUTO_TEST_CASE(trie_empty_3)
{
	test_trie<char> trie{};
	BOOST_CHECK(trie.empty());
}

BOOST_AUTO_TEST_CASE(trie_empty_4)
{
	test_trie<char32_t> trie{};
	trie.insert(std::u32string(U"abc"));
	BOOST_CHECK(trie.empty() == false);
}

BOOST_AUTO_TEST_CASE(trie_empty_5)
{
	test_trie<char16_t> trie{};
	trie.insert(std::u16string(u"abc"));
	BOOST_CHECK(trie.empty() == false);
}

BOOST_AUTO_TEST_CASE(trie_empty_6)
{
	test_trie<char> trie{};
	trie.insert(std::string("abc"));
	BOOST_CHECK(trie.empty() == false);
}

// size_type size() const noexcept
BOOST_AUTO_TEST_CASE(trie_size_1)
{
	test_trie<char32_t> trie{};
	BOOST_CHECK(trie.size() == 0);
}

BOOST_AUTO_TEST_CASE(trie_size_2)
{
	test_trie<char16_t> trie{};
	BOOST_CHECK(trie.size() == 0);
}

BOOST_AUTO_TEST_CASE(trie_size_3)
{
	test_trie<char> trie{};
	BOOST_CHECK(trie.size() == 0);
}

BOOST_AUTO_TEST_CASE(trie_size_4)
{
	test_trie<char32_t> trie{};
	trie.insert(std::u32string(U"abc"));
	BOOST_CHECK(trie.size() == 1);
	trie.insert(std::u32string(U"abc"));
	BOOST_CHECK(trie.size() == 1);
}

BOOST_AUTO_TEST_CASE(trie_size_5)
{
	test_trie<char16_t> trie{};
	trie.insert(std::u16string(u"abc"));
	BOOST_CHECK(trie.size() == 1);
	trie.insert(std::u16string(u"abc"));
	BOOST_CHECK(trie.size() == 1);
}

BOOST_AUTO_TEST_CASE(trie_size_6)
{
	test_trie<char> trie{};
	trie.insert(std::string("abc"));
	BOOST_CHECK(trie.size() == 1);
	trie.insert(std::string("abc"));
	BOOST_CHECK(trie.size() == 1);
}

// size_type max_size() const noexcept
BOOST_AUTO_TEST_CASE(trie_max_size_1)
{
	test_trie<char32_t> trie{};
	BOOST_CHECK(trie.max_size());
}

BOOST_AUTO_TEST_CASE(trie_max_size_2)
{
	test_trie<char16_t> trie{};
	BOOST_CHECK(trie.max_size());
}

BOOST_AUTO_TEST_CASE(trie_max_size_3)
{
	test_trie<char> trie{};
	BOOST_CHECK(trie.max_size());
}

// 変更 -----------------------------------------------------------------------

// const_iterator insert(InputIterator first, InputIterator last, value_type value = 0)
BOOST_AUTO_TEST_CASE(trie_insert_1)
{
	test_trie<char32_t> trie;

	auto it = trie.insert(std::u32string(U""));

	BOOST_CHECK(it == trie.end());
	BOOST_CHECK(trie.size() == 0);
	BOOST_CHECK(trie.size() == trie.count());
	BOOST_CHECK(trie.contains(std::u32string(1, U'\0')) == false);
}

BOOST_AUTO_TEST_CASE(trie_insert_2)
{
	test_trie<char32_t> trie;

	auto it = trie.insert(std::u32string(1, U'\0'));

	BOOST_CHECK(it == true);
	BOOST_CHECK(trie.size() == 1);
	BOOST_CHECK(trie.size() == trie.count());
	BOOST_CHECK(trie.contains(std::u32string(1, U'\0')));
}

BOOST_AUTO_TEST_CASE(trie_insert_3)
{
	test_trie<char32_t> trie;

	auto it = trie.insert(std::u32string(2, U'\0'));

	BOOST_CHECK(it == true);
	BOOST_CHECK(trie.size() == 1);
	BOOST_CHECK(trie.size() == trie.count());
	BOOST_CHECK(trie.contains(std::u32string(2, U'\0')));
}

BOOST_AUTO_TEST_CASE(trie_insert_4)
{
	test_trie<char32_t> trie;

	auto it = trie.insert(std::u32string(U"あいうえお"));

	BOOST_CHECK(it == true);
	BOOST_CHECK(trie.size() == 1);
	BOOST_CHECK(trie.size() == trie.count());
	BOOST_CHECK(trie.contains(std::u32string(U"あいうえお")));
	BOOST_CHECK(trie.contains(std::u32string(U"あいうえ")) == false);
}

BOOST_AUTO_TEST_CASE(trie_insert_5)
{
	test_trie<char32_t> trie;

	trie.insert(std::u32string(U"あ"));
	trie.insert(std::u32string(U"あう"));
	trie.insert(std::u32string(U"い"));
	trie.insert(std::u32string(U"うあい"));
	trie.insert(std::u32string(U"うえ"));

	BOOST_CHECK(trie.size() == 5);
	BOOST_CHECK(trie.size() == trie.count());
	BOOST_CHECK(trie.contains(std::u32string(U"あ")));
	BOOST_CHECK(trie.contains(std::u32string(U"あう")));
	BOOST_CHECK(trie.contains(std::u32string(U"い")));
	BOOST_CHECK(trie.contains(std::u32string(U"うあい")));
	BOOST_CHECK(trie.contains(std::u32string(U"うえ")));
}

BOOST_AUTO_TEST_CASE(trie_insert_6)
{
	test_trie<char16_t> trie;

	auto it = trie.insert(std::u16string(u""));

	BOOST_CHECK(it == trie.end());
	BOOST_CHECK(trie.size() == 0);
	BOOST_CHECK(trie.size() == trie.count());
	BOOST_CHECK(trie.contains(std::u16string(1, u'\0')) == false);
}

BOOST_AUTO_TEST_CASE(trie_insert_7)
{
	test_trie<char16_t> trie;

	auto it = trie.insert(std::u16string(1, u'\0'));

	BOOST_CHECK(it == true);
	BOOST_CHECK(trie.size() == 1);
	BOOST_CHECK(trie.size() == trie.count());
	BOOST_CHECK(trie.contains(std::u16string(1, u'\0')));
}

BOOST_AUTO_TEST_CASE(trie_insert_8)
{
	test_trie<char16_t> trie;

	auto it = trie.insert(std::u16string(2, u'\0'));

	BOOST_CHECK(it == true);
	BOOST_CHECK(trie.size() == 1);
	BOOST_CHECK(trie.size() == trie.count());
	BOOST_CHECK(trie.contains(std::u16string(2, u'\0')));
}

BOOST_AUTO_TEST_CASE(trie_insert_9)
{
	test_trie<char16_t> trie;

	auto it = trie.insert(std::u16string(u"あいうえお"));

	BOOST_CHECK(it == true);
	BOOST_CHECK(trie.size() == 1);
	BOOST_CHECK(trie.size() == trie.count());
	BOOST_CHECK(trie.contains(std::u16string(u"あいうえお")));
	BOOST_CHECK(trie.contains(std::u16string(u"あいうえ")) == false);
}

BOOST_AUTO_TEST_CASE(trie_insert_10)
{
	test_trie<char16_t> trie;

	trie.insert(std::u16string(u"あ"));
	trie.insert(std::u16string(u"あう"));
	trie.insert(std::u16string(u"い"));
	trie.insert(std::u16string(u"うあい"));
	trie.insert(std::u16string(u"うえ"));

	BOOST_CHECK(trie.size() == 5);
	BOOST_CHECK(trie.size() == trie.count());
	BOOST_CHECK(trie.contains(std::u16string(u"あ")));
	BOOST_CHECK(trie.contains(std::u16string(u"あう")));
	BOOST_CHECK(trie.contains(std::u16string(u"い")));
	BOOST_CHECK(trie.contains(std::u16string(u"うあい")));
	BOOST_CHECK(trie.contains(std::u16string(u"うえ")));
}

BOOST_AUTO_TEST_CASE(trie_insert_11)
{
	test_trie<char8_t> trie;

	auto it = trie.insert(std::u8string(u8""));

	BOOST_CHECK(it == trie.end());
	BOOST_CHECK(trie.size() == 0);
	BOOST_CHECK(trie.size() == trie.count());
	BOOST_CHECK(trie.contains(std::u8string(1, u8'\0')) == false);
}

BOOST_AUTO_TEST_CASE(trie_insert_12)
{
	test_trie<char> trie;

	auto it = trie.insert(std::string(1, u8'\0'));

	BOOST_CHECK(it == true);
	BOOST_CHECK(trie.size() == 1);
	BOOST_CHECK(trie.size() == trie.count());
	BOOST_CHECK(trie.contains(std::string(1, u8'\0')));
}

BOOST_AUTO_TEST_CASE(trie_insert_13)
{
	test_trie<char> trie;

	auto it = trie.insert(std::string(2, u8'\0'));

	BOOST_CHECK(it == true);
	BOOST_CHECK(trie.size() == 1);
	BOOST_CHECK(trie.size() == trie.count());
	BOOST_CHECK(trie.contains(std::string(2, u8'\0')));
}

BOOST_AUTO_TEST_CASE(trie_insert_14)
{
	test_trie<char8_t> trie;

	auto it = trie.insert(std::u8string(u8"あいうえお"));

	BOOST_CHECK(it == true);
	BOOST_CHECK(trie.size() == 1);
	BOOST_CHECK(trie.size() == trie.count());
	BOOST_CHECK(trie.contains(std::u8string(u8"あいうえお")));
	BOOST_CHECK(trie.contains(std::u8string(u8"あいうえ")) == false);
}

BOOST_AUTO_TEST_CASE(trie_insert_15)
{
	test_trie<char8_t> trie;

	trie.insert(std::u8string(u8"あ"));
	trie.insert(std::u8string(u8"あう"));
	trie.insert(std::u8string(u8"い"));
	trie.insert(std::u8string(u8"うあい"));
	trie.insert(std::u8string(u8"うえ"));

	BOOST_CHECK(trie.size() == 5);
	BOOST_CHECK(trie.size() == trie.count());
	BOOST_CHECK(trie.contains(std::u8string(u8"あ")));
	BOOST_CHECK(trie.contains(std::u8string(u8"あう")));
	BOOST_CHECK(trie.contains(std::u8string(u8"い")));
	BOOST_CHECK(trie.contains(std::u8string(u8"うあい")));
	BOOST_CHECK(trie.contains(std::u8string(u8"うえ")));
}

BOOST_AUTO_TEST_CASE(trie_insert_16) // 値の挿入
{
	test_trie<char32_t> trie;

	trie.insert(std::u32string(U"a"), 1);
	trie.insert(std::u32string(U"abbr"), 2);

	BOOST_CHECK(trie.size() == 2);
	BOOST_CHECK(trie.count() == 2);
	BOOST_CHECK(trie.contains(std::u32string(U"a")));
	BOOST_CHECK(trie.contains(std::u32string(U"abbr")));
}

// void erase(const_iterator pos)
// void erase(Key const& key)
BOOST_AUTO_TEST_CASE(trie_erase_1)
{
	std::vector<std::u32string> v{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
	test_trie<char32_t> trie;
	trie.assign(v.begin(), v.end());

	trie.erase(std::u32string(U"あ"));

	BOOST_CHECK(trie.size() == 4);
	BOOST_CHECK(trie.size() == trie.count());
	BOOST_CHECK(trie.contains(std::u32string(U"あ")) == false);
	BOOST_CHECK(trie.contains(std::u32string(U"あう")));
	BOOST_CHECK(trie.contains(std::u32string(U"い")));
	BOOST_CHECK(trie.contains(std::u32string(U"うあい")));
	BOOST_CHECK(trie.contains(std::u32string(U"うえ")));
}

BOOST_AUTO_TEST_CASE(trie_erase_2)
{
	std::vector<std::u16string> v{ u"あ", u"あう", u"い", u"うあい", u"うえ" };
	test_trie<char16_t> trie;
	trie.assign(v.begin(), v.end());

	trie.erase(std::u16string(u"あ"));

	BOOST_CHECK(trie.size() == 4);
	BOOST_CHECK(trie.size() == trie.count());
	BOOST_CHECK(trie.contains(std::u16string(u"あ")) == false);
	BOOST_CHECK(trie.contains(std::u16string(u"あう")));
	BOOST_CHECK(trie.contains(std::u16string(u"い")));
	BOOST_CHECK(trie.contains(std::u16string(u"うあい")));
	BOOST_CHECK(trie.contains(std::u16string(u"うえ")));
}

BOOST_AUTO_TEST_CASE(trie_erase_3)
{
	std::vector<std::string> v{ "a", "ac", "b", "cab", "cd" };
	test_trie<char> trie;
	trie.assign(v.begin(), v.end());

	trie.erase(std::string("a"));

	BOOST_CHECK(trie.size() == 4);
	BOOST_CHECK(trie.size() == trie.count());
	BOOST_CHECK(trie.contains(std::string("a")) == false);
	BOOST_CHECK(trie.contains(std::string("ac")));
	BOOST_CHECK(trie.contains(std::string("b")));
	BOOST_CHECK(trie.contains(std::string("cab")));
	BOOST_CHECK(trie.contains(std::string("cd")));
}

BOOST_AUTO_TEST_CASE(trie_erase_4)
{
	std::vector<std::u32string> v{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
	test_trie<char32_t> trie;
	trie.assign(v.begin(), v.end());

	trie.erase(std::u32string(U"あう"));

	BOOST_CHECK(trie.size() == 4);
	BOOST_CHECK(trie.size() == trie.count());
	BOOST_CHECK(trie.contains(std::u32string(U"あ")));
	BOOST_CHECK(trie.contains(std::u32string(U"あう")) == false);
	BOOST_CHECK(trie.contains(std::u32string(U"い")));
	BOOST_CHECK(trie.contains(std::u32string(U"うあい")));
	BOOST_CHECK(trie.contains(std::u32string(U"うえ")));
}

BOOST_AUTO_TEST_CASE(trie_erase_5)
{
	std::vector<std::u16string> v{ u"あ", u"あう", u"い", u"うあい", u"うえ" };
	test_trie<char16_t> trie;
	trie.assign(v.begin(), v.end());

	trie.erase(std::u16string(u"あう"));

	BOOST_CHECK(trie.size() == 4);
	BOOST_CHECK(trie.size() == trie.count());
	BOOST_CHECK(trie.contains(std::u16string(u"あ")));
	BOOST_CHECK(trie.contains(std::u16string(u"あう")) == false);
	BOOST_CHECK(trie.contains(std::u16string(u"い")));
	BOOST_CHECK(trie.contains(std::u16string(u"うあい")));
	BOOST_CHECK(trie.contains(std::u16string(u"うえ")));
}

BOOST_AUTO_TEST_CASE(trie_erase_6)
{
	std::vector<std::string> v{ "a", "ac", "b", "cab", "cd" };
	test_trie<char> trie;
	trie.assign(v.begin(), v.end());

	trie.erase(std::string("ac"));

	BOOST_CHECK(trie.size() == 4);
	BOOST_CHECK(trie.size() == trie.count());
	BOOST_CHECK(trie.contains(std::string("a")));
	BOOST_CHECK(trie.contains(std::string("ac")) == false);
	BOOST_CHECK(trie.contains(std::string("b")));
	BOOST_CHECK(trie.contains(std::string("cab")));
	BOOST_CHECK(trie.contains(std::string("cd")));
}

BOOST_AUTO_TEST_CASE(trie_erase_7)
{
	std::vector<std::u32string> v{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
	test_trie<char32_t> trie;
	trie.assign(v.begin(), v.end());

	trie.erase(std::u32string(U""));

	BOOST_CHECK(trie.size() == 5);
	BOOST_CHECK(trie.size() == trie.count());
	BOOST_CHECK(trie.contains(std::u32string(U"あ")));
	BOOST_CHECK(trie.contains(std::u32string(U"あう")));
	BOOST_CHECK(trie.contains(std::u32string(U"い")));
	BOOST_CHECK(trie.contains(std::u32string(U"うあい")));
	BOOST_CHECK(trie.contains(std::u32string(U"うえ")));
}

BOOST_AUTO_TEST_CASE(trie_erase_8)
{
	std::vector<std::u32string> v{};
	test_trie<char32_t> trie;
	trie.assign(v.begin(), v.end());

	trie.erase(std::u32string(U"あ"));

	BOOST_CHECK(trie.size() == 0);
	BOOST_CHECK(trie.size() == trie.count());
}

BOOST_AUTO_TEST_CASE(trie_erase_9)
{
	std::vector<std::u32string> v{};
	test_trie<char32_t> trie;
	trie.assign(v.begin(), v.end());

	trie.erase(std::u32string(U""));

	BOOST_CHECK(trie.size() == 0);
	BOOST_CHECK(trie.size() == trie.count());
}

// 検索 -----------------------------------------------------------------------

// auto lookup(InputIterator first, InputIterator last) const
BOOST_AUTO_TEST_CASE(trie_lookup_1)
{
	std::vector<std::u32string> v{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
	test_trie<char32_t> trie;
	trie.assign(v.begin(), v.end());

	std::u32string s{ U"あ" };
	auto p = trie.lookup(s.begin(), s.end());

	BOOST_CHECK(p.first);
	BOOST_CHECK(*p.first == U'あ');
	BOOST_CHECK(p.second == s.end());
}

BOOST_AUTO_TEST_CASE(trie_lookup_2)
{
	std::vector<std::u16string> v{ u"あ", u"あう", u"い", u"うあい", u"うえ" };
	test_trie<char16_t> trie;
	trie.assign(v.begin(), v.end());

	std::u16string s{ u"あう" };
	auto p = trie.lookup(s.begin(), s.end());

	BOOST_CHECK(p.first);
	BOOST_CHECK(*p.first == u'う');
	BOOST_CHECK(p.second == s.end());
}

BOOST_AUTO_TEST_CASE(trie_lookup_3)
{
	std::vector<std::string> v{ "a", "ac", "b", "cab", "cd" };
	test_trie<char> trie;
	trie.assign(v.begin(), v.end());

	std::string s{ "ac" };
	auto p = trie.lookup(s.begin(), s.end());

	BOOST_CHECK(p.first);
	BOOST_CHECK(*p.first == 'c');
	BOOST_CHECK(p.second == s.end());
}

BOOST_AUTO_TEST_CASE(trie_lookup_4)
{
	std::vector<std::u32string> v{};
	test_trie<char32_t> trie;
	trie.assign(v.begin(), v.end());

	std::u32string s{ U"あ" };
	auto p = trie.lookup(s.begin(), s.end());

	BOOST_CHECK(!p.first);
	BOOST_CHECK(p.second == s.begin());
}

BOOST_AUTO_TEST_CASE(trie_lookup_5)
{
	std::vector<std::u16string> v{};
	test_trie<char16_t> trie;
	trie.assign(v.begin(), v.end());

	std::u16string s{ u"あう" };
	auto p = trie.lookup(s.begin(), s.end());

	BOOST_CHECK(!p.first);
	BOOST_CHECK(p.second == s.begin());
}

BOOST_AUTO_TEST_CASE(trie_lookup_6)
{
	std::vector<std::string> v{};
	test_trie<char> trie;
	trie.assign(v.begin(), v.end());

	std::string s{ "ac" };
	auto p = trie.lookup(s.begin(), s.end());

	BOOST_CHECK(!p.first);
	BOOST_CHECK(p.second == s.begin());
}

BOOST_AUTO_TEST_CASE(trie_lookup_7)
{
	std::vector<std::u32string> v{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
	test_trie<char32_t> trie;
	trie.assign(v.begin(), v.end());

	std::u32string s{ U"" };
	auto p = trie.lookup(s.begin(), s.end());

	BOOST_CHECK(!p.first);
	BOOST_CHECK(p.second == s.begin());
}

BOOST_AUTO_TEST_CASE(trie_lookup_8)
{
	std::vector<std::u16string> v{ u"あ", u"あう", u"い", u"うあい", u"うえ" };
	test_trie<char16_t> trie;
	trie.assign(v.begin(), v.end());

	std::u16string s{ u"" };
	auto p = trie.lookup(s.begin(), s.end());

	BOOST_CHECK(!p.first);
	BOOST_CHECK(p.second == s.begin());
}

BOOST_AUTO_TEST_CASE(trie_lookup_9)
{
	std::vector<std::string> v{ "a", "ac", "b", "cab", "cd" };
	test_trie<char> trie;
	trie.assign(v.begin(), v.end());

	std::string s{ "" };
	auto p = trie.lookup(s.begin(), s.end());

	BOOST_CHECK(!p.first);
	BOOST_CHECK(p.second == s.begin());
}

/*
【バグ対応】

U"うあい"をU"うい"で検索する。
{ U'う', U'い' }が返るはずだが失敗する。

【原因】
4バイト目と一致するノードが返ってほしいが、7バイト目まで一致するため、7バイト目が返る。

【対策】
trie_bese と stable_trie_base の lookup メンバ関数を遷移回数を返すよう変更し、
ラベルのバイト数との剰余分を巻き戻すようにした。
*/
BOOST_AUTO_TEST_CASE(trie_lookup_10)
{
	using namespace wordring;

	// Trie木を作成
	std::vector<std::u32string> v{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
	auto t = trie<char32_t>(v.begin(), v.end());

	// キー文字列を部分一致検索する
	std::u32string s{ U"うい" };
	auto pair = t.lookup(s.begin(), s.end());

	// 一致した最後のノードと
	BOOST_CHECK(*pair.first == U'う');
	// 一致した最後のキー文字の次を返す
	BOOST_CHECK(*pair.second == U'い');
}

BOOST_AUTO_TEST_CASE(trie_lookup_11)
{
	using namespace wordring;

	// Trie木を作成
	std::vector<std::u16string> v{ u"あ", u"あう", u"い", u"うあい", u"うえ" };
	auto t = trie<char16_t>(v.begin(), v.end());

	// キー文字列を部分一致検索する
	std::u16string s{ u"うい" };
	auto pair = t.lookup(s.begin(), s.end());

	// 一致した最後のノードと
	BOOST_CHECK(*pair.first == u'う');
	// 一致した最後のキー文字の次を返す
	BOOST_CHECK(*pair.second == u'い');
}

BOOST_AUTO_TEST_CASE(trie_lookup_12)
{
	using namespace wordring;

	// Trie木を作成
	std::vector<std::string> v{ "a", "ac", "b", "cab", "cd" };
	auto t = trie<char>(v.begin(), v.end());

	// キー文字列を部分一致検索する
	std::string s{ "cb" };
	auto pair = t.lookup(s.begin(), s.end());

	// 一致した最後のノードと
	BOOST_CHECK(*pair.first == 'c');
	// 一致した最後のキー文字の次を返す
	BOOST_CHECK(*pair.second == 'b');
}

// const_iterator search(InputIterator first, InputIterator last) const
// const_iterator search(Key const& key) const
BOOST_AUTO_TEST_CASE(trie_search_1)
{
	std::vector<std::u32string> v{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
	test_trie<char32_t> trie;
	trie.assign(v.begin(), v.end());

	auto it = trie.search(std::u32string(U"うあ"));

	BOOST_CHECK(!it);
	BOOST_CHECK(*it == U'あ');
}

BOOST_AUTO_TEST_CASE(trie_search_2)
{
	std::vector<std::u32string> v{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
	test_trie<char32_t> trie;
	trie.assign(v.begin(), v.end());

	auto it = trie.search(std::u32string(U""));

	BOOST_CHECK(!it);
	BOOST_CHECK(it == trie.begin());
}

// const_iterator find(InputIterator first, InputIterator last) const
// const_iterator find(Key const& key) const
BOOST_AUTO_TEST_CASE(trie_find_1)
{
	std::vector<std::u32string> v{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
	test_trie<char32_t> trie;
	trie.assign(v.begin(), v.end());

	auto it = trie.find(std::u32string(U"あ"));

	BOOST_CHECK(it);
	BOOST_CHECK(*it == U'あ');
}

BOOST_AUTO_TEST_CASE(trie_find_2)
{
	std::vector<std::u16string> v{ u"あ", u"あう", u"い", u"うあい", u"うえ" };
	test_trie<char16_t> trie;
	trie.assign(v.begin(), v.end());

	auto it = trie.find(std::u16string(u"あ"));

	BOOST_CHECK(it);
	BOOST_CHECK(*it == u'あ');
}

BOOST_AUTO_TEST_CASE(trie_find_3)
{
	std::vector<std::string> v{ "a", "ac", "b", "cab", "cd" };
	test_trie<char> trie;
	trie.assign(v.begin(), v.end());

	auto it = trie.find(std::string("a"));

	BOOST_CHECK(it);
	BOOST_CHECK(*it == 'a');
}

BOOST_AUTO_TEST_CASE(trie_find_4)
{
	std::vector<std::u32string> v{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
	test_trie<char32_t> trie;
	trie.assign(v.begin(), v.end());

	auto it = trie.find(std::u32string(U""));

	BOOST_CHECK(!it);
	BOOST_CHECK(it == trie.end());
}

BOOST_AUTO_TEST_CASE(trie_find_5)
{
	std::vector<std::u16string> v{ u"あ", u"あう", u"い", u"うあい", u"うえ" };
	test_trie<char16_t> trie;
	trie.assign(v.begin(), v.end());

	auto it = trie.find(std::u16string(u""));

	BOOST_CHECK(!it);
	BOOST_CHECK(it == trie.end());
}

BOOST_AUTO_TEST_CASE(trie_find_6)
{
	std::vector<std::string> v{ "a", "ac", "b", "cab", "cd" };
	test_trie<char> trie;
	trie.assign(v.begin(), v.end());

	auto it = trie.find(std::string(""));

	BOOST_CHECK(!it);
	BOOST_CHECK(it == trie.end());
}

// bool contains(InputIterator first, InputIterator last) const
// bool contains(Key const& key) const
BOOST_AUTO_TEST_CASE(trie_contains_1)
{
	std::vector<std::u32string> v{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
	test_trie<char32_t> trie;
	trie.assign(v.begin(), v.end());

	BOOST_CHECK(trie.contains(std::u32string(U"あ")));
	BOOST_CHECK(trie.contains(std::u32string(U"あう")));
	BOOST_CHECK(trie.contains(std::u32string(U"い")));
	BOOST_CHECK(trie.contains(std::u32string(U"うあい")));
	BOOST_CHECK(trie.contains(std::u32string(U"うえ")));
	BOOST_CHECK(trie.contains(std::u32string(U"え")) == false);
	BOOST_CHECK(trie.contains(std::u32string(U"")) == false);
}

BOOST_AUTO_TEST_CASE(trie_contains_2)
{
	std::vector<std::u16string> v{ u"あ", u"あう", u"い", u"うあい", u"うえ" };
	test_trie<char16_t> trie;
	trie.assign(v.begin(), v.end());

	BOOST_CHECK(trie.contains(std::u16string(u"あ")));
	BOOST_CHECK(trie.contains(std::u16string(u"あう")));
	BOOST_CHECK(trie.contains(std::u16string(u"い")));
	BOOST_CHECK(trie.contains(std::u16string(u"うあい")));
	BOOST_CHECK(trie.contains(std::u16string(u"うえ")));
	BOOST_CHECK(trie.contains(std::u16string(u"え")) == false);
	BOOST_CHECK(trie.contains(std::u16string(u"")) == false);
}

BOOST_AUTO_TEST_CASE(trie_contains_3)
{
	std::vector<std::string> v{ "a", "ac", "b", "cab", "cd" };
	test_trie<char> trie;
	trie.assign(v.begin(), v.end());

	BOOST_CHECK(trie.contains(std::string("a")));
	BOOST_CHECK(trie.contains(std::string("ac")));
	BOOST_CHECK(trie.contains(std::string("b")));
	BOOST_CHECK(trie.contains(std::string("cab")));
	BOOST_CHECK(trie.contains(std::string("cd")));
	BOOST_CHECK(trie.contains(std::string("d")) == false);
	BOOST_CHECK(trie.contains(std::string("")) == false);
}

// 関数 -----------------------------------------------------------------------

// inline std::ostream& operator<<(std::ostream& os, basic_trie<Label1, Base1> const& trie)
// inline std::istream& operator>>(std::istream& is, basic_trie<Label1, Base1>& trie)
BOOST_AUTO_TEST_CASE(trie_stream_1)
{
	std::vector<std::u32string> v{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
	test_trie<char32_t> t1, t2;
	t1.assign(v.begin(), v.end());

	std::stringstream ss;
	ss << t1;
	ss >> t2;

	BOOST_CHECK(t1.m_c == t2.m_c);
}

BOOST_AUTO_TEST_CASE(trie_stream_2)
{
	test_trie<char32_t> t1{}, t2{};

	std::stringstream ss;
	ss << t1;
	ss >> t2;

	BOOST_CHECK(t1.m_c == t2.m_c);
}

// ストレステスト --------------------------------------------------------------

BOOST_AUTO_TEST_CASE(trie_stress_1)
{
	using wordring::whatwg::encoding_cast;

	std::ifstream is(japanese_words_path);
	BOOST_REQUIRE(is.is_open());

	std::vector<std::u32string> w;
	std::string buf{};
#ifdef NDEBUG
	while (std::getline(is, buf)) w.push_back(encoding_cast<std::u32string>(buf));
#else
	for (size_t i = 0; i < 1000 && std::getline(is, buf); ++i) w.push_back(encoding_cast<std::u32string>(buf));
#endif

	test_trie<char32_t> t;
	std::uint32_t i = 0;
	for (auto const& s : w) t.insert(s, i++);

	int e = 0;
	i = 0;
	for (auto const& s : w)
	{
		auto it = t.find(s);
		if (it == t.cend() || t.at(it) != i++) ++e;
	}

	BOOST_CHECK(e == 0);
	BOOST_CHECK(t.count() == t.size());
}

BOOST_AUTO_TEST_CASE(trie_stress_2)
{
	using wordring::whatwg::encoding_cast;

	std::ifstream is(japanese_words_path);
	BOOST_REQUIRE(is.is_open());

	std::vector<std::u16string> w;
	std::string buf{};
#ifdef NDEBUG
	while (std::getline(is, buf)) w.push_back(encoding_cast<std::u16string>(buf));
#else
	for (size_t i = 0; i < 1000 && std::getline(is, buf); ++i) w.push_back(encoding_cast<std::u16string>(buf));
#endif

	test_trie<char16_t> t;
	std::uint32_t i = 0;
	for (auto const& s : w) t.insert(s, i++);

	int e = 0;
	i = 0;
	for (auto const& s : w)
	{
		auto it = t.find(s);
		if (it == t.cend() || t.at(it) != i++) ++e;
	}

	BOOST_CHECK(e == 0);
	BOOST_CHECK(t.count() == t.size());
}

BOOST_AUTO_TEST_CASE(trie_stress_3)
{
	std::ifstream is(japanese_words_path);
	BOOST_REQUIRE(is.is_open());

	std::vector<std::string> w;
	std::string buf{};
#ifdef NDEBUG
	while (std::getline(is, buf)) w.push_back(buf);
#else
	for (size_t i = 0; i < 1000 && std::getline(is, buf); ++i) w.push_back(buf);
#endif

	test_trie<char> t;
	std::uint32_t i = 0;
	for (auto const& s : w) t.insert(s, i++);

	int e = 0;
	i = 0;
	for (auto const& s : w)
	{
		auto it = t.find(s);
		if (it == t.cend() || t.at(it) != i++) ++e;
	}

	BOOST_CHECK(e == 0);
	BOOST_CHECK(t.count() == t.size());
}

BOOST_AUTO_TEST_SUITE_END()
