// test/whatwg/html/simple_parser.cpp

#include <boost/test/unit_test.hpp>

#include <wordring/whatwg/html/simple_node.hpp>
#include <wordring/whatwg/html/simple_parser.hpp>
#include <wordring/whatwg/html/simple_policy.hpp>

#include <wordring/whatwg/html/parsing/atom_tbl.hpp>
#include <wordring/whatwg/html/parsing/tokenization.hpp>
#include <wordring/whatwg/html/parsing/tree_construction_dispatcher.hpp>

#include <wordring/tree/tree.hpp>

#include <string>
#include <vector>

namespace
{
	using namespace wordring::whatwg::html::simple;

	using node_type      = basic_node<std::string>;
	using tree           = wordring::tree<node_type>;
	using policy_type    = tree_construction_policy<std::string, tree>;

	class test_parser : public parser<test_parser, std::string, tree>
	{
		using base_type = parser<test_parser, std::string, tree>;

	public:
		using base_type::current_node;
		using base_type::adjusted_current_node;
		using base_type::is_special;
		using base_type::is_formatting;
		using base_type::in_specific_scope;

		using base_type::push_active_formatting_element_list;
		using base_type::reconstruct_active_formatting_element_list;


		using base_type::appropriate_place_for_inserting_node;

		using base_type::default_scope;
		using base_type::list_item_scope;
		using base_type::button_scope;
		using base_type::table_scope;
		using base_type::select_scope;

		using base_type::m_open_element_stack;
		using base_type::m_active_formatting_element_list;
		using base_type::m_foster_parenting;

		using base_type::namespace_uri_name;
		using base_type::local_name_name;
	};
}

BOOST_AUTO_TEST_SUITE(simple_parser_test)

BOOST_AUTO_TEST_CASE(simple_parser_construct_1)
{
	using namespace wordring::whatwg::html;
	using namespace wordring::whatwg::html::parsing;

	BOOST_CHECK(true);

	test_parser p;
	std::u32string s = U"<!-- Comment -->";
	for (char32_t cp : s) p.push_code_point(cp);
	p.eof(true);

	//attribute_set.contains(std::u32string());
}

BOOST_AUTO_TEST_CASE(simple_parser_current_node_1)
{
}

BOOST_AUTO_TEST_CASE(simple_parser_adjusted_current_node_1)
{
	using namespace wordring::whatwg::html::simple;
}

BOOST_AUTO_TEST_CASE(simple_parser_is_special_1)
{
	test_parser p;
	tree t;
	t.insert(t.end(), basic_element<std::string>(ns_name::HTML, "", tag_name::Address));

	BOOST_CHECK(p.is_special(t.begin()));
}

BOOST_AUTO_TEST_CASE(simple_parser_is_formatting_1)
{
	test_parser p;
	tree t;
	t.insert(t.end(), basic_element<std::string>(ns_name::HTML, "", tag_name::Font));

	BOOST_CHECK(p.is_formatting(t.begin()));
}

BOOST_AUTO_TEST_CASE(simple_parser_in_specific_scope_1)
{
	tree t;
	t.insert(t.end(), basic_element<std::string>(ns_name::HTML, "", tag_name::Html));
	t.insert(t.begin().end(), basic_element<std::string>(ns_name::HTML, "", tag_name::Font));
	test_parser p;
	p.m_open_element_stack.push_back(t.begin());
	p.m_open_element_stack.push_back(t.begin().begin());

	BOOST_CHECK(p.in_specific_scope(test_parser::default_scope, std::make_pair(ns_name::HTML, tag_name::Font)));
}

BOOST_AUTO_TEST_CASE(simple_parser_in_specific_scope_2)
{
	tree t;
	t.insert(t.end(), basic_element<std::string>(ns_name::HTML, "", tag_name::Html));
	t.insert(t.begin().end(), basic_element<std::string>(ns_name::HTML, "", tag_name::Font));
	test_parser p;
	p.m_open_element_stack.push_back(t.begin());
	p.m_open_element_stack.push_back(t.begin().begin());

	BOOST_CHECK(!p.in_specific_scope(test_parser::default_scope, std::make_pair(ns_name::HTML, tag_name::A)));
}

