// test/whatwg/html/parsing/tree_construction_dispatcher.cpp

#include <boost/test/unit_test.hpp>

#include <wordring/whatwg/html/simple_node.hpp>
#include <wordring/whatwg/html/simple_parser.hpp>
#include <wordring/whatwg/html/simple_policy.hpp>

#include <wordring/whatwg/html/parsing/atom_tbl.hpp>
#include <wordring/whatwg/html/parsing/token.hpp>
#include <wordring/whatwg/html/parsing/tokenization.hpp>
#include <wordring/whatwg/html/parsing/tree_construction_dispatcher.hpp>

#include <wordring/tree/tree.hpp>

#include <string>
#include <vector>

namespace
{
	using namespace wordring::whatwg::html;
	using namespace wordring::whatwg::html::parsing;

	using node_type   = simple_node<std::string>;
	using tree        = wordring::tree<node_type>;
	using policy_type = simple_policy<std::string, tree>;


	class test_parser : public simple_parser<test_parser, std::string, tree>
	{
	public:
		using base_type = simple_parser<test_parser, std::string, tree>;

	public:
		using document_type               = typename policy_type::document_type;
		using document_type_type          = typename policy_type::document_type_type;
		using document_fragment_type      = typename policy_type::document_fragment_type;
		using element_type                = typename policy_type::element_type;
		using text_type                   = typename policy_type::text_type;
		using processing_instruction_type = typename policy_type::processing_instruction_type;
		using comment_type                = typename policy_type::comment_type;

		using base_type::mode_type;
		using base_type::stack_entry;

		using base_type::mode_name;

	public:
		using base_type::push_code_point;

		using base_type::insertion_mode;
		using base_type::reset_insertion_mode_appropriately;
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
		using base_type::pop_until;

		using base_type::push_active_formatting_element_list;
		using base_type::reconstruct_active_formatting_element_list;
		using base_type::clear_active_formatting_element_list;

		using base_type::process_token;
		using base_type::reprocess_token;
		using base_type::on_emit_token;
		using base_type::is_mathml_text_integration_point;
		using base_type::is_html_integration_point;

		using base_type::appropriate_place_for_inserting_node;
		using base_type::create_element_for_token;
		using base_type::insert_foreign_element;
		using base_type::insert_html_element;
		using base_type::adjust_mathml_attributes;
		using base_type::adjust_svg_attributes;
		using base_type::adjust_foreign_attributes;

		using base_type::insert_character;
		using base_type::insert_comment;

		using base_type::parse_generic_raw_text_element;
		using base_type::parse_generic_raw_rcdata_element;
		using base_type::generate_implied_end_tags;
		using base_type::generate_all_implied_end_tags_thoroughly;

		using base_type::on_initial_insertion_mode;
		using base_type::in_quirks_condition;
		using base_type::in_limited_quirks_condition;
		using base_type::on_before_html_insertion_mode;
		using base_type::on_before_head_insertion_mode;
		using base_type::on_in_head_insertion_mode;
		using base_type::on_in_head_noscript_insertion_mode;
		using base_type::on_after_head_insertion_mode;
		using base_type::on_in_body_insertion_mode;
		using base_type::on_text_insertion_mode;
		using base_type::on_in_table_insertion_mode;
		using base_type::on_in_table_text_insertion_mode;
		using base_type::on_in_caption_insertion_mode;
		using base_type::on_in_column_group_insertion_mode;
		using base_type::on_in_table_body_insertion_mode;
		using base_type::on_in_row_insertion_mode;
		using base_type::on_in_cell_insertion_mode;
		using base_type::on_in_select_insertion_mode;
		using base_type::on_in_select_in_table_insertion_mode;
		using base_type::on_in_template_insertion_mode;
		using base_type::on_after_body_insertion_mode;
		using base_type::on_in_frameset_insertion_mode;
		using base_type::on_after_frameset_insertion_mode;
		using base_type::on_after_after_body_insertion_mode;
		using base_type::on_after_after_frameset_insertion_mode;
		using base_type::on_foreign_content;

		using base_type::default_scope;
		using base_type::list_item_scope;
		using base_type::button_scope;
		using base_type::table_scope;
		using base_type::select_scope;

		using base_type::m_insertion_mode;

		using base_type::m_open_element_stack;
		using base_type::m_active_formatting_element_list;
		using base_type::m_foster_parenting;

		//using base_type::to_document;
		//using base_type::to_document_type;
		//using base_type::to_document_fragment;
		//using base_type::to_element;
		//using base_type::to_text;
		//using base_type::to_processing_instruction;
		//using base_type::to_comment;

