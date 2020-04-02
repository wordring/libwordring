// test/whatwg/html/parsing/tokenization.cpp

#include <boost/test/unit_test.hpp>

#include <wordring/whatwg/html/simple_node.hpp>
#include <wordring/whatwg/html/simple_policy.hpp>
#include <wordring/whatwg/html/parsing/token.hpp>
#include <wordring/whatwg/html/parsing/tokenization.hpp>

#include <wordring/tree/tree.hpp>

#include <list>
#include <string>

namespace
{
	using namespace wordring::whatwg::html::simple;
	using namespace wordring::whatwg::html::parsing;

	using node_type = basic_node<std::u32string>;

	using tree = wordring::tree<node_type>;

	using test_policy = tree_construction_policy<std::u32string, tree>;

	struct test_tokenizer : tokenizer<test_tokenizer, test_policy>
	{
		using base_type = tokenizer<test_tokenizer, test_policy>;

		using base_type::m_c;
		using base_type::m_state;

		using base_type::eof;
		using base_type::return_state;
		using base_type::current_tag_token;

		error m_ec;
		std::u32string m_emited_codepoints;

		//

		bool in_html_namespace(node_pointer it) const
		{
			return true;
		}


		void on_report_error(error ec) { m_ec = ec; }

		template <typename Token>
		void on_emit_token(Token const& token)
		{
		}

		void on_emit_token(character_token const& token)
		{
			m_emited_codepoints.push_back(token.m_data);
		}
	};
}

BOOST_AUTO_TEST_SUITE(whatwg__html__parsing__tokenization__test)

/*
空のトークン化器を構築する

tokenizer()
*/
BOOST_AUTO_TEST_CASE(tokenizer__construct__1)
{
	test_tokenizer tt;
}

// 状態関数 ------------------------------------------------------------------

/* 12.2.5.1 Data state */
BOOST_AUTO_TEST_CASE(tokenizer__on_data_state__1)
{
	test_tokenizer tt;
	tt.push_code_point(U'A');
	BOOST_CHECK(tt.m_c.empty());
}

/* 12.2.5.2 RCDATA state */
BOOST_AUTO_TEST_CASE(tokenizer__on_RCDATA_state__1)
{
	test_tokenizer tt;

	tt.m_state = test_tokenizer::RCDATA_state;
	tt.push_code_point(U'A');

	BOOST_CHECK(tt.m_c.empty());
}

/* 12.2.5.3 RAWTEXT state */
BOOST_AUTO_TEST_CASE(tokenizer__on_RAWTEXT_state__1)
{
	test_tokenizer tt;

	tt.m_state = test_tokenizer::RAWTEXT_state;
	tt.push_code_point(U'A');

	BOOST_CHECK(tt.m_c.empty());
}

/* 12.2.5.4 Script data state */
BOOST_AUTO_TEST_CASE(tokenizer__on_script_data_state__1)
{
	test_tokenizer tt;

	tt.m_state = test_tokenizer::script_data_state;
	tt.push_code_point(U'A');

	BOOST_CHECK(tt.m_c.empty());
}

/* 12.2.5.5 PLAINTEXT state */
BOOST_AUTO_TEST_CASE(tokenizer__on_PLAINTEXT_state__1)
{
	test_tokenizer tt;

	tt.m_state = test_tokenizer::PLAINTEXT_state;
	tt.push_code_point(U'A');
	tt.eof(true);

	BOOST_CHECK(tt.m_c.empty());
}

/*
12.2.5.6 Tag open state
12.2.5.32 Before attribute name state
12.2.5.8 Tag name state
12.2.5.33 Attribute name state
*/
BOOST_AUTO_TEST_CASE(tokenizer__on_attribute_name_state__1)
{
	test_tokenizer tt;

	tt.m_state = test_tokenizer::tag_open_state;
	auto s = std::u32string(U"Tag Attribute=");

	for (char32_t cp : s) tt.push_code_point(cp);

	BOOST_CHECK(tt.m_state == test_tokenizer::before_attribute_value_state);
	BOOST_CHECK(tt.m_c.empty());
}

