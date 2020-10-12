// test/wwwc/css_syntax/tokenization.cpp

#include <boost/test/unit_test.hpp>

#include <wordring/wwwc/css_syntax/tokenization.hpp>

#include <cmath>
#include <iterator>
#include <vector>

BOOST_AUTO_TEST_SUITE(css_syntax_tokenization_test)

// ------------------------------------------------------------------------------------------------
// 4.3.1. Consume a token
//
// https://drafts.csswg.org/css-syntax-3/#consume-token
// https://triple-underscore.github.io/css-syntax-ja.html#consume-token
// ------------------------------------------------------------------------------------------------

// トークン化
BOOST_AUTO_TEST_CASE(tokenization_tokenize_1)
{
	using namespace wordring::wwwc::css;

	std::u32string in = UR"*( p { color: red; } )*";

	std::vector<css_token> ret;
	tokenize(in.begin(), in.end(), std::back_inserter(ret));
}

// コメントを除去出来るか検査
BOOST_AUTO_TEST_CASE(tokenization_consume_token_1)
{
	using namespace wordring::wwwc::css;

	std::u32string in = UR"*(/*ABC*/   )*";
	auto [it, token] = consume_token(in.begin(), in.end());

	BOOST_CHECK(it == in.end());
	BOOST_CHECK(std::holds_alternative<whitespace_token>(token));
}

// 空白文字
BOOST_AUTO_TEST_CASE(tokenization_consume_token_2)
{
	using namespace wordring::wwwc::css;

	std::u32string in = UR"*(   )*";
	auto [it, token] = consume_token(in.begin(), in.end(), 1);

	BOOST_CHECK(it == in.end());
	BOOST_CHECK(std::holds_alternative<whitespace_token>(token));
}

// 「"」に囲まれた文字列
BOOST_AUTO_TEST_CASE(tokenization_consume_token_3)
{
	using namespace wordring::wwwc::css;

	std::u32string in = UR"*("ABC")*";
	auto [it, token] = consume_token(in.begin(), in.end(), 1);

	BOOST_CHECK(it == in.end());
	BOOST_CHECK(std::holds_alternative<string_token>(token));
	auto tkn = std::get<string_token>(token);
	BOOST_CHECK(tkn.m_value == U"ABC");
}

// #ID
BOOST_AUTO_TEST_CASE(tokenization_consume_token_4)
{
	using namespace wordring::wwwc::css;

	std::u32string in = UR"*(#ID)*";
	auto [it, token] = consume_token(in.begin(), in.end(), 1);

	BOOST_CHECK(it == in.end());
	BOOST_CHECK(std::holds_alternative<hash_token>(token));
	auto tkn = std::get<hash_token>(token);
	BOOST_CHECK(tkn.m_type_flag == hash_token::type_flag_name::id);
	BOOST_CHECK(tkn.m_value == U"ID");
}

BOOST_AUTO_TEST_CASE(tokenization_consume_token_5)
{
	using namespace wordring::wwwc::css;

	std::u32string in = UR"*(#+)*";
	auto [it, token] = consume_token(in.begin(), in.end(), 1);

	BOOST_CHECK(++it == in.end());
	BOOST_CHECK(std::holds_alternative<delim_token>(token));
	auto tkn = std::get<delim_token>(token);
	BOOST_CHECK(tkn.m_value == U'#');
}

// 「'」に囲まれた文字列
BOOST_AUTO_TEST_CASE(tokenization_consume_token_6)
{
	using namespace wordring::wwwc::css;

	std::u32string in = UR"*('ABC')*";
	auto [it, token] = consume_token(in.begin(), in.end(), 1);

	BOOST_CHECK(it == in.end());
	BOOST_CHECK(std::holds_alternative<string_token>(token));
	auto tkn = std::get<string_token>(token);
	BOOST_CHECK(tkn.m_value == U"ABC");
}

// (
BOOST_AUTO_TEST_CASE(tokenization_consume_token_7)
{
	using namespace wordring::wwwc::css;

	std::u32string in = UR"*(()*";
	auto [it, token] = consume_token(in.begin(), in.end(), 1);

	BOOST_CHECK(it == in.end());
	BOOST_CHECK(std::holds_alternative<open_paren_token>(token));
}

// )
BOOST_AUTO_TEST_CASE(tokenization_consume_token_8)
{
	using namespace wordring::wwwc::css;

	std::u32string in = UR"*())*";
	auto [it, token] = consume_token(in.begin(), in.end(), 1);

	BOOST_CHECK(it == in.end());
	BOOST_CHECK(std::holds_alternative<close_paren_token>(token));
}

// +
BOOST_AUTO_TEST_CASE(tokenization_consume_token_9)
{
	using namespace wordring::wwwc::css;

	std::u32string in = UR"*(+10)*";
	auto [it, token] = consume_token(in.begin(), in.end(), 1);

	BOOST_CHECK(it == in.end());
	BOOST_CHECK(std::holds_alternative<number_token>(token));
	auto tkn = std::get<number_token>(token);
	BOOST_CHECK(std::abs(tkn.m_value - 10) < std::numeric_limits<double>::epsilon());
	BOOST_CHECK(tkn.m_type_flag == number_type_flag_name::integer);
}

