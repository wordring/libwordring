// test/whatwg/html/parsing/input_stream.cpp

#include <boost/test/unit_test.hpp>

#include <wordring/whatwg/html/parsing/input_stream.hpp>

#include <string>

namespace
{
	struct test_stream : wordring::whatwg::html::parsing::input_stream<test_stream>
	{
		using base_type = wordring::whatwg::html::parsing::input_stream<test_stream>;

		using base_type::m_c;

		wordring::whatwg::html::parsing::error m_ec;
		std::u32string m_emits;

		void on_report_error(wordring::whatwg::html::parsing::error ec) { m_ec = ec; }

		void on_emit_code_point() { if(!eof()) m_emits.push_back(m_c.back()); }
	};
}

BOOST_AUTO_TEST_SUITE(whatwg__html__parsing__input_stream__test)

/*
空の入力ストリームを構築する

input_stream()
*/
BOOST_AUTO_TEST_CASE(input_stream__construct__1)
{
	test_stream ts;
}

/*
エラー報告する

void report_error(error ec)
*/
BOOST_AUTO_TEST_CASE(input_stream__report_error__1)
{
	using namespace wordring::whatwg::html::parsing;

	test_stream ts;
	ts.report_error(error::cdata_in_html_content);

	BOOST_CHECK(ts.m_ec == error::cdata_in_html_content);
}

/*
文字を発送する

void emit_code_point(value_type cp)
*/
BOOST_AUTO_TEST_CASE(input_stream__emit_code_point__1)
{
	using namespace wordring::whatwg::html::parsing;

	test_stream ts;
	ts.emit_code_point(U'A');

	BOOST_CHECK(ts.m_emits == U"A");
}

/*
コード・ポイントを末尾に追加する

void push_code_point(value_type cp)
*/
BOOST_AUTO_TEST_CASE(input_stream__push_code_point__1)
{
	test_stream ts;
	ts.push_code_point(U'A');

	BOOST_CHECK(ts.next_input_character() == U'A');
}

/*
エラー報告をテスト
サロゲート文字
*/
BOOST_AUTO_TEST_CASE(input_stream__push_code_point__2)
{
	using namespace wordring::whatwg::html::parsing;

	test_stream ts;
	ts.push_code_point(U'\xD800');

	BOOST_CHECK(ts.m_ec == error::surrogate_in_input_stream);
	BOOST_CHECK(ts.next_input_character() == U'\xD800');
}

/*
エラー報告をテスト
非文字
*/
BOOST_AUTO_TEST_CASE(input_stream__push_code_point__3)
{
	using namespace wordring::whatwg::html::parsing;

	test_stream ts;
	ts.push_code_point(U'\xFFFE');

	BOOST_CHECK(ts.m_ec == error::noncharacter_in_input_stream);
	BOOST_CHECK(ts.next_input_character() == U'\xFFFE');
}

/*
エラー報告をテスト
コントロール文字
*/
BOOST_AUTO_TEST_CASE(input_stream__push_code_point__4)
{
	using namespace wordring::whatwg::html::parsing;

	test_stream ts;
	ts.push_code_point(U'\x7F');

	BOOST_CHECK(ts.m_ec == error::control_character_in_input_stream);
	BOOST_CHECK(ts.next_input_character() == U'\x7F');
}

BOOST_AUTO_TEST_CASE(input_stream__push_code_point__5)
{
	using namespace wordring::whatwg::html::parsing;

	test_stream ts;

	ts.m_ec = error::abrupt_closing_of_empty_comment; // エラー無しの判定用ダミー
	ts.push_code_point(U'\x9');

	BOOST_CHECK(ts.m_ec == error::abrupt_closing_of_empty_comment);
	BOOST_CHECK(ts.next_input_character() == U'\x9');
}

BOOST_AUTO_TEST_CASE(input_stream__push_code_point__6)
{
	using namespace wordring::whatwg::html::parsing;

	test_stream ts;
	ts.m_ec = error::abrupt_closing_of_empty_comment; // エラー無しの判定用ダミー
	ts.push_code_point(U'\x0');

	BOOST_CHECK(ts.m_ec == error::abrupt_closing_of_empty_comment);
	BOOST_CHECK(ts.next_input_character() == U'\x0');
}

