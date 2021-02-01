// test/wwwc/css_syntax/parsing.cpp

#include <boost/test/unit_test.hpp>

#include <wordring/wwwc/css_syntax/parsing.hpp>

#include <wordring/wwwc/selectors/grammar.hpp>

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

	inline std::u32string print(wordring::wwwc::css::syntax_primitive const& c);
	inline std::u32string print(wordring::wwwc::css::component_value const& c);
	inline std::u32string print(std::vector<wordring::wwwc::css::syntax_primitive> const& in);
	inline std::u32string print(std::vector<wordring::wwwc::css::component_value> const& in);

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
		out += in.m_associated_token;
		out += print(in.m_value);
		switch (in.m_associated_token)
		{
		case U'{':
			out += U"}";
			break;
		case U'[':
			out += U"]";
			break;
		case U'(':
			out += U")";
			break;
		default:
			assert(false);
		}
		return out;
	}

	inline std::u32string print(wordring::wwwc::css::at_rule const& in)
	{
		using namespace wordring::wwwc::css;

		std::u32string out;

		out += U"@";
		out += in.m_name;
		out += print(in.m_prelude);
		if (in.m_block)
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
		if (in.m_important_flag) out += U"!IMPORTANT";

		return out;
	}

	inline std::u32string print(std::optional<wordring::wwwc::css::declaration> const& in)
	{
		return in.has_value() ? print(*in) : U"";
	}