BOOST_AUTO_TEST_CASE(tokenization_consume_token_10)
{
	using namespace wordring::wwwc::css;

	std::u32string in = UR"*(++)*";
	auto [it, token] = consume_token(in.begin(), in.end(), 1);

	BOOST_CHECK(++it == in.end());
	BOOST_CHECK(std::holds_alternative<delim_token>(token));
	auto tkn = std::get<delim_token>(token);
	BOOST_CHECK(tkn.m_value == U'+');
}

// ,
BOOST_AUTO_TEST_CASE(tokenization_consume_token_11)
{
	using namespace wordring::wwwc::css;

	std::u32string in = UR"*(,)*";
	auto [it, token] = consume_token(in.begin(), in.end(), 1);

	BOOST_CHECK(it == in.end());
	BOOST_CHECK(std::holds_alternative<comma_token>(token));
}

// -
BOOST_AUTO_TEST_CASE(tokenization_consume_token_12)
{
	using namespace wordring::wwwc::css;

	std::u32string in = UR"*(-10)*";
	auto [it, token] = consume_token(in.begin(), in.end(), 1);

	BOOST_CHECK(it == in.end());
	BOOST_CHECK(std::holds_alternative<number_token>(token));
	auto tkn = std::get<number_token>(token);
	BOOST_CHECK(std::abs(tkn.m_value + 10) < std::numeric_limits<double>::epsilon());
	BOOST_CHECK(tkn.m_type_flag == number_type_flag_name::integer);
}

BOOST_AUTO_TEST_CASE(tokenization_consume_token_13)
{
	using namespace wordring::wwwc::css;

	std::u32string in = UR"*(-->)*";
	auto [it, token] = consume_token(in.begin(), in.end(), 1);

	BOOST_CHECK(it == in.end());
	BOOST_CHECK(std::holds_alternative<CDC_token>(token));
}

BOOST_AUTO_TEST_CASE(tokenization_consume_token_14)
{
	using namespace wordring::wwwc::css;

	std::u32string in = UR"*(-A)*";
	auto [it, token] = consume_token(in.begin(), in.end(), 1);

	BOOST_CHECK(it == in.end());
	BOOST_CHECK(std::holds_alternative<ident_token>(token));
	auto tkn = std::get<ident_token>(token);
	BOOST_CHECK(tkn.m_value == U"-A");
}

BOOST_AUTO_TEST_CASE(tokenization_consume_token_15)
{
	using namespace wordring::wwwc::css;

	std::u32string in = UR"*(-+)*";
	auto [it, token] = consume_token(in.begin(), in.end(), 1);

	BOOST_CHECK(++it == in.end());
	BOOST_CHECK(std::holds_alternative<delim_token>(token));
	auto tkn = std::get<delim_token>(token);
	BOOST_CHECK(tkn.m_value == U'-');
}

// .
BOOST_AUTO_TEST_CASE(tokenization_consume_token_16)
{
	using namespace wordring::wwwc::css;

	std::u32string in = UR"*(.1)*";
	auto [it, token] = consume_token(in.begin(), in.end(), 1);

	BOOST_CHECK(it == in.end());
	BOOST_CHECK(std::holds_alternative<number_token>(token));
	auto tkn = std::get<number_token>(token);
	BOOST_CHECK(std::abs(tkn.m_value - 0.1) < std::numeric_limits<double>::epsilon());
	BOOST_CHECK(tkn.m_type_flag == number_type_flag_name::number);
}

BOOST_AUTO_TEST_CASE(tokenization_consume_token_17)
{
	using namespace wordring::wwwc::css;

	std::u32string in = UR"*(.+)*";
	auto [it, token] = consume_token(in.begin(), in.end(), 1);

	BOOST_CHECK(++it == in.end());
	BOOST_CHECK(std::holds_alternative<delim_token>(token));
	auto tkn = std::get<delim_token>(token);
	BOOST_CHECK(tkn.m_value == U'.');
}

// :
BOOST_AUTO_TEST_CASE(tokenization_consume_token_18)
{
	using namespace wordring::wwwc::css;

	std::u32string in = UR"*(:)*";
	auto [it, token] = consume_token(in.begin(), in.end(), 1);

	BOOST_CHECK(it == in.end());
	BOOST_CHECK(std::holds_alternative<colon_token>(token));
}

// ;
BOOST_AUTO_TEST_CASE(tokenization_consume_token_19)
{
	using namespace wordring::wwwc::css;

	std::u32string in = UR"*(;)*";
	auto [it, token] = consume_token(in.begin(), in.end(), 1);

	BOOST_CHECK(it == in.end());
	BOOST_CHECK(std::holds_alternative<semicolon_token>(token));
}

