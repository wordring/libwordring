// test/trie/trie.cpp

#include <boost/test/unit_test.hpp>

#include <wordring/trie/list_trie.hpp>
#include <wordring/trie/trie.hpp>
#include <wordring/tree/tree_iterator.hpp>

#include <bitset>
#include <string>
#include <vector>

BOOST_AUTO_TEST_SUITE(trie__test)

class test_trie_base : public wordring::trie_base<>
{
public:
	using wordring::trie_base<>::container;

	using wordring::trie_base<>::reserve;
	using wordring::trie_base<>::free;
	using wordring::trie_base<>::allocate;
	using wordring::trie_base<>::add;

	using wordring::trie_base<>::m_c;

	std::size_t count_s() const
	{
		std::size_t n = 1; // 状態1のCHECKは常に0なので
		for (std::size_t i = 1; i < m_c.size(); ++i) if (0 < (m_c.data() + i)->m_check) ++n;
		return n;
	}

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

class test_iterator : public wordring::const_trie_base_iterator<test_trie_base::container const>
{
public:
	using wordring::const_trie_base_iterator<test_trie_base::container const>::m_c;
	//using wordring::const_trie_base_iterator<test_trie_base::container>::m_base;
	using wordring::const_trie_base_iterator<test_trie_base::container const>::m_index;

	using wordring::const_trie_base_iterator<test_trie_base::container const>::find;

public:
	test_iterator() : const_trie_base_iterator() {}

	test_iterator(test_trie_base::container& c, index_type index)
		: const_trie_base_iterator(c, index) {}

	test_iterator(const_trie_base_iterator const& base)
		: const_trie_base_iterator(base) {}
	/*
	operator wordring::const_trie_base_iterator<wordring::trie_base<>::container const>() const
	{
		return wordring::const_trie_base_iterator<wordring::trie_base<>::container const>(*m_c, m_index);
	}
	*/
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
	//BOOST_CHECK(test_iterator(trie.m_c, 1)[256] == test_iterator(trie.m_c, 1).end());

	BOOST_CHECK(test_iterator(trie.m_c, 2)[4] == test_iterator(trie.m_c, 2).end());
	//BOOST_CHECK(test_iterator(trie.m_c, 2)[256] == test_iterator(trie.m_c, 2).end());

	BOOST_CHECK(test_iterator(trie.m_c, 3)[0] == test_iterator(trie.m_c, 3).end());
	BOOST_CHECK(test_iterator(trie.m_c, 3)[1] == test_iterator(trie.m_c, 3).end());
	//BOOST_CHECK(test_iterator(trie.m_c, 3)[256] == test_iterator(trie.m_c, 3).end());

	BOOST_CHECK(test_iterator(trie.m_c, 4)[0] == test_iterator(trie.m_c, 4).end());
	BOOST_CHECK(test_iterator(trie.m_c, 4)[2] == test_iterator(trie.m_c, 4).end());
	//BOOST_CHECK(test_iterator(trie.m_c, 4)[256] == test_iterator(trie.m_c, 4).end());

	BOOST_CHECK(test_iterator(trie.m_c, 5)[0] == test_iterator(trie.m_c, 5).end());
	BOOST_CHECK(test_iterator(trie.m_c, 5)[1] == test_iterator(trie.m_c, 5).end());
	//BOOST_CHECK(test_iterator(trie.m_c, 5)[256] == test_iterator(trie.m_c, 5).end());

	BOOST_CHECK(test_iterator(trie.m_c, 8)[0] == test_iterator(trie.m_c, 8).end());
	BOOST_CHECK(test_iterator(trie.m_c, 8)[1] == test_iterator(trie.m_c, 8).end());
	//BOOST_CHECK(test_iterator(trie.m_c, 8)[256] == test_iterator(trie.m_c, 8).end());

	BOOST_CHECK(test_iterator(trie.m_c, 6)[0] == test_iterator(trie.m_c, 6).end());
	BOOST_CHECK(test_iterator(trie.m_c, 6)[1] == test_iterator(trie.m_c, 6).end());
	//BOOST_CHECK(test_iterator(trie.m_c, 6)[256] == test_iterator(trie.m_c, 6).end());

