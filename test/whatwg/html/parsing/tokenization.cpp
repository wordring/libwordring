// test/whatwg/html/parsing/tokenization.cpp

#include <boost/test/unit_test.hpp>

#include <wordring/whatwg/html/parsing/tokenization.hpp>

#include <string>

namespace
{
	struct test_tokenizer : wordring::whatwg::html::parsing::tokenizer<test_tokenizer>
	{
		using base_type = wordring::whatwg::html::parsing::tokenizer<test_tokenizer>;

		using base_type::m_c;
		using base_type::m_state;

		using base_type::eof;

		wordring::whatwg::html::parsing::error m_ec;
		//std::u32string m_emits;

		void on_report_error(wordring::whatwg::html::parsing::error ec) { m_ec = ec; }

		template <typename Token>
		void on_emit_token(Token const& token) {}
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

/*! 12.2.5.1 Data state */
BOOST_AUTO_TEST_CASE(tokenizer__on_data_state__1)
{
	test_tokenizer tt;
	tt.push_code_point(U'A');
	BOOST_CHECK(tt.m_c.empty());
}

/*! 12.2.5.2 RCDATA state */
BOOST_AUTO_TEST_CASE(tokenizer__on_RCDATA_state__1)
{
	test_tokenizer tt;

	tt.m_state = test_tokenizer::RCDATA_state;
	tt.push_code_point(U'A');

	BOOST_CHECK(tt.m_c.empty());
}

/*! 12.2.5.3 RAWTEXT state */
BOOST_AUTO_TEST_CASE(tokenizer__on_RAWTEXT_state__1)
{
	test_tokenizer tt;

	tt.m_state = test_tokenizer::RAWTEXT_state;
	tt.push_code_point(U'A');

	BOOST_CHECK(tt.m_c.empty());
}

/*! 12.2.5.4 Script data state */
BOOST_AUTO_TEST_CASE(tokenizer__on_script_data_state__1)
{
	test_tokenizer tt;

	tt.m_state = test_tokenizer::script_data_state;
	tt.push_code_point(U'A');

	BOOST_CHECK(tt.m_c.empty());
}

/*! 12.2.5.5 PLAINTEXT state */
BOOST_AUTO_TEST_CASE(tokenizer__on_PLAINTEXT_state__1)
{
	test_tokenizer tt;

	tt.m_state = test_tokenizer::PLAINTEXT_state;
	tt.push_code_point(U'A');
	tt.eof(true);

	BOOST_CHECK(tt.m_c.empty());
}





BOOST_AUTO_TEST_SUITE_END()