// <
BOOST_AUTO_TEST_CASE(tokenization_consume_token_20)
{
	using namespace wordring::wwwc::css;

	std::u32string in = UR"*(<!--)*";
	auto [it, token] = consume_token(in.begin(), in.end(), 1);

	BOOST_CHECK(it == in.end());
	BOOST_CHECK(std::holds_alternative<CDO_token>(token));
}

BOOST_AUTO_TEST_CASE(tokenization_consume_token_21)
{
	using namespace wordring::wwwc::css;

	std::u32string in = UR"*(<)*";
	auto [it, token] = consume_token(in.begin(), in.end(), 1);

	BOOST_CHECK(it == in.end());
	BOOST_CHECK(std::holds_alternative<delim_token>(token));
	auto tkn = std::get<delim_token>(token);
	BOOST_CHECK(tkn.m_value == U'<');
}

// @
BOOST_AUTO_TEST_CASE(tokenization_consume_token_22)
{
	using namespace wordring::wwwc::css;

	std::u32string in = UR"*(@ABC)*";
	auto [it, token] = consume_token(in.begin(), in.end(), 1);

	BOOST_CHECK(it == in.end());
	BOOST_CHECK(std::holds_alternative<at_keyword_token>(token));
	auto tkn = std::get<at_keyword_token>(token);
	BOOST_CHECK(tkn.m_value == U"ABC");
}

BOOST_AUTO_TEST_CASE(tokenization_consume_token_23)
{
	using namespace wordring::wwwc::css;

	std::u32string in = UR"*(@+)*";
	auto [it, token] = consume_token(in.begin(), in.end(), 1);

	BOOST_CHECK(++it == in.end());
	BOOST_CHECK(std::holds_alternative<delim_token>(token));
	auto tkn = std::get<delim_token>(token);
	BOOST_CHECK(tkn.m_value == U'@');
}

// [
BOOST_AUTO_TEST_CASE(tokenization_consume_token_24)
{
	using namespace wordring::wwwc::css;

	std::u32string in = UR"*([)*";
	auto [it, token] = consume_token(in.begin(), in.end(), 1);

	BOOST_CHECK(it == in.end());
	BOOST_CHECK(std::holds_alternative<open_square_token>(token));
}

// REVERSE SOLIDUS
BOOST_AUTO_TEST_CASE(tokenization_consume_token_25)
{
	using namespace wordring::wwwc::css;

	std::u32string in = UR"*(\1234)*";
	auto [it, token] = consume_token(in.begin(), in.end(), 1);

	BOOST_CHECK(it == in.end());
	BOOST_CHECK(std::holds_alternative<ident_token>(token));
	auto tkn = std::get<ident_token>(token);
	BOOST_CHECK(tkn.m_value == U"\x1234");
}

BOOST_AUTO_TEST_CASE(tokenization_consume_token_26)
{
	using namespace wordring::wwwc::css;

	std::u32string in = U"\\\n";
	auto [it, token] = consume_token(in.begin(), in.end(), 1);

	BOOST_CHECK(++it == in.end());
	BOOST_CHECK(std::holds_alternative<delim_token>(token));
	auto tkn = std::get<delim_token>(token);
	BOOST_CHECK(tkn.m_value == U'\\');
}

// ]
BOOST_AUTO_TEST_CASE(tokenization_consume_token_27)
{
	using namespace wordring::wwwc::css;

	std::u32string in = UR"*(])*";
	auto [it, token] = consume_token(in.begin(), in.end(), 1);

	BOOST_CHECK(it == in.end());
	BOOST_CHECK(std::holds_alternative<close_square_token>(token));
}

// {
BOOST_AUTO_TEST_CASE(tokenization_consume_token_28)
{
	using namespace wordring::wwwc::css;

	std::u32string in = UR"*({)*";
	auto [it, token] = consume_token(in.begin(), in.end(), 1);

	BOOST_CHECK(it == in.end());
	BOOST_CHECK(std::holds_alternative<open_curly_token>(token));
}

// }
BOOST_AUTO_TEST_CASE(tokenization_consume_token_29)
{
	using namespace wordring::wwwc::css;

	std::u32string in = UR"*(})*";
	auto [it, token] = consume_token(in.begin(), in.end(), 1);

	BOOST_CHECK(it == in.end());
	BOOST_CHECK(std::holds_alternative<close_curly_token>(token));
}

// 数字
BOOST_AUTO_TEST_CASE(tokenization_consume_token_30)
{
	using namespace wordring::wwwc::css;

	std::u32string in = UR"*(30)*";
	auto [it, token] = consume_token(in.begin(), in.end(), 1);

	BOOST_CHECK(it == in.end());
	BOOST_CHECK(std::holds_alternative<number_token>(token));
	auto tkn = std::get<number_token>(token);
	BOOST_CHECK(std::abs(tkn.m_value - 30) < std::numeric_limits<double>::epsilon());
	BOOST_CHECK(tkn.m_type_flag == number_type_flag_name::integer);
}