/*
	inline std::u32string print(std::vector<wordring::wwwc::css::declaration> const& in)
	{
		std::u32string out;
		for (auto const& c : in) out += print(c);

		return out;
	}
*/
	// トークン/コンポーネント値を文字列化する
	inline std::u32string print(wordring::wwwc::css::syntax_primitive const& c)
	{
		using namespace wordring::wwwc::css;

		switch (c.type())
		{
		case syntax_primitive_name::IdentToken:       return c.get<ident_token>().m_value;
		case syntax_primitive_name::FunctionToken:    return c.get<function_token>().m_value;
		case syntax_primitive_name::AtKeywordToken:   return c.get<at_keyword_token>().m_value;
		case syntax_primitive_name::HashToken:        return c.get<hash_token>().m_value;
		case syntax_primitive_name::StringToken:      return c.get<string_token>().m_value;
		case syntax_primitive_name::BadStringToken:   return U" BAD_STRING_TOKEN ";
		case syntax_primitive_name::UrlToken:         return c.get<url_token>().m_value;
		case syntax_primitive_name::BadUrlToken:      return U" BAD_URL_TOKEN ";
		case syntax_primitive_name::DelimToken:       return std::u32string(1, c.get<delim_token>().m_value);
		case syntax_primitive_name::NumberToken:      return print(c.get<number_token>().m_value);
		case syntax_primitive_name::PercentageToken:  return print(c.get<percentage_token>().m_value) + U"%";
		case syntax_primitive_name::DimensionToken:
		{
			dimension_token const& d = c.get<dimension_token>();
			return print(d.m_value) + d.m_unit;
		}
		case syntax_primitive_name::WhitespaceToken:  return U" ";
		case syntax_primitive_name::CdoToken:         return U"<!--";
		case syntax_primitive_name::CdcToken:         return U"-->";
		case syntax_primitive_name::ColonToken:       return U":";
		case syntax_primitive_name::SemicolonToken:   return U";";
		case syntax_primitive_name::CommaToken:       return U",";
		case syntax_primitive_name::OpenSquareToken:  return U"[";
		case syntax_primitive_name::CloseSquareToken: return U"]";
		case syntax_primitive_name::OpenParenToken:   return U"(";
		case syntax_primitive_name::CloseParenToken:  return U")";
		case syntax_primitive_name::OpenCurlyToken:   return U"{";
		case syntax_primitive_name::CloseCurlyToken:  return U"}";
		case syntax_primitive_name::EofToken:         return U"EOF_TOKEN";

		case syntax_primitive_name::PreservedTokens: return U"PRESERVED_TOKENS";
		case syntax_primitive_name::Function:        return print(c.get<function>());
		case syntax_primitive_name::SimpleBlock:     return print(c.get<simple_block>());
		case syntax_primitive_name::ComponentValue:  return print(c.get<component_value>());
		case syntax_primitive_name::AtRule:          return print(c.get<at_rule>());
		case syntax_primitive_name::QualifiedRule:   return print(c.get<qualified_rule>());
		case syntax_primitive_name::Declaration:     return print(c.get<declaration>());

		default:
			break;
		}

		assert(false);
		return U"";
	}

	inline std::u32string print(wordring::wwwc::css::component_value const& c)
	{
		using namespace wordring::wwwc::css;

		switch (c.type())
		{
		case syntax_primitive_name::IdentToken:       return c.get<ident_token>().m_value;
		//case syntax_primitive_name::FunctionToken:    return c.get<function_token>().m_value;
		case syntax_primitive_name::AtKeywordToken:   return c.get<at_keyword_token>().m_value;
		case syntax_primitive_name::HashToken:        return c.get<hash_token>().m_value;
		case syntax_primitive_name::StringToken:      return c.get<string_token>().m_value;
		case syntax_primitive_name::BadStringToken:   return U" BAD_STRING_TOKEN ";
		case syntax_primitive_name::UrlToken:         return c.get<url_token>().m_value;
		case syntax_primitive_name::BadUrlToken:      return U" BAD_URL_TOKEN ";
		case syntax_primitive_name::DelimToken:       return std::u32string(1, c.get<delim_token>().m_value);
		case syntax_primitive_name::NumberToken:      return print(c.get<number_token>().m_value);
		case syntax_primitive_name::PercentageToken:  return print(c.get<percentage_token>().m_value) + U"%";
		case syntax_primitive_name::DimensionToken:
		{
			dimension_token const& d = c.get<dimension_token>();
			return print(d.m_value) + d.m_unit;
		}
		case syntax_primitive_name::WhitespaceToken:  return U" ";
		case syntax_primitive_name::CdoToken:         return U"<!--";
		case syntax_primitive_name::CdcToken:         return U"-->";
		case syntax_primitive_name::ColonToken:       return U":";
		case syntax_primitive_name::SemicolonToken:   return U";";
		case syntax_primitive_name::CommaToken:       return U",";
		case syntax_primitive_name::CloseSquareToken: return U"]";
		case syntax_primitive_name::CloseParenToken:  return U")";
		case syntax_primitive_name::CloseCurlyToken:  return U"}";
		case syntax_primitive_name::EofToken:         return U"EOF_TOKEN";

		case syntax_primitive_name::Function:        return print(c.get<function>());
		case syntax_primitive_name::SimpleBlock:     return print(c.get<simple_block>());

		default:
			break;
		}

		assert(false);
		return U"";
	}

	// トークン列/コンポーネント値列を文字列化する
	inline std::u32string print(std::vector<wordring::wwwc::css::syntax_primitive> const& in)
	{
		using namespace wordring::wwwc::css;

		std::u32string out;

		for (auto c : in) out += print(c);

		return out;
	}
	inline std::u32string print(std::vector<wordring::wwwc::css::component_value> const& in)
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
	std::vector<syntax_primitive> tokens;
	tokenize(css.begin(), css.end(), std::back_inserter(tokens), 0);

	token_stream in(std::move(tokens));
	in.consume();
	BOOST_CHECK(print(in.current_input_token()) == U"p");
}

// token_stream::current_input_token
// 終端に達している場合、eof_tokenを返す。
BOOST_AUTO_TEST_CASE(parsing_token_stream_current_input_token_2)
{
	using namespace wordring::wwwc::css;

	std::u32string css = UR"*()*";
	std::vector<syntax_primitive> tokens;
	tokenize(css.begin(), css.end(), std::back_inserter(tokens), 0);

	token_stream in(std::move(tokens));
	in.consume();
	BOOST_CHECK(print(in.current_input_token()) == U"EOF_TOKEN");
}

// token_stream::next_input_token
BOOST_AUTO_TEST_CASE(parsing_token_stream_next_input_token_1)
{
	using namespace wordring::wwwc::css;

	std::u32string css = UR"*(p{color: red;})*";
	std::vector<syntax_primitive> tokens;
	tokenize(css.begin(), css.end(), std::back_inserter(tokens), 0);

	token_stream in(std::move(tokens));
	in.consume();
	BOOST_CHECK(print(in.next_input_token()) == U"{");
}

