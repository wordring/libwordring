// test/trie/const_trie_base_iterator.cpp

#include <boost/test/unit_test.hpp>

#include "trie_base.hpp"

#include <wordring/trie/trie_base.hpp>

#include <algorithm>
#include <fstream>
#include <memory>
#include <random>
#include <sstream>
#include <string>
#include <vector>


BOOST_AUTO_TEST_SUITE(const_trie_base_iterator__test)

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

BOOST_AUTO_TEST_SUITE_END()