// 識別子
BOOST_AUTO_TEST_CASE(tokenization_consume_token_31)
{
	using namespace wordring::wwwc::css;

	std::u32string in = UR"*(ABC)*";
	auto [it, token] = consume_token(in.begin(), in.end(), 1);

	BOOST_CHECK(it == in.end());
	BOOST_CHECK(std::holds_alternative<ident_token>(token));
	auto tkn = std::get<ident_token>(token);
	BOOST_CHECK(tkn.m_value == U"ABC");
}

// EOF
BOOST_AUTO_TEST_CASE(tokenization_consume_token_32)
{
	using namespace wordring::wwwc::css;

	std::u32string in = UR"*()*";
	auto [it, token] = consume_token(in.begin(), in.end(), 1);

	BOOST_CHECK(it == in.end());
	BOOST_CHECK(std::holds_alternative<eof_token>(token));
}

// etc.
BOOST_AUTO_TEST_CASE(tokenization_consume_token_33)
{
	using namespace wordring::wwwc::css;

	std::u32string in = UR"*(=)*";
	auto [it, token] = consume_token(in.begin(), in.end(), 1);

	BOOST_CHECK(it == in.end());
	BOOST_CHECK(std::holds_alternative<delim_token>(token));
	auto tkn = std::get<delim_token>(token);
	BOOST_CHECK(tkn.m_value == U'=');
}

// ------------------------------------------------------------------------------------------------
// 4.3.2. Consume comments
//
// https://drafts.csswg.org/css-syntax-3/#consume-comment
// https://triple-underscore.github.io/css-syntax-ja.html#consume-comments
// ------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(tokenization_consume_comments_1)
{
	using namespace wordring::wwwc::css;

	std::u32string in = UR"*(/*あいうえお*/)*";
	auto [it, token] = consume_comments(in.begin(), in.end(), 1);

	BOOST_CHECK(it == in.end());
}

BOOST_AUTO_TEST_CASE(tokenization_consume_comments_2)
{
	using namespace wordring::wwwc::css;

	std::u32string in = UR"*(/**/)*";
	auto [it, token] = consume_comments(in.begin(), in.end(), 1);

	BOOST_CHECK(it == in.end());
}

BOOST_AUTO_TEST_CASE(tokenization_consume_comments_3)
{
	using namespace wordring::wwwc::css;

	bool e = false;
	std::u32string in = UR"*(/*)*";
	auto [it, token] = consume_comments(in.begin(), in.end(), [&e](auto it) { e = true; });

	BOOST_CHECK(it == in.end());
	BOOST_CHECK(e == true);
}

BOOST_AUTO_TEST_CASE(tokenization_consume_comments_4)
{
	using namespace wordring::wwwc::css;

	std::u32string in = UR"*(/*あいうえお*/かきくけこ)*";
	auto [it, token] = consume_comments(in.begin(), in.end(), 1);

	BOOST_CHECK(std::u32string(it, in.end()) == U"かきくけこ");
}

// ------------------------------------------------------------------------------------------------
// 4.3.3. Consume a numeric token
//
// https://drafts.csswg.org/css-syntax-3/#consume-numeric-token
// https://triple-underscore.github.io/css-syntax-ja.html#consume-numeric-token
// ------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(tokenization_consume_numeric_token_1)
{
	using namespace wordring::wwwc::css;

	std::u32string in = UR"*(123.45)*";
	auto [it, token] = consume_numeric_token(in.begin(), in.end(), 1);

	BOOST_CHECK(it == in.end());
	auto tkn = std::get<number_token>(token);
	BOOST_CHECK(tkn.m_type_flag == number_type_flag_name::number);
	BOOST_CHECK(tkn.m_value == 123.45);
}

BOOST_AUTO_TEST_CASE(tokenization_consume_numeric_token_2)
{
	using namespace wordring::wwwc::css;

	std::u32string in = UR"*(33.3%)*";
	auto [it, token] = consume_numeric_token(in.begin(), in.end(), 1);

	BOOST_CHECK(it == in.end());
	auto tkn = std::get<percentage_token>(token);
	BOOST_CHECK(tkn.m_value == 33.3);
}

BOOST_AUTO_TEST_CASE(tokenization_consume_numeric_token_3)
{
	using namespace wordring::wwwc::css;

	std::u32string in = UR"*(1.5em)*";
	auto [it, token] = consume_numeric_token(in.begin(), in.end(), 1);

	BOOST_CHECK(it == in.end());
	auto tkn = std::get<dimension_token>(token);
	BOOST_CHECK(tkn.m_value == 1.5);
	BOOST_CHECK(tkn.m_unit == U"em");
}