		using base_type::get_namespace_uri_id;
		using base_type::get_local_name_id;

		using base_type::insert_element;

		using base_type::m_c;

		/*
		document_type* to_document(node_pointer it) { return std::get_if<document_type>(std::addressof(*it)); }

		document_type_type const* to_document_type(node_pointer it) const { return std::get_if<document_type_type>(std::addressof(*it)); }

		document_fragment_type const* to_document_fragment(node_pointer it) const { return std::get_if<document_fragment_type>(std::addressof(*it)); }

		element_type const* to_element(node_pointer it) const { return std::get_if<element_type>(std::addressof(*it)); }

		text_type* to_text(node_pointer it) { return std::get_if<text_type>(std::addressof(*it)); }

		processing_instruction_type const* to_processing_instruction(node_pointer it) const { return std::get_if<processing_instruction_type>(std::addressof(*it)); }

		comment_type const* to_comment(node_pointer it) const { return std::get_if<comment_type>(std::addressof(*it)); }
		*/

		void on_report_error(error_name ec) { ++m_error_count; }

		test_parser()
			: m_error_count(0)
		{
		}

		std::uint32_t m_error_count;
	};
}

BOOST_AUTO_TEST_SUITE(tree_construction_dispatcher_test)

BOOST_AUTO_TEST_CASE(tree_construction_dispatcher_construct_1)
{
	using namespace wordring::whatwg::html;
	using namespace wordring::whatwg::html::parsing;

	test_parser p;
	std::u32string s = U"<!-- Comment -->";
	for (char32_t cp : s) p.push_code_point(cp);
	p.push_eof();

	BOOST_CHECK(data(*p.document().begin()) == " Comment ");
}

// ------------------------------------------------------------------------------------------------
// 挿入モード
//
// 12.2.4.1 The insertion mode
// https://html.spec.whatwg.org/multipage/parsing.html#the-insertion-mode
// ------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(tree_construction_dispatcher_reset_insertion_mode_appropriately_1)
{
	using namespace wordring::whatwg::html;

	test_parser p;
	auto document = p.document();
	auto HTML = p.insert_element(document.end(), simple_element<std::string>(ns_name::HTML, "", tag_name::Html));
	auto TABLE = p.insert_element(HTML.end(), simple_element<std::string>(ns_name::HTML, "", tag_name::Table));
	auto SELECT = p.insert_element(TABLE.end(), simple_element<std::string>(ns_name::HTML, "", tag_name::Select));
	p.m_open_element_stack.push_back({ start_tag_token(), HTML });
	p.m_open_element_stack.push_back({ start_tag_token(), TABLE });
	p.m_open_element_stack.push_back({ start_tag_token(), SELECT });

	p.reset_insertion_mode_appropriately();

	BOOST_CHECK(p.m_insertion_mode == test_parser::mode_name::in_select_in_table_insertion_mode);
}

BOOST_AUTO_TEST_CASE(tree_construction_dispatcher_reset_insertion_mode_appropriately_2)
{
	test_parser p;
	auto document = p.document();
	auto HTML = p.insert_element(document.end(), simple_element<std::string>(ns_name::HTML, "", tag_name::Html));
	auto TABLE = p.insert_element(HTML.end(), simple_element<std::string>(ns_name::HTML, "", tag_name::Table));
	auto TEMPLATE = p.insert_element(TABLE.end(), simple_element<std::string>(ns_name::HTML, "", tag_name::Template));
	auto SELECT = p.insert_element(TEMPLATE.end(), simple_element<std::string>(ns_name::HTML, "", tag_name::Select));
	p.m_open_element_stack.push_back({ start_tag_token(), HTML });
	p.m_open_element_stack.push_back({ start_tag_token(), TABLE });
	p.m_open_element_stack.push_back({ start_tag_token(), TEMPLATE });
	p.m_open_element_stack.push_back({ start_tag_token(), SELECT });

	p.reset_insertion_mode_appropriately();

	BOOST_CHECK(p.m_insertion_mode == test_parser::mode_name::in_select_insertion_mode);
}

