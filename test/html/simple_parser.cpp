﻿// test/html/simple_parser.cpp

#include <boost/test/unit_test.hpp>

#include <wordring/html/simple_node.hpp>
#include <wordring/html/simple_parser.hpp>
#include <wordring/html/simple_traits.hpp>

#include <wordring/tree/tree.hpp>

namespace
{
	using tree = wordring::tree<wordring::html::simple_node<std::string>>;
	using test_parser = wordring::html::simple_parser<tree>;
}

BOOST_AUTO_TEST_SUITE(simple_parser_test)

BOOST_AUTO_TEST_CASE(simple_parser_construct_1)
{
	test_parser p;
	std::u32string s = U"<!-- Comment -->";
	for (char32_t cp : s) p.push_code_point(cp);
	p.push_eof();

	BOOST_CHECK(data(*p.get_document().begin()) == " Comment ");
}

BOOST_AUTO_TEST_CASE(simple_parser_contains_1)
{
	using namespace wordring::html;

	test_parser p;
	auto el = p.create_element(p.get_document(), tag_name::P, ns_name::HTML);
	p.append_attribute(el, ns_name::HTML, U"", U"abbr", U"");
	auto it = p.insert_element(p.get_document().end(), el);

	BOOST_CHECK(p.contains(it, ns_name::HTML, U"", U"abbr"));
}

BOOST_AUTO_TEST_CASE(simple_parser_contains_2)
{
	using namespace wordring::whatwg::html::parsing;

	test_parser p;
	auto el = p.create_element(p.get_document(), tag_name::P, ns_name::HTML);
	p.append_attribute(el, ns_name::HTML, U"", U"abbr", U"");
	auto it = p.insert_element(p.get_document().end(), el);

	BOOST_CHECK(p.contains(it, ns_name::HTML, U"", U"") == false);
}

BOOST_AUTO_TEST_CASE(simple_parser_find_attribute_1)
{
	using namespace wordring::html;

	test_parser p;
	auto el = p.create_element(p.get_document(), tag_name::P, ns_name::HTML);
	p.append_attribute(el, ns_name::HTML, U"", U"attr", U"");
	auto it = p.insert_element(p.get_document().end(), el);
	BOOST_CHECK(it != p.get_document().end());
	//auto attr = p.create_attribute(it, )

	//BOOST_CHECK(p.find_attribute(it, attribute_name::Abbr) != it->end());
}



/*
BOOST_AUTO_TEST_CASE(simple_parser__1)
{
}
BOOST_AUTO_TEST_CASE(simple_parser__1)
{
}
BOOST_AUTO_TEST_CASE(simple_parser__1)
{
}
BOOST_AUTO_TEST_CASE(simple_parser__1)
{
}
*/

BOOST_AUTO_TEST_SUITE_END()
