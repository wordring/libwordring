// test/css/selector.cpp

#include <boost/test/unit_test.hpp>

#include <wordring/css/selector.hpp>

#include <wordring/html/simple_html.hpp>

#include <iterator>
#include <string>
#include <string_view>
#include <vector>

BOOST_AUTO_TEST_SUITE(css_selector_test)

BOOST_AUTO_TEST_CASE(selector_query_selector_all_1)
{
	namespace css = wordring::css;
	namespace html = wordring::html;

	std::string_view sv = u8"<p>text1</p><p>text2</p>";
	html::u8simple_tree tree = html::make_document<html::u8simple_tree>(sv.begin(), sv.end());

	std::vector<html::u8simple_tree::const_iterator> v;
	css::query_selector_all(tree.begin(), u"p", std::back_inserter(v));
	BOOST_CHECK(v.size() == 2);
	std::u32string s1, s2;
	html::to_string(v[0], std::back_inserter(s1));
	BOOST_CHECK(s1 == U"text1");
	html::to_string(v[1], std::back_inserter(s2));
	BOOST_CHECK(s2 == U"text2");
}

BOOST_AUTO_TEST_CASE(selector_query_selector_1)
{
	namespace css = wordring::css;
	namespace html = wordring::html;

	std::u32string_view sv = U"<p>text1</p><p>text2</p>";
	html::u8simple_tree tree = html::make_document<html::u8simple_tree>(sv.begin(), sv.end());

	html::u8simple_tree::const_iterator it = css::query_selector(tree.begin(), u"p");
	std::u16string s;
	html::to_string(it, std::back_inserter(s));
	BOOST_CHECK(s == u"text1");
}

BOOST_AUTO_TEST_SUITE_END()