/*
改行文字正規化をテスト
CR
*/
BOOST_AUTO_TEST_CASE(input_stream__push_code_point__7)
{
	test_stream ts;
	ts.push_code_point(U'\r');
	ts.push_code_point(U'A');

	BOOST_CHECK(ts.next_input_character() == U'\n');
	ts.consume();
	BOOST_CHECK(ts.next_input_character() == U'A');
}

/*
LF
*/
BOOST_AUTO_TEST_CASE(input_stream__push_code_point__8)
{
	test_stream ts;
	ts.push_code_point(U'\n');
	ts.push_code_point(U'A');

	BOOST_CHECK(ts.next_input_character() == U'\n');
	ts.consume();
	BOOST_CHECK(ts.next_input_character() == U'A');
}

/*
CRLF
*/
BOOST_AUTO_TEST_CASE(input_stream__push_code_point__9)
{
	test_stream ts;
	ts.push_code_point(U'\r');
	ts.push_code_point(U'\n');
	ts.push_code_point(U'A');

	BOOST_CHECK(ts.next_input_character() == U'\n');
	ts.consume();
	BOOST_CHECK(ts.next_input_character() == U'A');
}

/*
CRCR
*/
BOOST_AUTO_TEST_CASE(input_stream__push_code_point__10)
{
	test_stream ts;
	ts.push_code_point(U'\r');
	ts.push_code_point(U'\r');
	ts.push_code_point(U'A');

	BOOST_CHECK(ts.next_input_character() == U'\n');
	ts.consume();
	BOOST_CHECK(ts.next_input_character() == U'\n');
	ts.consume();
	BOOST_CHECK(ts.next_input_character() == U'A');
}

/*
LFCR
*/
BOOST_AUTO_TEST_CASE(input_stream__push_code_point__11)
{
	test_stream ts;
	ts.push_code_point(U'\n');
	ts.push_code_point(U'\r');
	ts.push_code_point(U'A');

	BOOST_CHECK(ts.next_input_character() == U'\n');
	ts.consume();
	BOOST_CHECK(ts.next_input_character() == U'\n');
	ts.consume();
	BOOST_CHECK(ts.next_input_character() == U'A');
}

/*
CR, EOF
*/
BOOST_AUTO_TEST_CASE(input_stream__push_code_point__12)
{
	test_stream ts;
	ts.push_code_point(U'\r');
	ts.eof(true);

	BOOST_CHECK(ts.next_input_character() == U'\n');
	BOOST_CHECK(!ts.eof());
	ts.consume();
	BOOST_CHECK(ts.eof());
}

/*
現在の入力文字を返す

value_type current_input_character() const
*/
BOOST_AUTO_TEST_CASE(input_stream__current_input_character__1)
{
	test_stream ts;
	ts.push_code_point(U'A');
	ts.consume();

	BOOST_CHECK(ts.current_input_character() == U'A');
}

/*
次の入力文字を返す

value_type next_input_character() const
*/
BOOST_AUTO_TEST_CASE(input_stream__next_input_character__1)
{
	test_stream ts;
	ts.push_code_point(U'A');

	BOOST_CHECK(ts.next_input_character() == U'A');
}

/*
次の入力文字を消費する

void consume()
*/
BOOST_AUTO_TEST_CASE(input_stream__consume__1)
{
	test_stream ts;
	ts.push_code_point(U'A');
	ts.consume();

	BOOST_CHECK(ts.current_input_character() == U'A');
}

/*
現在の入力文字を再消費する

void reconsume()
*/
BOOST_AUTO_TEST_CASE(input_stream__reconsume__1)
{
	test_stream ts;
	ts.push_code_point(U'A');
	ts.consume();
	ts.reconsume();

	BOOST_CHECK(ts.next_input_character() == U'A');
}

/*
ストリーム終端に達しているか調べる

bool eof() const
*/
BOOST_AUTO_TEST_CASE(input_stream__eof__1)
{
	test_stream ts;

	BOOST_CHECK(ts.eof() == false);
}

/*
ストリーム終端を設定する

void eof(bool b)
*/
BOOST_AUTO_TEST_CASE(input_stream__eof__2)
{
	test_stream ts;
	ts.eof(true);

	BOOST_CHECK(ts.eof());
}

BOOST_AUTO_TEST_SUITE_END()
