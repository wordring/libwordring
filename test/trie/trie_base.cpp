﻿// test/trie/trie_base.cpp

#include <boost/test/unit_test.hpp>

#include <wordring/serialize/serialize_iterator.hpp>
#include <wordring/trie/trie_base.hpp>
#include <wordring/trie/trie_base_iterator.hpp>
#include <wordring/tree/tree_iterator.hpp>

#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <memory>
#include <random>
#include <sstream>
#include <string>
#include <vector>

#define STRING(str) #str
#define TO_STRING(str) STRING(str)

std::string const english_words_path{ TO_STRING(ENGLISH_WORDS_PATH) };
std::string const current_source_path{ TO_STRING(CURRENT_SOURCE_PATH) };

namespace
{
	using base_node     = wordring::detail::trie_node;
	using base_trie     = wordring::detail::trie_base<>;
	using base_iterator = typename base_trie::const_iterator;

	class test_iterator : public base_iterator
	{
	public:
		using base_type = base_iterator;

		using base_type::index_type;

		using base_type::find;
		using base_type::mother;
		using base_type::base;
		using base_type::limit;
		using base_type::has_null;
		using base_type::has_sibling;

		using base_type::m_c;
		using base_type::m_index;

	public:
		test_iterator() : base_type() {}

		test_iterator(container& c, index_type index)
			: base_type(c, index) {}

		test_iterator(base_type const& base)
			: base_type(base) {}
	};

	class test_trie : public base_trie
	{
	public:
		using base_type = base_trie;
		