// token_stream::next_input_token
// 終端に達している場合、eof_tokenを返す。
BOOST_AUTO_TEST_CASE(parsing_token_stream_next_input_token_2)
{
	using namespace wordring::wwwc::css;

	std::u32string css = UR"*(p)*";
	std::vector<syntax_primitive> tokens;
	tokenize(css.begin(), css.end(), std::back_inserter(tokens), 0);

	token_stream in(std::move(tokens));
	in.consume();
	BOOST_CHECK(print(in.next_input_token()) == U"EOF_TOKEN");
}

// token_stream::consume
BOOST_AUTO_TEST_CASE(parsing_token_stream_consume_1)
{
	using namespace wordring::wwwc::css;

	std::u32string css = UR"*(p{color: red;})*";
	std::vector<syntax_primitive> tokens;
	tokenize(css.begin(), css.end(), std::back_inserter(tokens), 0);

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
	std::vector<syntax_primitive> tokens;
	tokenize(css.begin(), css.end(), std::back_inserter(tokens), 0);

	token_stream in(std::move(tokens));

	BOOST_CHECK(print(in.consume()) == U"EOF_TOKEN");
}

// token_stream::reconsume
BOOST_AUTO_TEST_CASE(parsing_token_stream_reconsume_1)
{
	using namespace wordring::wwwc::css;

	std::u32string css = UR"*(p{color: red;})*";
	std::vector<syntax_primitive> tokens;
	tokenize(css.begin(), css.end(), std::back_inserter(tokens), 0);

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
	std::vector<syntax_primitive> v, v1, v2;

	tokenize(in.begin(), in.end(), std::back_inserter(v), 0);

	v1 = v;
	v2 = normalize_into_token_stream(std::move(v));

	BOOST_CHECK(print(v1) == print(v2));
}

// コード・ポイント列を正規化
BOOST_AUTO_TEST_CASE(parsing_normalize_into_token_stream_2)
{
	using namespace wordring::wwwc::css;

	std::u32string css = UR"*( p { color: red; } )*";

	std::vector<syntax_primitive> v1, v2;
	tokenize(css.begin(), css.end(), std::back_inserter(v1), 0);

	v2 = normalize_into_token_stream(std::move(css));

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

	parse_context pc;
	auto m = parse_grammar<complex_selector>(U"div span", pc);
	BOOST_CHECK(m);
}

// ------------------------------------------------------------------------------------------------
// 5.3.2. Parse A Comma-Separated List According To A CSS Grammar
//
// https://drafts.csswg.org/css-syntax-3/#parse-comma-list
// https://triple-underscore.github.io/css-syntax-ja.html#parse-comma-list
// ------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(parsing_parse_comma_list_1)
{
	using namespace wordring::wwwc::css;

	parse_context pc;
	auto v = parse_comma_list<complex_selector>(U"a, p h1", pc);
	BOOST_CHECK(v.size() == 2);
}

// ------------------------------------------------------------------------------------------------
// 5.3.3. Parse a stylesheet
//
// https://drafts.csswg.org/css-syntax-3/#parse-stylesheet
// https://triple-underscore.github.io/css-syntax-ja.html#parse-stylesheet
// ------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(parsing_parse_stylesheet_1)
{
	using namespace wordring::wwwc::css;

	std::string css =
		"@import \"my-styles.css\";"
		"p > a { color: blue; }";
	std::vector<syntax_primitive> v = parse_stylesheet(css);

	std::u32string s = print(v);
	BOOST_CHECK(s == U"@import my-styles.cssp > a { color: blue; }");
}

// ------------------------------------------------------------------------------------------------
// 5.3.4. Parse a list of rules
//
// https://drafts.csswg.org/css-syntax-3/#parse-list-of-rules
// https://triple-underscore.github.io/css-syntax-ja.html#parse-list-of-rules
// ------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(parsing_parse_list_of_rules_1)
{
	using namespace wordring::wwwc::css;

	std::u32string css =
		U"@import \"my-styles.css\";"
		U"p > a { color: blue; }";
	std::vector<syntax_primitive> v = parse_list_of_rules(std::move(css));

	std::u32string s = print(v);
	BOOST_CHECK(s == U"@import my-styles.cssp > a { color: blue; }");
}

