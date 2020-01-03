// test/trie/trie.cpp

#include <boost/test/unit_test.hpp>

#include <wordring/trie/list_trie.hpp>
#include <wordring/trie/trie.hpp>

#include <bitset>
#include <string>
#include <vector>

BOOST_AUTO_TEST_SUITE(trie__test)

// ----------------------------------------------------------------------------
// trie_base
// ----------------------------------------------------------------------------

class test_trie_base : public wordring::trie_base<>
{
public:
	using wordring::trie_base<>::reserve;
	using wordring::trie_base<>::free;
	using wordring::trie_base<>::allocate;
	using wordring::trie_base<>::insert_child;

	using wordring::trie_base<>::m_c;
};

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

// index_type allocate(index_type index, index_type before = 0)
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

	BOOST_CHECK(trie.allocate(3) == 1);

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

	BOOST_CHECK(trie.allocate(1) == 0);

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

	BOOST_CHECK(trie.allocate(2) == 0);

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

	BOOST_CHECK(trie.allocate(8) == 7);

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

	BOOST_CHECK(trie.allocate(8) == 0);

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

	BOOST_CHECK(trie.allocate(1) ==0);

	BOOST_CHECK(trie.m_c.at(0).m_base == 0);
	BOOST_CHECK(trie.m_c.at(0).m_check == 0);
	BOOST_CHECK(trie.m_c.at(1).m_check == 0);
}

BOOST_AUTO_TEST_CASE(trie_base__allocate__7)
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

	BOOST_CHECK(trie.allocate(6, trie.allocate(3)) == 5);

	BOOST_CHECK(trie.m_c.at(0).m_base == -8);
	BOOST_CHECK(trie.m_c.at(0).m_check == -1);
	BOOST_CHECK(trie.m_c.at(1).m_check == -5);
	BOOST_CHECK(trie.m_c.at(2).m_check == 0);
	BOOST_CHECK(trie.m_c.at(3).m_check == 0);
	BOOST_CHECK(trie.m_c.at(4).m_check == 0);
	BOOST_CHECK(trie.m_c.at(5).m_check == -7);
	BOOST_CHECK(trie.m_c.at(6).m_check == 0);
	BOOST_CHECK(trie.m_c.at(7).m_check == -8);
	BOOST_CHECK(trie.m_c.at(8).m_check == 0);
}

BOOST_AUTO_TEST_CASE(trie_base__allocate__8)
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

	BOOST_CHECK(trie.allocate(3, trie.allocate(1)) == 0);

	BOOST_CHECK(trie.m_c.at(0).m_base == -8);
	BOOST_CHECK(trie.m_c.at(0).m_check == -5);
	BOOST_CHECK(trie.m_c.at(1).m_check == 0);
	BOOST_CHECK(trie.m_c.at(2).m_check == 0);
	BOOST_CHECK(trie.m_c.at(3).m_check == 0);
	BOOST_CHECK(trie.m_c.at(4).m_check == 0);
	BOOST_CHECK(trie.m_c.at(5).m_check == -6);
	BOOST_CHECK(trie.m_c.at(6).m_check == -7);
	BOOST_CHECK(trie.m_c.at(7).m_check == -8);
	BOOST_CHECK(trie.m_c.at(8).m_check == 0);
}

BOOST_AUTO_TEST_CASE(trie_base__allocate__9)
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

	BOOST_CHECK(trie.allocate(8, trie.allocate(1)) == 7);

	BOOST_CHECK(trie.m_c.at(0).m_base == -7);
	BOOST_CHECK(trie.m_c.at(0).m_check == -3);
	BOOST_CHECK(trie.m_c.at(1).m_check == 0);
	BOOST_CHECK(trie.m_c.at(2).m_check == 0);
	BOOST_CHECK(trie.m_c.at(3).m_check == -5);
	BOOST_CHECK(trie.m_c.at(4).m_check == 0);
	BOOST_CHECK(trie.m_c.at(5).m_check == -6);
	BOOST_CHECK(trie.m_c.at(6).m_check == -7);
	BOOST_CHECK(trie.m_c.at(7).m_check == 0);
	BOOST_CHECK(trie.m_c.at(8).m_check == 0);
}

// index_type allocate(Range1 dests)
BOOST_AUTO_TEST_CASE(trie_base__allocate__10)
{
	test_trie_base trie{};
	trie.m_c.clear();

	trie.m_c.emplace_back(-5, -1); // 0
	trie.m_c.emplace_back(0, -3);  // 1
	trie.m_c.emplace_back(0, 0);   // 2
	trie.m_c.emplace_back(0, -5);  // 3
	trie.m_c.emplace_back(0, 0);   // 4
	trie.m_c.emplace_back(0, 0);   // 5

	auto i = trie.allocate(std::vector<std::uint8_t>{ 1 });

	BOOST_CHECK(i == 2);

	BOOST_CHECK(trie.m_c.at(0).m_base == -5);
	BOOST_CHECK(trie.m_c.at(0).m_check == -1);
	BOOST_CHECK(trie.m_c.at(1).m_check == -5);
	BOOST_CHECK(trie.m_c.at(2).m_check == 0);
	BOOST_CHECK(trie.m_c.at(3).m_check == 0);
	BOOST_CHECK(trie.m_c.at(4).m_check == 0);
	BOOST_CHECK(trie.m_c.at(5).m_check == 0);
}