// ------------------------------------------------------------------------------------------------
// 4.3.4. Consume an ident-like token
//
// https://drafts.csswg.org/css-syntax-3/#consume-ident-like-token
// https://triple-underscore.github.io/css-syntax-ja.html#consume-ident-like-token
// ------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(tokenization_consume_ident_like_token_1)
{
	using namespace wordring::wwwc::css;

	std::u32string in = UR"*(auto)*";
	auto [it, token] = consume_ident_like_token(in.begin(), in.end(), 1);

	BOOST_CHECK(it == in.end());
	auto tkn = std::get<ident_token>(token);
	BOOST_CHECK(tkn.m_value == U"auto");
}

BOOST_AUTO_TEST_CASE(tokenization_consume_ident_like_token_2)
{
	using namespace wordring::wwwc::css;

	std::u32string in = UR"*(calc()*";
	auto [it, token] = consume_ident_like_token(in.begin(), in.end(), 1);

	BOOST_CHECK(it == in.end());
	auto tkn = std::get<function_token>(token);
	BOOST_CHECK(tkn.m_value == U"calc");
}

BOOST_AUTO_TEST_CASE(tokenization_consume_ident_like_token_3)
{
	using namespace wordring::wwwc::css;

	std::u32string in = UR"*(url(example.png))*";
	auto [it, token] = consume_ident_like_token(in.begin(), in.end(), 1);

	BOOST_CHECK(it == in.end());
	auto tkn = std::get<url_token>(token);
	BOOST_CHECK(tkn.m_value == U"example.png");
}

BOOST_AUTO_TEST_CASE(tokenization_consume_ident_like_token_4)
{
	using namespace wordring::wwwc::css;

	std::u32string in = UR"*(url(+ +)*";
	auto [it, token] = consume_ident_like_token(in.begin(), in.end(), 1);

	BOOST_CHECK(it == in.end());
	BOOST_CHECK_NO_THROW([[maybe_unused]] auto r = std::get<bad_url_token>(token));
}

// ------------------------------------------------------------------------------------------------
// 4.3.5. Consume a string token
//
// https://drafts.csswg.org/css-syntax-3/#consume-string-token
// https://triple-underscore.github.io/css-syntax-ja.html#consume-string-token
// ------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(tokenization_consume_string_token_1)
{
	using namespace wordring::wwwc::css;

	std::u32string in = UR"*(ABC")*";
	auto [it, token] = consume_string_token(in.begin(), in.end(), U'"', 1);

	BOOST_CHECK(it == in.end());
	auto tkn = std::get<string_token>(token);
	BOOST_CHECK(tkn.m_value == U"ABC");
}

BOOST_AUTO_TEST_CASE(tokenization_consume_string_token_2)
{
	using namespace wordring::wwwc::css;

	std::u32string in = UR"*(ABC)*";
	auto [it, token] = consume_string_token(in.begin(), in.end(), U'"', 1);

	BOOST_CHECK(it == in.end());
	auto tkn = std::get<string_token>(token);
	BOOST_CHECK(tkn.m_value == U"ABC");
}

BOOST_AUTO_TEST_CASE(tokenization_consume_string_token_3)
{
	using namespace wordring::wwwc::css;

	std::u32string in = U"ABC\n";
	auto [it, token] = consume_string_token(in.begin(), in.end(), U'"', 1);

	BOOST_CHECK(++it == in.end());
	BOOST_CHECK_NO_THROW([[maybe_unused]] auto r = std::get<bad_string_token>(token));
}

BOOST_AUTO_TEST_CASE(tokenization_consume_string_token_4)
{
	using namespace wordring::wwwc::css;

	std::u32string in = UR"*(\41)*";
	auto [it, token] = consume_string_token(in.begin(), in.end(), U'"', 1);

	BOOST_CHECK(it == in.end());
	auto tkn = std::get<string_token>(token);
	BOOST_CHECK(tkn.m_value == U"A");
}

// ------------------------------------------------------------------------------------------------
// 4.3.6. Consume a url token
//
// https://drafts.csswg.org/css-syntax-3/#consume-url-token
// https://triple-underscore.github.io/css-syntax-ja.html#consume-url-token
// ------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(tokenization_consume_url_token_1)
{
	using namespace wordring::wwwc::css;

	std::u32string in = UR"*(example.png))*";
	auto [it, token] = consume_url_token(in.begin(), in.end(), 1);

	BOOST_CHECK(it == in.end());
	auto tkn = std::get<url_token>(token);
	BOOST_CHECK(tkn.m_value == U"example.png");
}

BOOST_AUTO_TEST_CASE(tokenization_consume_url_token_2)
{
	using namespace wordring::wwwc::css;

	std::u32string in = UR"*(example.png)*";
	auto [it, token] = consume_url_token(in.begin(), in.end(), 1);

	BOOST_CHECK(it == in.end());
	auto tkn = std::get<url_token>(token);
	BOOST_CHECK(tkn.m_value == U"example.png");
}

BOOST_AUTO_TEST_CASE(tokenization_consume_url_token_3)
{
	using namespace wordring::wwwc::css;

	std::u32string in = UR"*(example.png")*";
	auto [it, token] = consume_url_token(in.begin(), in.end(), 1);

	BOOST_CHECK(it == in.end());
	BOOST_CHECK_NO_THROW([[maybe_unused]] auto r = std::get<bad_url_token>(token));
}

