﻿// test/wwwc/css_syntax/parsing.cpp

#include <boost/test/unit_test.hpp>

#include <wordring/wwwc/css_syntax/parsing.hpp>

#include <algorithm>
#include <any>
#include <iterator>
#include <string>
#include <typeindex>
#include <vector>

namespace
{
	inline std::u32string print(wordring::wwwc::css::function const& in);
	inline std::u32string print(wordring::wwwc::css::simple_block const& in);
	inline std::u32string print(wordring::wwwc::css::at_rule const& in);
	inline std::u32string print(wordring::wwwc::css::qualified_rule const& in);
	inline std::u32string print(wordring::wwwc::css::declaration const& in);

	inline std::u32string print(std::vector<wordring::wwwc::css::declaration> const& in);

	inline std::u32string print(std::any const& c);
	inline std::u32string print(std::vector<std::any> const& in);

	// 数値を文字列化する
	inline std::u32string print(double i)
	{
		std::u32string result;

		std::string s = std::to_string(static_cast<int>(i));
		for (unsigned char c : s) result.push_back(c);

		return result;
	}

	inline std::u32string print(wordring::wwwc::css::function const& in)
	{
		using namespace wordring::wwwc::css;

		std::u32string out;
		out += in.m_name;
		out += U"(";
		out += print(in.m_value);
		out += U")";

		return out;
	}

	inline std::u32string print(wordring::wwwc::css::simple_block const& in)
	{
		using namespace wordring::wwwc::css;

		std::u32string out;
		out += print(in.m_associated_token);
		out += print(in.m_value);
		if (is_open_curly_token(in.m_associated_token)) out += U"}";
		if (is_open_square_token(in.m_associated_token)) out += U"]";
		if (is_open_paren_token(in.m_associated_token)) out += U")";

		return out;
	}

	inline std::u32string print(wordring::wwwc::css::at_rule const& in)
	{
		using namespace wordring::wwwc::css;

		std::u32string out;

		out += U"@";
		out += in.m_name;
		out += print(in.m_prelude);
		if (in.m_block.has_value())
		{
			out += print(*in.m_block);
		}

		return out;
	}

	inline std::u32string print(wordring::wwwc::css::qualified_rule const& in)
	{
		using namespace wordring::wwwc::css;

		std::u32string out;

		out += print(in.m_prelude);
		out += print(in.m_block);

		return out;
	}

	inline std::u32string print(std::optional<wordring::wwwc::css::qualified_rule> const& in)
	{
		return in.has_value() ? print(*in) : U"";
	}

	inline std::u32string print(wordring::wwwc::css::declaration const& in)
	{
		using namespace wordring::wwwc::css;

		std::u32string out;

		out += in.m_name;
		out += U":";
		for(component_value const& c : in.m_value) out += print(c);
		if (in.m_important_flag) out += U"!important";

		return out;
	}

	inline std::u32string print(std::optional<wordring::wwwc::css::declaration> const& in)
	{
		return in.has_value() ? print(*in) : U"";
	}

	inline std::u32string print(std::vector<wordring::wwwc::css::declaration> const& in)
	{
		std::u32string out;
		for (auto const& c : in) out += print(c);

		return out;
	}

