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
	using namespace wordring::whatwg::html;
	using namespace wordring::whatwg::html::parsing;

	using node_type      = simple_node<std::string>;
	using tree           = wordring::tree<node_type>;
	using policy_type    = simple_policy<std::string, tree>;

	class test_parser : public simple_parser<test_parser, std::string, tree>
	{
		using base_type = simple_parser<test_parser, std::string, tree>;

	public:
		using document_type               = typename policy::document_type;
		using document_type_type          = typename policy::document_type_type;
		using document_fragment_type      = typename policy::document_fragment_type;
		using element_type                = typename policy::element_type;
		using text_type                   = typename policy::text_type;
		using processing_instruction_type = typename policy::processing_instruction_type;
		using comment_type                = typename policy::comment_type;

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

		using base_type::push_formatting_element_list;
		using base_type::reconstruct_formatting_element_list;
		using base_type::clear_formatting_element_list;

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
		using base_type::is_mathml_text_integration_point;
		using base_type::is_html_integration_point;

		using base_type::on_initial_insertion_mode;
		using base_type::in_quirks_condition;
		using base_type::in_limited_quirks_condition;

		using base_type::default_scope;
		using base_type::list_item_scope;
		using base_type::button_scope;
		using base_type::table_scope;
		using base_type::select_scope;

		using base_type::m_insertion_mode;

		using base_type::m_stack;
		using base_type::m_list;
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


		document_type* to_document(node_pointer it) { return std::get_if<document_type>(std::addressof(*it)); }

		document_type_type const* to_document_type(node_pointer it) const { return std::get_if<document_type_type>(std::addressof(*it)); }

		document_fragment_type const* to_document_fragment(node_pointer it) const { return std::get_if<document_fragment_type>(std::addressof(*it)); }

		element_type const* to_element(node_pointer it) const { return std::get_if<element_type>(std::addressof(*it)); }

		text_type* to_text(node_pointer it) { return std::get_if<text_type>(std::addressof(*it)); }

		processing_instruction_type const* to_processing_instruction(node_pointer it) const { return std::get_if<processing_instruction_type>(std::addressof(*it)); }

		comment_type const* to_comment(node_pointer it) const { return std::get_if<comment_type>(std::addressof(*it)); }

	};
}

BOOST_AUTO_TEST_SUITE(simple_parser_test)

BOOST_AUTO_TEST_CASE(simple_parser_construct_1)
{
	test_parser p;
	std::u32string s = U"<!-- Comment -->";
	for (char32_t cp : s) p.push_code_point(cp);
	p.push_eof();

	BOOST_CHECK(p.to_comment(p.document().begin())->data() == " Comment ");
}

BOOST_AUTO_TEST_CASE(simple_parser_contains_1)
{
	using namespace wordring::whatwg::html::parsing;

	test_parser p;
	auto el = p.create_element(p.document(), tag_name::P);
	auto abbr = p.create_attribute(el, ns_name::HTML, U"", U"abbr");
	p.append_attribute(el, std::move(abbr));
	auto it = p.insert_element(p.document().end(), el);

	BOOST_CHECK(p.contains(it, ns_name::HTML, U"", U"abbr"));
}

BOOST_AUTO_TEST_CASE(simple_parser_contains_2)
{
	using namespace wordring::whatwg::html::parsing;

	test_parser p;
	auto el = p.create_element(p.document(), tag_name::P);
	auto abbr = p.create_attribute(el, ns_name::HTML, U"", U"abbr");
	p.append_attribute(el, std::move(abbr));
	auto it = p.insert_element(p.document().end(), el);

	BOOST_CHECK(p.contains(it, ns_name::HTML, U"", U"") == false);
}

BOOST_AUTO_TEST_CASE(simple_parser_find_attribute_1)
{
	test_parser p;
	auto el = p.create_element(p.document(), tag_name::P);
	auto attr = p.create_attribute(el, ns_name::HTML, U"", U"attr");
	p.append_attribute(el, std::move(attr));
	auto it = p.insert_element(p.document().end(), el);
	BOOST_CHECK(it != p.document().end());
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
