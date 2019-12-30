// test/trie/trie.cpp

#include <boost/test/unit_test.hpp>

#include <wordring/trie/trie.hpp>

#include <bitset>
#include <string>
#include <vector>

BOOST_AUTO_TEST_SUITE(trie__test)

class test_trie_base : public wordring::trie_base<>
{
public:
	using wordring::trie_base<>::reserve;
	using wordring::trie_base<>::free;
	using wordring::trie_base<>::allocate;

	using wordring::trie_base<>::m_container;
};

BOOST_AUTO_TEST_CASE(trie__constrcut__1)
{
	using namespace wordring;

	std::vector<std::string> list{ "a", "ac", "b", "cab", "cd" };
	trie_base trie{};
	//trie.insert(list.cbegin(), list.cend());
}

BOOST_AUTO_TEST_CASE(trie__reserve__1)
{
	test_trie_base trie{};

	trie.reserve(0);

	BOOST_CHECK(trie.m_container.size() == 1);
	BOOST_CHECK(trie.m_container.front().m_base == 0);
	BOOST_CHECK(trie.m_container.front().m_check == 0);
}

BOOST_AUTO_TEST_CASE(trie__reserve__2)
{
	test_trie_base trie{};

	trie.reserve(1);

	BOOST_CHECK(trie.m_container.size() == 2);
	BOOST_CHECK(trie.m_container.at(0).m_base == -1);
	BOOST_CHECK(trie.m_container.at(0).m_check == -1);
	BOOST_CHECK(trie.m_container.at(1).m_check == 0);
}

BOOST_AUTO_TEST_CASE(trie__reserve__3)
{
	test_trie_base trie{};

	trie.reserve(4);

	BOOST_CHECK(trie.m_container.size() == 5);

	BOOST_CHECK(trie.m_container.at(0).m_base == -4);
	BOOST_CHECK(trie.m_container.at(0).m_check == -1);
	BOOST_CHECK(trie.m_container.at(1).m_check == -2);
	BOOST_CHECK(trie.m_container.at(2).m_check == -3);
	BOOST_CHECK(trie.m_container.at(3).m_check == -4);
	BOOST_CHECK(trie.m_container.at(4).m_check == 0);
}

BOOST_AUTO_TEST_CASE(trie__reserve__4)
{
	test_trie_base trie{};

	trie.m_container.front() = wordring::trie_node{ -3, -1 };
	trie.m_container.emplace_back(0, -3);
	trie.m_container.emplace_back(0, 0);
	trie.m_container.emplace_back(0, 0);
	trie.m_container.emplace_back(0, 0);

	trie.reserve(4);

	BOOST_CHECK(trie.m_container.size() == 9);

	BOOST_CHECK(trie.m_container.at(0).m_base == -8);
	BOOST_CHECK(trie.m_container.at(0).m_check == -1);
	BOOST_CHECK(trie.m_container.at(1).m_check == -3);
	BOOST_CHECK(trie.m_container.at(2).m_check == 0);
	BOOST_CHECK(trie.m_container.at(3).m_check == -5);
	BOOST_CHECK(trie.m_container.at(4).m_check == 0);
	BOOST_CHECK(trie.m_container.at(5).m_check == -6);
	BOOST_CHECK(trie.m_container.at(6).m_check == -7);
	BOOST_CHECK(trie.m_container.at(7).m_check == -8);
	BOOST_CHECK(trie.m_container.at(8).m_check == 0);
}

BOOST_AUTO_TEST_CASE(trie__reserve__5)
{
	test_trie_base trie{};

	trie.m_container.front() = wordring::trie_node{ -2, -1 };
	trie.m_container.emplace_back(0, -2);
	trie.m_container.emplace_back(0, 0);

	trie.reserve(3);

	BOOST_CHECK(trie.m_container.size() == 6);

	BOOST_CHECK(trie.m_container.at(0).m_base == -5);
	BOOST_CHECK(trie.m_container.at(0).m_check == -1);
	BOOST_CHECK(trie.m_container.at(1).m_check == -2);
	BOOST_CHECK(trie.m_container.at(2).m_check == -3);
	BOOST_CHECK(trie.m_container.at(3).m_check == -4);
	BOOST_CHECK(trie.m_container.at(4).m_check == -5);
	BOOST_CHECK(trie.m_container.at(5).m_check == 0);
}