	// トークン/コンポーネント値を文字列化する
	inline std::u32string print(std::any const& c)
	{
		using namespace wordring::wwwc::css;

		std::u32string out;

		auto& i = c.type();
		if      (i == typeid(ident_token)       ) out += std::any_cast<ident_token>(c).m_value;
		else if (i == typeid(function_token)    ) out += std::any_cast<function_token>(c).m_value;
		else if (i == typeid(at_keyword_token)  ) out += std::any_cast<at_keyword_token>(c).m_value;
		else if (i == typeid(hash_token)        ) out += std::any_cast<hash_token>(c).m_value;
		else if (i == typeid(string_token)      ) out += std::any_cast<string_token>(c).m_value;
		else if (i == typeid(bad_string_token)  ) out += U" BAD_STRING_TOKEN ";
		else if (i == typeid(url_token)         ) out += std::any_cast<url_token>(c).m_value;
		else if (i == typeid(bad_url_token)     ) out += U" BAD_URL_TOKEN ";
		else if (i == typeid(delim_token)       ) out += std::any_cast<delim_token>(c).m_value;
		else if (i == typeid(number_token)      ) out += print(std::any_cast<number_token>(c).m_value);
		else if (i == typeid(percentage_token))
		{
			out += print(std::any_cast<percentage_token>(c).m_value);
			out += U"%";
		}
		else if (i == typeid(dimension_token))
		{
			dimension_token const& d = std::any_cast<dimension_token>(c);
			out += print(d.m_value);
			out += d.m_unit;
		}
		else if (i == typeid(whitespace_token)  ) out += U" ";
		else if (i == typeid(CDO_token)         ) out += U"<!--";
		else if (i == typeid(CDC_token)         ) out += U"-->";
		else if (i == typeid(colon_token)       ) out += U":";
		else if (i == typeid(semicolon_token)   ) out += U";";
		else if (i == typeid(comma_token)       ) out += U",";
		else if (i == typeid(open_square_token) ) out += U"[";
		else if (i == typeid(close_square_token)) out += U"]";
		else if (i == typeid(open_paren_token)  ) out += U"(";
		else if (i == typeid(close_paren_token) ) out += U")";
		else if (i == typeid(open_curly_token)  ) out += U"{";
		else if (i == typeid(close_curly_token) ) out += U"}";
		else if (i == typeid(eof_token)) out += U"EOF_TOKEN";

		else if (i == typeid(function)      ) out += print(std::any_cast<function>(c));
		else if (i == typeid(simple_block)  ) out += print(std::any_cast<simple_block>(c));
		else if (i == typeid(at_rule)       ) out += print(std::any_cast<at_rule>(c));
		else if (i == typeid(qualified_rule)) out += print(std::any_cast<qualified_rule>(c));
		else if (i == typeid(declaration)   ) out += print(std::any_cast<declaration>(c));

		else
		{
			assert(false);
		}

		return out;
	}

	// トークン列/コンポーネント値列を文字列化する
	inline std::u32string print(std::vector<std::any> const& in)
	{
		using namespace wordring::wwwc::css;

		std::u32string out;

		for (auto c : in) out += print(c);

		return out;
	}
}

BOOST_AUTO_TEST_SUITE(css_syntax_parsing_test)

// ------------------------------------------------------------------------------------------------
// 5. Parsing
//
// https://drafts.csswg.org/css-syntax-3/#parsing
// https://triple-underscore.github.io/css-syntax-ja.html#parsing
// ------------------------------------------------------------------------------------------------

// inline bool is_open_square_block(std::any const& val)
BOOST_AUTO_TEST_CASE(parsing_is_open_square_block_1)
{
	using namespace wordring::wwwc::css;

	simple_block block{ open_square_token{} };
	BOOST_CHECK(is_open_square_block(std::make_any<simple_block>(block)));
}

BOOST_AUTO_TEST_CASE(parsing_is_open_square_block_2)
{
	using namespace wordring::wwwc::css;

	simple_block block{ open_paren_token{} };
	BOOST_CHECK(is_open_square_block(std::make_any<simple_block>(block)) == false);
}

BOOST_AUTO_TEST_CASE(parsing_is_open_square_block_3)
{
	using namespace wordring::wwwc::css;

	simple_block block;
	BOOST_CHECK(is_open_square_block(std::make_any<simple_block>(block)) == false);
}


// inline bool is_preserved_token(std::any const& val)
BOOST_AUTO_TEST_CASE(parsing_is_preserved_token_1)
{
	using namespace wordring::wwwc::css;

	BOOST_CHECK(is_preserved_token(std::make_any<whitespace_token>()));
}

BOOST_AUTO_TEST_CASE(parsing_is_preserved_token_2)
{
	using namespace wordring::wwwc::css;

	BOOST_CHECK(is_preserved_token(std::make_any<function_token>(U"")) == false);
}

// inline bool is_component_value(std::any const& val)
BOOST_AUTO_TEST_CASE(parsing_is_component_value_1)
{
	using namespace wordring::wwwc::css;

	BOOST_CHECK(is_component_value(std::make_any<function>()));
}

BOOST_AUTO_TEST_CASE(parsing_is_component_value_2)
{
	using namespace wordring::wwwc::css;

	BOOST_CHECK(is_component_value(std::make_any<open_curly_token>()) == false);
}