// ------------------------------------------------------------------------------------------------
// 4.3.7. Consume an escaped code point
//
// https://drafts.csswg.org/css-syntax-3/#consume-escaped-code-point
// https://triple-underscore.github.io/css-syntax-ja.html#consume-escaped-code-point
// ------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(tokenization_consume_escaped_code_point_1)
{
	using namespace wordring::wwwc::css;

	std::u32string in = UR"*(12345)*";
	auto [it, cp] = consume_escaped_code_point(in.begin(), in.end(), 1);

	BOOST_CHECK(it == in.end());
	BOOST_CHECK(cp == U'\U00012345');
}

BOOST_AUTO_TEST_CASE(tokenization_consume_escaped_code_point_2)
{
	using namespace wordring::wwwc::css;

	std::u32string in = UR"*(G)*";
	auto [it, cp] = consume_escaped_code_point(in.begin(), in.end(), 1);

	BOOST_CHECK(it == in.end());
	BOOST_CHECK(cp == U'G');
}

BOOST_AUTO_TEST_CASE(tokenization_consume_escaped_code_point_3)
{
	using namespace wordring::wwwc::css;

	std::u32string in = UR"*(0)*";
	auto [it, cp] = consume_escaped_code_point(in.begin(), in.end(), 1);

	BOOST_CHECK(it == in.end());
	BOOST_CHECK(cp == U'\xFFFD');
}

// ------------------------------------------------------------------------------------------------
// 4.3.8. Check if two code points are a valid escape
//
// https://drafts.csswg.org/css-syntax-3/#starts-with-a-valid-escape
// https://triple-underscore.github.io/css-syntax-ja.html#starts-with-a-valid-escape
// ------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(tokenization_starts_with_valid_escape_1)
{
	using namespace wordring::wwwc::css;

	std::u32string in = UR"*(\A)*";
	bool f = starts_with_valid_escape(in.begin(), in.end());

	BOOST_CHECK(f);
}

BOOST_AUTO_TEST_CASE(tokenization_starts_with_valid_escape_2)
{
	using namespace wordring::wwwc::css;

	std::u32string in = U"\\\xA";
	bool f = starts_with_valid_escape(in.begin(), in.end());

	BOOST_CHECK(!f);
}

// ------------------------------------------------------------------------------------------------
// 4.3.9. Check if three code points would start an identifier
//
// https://drafts.csswg.org/css-syntax-3/#would-start-an-identifier
// https://triple-underscore.github.io/css-syntax-ja.html#would-start-an-identifier
// ------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(tokenization_starts_with_identifier_1)
{
	using namespace wordring::wwwc::css;

	std::u32string in = UR"*(-A)*";
	bool f = starts_with_identifier(in.begin(), in.end());

	BOOST_CHECK(f);
}

BOOST_AUTO_TEST_CASE(tokenization_starts_with_identifier_2)
{
	using namespace wordring::wwwc::css;

	std::u32string in = UR"*(--)*";
	bool f = starts_with_identifier(in.begin(), in.end());

	BOOST_CHECK(f);
}

BOOST_AUTO_TEST_CASE(tokenization_starts_with_identifier_3)
{
	using namespace wordring::wwwc::css;

	std::u32string in = UR"*(-\A)*";
	bool f = starts_with_identifier(in.begin(), in.end());

	BOOST_CHECK(f);
}

BOOST_AUTO_TEST_CASE(tokenization_starts_with_identifier_4)
{
	using namespace wordring::wwwc::css;

	std::u32string in = UR"*(A)*";
	bool f = starts_with_identifier(in.begin(), in.end());

	BOOST_CHECK(f);
}

BOOST_AUTO_TEST_CASE(tokenization_starts_with_identifier_5)
{
	using namespace wordring::wwwc::css;

	std::u32string in = UR"*(\A)*";
	bool f = starts_with_identifier(in.begin(), in.end());

	BOOST_CHECK(f);
}

BOOST_AUTO_TEST_CASE(tokenization_starts_with_identifier_6)
{
	using namespace wordring::wwwc::css;

	std::u32string in = UR"*(0)*";
	bool f = starts_with_identifier(in.begin(), in.end());

	BOOST_CHECK(!f);
}

// ------------------------------------------------------------------------------------------------
// 4.3.10. Check if three code points would start a number
//
// https://drafts.csswg.org/css-syntax-3/#starts-with-a-number
// https://triple-underscore.github.io/css-syntax-ja.html#starts-with-a-number
// ------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(tokenization_starts_with_number_1)
{
	using namespace wordring::wwwc::css;

	std::u32string in = UR"*(+0)*";
	bool f = starts_with_number(in.begin(), in.end());

	BOOST_CHECK(f);
}