BOOST_AUTO_TEST_CASE(trie__reserve__6)
{
	test_trie_base trie{};

	trie.m_container.front() = wordring::trie_node{ -1, -1 };
	trie.m_container.emplace_back(0, 0);

	trie.reserve(3);

	BOOST_CHECK(trie.m_container.size() == 5);

	BOOST_CHECK(trie.m_container.at(0).m_base == -4);
	BOOST_CHECK(trie.m_container.at(0).m_check == -1);
	BOOST_CHECK(trie.m_container.at(1).m_check == -2);
	BOOST_CHECK(trie.m_container.at(2).m_check == -3);
	BOOST_CHECK(trie.m_container.at(3).m_check == -4);
	BOOST_CHECK(trie.m_container.at(4).m_check == 0);
}

BOOST_AUTO_TEST_CASE(trie__allocate__1)
{
	test_trie_base trie{};

	trie.m_container.front() = wordring::trie_node{ -8, -1 }; // 0
	trie.m_container.emplace_back(0, -3);                     // 1
	trie.m_container.emplace_back(0, 0);                      // 2
	trie.m_container.emplace_back(0, -5);                     // 3
	trie.m_container.emplace_back(0, 0);                      // 4
	trie.m_container.emplace_back(0, -6);                     // 5
	trie.m_container.emplace_back(0, -7);                     // 6
	trie.m_container.emplace_back(0, -8);                     // 7
	trie.m_container.emplace_back(0, 0);                      // 8

	BOOST_CHECK(trie.allocate(3) == 1);

	BOOST_CHECK(trie.m_container.at(0).m_base == -8);
	BOOST_CHECK(trie.m_container.at(0).m_check == -1);
	BOOST_CHECK(trie.m_container.at(1).m_check == -5);
	BOOST_CHECK(trie.m_container.at(2).m_check == 0);
	BOOST_CHECK(trie.m_container.at(3).m_check == 0);
	BOOST_CHECK(trie.m_container.at(4).m_check == 0);
	BOOST_CHECK(trie.m_container.at(5).m_check == -6);
	BOOST_CHECK(trie.m_container.at(6).m_check == -7);
	BOOST_CHECK(trie.m_container.at(7).m_check == -8);
	BOOST_CHECK(trie.m_container.at(8).m_check == 0);
}

BOOST_AUTO_TEST_CASE(trie__allocate__2)
{
	test_trie_base trie{};

	trie.m_container.front() = wordring::trie_node{ -8, -1 }; // 0
	trie.m_container.emplace_back(0, -3);                     // 1
	trie.m_container.emplace_back(0, 0);                      // 2
	trie.m_container.emplace_back(0, -5);                     // 3
	trie.m_container.emplace_back(0, 0);                      // 4
	trie.m_container.emplace_back(0, -6);                     // 5
	trie.m_container.emplace_back(0, -7);                     // 6
	trie.m_container.emplace_back(0, -8);                     // 7
	trie.m_container.emplace_back(0, 0);                      // 8

	BOOST_CHECK(trie.allocate(1) == 0);

	BOOST_CHECK(trie.m_container.at(0).m_base == -8);
	BOOST_CHECK(trie.m_container.at(0).m_check == -3);
	BOOST_CHECK(trie.m_container.at(1).m_check == 0);
	BOOST_CHECK(trie.m_container.at(2).m_check == 0);
	BOOST_CHECK(trie.m_container.at(3).m_check == -5);
	BOOST_CHECK(trie.m_container.at(4).m_check == 0);
	BOOST_CHECK(trie.m_container.at(5).m_check == -6);
	BOOST_CHECK(trie.m_container.at(6).m_check == -7);
	BOOST_CHECK(trie.m_container.at(7).m_check == -8);
	BOOST_CHECK(trie.m_container.at(8).m_check == 0);
}

BOOST_AUTO_TEST_CASE(trie__allocate__3)
{
	test_trie_base trie{};

	trie.m_container.front() = wordring::trie_node{ -8, -2 }; // 0
	trie.m_container.emplace_back(0, 0);                      // 1
	trie.m_container.emplace_back(0, -3);                     // 2
	trie.m_container.emplace_back(0, -5);                     // 3
	trie.m_container.emplace_back(0, 0);                      // 4
	trie.m_container.emplace_back(0, -6);                     // 5
	trie.m_container.emplace_back(0, -7);                     // 6
	trie.m_container.emplace_back(0, -8);                     // 7
	trie.m_container.emplace_back(0, 0);                      // 8

	BOOST_CHECK(trie.allocate(2) == 0);

	BOOST_CHECK(trie.m_container.at(0).m_base == -8);
	BOOST_CHECK(trie.m_container.at(0).m_check == -3);
	BOOST_CHECK(trie.m_container.at(1).m_check == 0);
	BOOST_CHECK(trie.m_container.at(2).m_check == 0);
	BOOST_CHECK(trie.m_container.at(3).m_check == -5);
	BOOST_CHECK(trie.m_container.at(4).m_check == 0);
	BOOST_CHECK(trie.m_container.at(5).m_check == -6);
	BOOST_CHECK(trie.m_container.at(6).m_check == -7);
	BOOST_CHECK(trie.m_container.at(7).m_check == -8);
	BOOST_CHECK(trie.m_container.at(8).m_check == 0);
}