// ------------------------------------------------------------------------------------------------
// 5.2. Definitions
//
// https://drafts.csswg.org/css-syntax-3/#parser-definitions
// https://triple-underscore.github.io/css-syntax-ja.html#parser-definitions
// ------------------------------------------------------------------------------------------------

// token_stream::current_input_token
BOOST_AUTO_TEST_CASE(parsing_token_stream_current_input_token_1)
{
	using namespace wordring::wwwc::css;

	std::u32string css = UR"*(p{color: red;})*";
	std::vector<css_token> tokens;
	tokenize(css.begin(), css.end(), std::back_inserter(tokens));

	token_stream in(std::move(tokens));

	BOOST_CHECK(print(in.current_input_token()) == U"p");
}

// token_stream::current_input_token
// 終端に達している場合、eof_tokenを返す。
BOOST_AUTO_TEST_CASE(parsing_token_stream_current_input_token_2)
{
	using namespace wordring::wwwc::css;

	std::u32string css = UR"*()*";
	std::vector<css_token> tokens;
	tokenize(css.begin(), css.end(), std::back_inserter(tokens));

	token_stream in(std::move(tokens));

	BOOST_CHECK(print(in.current_input_token()) == U"EOF_TOKEN");
}

// token_stream::next_input_token
BOOST_AUTO_TEST_CASE(parsing_token_stream_next_input_token_1)
{
	using namespace wordring::wwwc::css;

	std::u32string css = UR"*(p{color: red;})*";
	std::vector<css_token> tokens;
	tokenize(css.begin(), css.end(), std::back_inserter(tokens));

	token_stream in(std::move(tokens));

	BOOST_CHECK(print(in.next_input_token()) == U"{");
}

// token_stream::next_input_token
// 終端に達している場合、eof_tokenを返す。
BOOST_AUTO_TEST_CASE(parsing_token_stream_next_input_token_2)
{
	using namespace wordring::wwwc::css;

	std::u32string css = UR"*(p)*";
	std::vector<css_token> tokens;
	tokenize(css.begin(), css.end(), std::back_inserter(tokens));

	token_stream in(std::move(tokens));

	BOOST_CHECK(print(in.next_input_token()) == U"EOF_TOKEN");
}

// token_stream::consume
BOOST_AUTO_TEST_CASE(parsing_token_stream_consume_1)
{
	using namespace wordring::wwwc::css;

	std::u32string css = UR"*(p{color: red;})*";
	std::vector<css_token> tokens;
	tokenize(css.begin(), css.end(), std::back_inserter(tokens));

	token_stream in(std::move(tokens));

	BOOST_CHECK(print(in.consume()) == U"p");
	BOOST_CHECK(print(in.consume()) == U"{");
	BOOST_CHECK(print(in.consume()) == U"color");
	BOOST_CHECK(print(in.consume()) == U":");
	BOOST_CHECK(print(in.consume()) == U" ");
	BOOST_CHECK(print(in.consume()) == U"red");
	BOOST_CHECK(print(in.consume()) == U";");
	BOOST_CHECK(print(in.consume()) == U"}");
	BOOST_CHECK(print(in.consume()) == U"EOF_TOKEN");
}

// token_stream::consume
// 終端に達している場合、eof_tokenを返す。
BOOST_AUTO_TEST_CASE(parsing_token_stream_consume_2)
{
	using namespace wordring::wwwc::css;

	std::u32string css = UR"*()*";
	std::vector<css_token> tokens;
	tokenize(css.begin(), css.end(), std::back_inserter(tokens));

	token_stream in(std::move(tokens));

	BOOST_CHECK(print(in.consume()) == U"EOF_TOKEN");
}

// token_stream::reconsume
BOOST_AUTO_TEST_CASE(parsing_token_stream_reconsume_1)
{
	using namespace wordring::wwwc::css;

	std::u32string css = UR"*(p{color: red;})*";
	std::vector<css_token> tokens;
	tokenize(css.begin(), css.end(), std::back_inserter(tokens));

	token_stream in(std::move(tokens));

	BOOST_CHECK(print(in.consume()) == U"p");
	in.reconsume();
	BOOST_CHECK(print(in.consume()) == U"p");
	BOOST_CHECK(print(in.consume()) == U"{");
}