BOOST_AUTO_TEST_CASE(tokenization_starts_with_number_2)
{
	using namespace wordring::wwwc::css;

	std::u32string in = UR"*(-0)*";
	bool f = starts_with_number(in.begin(), in.end());

	BOOST_CHECK(f);
}

BOOST_AUTO_TEST_CASE(tokenization_starts_with_number_3)
{
	using namespace wordring::wwwc::css;

	std::u32string in = UR"*(+.0)*";
	bool f = starts_with_number(in.begin(), in.end());

	BOOST_CHECK(f);
}

BOOST_AUTO_TEST_CASE(tokenization_starts_with_number_4)
{
	using namespace wordring::wwwc::css;

	std::u32string in = UR"*(.0)*";
	bool f = starts_with_number(in.begin(), in.end());

	BOOST_CHECK(f);
}

BOOST_AUTO_TEST_CASE(tokenization_starts_with_number_5)
{
	using namespace wordring::wwwc::css;

	std::u32string in = UR"*(0)*";
	bool f = starts_with_number(in.begin(), in.end());

	BOOST_CHECK(f);
}

BOOST_AUTO_TEST_CASE(tokenization_starts_with_number_6)
{
	using namespace wordring::wwwc::css;

	std::u32string in = UR"*(A)*";
	bool f = starts_with_number(in.begin(), in.end());

	BOOST_CHECK(!f);
}

// ------------------------------------------------------------------------------------------------
// 4.3.11. Consume an identifier
//
// https://drafts.csswg.org/css-syntax-3/#consume-name
// https://triple-underscore.github.io/css-syntax-ja.html#consume-name
// ------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(tokenization_consume_identifier_1)
{
	using namespace wordring::wwwc::css;

	std::u32string in = UR"*(A\41)*";
	auto [it, s] = consume_identifier(in.begin(), in.end(), 1);

	BOOST_CHECK(it == in.end());
	BOOST_CHECK(s == U"AA");
}

BOOST_AUTO_TEST_CASE(tokenization_consume_identifier_2)
{
	using namespace wordring::wwwc::css;

	std::u32string in = UR"*(A\41+)*";
	auto [it, s] = consume_identifier(in.begin(), in.end(), 1);

	BOOST_CHECK(++it == in.end());
	BOOST_CHECK(s == U"AA");
}

// ------------------------------------------------------------------------------------------------
// 4.3.12. Consume a number
//
// https://drafts.csswg.org/css-syntax-3/#consume-number
// https://triple-underscore.github.io/css-syntax-ja.html#consume-number
// ------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(tokenization_consume_number_1)
{
	using namespace wordring::wwwc::css;

	std::u32string in = UR"*(+1)*";
	auto [it, num, flg] = consume_number(in.begin(), in.end(), 1);

	BOOST_CHECK(it == in.end());
	BOOST_CHECK(std::abs(num - 1) < std::numeric_limits<double>::epsilon());
	BOOST_CHECK(flg == number_type_flag_name::integer);
}

BOOST_AUTO_TEST_CASE(tokenization_consume_number_2)
{
	using namespace wordring::wwwc::css;

	std::u32string in = UR"*(-2)*";
	auto [it, num, flg] = consume_number(in.begin(), in.end(), 1);

	BOOST_CHECK(it == in.end());
	BOOST_CHECK(std::abs(num + 2) < std::numeric_limits<double>::epsilon());
	BOOST_CHECK(flg == number_type_flag_name::integer);
}

BOOST_AUTO_TEST_CASE(tokenization_consume_number_3)
{
	using namespace wordring::wwwc::css;

	std::u32string in = UR"*(+.3)*";
	auto [it, num, flg] = consume_number(in.begin(), in.end(), 1);

	BOOST_CHECK(it == in.end());
	BOOST_CHECK(std::abs(num - 0.3) < std::numeric_limits<double>::epsilon());
	BOOST_CHECK(flg == number_type_flag_name::number);
}

BOOST_AUTO_TEST_CASE(tokenization_consume_number_4)
{
	using namespace wordring::wwwc::css;

	std::u32string in = UR"*(-.4)*";
	auto [it, num, flg] = consume_number(in.begin(), in.end(), 1);

	BOOST_CHECK(it == in.end());
	BOOST_CHECK(std::abs(num + 0.4) < std::numeric_limits<double>::epsilon());
	BOOST_CHECK(flg == number_type_flag_name::number);
}

BOOST_AUTO_TEST_CASE(tokenization_consume_number_5)
{
	using namespace wordring::wwwc::css;

	std::u32string in = UR"*(.5)*";
	auto [it, num, flg] = consume_number(in.begin(), in.end(), 1);

	BOOST_CHECK(it == in.end());
	BOOST_CHECK(std::abs(num - 0.5) < std::numeric_limits<double>::epsilon());
	BOOST_CHECK(flg == number_type_flag_name::number);
}