BOOST_AUTO_TEST_CASE(tree_construction_dispatcher_reset_insertion_mode_appropriately_3)
{
	test_parser p;
	auto document = p.document();
	auto HTML = p.insert_element(document.end(), simple_element<std::string>(ns_name::HTML, "", tag_name::Html));
	auto HEAD = p.insert_element(HTML.end(), simple_element<std::string>(ns_name::HTML, "", tag_name::Head));
	auto A = p.insert_element(HEAD.end(), simple_element<std::string>(ns_name::HTML, "", tag_name::A));
	p.m_open_element_stack.push_back({ start_tag_token(), HTML });
	p.m_open_element_stack.push_back({ start_tag_token(), HEAD });
	p.m_open_element_stack.push_back({ start_tag_token(), A });

	p.reset_insertion_mode_appropriately();

	BOOST_CHECK(p.m_insertion_mode == test_parser::mode_name::in_head_insertion_mode);
}

BOOST_AUTO_TEST_CASE(tree_construction_dispatcher_reset_insertion_mode_appropriately_4)
{
	test_parser p;
	auto document = p.document();
	auto HTML = p.insert_element(document.end(), simple_element<std::string>(ns_name::HTML, "", tag_name::Html));
	auto HTML2 = p.insert_element(HTML.end(), simple_element<std::string>(ns_name::HTML, "", tag_name::Html));
	auto A = p.insert_element(HTML2.end(), simple_element<std::string>(ns_name::HTML, "", tag_name::A));
	p.m_open_element_stack.push_back({ start_tag_token(), HTML });
	p.m_open_element_stack.push_back({ start_tag_token(), HTML2 });
	p.m_open_element_stack.push_back({ start_tag_token(), A });

	p.reset_insertion_mode_appropriately();

	BOOST_CHECK(p.m_insertion_mode == test_parser::mode_name::before_head_insertion_mode);
}


// ------------------------------------------------------------------------------------------------
// スタック
//
// 12.2.4.2 The stack of open elements
// https://html.spec.whatwg.org/multipage/parsing.html#the-stack-of-open-elements
// ------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(tree_construction_dispatcher_current_node_1)
{
}

BOOST_AUTO_TEST_CASE(tree_construction_dispatcher_adjusted_current_node_1)
{
}

BOOST_AUTO_TEST_CASE(tree_construction_dispatcher_is_special_1)
{
	test_parser p;
	tree t;
	t.insert(t.end(), simple_element<std::string>(ns_name::HTML, "", tag_name::Address));

	BOOST_CHECK(p.is_special(t.begin()));
}

BOOST_AUTO_TEST_CASE(tree_construction_dispatcher_is_formatting_1)
{
	test_parser p;
	tree t;
	t.insert(t.end(), simple_element<std::string>(ns_name::HTML, "", tag_name::Font));

	BOOST_CHECK(p.is_formatting(t.begin()));
}

BOOST_AUTO_TEST_CASE(tree_construction_dispatcher_in_specific_scope_1)
{
	tree t;
	t.insert(t.end(), simple_element<std::string>(ns_name::HTML, "", tag_name::Html));
	t.insert(t.begin().end(), simple_element<std::string>(ns_name::HTML, "", tag_name::Font));
	test_parser p;
	p.m_open_element_stack.push_back({ start_tag_token(), t.begin() });
	p.m_open_element_stack.push_back({ start_tag_token(), t.begin().begin() });

	BOOST_CHECK(p.in_specific_scope(test_parser::default_scope, std::make_pair(ns_name::HTML, tag_name::Font)));
}

BOOST_AUTO_TEST_CASE(tree_construction_dispatcher_in_specific_scope_2)
{
	tree t;
	t.insert(t.end(), simple_element<std::string>(ns_name::HTML, "", tag_name::Html));
	t.insert(t.begin().end(), simple_element<std::string>(ns_name::HTML, "", tag_name::Font));
	test_parser p;
	p.m_open_element_stack.push_back({ start_tag_token(), t.begin() });
	p.m_open_element_stack.push_back({ start_tag_token(), t.begin().begin() });

	BOOST_CHECK(!p.in_specific_scope(test_parser::default_scope, std::make_pair(ns_name::HTML, tag_name::A)));
}

BOOST_AUTO_TEST_CASE(tree_construction_dispatcher_pop_until_1)
{
	tree t;

	test_parser p;
	auto HTML = p.m_c.insert(p.document().end(), simple_element<std::string>(ns_name::HTML, "", tag_name::Html));
	auto BODY = p.m_c.insert(HTML.end(), simple_element<std::string>(ns_name::HTML, "", tag_name::Body));
	auto P = p.m_c.insert(BODY.end(), simple_element<std::string>(ns_name::HTML, "", tag_name::P));

	p.m_open_element_stack.push_back({ start_tag_token(), HTML });
	p.m_open_element_stack.push_back({ start_tag_token(), BODY });
	p.m_open_element_stack.push_back({ start_tag_token(), P });

	p.pop_until(tag_name::Body);

	BOOST_CHECK(p.get_local_name_id(p.m_open_element_stack.back().m_it) == tag_name::Html);
}