// ------------------------------------------------------------------------------------------------
// 5.3. Parser Entry Points
//
// https://drafts.csswg.org/css-syntax-3/#parser-entry-points
// https://triple-underscore.github.io/css-syntax-ja.html#parser-entry-points
// ------------------------------------------------------------------------------------------------

// トークン列を正規化
BOOST_AUTO_TEST_CASE(parsing_normalize_into_token_stream_1)
{
	using namespace wordring::wwwc::css;

	std::u32string in = UR"*( p { color: red; } )*";
	std::vector<css_token> v, v1, v2;

	tokenize(in.begin(), in.end(), std::back_inserter(v));

	v1 = v;
	v2 = normalize_into_token_stream(std::move(v));

	BOOST_CHECK(print(v1) == print(v2));
}

// コード・ポイント列を正規化
BOOST_AUTO_TEST_CASE(parsing_normalize_into_token_stream_2)
{
	using namespace wordring::wwwc::css;

	std::u32string in = UR"*( p { color: red; } )*";

	std::vector<std::any> v1, v2;
	tokenize(in.begin(), in.end(), std::back_inserter(v1));

	v2 = normalize_into_token_stream(std::move(in));

	BOOST_CHECK(print(v1) == print(v2));
}

// ------------------------------------------------------------------------------------------------
// 5.3.1. Parse something according to a CSS grammar
//
// https://drafts.csswg.org/css-syntax-3/#parse-grammar
// https://triple-underscore.github.io/css-syntax-ja.html#parse-grammar
// ------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(parsing_parse_grammar_1)
{
	using namespace wordring::wwwc::css;

	std::u32string in = UR"*( p { color: red; } )*";
	/*
	auto v1 = parse_grammar(std::u32string(in), [](auto const& c) { return true; });
	auto v2 = normalize_into_token_stream(std::u32string(in));

	BOOST_CHECK(print(v1) == print(v2));
	*/
}

// --------------------------------------------------------------------------------------------
// 5.4.1. Consume a list of rules
//
// https://drafts.csswg.org/css-syntax-3/#consume-list-of-rules
// https://triple-underscore.github.io/css-syntax-ja.html#consume-list-of-rules
// --------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(parsing_consume_list_of_rules_1)
{
	using namespace wordring::wwwc::css;

	std::u32string css =
		U"@media print {\r\n"
		U"	body{ font - size: 10pt }\r\n"
		U"}\r\n"
		U"p > a {\r\n"
		U"	color: blue;\r\n"
		U"	text-decoration: underline;\r\n"
		U"}\r\n";
	token_stream in(normalize_into_token_stream(std::move(css), 0));

	std::vector<css_component> v = consume_list_of_rules(in, false, nullptr);
	std::u32string s = print(v);
	BOOST_CHECK(s == U"@media print { body{ font - size: 10pt } }p > a { color: blue; text-decoration: underline; }");
}

// ------------------------------------------------------------------------------------------------
// 5.4.2. Consume an at-rule
//
// https://drafts.csswg.org/css-syntax-3/#consume-at-rule
// https://triple-underscore.github.io/css-syntax-ja.html#consume-at-rule
// ------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(parsing_consume_at_rule_1)
{
	using namespace wordring::wwwc::css;

	std::u32string css = UR"*(@import "my-styles.css";)*";
	token_stream in(normalize_into_token_stream(std::move(css), 0));

	BOOST_CHECK(is_at_keyword_token(in.current_input_token()));
	at_rule rule = consume_at_rule(in, nullptr);
	std::u32string s = print(rule);
	BOOST_CHECK(print(rule.m_prelude.back()) == U"my-styles.css");
}

BOOST_AUTO_TEST_CASE(parsing_consume_at_rule_2)
{
	using namespace wordring::wwwc::css;

	std::u32string css =
		U"@page :left {\r\n"
		U"	margin-left: 4cm;\r\n"
		U"	margin-right: 3cm;\r\n"
		U"}";
	token_stream in(normalize_into_token_stream(std::move(css), 0));

	BOOST_CHECK(is_at_keyword_token(in.current_input_token()));
	at_rule rule = consume_at_rule(in, nullptr);
	std::u32string s = print(rule);
	BOOST_CHECK(s == U"@page :left { margin-left: 4cm; margin-right: 3cm; }");
}

