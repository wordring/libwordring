﻿// test/tree/simple_html.cpp

#include <boost/test/unit_test.hpp>

#include <wordring/tag_tree/tag_tree.hpp>

#include <wordring/css/selector.hpp>
#include <wordring/html/simple_html.hpp>
#include <wordring/html/simple_traits.hpp>

#include <iterator>
#include <string>

namespace wordring::html
{
	using test_iterator = typename wordring::tree<simple_node<std::u8string>>::iterator;
	using const_test_iterator = typename wordring::tree<simple_node<std::u8string>>::const_iterator;

	template <>
	struct node_traits<test_iterator> : public simple_node_traits<test_iterator> {};

	template <>
	struct node_traits<const_test_iterator> : public simple_node_traits<const_test_iterator> {};

	class test_tree : public tree<simple_node<std::u8string>> {};
}

namespace
{
	/*
	std::u8string print(wordring::html::test_iterator it)
	{
		using namespace wordring::html;
		using traits = node_traits<test_iterator>;
		if (traits::is_text(it)) return traits::data(it);
		return u8"";
	}
	*/

	wordring::html::test_tree make_test_tree(std::u8string const& s)
	{
		using namespace wordring;
		using namespace wordring::html;
		auto p = basic_simple_parser<test_tree, decltype(s.begin())>(encoding_confidence_name::tentative, encoding_name::UTF_8, false);
		p.parse(s.begin(), s.end());
		return p.get();
	}
}

BOOST_AUTO_TEST_SUITE(tag_tree_simple_html_test)

BOOST_AUTO_TEST_CASE(simple_html_parse_1)
{
	namespace css = wordring::css;
	using namespace wordring::html;

	std::u8string src = u8"<p>text1</p>";
	test_tree t1 = make_test_tree(src);
	std::u8string s;
	to_string(t1.begin(), std::back_inserter(s));

	BOOST_CHECK(s == u8"<html><head></head><body><p>text1</p></body></html>");

	test_tree::const_iterator it = css::query_selector(t1.begin(), u"p");
	std::u16string html;
	to_string(it, std::back_inserter(html));
	BOOST_CHECK(html == u"text1");
}

BOOST_AUTO_TEST_SUITE_END()