BOOST_AUTO_TEST_CASE(tree_construction_dispatcher_pop_until_2)
{
	tree t;

	test_parser p;
	auto HTML = p.m_c.insert(p.document().end(), simple_element<std::string>(ns_name::HTML, "", tag_name::Html));
	auto BODY = p.m_c.insert(HTML.end(), simple_element<std::string>(ns_name::HTML, "", tag_name::Body));
	auto P = p.m_c.insert(BODY.end(), simple_element<std::string>(ns_name::HTML, "", tag_name::P));

	p.m_open_element_stack.push_back({ start_tag_token(), HTML });
	p.m_open_element_stack.push_back({ start_tag_token(), BODY });
	p.m_open_element_stack.push_back({ start_tag_token(), P });

	std::array<tag_name, 2> constexpr tags = { tag_name::A, tag_name::Body };
	p.pop_until(tags);

	BOOST_CHECK(p.get_local_name_id(p.m_open_element_stack.back().m_it) == tag_name::Html);
}

BOOST_AUTO_TEST_CASE(tree_construction_dispatcher_pop_until_3)
{
	tree t;

	test_parser p;
	auto HTML = p.m_c.insert(p.document().end(), simple_element<std::string>(ns_name::HTML, "", tag_name::Html));
	auto BODY = p.m_c.insert(HTML.end(), simple_element<std::string>(ns_name::HTML, "", tag_name::Body));
	auto P = p.m_c.insert(BODY.end(), simple_element<std::string>(ns_name::HTML, "", tag_name::P));

	p.m_open_element_stack.push_back({ start_tag_token(), HTML });
	p.m_open_element_stack.push_back({ start_tag_token(), BODY });
	p.m_open_element_stack.push_back({ start_tag_token(), P });

	p.pop_until(BODY);

	BOOST_CHECK(p.get_local_name_id(p.m_open_element_stack.back().m_it) == tag_name::Html);
}

// ------------------------------------------------------------------------------------------------
// 整形要素のリスト
// 
// 12.2.4.3 The list of active formatting elements
// https://html.spec.whatwg.org/multipage/parsing.html#the-list-of-active-formatting-elements
// ------------------------------------------------------------------------------------------------

