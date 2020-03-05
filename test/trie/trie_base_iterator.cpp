// test/trie/trie_base_iterator.cpp

#include <boost/test/unit_test.hpp>

#include <wordring/trie/trie_base.hpp>
#include <wordring/trie/trie_base_iterator.hpp>
#include <wordring/tree/tree_iterator.hpp>

#include <algorithm>
#include <fstream>
#include <memory>
#include <random>
#include <sstream>
#include <string>
#include <vector>

namespace
{
	using wordring::detail::trie_node;
	using base_iterator = wordring::detail::const_trie_base_iterator<std::vector<trie_node, std::allocator<trie_node>> const>;
	using base_trie     = wordring::detail::trie_base<>;

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

BOOST_AUTO_TEST_SUITE(const_trie_base_iterator__test)

// ----------------------------------------------------------------------------
// const_trie_base_iterator
// ----------------------------------------------------------------------------

// const_trie_base_iterator()
BOOST_AUTO_TEST_CASE(const_trie_base_iterator__construct__1)
{
	using namespace wordring;

	test_iterator it;
}

BOOST_AUTO_TEST_CASE(const_trie_base_iterator__construct__2)
{
	using namespace wordring;

	test_iterator it{};
}

// const_trie_base_iterator(container& c, index_type index)
BOOST_AUTO_TEST_CASE(const_trie_base_iterator__construct__3)
{
	using namespace wordring;

	test_trie trie{};
	test_iterator it(trie.m_c, 1);
}

// operator bool() const
BOOST_AUTO_TEST_CASE(const_trie_base_iterator__bool__1)
{
	using namespace wordring;

	test_trie trie{};
	trie.m_c = {
		{ 0,  0 }, // 0
		{ 1,  0 }, // 1
		{ 5,  1 }, // 2 
		{ -3, 1 }, // 3
		{ 5,  1 }, // 4
		{ 0,  0 }, // 5
		{ 5,  4 }, // 6
		{ -4, 6 }, // 7
		{ -2, 2 }, // 8
		{ -5, 4 }  // 9
	};

	// 親状態2に対してnull_valueによる遷移を追加
	trie.m_c.insert(trie.m_c.end(), 252, { 0, 0 });
	trie.m_c[261] = wordring::detail::trie_node{ 0, 2 };

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
	using namespace wordring;

	test_trie trie{};

	trie.insert(std::string("a"));
	BOOST_CHECK(trie.count() == 1);
	BOOST_CHECK(trie.find(std::string("a")).operator bool());
	BOOST_CHECK(trie.find(std::string("a")).operator!() == false);

	BOOST_CHECK(trie.find(std::string("c")).operator bool() == false);
}

BOOST_AUTO_TEST_CASE(const_trie_base_iterator__bool__3)
{
	using namespace wordring;

	test_trie trie{};

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
	using namespace wordring;

	test_trie trie{};

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
	using namespace wordring;

	test_trie trie{};
	
	trie.m_c = {
		{ 0,  0 }, // 0
		{ 1,  0 }, // 1
		{ 5,  1 }, // 2 
		{ -3, 1 }, // 3
		{ 5,  1 }, // 4
		{ -1, 2 }, // 5
		{ 5,  4 }, // 6
		{ -4, 6 }, // 7
		{ -2, 2 }, // 8
		{ -5, 4 }  // 9
	};

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
	using namespace wordring;

	test_trie trie{};

	trie.m_c = {
		{ 0,  0 }, // 0
		{ 1,  0 }, // 1
		{ 5,  1 }, // 2 
		{ -3, 1 }, // 3
		{ 5,  1 }, // 4
		{ -1, 2 }, // 5
		{ 5,  4 }, // 6
		{ -4, 6 }, // 7
		{ -2, 2 }, // 8
		{ -5, 4 }  // 9
	};

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
	using namespace wordring;

	test_trie trie{};

	trie.m_c = {
		{ 0,  0 }, // 0
		{ 1,  0 }, // 1
		{ 5,  1 }, // 2 
		{ -3, 1 }, // 3
		{ 5,  1 }, // 4
		{ -1, 2 }, // 5
		{ 5,  4 }, // 6
		{ -4, 6 }, // 7
		{ -2, 2 }, // 8
		{ -5, 4 }  // 9
	};

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
	using namespace wordring;

	test_trie trie{};

	trie.m_c = {
		{ 0,  0 }, // 0
		{ 1,  0 }, // 1
		{ 5,  1 }, // 2 
		{ -3, 1 }, // 3
		{ 5,  1 }, // 4
		{ -1, 2 }, // 5
		{ 5,  4 }, // 6
		{ -4, 6 }, // 7
		{ -2, 2 }, // 8
		{ -5, 4 }  // 9
	};

	BOOST_CHECK(*++test_iterator(trie.m_c, 2) == 2);
	BOOST_CHECK(*++test_iterator(trie.m_c, 3) == 3);
	BOOST_CHECK(*++test_iterator(trie.m_c, 5) == 3);
	BOOST_CHECK(*++test_iterator(trie.m_c, 6) == 4);
}

BOOST_AUTO_TEST_CASE(const_trie_base_iterator__increment__2)
{
	using namespace wordring;

	test_trie trie{};

	trie.m_c = {
		{ 0,  0 }, // 0
		{ 1,  0 }, // 1
		{ 5,  1 }, // 2 
		{ -3, 1 }, // 3
		{ 5,  1 }, // 4
		{ -1, 2 }, // 5
		{ 5,  4 }, // 6
		{ -4, 6 }, // 7
		{ -2, 2 }, // 8
		{ -5, 4 }  // 9
	};

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
	using namespace wordring;

	test_trie trie{};

	trie.m_c = {
		{ 0,  0 }, // 0
		{ 1,  0 }, // 1
		{ 5,  1 }, // 2 
		{ -3, 1 }, // 3
		{ 5,  1 }, // 4
		{ -1, 2 }, // 5
		{ 5,  4 }, // 6
		{ -4, 6 }, // 7
		{ -2, 2 }, // 8
		{ -5, 4 }  // 9
	};

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
	using namespace wordring;

	test_trie trie{};

	trie.m_c = {
		{ 0,  0 }, // 0
		{ 1,  0 }, // 1
		{ 5,  1 }, // 2 
		{ -3, 1 }, // 3
		{ 5,  1 }, // 4
		{ -1, 2 }, // 5
		{ 5,  4 }, // 6
		{ -4, 6 }, // 7
		{ -2, 2 }, // 8
		{ -5, 4 }  // 9
	};

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
	using namespace wordring;

	test_trie trie{};

	std::string s;

	test_iterator it0{};
	it0.string(s);
	BOOST_CHECK(s == "");

	test_iterator it1 = trie.insert(std::string("\1"));
	it1.string(s);
	BOOST_CHECK(s == "\1");

	test_iterator it2 = trie.insert(std::string("\1\3"));
	it2.string(s);
	BOOST_CHECK(s == "\1\3");

	test_iterator it3 = trie.insert(std::string("\2"));
	it3.string(s);
	BOOST_CHECK(s == "\2");

	test_iterator it4 = trie.insert(std::string("\3\1\2"));
	it4.string(s);
	BOOST_CHECK(s == "\3\1\2");

	test_iterator it5 = trie.insert(std::string("\3\4"));
	it5.string(s);
	BOOST_CHECK(s == "\3\4");
}

// std::optional<const_trie_base_iterator> parent() const
BOOST_AUTO_TEST_CASE(const_trie_base_iterator__parent__1)
{
	using namespace wordring;

	test_trie trie{};

	test_iterator it1 = trie.insert(std::string("\1"));
	BOOST_CHECK(it1.parent().parent() == trie.cend());

	test_iterator it2 = trie.insert(std::string("\1\3"));
	BOOST_CHECK(*it2.parent() == 1);

	test_iterator it3 = trie.insert(std::string("\2"));
	BOOST_CHECK(it3.parent().parent() == trie.cend());

	test_iterator it4 = trie.insert(std::string("\3\1\2"));
	BOOST_CHECK(*it4.parent() == 1);
	BOOST_CHECK(*it4.parent().parent() == 3);

	test_iterator it5 = trie.insert(std::string("\3\4"));
	BOOST_CHECK(*it5.parent() == 3);
	BOOST_CHECK(it5.parent().parent().parent() == trie.cend());
}


// const_trie_base_iterator begin() const
BOOST_AUTO_TEST_CASE(const_trie_base_iterator__begin__1)
{
	using namespace wordring;

	test_trie trie{};

	trie.m_c = {
		{ 0,  0 }, // 0
		{ 1,  0 }, // 1
		{ 5,  1 }, // 2 
		{ -3, 1 }, // 3
		{ 5,  1 }, // 4
		{ -1, 2 }, // 5
		{ 5,  4 }, // 6
		{ -4, 6 }, // 7
		{ -2, 2 }, // 8
		{ -5, 4 }  // 9
	};

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
	using namespace wordring;

	test_trie trie{};

	trie.m_c = {
		{ 0,  0 }, // 0
		{ 1,  0 }, // 1
		{ 5,  1 }, // 2 
		{ -3, 1 }, // 3
		{ 5,  1 }, // 4
		{ -1, 2 }, // 5
		{ 5,  4 }, // 6
		{ -4, 6 }, // 7
		{ -2, 2 }, // 8
		{ -5, 4 }  // 9
	};

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
	using namespace wordring;

	test_trie trie{};

	trie.m_c = {
		{ 0,  0 }, // 0
		{ 1,  0 }, // 1
		{ 5,  1 }, // 2 
		{ -3, 1 }, // 3
		{ 5,  1 }, // 4
		{ -1, 2 }, // 5
		{ 5,  4 }, // 6
		{ -4, 6 }, // 7
		{ -2, 2 }, // 8
		{ -5, 4 }  // 9
	};

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
	using namespace wordring;

	test_trie trie{};

	trie.m_c = {
		{  0, 0 }, // 0
		{  1, 0 }, // 1
		{  5, 1 }, // 2 
		{ -3, 1 }, // 3
		{  5, 1 }, // 4
		{ -1, 2 }, // 5
		{  5, 4 }, // 6
		{ -4, 6 }, // 7
		{ -2, 2 }, // 8
		{ -5, 4 }  // 9
	};

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
	using namespace wordring;

	test_trie trie{};

	trie.m_c = {
		{ 0,  0 }, // 0
		{ 1,  0 }, // 1
		{ 5,  1 }, // 2 
		{ -3, 1 }, // 3
		{ 5,  1 }, // 4
		{ -1, 2 }, // 5
		{ 5,  4 }, // 6
		{ -4, 6 }, // 7
		{ -2, 2 }, // 8
		{ -5, 4 }  // 9
	};

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
	using namespace wordring;

	test_trie trie{};

	trie.m_c = {
		{ 0, 0 },  // 0
		{ 1, 0 },  // 1
		{ 5, -1 }, // 2
		{ 5, 1 }   // 3
	};

	BOOST_CHECK(test_iterator(trie.m_c, 1).mother() == 0);
	BOOST_CHECK(test_iterator(trie.m_c, 2).mother() == 0);
	BOOST_CHECK(test_iterator(trie.m_c, 3).mother() == 1);
}

// index_type base() const
BOOST_AUTO_TEST_CASE(const_trie_base_iterator__base__1)
{
	using namespace wordring;

	test_trie trie{};

	trie.m_c = {
		{ 0,  0 }, // 0
		{ 1,  0 }, // 1
		{ 5,  1 }, // 2 
		{ -3, 1 }, // 3
		{ 5,  1 }, // 4
		{ -1, 2 }, // 5
		{ 5,  4 }, // 6
		{ -4, 6 }, // 7
		{ -2, 2 }, // 8
		{ -5, 4 }  // 9
	};

	BOOST_CHECK(test_iterator(trie.m_c, 2).base() == 1);
	BOOST_CHECK(test_iterator(trie.m_c, 3).base() == 1);
	BOOST_CHECK(test_iterator(trie.m_c, 4).base() == 1);
	BOOST_CHECK(test_iterator(trie.m_c, 5).base() == 5);
	BOOST_CHECK(test_iterator(trie.m_c, 6).base() == 5);
	BOOST_CHECK(test_iterator(trie.m_c, 7).base() == 5);
	BOOST_CHECK(test_iterator(trie.m_c, 8).base() == 5);
	BOOST_CHECK(test_iterator(trie.m_c, 9).base() == 5);
}

// index_type limit() const
BOOST_AUTO_TEST_CASE(const_trie_base_iterator__tail__1)
{
	using namespace wordring;

	test_trie trie{};

	trie.m_c = {
		{ 0,  0 }, // 0
		{ 1,  0 }, // 1
		{ 5,  1 }, // 2 
		{ -3, 1 }, // 3
		{ 5,  1 }, // 4
		{ -1, 2 }, // 5
		{ 5,  4 }, // 6
		{ -4, 6 }, // 7
		{ -2, 2 }, // 8
		{ -5, 4 }  // 9
	};

	BOOST_CHECK(test_iterator(trie.m_c, 1).limit() == 10);
	BOOST_CHECK(test_iterator(trie.m_c, 2).limit() == 10);
	BOOST_CHECK(test_iterator(trie.m_c, 3).limit() == 10);
	BOOST_CHECK(test_iterator(trie.m_c, 4).limit() == 10);
	BOOST_CHECK(test_iterator(trie.m_c, 5).limit() == 10);
	BOOST_CHECK(test_iterator(trie.m_c, 6).limit() == 10);
	BOOST_CHECK(test_iterator(trie.m_c, 7).limit() == 10);
	BOOST_CHECK(test_iterator(trie.m_c, 8).limit() == 10);
	BOOST_CHECK(test_iterator(trie.m_c, 9).limit() == 10);
}

// bool has_null() const
BOOST_AUTO_TEST_CASE(const_trie_base_iterator__has_null__1)
{
	using namespace wordring;

	test_trie trie{};

	trie.m_c = {
		{ 0, 0 }, // 0
		{ 1, 0 }, // 1
		{ 1, 1 }, // 2
		{ 0, 0 }, // 3
		{ 0, 2 }  // 4
	};
	trie.m_c.insert(trie.m_c.cend(), 252, { 0, 0 });
	trie.m_c.push_back({ 0, 2 }); // 257
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
	using namespace wordring;

	test_trie trie{};

	std::vector<std::string> v{ "a", "ac", "b", "cab", "cd" };
	trie.assign(v.begin(), v.end());

	test_iterator it1 = trie.search(std::string("a"));
	BOOST_CHECK(it1.has_sibling());

	test_iterator it2 = trie.search(std::string("b"));
	BOOST_CHECK(it2.has_sibling());

	test_iterator it3 = trie.search(std::string("c"));
	BOOST_CHECK(it3.has_sibling());
}

BOOST_AUTO_TEST_CASE(const_trie_base_iterator__has_sibling__2)
{
	using namespace wordring;

	test_trie trie{};

	std::vector<std::string> v{ "a", "ac", "b", "cab", "cd" };
	trie.assign(v.begin(), v.end());

	test_iterator it1 = trie.search(std::string("cab"));
	BOOST_CHECK(it1.has_sibling() == false);
}

BOOST_AUTO_TEST_CASE(const_trie_base_iterator__has_sibling__3)
{
	using namespace wordring;

	test_trie trie{};

	std::vector<std::string> v{ "a", "ac", "b", "cab", "cd" };
	trie.assign(v.begin(), v.end());

	test_iterator it1 = trie.search(std::string("ac"));
	BOOST_CHECK(it1.has_sibling() == false);
}

BOOST_AUTO_TEST_CASE(const_trie_base_iterator__has_sibling__4)
{
	using namespace wordring;

	test_trie trie{};

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
	using namespace wordring;

	test_trie trie{};
	BOOST_CHECK(test_iterator(trie.m_c, 1) == test_iterator(trie.m_c, 1));
}

BOOST_AUTO_TEST_CASE(const_trie_base_iterator__equal__2)
{
	using namespace wordring;

	BOOST_CHECK(test_iterator() == test_iterator());
}

BOOST_AUTO_TEST_CASE(const_trie_base_iterator__equal__3)
{
	using namespace wordring;

	test_trie trie{};

	trie.m_c = {
		{ 0, 0 }, // 0
		{ 1, 0 }, // 1
		{ 5, 1 }, // 2
	};

	BOOST_CHECK((test_iterator(trie.m_c, 1) == test_iterator(trie.m_c, 2)) == false);
}

// inline bool operator!=(const_trie_base_iterator<Container1> const& lhs, const_trie_base_iterator<Container1> const& rhs)
BOOST_AUTO_TEST_CASE(const_trie_base_iterator__not_equal__1)
{
	using namespace wordring;

	test_trie trie{};
	BOOST_CHECK((test_iterator(trie.m_c, 1) != test_iterator(trie.m_c, 1)) == false);
}

BOOST_AUTO_TEST_CASE(const_trie_base_iterator__not_equal__2)
{
	using namespace wordring;

	BOOST_CHECK((test_iterator() != test_iterator()) == false);
}

BOOST_AUTO_TEST_CASE(const_trie_base_iterator__not_equal__3)
{
	using namespace wordring;

	test_trie trie{};

	trie.m_c = {
		{ 0, 0 }, // 0
		{ 1, 0 }, // 1
		{ 5, 1 }, // 2
	};

	BOOST_CHECK(test_iterator(trie.m_c, 1) != test_iterator(trie.m_c, 2));
}

BOOST_AUTO_TEST_SUITE_END()