BOOST_AUTO_TEST_CASE(trie__allocate__4)
{
	test_trie_base trie{};

	trie.m_container.front() = wordring::trie_node{ -8, -1 }; // 0
	trie.m_container.emplace_back(0, -3);                     // 1
	trie.m_container.emplace_back(0, 0);                      // 2
	trie.m_container.emplace_back(0, -5);                     // 3
	trie.m_container.emplace_back(0, 0);                      // 4
	trie.m_container.emplace_back(0, -6);                     // 5
	trie.m_container.emplace_back(0, -7);                     // 6
	trie.m_container.emplace_back(0, -8);                     // 7
	trie.m_container.emplace_back(0, 0);                      // 8

	BOOST_CHECK(trie.allocate(8) == 7);

	BOOST_CHECK(trie.m_container.at(0).m_base == -7);
	BOOST_CHECK(trie.m_container.at(0).m_check == -1);
	BOOST_CHECK(trie.m_container.at(1).m_check == -3);
	BOOST_CHECK(trie.m_container.at(2).m_check == 0);
	BOOST_CHECK(trie.m_container.at(3).m_check == -5);
	BOOST_CHECK(trie.m_container.at(4).m_check == 0);
	BOOST_CHECK(trie.m_container.at(5).m_check == -6);
	BOOST_CHECK(trie.m_container.at(6).m_check == -7);
	BOOST_CHECK(trie.m_container.at(7).m_check == 0);
	BOOST_CHECK(trie.m_container.at(8).m_check == 0);
}

BOOST_AUTO_TEST_CASE(trie__allocate__5)
{
	test_trie_base trie{};

	trie.m_container.front() = wordring::trie_node{ -8, -8 }; // 0
	trie.m_container.emplace_back(0, 0);                      // 1
	trie.m_container.emplace_back(0, 0);                      // 2
	trie.m_container.emplace_back(0, 0);                      // 3
	trie.m_container.emplace_back(0, 0);                      // 4
	trie.m_container.emplace_back(0, 0);                      // 5
	trie.m_container.emplace_back(0, 0);                      // 6
	trie.m_container.emplace_back(0, 0);                      // 7
	trie.m_container.emplace_back(0, 0);                      // 8

	BOOST_CHECK(trie.allocate(8) == 0);

	BOOST_CHECK(trie.m_container.at(0).m_base == 0);
	BOOST_CHECK(trie.m_container.at(0).m_check == 0);
	BOOST_CHECK(trie.m_container.at(1).m_check == 0);
	BOOST_CHECK(trie.m_container.at(2).m_check == 0);
	BOOST_CHECK(trie.m_container.at(3).m_check == 0);
	BOOST_CHECK(trie.m_container.at(4).m_check == 0);
	BOOST_CHECK(trie.m_container.at(5).m_check == 0);
	BOOST_CHECK(trie.m_container.at(6).m_check == 0);
	BOOST_CHECK(trie.m_container.at(7).m_check == 0);
	BOOST_CHECK(trie.m_container.at(8).m_check == 0);
}

BOOST_AUTO_TEST_CASE(trie__allocate__6)
{
	test_trie_base trie{};

	trie.m_container.front() = wordring::trie_node{ -1, -1 }; // 0
	trie.m_container.emplace_back(0, 0);                      // 1

	BOOST_CHECK(trie.allocate(1) ==0);

	BOOST_CHECK(trie.m_container.at(0).m_base == 0);
	BOOST_CHECK(trie.m_container.at(0).m_check == 0);
	BOOST_CHECK(trie.m_container.at(1).m_check == 0);
}