BOOST_AUTO_TEST_CASE(parsing_consume_at_rule_3)
{
	using namespace wordring::wwwc::css;

	std::u32string css =
		U"@media print {\r\n"
		U"	body{ font - size: 10pt }\r\n"
		U"}\r\n";
	token_stream in(normalize_into_token_stream(std::move(css), 0));

	BOOST_CHECK(is_at_keyword_token(in.current_input_token()));
	at_rule rule = consume_at_rule(in, nullptr);
	std::u32string s = print(rule);
	BOOST_CHECK(s == U"@media print { body{ font - size: 10pt } }");
}

// ------------------------------------------------------------------------------------------------
// 5.4.3. Consume a qualified rule
//
// https://drafts.csswg.org/css-syntax-3/#consume-qualified-rule
// 
// ------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(parsing_consume_qualified_rule_1)
{
	using namespace wordring::wwwc::css;

	std::u32string css =
		U"p > a {\r\n"
		U"	color: blue;\r\n"
		U"	text-decoration: underline;"
		U"}\r\n";
	token_stream in(normalize_into_token_stream(std::move(css), 0));

	std::optional<qualified_rule> rule = consume_qualified_rule(in, nullptr);
	std::u32string s = print(rule);
	BOOST_CHECK(s == U"p > a { color: blue; text-decoration: underline;}");
}

// ------------------------------------------------------------------------------------------------
// 5.4.4. Consume a list of declarations
//
// https://drafts.csswg.org/css-syntax-3/#consume-list-of-declarations
// 
// ------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(parsing_consume_list_of_declarations_1)
{
	using namespace wordring::wwwc::css;

	std::u32string css =
		U"	color: blue;\r\n"
		U"	text-decoration: underline !important;";
	token_stream in(normalize_into_token_stream(std::move(css), 0));

	std::vector<css_component> decl = consume_list_of_declarations(in, nullptr);
	std::u32string s = print(decl);
	BOOST_CHECK(s == U"color:bluetext-decoration:underline !important");
}

// ------------------------------------------------------------------------------------------------
// 5.4.5. Consume a declaration
//
// https://drafts.csswg.org/css-syntax-3/#consume-declaration
// 
// ------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(parsing_consume_declaration_1)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"color: blue;";
	token_stream in(normalize_into_token_stream(std::move(css), 0));

	std::optional<declaration> decl = consume_declaration(in, nullptr);
	auto s = print(decl);

	BOOST_CHECK(s == U"color:blue;");
}

// ------------------------------------------------------------------------------------------------
// 5.4.6. Consume a component value
//
// https://drafts.csswg.org/css-syntax-3/#consume-component-value
// 
// ------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(parsing_consume_component_value_1)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"[ABC]";
	token_stream in(normalize_into_token_stream(std::move(css), 0));

	in.consume();
	simple_block block = consume_simple_block(in, nullptr);
	auto s = print(block);

	BOOST_CHECK(s == U"[ABC]");
}

BOOST_AUTO_TEST_CASE(parsing_consume_component_value_2)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"opacity(50%)";
	token_stream in(normalize_into_token_stream(std::move(css), 0));

	in.consume();
	function fn = consume_function(in, nullptr);
	auto s = print(fn);

	BOOST_CHECK(s == U"opacity(50%)");
}

// ------------------------------------------------------------------------------------------------
// 5.4.7. Consume a simple block
//
// https://drafts.csswg.org/css-syntax-3/#consume-simple-block
// 
// ------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(parsing_consume_simple_block_1)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"[ABC]";
	token_stream in(normalize_into_token_stream(std::move(css), 0));

	in.consume();
	simple_block block = consume_simple_block(in, nullptr);
	auto s = print(block);

	BOOST_CHECK(s == U"[ABC]");
}

// ------------------------------------------------------------------------------------------------
// 5.4.8. Consume a function
//
// https://drafts.csswg.org/css-syntax-3/#consume-function
// 
// ------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(parsing_consume_function_1)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"opacity(50%)";
	token_stream in(normalize_into_token_stream(std::move(css), 0));

	in.consume();
	function fn = consume_function(in, nullptr);
	auto s = print(fn);

	BOOST_CHECK(s == U"opacity(50%)");
}

BOOST_AUTO_TEST_SUITE_END()
