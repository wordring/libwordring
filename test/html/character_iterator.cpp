// test/html/character_iterator.cpp

#include <boost/test/unit_test.hpp>

#include <wordring/html/character_iterator.hpp>

#include <wordring/css/selector.hpp>
#include <wordring/html/simple_html.hpp>
#include <wordring/tree/tree_iterator.hpp>

#include <iterator>
#include <regex>
#include <string_view>

namespace wordring::html
{
	using vector_node_iterator = typename std::vector<u8simple_node>::iterator;

	template<>
	struct node_traits<vector_node_iterator> : simple_node_traits<vector_node_iterator> {};
}

BOOST_AUTO_TEST_SUITE(character_iterator_test)

BOOST_AUTO_TEST_CASE(character_iterator_construct_1)
{
	using namespace wordring;
	using namespace wordring::html;

	using tree_iterator = wordring::tree_iterator<u8simple_tree::const_iterator>;

	std::u8string_view sv = u8"<p>1</p><p>2</p><p>3</p>";
	auto doc = make_document<u8simple_tree>(sv.begin(), sv.end());

	auto it1 = tree_iterator(doc.begin());
	auto it2 = tree_iterator();

	std::u8string s;

	auto it = character_iterator(it1, it2);
	while (it != it.end())
	{
		s.push_back(*it);
		++it;
	}

	BOOST_CHECK(s == u8"123");
}

template <typename T>
class has_base
{
private:
	template <typename U>
	static auto test(U u)->decltype(u.base(), std::true_type());
	static auto test(...)->decltype(std::false_type());

public:
	static bool const value = decltype(test(std::declval<T>()))::value;
};

BOOST_AUTO_TEST_CASE(character_iterator_construct_2)
{/*
 using namespace wordring;
	using namespace wordring::css;
	using namespace wordring::html;

	bool b1 = has_base<int>::value;
	bool b2 = has_base<std::vector<int>::const_iterator>::value;

	bool b3 = has_base<wordring::tree_iterator<u8simple_tree::const_iterator>>::value;
	int i = 0;
	
	using tree_iterator = wordring::tree_iterator<u8simple_tree::const_iterator>;

	std::u8string_view sv = u8"<p>1</p><p>2</p><p>3</p>";
	auto doc = make_document<u8simple_tree>(sv.begin(), sv.end());

	std::vector<u8simple_tree::iterator> v;
	query_selector_all(doc.begin(), u8"p", std::back_inserter(v));
	std::vector<u8simple_node> tree;
	for (auto it : v) tree.push_back(*it);
 
	std::u8string s;
	auto it = character_iterator(tree.begin(), tree.end());
	while (it != it.end())
	{
		s.push_back(*it);
		++it;
	}

	BOOST_CHECK(s == u8"123");
	*/
}

BOOST_AUTO_TEST_CASE(character_iterator_regex_1)
{
	using namespace wordring;
	using namespace wordring::html;

	using tree_iterator = wordring::tree_iterator<u8simple_tree::const_iterator>;

	std::u8string_view sv = u8"<p>1</p><p>2</p><p>3</p>";
	auto doc = make_document<u8simple_tree>(sv.begin(), sv.end());

	auto it1 = tree_iterator(doc.begin());
	auto it2 = tree_iterator();
	auto it = character_iterator(it1, it2);
	//std::basic_regex<char> rx(".*");
	//std::match_results<char> mr;
	//std::regex_match()

	BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END()