/*
https://html.spec.whatwg.org/multipage/parsing.html#the-list-of-active-formatting-elements
*/
BOOST_AUTO_TEST_CASE(simple_parser_push_active_formatting_element_list_1)
{
	tree t;
	auto html = t.insert(t.end(), basic_element<std::string>(ns_name::HTML, "", tag_name::Html));
	auto a1 = t.insert(html.end(), basic_element<std::string>(ns_name::HTML, "", tag_name::A));
	auto a2 = t.insert(a1.end(), basic_element<std::string>(ns_name::HTML, "", tag_name::A));
	auto a3 = t.insert(a2.end(), basic_element<std::string>(ns_name::HTML, "", tag_name::A));
	auto a4 = t.insert(a3.end(), basic_element<std::string>(ns_name::HTML, "", tag_name::A));
	auto a5 = t.insert(a4.end(), basic_element<std::string>(ns_name::HTML, "", tag_name::A));
	test_parser p;
	start_tag_token token1;
	token1.m_tag_name = U"A";
	p.push_active_formatting_element_list(a1, token1);
	p.push_active_formatting_element_list(a2, token1);
	p.push_active_formatting_element_list(a3, token1);
	p.push_active_formatting_element_list(a4, token1);
	p.push_active_formatting_element_list(a5, token1);

	BOOST_CHECK(p.m_active_formatting_element_list.back().m_it == a3);
	BOOST_CHECK(p.m_active_formatting_element_list.size() == 3);
}

BOOST_AUTO_TEST_CASE(simple_parser_push_active_formatting_element_list_2)
{
	tree t;
	auto html = t.insert(t.end(), basic_element<std::string>(ns_name::HTML, "", tag_name::Html));
	auto a1 = t.insert(html.end(), basic_element<std::string>(ns_name::HTML, "", tag_name::A));
	auto a2 = t.insert(a1.end(), basic_element<std::string>(ns_name::HTML, "", tag_name::A));
	auto a3 = t.insert(a2.end(), basic_element<std::string>(ns_name::HTML, "", tag_name::A));
	auto a4 = t.insert(a3.end(), basic_element<std::string>(ns_name::HTML, "", tag_name::A));
	auto a5 = t.insert(a4.end(), basic_element<std::string>(ns_name::HTML, "", tag_name::A));
	test_parser p;
	start_tag_token token1;
	token1.m_tag_name = U"A";
	p.push_active_formatting_element_list(a1, token1);
	p.push_active_formatting_element_list();
	p.push_active_formatting_element_list(a2, token1);
	p.push_active_formatting_element_list(a3, token1);
	p.push_active_formatting_element_list(a4, token1);
	p.push_active_formatting_element_list(a5, token1);

	BOOST_CHECK(p.m_active_formatting_element_list.size() == 5);

	BOOST_CHECK(p.m_active_formatting_element_list[0].m_it == a5);
	BOOST_CHECK(p.m_active_formatting_element_list[1].m_it == a4);
	BOOST_CHECK(p.m_active_formatting_element_list[2].m_it == a3);
	BOOST_CHECK(p.m_active_formatting_element_list[3].m_marker == true);
	BOOST_CHECK(p.m_active_formatting_element_list[4].m_it == a1);
}

BOOST_AUTO_TEST_CASE(simple_parser_reconstruct_active_formatting_element_list_1)
{
	tree t;
	auto html = t.insert(t.end(), basic_element<std::string>(ns_name::HTML, "", tag_name::Html));
	auto a1 = t.insert(html.end(), basic_element<std::string>(ns_name::HTML, "", tag_name::A));
	auto a2 = t.insert(a1.end(), basic_element<std::string>(ns_name::HTML, "", tag_name::A));
	auto a3 = t.insert(a2.end(), basic_element<std::string>(ns_name::HTML, "", tag_name::A));
	auto a4 = t.insert(a3.end(), basic_element<std::string>(ns_name::HTML, "", tag_name::A));
	auto a5 = t.insert(a4.end(), basic_element<std::string>(ns_name::HTML, "", tag_name::A));
	test_parser p;
	start_tag_token token1;
	token1.m_tag_name = U"A";
	p.push_active_formatting_element_list(a1, token1);
	p.push_active_formatting_element_list();
	p.push_active_formatting_element_list(a2, token1);
	p.push_active_formatting_element_list(a3, token1);
	p.push_active_formatting_element_list(a4, token1);
	p.push_active_formatting_element_list(a5, token1);

	p.reconstruct_active_formatting_element_list();

	BOOST_CHECK(p.m_active_formatting_element_list[3].m_marker == true);
}