BOOST_AUTO_TEST_CASE(trie__allocate__7)
{
	test_trie_base trie{};

	trie.m_container.front() = wordring::trie_node{ -8, -1 }; // 0
	trie.m_container.emplace_back(0, -3);                     // 1
	trie.m_container.emplace_back(0, 0);                      // 2
	trie.m_container.emplace_back(0, -5);                     // 3
	trie.m_container.emplace_back(0, 0);                      // 4
	trie.m_container.emplace_back(0, -6);                     // 5
	trie.m_container.emplace_back(0, -7);                     // 6
	trie.m_container.emplace_back(0, -8);                     // 7
	trie.m_container.emplace_back(0, 0);                      // 8

	BOOST_CHECK(trie.allocate(6, trie.allocate(3)) == 5);

	BOOST_CHECK(trie.m_container.at(0).m_base == -8);
	BOOST_CHECK(trie.m_container.at(0).m_check == -1);
	BOOST_CHECK(trie.m_container.at(1).m_check == -5);
	BOOST_CHECK(trie.m_container.at(2).m_check == 0);
	BOOST_CHECK(trie.m_container.at(3).m_check == 0);
	BOOST_CHECK(trie.m_container.at(4).m_check == 0);
	BOOST_CHECK(trie.m_container.at(5).m_check == -7);
	BOOST_CHECK(trie.m_container.at(6).m_check == 0);
	BOOST_CHECK(trie.m_container.at(7).m_check == -8);
	BOOST_CHECK(trie.m_container.at(8).m_check == 0);
}

BOOST_AUTO_TEST_CASE(trie__allocate__8)
{
	test_trie_base trie{};

	trie.m_container.front() = wordring::trie_node{ -8, -1 }; // 0
	trie.m_container.emplace_back(0, -3);                     // 1
	trie.m_container.emplace_back(0, 0);                      // 2
	trie.m_container.emplace_back(0, -5);                     // 3
	trie.m_container.emplace_back(0, 0);                      // 4
	trie.m_container.emplace_back(0, -6);                     // 5
	trie.m_container.emplace_back(0, -7);                     // 6
	trie.m_container.emplace_back(0, -8);                     // 7
	trie.m_container.emplace_back(0, 0);                      // 8

	BOOST_CHECK(trie.allocate(3, trie.allocate(1)) == 0);

	BOOST_CHECK(trie.m_container.at(0).m_base == -8);
	BOOST_CHECK(trie.m_container.at(0).m_check == -5);
	BOOST_CHECK(trie.m_container.at(1).m_check == 0);
	BOOST_CHECK(trie.m_container.at(2).m_check == 0);
	BOOST_CHECK(trie.m_container.at(3).m_check == 0);
	BOOST_CHECK(trie.m_container.at(4).m_check == 0);
	BOOST_CHECK(trie.m_container.at(5).m_check == -6);
	BOOST_CHECK(trie.m_container.at(6).m_check == -7);
	BOOST_CHECK(trie.m_container.at(7).m_check == -8);
	BOOST_CHECK(trie.m_container.at(8).m_check == 0);
}

BOOST_AUTO_TEST_CASE(trie__allocate__9)
{
	test_trie_base trie{};

	trie.m_container.front() = wordring::trie_node{ -8, -1 }; // 0
	trie.m_container.emplace_back(0, -3);                     // 1
	trie.m_container.emplace_back(0, 0);                      // 2
	trie.m_container.emplace_back(0, -5);                     // 3
	trie.m_container.emplace_back(0, 0);                      // 4
	trie.m_container.emplace_back(0, -6);                     // 5
	trie.m_container.emplace_back(0, -7);                     // 6
	trie.m_container.emplace_back(0, -8);                     // 7
	trie.m_container.emplace_back(0, 0);                      // 8

	BOOST_CHECK(trie.allocate(8, trie.allocate(1)) == 7);

	BOOST_CHECK(trie.m_container.at(0).m_base == -7);
	BOOST_CHECK(trie.m_container.at(0).m_check == -3);
	BOOST_CHECK(trie.m_container.at(1).m_check == 0);
	BOOST_CHECK(trie.m_container.at(2).m_check == 0);
	BOOST_CHECK(trie.m_container.at(3).m_check == -5);
	BOOST_CHECK(trie.m_container.at(4).m_check == 0);
	BOOST_CHECK(trie.m_container.at(5).m_check == -6);
	BOOST_CHECK(trie.m_container.at(6).m_check == -7);
	BOOST_CHECK(trie.m_container.at(7).m_check == 0);
	BOOST_CHECK(trie.m_container.at(8).m_check == 0);
}

// index_type allocate(static_vector const& children)
BOOST_AUTO_TEST_CASE(trie__allocate__10)
{
	std::bitset<257>  bits{};
	//bits = (~bits) & (bits - 1);
	int i = bits.count();
}

