// test/trie/trie_base.cpp

#include <boost/test/unit_test.hpp>

//#include <wordring/trie/list_trie.hpp>
#include <wordring/trie/trie_base.hpp>
#include <wordring/tree/tree_iterator.hpp>

#include <algorithm>
#include <fstream>
#include <random>
#include <sstream>
#include <set>
#include <string>
#include <vector>

#define STRING(str) #str
#define TO_STRING(str) STRING(str)

std::string const english_words_path{ TO_STRING(ENGLISH_WORDS_PATH) };

namespace wordring
{
	inline bool operator==(wordring::trie_node const& lhs, wordring::trie_node const& rhs)
	{
		return lhs.m_base == rhs.m_base && lhs.m_check == rhs.m_check;
	}
}

BOOST_AUTO_TEST_SUITE(trie_base__test)

using base_iterator = wordring::const_trie_base_iterator<std::vector<wordring::trie_node, std::allocator<wordring::trie_node>> const>;

class test_iterator : public base_iterator
{
public:
	using base_iterator::index_type;

	using base_iterator::find;
	using base_iterator::mother;
	using base_iterator::base;
	using base_iterator::tail;
	using base_iterator::has_null;
	using base_iterator::has_sibling;

	using base_iterator::m_c;
	using base_iterator::m_index;

public:
	test_iterator() : const_trie_base_iterator() {}

	test_iterator(container& c, index_type index)
		: const_trie_base_iterator(c, index) {}

	test_iterator(const_trie_base_iterator const& base)
		: const_trie_base_iterator(base) {}
};

class test_trie_base : public wordring::trie_base<>
{
public:
	using wordring::trie_base<>::container;

	using wordring::trie_base<>::reserve;
	using wordring::trie_base<>::free;
	using wordring::trie_base<>::is_free;
	using wordring::trie_base<>::allocate;
	using wordring::trie_base<>::add;

	using wordring::trie_base<>::m_c;

