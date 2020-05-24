﻿// wordring_cpp/test/whatwg/html/parsing/on_initial_insertion_mode.cpp

#include <boost/test/unit_test.hpp>

#include <wordring/html/simple_adapter.hpp>
#include <wordring/html/simple_node.hpp>
#include <wordring/html/simple_parser.hpp>

#include <wordring/whatwg/html/parsing/atom_tbl.hpp>
#include <wordring/whatwg/html/parsing/token.hpp>
#include <wordring/whatwg/html/parsing/tokenization.hpp>
#include <wordring/whatwg/html/parsing/tree_construction_dispatcher.hpp>

#include <wordring/tree/tree.hpp>

#include <string>


namespace
{
	using namespace wordring::html;
	//using namespace wordring::html::parsing;

	using node_type   = simple_node<std::string>;
	using tree        = wordring::tree<node_type>;
	using adapter_type = simple_adapter<std::string, tree>;
	
	using document_type               = typename adapter_type::document_type;
	using document_type_type          = typename adapter_type::document_type_type;
	using document_fragment_type      = typename adapter_type::document_fragment_type;
	using element_type                = typename adapter_type::element_type;
	using text_type                   = typename adapter_type::text_type;
	using processing_instruction_type = typename adapter_type::processing_instruction_type;
	using comment_type                = typename adapter_type::comment_type;
	
	struct test_parser : public simple_parser_base<test_parser, std::string, tree>
	{
	public:
		using base_type = simple_parser_base<test_parser, std::string, tree>;

		using base_type::mode_name;

		//using base_type::m_insertion_mode;
	};
}

BOOST_AUTO_TEST_SUITE(on_initial_insertion_mode_test)

BOOST_AUTO_TEST_CASE(on_initial_insertion_mode_1)
{
	test_parser p;
	std::u32string s = U"<!DOCTYPE HTML>";
	for (char32_t cp : s) p.push_code_point(cp);

	BOOST_CHECK(p.m_insertion_mode == test_parser::mode_name::before_html_insertion_mode);
	BOOST_CHECK(std::get<document_type>(*p.get_document()).document_mode() == document_mode_name::no_quirks);
}

BOOST_AUTO_TEST_CASE(on_initial_insertion_mode_2)
{
	test_parser p;
	std::u32string s = U"<!DOCTYPE HTML PUBLIC \"-//W3O//DTD W3 HTML Strict 3.0//EN//\">";
	for (char32_t cp : s) p.push_code_point(cp);

	BOOST_CHECK(p.m_insertion_mode == test_parser::mode_name::before_html_insertion_mode);
	BOOST_CHECK(std::get<document_type>(*p.get_document()).document_mode() == document_mode_name::quirks);
}

BOOST_AUTO_TEST_CASE(on_initial_insertion_mode_3)
{
	test_parser p;
	std::u32string s = U"<!DOCTYPE HTML PUBLIC \"-//AS//DTD HTML 3.0 asWedit + extensions//ABCD\">";
	for (char32_t cp : s) p.push_code_point(cp);

	BOOST_CHECK(p.m_insertion_mode == test_parser::mode_name::before_html_insertion_mode);
	BOOST_CHECK(std::get<document_type>(*p.get_document()).document_mode() == document_mode_name::quirks);
}

BOOST_AUTO_TEST_CASE(on_initial_insertion_mode_4)
{
	test_parser p;
	std::u32string s = U"<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Frameset//ABCD\">";
	for (char32_t cp : s) p.push_code_point(cp);

	BOOST_CHECK(p.m_insertion_mode == test_parser::mode_name::before_html_insertion_mode);
	BOOST_CHECK(std::get<document_type>(*p.get_document()).document_mode() == document_mode_name::quirks);
}

BOOST_AUTO_TEST_CASE(on_initial_insertion_mode_5)
{
	test_parser p;
	std::u32string s = U"<!DOCTYPE HTML PUBLIC \"-//W3C//DTD XHTML 1.0 Frameset//ABCD\">";
	for (char32_t cp : s) p.push_code_point(cp);

	BOOST_CHECK(p.m_insertion_mode == test_parser::mode_name::before_html_insertion_mode);
	BOOST_CHECK(std::get<document_type>(*p.get_document()).document_mode() == document_mode_name::limited_quirks);
}

BOOST_AUTO_TEST_CASE(on_initial_insertion_mode_6)
{
	test_parser p;
	std::u32string s = U"<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Frameset//ABCD\" \"http:://ABCD\">";
	for (char32_t cp : s) p.push_code_point(cp);

	BOOST_CHECK(p.m_insertion_mode == test_parser::mode_name::before_html_insertion_mode);
	BOOST_CHECK(std::get<document_type>(*p.get_document()).document_mode() == document_mode_name::limited_quirks);
}

BOOST_AUTO_TEST_SUITE_END()