BOOST_AUTO_TEST_CASE(trie__allocate__11)
{
	using namespace wordring;

	test_trie_base trie{};

	trie.m_container.front() = wordring::trie_node{ -8, -1 }; // 0
	trie.m_container.emplace_back(0, -3);                     // 1
	trie.m_container.emplace_back(0, 0);                      // 2
	trie.m_container.emplace_back(0, -5);                     // 3
	trie.m_container.emplace_back(0, 0);                      // 4
	trie.m_container.emplace_back(0, -6);                     // 5
	trie.m_container.emplace_back(0, -7);                     // 6
	trie.m_container.emplace_back(0, -8);                     // 7
	trie.m_container.emplace_back(0, 0);                      // 8

	//auto i = trie.allocate(bits);
}

BOOST_AUTO_TEST_CASE(trie__allocate__12)
{
}

BOOST_AUTO_TEST_CASE(trie__allocate__13)
{
}

BOOST_AUTO_TEST_CASE(trie__allocate__14)
{
}

BOOST_AUTO_TEST_CASE(trie__allocate__15)
{
}

BOOST_AUTO_TEST_CASE(trie__allocate__16)
{
}

BOOST_AUTO_TEST_CASE(trie__allocate__17)
{
}

BOOST_AUTO_TEST_CASE(trie__allocate__18)
{
}

BOOST_AUTO_TEST_CASE(trie__free__1)
{
	test_trie_base trie{};

	trie.m_container.front() = wordring::trie_node{ -2, -2 }; // 0
	trie.m_container.emplace_back(0, 0);                     // 1
	trie.m_container.emplace_back(0, 0);                      // 2

	trie.free(1);

	BOOST_CHECK(trie.m_container.at(0).m_base == -2);
	BOOST_CHECK(trie.m_container.at(0).m_check == -1);
	BOOST_CHECK(trie.m_container.at(1).m_check == -2);
	BOOST_CHECK(trie.m_container.at(2).m_check == 0);
}
BOOST_AUTO_TEST_CASE(trie__free__2)
{
	test_trie_base trie{};

	trie.m_container.front() = wordring::trie_node{ -2, -2 }; // 0
	trie.m_container.emplace_back(0, 0);                     // 1
	trie.m_container.emplace_back(0, 0);                      // 2

	trie.free(1);

	BOOST_CHECK(trie.m_container.at(0).m_base == -2);
	BOOST_CHECK(trie.m_container.at(0).m_check == -1);
	BOOST_CHECK(trie.m_container.at(1).m_check == -2);
	BOOST_CHECK(trie.m_container.at(2).m_check == 0);
}

BOOST_AUTO_TEST_CASE(trie__free__3)
{
	test_trie_base trie{};

	trie.m_container.front() = wordring::trie_node{ -2, -2 }; // 0
	trie.m_container.emplace_back(0, 0);                     // 1
	trie.m_container.emplace_back(0, 0);                      // 2

	trie.free(1);

	BOOST_CHECK(trie.m_container.at(0).m_base == -2);
	BOOST_CHECK(trie.m_container.at(0).m_check == -1);
	BOOST_CHECK(trie.m_container.at(1).m_check == -2);
	BOOST_CHECK(trie.m_container.at(2).m_check == 0);
}

BOOST_AUTO_TEST_CASE(trie__free__4)
{
	test_trie_base trie{};

	trie.m_container.front() = wordring::trie_node{ -2, -2 }; // 0
	trie.m_container.emplace_back(0, 0);                     // 1
	trie.m_container.emplace_back(0, 0);                      // 2

	trie.free(1);

	BOOST_CHECK(trie.m_container.at(0).m_base == -2);
	BOOST_CHECK(trie.m_container.at(0).m_check == -1);
	BOOST_CHECK(trie.m_container.at(1).m_check == -2);
	BOOST_CHECK(trie.m_container.at(2).m_check == 0);
}

BOOST_AUTO_TEST_CASE(trie__free__5)
{
	test_trie_base trie{};

	trie.m_container.front() = wordring::trie_node{ -2, -2 }; // 0
	trie.m_container.emplace_back(0, 0);                     // 1
	trie.m_container.emplace_back(0, 0);                      // 2

	trie.free(1);

	BOOST_CHECK(trie.m_container.at(0).m_base == -2);
	BOOST_CHECK(trie.m_container.at(0).m_check == -1);
	BOOST_CHECK(trie.m_container.at(1).m_check == -2);
	BOOST_CHECK(trie.m_container.at(2).m_check == 0);
}

BOOST_AUTO_TEST_SUITE_END()