/*
12.2.5.6 Tag open state
12.2.5.32 Before attribute name state
12.2.5.8 Tag name state
12.2.5.33 Attribute name state
unify_attribute()
12.2.5.35 Before attribute value state
12.2.5.37 Attribute value (single-quoted) state
12.2.5.39 After attribute value (quoted) state
*/
BOOST_AUTO_TEST_CASE(tokenizer__on_attribute_name_state__2)
{
	test_tokenizer tt;

	tt.m_state = test_tokenizer::tag_open_state;
	auto s = std::u32string(U"Tag Attribute='Value1' Attribute='Value2'");

	for (char32_t cp : s) tt.push_code_point(cp);

	BOOST_CHECK(tt.m_state == test_tokenizer::after_attribute_value_quoted_state);
	BOOST_CHECK(tt.current_tag_token().attributes().begin()->m_name == U"attribute");
	BOOST_CHECK(tt.current_tag_token().attributes().begin()->m_value == U"Value1");
	BOOST_CHECK(tt.current_tag_token().attributes().begin()->m_omitted == false);
	BOOST_CHECK((++tt.current_tag_token().attributes().begin())->m_name == U"attribute");
	BOOST_CHECK((++tt.current_tag_token().attributes().begin())->m_value == U"Value2");
	BOOST_CHECK((++tt.current_tag_token().attributes().begin())->m_omitted == true);
	BOOST_CHECK(tt.m_c.empty());
}


/* 12.2.5.42 Markup declaration open state */
BOOST_AUTO_TEST_CASE(tokenizer__on_markup_declaration_open_state__1)
{
	BOOST_CHECK(std::size(U"--") == 3);

	test_tokenizer tt;

	tt.m_state = test_tokenizer::markup_declaration_open_state;
	auto s = std::u32string(U"--");
	for (char32_t cp : s) tt.push_code_point(cp);

	BOOST_CHECK(tt.m_state == test_tokenizer::comment_start_state);
	BOOST_CHECK(tt.m_c.empty());
}

BOOST_AUTO_TEST_CASE(tokenizer__on_markup_declaration_open_state__2)
{
	test_tokenizer tt;

	tt.m_state = test_tokenizer::markup_declaration_open_state;
	auto s = std::u32string(U"--A");
	for (char32_t cp : s) tt.push_code_point(cp);

	BOOST_CHECK(tt.m_state == test_tokenizer::comment_state);
	BOOST_CHECK(tt.m_c.empty());
	BOOST_CHECK(tt.current_comment_token().m_data == U"A");
}

BOOST_AUTO_TEST_CASE(tokenizer__on_markup_declaration_open_state__3)
{
	test_tokenizer tt;

	tt.m_state = test_tokenizer::markup_declaration_open_state;
	auto s = std::u32string(U"-");
	for (char32_t cp : s) tt.push_code_point(cp);
	tt.eof(true);

	BOOST_CHECK(tt.m_state == test_tokenizer::bogus_comment_state);
	BOOST_CHECK(tt.m_c.empty());
}

BOOST_AUTO_TEST_CASE(tokenizer__on_markup_declaration_open_state__4)
{
	BOOST_CHECK(std::size(U"DOCTYPE") == 8);

	test_tokenizer tt;

	tt.m_state = test_tokenizer::markup_declaration_open_state;
	auto s = std::u32string(U"DOCTYPE");
	for (char32_t cp : s) tt.push_code_point(cp);
	BOOST_CHECK(tt.m_state == test_tokenizer::DOCTYPE_state);
}

BOOST_AUTO_TEST_CASE(tokenizer__on_markup_declaration_open_state__5)
{
	BOOST_CHECK(std::size(U"doctype") == 8);

	test_tokenizer tt;

	tt.m_state = test_tokenizer::markup_declaration_open_state;
	auto s = std::u32string(U"doctype");
	for (char32_t cp : s) tt.push_code_point(cp);
	BOOST_CHECK(tt.m_state == test_tokenizer::DOCTYPE_state);
}

BOOST_AUTO_TEST_CASE(tokenizer__on_markup_declaration_open_state__6)
{
	test_tokenizer tt;

	tt.m_state = test_tokenizer::markup_declaration_open_state;
	auto s = std::u32string(U"[CDATA[");
	for (char32_t cp : s) tt.push_code_point(cp);
	BOOST_CHECK(tt.m_state == test_tokenizer::bogus_comment_state);
}

BOOST_AUTO_TEST_CASE(tokenizer__on_markup_declaration_open_state__7)
{
	test_tokenizer tt;

	tt.m_state = test_tokenizer::markup_declaration_open_state;
	auto s = std::u32string(U"[CDATA]");
	for (char32_t cp : s) tt.push_code_point(cp);
	BOOST_CHECK(tt.m_state == test_tokenizer::bogus_comment_state);
}

