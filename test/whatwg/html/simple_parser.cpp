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
		using base_type::push_code_point;

		using base_type::insertion_mode;
		using base_type::current_node;
		using base_type::adjusted_current_node;
		using base_type::is_special;
		using base_type::is_formatting;
		using base_type::in_specific_scope;
		using base_type::is_default_scope;
		using base_type::is_list_item_scope;
		using base_type::is_button_scope;
		using base_type::is_table_scope;
		using base_type::is_select_scope;

		using base_type::push_active_formatting_element_list;
		using base_type::reconstruct_active_formatting_element_list;
		using base_type::clear_active_formatting_element_list;

		using base_type::appropriate_place_for_inserting_node;
		using base_type::create_element_for_token;
		using base_type::insert_foreign_element;
		using base_type::insert_html_element;
		using base_type::adjust_mathml_attributes;
		using base_type::adjust_svg_attributes;
		using base_type::adjust_foreign_attributes;

		using base_type::insert_character;
		using base_type::insert_comment;

		using base_type::process_token;
		using base_type::reprocess_token;
		using base_type::on_emit_token;

		using base_type::on_initial_insertion_mode;
		using base_type::in_quirks_condition;
		using base_type::in_limited_quirks_condition;

		using base_type::default_scope;
		using base_type::list_item_scope;
		using base_type::button_scope;
		using base_type::table_scope;
		using base_type::select_scope;

		using base_type::m_open_element_stack;
		using base_type::m_active_formatting_element_list;
		using base_type::m_foster_parenting;

		using base_type::to_document;
		using base_type::to_document_type;
		using base_type::to_document_fragment;
		using base_type::to_element;
		using base_type::to_text;
		using base_type::to_processing_instruction;
		using base_type::to_comment;

		using base_type::namespace_uri_name;
		using base_type::local_name_name;

		using base_type::insert;

		using base_type::m_c;
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

	BOOST_CHECK(p.to_comment(p.document().begin())->data() == " Comment ");
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

// --------------------------------------------------------------------
// 整形要素のリスト
// 
// 12.2.4.3 The list of active formatting elements
// https://html.spec.whatwg.org/multipage/parsing.html#the-list-of-active-formatting-elements
// --------------------------------------------------------------------

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

// --------------------------------------------------------------------
// ノードの作成と挿入
//
// 12.2.6.1 Creating and inserting nodes
// https://html.spec.whatwg.org/multipage/parsing.html#creating-and-inserting-nodes
// --------------------------------------------------------------------

/*
https://html.spec.whatwg.org/multipage/parsing.html#appropriate-place-for-inserting-a-node
*/
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
	auto document = p.document();
	auto HTML = p.insert(document.end(), basic_element<std::string>(ns_name::HTML, "", tag_name::Html));
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
	auto document = p.document();
	auto HTML = p.insert(document.end(), basic_element<std::string>(ns_name::HTML, "", tag_name::Html));
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
	auto document = p.document();
	auto TABLE = p.insert(document.end(), basic_element<std::string>(ns_name::HTML, "", tag_name::Table));
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
	auto document = p.document();
	auto TABLE = p.insert(document.end(), basic_element<std::string>(ns_name::HTML, "", tag_name::Table));
	auto P = p.insert(TABLE.end(), basic_element<std::string>(ns_name::HTML, "", tag_name::P));
	p.m_open_element_stack.push_back(P);
	p.m_open_element_stack.push_back(TABLE);

	auto it = p.appropriate_place_for_inserting_node(TABLE);

	BOOST_CHECK(it == TABLE.end());
}

/*
https://html.spec.whatwg.org/multipage/parsing.html#create-an-element-for-the-token
*/
BOOST_AUTO_TEST_CASE(simple_parser_create_element_for_token_1)
{
	test_parser p;
	auto document = p.document();
	auto HTML = p.insert(document.end(), basic_element<std::string>(ns_name::HTML, "", tag_name::Html));
	start_tag_token token;
	token.m_tag_name = U"body";
	auto el = p.create_element_for_token(token, ns_name::HTML, HTML);

}

