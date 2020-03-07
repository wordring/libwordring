// test/trie/trie_base.cpp

#include <boost/test/unit_test.hpp>

#include <wordring/serialize/serialize_iterator.hpp>
#include <wordring/trie/trie_base.hpp>
#include <wordring/trie/trie_base_iterator.hpp>
#include <wordring/tree/tree_iterator.hpp>


BOOST_AUTO_TEST_SUITE(trie_base__sample)


/* 空のコンテナを構築する

trie_base()
*/
BOOST_AUTO_TEST_CASE(trie_base__constrcut__1)
{
	using namespace wordring::detail;

	auto trie = trie_base();
}


BOOST_AUTO_TEST_SUITE_END()