BOOST_AUTO_TEST_CASE(trie_base__allocate__11)
{
	test_trie_base trie{};
	trie.m_c.clear();

	trie.m_c.emplace_back(-5, -1); // 0
	trie.m_c.emplace_back(0, -3);  // 1
	trie.m_c.emplace_back(0, 0);   // 2
	trie.m_c.emplace_back(0, -5);  // 3
	trie.m_c.emplace_back(0, 0);   // 4
	trie.m_c.emplace_back(0, 0);   // 5

	auto i = trie.allocate(std::vector<std::uint8_t>{ 2, 4 });

	BOOST_CHECK(i == 1);

	BOOST_CHECK(trie.m_c.at(0).m_base == -1);
	BOOST_CHECK(trie.m_c.at(0).m_check == -1);
	BOOST_CHECK(trie.m_c.at(1).m_check == 0);
	BOOST_CHECK(trie.m_c.at(2).m_check == 0);
	BOOST_CHECK(trie.m_c.at(3).m_check == 0);
	BOOST_CHECK(trie.m_c.at(4).m_check == 0);
	BOOST_CHECK(trie.m_c.at(5).m_check == 0);
}

BOOST_AUTO_TEST_CASE(trie_base__allocate__12)
{
	test_trie_base trie{};
	trie.m_c.clear();

	trie.m_c.emplace_back(-5, -1); // 0
	trie.m_c.emplace_back(0, -3);  // 1
	trie.m_c.emplace_back(0, 0);   // 2
	trie.m_c.emplace_back(0, -5);  // 3
	trie.m_c.emplace_back(0, 0);   // 4
	trie.m_c.emplace_back(0, 0);   // 5

	auto i = trie.allocate(std::vector<std::uint8_t>{ 0, 1 });

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

// void free(index_type index, index_type before = 0)
BOOST_AUTO_TEST_CASE(trie_base__free__1)
{
	test_trie_base trie{};
	trie.m_c.clear();

	trie.m_c.emplace_back(-2, -2); // 0
	trie.m_c.emplace_back(0, 0);   // 1
	trie.m_c.emplace_back(0, 0);   // 2

	trie.free(1);

	BOOST_CHECK(trie.m_c.at(0).m_base == -2);
	BOOST_CHECK(trie.m_c.at(0).m_check == -1);
	BOOST_CHECK(trie.m_c.at(1).m_check == -2);
	BOOST_CHECK(trie.m_c.at(2).m_check == 0);
}

BOOST_AUTO_TEST_CASE(trie_base__free__2)
{
	test_trie_base trie{};
	trie.m_c.clear();

	trie.m_c.emplace_back(-5, -1); // 0
	trie.m_c.emplace_back(0, -5);  // 1
	trie.m_c.emplace_back(0, 0);   // 2
	trie.m_c.emplace_back(0, 0);   // 3
	trie.m_c.emplace_back(0, 0);   // 4
	trie.m_c.emplace_back(0, 0);   // 5

	trie.free(3);

	BOOST_CHECK(trie.m_c.at(0).m_base == -5);
	BOOST_CHECK(trie.m_c.at(0).m_check == -1);
	BOOST_CHECK(trie.m_c.at(1).m_check == -3);
	BOOST_CHECK(trie.m_c.at(2).m_check == 0);
	BOOST_CHECK(trie.m_c.at(3).m_check == -5);
	BOOST_CHECK(trie.m_c.at(4).m_check == 0);
	BOOST_CHECK(trie.m_c.at(5).m_check == 0);
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

	trie.m_c.emplace_back(0, 0); // 0
	trie.m_c.emplace_back(0, 0); // 1

	trie.free(1);

	BOOST_CHECK(trie.m_c.at(0).m_base == -1);
	BOOST_CHECK(trie.m_c.at(0).m_check == -1);
	BOOST_CHECK(trie.m_c.at(1).m_check == 0);
}

// bool is_free(index_type base, Range1 dests) const
BOOST_AUTO_TEST_CASE(trie_base__is_free__1)
{

}

// index_type insert_child(index_type parent, Range1 range)
BOOST_AUTO_TEST_CASE(trie_base__insert_child__1)
{
	using namespace wordring;

	std::vector<std::string> list{ "\1", "\1\3", "\2", "\3\1\2", "\3\4" };
	list_trie_iterator lt{ list.begin(), list.end() };

	test_trie_base trie{};

	trie.insert_child(1, lt);
	trie.insert_child(2, lt.begin());
}

BOOST_AUTO_TEST_CASE(trie_base__constrcut__1)
{
	using namespace wordring;

	std::vector<std::string> list{ "a", "ac", "b", "cab", "cd" };
	trie_base trie{};
	//trie.insert(list.cbegin(), list.cend());
}

BOOST_AUTO_TEST_SUITE_END()