		using base_type::is_tail;
		using base_type::has_child;
		using base_type::has_null;
		using base_type::has_sibling;
		using base_type::at;
		using base_type::add;

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

BOOST_AUTO_TEST_SUITE(trie_base_test)

// ----------------------------------------------------------------------------
// trie_base
// ----------------------------------------------------------------------------

// trie_base()
BOOST_AUTO_TEST_CASE(trie_base_constrcut_1)
{
	using namespace wordring;

	detail::trie_base<> trie{};
}

// explicit trie_base(allocator_type const& alloc)
BOOST_AUTO_TEST_CASE(trie_base_constrcut_2)
{
	using namespace wordring;

	detail::trie_base<> trie{ std::allocator<detail::trie_node>() };
}

// trie_base(InputIterator first, InputIterator last, allocator_type const& alloc = allocator_type())
BOOST_AUTO_TEST_CASE(trie_base_constrcut_3)
{
	using namespace wordring;

	std::vector<std::string> v1{ "a", "ac", "b", "cab", "cd" };
	detail::trie_base<> t1{ v1.begin(), v1.end() };

	std::vector<std::int32_t> v2{ t1.ibegin(), t1.iend() };
	detail::trie_base<> t2{ v2.begin(), v2.end(), std::allocator<detail::trie_node>() };

	BOOST_CHECK(t1.size() == t2.size());
}

// trie_base(ForwardIterator first, ForwardIterator last, allocator_type const& alloc = allocator_type())
BOOST_AUTO_TEST_CASE(trie_base_constrcut_4)
{
	using namespace wordring;

	std::vector<std::string> v{ "a", "ac", "b", "cab", "cd" };
	detail::trie_base<> trie{ v.begin(), v.end(), std::allocator<detail::trie_node>() };
}

// trie_base(std::initializer_list<detail::trie_node> il)
BOOST_AUTO_TEST_CASE(trie_base_constrcut_5)
{
	using namespace wordring;

	detail::trie_base<> trie{ { { 0, 1 }, { 2, 3 } } };
}

// void assign(InputIterator first, InputIterator last)
BOOST_AUTO_TEST_CASE(trie_base_assign_1)
{
	using namespace wordring;

	detail::trie_base<> t1{};

	std::vector<std::string> v1{ "a", "ac", "b", "cab", "cd" };
	t1.assign(v1.begin(), v1.end());

	auto v2 = std::vector<std::int32_t>(t1.ibegin(), t1.iend());

	test_trie t2{};
	t2.assign(v2.begin(), v2.end());

	BOOST_CHECK(t2.count() == 5);
	BOOST_CHECK(t2.size() == 5);
	BOOST_CHECK(t2.contains(std::string("a")));
	BOOST_CHECK(t2.contains(std::string("ac")));
	BOOST_CHECK(t2.contains(std::string("b")));
	BOOST_CHECK(t2.contains(std::string("cab")));
	BOOST_CHECK(t2.contains(std::string("cd")));
}

BOOST_AUTO_TEST_CASE(trie_base_assign_2)
{
	using namespace wordring;

	detail::trie_base<> t1{};

	std::vector<std::int32_t> v{};
	std::copy(t1.ibegin(), t1.iend(), std::back_inserter(v));

	test_trie t2{};
	t2.assign(v.begin(), v.end());

	BOOST_CHECK(t2.count() == 0);
	BOOST_CHECK(t2.size() == 0);
}

BOOST_AUTO_TEST_CASE(trie_base_assign_3)
{
	using namespace wordring;

	detail::trie_base<> t1{};

	std::vector<std::string> v1{ "a", "ac", "b", "cab", "cd" };
	t1.assign(v1.begin(), v1.end());

	auto it1 = serialize_iterator(t1.ibegin());
	auto it2 = serialize_iterator(t1.iend());

	auto v2 = std::vector<std::int8_t>(it1, it2);

	test_trie t2{};
	t2.assign(v2.begin(), v2.end());

	BOOST_CHECK(t2.count() == 5);
	BOOST_CHECK(t2.size() == 5);
	BOOST_CHECK(t2.contains(std::string("a")));
	BOOST_CHECK(t2.contains(std::string("ac")));
	BOOST_CHECK(t2.contains(std::string("b")));
	BOOST_CHECK(t2.contains(std::string("cab")));
	BOOST_CHECK(t2.contains(std::string("cd")));
}

BOOST_AUTO_TEST_CASE(trie_base_assign_4)
{
	using namespace wordring;

	detail::trie_base<> t1{};

	auto it1 = serialize_iterator(t1.ibegin());
	auto it2 = serialize_iterator(t1.iend());

	auto v2 = std::vector<std::int8_t>(it1, it2);

	test_trie t2{};
	t2.assign(v2.begin(), v2.end());

	BOOST_CHECK(t2.count() == 0);
	BOOST_CHECK(t2.size() == 0);
}

// void assign(ForwardIterator first, ForwardIterator last)
BOOST_AUTO_TEST_CASE(trie_base_assign_5)
{
	using namespace wordring;

	detail::trie_base<> t1{};

	std::vector<std::string> v{ "a", "ac", "b", "cab", "cd" };
	t1.assign(v.begin(), v.end());

	detail::trie_base<> t2{};
	t2 = t1;

	BOOST_CHECK(t2.size() == 5);
	BOOST_CHECK(t2.contains(std::string("a")));
	BOOST_CHECK(t2.contains(std::string("ac")));
	BOOST_CHECK(t2.contains(std::string("b")));
	BOOST_CHECK(t2.contains(std::string("cab")));
	BOOST_CHECK(t2.contains(std::string("cd")));
}

BOOST_AUTO_TEST_CASE(trie_base_assign_6)
{
	test_trie trie{};

	std::vector<std::string> v{ "a", "ac", "b", "cab", "cd" };
	trie.assign(v.begin(), v.end());

	BOOST_CHECK(trie.size() == 5);
	BOOST_CHECK(trie.count() == 5);
	BOOST_CHECK(trie.contains(std::string("a")));
	BOOST_CHECK(trie.contains(std::string("ac")));
	BOOST_CHECK(trie.contains(std::string("b")));
	BOOST_CHECK(trie.contains(std::string("cab")));
	BOOST_CHECK(trie.contains(std::string("cd")));
}

BOOST_AUTO_TEST_CASE(trie_base_assign_7)
{
	test_trie trie{};

	std::vector<std::string> v{};
	trie.assign(v.begin(), v.end());

	BOOST_CHECK(trie.size() == 0);
	BOOST_CHECK(trie.count() == 0);
}

// 要素アクセス ----------------------------------------------------------------

// reference at(Key const& key)
BOOST_AUTO_TEST_CASE(trie_base_at_1)
{
	test_trie trie{};

	std::vector<std::string> v{ "a", "ac", "b", "cab", "cd" };
	trie.assign(v.begin(), v.end());

	auto val = trie.at(std::string("a"));
	val = 100;

	BOOST_CHECK(trie.at(std::string("a")) == 100);
}

BOOST_AUTO_TEST_CASE(trie_base_at_2)
{
	test_trie trie{};

	std::vector<std::string> v{ "a", "ac", "b", "cab", "cd" };
	trie.assign(v.begin(), v.end());

	trie.at(std::string("ac")) = 100;

	BOOST_CHECK(trie.at(std::string("ac")) == 100);
}

BOOST_AUTO_TEST_CASE(trie_base_at_3)
{
	test_trie trie{};

	std::vector<std::string> v{ "a", "ac", "b", "cab", "cd" };
	trie.assign(v.begin(), v.end());

	BOOST_CHECK_THROW(trie.at(std::string("d")), std::out_of_range);
}

BOOST_AUTO_TEST_CASE(trie_base_at_4)
{
	test_trie trie{};

	BOOST_CHECK_THROW(trie.at(std::string("a")), std::out_of_range);
}

BOOST_AUTO_TEST_CASE(trie_base_at_5)
{
	test_trie trie{};

	std::vector<std::string> v{ "a", "ac", "b", "cab", "cd" };
	trie.assign(v.begin(), v.end());

	trie.at(std::string("ac")) = 100;
	trie.at(std::string("a")) = 1000;

	BOOST_CHECK(trie.at(std::string("ac")) == 100);
	BOOST_CHECK(trie.at(std::string("a")) == 1000);

	BOOST_CHECK(trie.count() == 5);
}

// const_reference const at(Key const& key) const
BOOST_AUTO_TEST_CASE(trie_base_at_6)
{
	test_trie trie{};

	std::vector<std::string> v{ "a", "ac", "b", "cab", "cd" };
	trie.assign(v.begin(), v.end());

	trie.at(std::string("ac")) = 100;
	trie.at(std::string("a")) = 1000;

	test_trie const& t = trie;

	BOOST_CHECK(t.at(std::string("ac")) == 100);
	BOOST_CHECK(t.at(std::string("a")) == 1000);

	BOOST_CHECK(t.count() == 5);
}

// reference operator[](Key const& key)
BOOST_AUTO_TEST_CASE(trie_base_at_7)
{
	test_trie trie{};

	std::vector<std::string> v{ "a", "ac", "b", "cab", "cd" };
	trie.assign(v.begin(), v.end());

	trie[std::string("ac")] = 100;
	trie[std::string("a")] = 1000;

	BOOST_CHECK(trie[std::string("ac")] == 100);
	BOOST_CHECK(trie[std::string("a")] == 1000);

	BOOST_CHECK(trie.count() == 5);
}

BOOST_AUTO_TEST_CASE(trie_base_at_8)
{
	test_trie trie{};

	std::vector<std::string> v{};
	trie.assign(v.begin(), v.end());

	trie[std::string("ac")] = 100;
	trie[std::string("a")] = 1000;

	BOOST_CHECK(trie[std::string("ac")] == 100);
	BOOST_CHECK(trie[std::string("a")] == 1000);

	BOOST_CHECK(trie.count() == 2);
}

// イテレータ ------------------------------------------------------------------

// const_iterator begin() const noexcept
BOOST_AUTO_TEST_CASE(trie_base_begin_1)
{
	test_trie trie{};
	BOOST_CHECK(trie.begin() != trie.end());
}

// const_iterator cbegin() const noexcept
BOOST_AUTO_TEST_CASE(trie_base_cbegin_1)
{
	test_trie trie{};
	BOOST_CHECK(trie.cbegin() != trie.cend());
}

// const_iterator end() const noexcept
BOOST_AUTO_TEST_CASE(trie_base_end_1)
{
	test_trie trie{};
	BOOST_CHECK(trie.begin() != trie.end());
}

// const_iterator cend() const noexcept
BOOST_AUTO_TEST_CASE(trie_base_cend_1)
{
	test_trie trie{};
	BOOST_CHECK(trie.cbegin() != trie.cend());
}

// 容量 -----------------------------------------------------------------------

// bool empty() const noexcept
BOOST_AUTO_TEST_CASE(trie_base_empty_1)
{
	test_trie trie{};
	BOOST_CHECK(trie.empty());
}

BOOST_AUTO_TEST_CASE(trie_base_empty_2)
{
	test_trie trie{};
	trie.insert(std::string("abc"));
	BOOST_CHECK(trie.empty() == false);
}

// size_type size() const noexcept
BOOST_AUTO_TEST_CASE(trie_base_size_1)
{
	test_trie trie{};
	BOOST_CHECK(trie.size() == 0);
}

BOOST_AUTO_TEST_CASE(trie_base_size_2)
{
	test_trie trie{};
	trie.insert(std::string("abc"));
	BOOST_CHECK(trie.size() == 1);
	trie.insert(std::string("abc"));
	BOOST_CHECK(trie.size() == 1);
}

// size_type max_size() const noexcept
BOOST_AUTO_TEST_CASE(trie_base_max_size_1)
{
	test_trie trie{};
	BOOST_CHECK(trie.max_size());
}

// 変更 -----------------------------------------------------------------------

// using base_type::clear;
BOOST_AUTO_TEST_CASE(trie_base_clear_1)
{
	test_trie trie{};

	std::vector<std::string> v{ "\1\1", "\1" };
	trie.assign(v.begin(), v.end());

	BOOST_CHECK(trie.count() == 2);
	BOOST_CHECK(trie.size() == 2);

	trie.clear();

	BOOST_CHECK(trie.count() == 0);
	BOOST_CHECK(trie.size() == 0);
}

// const_iterator insert(InputIterator first, InputIterator last, value_type value = 0)
BOOST_AUTO_TEST_CASE(trie_base_insert_1)
{
	test_trie trie{};

	auto s = std::string("\1");
	trie.insert(s.begin(), s.end());

	BOOST_CHECK(trie.size() == 1);
	BOOST_CHECK(trie.count() == 1);
	BOOST_CHECK(trie.contains(std::string("\1")));
}

BOOST_AUTO_TEST_CASE(trie_base_insert_2)
{
	test_trie trie{};

	auto s = std::string("\1\1");
	trie.insert(s.begin(), s.end());

	BOOST_CHECK(trie.size() == 1);
	BOOST_CHECK(trie.count() == 1);
	BOOST_CHECK(trie.contains(std::string("\1\1")));
}

BOOST_AUTO_TEST_CASE(trie_base_insert_3)
{
	test_trie trie{};

	auto s1 = std::string("\1");
	trie.insert(s1.begin(), s1.end());
	auto s2 = std::string("\1\1");
	trie.insert(s2.begin(), s2.end());

	BOOST_CHECK(trie.size() == 2);
	BOOST_CHECK(trie.count() == 2);
	BOOST_CHECK(trie.contains(std::string("\1")));
	BOOST_CHECK(trie.contains(std::string("\1\1")));
}

BOOST_AUTO_TEST_CASE(trie_base_insert_4)
{
	test_trie trie{};

	auto s1 = std::string("\1\1");
	trie.insert(s1.begin(), s1.end());
	auto s2 = std::string("\1");
	trie.insert(s2.begin(), s2.end());

	BOOST_CHECK(trie.size() == 2);
	BOOST_CHECK(trie.count() == 2);
	BOOST_CHECK(trie.contains(std::string("\1")));
	BOOST_CHECK(trie.contains(std::string("\1\1")));
}

BOOST_AUTO_TEST_CASE(trie_base_insert_5)
{
	test_trie trie{};

	auto s1 = std::string("\1\1");
	trie.insert(s1.begin(), s1.end());
	auto s2 = std::string("\1");
	trie.insert(s2.begin(), s2.end());
	auto s3 = std::string("\1\1");
	trie.insert(s3.begin(), s3.end());

	BOOST_CHECK(trie.size() == 2);
	BOOST_CHECK(trie.count() == 2);
	BOOST_CHECK(trie.contains(std::string("\1")));
	BOOST_CHECK(trie.contains(std::string("\1\1")));
}

BOOST_AUTO_TEST_CASE(trie_base_insert_6)
{
	test_trie trie{};

	auto s1 = std::string("\1\1");
	trie.insert(s1.begin(), s1.end());
	auto s2 = std::string("\1");
	trie.insert(s2.begin(), s2.end());
	auto s3 = std::string("\1");
	trie.insert(s3.begin(), s3.end());

	BOOST_CHECK(trie.size() == 2);
	BOOST_CHECK(trie.count() == 2);
	BOOST_CHECK(trie.contains(std::string("\1")));
	BOOST_CHECK(trie.contains(std::string("\1\1")));
}

BOOST_AUTO_TEST_CASE(trie_base_insert_7)
{
	test_trie trie{};

	auto s1 = std::string("\1\1");
	trie.insert(s1.begin(), s1.end());
	auto s2 = std::string("\1");
	trie.insert(s2.begin(), s2.end());
	auto s3 = std::string("");
	trie.insert(s3.begin(), s3.end());

	BOOST_CHECK(trie.size() == 2);
	BOOST_CHECK(trie.count() == 2);
	BOOST_CHECK(trie.contains(std::string("\1")));
	BOOST_CHECK(trie.contains(std::string("\1\1")));
}

BOOST_AUTO_TEST_CASE(trie_base_insert_8)
{
	test_trie trie{};

	auto s1 = std::string({ '\1' });
	trie.insert(s1.begin(), s1.end());
	auto s2 = std::string({ '\1', '\0' });
	trie.insert(s2.begin(), s2.end());

	BOOST_CHECK(trie.size() == 2);
	BOOST_CHECK(trie.count() == 2);
	BOOST_CHECK(trie.contains(std::string({ '\1' })));
	BOOST_CHECK(trie.contains(std::string({ '\1', '\0' })));
}

BOOST_AUTO_TEST_CASE(trie_base_insert_9)
{
	test_trie trie{};

	auto s1 = std::string({ '\0' });
	trie.insert(s1.begin(), s1.end());
	auto s2 = std::string({ '\0', '\1' });
	trie.insert(s2.begin(), s2.end());

	BOOST_CHECK(trie.size() == 2);
	BOOST_CHECK(trie.count() == 2);
	BOOST_CHECK(trie.contains(std::string({ '\0' })));
	BOOST_CHECK(trie.contains(std::string({ '\0', '\1' })));
}

BOOST_AUTO_TEST_CASE(trie_base_insert_10) // ストレステスト
{
	std::vector<std::pair<std::string, std::uint32_t>> w1{};
	std::vector<std::pair<std::string, std::uint32_t>> w2{};

	{
		std::ifstream is(english_words_path);
		BOOST_REQUIRE(is.is_open());

		std::vector<std::string> v{};
		std::string buf{};

		for (size_t i = 0; i < 10000 && std::getline(is, buf); ++i) v.push_back(buf);

		std::random_device rd;
		std::mt19937 mt(rd());
		std::shuffle(v.begin(), v.end(), mt);

		for (auto const& s : v) w1.push_back(std::make_pair(s, static_cast<std::uint32_t>(w1.size())));
		w2 = w1;
		for (auto const& s : v) w2.push_back(std::make_pair(s, static_cast<std::uint32_t>(w2.size())));
	}

	test_trie trie{};

	for (auto const& p : w2) trie.insert(p.first, p.second);

	BOOST_CHECK(trie.count() == w1.size());
	BOOST_CHECK(trie.size() == w1.size());

	int error = 0;
	for (auto const& p : w1) if (trie.at(p.first) != p.second) ++error;

	BOOST_CHECK(error == 0);
}

BOOST_AUTO_TEST_CASE(trie_base_insert_11) // 値の挿入
{
	test_trie trie{};

	auto s1 = std::string("a");
	trie.insert(s1.begin(), s1.end(), 1);
	auto s2 = std::string("abbr");
	trie.insert(s2.begin(), s2.end(), 2);

	BOOST_CHECK(trie.size() == 2);
	BOOST_CHECK(trie.count() == 2);
	BOOST_CHECK(trie.contains(std::string("a")));
	BOOST_CHECK(trie.contains(std::string("abbr")));
}

// void erase(const_iterator pos)
// void erase(Key const& key)
BOOST_AUTO_TEST_CASE(trie_base_erase_1)
{
	test_trie trie{};

	std::vector<std::string> v{ "a", "ac", "b", "cab", "cd" };
	trie.assign(v.begin(), v.end());

	trie.erase(std::string("a"));

	BOOST_CHECK(trie.size() == 4);
	BOOST_CHECK(trie.count() == 4);
	BOOST_CHECK(trie.contains(std::string("a")) == false);
	BOOST_CHECK(trie.contains(std::string("ac")));
	BOOST_CHECK(trie.contains(std::string("b")));
	BOOST_CHECK(trie.contains(std::string("cab")));
	BOOST_CHECK(trie.contains(std::string("cd")));
}

BOOST_AUTO_TEST_CASE(trie_base_erase_2)
{
	test_trie trie{};

	std::vector<std::string> v{ "a", "ac", "b", "cab", "cd" };
	trie.assign(v.begin(), v.end());

	trie.erase(std::string("ac"));

	BOOST_CHECK(trie.size() == 4);
	BOOST_CHECK(trie.count() == 4);
	BOOST_CHECK(trie.contains(std::string("a")));
	BOOST_CHECK(trie.contains(std::string("ac")) == false);
	BOOST_CHECK(trie.contains(std::string("b")));
	BOOST_CHECK(trie.contains(std::string("cab")));
	BOOST_CHECK(trie.contains(std::string("cd")));
}

BOOST_AUTO_TEST_CASE(trie_base_erase_3)
{
	test_trie trie{};

	std::vector<std::string> v{ "a", "ac", "b", "cab", "cd" };
	trie.assign(v.begin(), v.end());

	trie.erase(std::string("b"));

	BOOST_CHECK(trie.size() == 4);
	BOOST_CHECK(trie.count() == 4);
	BOOST_CHECK(trie.contains(std::string("a")));
	BOOST_CHECK(trie.contains(std::string("ac")));
	BOOST_CHECK(trie.contains(std::string("b")) == false);
	BOOST_CHECK(trie.contains(std::string("cab")));
	BOOST_CHECK(trie.contains(std::string("cd")));
}

BOOST_AUTO_TEST_CASE(trie_base_erase_4)
{
	test_trie trie{};

	std::vector<std::string> v{ "a", "ac", "b", "cab", "cd" };
	trie.assign(v.begin(), v.end());

	trie.erase(std::string("cab"));

	BOOST_CHECK(trie.size() == 4);
	BOOST_CHECK(trie.count() == 4);
	BOOST_CHECK(trie.contains(std::string("a")));
	BOOST_CHECK(trie.contains(std::string("ac")));
	BOOST_CHECK(trie.contains(std::string("b")));
	BOOST_CHECK(trie.contains(std::string("cab")) == false);
	BOOST_CHECK(trie.contains(std::string("cd")));
}

BOOST_AUTO_TEST_CASE(trie_base_erase_5)
{
	test_trie trie{};

	std::vector<std::string> v{ "a", "ac", "b", "cab", "cd" };
	trie.assign(v.begin(), v.end());

	trie.erase(std::string("cd"));

	BOOST_CHECK(trie.size() == 4);
	BOOST_CHECK(trie.count() == 4);
	BOOST_CHECK(trie.contains(std::string("a")));
	BOOST_CHECK(trie.contains(std::string("ac")));
	BOOST_CHECK(trie.contains(std::string("b")));
	BOOST_CHECK(trie.contains(std::string("cab")));
	BOOST_CHECK(trie.contains(std::string("cd")) == false);
}

BOOST_AUTO_TEST_CASE(trie_base_erase_6)
{
	test_trie trie{};

	std::vector<std::string> v{ "a", "ac", "b", "cab", "cd" };
	trie.assign(v.begin(), v.end());

	trie.erase(std::string(""));

	BOOST_CHECK(trie.size() == 5);
	BOOST_CHECK(trie.count() == 5);
	BOOST_CHECK(trie.contains(std::string("a")));
	BOOST_CHECK(trie.contains(std::string("ac")));
	BOOST_CHECK(trie.contains(std::string("b")));
	BOOST_CHECK(trie.contains(std::string("cab")));
	BOOST_CHECK(trie.contains(std::string("cd")));
}

BOOST_AUTO_TEST_CASE(trie_base_erase_7)
{
	test_trie trie{};

	trie.erase(std::string("a"));

	BOOST_CHECK(trie.size() == 0);
	BOOST_CHECK(trie.count() == 0);
}

BOOST_AUTO_TEST_CASE(trie_base_erase_8)
{
	test_trie trie{};

	trie.erase(std::string(""));

	BOOST_CHECK(trie.size() == 0);
	BOOST_CHECK(trie.count() == 0);
}

BOOST_AUTO_TEST_CASE(trie_base_erase_9)
{
	test_trie trie{};

	std::vector<std::string> words{};
	{
		std::ifstream is(english_words_path);
		BOOST_REQUIRE(is.is_open());

		std::vector<std::string> v{};
		std::string buf{};

		for (size_t i = 0; i < 10000 && std::getline(is, buf); ++i) v.push_back(buf);
		for (std::string const& s : v) words.push_back(s);

		for (std::string const& s : words) trie.insert(s);
	}

	std::random_device rd;
	std::mt19937 mt(rd());
	std::shuffle(words.begin(), words.end(), mt);

	for (std::string const& s : words) trie.erase(s);

	BOOST_CHECK(trie.count() == 0);
	BOOST_CHECK(trie.size() == 0);

	int error = 0;
	for (std::string const& s : words) if (trie.contains(s)) ++error;

	BOOST_CHECK(error == 0);
}

// 検索 -----------------------------------------------------------------------

// auto lookup(InputIterator first, InputIterator last) const
BOOST_AUTO_TEST_CASE(trie_base_lookup_1)
{
	test_trie trie{};

	std::vector<std::string> v{ "a", "ac", "b", "cab", "cd" };
	trie.assign(v.begin(), v.end());

	std::string s{ "ac" };
	auto p = trie.lookup(s.begin(), s.end());

	BOOST_CHECK(p.first);
	BOOST_CHECK(*p.first == 'c');
	BOOST_CHECK(p.second == s.end());
}

BOOST_AUTO_TEST_CASE(trie_base_lookup_2)
{
	test_trie trie{};

	std::vector<std::string> v{ "a", "ac", "b", "cab", "cd" };
	trie.assign(v.begin(), v.end());

	std::string s{ "a" };
	auto p = trie.lookup(s.begin(), s.end());

	BOOST_CHECK(p.first);
	BOOST_CHECK(*p.first == 'a');
	BOOST_CHECK(p.second == s.end());
}

BOOST_AUTO_TEST_CASE(trie_base_lookup_3)
{
	test_trie trie{};

	std::vector<std::string> v{ "a", "ac", "b", "cab", "cd" };
	trie.assign(v.begin(), v.end());

	std::string s{ "" };
	auto p = trie.lookup(s.begin(), s.end());

	BOOST_CHECK(p.first == trie.cbegin());
	BOOST_CHECK(p.second == s.end());
}

BOOST_AUTO_TEST_CASE(trie_base_lookup_4)
{
	test_trie trie{};

	std::vector<std::string> v{ "a", "ac", "b", "cab", "cd" };
	trie.assign(v.begin(), v.end());

	std::string s{ "ab" };
	auto p = trie.lookup(s.begin(), s.end());

	BOOST_CHECK(p.first); // "a"に一致するためtrueを返すことに注意。
	BOOST_CHECK(*p.first == 'a');
	BOOST_CHECK(*p.second == 'b');
}

BOOST_AUTO_TEST_CASE(trie_base_lookup_5)
{
	test_trie trie{};

	std::vector<std::string> v{ "a", "ac", "b", "cab", "cd" };
	trie.assign(v.begin(), v.end());

	std::string s{ "b" };
	auto p = trie.lookup(s.begin(), s.end());

	BOOST_CHECK(p.first);
	BOOST_CHECK(*p.first == 'b');
	BOOST_CHECK(p.second == s.end());
}

BOOST_AUTO_TEST_CASE(trie_base_lookup_6)
{
	test_trie trie{};

	std::vector<std::string> v{};
	trie.assign(v.begin(), v.end());

	std::string s{ "ab" };
	auto p = trie.lookup(s.begin(), s.end());

	BOOST_CHECK(!p.first);
	BOOST_CHECK(p.second == s.begin());
}

BOOST_AUTO_TEST_CASE(trie_base_lookup_7)
{
	test_trie trie{};

	std::vector<std::string> v{};
	trie.assign(v.begin(), v.end());

	std::string s{ "" };
	auto p = trie.lookup(s.begin(), s.end());

	BOOST_CHECK(!p.first);
	BOOST_CHECK(p.second == s.begin());
}

// const_iterator search(Key const& key) const
BOOST_AUTO_TEST_CASE(trie_base_search_1)
{
	test_trie trie{};

	std::vector<std::string> v{ "a", "ac", "b", "cab", "cd" };
	trie.assign(v.begin(), v.end());

	auto it = trie.search(std::string("a"));

	BOOST_CHECK(it);
	BOOST_CHECK(*it == 'a');
}

BOOST_AUTO_TEST_CASE(trie_base_search_2)
{
	test_trie trie{};

	std::vector<std::string> v{ "a", "ac", "b", "cab", "cd" };
	trie.assign(v.begin(), v.end());

	auto it = trie.search(std::string(""));

	BOOST_CHECK(!it);
	BOOST_CHECK(it == trie.cbegin());
}

// const_iterator find(Key const& key) const
BOOST_AUTO_TEST_CASE(trie_base_find_1)
{
	test_trie trie{};

	std::vector<std::string> v{ "a", "ac", "b", "cab", "cd" };
	trie.assign(v.begin(), v.end());

	auto it = trie.find(std::string("a"));

	BOOST_CHECK(it);
	BOOST_CHECK(*it == 'a');
}

BOOST_AUTO_TEST_CASE(trie_base_find_2)
{
	test_trie trie{};

	std::vector<std::string> v{ "a", "ac", "b", "cab", "cd" };
	trie.assign(v.begin(), v.end());

	auto it = trie.find(std::string("ac"));

	BOOST_CHECK(it);
	BOOST_CHECK(*it == 'c');
}

BOOST_AUTO_TEST_CASE(trie_base_find_3)
{
	test_trie trie{};

	std::vector<std::string> v{ "a", "ac", "b", "cab", "cd" };
	trie.assign(v.begin(), v.end());

	auto it = trie.find(std::string(""));

	BOOST_CHECK(!it);
	BOOST_CHECK(it == trie.cend());
}

BOOST_AUTO_TEST_CASE(trie_base_find_4)
{
	test_trie trie{};

	std::vector<std::string> v{};
	trie.assign(v.begin(), v.end());

	auto it = trie.find(std::string(""));

	BOOST_CHECK(!it);
	BOOST_CHECK(it == trie.cend());
}

BOOST_AUTO_TEST_CASE(trie_base_find_5)
{
	test_trie trie{};

	std::vector<std::string> v{ "a", "ac", "b", "cab", "cd" };
	trie.assign(v.begin(), v.end());

	auto it = trie.find(std::string("ab"));

	BOOST_CHECK(!it);
	BOOST_CHECK(it == trie.cend());
}

// bool contains(Key const& key) const
BOOST_AUTO_TEST_CASE(trie_base_contains_1)
{
	test_trie trie{};

	std::vector<std::string> v{ "a", "ac", "b", "cab", "cd" };
	trie.assign(v.begin(), v.end());

	BOOST_CHECK(trie.contains(std::string("a")));
}

BOOST_AUTO_TEST_CASE(trie_base_contains_2)
{
	test_trie trie{};

	std::vector<std::string> v{ "a", "ac", "b", "cab", "cd" };
	trie.assign(v.begin(), v.end());

	BOOST_CHECK(trie.contains(std::string("ac")));
}

BOOST_AUTO_TEST_CASE(trie_base_contains_3)
{
	test_trie trie{};

	std::vector<std::string> v{ "a", "ac", "b", "cab", "cd" };
	trie.assign(v.begin(), v.end());

	BOOST_CHECK(trie.contains(std::string("cab")));
	BOOST_CHECK(trie.contains(std::string("cd")));
}

BOOST_AUTO_TEST_CASE(trie_base_contains_4)
{
	test_trie trie{};

	std::vector<std::string> v{ "a", "ac", "b", "cab", "cd" };
	trie.assign(v.begin(), v.end());

	BOOST_CHECK(trie.contains(std::string("")) == false);
}

BOOST_AUTO_TEST_CASE(trie_base_contains_5)
{
	test_trie trie{};

	std::vector<std::string> v{ "a", "ac", "b", "cab", "cd" };
	trie.assign(v.begin(), v.end());

	BOOST_CHECK(trie.contains(std::string("ab")) == false);
	BOOST_CHECK(trie.contains(std::string("cb")) == false);
	BOOST_CHECK(trie.contains(std::string("caba")) == false);
	BOOST_CHECK(trie.contains(std::string("cda")) == false);
}

// 内部 -----------------------------------------------------------------------

// bool is_tail(index_type idx) const
BOOST_AUTO_TEST_CASE(trie_base_is_tail_1)
{
	test_trie trie{};

	std::vector<std::string> v{ "a", "ac", "b", "cab", "cd" };
	trie.assign(v.begin(), v.end());

	std::int32_t i = trie.at(1, 'a');
	BOOST_CHECK(trie.is_tail(i));
}

BOOST_AUTO_TEST_CASE(trie_base_is_tail_2)
{
	test_trie trie{};

	std::vector<std::string> v{ "a", "ac", "b", "cab", "cd" };
	trie.assign(v.begin(), v.end());

	std::int32_t i = trie.at(1, 'a');
	i = trie.at(i, 'c');
	BOOST_CHECK(trie.is_tail(i));
}

BOOST_AUTO_TEST_CASE(trie_base_is_tail_3)
{
	test_trie trie{};

	std::vector<std::string> v{ "a", "ac", "b", "cab", "cd" };
	trie.assign(v.begin(), v.end());

	std::int32_t i = trie.at(1, 'c');
	BOOST_CHECK(trie.is_tail(i) == false);
}

BOOST_AUTO_TEST_CASE(trie_base_is_tail_4)
{
	test_trie trie{};

	std::vector<std::string> v{ "a", "ac", "b", "cab", "cd" };
	trie.assign(v.begin(), v.end());

	BOOST_CHECK(trie.is_tail(1) == false);
}

BOOST_AUTO_TEST_CASE(trie_base_is_tail_5)
{
	test_trie trie{};

	BOOST_CHECK(trie.is_tail(1) == false);
}

// bool has_child(index_type parent) const
BOOST_AUTO_TEST_CASE(trie_base_has_child_1)
{
	test_trie trie{};

	std::vector<std::string> v{ "a", "ac", "b", "cab", "cd" };
	trie.assign(v.begin(), v.end());

	std::int32_t i = trie.at(1, 'a');
	BOOST_CHECK(trie.has_child(i));
}

BOOST_AUTO_TEST_CASE(trie_base_has_child_2)
{
	test_trie trie{};

	std::vector<std::string> v{ "a", "ac", "b", "cab", "cd" };
	trie.assign(v.begin(), v.end());

	std::int32_t i = trie.at(1, 'b');
	BOOST_CHECK(trie.has_child(i) == false);
}

// bool has_null(index_type parent) const
BOOST_AUTO_TEST_CASE(trie_base_has_null_1)
{
	test_trie trie{};

	std::vector<std::string> v{ "a", "ac", "b", "cab", "cd" };
	trie.assign(v.begin(), v.end());

	std::int32_t i = trie.at(1, 'a');
	BOOST_CHECK(trie.has_null(i));
}

BOOST_AUTO_TEST_CASE(trie_base_has_null_2)
{
	test_trie trie{};

	std::vector<std::string> v{ "a", "ac", "b", "cab", "cd" };
	trie.assign(v.begin(), v.end());

	std::int32_t i = trie.at(1, 'b');
	BOOST_CHECK(trie.has_null(i) == false);
}

BOOST_AUTO_TEST_CASE(trie_base_has_null_3)
{
	test_trie trie{};

	std::vector<std::string> v{ "a", "ac", "b", "cab", "cd" };
	trie.assign(v.begin(), v.end());

	std::int32_t i = trie.at(1, 'c');
	i = trie.at(i, 'a');
	BOOST_CHECK(trie.has_null(i) == false);
}

// bool has_sibling(index_type idx) const
BOOST_AUTO_TEST_CASE(trie_base_has_sibling_1)
{
	test_trie trie{};

	std::vector<std::string> v{ "a", "ac", "b", "cab", "cd" };
	trie.assign(v.begin(), v.end());

	std::int32_t i = trie.at(1, 'a');
	BOOST_CHECK(trie.has_sibling(i));
}

BOOST_AUTO_TEST_CASE(trie_base_has_sibling_2)
{
	test_trie trie{};

	std::vector<std::string> v{ "a", "ac", "b", "cab", "cd" };
	trie.assign(v.begin(), v.end());

	std::int32_t i = trie.at(1, 'a');
	i = trie.at(i, 'c');
	BOOST_CHECK(trie.has_sibling(i) == false);
}

// index_type at(index_type parent, std::uint16_t label) const
BOOST_AUTO_TEST_CASE(trie_base_at_2_1)
{
	test_trie trie{};

	std::vector<std::string> v{ "a", "ac", "b", "cab", "cd" };
	trie.assign(v.begin(), v.end());

	std::int32_t i = trie.at(1, 'a');
	auto it = test_iterator(trie.m_c, i);

	BOOST_CHECK(it);
	BOOST_CHECK(*it == 'a');
}

// index_type add(index_type parent, std::uint16_t label)
BOOST_AUTO_TEST_CASE(trie_base_add_1)
{
	using namespace wordring;
	test_trie trie{};

	trie.add(1, 0);

	std::vector<detail::trie_node> v{ { 0, 0 }, { 2, 0 }, { 0, 1 } };
	BOOST_CHECK(trie.m_c == v);
}

// index_type add(index_type parent, label_vector const& labels)
BOOST_AUTO_TEST_CASE(trie_base_add_2)
{
	using namespace wordring;
	test_trie trie{};

	trie.add(1, { 0, 1, 2 });

	std::vector<detail::trie_node> v{ { 0, 0 }, { 2, 0 }, { 0, 1 }, { 0, 1 }, { 0, 1 } };
	BOOST_CHECK(trie.m_c == v);
}

BOOST_AUTO_TEST_CASE(trie_base_add_3)
{
	using namespace wordring;
	test_trie trie{};

	trie.add(1, { 1, 2, 3 });

	std::vector<detail::trie_node> v{ { 0, 0 }, { 1, 0 }, { 0, 1 }, { 0, 1 }, { 0, 1 } };
	BOOST_CHECK(trie.m_c == v);
}

// 関数 -----------------------------------------------------------------------

// inline std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, trie_heap<Allocator1> const& heap)
// inline std::basic_istream<char>& operator>>(std::basic_istream<char>& is, trie_heap<Allocator1>& heap)
BOOST_AUTO_TEST_CASE(trie_base_stream_1)
{
	test_trie t1{}, t2{};
	std::vector<std::string> v{ "a", "ac", "b", "cab", "cd" };
	t1.assign(v.begin(), v.end());

	std::stringstream ss;
	ss << t1;
	ss >> t2;

	BOOST_CHECK(t1.m_c == t2.m_c);
}

BOOST_AUTO_TEST_CASE(trie_base_stream_2)
{
	test_trie t1{}, t2{};

	std::stringstream ss;
	ss << t1;
	ss >> t2;

	BOOST_CHECK(t1.m_c == t2.m_c);
}

// 文書用状態プリント ----------------------------------------------------------

BOOST_AUTO_TEST_CASE(trie_base_states_1)
{
	test_trie trie{};
	std::vector<std::string> v{ "a", "ac", "b", "cab", "cd" };
	trie.assign(v.begin(), v.end());

	std::cout << "index\tbase\tcheck" << std::endl;
	for (std::uint32_t i = 0; i < trie.m_c.size(); ++i)
	{
		base_node const& node = trie.m_c[i];
		if (1 == i || 1 <= node.m_check)
		{
			std::cout << i << ",\t" << node.m_base << ",\t" << node.m_check << std::endl;
		}
	}
}

BOOST_AUTO_TEST_SUITE_END()