	BOOST_CHECK(test_iterator(trie.m_c, 9)[0] == test_iterator(trie.m_c, 9).end());
	BOOST_CHECK(test_iterator(trie.m_c, 9)[1] == test_iterator(trie.m_c, 9).end());
	//BOOST_CHECK(test_iterator(trie.m_c, 9)[256] == test_iterator(trie.m_c, 9).end());

	BOOST_CHECK(test_iterator(trie.m_c, 7)[0] == test_iterator(trie.m_c, 7).end());
	BOOST_CHECK(test_iterator(trie.m_c, 7)[1] == test_iterator(trie.m_c, 7).end());
	//BOOST_CHECK(test_iterator(trie.m_c, 7)[256] == test_iterator(trie.m_c, 7).end());
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

BOOST_AUTO_TEST_CASE(const_trie_base_iterator__parent__1)
{
	test_trie_base trie{};

	trie.begin().parent();
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
	BOOST_CHECK(it1.find(5, 256, 1) == 1 + 256);
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

// void reserve(std::size_t n = 1024)
BOOST_AUTO_TEST_CASE(trie_base__reserve__1)
{
	test_trie_base trie{};
	trie.m_c.clear();
	trie.m_c.emplace_back(0, 0);

	trie.reserve(0);

	BOOST_CHECK(trie.m_c.size() == 1);
	BOOST_CHECK(trie.m_c.front().m_base == 0);
	BOOST_CHECK(trie.m_c.front().m_check == 0);
}

BOOST_AUTO_TEST_CASE(trie_base__reserve__2)
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

BOOST_AUTO_TEST_CASE(trie_base__reserve__3)
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

BOOST_AUTO_TEST_CASE(trie_base__reserve__4)
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

BOOST_AUTO_TEST_CASE(trie_base__reserve__5)
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

BOOST_AUTO_TEST_CASE(trie_base__reserve__6)
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

	BOOST_CHECK(trie.m_c.at(0).m_base == -5);
	BOOST_CHECK(trie.m_c.at(0).m_check == -1);
	BOOST_CHECK(trie.m_c.at(1).m_check == -5);
	BOOST_CHECK(trie.m_c.at(2).m_check == 0);
	BOOST_CHECK(trie.m_c.at(3).m_check == 0);
	BOOST_CHECK(trie.m_c.at(4).m_check == 0);
	BOOST_CHECK(trie.m_c.at(5).m_check == 0);
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

	BOOST_CHECK(trie.m_c.at(0).m_base == -1);
	BOOST_CHECK(trie.m_c.at(0).m_check == -1);
	BOOST_CHECK(trie.m_c.at(1).m_check == 0);
	BOOST_CHECK(trie.m_c.at(2).m_check == 0);
	BOOST_CHECK(trie.m_c.at(3).m_check == 0);
	BOOST_CHECK(trie.m_c.at(4).m_check == 0);
	BOOST_CHECK(trie.m_c.at(5).m_check == 0);
}

BOOST_AUTO_TEST_CASE(trie_base__allocate__9)
{
	test_trie_base trie{};
	trie.m_c.clear();

	trie.m_c.emplace_back(-5, -1); // 0
	trie.m_c.emplace_back(0, -3);  // 1
	trie.m_c.emplace_back(0, 0);   // 2
	trie.m_c.emplace_back(0, -5);  // 3
	trie.m_c.emplace_back(0, 0);   // 4
	trie.m_c.emplace_back(0, 0);   // 5

	auto i = trie.allocate(wordring::static_vector<std::uint16_t, 257>{ 0, 1 });

	BOOST_CHECK(i == 5);

	BOOST_CHECK(trie.m_c.at(0).m_check == -1);
	BOOST_CHECK(trie.m_c.at(1).m_check == -3);
	BOOST_CHECK(trie.m_c.at(2).m_check == 0);
	BOOST_CHECK(trie.m_c.at(3).m_check == -7);
	BOOST_CHECK(trie.m_c.at(4).m_check == 0);
	BOOST_CHECK(trie.m_c.at(5).m_check == 0);
	BOOST_CHECK(trie.m_c.at(6).m_check == 0);
	BOOST_CHECK(trie.m_c.at(7).m_check == -8);
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

BOOST_AUTO_TEST_CASE(trie_base__modify__1)
{
	/*
	using namespace wordring;

	static_vector<std::uint16_t, 257> sv1{ 1, 2, 3 };
	static_vector<std::uint16_t, 257> sv2{ 0, 3 };
	static_vector<std::uint16_t, 257> sv3{ 1, 4 };
	static_vector<std::uint16_t, 257> sv4{ 2 };

	test_trie_base trie{};

	trie.insert_children(1, sv1); // [1]0, [1]3, [2], [3]12, [3]4
	trie.insert_children(2, sv2); // 1[0], 1[3]
	trie.insert_children(4, sv3); // 3[1]2, 3[4]
	trie.insert_children(6, sv4); // 31[2]

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

	trie.allocate(10);
	trie.allocate(13);
	trie.allocate(15);
	trie.allocate(18);

	std::vector<int> v{ 1 };

	BOOST_CHECK(v.begin() == v.cbegin());
	*/
	//trie.move(5, 10, static_vector<std::uint16_t, 257>{ 8, 5, 0, 3 });
}

// bool is_free(index_type base, Range1 dests) const
BOOST_AUTO_TEST_CASE(trie_base__is_free__1)
{

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

// 衝突無しの動的構築
BOOST_AUTO_TEST_CASE(trie_base__add__2)
{
	using namespace wordring;

	test_trie_base trie{};
	trie.add(test_iterator(trie.m_c, 1), { 1, 2, 3 });
	trie.add(test_iterator(trie.m_c, 2), { 0, 3 });
	trie.add(test_iterator(trie.m_c, 4), { 1, 4 });
	trie.add(test_iterator(trie.m_c, 6), 2);

	trie.add(test_iterator(trie.m_c, 2), 5);

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

// 衝突有りの動的構築
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
	BOOST_CHECK(trie.m_c[2].m_base == 8);
	BOOST_CHECK(trie.m_c[2].m_check == 1);
	BOOST_CHECK(trie.m_c[3].m_check == 1);
	BOOST_CHECK(trie.m_c[4].m_base == 5);
	BOOST_CHECK(trie.m_c[4].m_check == 1);
	BOOST_CHECK(trie.m_c[5].m_check == 0);
	BOOST_CHECK(trie.m_c[6].m_base == 5);
	BOOST_CHECK(trie.m_c[6].m_check == 4);
	BOOST_CHECK(trie.m_c[7].m_check == 6);
	BOOST_CHECK(trie.m_c[9].m_check == 4);

	BOOST_CHECK(trie.m_c[8].m_check == 2);
	BOOST_CHECK(trie.m_c[11].m_check == 2);
	BOOST_CHECK(trie.m_c[12].m_check == 2);
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
	test_iterator it2 = trie.insert(std::string("\1"));
	BOOST_CHECK(trie.count_s() == 2);
	BOOST_CHECK(it2.m_index == 2);
	test_iterator it4 = trie.insert(std::string("\1\3"));
	test_iterator it3 = trie.insert(std::string("\2"));
	test_iterator it10 = trie.insert(std::string("\3\1\2"));
	test_iterator it11 = trie.insert(std::string("\3\4"));

	BOOST_CHECK(trie.count() == 5);

}

BOOST_AUTO_TEST_CASE(trie_base__insert__4)
{
	test_trie_base trie{};
	trie.insert(std::string("\1"));
	trie.insert(std::string("\1\3"));
}

BOOST_AUTO_TEST_CASE(trie_base__insert__5)
{
	test_trie_base trie{};
	trie.insert(std::string("\1\3"));
	trie.insert(std::string("\1"));
}


BOOST_AUTO_TEST_CASE(trie_base__constrcut__1)
{
	using namespace wordring;

	std::vector<std::string> list{ "a", "ac", "b", "cab", "cd" };
	trie_base trie{};
	//trie.insert(list.cbegin(), list.cend());
}

BOOST_AUTO_TEST_SUITE_END()