// ------------------------------------------------------------------------------------------------
// 5.3.5. Parse a rule
//
// https://drafts.csswg.org/css-syntax-3/#parse-rule
// https://triple-underscore.github.io/css-syntax-ja.html#parse-rule
// ------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(parsing_parse_rule_1)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"@import \"my-styles.css\" ; ";
	std::optional<syntax_primitive> c = parse_rule(std::move(css));

	BOOST_CHECK(c);
	std::u32string s = print(*c);
	BOOST_CHECK(s == U"@import my-styles.css ");
}

// ------------------------------------------------------------------------------------------------
// 5.3.6. Parse a declaration
//
// https://drafts.csswg.org/css-syntax-3/#parse-declaration
// https://triple-underscore.github.io/css-syntax-ja.html#parse-declaration
// ------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(parsing_parse_declaration_1)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"background-color: red;";
	std::optional<declaration> c = parse_declaration(std::move(css));

	BOOST_CHECK(c);
	std::u32string s = print(*c);
	BOOST_CHECK(s == U"background-color:red;");
}

// 失敗させてみる
BOOST_AUTO_TEST_CASE(parsing_parse_declaration_2)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"a, p h1";
	std::optional<declaration> c = parse_declaration(std::move(css));

	BOOST_CHECK(!c);
}

// ------------------------------------------------------------------------------------------------
// 5.3.7. Parse a list of declarations
//
// https://drafts.csswg.org/css-syntax-3/#parse-list-of-declarations
// https://triple-underscore.github.io/css-syntax-ja.html#parse-list-of-declarations
// ------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(parsing_parse_list_of_declarations_1)
{
	using namespace wordring::wwwc::css;

	std::u32string css =
		U"background-color: red;"
		U"color: blue !important;";
	std::vector<syntax_primitive> v = parse_list_of_declarations(std::move(css));

	std::u32string s = print(v);
	BOOST_CHECK(s == U"background-color:redcolor:blue!IMPORTANT");
}

// ------------------------------------------------------------------------------------------------
// 5.3.8. Parse a component value
//
// https://drafts.csswg.org/css-syntax-3/#parse-component-value
// https://triple-underscore.github.io/css-syntax-ja.html#parse-component-value
// ------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(parsing_parse_component_value_1)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"a";
	std::optional<component_value> c = parse_component_value(std::move(css));

	BOOST_CHECK(c);
	std::u32string s = print(*c);
	BOOST_CHECK(s == U"a");
}

// ------------------------------------------------------------------------------------------------
// 5.3.9. Parse a list of component values
//
// https://drafts.csswg.org/css-syntax-3/#parse-list-of-component-values
// https://triple-underscore.github.io/css-syntax-ja.html#parse-list-of-component-values
// ------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(parsing_parse_list_of_component_values_1)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"a, p h1";
	std::vector<syntax_primitive> v = parse_list_of_component_values(std::move(css));

	std::u32string s = print(v);
	BOOST_CHECK(s == U"a, p h1");
}

// ------------------------------------------------------------------------------------------------
// 5.3.10. Parse a comma-separated list of component values
//
// https://drafts.csswg.org/css-syntax-3/#parse-comma-separated-list-of-component-values
// 
// ------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(parsing_parse_comma_separated_list_of_component_values_1)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"a, p h1";
	std::vector<std::vector<syntax_primitive>> v = parse_comma_separated_list_of_component_values(std::move(css));

	std::u32string s;
	for (auto const& x : v) s += print(x);

	BOOST_CHECK(s == U"a p h1");
}

// ------------------------------------------------------------------------------------------------
// 5.4.1. Consume a list of rules
//
// https://drafts.csswg.org/css-syntax-3/#consume-list-of-rules
// https://triple-underscore.github.io/css-syntax-ja.html#consume-list-of-rules
// ------------------------------------------------------------------------------------------------

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

	std::vector<syntax_primitive> v = consume_list_of_rules(in, false, nullptr);
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

	//BOOST_CHECK(is_at_keyword_token(in.current_input_token()));
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

	//BOOST_CHECK(is_at_keyword_token(in.current_input_token()));
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

	//BOOST_CHECK(is_at_keyword_token(in.current_input_token()));
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

	std::vector<syntax_primitive> decl = consume_list_of_declarations(in, nullptr);
	std::u32string s = print(decl);
	BOOST_CHECK(s == U"color:bluetext-decoration:underline!IMPORTANT");
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