/*
12.2.5.73 Named character reference state
12.2.5.73 Named character reference state
*/
BOOST_AUTO_TEST_CASE(tokenizer__on_named_character_reference_state__1)
{
	test_tokenizer tt;

	tt.m_state = test_tokenizer::named_character_reference_state;
	tt.return_state(test_tokenizer::data_state);

	auto s = std::u32string(U"Aacute");
	for (char32_t cp : s) tt.push_code_point(cp);
	tt.eof(true);

	BOOST_CHECK(tt.m_state == test_tokenizer::data_state);
	BOOST_CHECK(tt.m_c.empty());
	BOOST_CHECK(tt.m_emited_codepoints == U"\xC1");
}

BOOST_AUTO_TEST_CASE(tokenizer__on_named_character_reference_state__2)
{
	test_tokenizer tt;

	tt.m_state = test_tokenizer::named_character_reference_state;
	tt.return_state(test_tokenizer::data_state);

	auto s = std::u32string(U"Aacute;");
	for (char32_t cp : s) tt.push_code_point(cp);
	tt.eof(true);

	BOOST_CHECK(tt.m_state == test_tokenizer::data_state);
	BOOST_CHECK(tt.m_c.empty());
	BOOST_CHECK(tt.m_emited_codepoints == U"\xC1");
}

BOOST_AUTO_TEST_CASE(tokenizer__on_named_character_reference_state__3)
{
	test_tokenizer tt;

	tt.m_state = test_tokenizer::named_character_reference_state;
	tt.return_state(test_tokenizer::data_state);

	auto s = std::u32string(U"Aacute;=");
	for (char32_t cp : s) tt.push_code_point(cp);
	tt.eof(true);

	BOOST_CHECK(tt.m_state == test_tokenizer::data_state);
	BOOST_CHECK(tt.m_c.empty());
	BOOST_CHECK(tt.m_emited_codepoints == U"\xC1=");
}

BOOST_AUTO_TEST_CASE(tokenizer__on_named_character_reference_state__4)
{
	test_tokenizer tt;

	tt.m_state = test_tokenizer::named_character_reference_state;
	tt.return_state(test_tokenizer::data_state);

	auto s = std::u32string(U"acE;");
	for (char32_t cp : s) tt.push_code_point(cp);
	tt.eof(true);

	BOOST_CHECK(tt.m_state == test_tokenizer::data_state);
	BOOST_CHECK(tt.m_c.empty());
	BOOST_CHECK(tt.m_emited_codepoints == U"\x223E\x333");
}

/*
12.2.5.72 Character reference state
12.2.5.75 Numeric character reference state
12.2.5.76 Hexadecimal character reference start state
12.2.5.78 Hexadecimal character reference state
12.2.5.80 Numeric character reference end state
*/
BOOST_AUTO_TEST_CASE(tokenizer__on_numeric_character_reference_end_state__1)
{
	test_tokenizer tt;

	tt.m_state = test_tokenizer::character_reference_state;
	tt.return_state(test_tokenizer::data_state);

	auto s = std::u32string(U"#x3042;");
	for (char32_t cp : s) tt.push_code_point(cp);
	tt.eof(true);

	BOOST_CHECK(tt.m_state == test_tokenizer::data_state);
	BOOST_CHECK(tt.m_c.empty());
	BOOST_CHECK(tt.m_emited_codepoints == U"あ");
}

/*
12.2.5.72 Character reference state
12.2.5.75 Numeric character reference state
12.2.5.77 Decimal character reference start state
12.2.5.79 Decimal character reference state
12.2.5.80 Numeric character reference end state
*/
BOOST_AUTO_TEST_CASE(tokenizer__on_numeric_character_reference_end_state__2)
{
	test_tokenizer tt;

	tt.m_state = test_tokenizer::character_reference_state;
	tt.return_state(test_tokenizer::data_state);

	auto s = std::u32string(U"#12354;");
	for (char32_t cp : s) tt.push_code_point(cp);
	tt.eof(true);

	BOOST_CHECK(tt.m_state == test_tokenizer::data_state);
	BOOST_CHECK(tt.m_c.empty());
	BOOST_CHECK(tt.m_emited_codepoints == U"あ");
}

/*
12.2.5.80 Numeric character reference end state

character_reference_code_tbl のテスト
*/
BOOST_AUTO_TEST_CASE(tokenizer__on_numeric_character_reference_end_state__3)
{
	test_tokenizer tt;

	tt.m_state = test_tokenizer::character_reference_state;
	tt.return_state(test_tokenizer::data_state);

	auto s = std::u32string(U"#x80;");
	for (char32_t cp : s) tt.push_code_point(cp);
	tt.eof(true);

	BOOST_CHECK(tt.m_state == test_tokenizer::data_state);
	BOOST_CHECK(tt.m_c.empty());
	BOOST_CHECK(tt.m_emited_codepoints == U"\x20AC");
}

BOOST_AUTO_TEST_SUITE_END()
