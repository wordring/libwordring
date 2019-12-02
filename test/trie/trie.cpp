// test/trie/trie.cpp

#include <boost/test/unit_test.hpp>

#include <string>
#include <vector>

#include <wordring/trie/trie.hpp>

BOOST_AUTO_TEST_SUITE(trie__test)

BOOST_AUTO_TEST_CASE(trie__constrcut__1)
{
	using namespace wordring;

	std::vector<std::string> list{ "a", "ac", "b", "cab", "cd" };
	trie_base trie{};
	trie.insert(list.cbegin(), list.cend());
}

BOOST_AUTO_TEST_SUITE_END()