BOOST_AUTO_TEST_CASE(simple_parser_adjust_mathml_attributes_1)
{
	test_parser p;
	start_tag_token token;
	token.m_attributes.create();
	token.m_attributes.current().m_name = U"definitionurl";

	p.adjust_mathml_attributes(token);

	BOOST_CHECK(token.m_attributes.current().m_name == U"definitionURL");
}

BOOST_AUTO_TEST_CASE(simple_parser_adjust_svg_attributes_1)
{
	test_parser p;
	start_tag_token token;
	token.m_attributes.create();
	token.m_attributes.current().m_name = U"attributename";

	p.adjust_svg_attributes(token);

	BOOST_CHECK(token.m_attributes.current().m_name == U"attributeName");
}

BOOST_AUTO_TEST_CASE(simple_parser_adjust_svg_attributes_2)
{
	test_parser p;
	start_tag_token token;
	token.m_attributes.create();
	token.m_attributes.current().m_name = U"attributenameABCD";

	p.adjust_svg_attributes(token);

	BOOST_CHECK(token.m_attributes.current().m_name == U"attributenameABCD");
}

BOOST_AUTO_TEST_CASE(simple_parser_adjust_foreign_attributes_1)
{
	test_parser p;
	start_tag_token token;
	token.m_attributes.create();
	token.m_attributes.current().m_name = U"xlink:actuate";

	p.adjust_foreign_attributes(token);

	BOOST_CHECK(token.m_attributes.current().m_name == U"xlink:actuate");
	BOOST_CHECK(token.m_attributes.current().m_prefix == U"xlink");
	BOOST_CHECK(token.m_attributes.current().m_local_name == U"actuate");
	BOOST_CHECK(token.m_attributes.current().m_namespace == ns_name::XLink);
}

BOOST_AUTO_TEST_CASE(simple_parser_adjust_foreign_attributes_2)
{
	test_parser p;
	start_tag_token token;
	token.m_attributes.create();
	token.m_attributes.current().m_name = U"xmlns";

	p.adjust_foreign_attributes(token);

	BOOST_CHECK(token.m_attributes.current().m_name == U"xmlns");
	BOOST_CHECK(token.m_attributes.current().m_prefix == U"");
	BOOST_CHECK(token.m_attributes.current().m_local_name == U"xmlns");
	BOOST_CHECK(token.m_attributes.current().m_namespace == ns_name::XMLNS);
}




// --------------------------------------------------------------------
// 12.2.6.4.1 The "initial" insertion mode
//
// https://html.spec.whatwg.org/multipage/parsing.html#the-initial-insertion-mode
// --------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(simple_parser_in_quirks_condition_1)
{
	test_parser p;
	DOCTYPE_token token;
	token.m_name = U"html";
	token.m_public_identifier = U"-//ietf//dtd html strict//";

	BOOST_CHECK(p.in_quirks_condition(token));
}

BOOST_AUTO_TEST_CASE(simple_parser_in_quirks_condition_2)
{
	test_parser p;
	DOCTYPE_token token;
	token.m_name = U"html";
	token.m_public_identifier = U"-//ietf//dtd html strict//abcd";

	BOOST_CHECK(p.in_quirks_condition(token));
}

BOOST_AUTO_TEST_CASE(simple_parser_in_quirks_condition_3)
{
	test_parser p;
	DOCTYPE_token token;
	token.m_name = U"html";
	token.m_public_identifier = U"-//ietf//dtd html strict/";

	BOOST_CHECK(!(p.in_quirks_condition(token)));
}