/*
https://html.spec.whatwg.org/multipage/parsing.html#the-list-of-active-formatting-elements
*/
BOOST_AUTO_TEST_CASE(tree_construction_dispatcher_push_active_formatting_element_list_1)
{
	tree t;
	auto html = t.insert(t.end(), simple_element<std::string>(ns_name::HTML, "", tag_name::Html));
	auto a1 = t.insert(html.end(), simple_element<std::string>(ns_name::HTML, "", tag_name::A));
	auto a2 = t.insert(a1.end(), simple_element<std::string>(ns_name::HTML, "", tag_name::A));
	auto a3 = t.insert(a2.end(), simple_element<std::string>(ns_name::HTML, "", tag_name::A));
	auto a4 = t.insert(a3.end(), simple_element<std::string>(ns_name::HTML, "", tag_name::A));
	auto a5 = t.insert(a4.end(), simple_element<std::string>(ns_name::HTML, "", tag_name::A));
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

BOOST_AUTO_TEST_CASE(tree_construction_dispatcher_push_active_formatting_element_list_2)
{
	tree t;
	auto html = t.insert(t.end(), simple_element<std::string>(ns_name::HTML, "", tag_name::Html));
	auto a1 = t.insert(html.end(), simple_element<std::string>(ns_name::HTML, "", tag_name::A));
	auto a2 = t.insert(a1.end(), simple_element<std::string>(ns_name::HTML, "", tag_name::A));
	auto a3 = t.insert(a2.end(), simple_element<std::string>(ns_name::HTML, "", tag_name::A));
	auto a4 = t.insert(a3.end(), simple_element<std::string>(ns_name::HTML, "", tag_name::A));
	auto a5 = t.insert(a4.end(), simple_element<std::string>(ns_name::HTML, "", tag_name::A));
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

BOOST_AUTO_TEST_CASE(tree_construction_dispatcher_reconstruct_active_formatting_element_list_1)
{
	test_parser p;
	auto HTML = p.insert_element(p.document().end(), simple_element<std::string>(ns_name::HTML, "", tag_name::Html));
	auto A1 = p.insert_element(HTML.end(), simple_element<std::string>(ns_name::HTML, "", tag_name::A));
	auto A2 = p.insert_element(A1.end(), simple_element<std::string>(ns_name::HTML, "", tag_name::A));
	auto A3 = p.insert_element(A2.end(), simple_element<std::string>(ns_name::HTML, "", tag_name::A));
	auto A4 = p.insert_element(A3.end(), simple_element<std::string>(ns_name::HTML, "", tag_name::A));
	auto A5 = p.insert_element(A4.end(), simple_element<std::string>(ns_name::HTML, "", tag_name::A));
	start_tag_token token1;
	token1.m_tag_name = U"A";
	p.m_open_element_stack.push_back({ token1, HTML });
	p.push_active_formatting_element_list(A1, token1);
	p.push_active_formatting_element_list();
	p.push_active_formatting_element_list(A2, token1);
	p.push_active_formatting_element_list(A3, token1);
	p.push_active_formatting_element_list(A4, token1);
	p.push_active_formatting_element_list(A5, token1);

	p.reconstruct_active_formatting_element_list();

	BOOST_CHECK(p.m_active_formatting_element_list[3].m_marker == true);
}

BOOST_AUTO_TEST_CASE(tree_construction_dispatcher_clear_active_formatting_element_list_1)
{
	tree t;
	auto html = t.insert(t.end(), simple_element<std::string>(ns_name::HTML, "", tag_name::Html));
	auto a1 = t.insert(html.end(), simple_element<std::string>(ns_name::HTML, "", tag_name::A));
	auto a2 = t.insert(a1.end(), simple_element<std::string>(ns_name::HTML, "", tag_name::A));
	auto a3 = t.insert(a2.end(), simple_element<std::string>(ns_name::HTML, "", tag_name::A));
	auto a4 = t.insert(a3.end(), simple_element<std::string>(ns_name::HTML, "", tag_name::A));
	auto a5 = t.insert(a4.end(), simple_element<std::string>(ns_name::HTML, "", tag_name::A));
	test_parser p;
	start_tag_token token1;
	token1.m_tag_name = U"A";
	p.push_active_formatting_element_list(a1, token1);
	p.push_active_formatting_element_list();
	p.push_active_formatting_element_list(a2, token1);
	p.push_active_formatting_element_list(a3, token1);
	p.push_active_formatting_element_list(a4, token1);
	p.push_active_formatting_element_list(a5, token1);

	p.clear_active_formatting_element_list();

	BOOST_CHECK(p.m_active_formatting_element_list.size() == 1);
}

// ------------------------------------------------------------------------------------------------
// ツリー構築
//
// 12.2.6 Tree construction
// https://html.spec.whatwg.org/multipage/parsing.html#tree-construction
// ------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(tree_construction_dispatcher_is_mathml_text_integration_point_1)
{
	test_parser p;
	auto it = p.insert_element(p.document().end(), simple_element<std::string>(ns_name::MathML, "", tag_name::Mtext));
	start_tag_token token;
	token.m_tag_name_id = tag_name::Mtext;
	test_parser::stack_entry entry{ token, it };

	BOOST_CHECK(p.is_mathml_text_integration_point(entry));
}

BOOST_AUTO_TEST_CASE(tree_construction_dispatcher_is_html_integration_point_1)
{
	test_parser p;
	auto it = p.insert_element(p.document().end(), simple_element<std::string>(ns_name::MathML, "", tag_name::Annotation_xml));
	start_tag_token token;
	token.m_tag_name_id = tag_name::Annotation_xml;
	auto& attr = token.m_attributes.create();
	attr.m_name = U"encoding";// attribute_name::Encoding;
	attr.m_value = U"Application/xhtml+xml";
	test_parser::stack_entry entry{ token, it };

	BOOST_CHECK(p.is_html_integration_point(entry));
}

// ------------------------------------------------------------------------------------------------
// ノードの作成と挿入
//
// 12.2.6.1 Creating and inserting nodes
// https://html.spec.whatwg.org/multipage/parsing.html#creating-and-inserting-nodes
// ------------------------------------------------------------------------------------------------

/*
https://html.spec.whatwg.org/multipage/parsing.html#appropriate-place-for-inserting-a-node
*/
BOOST_AUTO_TEST_CASE(tree_construction_dispatcher_appropriate_place_for_inserting_node_1)
{
	tree t;
	auto HTML = t.insert(t.end(), simple_element<std::string>(ns_name::HTML, "", tag_name::Html));
	auto TABLE = t.insert(HTML.end(), simple_element<std::string>(ns_name::HTML, "", tag_name::Table));
	auto TEMPLATE = t.insert(TABLE.end(), simple_element<std::string>(ns_name::HTML, "", tag_name::Template));
	test_parser p;
	p.m_open_element_stack.push_back({ start_tag_token(), HTML });
	p.m_open_element_stack.push_back({ start_tag_token(), TABLE });
	p.m_open_element_stack.push_back({ start_tag_token(), TEMPLATE });
	p.m_foster_parenting = true;

	auto it = p.appropriate_place_for_inserting_node(TABLE);

	BOOST_CHECK(it == TEMPLATE.end());
}

BOOST_AUTO_TEST_CASE(tree_construction_dispatcher_appropriate_place_for_inserting_node_2)
{
	test_parser p;
	auto document = p.document();
	auto HTML = p.insert_element(document.end(), simple_element<std::string>(ns_name::HTML, "", tag_name::Html));
	auto P = p.insert_element(HTML.end(), simple_element<std::string>(ns_name::HTML, "", tag_name::P));
	auto TABLE = p.insert_element(P.end(), simple_element<std::string>(ns_name::HTML, "", tag_name::Table));
	p.m_open_element_stack.push_back({ start_tag_token(), HTML });
	p.m_open_element_stack.push_back({ start_tag_token(), P });
	p.m_foster_parenting = true;

	auto it = p.appropriate_place_for_inserting_node(TABLE);

	BOOST_CHECK(it == HTML.end());
}

BOOST_AUTO_TEST_CASE(tree_construction_dispatcher_appropriate_place_for_inserting_node_3)
{
	test_parser p;
	auto document = p.document();
	auto HTML = p.insert_element(document.end(), simple_element<std::string>(ns_name::HTML, "", tag_name::Html));
	auto TEMPLATE = p.insert_element(HTML.end(), simple_element<std::string>(ns_name::HTML, "", tag_name::Template));
	auto TABLE = p.insert_element(TEMPLATE.end(), simple_element<std::string>(ns_name::HTML, "", tag_name::Table));
	p.m_open_element_stack.push_back({ start_tag_token(), HTML });
	p.m_open_element_stack.push_back({ start_tag_token(), TEMPLATE });
	p.m_open_element_stack.push_back({ start_tag_token(), TABLE });
	p.m_foster_parenting = true;

	auto it = p.appropriate_place_for_inserting_node(TABLE);

	BOOST_CHECK(it == TABLE);
}

BOOST_AUTO_TEST_CASE(tree_construction_dispatcher_appropriate_place_for_inserting_node_4)
{
	test_parser p;
	auto document = p.document();
	auto TABLE = p.insert_element(document.end(), simple_element<std::string>(ns_name::HTML, "", tag_name::Table));
	auto P = p.insert_element(TABLE.end(), simple_element<std::string>(ns_name::HTML, "", tag_name::P));
	p.m_open_element_stack.push_back({ start_tag_token(), P });
	p.m_open_element_stack.push_back({ start_tag_token(), TABLE });
	p.m_foster_parenting = true;

	auto it = p.appropriate_place_for_inserting_node(TABLE);

	BOOST_CHECK(it == P.end());
}

BOOST_AUTO_TEST_CASE(tree_construction_dispatcher_appropriate_place_for_inserting_node_5)
{
	test_parser p;
	auto document = p.document();
	auto TABLE = p.insert_element(document.end(), simple_element<std::string>(ns_name::HTML, "", tag_name::Table));
	auto P = p.insert_element(TABLE.end(), simple_element<std::string>(ns_name::HTML, "", tag_name::P));
	p.m_open_element_stack.push_back({ start_tag_token(), P });
	p.m_open_element_stack.push_back({ start_tag_token(), TABLE });

	auto it = p.appropriate_place_for_inserting_node(TABLE);

	BOOST_CHECK(it == TABLE.end());
}

/*
https://html.spec.whatwg.org/multipage/parsing.html#create-an-element-for-the-token
*/
BOOST_AUTO_TEST_CASE(tree_construction_dispatcher_create_element_for_token_1)
{
	test_parser p;
	auto document = p.document();
	auto HTML = p.insert_element(document.end(), simple_element<std::string>(ns_name::HTML, "", tag_name::Html));
	start_tag_token token;
	token.m_tag_name = U"body";
	auto el = p.create_element_for_token(token, ns_name::HTML, HTML);

}

BOOST_AUTO_TEST_CASE(tree_construction_dispatcher_create_element_for_token_2)
{
	test_parser p;

	std::u32string s(U"<A href=\"http://uri.com/\">ABC</A>");
	for (char32_t cp : s) p.push_code_point(cp);

}

BOOST_AUTO_TEST_CASE(tree_construction_dispatcher_adjust_mathml_attributes_1)
{
	test_parser p;
	start_tag_token token;
	token.m_attributes.create();
	token.m_attributes.current().m_name = U"definitionurl";

	p.adjust_mathml_attributes(token);

	BOOST_CHECK(token.m_attributes.current().m_name == U"definitionURL");
}

BOOST_AUTO_TEST_CASE(tree_construction_dispatcher_adjust_svg_attributes_1)
{
	test_parser p;
	start_tag_token token;
	token.m_attributes.create();
	token.m_attributes.current().m_name = U"attributename";

	p.adjust_svg_attributes(token);

	BOOST_CHECK(token.m_attributes.current().m_name == U"attributeName");
}

BOOST_AUTO_TEST_CASE(tree_construction_dispatcher_adjust_svg_attributes_2)
{
	test_parser p;
	start_tag_token token;
	token.m_attributes.create();
	token.m_attributes.current().m_name = U"attributenameABCD";

	p.adjust_svg_attributes(token);

	BOOST_CHECK(token.m_attributes.current().m_name == U"attributenameABCD");
}

BOOST_AUTO_TEST_CASE(tree_construction_dispatcher_adjust_foreign_attributes_1)
{
	test_parser p;
	start_tag_token token;
	token.m_attributes.create();
	token.m_attributes.current().m_name = U"xlink:actuate";

	p.adjust_foreign_attributes(token);

	BOOST_CHECK(token.m_attributes.current().m_prefix == U"xlink");
	BOOST_CHECK(token.m_attributes.current().m_name == U"actuate");
	BOOST_CHECK(token.m_attributes.current().m_namespace == ns_name::XLink);
}

BOOST_AUTO_TEST_CASE(tree_construction_dispatcher_adjust_foreign_attributes_2)
{
	test_parser p;
	start_tag_token token;
	token.m_attributes.create();
	token.m_attributes.current().m_name = U"xmlns";

	p.adjust_foreign_attributes(token);

	BOOST_CHECK(token.m_attributes.current().m_name == U"xmlns");
	BOOST_CHECK(token.m_attributes.current().m_prefix == U"");
	BOOST_CHECK(token.m_attributes.current().m_name == U"xmlns");
	BOOST_CHECK(token.m_attributes.current().m_namespace == ns_name::XMLNS);
}

// ------------------------------------------------------------------------------------------------
// 12.2.6.4.1 The "initial" insertion mode
//
// https://html.spec.whatwg.org/multipage/parsing.html#the-initial-insertion-mode
// ------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(tree_construction_dispatcher_in_quirks_condition_1)
{
	test_parser p;
	DOCTYPE_token token;
	token.m_name = U"html";
	token.m_public_identifier = U"-//ietf//dtd html strict//";

	BOOST_CHECK(p.in_quirks_condition(token));
}

BOOST_AUTO_TEST_CASE(tree_construction_dispatcher_in_quirks_condition_2)
{
	test_parser p;
	DOCTYPE_token token;
	token.m_name = U"html";
	token.m_public_identifier = U"-//ietf//dtd html strict//abcd";

	BOOST_CHECK(p.in_quirks_condition(token));
}

BOOST_AUTO_TEST_CASE(tree_construction_dispatcher_in_quirks_condition_3)
{
	test_parser p;
	DOCTYPE_token token;
	token.m_name = U"html";
	token.m_public_identifier = U"-//ietf//dtd html strict/";

	BOOST_CHECK(!(p.in_quirks_condition(token)));
}

BOOST_AUTO_TEST_CASE(tree_construction_dispatcher_in_quirks_condition_4)
{
	test_parser p;
	DOCTYPE_token token;
	token.m_name = U"html";
	token.m_public_identifier = U"-//w3c//dtd html 4.01 frameset//";

	BOOST_CHECK(p.in_quirks_condition(token));
}

BOOST_AUTO_TEST_CASE(tree_construction_dispatcher_in_quirks_condition_5)
{
	test_parser p;
	DOCTYPE_token token;
	token.m_name = U"html";
	token.m_public_identifier = U"-//w3c//dtd html 4.01 frameset//ABCD";

	BOOST_CHECK(p.in_quirks_condition(token));
}

BOOST_AUTO_TEST_CASE(tree_construction_dispatcher_in_quirks_condition_6)
{
	test_parser p;
	DOCTYPE_token token;
	token.m_name = U"html";
	token.m_public_identifier = U"-//w3c//dtd html 4.01 frameset/";

	BOOST_CHECK(!p.in_quirks_condition(token));
}

BOOST_AUTO_TEST_CASE(tree_construction_dispatcher_in_limited_quirks_condition_1)
{
	test_parser p;
	DOCTYPE_token token;
	token.m_name = U"html";
	token.m_public_identifier = U"-//w3c//dtd xhtml 1.0 frameset//";

	BOOST_CHECK(p.in_limited_quirks_condition(token));
}

BOOST_AUTO_TEST_CASE(tree_construction_dispatcher_in_limited_quirks_condition_2)
{
	test_parser p;
	DOCTYPE_token token;
	token.m_name = U"html";
	token.m_public_identifier = U"-//w3c//dtd xhtml 1.0 frameset//ABCD";

	BOOST_CHECK(p.in_limited_quirks_condition(token));
}

BOOST_AUTO_TEST_CASE(tree_construction_dispatcher_insert_character_1)
{
	test_parser p;
	auto HTML = p.m_c.insert(p.document().end(), simple_element<std::string>(ns_name::HTML, "", tag_name::Html));
	p.m_open_element_stack.push_back({ start_tag_token(), HTML });
	p.m_foster_parenting = true;

	auto it = p.appropriate_place_for_inserting_node(HTML);
	BOOST_CHECK(it == HTML.end());

	p.insert_character(U'A');

	BOOST_CHECK(data(*HTML.begin()) == "A");
}

BOOST_AUTO_TEST_CASE(tree_construction_dispatcher_insert_character_2)
{
	test_parser p;
	auto HTML = p.m_c.insert(p.document().end(), simple_element<std::string>(ns_name::HTML, "", tag_name::Html));
	p.m_open_element_stack.push_back({ start_tag_token(), HTML });
	p.m_foster_parenting = true;

	auto it = p.appropriate_place_for_inserting_node(HTML);
	BOOST_CHECK(it == HTML.end());

	p.insert_character(U'A');
	p.insert_character(U'B');
	p.insert_character(U'C');

	BOOST_CHECK(data(*HTML.begin()) == "ABC");
}

BOOST_AUTO_TEST_CASE(tree_construction_dispatcher_insert_comment_1)
{
	test_parser p;
	auto HTML = p.m_c.insert(p.document().end(), simple_element<std::string>(ns_name::HTML, "", tag_name::Html));

	comment_token comment;
	comment.m_data = U"ABC";

	p.base_type::base_type::insert_comment(comment, HTML.end());

	BOOST_CHECK(data(*HTML.begin()) == "ABC");
}

BOOST_AUTO_TEST_CASE(tree_construction_dispatcher_insert_comment_2)
{
	test_parser p;
	auto HTML = p.m_c.insert(p.document().end(), simple_element<std::string>(ns_name::HTML, "", tag_name::Html));

	p.m_open_element_stack.push_back({ start_tag_token(), HTML });
	p.m_foster_parenting = true;

	auto it = p.appropriate_place_for_inserting_node(HTML);
	BOOST_CHECK(it == HTML.end());

	comment_token comment;
	comment.m_data = U"ABC";

	p.base_type::base_type::insert_comment(comment);

	BOOST_CHECK(data(*HTML.begin()) == "ABC");
}


// ------------------------------------------------------------------------------------------------
// 12.2.6.4.4 The "in head" insertion mode
//
// https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-inhead
// ------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(tree_construction_dispatcher_on_in_head_insertion_mode_1)
{
	test_parser p;
	p.insertion_mode(test_parser::mode_name::in_head_insertion_mode);
	end_tag_token token(tag_name::Template);
	p.on_in_head_insertion_mode(token);

	BOOST_CHECK(p.m_error_count == 1);
}


/*
BOOST_AUTO_TEST_CASE(tree_construction_dispatcher__1)
{
}
BOOST_AUTO_TEST_CASE(tree_construction_dispatcher__1)
{
}
BOOST_AUTO_TEST_CASE(tree_construction_dispatcher__1)
{
}
BOOST_AUTO_TEST_CASE(tree_construction_dispatcher__1)
{
}
*/

BOOST_AUTO_TEST_SUITE_END()
