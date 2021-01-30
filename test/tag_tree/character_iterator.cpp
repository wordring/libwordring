// test/tag_tree/character_iterator.cpp

#include <boost/test/unit_test.hpp>

#include <wordring/tag_tree/character_iterator.hpp>

#include <wordring/css/selector.hpp>
#include <wordring/html/simple_html.hpp>
#include <wordring/html/simple_traits.hpp>
#include <wordring/tree/tree_iterator.hpp>

#include <iterator>
#include <regex>
#include <string_view>

BOOST_AUTO_TEST_SUITE(character_iterator_test)

BOOST_AUTO_TEST_CASE(character_iterator_construct_1)
{
	using namespace wordring::html;

	std::string_view sv = "<p>1</p><p>2</p><p>3</p>";
	auto doc = make_document<u8simple_tree>(sv.begin(), sv.end());

	auto it = u8simple_tree::character_iterator(doc.begin());
	std::u8string s;
	while (it != it.end()) s.push_back(*it++);

	BOOST_CHECK(s == u8"123");
}

BOOST_AUTO_TEST_CASE(character_iterator_regex_1)
{
	using namespace wordring::html;

	std::string_view sv = "<p>ABC1</p><p>2</p><p>3DEF</p>";
	auto doc = make_document<u8simple_tree>(sv.begin(), sv.end());

	auto it = u8simple_tree::character_iterator(doc.begin());
	std::basic_regex<char> rx("[0-9]+");
	std::match_results<u8simple_tree::character_iterator> mr;
	std::regex_search(it, it.end(), mr, rx);

	BOOST_CHECK(mr.length() == 3);
	BOOST_CHECK(mr.str() == u8"123");
}

BOOST_AUTO_TEST_SUITE_END()