BOOST_AUTO_TEST_CASE(simple_parser_in_quirks_condition_4)
{
	test_parser p;
	DOCTYPE_token token;
	token.m_name = U"html";
	token.m_public_identifier = U"-//w3c//dtd html 4.01 frameset//";

	BOOST_CHECK(p.in_quirks_condition(token));
}

BOOST_AUTO_TEST_CASE(simple_parser_in_quirks_condition_5)
{
	test_parser p;
	DOCTYPE_token token;
	token.m_name = U"html";
	token.m_public_identifier = U"-//w3c//dtd html 4.01 frameset//ABCD";

	BOOST_CHECK(p.in_quirks_condition(token));
}

BOOST_AUTO_TEST_CASE(simple_parser_in_quirks_condition_6)
{
	test_parser p;
	DOCTYPE_token token;
	token.m_name = U"html";
	token.m_public_identifier = U"-//w3c//dtd html 4.01 frameset/";

	BOOST_CHECK(!p.in_quirks_condition(token));
}

BOOST_AUTO_TEST_CASE(simple_parser_in_limited_quirks_condition_1)
{
	test_parser p;
	DOCTYPE_token token;
	token.m_name = U"html";
	token.m_public_identifier = U"-//w3c//dtd xhtml 1.0 frameset//";

	BOOST_CHECK(p.in_limited_quirks_condition(token));
}

BOOST_AUTO_TEST_CASE(simple_parser_in_limited_quirks_condition_2)
{
	test_parser p;
	DOCTYPE_token token;
	token.m_name = U"html";
	token.m_public_identifier = U"-//w3c//dtd xhtml 1.0 frameset//ABCD";

	BOOST_CHECK(p.in_limited_quirks_condition(token));
}

BOOST_AUTO_TEST_CASE(simple_parser_insert_character_1)
{
	test_parser p;
	auto HTML = p.m_c.insert(p.document().end(), basic_element<std::string>(ns_name::HTML, "", tag_name::Html));
	p.m_open_element_stack.push_back(HTML);
	p.m_foster_parenting = true;

	auto it = p.appropriate_place_for_inserting_node(HTML);
	BOOST_CHECK(it == HTML.end());

	p.insert_character(U'A');

	BOOST_CHECK(p.to_text(HTML.begin())->data() == "A");
}

BOOST_AUTO_TEST_CASE(simple_parser_insert_character_2)
{
	test_parser p;
	auto HTML = p.m_c.insert(p.document().end(), basic_element<std::string>(ns_name::HTML, "", tag_name::Html));
	p.m_open_element_stack.push_back(HTML);
	p.m_foster_parenting = true;

	auto it = p.appropriate_place_for_inserting_node(HTML);
	BOOST_CHECK(it == HTML.end());

	p.insert_character(U'A');
	p.insert_character(U'B');
	p.insert_character(U'C');

	BOOST_CHECK(p.to_text(HTML.begin())->data() == "ABC");
}

BOOST_AUTO_TEST_CASE(simple_parser_insert_comment_1)
{
	test_parser p;
	auto HTML = p.m_c.insert(p.document().end(), basic_element<std::string>(ns_name::HTML, "", tag_name::Html));

	comment_token comment;
	comment.m_data = U"ABC";

	p.insert_comment(comment, HTML.end());

	BOOST_CHECK(p.to_comment(HTML.begin())->data() == "ABC");
}

BOOST_AUTO_TEST_CASE(simple_parser_insert_comment_2)
{
	test_parser p;
	auto HTML = p.m_c.insert(p.document().end(), basic_element<std::string>(ns_name::HTML, "", tag_name::Html));

	p.m_open_element_stack.push_back(HTML);
	p.m_foster_parenting = true;

	auto it = p.appropriate_place_for_inserting_node(HTML);
	BOOST_CHECK(it == HTML.end());

	comment_token comment;
	comment.m_data = U"ABC";

	p.insert_comment(comment);

	BOOST_CHECK(p.to_comment(HTML.begin())->data() == "ABC");
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