BOOST_AUTO_TEST_CASE(tokenization_consume_number_6)
{
	using namespace wordring::wwwc::css;

	std::u32string in = UR"*(6)*";
	auto [it, num, flg] = consume_number(in.begin(), in.end(), 1);

	BOOST_CHECK(it == in.end());
	BOOST_CHECK(std::abs(num - 6) < std::numeric_limits<double>::epsilon());
	BOOST_CHECK(flg == number_type_flag_name::integer);
}

// ------------------------------------------------------------------------------------------------
// 4.3.13. Convert a string to a number
//
// https://drafts.csswg.org/css-syntax-3/#convert-string-to-number
// https://triple-underscore.github.io/css-syntax-ja.html#convert-string-to-number
//
// https://developer.mozilla.org/ja/docs/Web/CSS/number
// ------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(tokenization_convert_string_to_number_1)
{
	using namespace wordring::wwwc::css;

	std::u32string in = UR"*(10)*";
	double d = convert_string_to_number(in.begin(), in.end());

	BOOST_CHECK(std::abs(d - 10.0) < std::numeric_limits<double>::epsilon());
}

BOOST_AUTO_TEST_CASE(tokenization_convert_string_to_number_2)
{
	using namespace wordring::wwwc::css;

	std::u32string in = UR"*(4.01)*";
	double d = convert_string_to_number(in.begin(), in.end());

	BOOST_CHECK(std::abs(d - 4.01) < std::numeric_limits<double>::epsilon());
}

BOOST_AUTO_TEST_CASE(tokenization_convert_string_to_number_3)
{
	using namespace wordring::wwwc::css;

	std::u32string in = UR"*(-456.8)*";
	double d = convert_string_to_number(in.begin(), in.end());

	BOOST_CHECK(std::abs(d + 456.8) < std::numeric_limits<double>::epsilon());
}

BOOST_AUTO_TEST_CASE(tokenization_convert_string_to_number_4)
{
	using namespace wordring::wwwc::css;

	std::u32string in = UR"*(0.0)*";
	double d = convert_string_to_number(in.begin(), in.end());

	BOOST_CHECK(std::abs(d - 0.0) < std::numeric_limits<double>::epsilon());
}

BOOST_AUTO_TEST_CASE(tokenization_convert_string_to_number_5)
{
	using namespace wordring::wwwc::css;

	std::u32string in = UR"*(+0.0)*";
	double d = convert_string_to_number(in.begin(), in.end());

	BOOST_CHECK(d == 0);
}

BOOST_AUTO_TEST_CASE(tokenization_convert_string_to_number_6)
{
	using namespace wordring::wwwc::css;

	std::u32string in = UR"*(-0.0)*";
	double d = convert_string_to_number(in.begin(), in.end());

	BOOST_CHECK(std::abs(d + 0.0) < std::numeric_limits<double>::epsilon());
}

BOOST_AUTO_TEST_CASE(tokenization_convert_string_to_number_7)
{
	using namespace wordring::wwwc::css;

	std::u32string in = UR"*(.60)*";
	double d = convert_string_to_number(in.begin(), in.end());

	BOOST_CHECK(std::abs(d - 0.6) < std::numeric_limits<double>::epsilon());
}

BOOST_AUTO_TEST_CASE(tokenization_convert_string_to_number_8)
{
	using namespace wordring::wwwc::css;

	std::u32string in = UR"*(10e3)*";
	double d = convert_string_to_number(in.begin(), in.end());

	BOOST_CHECK(std::abs(d - 10e3) < std::numeric_limits<double>::epsilon());
}

BOOST_AUTO_TEST_CASE(tokenization_convert_string_to_number_9)
{
	using namespace wordring::wwwc::css;

	std::u32string in = UR"*(-3.4e-2)*";
	double d = convert_string_to_number(in.begin(), in.end());

	BOOST_CHECK(std::abs(d + 3.4e-2) < std::numeric_limits<double>::epsilon());
}

// ------------------------------------------------------------------------------------------------
// 4.3.14. Consume the remnants of a bad url
//
// https://drafts.csswg.org/css-syntax-3/#consume-remnants-of-bad-url
// https://triple-underscore.github.io/css-syntax-ja.html#consume-remnants-of-bad-url
// ------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(tokenization_consume_remnants_of_bad_url_1)
{
	using namespace wordring::wwwc::css;

	std::u32string in = UR"*(ABC))*";
	auto [it, token] = consume_remnants_of_bad_url(in.begin(), in.end(), 1);

	BOOST_CHECK(it == in.end());
	BOOST_CHECK(std::holds_alternative<std::monostate>(token));
}

BOOST_AUTO_TEST_CASE(tokenization_consume_remnants_of_bad_url_2)
{
	using namespace wordring::wwwc::css;

	std::u32string in = UR"*(ABC\)))*";
	auto [it, token] = consume_remnants_of_bad_url(in.begin(), in.end(), 1);

	BOOST_CHECK(it == in.end());
	BOOST_CHECK(std::holds_alternative<std::monostate>(token));
}

BOOST_AUTO_TEST_SUITE_END()
