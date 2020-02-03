// test/trie/trie_heap.cpp

#include <boost/test/unit_test.hpp>

#include "trie_base.hpp"

#include <wordring/trie/trie_heap.hpp>

BOOST_AUTO_TEST_SUITE(trie_heap__test)

BOOST_AUTO_TEST_CASE(trie_heap__construct__1)
{
}

// 内部 -----------------------------------------------------------------------

// void reserve(std::size_t n = 1024)
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

	wordring::static_vector<std::uint16_t, 257> sv{ 1 };
	
	auto i = trie.allocate(sv);

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

	wordring::static_vector<std::uint16_t, 257> sv{ 2, 4 };

	auto i = trie.allocate(sv);

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

	wordring::static_vector<std::uint16_t, 257> sv{ 0, 1 };

	auto i = trie.allocate(sv);

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
	trie.m_c.insert(trie.m_c.end(), 257, { 0, 0 });

	BOOST_CHECK(trie.is_free(1, { 256 }) == false);
}

BOOST_AUTO_TEST_SUITE_END()
