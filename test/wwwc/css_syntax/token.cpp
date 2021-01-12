// test/wwwc/css_syntax/token.cpp

#include <boost/test/unit_test.hpp>

#include <wordring/wwwc/css_syntax/token.hpp>

#include <vector>

BOOST_AUTO_TEST_SUITE(css_syntax_token_test)

// ------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(token_ident_token_1)
{
	using namespace wordring::wwwc::css;

	ident_token tkn{ U"TEST" };
	BOOST_CHECK(tkn.m_value == U"TEST");

	function fn1{};

	function fn2(fn1);

	component_value cv1{};
	component_value cv2(cv1);
}

// ------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(token_syntax_primitive_stream_iterator_1)
{
	using namespace wordring::wwwc::css;

	std::vector<syntax_primitive>::const_iterator it1, it2;
	BOOST_CHECK(it1 == it2);
}

BOOST_AUTO_TEST_CASE(token_syntax_primitive_stream_copy_construct_1)
{
	using namespace wordring::wwwc::css;

	std::vector<syntax_primitive> v(5, ident_token{});
	syntax_primitive_stream s1(v);
	BOOST_CHECK(s1.size() == 5);
	syntax_primitive_stream s2(s1);
	BOOST_CHECK(s2.size() == 5);
}

BOOST_AUTO_TEST_CASE(token_syntax_primitive_stream_operator_bool_1)
{
	using namespace wordring::wwwc::css;

	std::vector<syntax_primitive> v(5, ident_token{});
	syntax_primitive_stream s(v);
	BOOST_CHECK(s);
}

BOOST_AUTO_TEST_CASE(token_syntax_primitive_stream_operator_bool_2)
{
	using namespace wordring::wwwc::css;

	std::vector<syntax_primitive> v;
	syntax_primitive_stream s(v);
	BOOST_CHECK(s == false);
}

BOOST_AUTO_TEST_CASE(token_syntax_primitive_stream_operator_bool_3)
{
	using namespace wordring::wwwc::css;

	std::vector<syntax_primitive> v(5, ident_token{});
	syntax_primitive_stream s(v);
	for (int i = 0; i < 5; ++i) s.consume();
	BOOST_CHECK(s == false);
}

BOOST_AUTO_TEST_CASE(token_syntax_primitive_stream_operator_not_1)
{
	using namespace wordring::wwwc::css;

	std::vector<syntax_primitive> v;
	syntax_primitive_stream s(v);
	BOOST_CHECK(!s);
}

BOOST_AUTO_TEST_CASE(token_syntax_primitive_stream_operator_not_2)
{
	using namespace wordring::wwwc::css;

	std::vector<syntax_primitive> v(5, ident_token{});
	syntax_primitive_stream s(v);
	BOOST_CHECK((!s) == false);
}

BOOST_AUTO_TEST_CASE(token_syntax_primitive_stream_operator_not_3)
{
	using namespace wordring::wwwc::css;

	std::vector<syntax_primitive> v(5, ident_token{});
	syntax_primitive_stream s(v);
	for (int i = 0; i < 5; ++i) s.consume();
	BOOST_CHECK(!s);
}

BOOST_AUTO_TEST_CASE(token_syntax_primitive_stream_size_1)
{
	using namespace wordring::wwwc::css;

	std::vector<syntax_primitive> v(30, ident_token{});
	syntax_primitive_stream s(v);

	BOOST_CHECK(s.size() == 30);
}

BOOST_AUTO_TEST_CASE(token_syntax_primitive_stream_current_1)
{
	using namespace wordring::wwwc::css;

	std::u32string sz = U"0123456789";
	std::vector<syntax_primitive> v;
	for (std::size_t i = 0; i < 10; ++i) v.push_back(delim_token{ sz[i] });
	syntax_primitive_stream s(v);

	BOOST_CHECK(s.current().get<delim_token>().m_value == U'0');
	s.consume();
	BOOST_CHECK(s.current().get<delim_token>().m_value == U'1');
}

BOOST_AUTO_TEST_CASE(token_syntax_primitive_stream_consume_1)
{
	using namespace wordring::wwwc::css;

	std::u32string sz = U"0123456789";
	std::vector<syntax_primitive> v;
	for (std::size_t i = 0; i < 10; ++i) v.push_back(delim_token{ sz[i] });
	syntax_primitive_stream s(v);

	for (int i = 0; i < 9; ++i) s.consume();
	BOOST_CHECK(s.current().get<delim_token>().m_value == U'9');
}

BOOST_AUTO_TEST_CASE(token_syntax_primitive_stream_consume_2)
{
	using namespace wordring::wwwc::css;

	std::u32string sz = U"0123456789";
	std::vector<syntax_primitive> v;
	v.push_back(delim_token{ U'A' });
	syntax_primitive_stream s(v);

	BOOST_CHECK(s.consume().type() == syntax_primitive_name::DelimToken);
	BOOST_CHECK(s.current().type() == syntax_primitive_name::EofToken);
}

BOOST_AUTO_TEST_CASE(token_syntax_primitive_stream_reconsume_1)
{
	using namespace wordring::wwwc::css;

	std::u32string sz = U"0123456789";
	std::vector<syntax_primitive> v;
	for (std::size_t i = 0; i < 10; ++i) v.push_back(delim_token{ sz[i] });
	syntax_primitive_stream s(v);

	for (int i = 0; i < 5; ++i) s.consume();
	s.reconsume();
	BOOST_CHECK(s.current().get<delim_token>().m_value == U'4');
}

BOOST_AUTO_TEST_CASE(token_syntax_primitive_stream_skip_whitespace_1)
{
	using namespace wordring::wwwc::css;

	std::vector<syntax_primitive> v;
	v.push_back(whitespace_token{});
	v.push_back(ident_token{ U"A" });
	syntax_primitive_stream s(v);
	s.skip_whitespace();
	BOOST_CHECK(s.current().type() == syntax_primitive_name::IdentToken);
}

BOOST_AUTO_TEST_CASE(token_syntax_primitive_stream_skip_whitespace_2)
{
	using namespace wordring::wwwc::css;

	std::vector<syntax_primitive> v;
	v.push_back(ident_token{ U"A" });
	v.push_back(whitespace_token{});
	syntax_primitive_stream s(v);
	s.skip_whitespace();
	BOOST_CHECK(s.current().type() == syntax_primitive_name::IdentToken);
}

// ------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