	/*! 状態数を数える
	*/
	std::size_t count_s() const
	{
		std::size_t n = 1; // 状態1のCHECKは常に0なので
		for (std::size_t i = 1; i < m_c.size(); ++i) if (0 < (m_c.data() + i)->m_check) ++n;
		return n;
	}

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

// ----------------------------------------------------------------------------
// const_trie_base_iterator
// ----------------------------------------------------------------------------

// const_trie_base_iterator()
BOOST_AUTO_TEST_CASE(const_trie_base_iterator__construct__1)
{
	test_iterator it;
}

BOOST_AUTO_TEST_CASE(const_trie_base_iterator__construct__2)
{
	test_iterator it{};
}

// const_trie_base_iterator(container& c, index_type index)
BOOST_AUTO_TEST_CASE(const_trie_base_iterator__construct__3)
{
	test_trie_base trie{};
	test_iterator it(trie.m_c, 1);
}

// operator bool() const
BOOST_AUTO_TEST_CASE(const_trie_base_iterator__bool__1)
{
	test_trie_base trie{};
	trie.m_c.clear();
	trie.m_c.emplace_back(0, 0);  // 0
	trie.m_c.emplace_back(1, 0);  // 1
	trie.m_c.emplace_back(5, 1);  // 2
	trie.m_c.emplace_back(-3, 1); // 3
	trie.m_c.emplace_back(5, 1);  // 4
	trie.m_c.emplace_back(0, 0);  // 5
	trie.m_c.emplace_back(5, 4);  // 6
	trie.m_c.emplace_back(-4, 6); // 7
	trie.m_c.emplace_back(-2, 2); // 8
	trie.m_c.emplace_back(-5, 4); // 9

	// 親状態2に対してnull_valueによる遷移を追加
	trie.m_c.insert(trie.m_c.end(), 252, { 0, 0 });
	trie.m_c[261] = wordring::trie_node{ 0, 2 };

	BOOST_CHECK(test_iterator(trie.m_c, 1) == false);
	BOOST_CHECK(test_iterator(trie.m_c, 2) == true);
	BOOST_CHECK(test_iterator(trie.m_c, 3) == true);
	BOOST_CHECK(test_iterator(trie.m_c, 4) == false);
	BOOST_CHECK(test_iterator(trie.m_c, 5) == true);
	BOOST_CHECK(test_iterator(trie.m_c, 6) == false);
	BOOST_CHECK(test_iterator(trie.m_c, 7) == true);
	BOOST_CHECK(test_iterator(trie.m_c, 8) == true);
	BOOST_CHECK(test_iterator(trie.m_c, 9) == true);
}

BOOST_AUTO_TEST_CASE(const_trie_base_iterator__bool__2)
{
	test_trie_base trie{};
	trie.insert(std::string("a"));
	BOOST_CHECK(trie.count() == 1);
	BOOST_CHECK(trie.find(std::string("a")).operator bool());
	BOOST_CHECK(trie.find(std::string("a")).operator!() == false);

	BOOST_CHECK(trie.find(std::string("c")).operator bool() == false);
}

BOOST_AUTO_TEST_CASE(const_trie_base_iterator__bool__3)
{
	test_trie_base trie{};
	trie.insert(std::string("a"));
	trie.insert(std::string("ab"));
	BOOST_CHECK(trie.count() == 2);
	BOOST_CHECK(trie.find(std::string("a")).operator bool());
	BOOST_CHECK(trie.find(std::string("ab")).operator bool());
	BOOST_CHECK(trie.find(std::string("a")).operator!() == false);
	BOOST_CHECK(trie.find(std::string("ab")).operator!() == false);

	BOOST_CHECK(trie.find(std::string("c")).operator bool() == false);
}

BOOST_AUTO_TEST_CASE(const_trie_base_iterator__bool__4)
{
	test_trie_base trie{};
	trie.insert(std::string("ab"));
	trie.insert(std::string("a"));
	BOOST_CHECK(trie.count() == 2);
	BOOST_CHECK(trie.find(std::string("a")).operator bool());
	BOOST_CHECK(trie.find(std::string("ab")).operator bool());
	BOOST_CHECK(trie.find(std::string("a")).operator!() == false);
	BOOST_CHECK(trie.find(std::string("ab")).operator!() == false);

	BOOST_CHECK(trie.find(std::string("c")).operator bool() == false);
}

// value_type operator*() const
BOOST_AUTO_TEST_CASE(const_trie_base_iterator__reference__1)
{
	test_trie_base trie{};
	trie.m_c.clear();
	trie.m_c.emplace_back(0, 0);  // 0
	trie.m_c.emplace_back(1, 0);  // 1
	trie.m_c.emplace_back(5, 1);  // 2
	trie.m_c.emplace_back(-3, 1); // 3
	trie.m_c.emplace_back(5, 1);  // 4
	trie.m_c.emplace_back(-1, 2); // 5
	trie.m_c.emplace_back(5, 4);  // 6
	trie.m_c.emplace_back(-4, 6); // 7
	trie.m_c.emplace_back(-2, 2); // 8
	trie.m_c.emplace_back(-5, 4); // 9

	BOOST_CHECK(*test_iterator(trie.m_c, 2) == 1);
	BOOST_CHECK(*test_iterator(trie.m_c, 3) == 2);
	BOOST_CHECK(*test_iterator(trie.m_c, 4) == 3);
	BOOST_CHECK(*test_iterator(trie.m_c, 5) == 0);
	BOOST_CHECK(*test_iterator(trie.m_c, 6) == 1);
	BOOST_CHECK(*test_iterator(trie.m_c, 7) == 2);
	BOOST_CHECK(*test_iterator(trie.m_c, 8) == 3);
	BOOST_CHECK(*test_iterator(trie.m_c, 9) == 4);
}

// const_trie_base_iterator operator[](std::uint16_t label) const
BOOST_AUTO_TEST_CASE(const_trie_base_iterator__at__1)
{
	test_trie_base trie{};
	trie.m_c.clear();
	trie.m_c.emplace_back(0, 0);  // 0
	trie.m_c.emplace_back(1, 0);  // 1
	trie.m_c.emplace_back(5, 1);  // 2
	trie.m_c.emplace_back(-3, 1); // 3
	trie.m_c.emplace_back(5, 1);  // 4
	trie.m_c.emplace_back(-1, 2); // 5
	trie.m_c.emplace_back(5, 4);  // 6
	trie.m_c.emplace_back(-4, 6); // 7
	trie.m_c.emplace_back(-2, 2); // 8
	trie.m_c.emplace_back(-5, 4); // 9

	test_iterator it2 = test_iterator(trie.m_c, 1)[1];
	BOOST_CHECK(it2.m_index == 2);

	test_iterator it3 = test_iterator(trie.m_c, 1)[2];
	BOOST_CHECK(it3.m_index == 3);

	test_iterator it4 = test_iterator(trie.m_c, 1)[3];
	BOOST_CHECK(it4.m_index == 4);

	test_iterator it5 = test_iterator(trie.m_c, 2)[0];
	BOOST_CHECK(it5.m_index == 5);

	test_iterator it8 = test_iterator(trie.m_c, 2)[3];
	BOOST_CHECK(it8.m_index == 8);

	test_iterator it6 = test_iterator(trie.m_c, 4)[1];
	BOOST_CHECK(it6.m_index == 6);

	test_iterator it9 = test_iterator(trie.m_c, 4)[4];
	BOOST_CHECK(it9.m_index == 9);

	test_iterator it7 = test_iterator(trie.m_c, 6)[2];
	BOOST_CHECK(it7.m_index == 7);
}

BOOST_AUTO_TEST_CASE(const_trie_base_iterator__at__2)
{
	test_trie_base trie{};
	trie.m_c.clear();
	trie.m_c.emplace_back(0, 0);  // 0
	trie.m_c.emplace_back(1, 0);  // 1
	trie.m_c.emplace_back(5, 1);  // 2
	trie.m_c.emplace_back(-3, 1); // 3
	trie.m_c.emplace_back(5, 1);  // 4
	trie.m_c.emplace_back(-1, 2); // 5
	trie.m_c.emplace_back(5, 4);  // 6
	trie.m_c.emplace_back(-4, 6); // 7
	trie.m_c.emplace_back(-2, 2); // 8
	trie.m_c.emplace_back(-5, 4); // 9

	BOOST_CHECK(test_iterator(trie.m_c, 1)[0] == test_iterator(trie.m_c, 1).end());
	BOOST_CHECK(test_iterator(trie.m_c, 1)[4] == test_iterator(trie.m_c, 1).end());

	BOOST_CHECK(test_iterator(trie.m_c, 2)[4] == test_iterator(trie.m_c, 2).end());

	BOOST_CHECK(test_iterator(trie.m_c, 3)[0] == test_iterator(trie.m_c, 3).end());
	BOOST_CHECK(test_iterator(trie.m_c, 3)[1] == test_iterator(trie.m_c, 3).end());

	BOOST_CHECK(test_iterator(trie.m_c, 4)[0] == test_iterator(trie.m_c, 4).end());
	BOOST_CHECK(test_iterator(trie.m_c, 4)[2] == test_iterator(trie.m_c, 4).end());

	BOOST_CHECK(test_iterator(trie.m_c, 5)[0] == test_iterator(trie.m_c, 5).end());
	BOOST_CHECK(test_iterator(trie.m_c, 5)[1] == test_iterator(trie.m_c, 5).end());

	BOOST_CHECK(test_iterator(trie.m_c, 8)[0] == test_iterator(trie.m_c, 8).end());
	BOOST_CHECK(test_iterator(trie.m_c, 8)[1] == test_iterator(trie.m_c, 8).end());

	BOOST_CHECK(test_iterator(trie.m_c, 6)[0] == test_iterator(trie.m_c, 6).end());
	BOOST_CHECK(test_iterator(trie.m_c, 6)[1] == test_iterator(trie.m_c, 6).end());

	BOOST_CHECK(test_iterator(trie.m_c, 9)[0] == test_iterator(trie.m_c, 9).end());
	BOOST_CHECK(test_iterator(trie.m_c, 9)[1] == test_iterator(trie.m_c, 9).end());

	BOOST_CHECK(test_iterator(trie.m_c, 7)[0] == test_iterator(trie.m_c, 7).end());
	BOOST_CHECK(test_iterator(trie.m_c, 7)[1] == test_iterator(trie.m_c, 7).end());
}

// const_trie_base_iterator& operator++()
BOOST_AUTO_TEST_CASE(const_trie_base_iterator__increment__1)
{
	test_trie_base trie{};
	trie.m_c.clear();
	trie.m_c.emplace_back(0, 0);  // 0
	trie.m_c.emplace_back(1, 0);  // 1
	trie.m_c.emplace_back(5, 1);  // 2
	trie.m_c.emplace_back(-3, 1); // 3
	trie.m_c.emplace_back(5, 1);  // 4
	trie.m_c.emplace_back(-1, 2); // 5
	trie.m_c.emplace_back(5, 4);  // 6
	trie.m_c.emplace_back(-4, 6); // 7
	trie.m_c.emplace_back(-2, 2); // 8
	trie.m_c.emplace_back(-5, 4); // 9

	BOOST_CHECK(*++test_iterator(trie.m_c, 2) == 2);
	BOOST_CHECK(*++test_iterator(trie.m_c, 3) == 3);
	BOOST_CHECK(*++test_iterator(trie.m_c, 5) == 3);
	BOOST_CHECK(*++test_iterator(trie.m_c, 6) == 4);
}

BOOST_AUTO_TEST_CASE(const_trie_base_iterator__increment__2)
{
	test_trie_base trie{};
	trie.m_c.clear();
	trie.m_c.emplace_back(0, 0);  // 0
	trie.m_c.emplace_back(1, 0);  // 1
	trie.m_c.emplace_back(5, 1);  // 2
	trie.m_c.emplace_back(-3, 1); // 3
	trie.m_c.emplace_back(5, 1);  // 4
	trie.m_c.emplace_back(-1, 2); // 5
	trie.m_c.emplace_back(5, 4);  // 6
	trie.m_c.emplace_back(-4, 6); // 7
	trie.m_c.emplace_back(-2, 2); // 8
	trie.m_c.emplace_back(-5, 4); // 9

	test_iterator it1 = ++test_iterator(trie.m_c, 4);
	BOOST_CHECK(it1.m_index == 0);

	test_iterator it2 = ++test_iterator(trie.m_c, 8);
	BOOST_CHECK(it2.m_index == 0);

	test_iterator it3 = ++test_iterator(trie.m_c, 9);
	BOOST_CHECK(it3.m_index == 0);
}

// const_trie_base_iterator operator++(int)
BOOST_AUTO_TEST_CASE(const_trie_base_iterator__increment__3)
{
	test_trie_base trie{};
	trie.m_c.clear();
	trie.m_c.emplace_back(0, 0);  // 0
	trie.m_c.emplace_back(1, 0);  // 1
	trie.m_c.emplace_back(5, 1);  // 2
	trie.m_c.emplace_back(-3, 1); // 3
	trie.m_c.emplace_back(5, 1);  // 4
	trie.m_c.emplace_back(-1, 2); // 5
	trie.m_c.emplace_back(5, 4);  // 6
	trie.m_c.emplace_back(-4, 6); // 7
	trie.m_c.emplace_back(-2, 2); // 8
	trie.m_c.emplace_back(-5, 4); // 9

	auto it2 = test_iterator(trie.m_c, 2);
	BOOST_CHECK(*it2++ == 1); // a
	BOOST_CHECK(*it2 == 2);   // b

	BOOST_CHECK(*it2++ == 2); // b
	BOOST_CHECK(*it2 == 3);   // c

	BOOST_CHECK(*it2++ == 3); // c

	auto it5 = test_iterator(trie.m_c, 5);
	BOOST_CHECK(*it5++ == 0); // #
	BOOST_CHECK(*it5 == 3);   // c

	BOOST_CHECK(*it5++ == 3); // c

	auto it6 = test_iterator(trie.m_c, 6);
	BOOST_CHECK(*it6++ == 1); // a
	BOOST_CHECK(*it6 == 4);   // d

	BOOST_CHECK(*it6++ == 4); // d
}

BOOST_AUTO_TEST_CASE(const_trie_base_iterator__increment__4)
{
	test_trie_base trie{};
	trie.m_c.clear();
	trie.m_c.emplace_back(0, 0);  // 0
	trie.m_c.emplace_back(1, 0);  // 1
	trie.m_c.emplace_back(5, 1);  // 2
	trie.m_c.emplace_back(-3, 1); // 3
	trie.m_c.emplace_back(5, 1);  // 4
	trie.m_c.emplace_back(-1, 2); // 5
	trie.m_c.emplace_back(5, 4);  // 6
	trie.m_c.emplace_back(-4, 6); // 7
	trie.m_c.emplace_back(-2, 2); // 8
	trie.m_c.emplace_back(-5, 4); // 9

	auto it4 = test_iterator(trie.m_c, 4);
	BOOST_CHECK(*it4++ == 3);
	BOOST_CHECK(it4.m_index == 0);

	auto it8 = test_iterator(trie.m_c, 8);
	BOOST_CHECK(*it8++ == 3);
	BOOST_CHECK(it8.m_index == 0);

	auto it9 = test_iterator(trie.m_c, 9);
	BOOST_CHECK(*it9++ == 4);
	BOOST_CHECK(it9.m_index == 0);
}

// String string() const
BOOST_AUTO_TEST_CASE(const_trie_base_iterator__string__1)
{
	test_trie_base trie{};

	test_iterator it0{};
	BOOST_CHECK(it0.string<std::string>() == "");

	test_iterator it1 = trie.insert(std::string("\1"));
	BOOST_CHECK(it1.string<std::string>() == "\1");

	test_iterator it2 = trie.insert(std::string("\1\3"));
	BOOST_CHECK(it2.string<std::string>() == "\1\3");

	test_iterator it3 = trie.insert(std::string("\2"));
	BOOST_CHECK(it3.string<std::string>() == "\2");

	test_iterator it4 = trie.insert(std::string("\3\1\2"));
	BOOST_CHECK(it4.string<std::string>() == "\3\1\2");

	test_iterator it5 = trie.insert(std::string("\3\4"));
	BOOST_CHECK(it5.string<std::string>() == "\3\4");
}

// std::optional<const_trie_base_iterator> parent() const
BOOST_AUTO_TEST_CASE(const_trie_base_iterator__parent__1)
{
	test_trie_base trie{};

	test_iterator it1 = trie.insert(std::string("\1"));
	BOOST_CHECK(it1.parent()->parent().has_value() == false);

	test_iterator it2 = trie.insert(std::string("\1\3"));
	BOOST_CHECK(*it2.parent().value() == 1);
	
	test_iterator it3 = trie.insert(std::string("\2"));
	BOOST_CHECK(it3.parent()->parent().has_value() == false);
	
	test_iterator it4 = trie.insert(std::string("\3\1\2"));
	BOOST_CHECK(*it4.parent().value() == 1);
	BOOST_CHECK(*it4.parent()->parent().value() == 3);

	test_iterator it5 = trie.insert(std::string("\3\4"));
	BOOST_CHECK(*it5.parent().value() == 3);
	BOOST_CHECK(it5.parent()->parent()->parent().has_value() == false);
}


// const_trie_base_iterator begin() const
BOOST_AUTO_TEST_CASE(const_trie_base_iterator__begin__1)
{
	test_trie_base trie{};
	trie.m_c.clear();
	trie.m_c.emplace_back(0, 0);  // 0
	trie.m_c.emplace_back(1, 0);  // 1
	trie.m_c.emplace_back(5, 1);  // 2
	trie.m_c.emplace_back(-3, 1); // 3
	trie.m_c.emplace_back(5, 1);  // 4
	trie.m_c.emplace_back(-1, 2); // 5
	trie.m_c.emplace_back(5, 4);  // 6
	trie.m_c.emplace_back(-4, 6); // 7
	trie.m_c.emplace_back(-2, 2); // 8
	trie.m_c.emplace_back(-5, 4); // 9

	test_iterator it1 = test_iterator(trie.m_c, 1).begin();
	BOOST_CHECK(it1.m_index == 1 + 1);

	test_iterator it2 = test_iterator(trie.m_c, 2).begin();
	BOOST_CHECK(it2.m_index == 5 + 0);

	test_iterator it4 = test_iterator(trie.m_c, 4).begin();
	BOOST_CHECK(it4.m_index == 5 + 1);

	test_iterator it6 = test_iterator(trie.m_c, 6).begin();
	BOOST_CHECK(it6.m_index == 5 + 2);
}

BOOST_AUTO_TEST_CASE(const_trie_base_iterator__begin__2)
{
	test_trie_base trie{};
	trie.m_c.clear();
	trie.m_c.emplace_back(0, 0);  // 0
	trie.m_c.emplace_back(1, 0);  // 1
	trie.m_c.emplace_back(5, 1);  // 2
	trie.m_c.emplace_back(-3, 1); // 3
	trie.m_c.emplace_back(5, 1);  // 4
	trie.m_c.emplace_back(-1, 2); // 5
	trie.m_c.emplace_back(5, 4);  // 6
	trie.m_c.emplace_back(-4, 6); // 7
	trie.m_c.emplace_back(-2, 2); // 8
	trie.m_c.emplace_back(-5, 4); // 9

	test_iterator it5 = test_iterator(trie.m_c, 5).begin();
	BOOST_CHECK(it5.m_index == 0);

	test_iterator it8 = test_iterator(trie.m_c, 8).begin();
	BOOST_CHECK(it8.m_index == 0);

	test_iterator it3 = test_iterator(trie.m_c, 3).begin();
	BOOST_CHECK(it3.m_index == 0);

	test_iterator it7 = test_iterator(trie.m_c, 7).begin();
	BOOST_CHECK(it7.m_index == 0);

	test_iterator it9 = test_iterator(trie.m_c, 9).begin();
	BOOST_CHECK(it9.m_index == 0);
}

// const_trie_base_iterator end() const
BOOST_AUTO_TEST_CASE(const_trie_base_iterator__end__1)
{
	test_trie_base trie{};
	trie.m_c.clear();
	trie.m_c.emplace_back(0, 0);  // 0
	trie.m_c.emplace_back(1, 0);  // 1
	trie.m_c.emplace_back(5, 1);  // 2
	trie.m_c.emplace_back(-3, 1); // 3
	trie.m_c.emplace_back(5, 1);  // 4
	trie.m_c.emplace_back(-1, 2); // 5
	trie.m_c.emplace_back(5, 4);  // 6
	trie.m_c.emplace_back(-4, 6); // 7
	trie.m_c.emplace_back(-2, 2); // 8
	trie.m_c.emplace_back(-5, 4); // 9

	test_iterator it1 = test_iterator(trie.m_c, 1).end();
	BOOST_CHECK(it1.m_index == 0);

	test_iterator it2 = test_iterator(trie.m_c, 2).end();
	BOOST_CHECK(it2.m_index == 0);

	test_iterator it4 = test_iterator(trie.m_c, 4).end();
	BOOST_CHECK(it4.m_index == 0);

	test_iterator it6 = test_iterator(trie.m_c, 6).end();
	BOOST_CHECK(it6.m_index == 0);
}

BOOST_AUTO_TEST_CASE(const_trie_base_iterator__end__2)
{
	test_trie_base trie{};
	trie.m_c.clear();
	trie.m_c.emplace_back(0, 0);  // 0
	trie.m_c.emplace_back(1, 0);  // 1
	trie.m_c.emplace_back(5, 1);  // 2
	trie.m_c.emplace_back(-3, 1); // 3
	trie.m_c.emplace_back(5, 1);  // 4
	trie.m_c.emplace_back(-1, 2); // 5
	trie.m_c.emplace_back(5, 4);  // 6
	trie.m_c.emplace_back(-4, 6); // 7
	trie.m_c.emplace_back(-2, 2); // 8
	trie.m_c.emplace_back(-5, 4); // 9

	test_iterator it5 = test_iterator(trie.m_c, 5).end();
	BOOST_CHECK(it5.m_index == 0);

	test_iterator it8 = test_iterator(trie.m_c, 8).end();
	BOOST_CHECK(it8.m_index == 0);

	test_iterator it3 = test_iterator(trie.m_c, 3).end();
	BOOST_CHECK(it3.m_index == 0);

	test_iterator it7 = test_iterator(trie.m_c, 7).end();
	BOOST_CHECK(it7.m_index == 0);

	test_iterator it9 = test_iterator(trie.m_c, 9).end();
	BOOST_CHECK(it9.m_index == 0);
}

// index_type find(index_type first, index_type last, index_type check)
BOOST_AUTO_TEST_CASE(const_trie_base_iterator__find__1)
{
	test_trie_base trie{};
	trie.m_c.clear();
	trie.m_c.emplace_back(0, 0);  // 0
	trie.m_c.emplace_back(1, 0);  // 1
	trie.m_c.emplace_back(5, 1);  // 2
	trie.m_c.emplace_back(-3, 1); // 3
	trie.m_c.emplace_back(5, 1);  // 4
	trie.m_c.emplace_back(-1, 2); // 5
	trie.m_c.emplace_back(5, 4);  // 6
	trie.m_c.emplace_back(-4, 6); // 7
	trie.m_c.emplace_back(-2, 2); // 8
	trie.m_c.emplace_back(-5, 4); // 9

	auto it1 = test_iterator(trie.m_c, 1);

	BOOST_CHECK(it1.find(1, 256, 1) == 2);
	BOOST_CHECK(it1.find(2, 256, 1) == 2);
	BOOST_CHECK(it1.find(3, 256, 1) == 3);
	BOOST_CHECK(it1.find(4, 256, 1) == 4);
	BOOST_CHECK(it1.find(5, 256, 1) == 0);
}

// index_type mother() const
BOOST_AUTO_TEST_CASE(const_trie_base_iterator__mother__1)
{
	test_trie_base trie{};
	trie.m_c.clear();
	trie.m_c.emplace_back(0, 0);  // 0
	trie.m_c.emplace_back(1, 0);  // 1
	trie.m_c.emplace_back(5, -1); // 2
	trie.m_c.emplace_back(5, 1);  // 3

	BOOST_CHECK(test_iterator(trie.m_c, 1).mother() == 0);
	BOOST_CHECK(test_iterator(trie.m_c, 2).mother() == 0);
	BOOST_CHECK(test_iterator(trie.m_c, 3).mother() == 1);
}

// index_type base() const
BOOST_AUTO_TEST_CASE(const_trie_base_iterator__base__1)
{
	test_trie_base trie{};
	trie.m_c.clear();
	trie.m_c.emplace_back(0, 0);  // 0
	trie.m_c.emplace_back(1, 0);  // 1
	trie.m_c.emplace_back(5, 1);  // 2
	trie.m_c.emplace_back(-3, 1); // 3
	trie.m_c.emplace_back(5, 1);  // 4
	trie.m_c.emplace_back(-1, 2); // 5
	trie.m_c.emplace_back(5, 4);  // 6
	trie.m_c.emplace_back(-4, 6); // 7
	trie.m_c.emplace_back(-2, 2); // 8
	trie.m_c.emplace_back(-5, 4); // 9

	BOOST_CHECK(test_iterator(trie.m_c, 2).base() == 1);
	BOOST_CHECK(test_iterator(trie.m_c, 3).base() == 1);
	BOOST_CHECK(test_iterator(trie.m_c, 4).base() == 1);
	BOOST_CHECK(test_iterator(trie.m_c, 5).base() == 5);
	BOOST_CHECK(test_iterator(trie.m_c, 6).base() == 5);
	BOOST_CHECK(test_iterator(trie.m_c, 7).base() == 5);
	BOOST_CHECK(test_iterator(trie.m_c, 8).base() == 5);
	BOOST_CHECK(test_iterator(trie.m_c, 9).base() == 5);
}

// index_type tail() const
BOOST_AUTO_TEST_CASE(const_trie_base_iterator__tail__1)
{
	test_trie_base trie{};
	trie.m_c.clear();
	trie.m_c.emplace_back(0, 0);  // 0
	trie.m_c.emplace_back(1, 0);  // 1
	trie.m_c.emplace_back(5, 1);  // 2
	trie.m_c.emplace_back(-3, 1); // 3
	trie.m_c.emplace_back(5, 1);  // 4
	trie.m_c.emplace_back(-1, 2); // 5
	trie.m_c.emplace_back(5, 4);  // 6
	trie.m_c.emplace_back(-4, 6); // 7
	trie.m_c.emplace_back(-2, 2); // 8
	trie.m_c.emplace_back(-5, 4); // 9

	BOOST_CHECK(test_iterator(trie.m_c, 1).tail() == 10);
	BOOST_CHECK(test_iterator(trie.m_c, 2).tail() == 10);
	BOOST_CHECK(test_iterator(trie.m_c, 3).tail() == 10);
	BOOST_CHECK(test_iterator(trie.m_c, 4).tail() == 10);
	BOOST_CHECK(test_iterator(trie.m_c, 5).tail() == 10);
	BOOST_CHECK(test_iterator(trie.m_c, 6).tail() == 10);
	BOOST_CHECK(test_iterator(trie.m_c, 7).tail() == 10);
	BOOST_CHECK(test_iterator(trie.m_c, 8).tail() == 10);
	BOOST_CHECK(test_iterator(trie.m_c, 9).tail() == 10);
}

// bool has_null() const
BOOST_AUTO_TEST_CASE(const_trie_base_iterator__has_null__1)
{
	test_trie_base trie{};
	trie.m_c.clear();
	trie.m_c.emplace_back(0, 0); // 0
	trie.m_c.emplace_back(1, 0); // 1
	trie.m_c.emplace_back(1, 1); // 2
	trie.m_c.emplace_back(0, 0); // 3
	trie.m_c.emplace_back(0, 2); // 4
	trie.m_c.insert(trie.m_c.cend(), 252, { 0, 0 });
	trie.m_c.emplace_back(0, 2); // 257
	BOOST_CHECK(trie.m_c.size() == 258);

	BOOST_CHECK(test_iterator(trie.m_c, 1).has_null() == false);
	BOOST_CHECK(test_iterator(trie.m_c, 2).has_null());
	BOOST_CHECK(test_iterator(trie.m_c, 3).has_null() == false);
	BOOST_CHECK(test_iterator(trie.m_c, 4).has_null() == false);
	BOOST_CHECK(test_iterator(trie.m_c, 5).has_null() == false);
	BOOST_CHECK(test_iterator(trie.m_c, 257).has_null() == false);
}

// bool has_sibling() const
BOOST_AUTO_TEST_CASE(const_trie_base_iterator__has_sibling__1)
{
	test_trie_base trie{};

	trie.insert(std::string("a"));
	trie.insert(std::string("ac"));
	trie.insert(std::string("b"));
	trie.insert(std::string("cab"));
	trie.insert(std::string("cd"));

	test_iterator it1 = trie.search(std::string("a"));
	BOOST_CHECK(it1.has_sibling());

	test_iterator it2 = trie.search(std::string("b"));
	BOOST_CHECK(it2.has_sibling());

	test_iterator it3 = trie.search(std::string("c"));
	BOOST_CHECK(it3.has_sibling());
}

BOOST_AUTO_TEST_CASE(const_trie_base_iterator__has_sibling__2)
{
	test_trie_base trie{};

	trie.insert(std::string("a"));
	trie.insert(std::string("ac"));
	trie.insert(std::string("b"));
	trie.insert(std::string("cab"));
	trie.insert(std::string("cd"));

	test_iterator it1 = trie.search(std::string("cab"));
	BOOST_CHECK(it1.has_sibling() == false);
}

BOOST_AUTO_TEST_CASE(const_trie_base_iterator__has_sibling__3)
{
	test_trie_base trie{};

	trie.insert(std::string("a"));
	trie.insert(std::string("ac"));
	trie.insert(std::string("b"));
	trie.insert(std::string("cab"));
	trie.insert(std::string("cd"));

	test_iterator it1 = trie.search(std::string("ac"));
	BOOST_CHECK(it1.has_sibling() == false);
}

BOOST_AUTO_TEST_CASE(const_trie_base_iterator__has_sibling__4)
{
	test_trie_base trie{};

	trie.insert(std::string("a"));
	trie.insert(std::string("ac"));

	test_iterator it1 = trie.search(std::string("a"));
	BOOST_CHECK(it1.has_sibling() == false);

	test_iterator it2 = trie.search(std::string("ac"));
	BOOST_CHECK(it2.has_sibling() == false);
}

// inline bool operator==(const_trie_base_iterator<Container1> const& lhs, const_trie_base_iterator<Container1> const& rhs)
BOOST_AUTO_TEST_CASE(const_trie_base_iterator__equal__1)
{
	test_trie_base trie{};
	BOOST_CHECK(test_iterator(trie.m_c, 1) == test_iterator(trie.m_c, 1));
}

BOOST_AUTO_TEST_CASE(const_trie_base_iterator__equal__2)
{
	BOOST_CHECK(test_iterator() == test_iterator());
}

BOOST_AUTO_TEST_CASE(const_trie_base_iterator__equal__3)
{
	test_trie_base trie{};
	trie.m_c.clear();
	trie.m_c.emplace_back(0, 0);  // 0
	trie.m_c.emplace_back(1, 0);  // 1
	trie.m_c.emplace_back(5, 1);  // 2
	BOOST_CHECK((test_iterator(trie.m_c, 1) == test_iterator(trie.m_c, 2)) == false);
}

// inline bool operator!=(const_trie_base_iterator<Container1> const& lhs, const_trie_base_iterator<Container1> const& rhs)
BOOST_AUTO_TEST_CASE(const_trie_base_iterator__not_equal__1)
{
	test_trie_base trie{};
	BOOST_CHECK((test_iterator(trie.m_c, 1) != test_iterator(trie.m_c, 1)) == false);
}

BOOST_AUTO_TEST_CASE(const_trie_base_iterator__not_equal__2)
{
	BOOST_CHECK((test_iterator() != test_iterator()) == false);
}

BOOST_AUTO_TEST_CASE(const_trie_base_iterator__not_equal__3)
{
	test_trie_base trie{};
	trie.m_c.clear();
	trie.m_c.emplace_back(0, 0);  // 0
	trie.m_c.emplace_back(1, 0);  // 1
	trie.m_c.emplace_back(5, 1);  // 2
	BOOST_CHECK(test_iterator(trie.m_c, 1) != test_iterator(trie.m_c, 2));
}

// ----------------------------------------------------------------------------
// trie_base
// ----------------------------------------------------------------------------


BOOST_AUTO_TEST_CASE(trie_base__constrcut__1)
{
	using namespace wordring;
	trie_base trie{};
}

// 要素アクセス ----------------------------------------------------------------

// value_type at(Key const& key)
BOOST_AUTO_TEST_CASE(trie_base__at__1)
{
	test_trie_base trie{};
	trie.insert(std::string("a"));
	trie.insert(std::string("ac"));
	trie.insert(std::string("b"));
	trie.insert(std::string("cab"));
	trie.insert(std::string("cd"));

	auto v1 = trie.at(std::string("a"));
	BOOST_CHECK(v1 == 0);

	v1 = 10;

	auto v2 = trie.at(std::string("a"));
	BOOST_CHECK(v2 == 10);

	BOOST_CHECK(trie.count() == 5);
}

BOOST_AUTO_TEST_CASE(trie_base__at__2)
{
	test_trie_base trie{};
	trie.insert(std::string("a"));
	trie.insert(std::string("ac"));
	trie.insert(std::string("b"));
	trie.insert(std::string("cab"));
	trie.insert(std::string("cd"));

	auto v1 = trie.at(std::string("ac"));
	BOOST_CHECK(v1 == 0);

	v1 = 10;

	auto v2 = trie.at(std::string("ac"));
	BOOST_CHECK(v2 == 10);

	BOOST_CHECK(trie.count() == 5);
}

BOOST_AUTO_TEST_CASE(trie_base__at__3)
{
	test_trie_base trie{};
	trie.insert(std::string("a"));
	trie.insert(std::string("ac"));
	trie.insert(std::string("b"));
	trie.insert(std::string("cab"));
	trie.insert(std::string("cd"));

	BOOST_CHECK_THROW(trie.at(std::string("d")), std::out_of_range);
}

BOOST_AUTO_TEST_CASE(trie_base__at__4)
{
	test_trie_base trie{};
	trie.insert(std::string("a"));
	trie.insert(std::string("ac"));
	trie.insert(std::string("b"));
	trie.insert(std::string("cab"));
	trie.insert(std::string("cd"));

	BOOST_CHECK_THROW(trie.at(std::string("a")) = -1, std::invalid_argument);
}

// value_type const at(Key const& key) const
BOOST_AUTO_TEST_CASE(trie_base__at__5)
{
	test_trie_base trie{};
	trie.insert(std::string("a"));
	trie.insert(std::string("ac"));
	trie.insert(std::string("b"));
	trie.insert(std::string("cab"));
	trie.insert(std::string("cd"));

	auto const& t1 = trie;

	auto v1 = t1.at(std::string("a"));
	BOOST_CHECK(v1 == 0);
}

// value_type operator[](Key const& key)
BOOST_AUTO_TEST_CASE(trie_base__at__6)
{
	test_trie_base trie{};
	trie.insert(std::string("a"));
	trie.insert(std::string("ac"));
	trie.insert(std::string("b"));
	trie.insert(std::string("cab"));
	trie.insert(std::string("cd"));

	auto v1 = trie[std::string("a")];
	BOOST_CHECK(v1 == 0);

	v1 = 10;

	auto v2 = trie[std::string("a")];
	BOOST_CHECK(v2 == 10);

	BOOST_CHECK(trie.count() == 5);
}

BOOST_AUTO_TEST_CASE(trie_base__at__7)
{
	test_trie_base trie{};
	trie.insert(std::string("a"));
	trie.insert(std::string("ac"));
	trie.insert(std::string("b"));
	trie.insert(std::string("cab"));
	trie.insert(std::string("cd"));

	auto v1 = trie[std::string("ac")];
	BOOST_CHECK(v1 == 0);

	v1 = 10;

	auto v2 = trie[std::string("ac")];
	BOOST_CHECK(v2 == 10);

	BOOST_CHECK(trie.count() == 5);
}

BOOST_AUTO_TEST_CASE(trie_base__at__8)
{
	test_trie_base trie{};
	trie.insert(std::string("a"));
	trie.insert(std::string("ac"));
	trie.insert(std::string("b"));
	trie.insert(std::string("cab"));
	trie.insert(std::string("cd"));

	BOOST_CHECK(trie.count() == 5);

	auto v1 = trie[std::string("d")];
	BOOST_CHECK(v1 == 0);

	v1 = 10;

	auto v2 = trie[std::string("d")];
	BOOST_CHECK(v2 == 10);

	BOOST_CHECK(trie.count() == 6);
}

// value_type const operator[](Key const& key) const
BOOST_AUTO_TEST_CASE(trie_base__at__9)
{
	test_trie_base trie{};
	trie.insert(std::string("a"));
	trie.insert(std::string("ac"));
	trie.insert(std::string("b"));
	trie.insert(std::string("cab"));
	trie.insert(std::string("cd"));

	auto v1 = trie[std::string("d")];
	BOOST_CHECK(v1 == 0);
	BOOST_CHECK(trie.count() == 6);
}

// イテレータ ------------------------------------------------------------------

// const_iterator begin() const noexcept
BOOST_AUTO_TEST_CASE(trie_base__begin__1)
{
	test_trie_base trie{};
	BOOST_CHECK(trie.begin() != trie.end());
}

// const_iterator cbegin() const noexcept
BOOST_AUTO_TEST_CASE(trie_base__cbegin__1)
{
	test_trie_base trie{};
	BOOST_CHECK(trie.cbegin() != trie.cend());
}

// const_iterator end() const noexcept
BOOST_AUTO_TEST_CASE(trie_base__end__1)
{
	test_trie_base trie{};
	BOOST_CHECK(trie.begin() != trie.end());
}

// const_iterator cend() const noexcept
BOOST_AUTO_TEST_CASE(trie_base__cend__1)
{
	test_trie_base trie{};
	BOOST_CHECK(trie.cbegin() != trie.cend());
}

// 容量 -----------------------------------------------------------------------

// bool empty() const noexcept
BOOST_AUTO_TEST_CASE(trie_base__empty__1)
{
	test_trie_base trie{};
	BOOST_CHECK(trie.empty());
}

BOOST_AUTO_TEST_CASE(trie_base__empty__2)
{
	test_trie_base trie{};
	trie.insert(std::string("abc"));
	BOOST_CHECK(trie.empty() == false);
}

// size_type size() const noexcept
BOOST_AUTO_TEST_CASE(trie_base__size__1)
{
	test_trie_base trie{};
	BOOST_CHECK(trie.size() == 0);
}

BOOST_AUTO_TEST_CASE(trie_base__size__2)
{
	test_trie_base trie{};
	trie.insert(std::string("abc"));
	BOOST_CHECK(trie.size() == 1);
	trie.insert(std::string("abc"));
	BOOST_CHECK(trie.size() == 1);
}

// size_type max_size() const noexcept
BOOST_AUTO_TEST_CASE(trie_base__max_size__1)
{
	test_trie_base trie{};
	BOOST_CHECK(trie.max_size());
}

// 変更 -----------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(trie_base__clear__1)
{
	test_trie_base trie{};

	trie.insert(std::string("\1"));
	trie.insert(std::string("\1\3"));
	trie.insert(std::string("\2"));
	trie.insert(std::string("\3\1\2"));
	trie.insert(std::string("\3\4"));

	BOOST_CHECK(trie.count() == 5);
	BOOST_CHECK(trie.size() == 5);

	trie.clear();

	BOOST_CHECK(trie.count() == 0);
	BOOST_CHECK(trie.size() == 0);
}

// aにacを追加
BOOST_AUTO_TEST_CASE(trie_base__insert__1)
{
	test_trie_base trie{};
	trie.m_c.clear();

	trie.m_c.emplace_back(0, 0); // 0
	trie.m_c.emplace_back(1, 0); // 1
	trie.m_c.emplace_back(0, 1); // 2

	BOOST_CHECK(trie.count_s() == 2);

	test_iterator it = trie.insert(std::string("\1\3"));
	
	BOOST_CHECK(it.m_index == 4);
	BOOST_CHECK(trie.count_s() == 4);

	BOOST_CHECK(trie.m_c[1].m_base == 1);
	BOOST_CHECK(trie.m_c[1].m_check == 0);
	BOOST_CHECK(trie.m_c[2].m_base == 1);
	BOOST_CHECK(trie.m_c[2].m_check == 1);
	BOOST_CHECK(trie.m_c[4].m_check == 2);
	BOOST_CHECK(trie.m_c[257].m_check == 2);
}

// acにaを追加
BOOST_AUTO_TEST_CASE(trie_base__insert__2)
{
	test_trie_base trie{};

	test_iterator it1 = trie.insert(std::string("\1\3"));

	BOOST_CHECK(trie.count_s() == 3);
	BOOST_CHECK(it1.m_index == 4);

	BOOST_CHECK(trie.m_c[1].m_base == 1);
	BOOST_CHECK(trie.m_c[1].m_check == 0);
	BOOST_CHECK(trie.m_c[2].m_base == 1);
	BOOST_CHECK(trie.m_c[2].m_check == 1);
	BOOST_CHECK(trie.m_c[4].m_check == 2);

	test_iterator it2 = trie.insert(std::string("\1"));

	BOOST_CHECK(trie.count_s() == 4);
	BOOST_CHECK(it2.m_index == 2);

	BOOST_CHECK(trie.m_c[1].m_base == 1);
	BOOST_CHECK(trie.m_c[1].m_check == 0);
	BOOST_CHECK(trie.m_c[2].m_base == 1);
	BOOST_CHECK(trie.m_c[2].m_check == 1);
	BOOST_CHECK(trie.m_c[4].m_check == 2);
	BOOST_CHECK(trie.m_c[257].m_check == 2);
}

BOOST_AUTO_TEST_CASE(trie_base__insert__3)
{
	test_trie_base trie{};

	trie.insert(std::string("\1"));
	trie.insert(std::string("\1\3"));
	trie.insert(std::string("\2"));
	trie.insert(std::string("\3\1\2"));
	trie.insert(std::string("\3\4"));

	BOOST_CHECK(trie.count() == 5);
	BOOST_CHECK(trie.find(std::string("\1")) != trie.cend());
	BOOST_CHECK(trie.find(std::string("\1\3")) != trie.cend());
	BOOST_CHECK(trie.find(std::string("\2")) != trie.cend());
	BOOST_CHECK(trie.find(std::string("\3\1\2")) != trie.cend());
	BOOST_CHECK(trie.find(std::string("\3\4")) != trie.cend());
}

BOOST_AUTO_TEST_CASE(trie_base__insert__4)
{
	test_trie_base trie{};
	trie.insert(std::string("\1"));
	trie.insert(std::string("\1\3"));

	BOOST_CHECK(trie.count() == 2);
	BOOST_CHECK(trie.find(std::string("\1")) != trie.cend());
	BOOST_CHECK(trie.find(std::string("\1\3")) != trie.cend());
}

BOOST_AUTO_TEST_CASE(trie_base__insert__5)
{
	test_trie_base trie{};
	trie.insert(std::string("\1\3"));
	trie.insert(std::string("\1"));

	BOOST_CHECK(trie.count() == 2);
	BOOST_CHECK(trie.find(std::string("\1")) != trie.cend());
	BOOST_CHECK(trie.find(std::string("\1\3")) != trie.cend());
}

BOOST_AUTO_TEST_CASE(trie_base__insert__6)
{
	test_trie_base trie{};
	auto it = trie.insert(std::string(""));

	BOOST_CHECK(trie.count() == 0);
	BOOST_CHECK(it == trie.cend());
}

BOOST_AUTO_TEST_CASE(trie_base__insert__7)
{
	test_trie_base trie{};

	trie.insert(std::string("a"));
	trie.insert(std::string("ac"));
	trie.insert(std::string("b"));
	trie.insert(std::string("cab"));
	trie.insert(std::string("cd"));

	BOOST_CHECK(trie.count() == 5);

	auto it = trie.insert(std::string(""));

	BOOST_CHECK(it == trie.cend());
	BOOST_CHECK(trie.count() == 5);
}

// 0を挿入してみる
BOOST_AUTO_TEST_CASE(trie_base__insert__8)
{
	test_trie_base trie{};

	trie.insert(std::string("a"));
	trie.insert(std::string("ac"));
	trie.insert(std::string("b"));
	trie.insert(std::string("cab"));
	trie.insert(std::string("cd"));

	BOOST_CHECK(trie.count() == 5);

	auto it = trie.insert(std::string(1, '\0'));

	BOOST_CHECK(*it == '\0');
	BOOST_CHECK(trie.count() == 6);
}

// void erase(const_iterator pos)
BOOST_AUTO_TEST_CASE(trie_base__erase__1)
{
	test_trie_base trie{};

	trie.insert(std::string("a"));
	trie.insert(std::string("ac"));
	trie.insert(std::string("b"));
	trie.insert(std::string("cab"));
	trie.insert(std::string("cd"));
	trie.insert(std::string(1, '\0'));

	BOOST_CHECK(trie.count() == 6);

	trie.erase(trie.find(std::string(1, '\0')));

	BOOST_CHECK(trie.count() == 5);
	BOOST_CHECK(trie.find(std::string("a")) != trie.cend());
	BOOST_CHECK(trie.find(std::string("ac")) != trie.cend());
	BOOST_CHECK(trie.find(std::string("b")) != trie.cend());
	BOOST_CHECK(trie.find(std::string("cab")) != trie.cend());
	BOOST_CHECK(trie.find(std::string("cd")) != trie.cend());
	BOOST_CHECK(trie.find(std::string(1, '\0')) == trie.cend());
}

BOOST_AUTO_TEST_CASE(trie_base__erase__2)
{
	test_trie_base trie{};

	trie.insert(std::string("a"));
	trie.insert(std::string("ac"));
	trie.insert(std::string("b"));
	trie.insert(std::string("cab"));
	trie.insert(std::string("cd"));
	trie.insert(std::string(1, '\0'));

	BOOST_CHECK(trie.count() == 6);

	trie.erase(trie.find(std::string("a")));

	BOOST_CHECK(trie.count() == 5);
	BOOST_CHECK(trie.find(std::string("a")) == trie.cend());
	BOOST_CHECK(trie.find(std::string("ac")) != trie.cend());
	BOOST_CHECK(trie.find(std::string("b")) != trie.cend());
	BOOST_CHECK(trie.find(std::string("cab")) != trie.cend());
	BOOST_CHECK(trie.find(std::string("cd")) != trie.cend());
	BOOST_CHECK(trie.find(std::string(1, '\0')) != trie.cend());
}

BOOST_AUTO_TEST_CASE(trie_base__erase__3)
{
	test_trie_base trie{};

	trie.insert(std::string("a"));
	trie.insert(std::string("ac"));
	trie.insert(std::string("b"));
	trie.insert(std::string("cab"));
	trie.insert(std::string("cd"));
	trie.insert(std::string(1, '\0'));

	BOOST_CHECK(trie.count() == 6);

	trie.erase(trie.find(std::string("ac")));

	BOOST_CHECK(trie.count() == 5);

	BOOST_CHECK(trie.find(std::string("a")) != trie.cend());
	BOOST_CHECK(trie.find(std::string("ac")) == trie.cend());
	BOOST_CHECK(trie.find(std::string("b")) != trie.cend());
	BOOST_CHECK(trie.find(std::string("cab")) != trie.cend());
	BOOST_CHECK(trie.find(std::string("cd")) != trie.cend());
	BOOST_CHECK(trie.find(std::string(1, '\0')) != trie.cend());
}

BOOST_AUTO_TEST_CASE(trie_base__erase__4)
{
	test_trie_base trie{};

	trie.insert(std::string("a"));
	trie.insert(std::string("ac"));
	trie.insert(std::string("b"));
	trie.insert(std::string("cab"));
	trie.insert(std::string("cd"));
	trie.insert(std::string(1, '\0'));

	BOOST_CHECK(trie.count() == 6);

	trie.erase(trie.find(std::string("cab")));

	BOOST_CHECK(trie.count() == 5);
	BOOST_CHECK(trie.find(std::string("a")) != trie.cend());
	BOOST_CHECK(trie.find(std::string("ac")) != trie.cend());
	BOOST_CHECK(trie.find(std::string("b")) != trie.cend());
	BOOST_CHECK(trie.find(std::string("cab")) == trie.cend());
	BOOST_CHECK(trie.find(std::string("cd")) != trie.cend());
	BOOST_CHECK(trie.find(std::string(1, '\0')) != trie.cend());

	BOOST_CHECK(trie.find(std::string("ca")) == trie.cend());
}

BOOST_AUTO_TEST_CASE(trie_base__erase__5)
{
	test_trie_base trie{};

	trie.insert(std::string("a"));
	trie.insert(std::string("ac"));
	trie.insert(std::string("b"));
	trie.insert(std::string("cab"));
	trie.insert(std::string("cd"));
	trie.insert(std::string(1, '\0'));

	BOOST_CHECK(trie.count() == 6);

	trie.erase(trie.find(std::string("a")));
	trie.erase(trie.find(std::string("ac")));
	trie.erase(trie.find(std::string("b")));
	trie.erase(trie.find(std::string("cab")));
	trie.erase(trie.find(std::string("cd")));
	trie.erase(trie.find(std::string(1, '\0')));

	BOOST_CHECK(trie.count() == 0);
	BOOST_CHECK(trie.find(std::string("a")) == trie.cend());
	BOOST_CHECK(trie.find(std::string("ac")) == trie.cend());
	BOOST_CHECK(trie.find(std::string("b")) == trie.cend());
	BOOST_CHECK(trie.find(std::string("cab")) == trie.cend());
	BOOST_CHECK(trie.find(std::string("cd")) == trie.cend());
	BOOST_CHECK(trie.find(std::string(1, '\0')) == trie.cend());
}

BOOST_AUTO_TEST_CASE(trie_base__erase__6)
{
	test_trie_base trie{};

	trie.insert(std::string("a"));
	trie.insert(std::string("ac"));
	trie.insert(std::string("b"));
	trie.insert(std::string("cab"));
	trie.insert(std::string("cd"));
	trie.insert(std::string(1, '\0'));

	BOOST_CHECK(trie.count() == 6);

	trie.erase(trie.begin());

	BOOST_CHECK(trie.count() == 6);
}

BOOST_AUTO_TEST_CASE(trie_base__erase__7)
{
	test_trie_base trie{};
	BOOST_CHECK(trie.count() == 0);

	trie.erase(trie.begin());

	BOOST_CHECK(trie.count() == 0);
}

BOOST_AUTO_TEST_CASE(trie_base__erase__8)
{
	test_trie_base trie{};

	trie.insert(std::string("a"));
	trie.insert(std::string("ac"));
	trie.insert(std::string("b"));
	trie.insert(std::string("cab"));
	trie.insert(std::string("cd"));
	trie.insert(std::string(1, '\0'));

	BOOST_CHECK(trie.count() == 6);

	trie.erase(trie.end());

	BOOST_CHECK(trie.count() == 6);
}

BOOST_AUTO_TEST_CASE(trie_base__erase__9)
{
	test_trie_base trie{};
	BOOST_CHECK(trie.count() == 0);

	trie.erase(trie.end());

	BOOST_CHECK(trie.count() == 0);
}

BOOST_AUTO_TEST_CASE(trie_base__erase__10)
{
	test_trie_base trie{};

	trie.insert(std::string("a"));
	trie.insert(std::string("ac"));
	trie.insert(std::string("b"));
	trie.insert(std::string("cab"));
	trie.insert(std::string("cd"));
	trie.insert(std::string(1, '\0'));

	BOOST_CHECK(trie.count() == 6);

	trie.erase(std::string("a"));

	BOOST_CHECK(trie.count() == 5);
}

BOOST_AUTO_TEST_CASE(trie_base__erase__11)
{
	test_trie_base trie{};

	trie.insert(std::string("a"));
	trie.insert(std::string("ac"));
	trie.insert(std::string("b"));
	trie.insert(std::string("cab"));
	trie.insert(std::string("cd"));
	trie.insert(std::string(1, '\0'));

	BOOST_CHECK(trie.count() == 6);

	trie.erase(std::string(""));

	BOOST_CHECK(trie.count() == 6);
}

BOOST_AUTO_TEST_CASE(trie_base__erase__12)
{
	test_trie_base trie{};
	BOOST_CHECK(trie.count() == 0);

	trie.erase(std::string(""));

	BOOST_CHECK(trie.count() == 0);
}

// 検索 -----------------------------------------------------------------------

// auto search(Iterator first, Iterator last) const->std::pair<Iterator, const_iterator>
BOOST_AUTO_TEST_CASE(trie_base__search__1)
{
	test_trie_base trie{};

	trie.insert(std::string("a"));
	trie.insert(std::string("ac"));
	trie.insert(std::string("b"));
	trie.insert(std::string("cab"));
	trie.insert(std::string("cd"));

	std::string s{ "d" };
	auto p = trie.search(s.begin(), s.end());

	BOOST_CHECK(p.first == trie.end());
	BOOST_CHECK(p.second == s.begin());
}

BOOST_AUTO_TEST_CASE(trie_base__search__2)
{
	test_trie_base trie{};

	trie.insert(std::string("a"));
	trie.insert(std::string("ac"));
	trie.insert(std::string("b"));
	trie.insert(std::string("cab"));
	trie.insert(std::string("cd"));

	std::string s{ "c" };
	auto p = trie.search(s.begin(), s.end());

	BOOST_CHECK(*p.first == 'c');
	BOOST_CHECK(p.second == s.end());
}

BOOST_AUTO_TEST_CASE(trie_base__search__3)
{
	test_trie_base trie{};

	trie.insert(std::string("a"));
	trie.insert(std::string("ac"));
	trie.insert(std::string("b"));
	trie.insert(std::string("cab"));
	trie.insert(std::string("cd"));

	std::string s{ "cb" };
	auto p = trie.search(s.begin(), s.end());

	BOOST_CHECK(*p.first == 'c');
	BOOST_CHECK(*p.second == 'b');
}

BOOST_AUTO_TEST_CASE(trie_base__search__4)
{
	test_trie_base trie{};

	trie.insert(std::string("a"));
	trie.insert(std::string("ac"));
	trie.insert(std::string("b"));
	trie.insert(std::string("cab"));
	trie.insert(std::string("cd"));

	std::string s{ "a" };
	auto p = trie.search(s.begin(), s.end());

	BOOST_CHECK(*p.first == 'a');
	BOOST_CHECK(p.second == s.end());
}

BOOST_AUTO_TEST_CASE(trie_base__search__5)
{
	test_trie_base trie{};

	trie.insert(std::string("a"));
	trie.insert(std::string("ac"));
	trie.insert(std::string("b"));
	trie.insert(std::string("cab"));
	trie.insert(std::string("cd"));

	std::string s{ "cab" };
	auto p = trie.search(s.begin(), s.end());

	BOOST_CHECK(*p.first == 'b');
	BOOST_CHECK(p.second == s.end());
}

BOOST_AUTO_TEST_CASE(trie_base__search__6)
{
	test_trie_base trie{};

	trie.insert(std::string("a"));
	trie.insert(std::string("ac"));
	trie.insert(std::string("b"));
	trie.insert(std::string("cab"));
	trie.insert(std::string("cd"));

	std::string s{ "" };
	auto p = trie.search(s.begin(), s.end());

	BOOST_CHECK(p.first == trie.end());
	BOOST_CHECK(p.second == s.begin());
}

BOOST_AUTO_TEST_CASE(trie_base__search__7)
{
	test_trie_base trie{};

	std::string s{ "" };
	auto p = trie.search(s.begin(), s.end());

	BOOST_CHECK(p.first == trie.end());
	BOOST_CHECK(p.second == s.begin());
}

// const_iterator search(Key const& key) const
BOOST_AUTO_TEST_CASE(trie_base__search__8)
{
	test_trie_base trie{};

	trie.insert(std::string("a"));
	trie.insert(std::string("ac"));
	trie.insert(std::string("b"));
	trie.insert(std::string("cab"));
	trie.insert(std::string("cd"));

	auto it = trie.search(std::string{ "" });

	BOOST_CHECK(it == trie.cend());
}

BOOST_AUTO_TEST_CASE(trie_base__search__9)
{
	test_trie_base trie{};

	auto it = trie.search(std::string{ "a" });

	BOOST_CHECK(it == trie.cend());
}

BOOST_AUTO_TEST_CASE(trie_base__search__10)
{
	test_trie_base trie{};

	auto it = trie.search(std::string{ "" });

	BOOST_CHECK(it == trie.cend());
}

BOOST_AUTO_TEST_CASE(trie_base__search__11)
{
	test_trie_base trie{};

	trie.insert(std::string("a"));
	trie.insert(std::string("ac"));
	trie.insert(std::string("b"));
	trie.insert(std::string("cab"));
	trie.insert(std::string("cd"));

	auto it = trie.search(std::string{ "c" });

	BOOST_CHECK(*it == 'c');
}

BOOST_AUTO_TEST_CASE(trie_base__search__12)
{
	test_trie_base trie{};

	trie.insert(std::string("a"));
	trie.insert(std::string("ac"));
	trie.insert(std::string("b"));
	trie.insert(std::string("cab"));
	trie.insert(std::string("cd"));

	auto it = trie.search(std::string{ "cab" });

	BOOST_CHECK(*it == 'b');
}

// const_iterator find(Key const& key) const
BOOST_AUTO_TEST_CASE(trie_base__find__1)
{
	test_trie_base trie{};

	trie.insert(std::string("a"));
	trie.insert(std::string("ac"));
	trie.insert(std::string("b"));
	trie.insert(std::string("cab"));
	trie.insert(std::string("cd"));

	auto it = trie.find(std::string{ "c" });

	BOOST_CHECK(it == trie.cend());
}

BOOST_AUTO_TEST_CASE(trie_base__find__2)
{
	test_trie_base trie{};

	trie.insert(std::string("a"));
	trie.insert(std::string("ac"));
	trie.insert(std::string("b"));
	trie.insert(std::string("cab"));
	trie.insert(std::string("cd"));

	auto it = trie.find(std::string{ "" });

	BOOST_CHECK(it == trie.cend());
}

BOOST_AUTO_TEST_CASE(trie_base__find__3)
{
	test_trie_base trie{};

	auto it = trie.find(std::string{ "c" });

	BOOST_CHECK(it == trie.cend());
}

BOOST_AUTO_TEST_CASE(trie_base__find__4)
{
	test_trie_base trie{};

	trie.insert(std::string("a"));
	trie.insert(std::string("ac"));
	trie.insert(std::string("b"));
	trie.insert(std::string("cab"));
	trie.insert(std::string("cd"));

	auto it = trie.find(std::string{ "a" });
	BOOST_CHECK(*it == 'a');

	it = trie.find(std::string{ "ac" });
	BOOST_CHECK(*it == 'c');

	it = trie.find(std::string{ "b" });
	BOOST_CHECK(*it == 'b');

	it = trie.find(std::string{ "cab" });
	BOOST_CHECK(*it == 'b');

	it = trie.find(std::string{ "cd" });
	BOOST_CHECK(*it == 'd');
}

// 0を検索してみる
BOOST_AUTO_TEST_CASE(trie_base__find__5)
{
	test_trie_base trie{};

	trie.insert(std::string("a"));
	trie.insert(std::string("ac"));
	trie.insert(std::string("b"));
	trie.insert(std::string("cab"));
	trie.insert(std::string("cd"));
	trie.insert(std::string(1, '\0'));

	BOOST_CHECK(trie.count() == 6);
	auto it = trie.find(std::string(1, '\0'));
	BOOST_CHECK(*it == '\0');
}

// 内部 -----------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(trie_base__reserve__1)
{
	std::vector<int> v(1024, 0);
	std::size_t i1 = v.size();
	std::size_t i2 = v.capacity();

	v.clear();

	std::size_t i3 = v.size();
	std::size_t i4 = v.capacity();

	BOOST_CHECK(i1 != i3);
	BOOST_CHECK(i2 == i4);
}

// void reserve(std::size_t n = 1024)
BOOST_AUTO_TEST_CASE(trie_base__reserve__2)
{
	test_trie_base trie{};
	trie.m_c.clear();
	trie.m_c.emplace_back(0, 0);

	trie.reserve(0);

	BOOST_CHECK(trie.m_c.size() == 1);
	BOOST_CHECK(trie.m_c.front().m_base == 0);
	BOOST_CHECK(trie.m_c.front().m_check == 0);
}

BOOST_AUTO_TEST_CASE(trie_base__reserve__3)
{
	test_trie_base trie{};
	trie.m_c.clear();
	trie.m_c.emplace_back(0, 0);

	trie.reserve(1);

	BOOST_CHECK(trie.m_c.size() == 2);
	BOOST_CHECK(trie.m_c.at(0).m_base == -1);
	BOOST_CHECK(trie.m_c.at(0).m_check == -1);
	BOOST_CHECK(trie.m_c.at(1).m_check == 0);
}

BOOST_AUTO_TEST_CASE(trie_base__reserve__4)
{
	test_trie_base trie{};
	trie.m_c.clear();
	trie.m_c.emplace_back(0, 0);

	trie.reserve(4);

	BOOST_CHECK(trie.m_c.size() == 5);

	BOOST_CHECK(trie.m_c.at(0).m_base == -4);
	BOOST_CHECK(trie.m_c.at(0).m_check == -1);
	BOOST_CHECK(trie.m_c.at(1).m_check == -2);
	BOOST_CHECK(trie.m_c.at(2).m_check == -3);
	BOOST_CHECK(trie.m_c.at(3).m_check == -4);
	BOOST_CHECK(trie.m_c.at(4).m_check == 0);
}

BOOST_AUTO_TEST_CASE(trie_base__reserve__5)
{
	test_trie_base trie{};
	trie.m_c.clear();
	trie.m_c.emplace_back(0, 0);

	trie.m_c.front() = wordring::trie_node{ -3, -1 };
	trie.m_c.emplace_back(0, -3);
	trie.m_c.emplace_back(0, 0);
	trie.m_c.emplace_back(0, 0);
	trie.m_c.emplace_back(0, 0);

	trie.reserve(4);

	BOOST_CHECK(trie.m_c.size() == 9);

	BOOST_CHECK(trie.m_c.at(0).m_base == -8);
	BOOST_CHECK(trie.m_c.at(0).m_check == -1);
	BOOST_CHECK(trie.m_c.at(1).m_check == -3);
	BOOST_CHECK(trie.m_c.at(2).m_check == 0);
	BOOST_CHECK(trie.m_c.at(3).m_check == -5);
	BOOST_CHECK(trie.m_c.at(4).m_check == 0);
	BOOST_CHECK(trie.m_c.at(5).m_check == -6);
	BOOST_CHECK(trie.m_c.at(6).m_check == -7);
	BOOST_CHECK(trie.m_c.at(7).m_check == -8);
	BOOST_CHECK(trie.m_c.at(8).m_check == 0);
}

BOOST_AUTO_TEST_CASE(trie_base__reserve__6)
{
	test_trie_base trie{};
	trie.m_c.clear();
	trie.m_c.emplace_back(0, 0);

	trie.m_c.front() = wordring::trie_node{ -2, -1 };
	trie.m_c.emplace_back(0, -2);
	trie.m_c.emplace_back(0, 0);

	trie.reserve(3);

	BOOST_CHECK(trie.m_c.size() == 6);

	BOOST_CHECK(trie.m_c.at(0).m_base == -5);
	BOOST_CHECK(trie.m_c.at(0).m_check == -1);
	BOOST_CHECK(trie.m_c.at(1).m_check == -2);
	BOOST_CHECK(trie.m_c.at(2).m_check == -3);
	BOOST_CHECK(trie.m_c.at(3).m_check == -4);
	BOOST_CHECK(trie.m_c.at(4).m_check == -5);
	BOOST_CHECK(trie.m_c.at(5).m_check == 0);
}

BOOST_AUTO_TEST_CASE(trie_base__reserve__7)
{
	test_trie_base trie{};
	trie.m_c.clear();
	trie.m_c.emplace_back(0, 0);

	trie.m_c.front() = wordring::trie_node{ -1, -1 };
	trie.m_c.emplace_back(0, 0);

	trie.reserve(3);

	BOOST_CHECK(trie.m_c.size() == 5);

	BOOST_CHECK(trie.m_c.at(0).m_base == -4);
	BOOST_CHECK(trie.m_c.at(0).m_check == -1);
	BOOST_CHECK(trie.m_c.at(1).m_check == -2);
	BOOST_CHECK(trie.m_c.at(2).m_check == -3);
	BOOST_CHECK(trie.m_c.at(3).m_check == -4);
	BOOST_CHECK(trie.m_c.at(4).m_check == 0);
}

// void allocate(index_type index)
BOOST_AUTO_TEST_CASE(trie_base__allocate__1)
{
	test_trie_base trie{};
	trie.m_c.clear();

	trie.m_c.emplace_back(-8, -1); // 0
	trie.m_c.emplace_back(0, -3);  // 1
	trie.m_c.emplace_back(0, 0);   // 2
	trie.m_c.emplace_back(0, -5);  // 3
	trie.m_c.emplace_back(0, 0);   // 4
	trie.m_c.emplace_back(0, -6);  // 5
	trie.m_c.emplace_back(0, -7);  // 6
	trie.m_c.emplace_back(0, -8);  // 7
	trie.m_c.emplace_back(0, 0);   // 8

	trie.allocate(3);

	BOOST_CHECK(trie.m_c.at(0).m_base == -8);
	BOOST_CHECK(trie.m_c.at(0).m_check == -1);
	BOOST_CHECK(trie.m_c.at(1).m_check == -5);
	BOOST_CHECK(trie.m_c.at(2).m_check == 0);
	BOOST_CHECK(trie.m_c.at(3).m_check == 0);
	BOOST_CHECK(trie.m_c.at(4).m_check == 0);
	BOOST_CHECK(trie.m_c.at(5).m_check == -6);
	BOOST_CHECK(trie.m_c.at(6).m_check == -7);
	BOOST_CHECK(trie.m_c.at(7).m_check == -8);
	BOOST_CHECK(trie.m_c.at(8).m_check == 0);
}

BOOST_AUTO_TEST_CASE(trie_base__allocate__2)
{
	test_trie_base trie{};
	trie.m_c.clear();

	trie.m_c.emplace_back(-8, -1); // 0
	trie.m_c.emplace_back(0, -3);  // 1
	trie.m_c.emplace_back(0, 0);   // 2
	trie.m_c.emplace_back(0, -5);  // 3
	trie.m_c.emplace_back(0, 0);   // 4
	trie.m_c.emplace_back(0, -6);  // 5
	trie.m_c.emplace_back(0, -7);  // 6
	trie.m_c.emplace_back(0, -8);  // 7
	trie.m_c.emplace_back(0, 0);   // 8

	trie.allocate(1);

	BOOST_CHECK(trie.m_c.at(0).m_base == -8);
	BOOST_CHECK(trie.m_c.at(0).m_check == -3);
	BOOST_CHECK(trie.m_c.at(1).m_check == 0);
	BOOST_CHECK(trie.m_c.at(2).m_check == 0);
	BOOST_CHECK(trie.m_c.at(3).m_check == -5);
	BOOST_CHECK(trie.m_c.at(4).m_check == 0);
	BOOST_CHECK(trie.m_c.at(5).m_check == -6);
	BOOST_CHECK(trie.m_c.at(6).m_check == -7);
	BOOST_CHECK(trie.m_c.at(7).m_check == -8);
	BOOST_CHECK(trie.m_c.at(8).m_check == 0);
}

BOOST_AUTO_TEST_CASE(trie_base__allocate__3)
{
	test_trie_base trie{};
	trie.m_c.clear();

	trie.m_c.emplace_back(-8, -2); // 0
	trie.m_c.emplace_back(0, 0);   // 1
	trie.m_c.emplace_back(0, -3);  // 2
	trie.m_c.emplace_back(0, -5);  // 3
	trie.m_c.emplace_back(0, 0);   // 4
	trie.m_c.emplace_back(0, -6);  // 5
	trie.m_c.emplace_back(0, -7);  // 6
	trie.m_c.emplace_back(0, -8);  // 7
	trie.m_c.emplace_back(0, 0);   // 8

	trie.allocate(2);

	BOOST_CHECK(trie.m_c.at(0).m_base == -8);
	BOOST_CHECK(trie.m_c.at(0).m_check == -3);
	BOOST_CHECK(trie.m_c.at(1).m_check == 0);
	BOOST_CHECK(trie.m_c.at(2).m_check == 0);
	BOOST_CHECK(trie.m_c.at(3).m_check == -5);
	BOOST_CHECK(trie.m_c.at(4).m_check == 0);
	BOOST_CHECK(trie.m_c.at(5).m_check == -6);
	BOOST_CHECK(trie.m_c.at(6).m_check == -7);
	BOOST_CHECK(trie.m_c.at(7).m_check == -8);
	BOOST_CHECK(trie.m_c.at(8).m_check == 0);
}

BOOST_AUTO_TEST_CASE(trie_base__allocate__4)
{
	test_trie_base trie{};
	trie.m_c.clear();

	trie.m_c.emplace_back(-8, -1); // 0
	trie.m_c.emplace_back(0, -3);  // 1
	trie.m_c.emplace_back(0, 0);   // 2
	trie.m_c.emplace_back(0, -5);  // 3
	trie.m_c.emplace_back(0, 0);   // 4
	trie.m_c.emplace_back(0, -6);  // 5
	trie.m_c.emplace_back(0, -7);  // 6
	trie.m_c.emplace_back(0, -8);  // 7
	trie.m_c.emplace_back(0, 0);   // 8

	trie.allocate(8);

	BOOST_CHECK(trie.m_c.at(0).m_base == -7);
	BOOST_CHECK(trie.m_c.at(0).m_check == -1);
	BOOST_CHECK(trie.m_c.at(1).m_check == -3);
	BOOST_CHECK(trie.m_c.at(2).m_check == 0);
	BOOST_CHECK(trie.m_c.at(3).m_check == -5);
	BOOST_CHECK(trie.m_c.at(4).m_check == 0);
	BOOST_CHECK(trie.m_c.at(5).m_check == -6);
	BOOST_CHECK(trie.m_c.at(6).m_check == -7);
	BOOST_CHECK(trie.m_c.at(7).m_check == 0);
	BOOST_CHECK(trie.m_c.at(8).m_check == 0);
}

BOOST_AUTO_TEST_CASE(trie_base__allocate__5)
{
	test_trie_base trie{};
	trie.m_c.clear();

	trie.m_c.emplace_back(-8, -8); // 0
	trie.m_c.emplace_back(0, 0);   // 1
	trie.m_c.emplace_back(0, 0);   // 2
	trie.m_c.emplace_back(0, 0);   // 3
	trie.m_c.emplace_back(0, 0);   // 4
	trie.m_c.emplace_back(0, 0);   // 5
	trie.m_c.emplace_back(0, 0);   // 6
	trie.m_c.emplace_back(0, 0);   // 7
	trie.m_c.emplace_back(0, 0);   // 8

	trie.allocate(8);

	BOOST_CHECK(trie.m_c.at(0).m_base == 0);
	BOOST_CHECK(trie.m_c.at(0).m_check == 0);
	BOOST_CHECK(trie.m_c.at(1).m_check == 0);
	BOOST_CHECK(trie.m_c.at(2).m_check == 0);
	BOOST_CHECK(trie.m_c.at(3).m_check == 0);
	BOOST_CHECK(trie.m_c.at(4).m_check == 0);
	BOOST_CHECK(trie.m_c.at(5).m_check == 0);
	BOOST_CHECK(trie.m_c.at(6).m_check == 0);
	BOOST_CHECK(trie.m_c.at(7).m_check == 0);
	BOOST_CHECK(trie.m_c.at(8).m_check == 0);
}

BOOST_AUTO_TEST_CASE(trie_base__allocate__6)
{
	test_trie_base trie{};
	trie.m_c.clear();

	trie.m_c.emplace_back(-1, -1); // 0
	trie.m_c.emplace_back(0, 0);   // 1

	trie.allocate(1);

	BOOST_CHECK(trie.m_c.at(0).m_base == 0);
	BOOST_CHECK(trie.m_c.at(0).m_check == 0);
	BOOST_CHECK(trie.m_c.at(1).m_check == 0);
}

// index_type allocate(Range1 dests)
BOOST_AUTO_TEST_CASE(trie_base__allocate__7)
{
	test_trie_base trie{};
	trie.m_c.clear();

	trie.m_c.emplace_back(-5, -1); // 0
	trie.m_c.emplace_back(0, -3);  // 1
	trie.m_c.emplace_back(0, 0);   // 2
	trie.m_c.emplace_back(0, -5);  // 3
	trie.m_c.emplace_back(0, 0);   // 4
	trie.m_c.emplace_back(0, 0);   // 5

	auto i = trie.allocate(wordring::static_vector<std::uint16_t, 257>{ 1 });

	BOOST_CHECK(i == 2);

	BOOST_CHECK(trie.m_c.at(0).m_base < 0);
	BOOST_CHECK(trie.m_c.at(0).m_check == -1);
	BOOST_CHECK(trie.m_c.at(1).m_check == -5);
	BOOST_CHECK(trie.m_c.at(2).m_check == 0);
	BOOST_CHECK(trie.m_c.at(3).m_check == 0);
	BOOST_CHECK(trie.m_c.at(4).m_check == 0);
	BOOST_CHECK(trie.m_c.at(5).m_check < 0);
}

BOOST_AUTO_TEST_CASE(trie_base__allocate__8)
{
	test_trie_base trie{};
	trie.m_c.clear();

	trie.m_c.emplace_back(-5, -1); // 0
	trie.m_c.emplace_back(0, -3);  // 1
	trie.m_c.emplace_back(0, 0);   // 2
	trie.m_c.emplace_back(0, -5);  // 3
	trie.m_c.emplace_back(0, 0);   // 4
	trie.m_c.emplace_back(0, 0);   // 5

	auto i = trie.allocate(wordring::static_vector<std::uint16_t, 257>{ 2, 4 });

	BOOST_CHECK(i == 1);

	BOOST_CHECK(trie.m_c.at(0).m_base < 0);
	BOOST_CHECK(trie.m_c.at(0).m_check == -1);
	BOOST_CHECK(trie.m_c.at(1).m_check < 0);
	BOOST_CHECK(trie.m_c.at(2).m_check == 0);
	BOOST_CHECK(trie.m_c.at(3).m_check == 0);
	BOOST_CHECK(trie.m_c.at(4).m_check == 0);
	BOOST_CHECK(trie.m_c.at(5).m_check == 0);
	BOOST_CHECK(trie.m_c.at(6).m_check < 0);
}

BOOST_AUTO_TEST_CASE(trie_base__allocate__9)
{
	test_trie_base trie{};
	trie.m_c.clear();

	trie.m_c.emplace_back(-5, -1); // 0
	trie.m_c.emplace_back(0, -3);  // 1
	trie.m_c.emplace_back(0, 0);   // 2 使用済み
	trie.m_c.emplace_back(0, -5);  // 3
	trie.m_c.emplace_back(0, 0);   // 4 使用済み
	trie.m_c.emplace_back(0, 0);   // 5

	auto i = trie.allocate(wordring::static_vector<std::uint16_t, 257>{ 0, 1 });

	BOOST_CHECK(i == 5);

	BOOST_CHECK(trie.m_c.at(0).m_base <= 0);
	BOOST_CHECK(trie.m_c.at(0).m_check == -1);

	BOOST_CHECK(trie.m_c.at(1).m_check == -3);
	BOOST_CHECK(trie.m_c.at(2).m_check == 0); // 2 使用済み
	BOOST_CHECK(trie.m_c.at(3).m_check <= 0); 
	BOOST_CHECK(trie.m_c.at(4).m_check == 0); // 4 使用済み
	BOOST_CHECK(trie.m_c.at(5).m_check == 0); // 5 新規確保
	BOOST_CHECK(trie.m_c.at(6).m_check == 0); // 6 新規確保
}

// void free(index_type index)
BOOST_AUTO_TEST_CASE(trie_base__free__1)
{
	test_trie_base trie{};
	trie.m_c.clear();

	trie.m_c.emplace_back(0, 0); // 0
	trie.m_c.emplace_back(0, 5); // 1

	trie.free(1);

	BOOST_CHECK(trie.m_c.at(0).m_base == -1);
	BOOST_CHECK(trie.m_c.at(0).m_check == -1);
	BOOST_CHECK(trie.m_c.at(1).m_check == 0);
}

BOOST_AUTO_TEST_CASE(trie_base__free__2)
{
	test_trie_base trie{};
	trie.m_c.clear();

	trie.m_c.emplace_back(-3, -2); // 0
	trie.m_c.emplace_back(0, 5);   // 1
	trie.m_c.emplace_back(0, -3);  // 2
	trie.m_c.emplace_back(0, 0);   // 3

	trie.free(1);

	BOOST_CHECK(trie.m_c.at(0).m_base == -1);
	BOOST_CHECK(trie.m_c.at(0).m_check == -2);
	BOOST_CHECK(trie.m_c.at(1).m_check == 0);
	BOOST_CHECK(trie.m_c.at(2).m_check == -3);
	BOOST_CHECK(trie.m_c.at(3).m_check == -1);
}

BOOST_AUTO_TEST_CASE(trie_base__free__3)
{
	test_trie_base trie{};
	trie.m_c.clear();

	trie.m_c.emplace_back(0, 0); // 0
	trie.m_c.emplace_back(0, 0); // 1
	trie.m_c.emplace_back(0, 0); // 2
	trie.m_c.emplace_back(0, 0); // 3

	trie.free(3);

	BOOST_CHECK(trie.m_c.at(0).m_base == -3);
	BOOST_CHECK(trie.m_c.at(0).m_check == -3);
	BOOST_CHECK(trie.m_c.at(1).m_check == 0);
	BOOST_CHECK(trie.m_c.at(2).m_check == 0);
	BOOST_CHECK(trie.m_c.at(3).m_check == 0);
}

BOOST_AUTO_TEST_CASE(trie_base__free__4)
{
	test_trie_base trie{};
	trie.m_c.clear();

	trie.m_c.emplace_back(-3, -1); // 0
	trie.m_c.emplace_back(0, -3);  // 1
	trie.m_c.emplace_back(0, 0);   // 2
	trie.m_c.emplace_back(0, 0);   // 3

	trie.free(2);

	BOOST_CHECK(trie.m_c.at(0).m_base == -2);
	BOOST_CHECK(trie.m_c.at(0).m_check == -1);
	BOOST_CHECK(trie.m_c.at(1).m_check == -3);
	BOOST_CHECK(trie.m_c.at(2).m_check == 0);
	BOOST_CHECK(trie.m_c.at(3).m_check == -2);
}

// bool is_free(index_type base, label_vector const& labels) const
BOOST_AUTO_TEST_CASE(trie_base__is_free__1)
{
	test_trie_base trie{};
	trie.m_c.clear();
	trie.m_c.emplace_back(-1, -1); // 0
	trie.m_c.emplace_back(0, 0);   // 1

	BOOST_CHECK(trie.is_free(1, { 0 }));
}

BOOST_AUTO_TEST_CASE(trie_base__is_free__2)
{
	test_trie_base trie{};
	trie.m_c.clear();
	trie.m_c.emplace_back(-1, -1); // 0
	trie.m_c.emplace_back(0, 0);   // 1

	BOOST_CHECK(trie.is_free(1, { 0, 256 }));
}

BOOST_AUTO_TEST_CASE(trie_base__is_free__3)
{
	test_trie_base trie{};
	trie.m_c.clear();
	trie.m_c.emplace_back(-1, -1); // 0
	trie.m_c.insert(trie.m_c.end(), 257, { 0, 0 } );

	BOOST_CHECK(trie.is_free(1, { 256 }) == false);
}

// index_type modify(const_iterator parent, label_vector& labels)
BOOST_AUTO_TEST_CASE(trie_base__modify__1)
{
	test_trie_base trie{};
	trie.insert(std::string("a"));
	trie.insert(std::string("ac"));
	trie.insert(std::string("acb"));
	trie.insert(std::string("ab"));
	trie.insert(std::string("b"));
	trie.insert(std::string("cab"));
	trie.insert(std::string("cd"));
	trie.insert(std::string("cd"));

	BOOST_CHECK(trie.count() == 7);

	trie.insert(std::string("d"));

	BOOST_CHECK(trie.count() == 8);
}

BOOST_AUTO_TEST_CASE(trie_base__modify__2)
{
	BOOST_WARN(false);
}

// void add(const_iterator parent, label_vector const& labels)
// 静的構築
BOOST_AUTO_TEST_CASE(trie_base__add__1)
{
	using namespace wordring;

	test_trie_base trie{};
	trie.add(test_iterator(trie.m_c, 1), { 1, 2, 3 });
	trie.add(test_iterator(trie.m_c, 2), { 0, 3 });
	trie.add(test_iterator(trie.m_c, 4), { 1, 4 });
	trie.add(test_iterator(trie.m_c, 6), 2);

	BOOST_CHECK(trie.m_c[1].m_base == 1);
	BOOST_CHECK(trie.m_c[1].m_check == 0);
	BOOST_CHECK(trie.m_c[2].m_base == 5);
	BOOST_CHECK(trie.m_c[2].m_check == 1);
	BOOST_CHECK(trie.m_c[3].m_check == 1);
	BOOST_CHECK(trie.m_c[4].m_base == 5);
	BOOST_CHECK(trie.m_c[4].m_check == 1);
	BOOST_CHECK(trie.m_c[5].m_check == 2);
	BOOST_CHECK(trie.m_c[6].m_base == 5);
	BOOST_CHECK(trie.m_c[6].m_check == 4);
	BOOST_CHECK(trie.m_c[7].m_check == 6);
	BOOST_CHECK(trie.m_c[8].m_check == 2);
	BOOST_CHECK(trie.m_c[9].m_check == 4);
}

// 動的構築
BOOST_AUTO_TEST_CASE(trie_base__add__2)
{
	using namespace wordring;

	test_trie_base trie{};
	trie.add(test_iterator(trie.m_c, 1), { 1, 2, 3 });
	trie.add(test_iterator(trie.m_c, 2), { 0, 3 });
	trie.add(test_iterator(trie.m_c, 4), { 1, 4 });
	trie.add(test_iterator(trie.m_c, 6), 2);

	trie.add(test_iterator(trie.m_c, 2), 5);

	BOOST_CHECK(trie.count());

	BOOST_CHECK(trie.m_c[1].m_base == 1);
	BOOST_CHECK(trie.m_c[1].m_check == 0);
	BOOST_CHECK(trie.m_c[2].m_base == 5);
	BOOST_CHECK(trie.m_c[2].m_check == 1);
	BOOST_CHECK(trie.m_c[3].m_check == 1);
	BOOST_CHECK(trie.m_c[4].m_base == 5);
	BOOST_CHECK(trie.m_c[4].m_check == 1);
	BOOST_CHECK(trie.m_c[5].m_check == 2);
	BOOST_CHECK(trie.m_c[6].m_base == 5);
	BOOST_CHECK(trie.m_c[6].m_check == 4);
	BOOST_CHECK(trie.m_c[7].m_check == 6);
	BOOST_CHECK(trie.m_c[8].m_check == 2);
	BOOST_CHECK(trie.m_c[9].m_check == 4);

	BOOST_CHECK(trie.m_c[10].m_check == 2);
}

// 動的構築
BOOST_AUTO_TEST_CASE(trie_base__add__3)
{
	using namespace wordring;

	test_trie_base trie{};
	trie.add(test_iterator(trie.m_c, 1), { 1, 2, 3 });
	trie.add(test_iterator(trie.m_c, 2), { 0, 3 });
	trie.add(test_iterator(trie.m_c, 4), { 1, 4 });
	trie.add(test_iterator(trie.m_c, 6), 2);

	trie.add(test_iterator(trie.m_c, 2), 4);

	BOOST_CHECK(trie.m_c[1].m_base == 1);
	BOOST_CHECK(trie.m_c[1].m_check == 0);
	BOOST_CHECK(trie.m_c[2].m_base == 10);
	BOOST_CHECK(trie.m_c[2].m_check == 1);
	BOOST_CHECK(trie.m_c[3].m_check == 1);
	BOOST_CHECK(trie.m_c[4].m_base == 5);
	BOOST_CHECK(trie.m_c[4].m_check == 1);
	BOOST_CHECK(trie.m_c[6].m_base == 5);
	BOOST_CHECK(trie.m_c[6].m_check == 4);
	BOOST_CHECK(trie.m_c[7].m_check == 6);
	BOOST_CHECK(trie.m_c[9].m_check == 4);

	BOOST_CHECK(trie.m_c[10].m_check == 2);
	BOOST_CHECK(trie.m_c[13].m_check == 2);
	BOOST_CHECK(trie.m_c[14].m_check == 2);
}

// 入出力 ---------------------------------------------------------------------

// inline std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, trie_base<Allocator>& trie)

BOOST_AUTO_TEST_CASE(trie_base__stream__1)
{
	test_trie_base t1{};

	t1.insert(std::string("a"));
	t1.insert(std::string("ac"));
	t1.insert(std::string("b"));
	t1.insert(std::string("cab"));
	t1.insert(std::string("cd"));

	std::stringstream s;
	s << t1;

	test_trie_base t2{};
	s >> t2;

	BOOST_CHECK(t1.m_c == t2.m_c);
	BOOST_CHECK(t1.size() == t2.size());

	BOOST_CHECK(*t2.find(std::string("a")) == 'a');
	BOOST_CHECK(*t2.find(std::string("ac")) == 'c');
	BOOST_CHECK(*t2.find(std::string("b")) == 'b');
	BOOST_CHECK(*t2.find(std::string("cab")) == 'b');
	BOOST_CHECK(*t2.find(std::string("cd")) == 'd');

	BOOST_CHECK(t2.size() == 5);
	BOOST_CHECK(t2.count() == 5);
}

BOOST_AUTO_TEST_CASE(trie_base__stream__2)
{
	test_trie_base t1{};

	std::stringstream s;
	s << t1;

	test_trie_base t2{};
	s >> t2;

	BOOST_CHECK(t1.m_c == t2.m_c);
	BOOST_CHECK(t1.size() == t2.size());

	BOOST_CHECK(t2.size() == 0);
	BOOST_CHECK(t2.count() == 0);
}

BOOST_AUTO_TEST_CASE(trie_base__english_words_txt__1)
{
	std::vector<std::string> words{};
	{
		std::string buf{};
		std::ifstream is(english_words_path);
		BOOST_REQUIRE(is.is_open());
		for (size_t i = 0; i < 10000 && std::getline(is, buf); ++i) words.push_back(buf);

		std::mt19937 mt;
		std::shuffle(words.begin(), words.end(), mt);
	}

	test_trie_base trie{};
	for (auto const& s : words) trie.insert(s);

	std::cout << "trie_base__english_words_txt__1" << std::endl;
	std::cout << "node  : " << trie.m_c.size() << std::endl;
	std::cout << "count : " << trie.count()    << std::endl;
	std::cout << "size  : " << trie.size()     << std::endl;
	std::cout << "vector: " << words.size()    << std::endl;

	BOOST_CHECK(trie.count() == words.size());
	BOOST_CHECK(trie.size() == words.size());
}

BOOST_AUTO_TEST_SUITE_END()