BOOST_AUTO_TEST_CASE(simple_parser_appropriate_place_for_inserting_node_1)
{
	tree t;
	auto HTML = t.insert(t.end(), basic_element<std::string>(ns_name::HTML, "", tag_name::Html));
	auto TABLE = t.insert(HTML.end(), basic_element<std::string>(ns_name::HTML, "", tag_name::Table));
	auto TEMPLATE = t.insert(TABLE.end(), basic_element<std::string>(ns_name::HTML, "", tag_name::Template));
	test_parser p;
	p.m_open_element_stack.push_back(HTML);
	p.m_open_element_stack.push_back(TABLE);
	p.m_open_element_stack.push_back(TEMPLATE);
	p.m_foster_parenting = true;

	auto it = p.appropriate_place_for_inserting_node(TABLE);

	BOOST_CHECK(it == TEMPLATE.end());
}

BOOST_AUTO_TEST_CASE(simple_parser_appropriate_place_for_inserting_node_2)
{
	test_parser p;
	auto HTML = p.insert(p.end(), basic_element<std::string>(ns_name::HTML, "", tag_name::Html));
	auto P = p.insert(HTML.end(), basic_element<std::string>(ns_name::HTML, "", tag_name::P));
	auto TABLE = p.insert(P.end(), basic_element<std::string>(ns_name::HTML, "", tag_name::Table));
	p.m_open_element_stack.push_back(HTML);
	p.m_open_element_stack.push_back(P);
	p.m_foster_parenting = true;

	auto it = p.appropriate_place_for_inserting_node(TABLE);

	BOOST_CHECK(it == HTML.end());
}

BOOST_AUTO_TEST_CASE(simple_parser_appropriate_place_for_inserting_node_3)
{
	test_parser p;
	auto HTML = p.insert(p.end(), basic_element<std::string>(ns_name::HTML, "", tag_name::Html));
	auto TEMPLATE = p.insert(HTML.end(), basic_element<std::string>(ns_name::HTML, "", tag_name::Template));
	auto TABLE = p.insert(TEMPLATE.end(), basic_element<std::string>(ns_name::HTML, "", tag_name::Table));
	p.m_open_element_stack.push_back(HTML);
	p.m_open_element_stack.push_back(TEMPLATE);
	p.m_open_element_stack.push_back(TABLE);
	p.m_foster_parenting = true;

	auto it = p.appropriate_place_for_inserting_node(TABLE);

	BOOST_CHECK(it == TABLE);
}

BOOST_AUTO_TEST_CASE(simple_parser_appropriate_place_for_inserting_node_4)
{
	test_parser p;
	auto TABLE = p.insert(p.end(), basic_element<std::string>(ns_name::HTML, "", tag_name::Table));
	auto P = p.insert(TABLE.end(), basic_element<std::string>(ns_name::HTML, "", tag_name::P));
	p.m_open_element_stack.push_back(P);
	p.m_open_element_stack.push_back(TABLE);
	p.m_foster_parenting = true;

	auto it = p.appropriate_place_for_inserting_node(TABLE);

	BOOST_CHECK(it == P.end());
}

BOOST_AUTO_TEST_CASE(simple_parser_appropriate_place_for_inserting_node_5)
{
	test_parser p;
	auto TABLE = p.insert(p.end(), basic_element<std::string>(ns_name::HTML, "", tag_name::Table));
	auto P = p.insert(TABLE.end(), basic_element<std::string>(ns_name::HTML, "", tag_name::P));
	p.m_open_element_stack.push_back(P);
	p.m_open_element_stack.push_back(TABLE);

	auto it = p.appropriate_place_for_inserting_node(TABLE);

	BOOST_CHECK(it == TABLE.end());
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
