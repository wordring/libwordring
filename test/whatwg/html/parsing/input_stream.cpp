// test/whatwg/html/parsing/input_stream.cpp

#include <boost/test/unit_test.hpp>

#include <wordring/whatwg/html/parsing/input_stream.hpp>

#include <string>

namespace
{
	struct test_stream : wordring::whatwg::html::parsing::input_stream<test_stream>
	{
		using base_type = wordring::whatwg::html::parsing::input_stream<test_stream>;

		using base_type::match_named_character_reference;
		using base_type::consume;

		using base_type::m_c;

		wordring::whatwg::html::parsing::error_name m_ec;

		std::u32string m_emits;

		void on_report_error(wordring::whatwg::html::parsing::error_name ec) { m_ec = ec; }

		void on_emit_code_point()
		{
			if(!m_c.empty()) m_emits.push_back(m_c.back());
		}
	};
}

BOOST_AUTO_TEST_SUITE(parsing_input_stream_test)

/*
空の入力ストリームを構築する

input_stream()
*/
BOOST_AUTO_TEST_CASE(input_stream_construct_1)
{
	test_stream ts;
}

/*
エラー報告する

void report_error(error_name ec)
*/
BOOST_AUTO_TEST_CASE(input_stream_report_error_1)
{
	using namespace wordring::whatwg::html::parsing;

	test_stream ts;
	ts.report_error(error_name::cdata_in_html_content);

	BOOST_CHECK(ts.m_ec == error_name::cdata_in_html_content);
}

/*
文字を発送する

void emit_code_point(value_type cp)
*/
BOOST_AUTO_TEST_CASE(input_stream_emit_code_point_1)
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
BOOST_AUTO_TEST_CASE(input_stream_push_code_point_1)
{
	test_stream ts;
	ts.push_code_point(U'A');

	BOOST_CHECK(ts.next_input_character() == U'A');
}

/*
エラー報告をテスト
サロゲート文字
*/
BOOST_AUTO_TEST_CASE(input_stream_push_code_point_2)
{
	using namespace wordring::whatwg::html::parsing;

	test_stream ts;
	ts.push_code_point(U'\xD800');

	BOOST_CHECK(ts.m_ec == error_name::surrogate_in_input_stream);
	BOOST_CHECK(ts.next_input_character() == U'\xD800');
}

/*
エラー報告をテスト
非文字
*/
BOOST_AUTO_TEST_CASE(input_stream_push_code_point_3)
{
	using namespace wordring::whatwg::html::parsing;

	test_stream ts;
	ts.push_code_point(U'\xFFFE');

	BOOST_CHECK(ts.m_ec == error_name::noncharacter_in_input_stream);
	BOOST_CHECK(ts.next_input_character() == U'\xFFFE');
}

/*
エラー報告をテスト
コントロール文字
*/
BOOST_AUTO_TEST_CASE(input_stream_push_code_point_4)
{
	using namespace wordring::whatwg::html::parsing;

	test_stream ts;
	ts.push_code_point(U'\x7F');

	BOOST_CHECK(ts.m_ec == error_name::control_character_in_input_stream);
	BOOST_CHECK(ts.next_input_character() == U'\x7F');
}

BOOST_AUTO_TEST_CASE(input_stream_push_code_point_5)
{
	using namespace wordring::whatwg::html::parsing;

	test_stream ts;

	ts.m_ec = error_name::abrupt_closing_of_empty_comment; // エラー無しの判定用ダミー
	ts.push_code_point(U'\x9');

	BOOST_CHECK(ts.m_ec == error_name::abrupt_closing_of_empty_comment);
	BOOST_CHECK(ts.next_input_character() == U'\x9');
}

BOOST_AUTO_TEST_CASE(input_stream_push_code_point_6)
{
	using namespace wordring::whatwg::html::parsing;

	test_stream ts;
	ts.m_ec = error_name::abrupt_closing_of_empty_comment; // エラー無しの判定用ダミー
	ts.push_code_point(U'\x0');

	BOOST_CHECK(ts.m_ec == error_name::abrupt_closing_of_empty_comment);
	BOOST_CHECK(ts.next_input_character() == U'\x0');
}

/*
改行文字正規化をテスト
CR
*/
BOOST_AUTO_TEST_CASE(input_stream_push_code_point_7)
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
BOOST_AUTO_TEST_CASE(input_stream_push_code_point_8)
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
BOOST_AUTO_TEST_CASE(input_stream_push_code_point_9)
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
BOOST_AUTO_TEST_CASE(input_stream_push_code_point_10)
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
BOOST_AUTO_TEST_CASE(input_stream_push_code_point_11)
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
BOOST_AUTO_TEST_CASE(input_stream_push_code_point_12)
{
	test_stream ts;
	ts.push_code_point(U'\r');
	ts.push_eof();

	BOOST_CHECK(ts.next_input_character() == U'\n');
	ts.consume();
	BOOST_CHECK(!ts.eof());
	ts.consume();
	BOOST_CHECK(ts.eof());
}

/*
現在の入力文字を返す

value_type current_input_character() const
*/
BOOST_AUTO_TEST_CASE(input_stream_current_input_character_1)
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
BOOST_AUTO_TEST_CASE(input_stream_next_input_character_1)
{
	test_stream ts;
	ts.push_code_point(U'A');

	BOOST_CHECK(ts.next_input_character() == U'A');
}

/*
与えられた文字列とストリーム・バッファ内の文字列を比較する
*/
BOOST_AUTO_TEST_CASE(input_stream_match_1)
{
	using namespace wordring::whatwg::html::parsing;

	test_stream ts;
	auto s = std::u32string(U"PUBL");
	for (char32_t cp : s) ts.push_code_point(cp);

	BOOST_CHECK(!ts.match(U"public", false, true));
}

BOOST_AUTO_TEST_CASE(input_stream_match_2)
{
	using namespace wordring::whatwg::html::parsing;

	test_stream ts;
	auto s = std::u32string(U"PUBLICA");
	for (char32_t cp : s) ts.push_code_point(cp);

	BOOST_CHECK(ts.match(U"public", false, true));
}

/*
名前付き文字参照とストリーム・バッファ内の文字列を比較する

*/
BOOST_AUTO_TEST_CASE(input_stream_match_named_character_reference_1)
{
	test_stream ts;
	auto s = std::u32string(U"xodot;");
	for (char32_t cp : s) ts.push_code_point(cp);
	ts.eof();

	std::uint32_t len;
	std::array<char32_t, 2> a = ts.match_named_character_reference(len);

	BOOST_CHECK(len == 6);
	BOOST_CHECK(a[0] == U'\x2A00');
	BOOST_CHECK(a[1] == U'\0');
}

BOOST_AUTO_TEST_CASE(input_stream_match_named_character_reference_2)
{
	test_stream ts;
	auto s = std::u32string(U"xodot");
	for (char32_t cp : s) ts.push_code_point(cp);

	std::uint32_t len;
	std::array<char32_t, 2> a = ts.match_named_character_reference(len);

	BOOST_CHECK(len == 0);
	BOOST_CHECK(a[0] == U'\0');
	BOOST_CHECK(a[1] == U'\0');
}

BOOST_AUTO_TEST_CASE(input_stream_match_named_character_reference_3)
{
	test_stream ts;
	auto s = std::u32string(U"xodotA");
	for (char32_t cp : s) ts.push_code_point(cp);

	std::uint32_t len;
	std::array<char32_t, 2> a = ts.match_named_character_reference(len);

	BOOST_CHECK(len == 0);
	BOOST_CHECK(a[0] == U'\0');
	BOOST_CHECK(a[1] == U'\0');
}

BOOST_AUTO_TEST_CASE(input_stream_match_named_character_reference_4)
{
	test_stream ts;
	auto s = std::u32string(U"xodot;A");
	for (char32_t cp : s) ts.push_code_point(cp);
//TODO:
	std::uint32_t len;
	std::array<char32_t, 2> a = ts.match_named_character_reference(len);

	BOOST_CHECK(len == 6);
	BOOST_CHECK(a[0] == U'\x2A00');
	BOOST_CHECK(a[1] == U'\0');
}

BOOST_AUTO_TEST_CASE(input_stream_match_named_character_reference_5)
{
	test_stream ts;
	auto s = std::u32string(U"Aacute;");
	for (char32_t cp : s) ts.push_code_point(cp);

	std::uint32_t len;
	std::array<char32_t, 2> a = ts.match_named_character_reference(len);

	BOOST_CHECK(len == 7);
	BOOST_CHECK(a[0] == U'\xC1');
	BOOST_CHECK(a[1] == U'\0');
}

BOOST_AUTO_TEST_CASE(input_stream_match_named_character_reference_6)
{
	test_stream ts;
	auto s = std::u32string(U"Aacute");
	for (char32_t cp : s) ts.push_code_point(cp);

	std::uint32_t len;
	std::array<char32_t, 2> a = ts.match_named_character_reference(len);

	BOOST_CHECK(len == 6);
	BOOST_CHECK(a[0] == U'\xC1');
	BOOST_CHECK(a[1] == U'\0');
}

/*
次の入力文字を消費する

void consume()
*/
BOOST_AUTO_TEST_CASE(input_stream_consume_1)
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
BOOST_AUTO_TEST_CASE(input_stream_reconsume_1)
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
BOOST_AUTO_TEST_CASE(input_stream_eof_1)
{
	test_stream ts;

	BOOST_CHECK(ts.eof() == false);
}

/*
ストリーム終端を設定する

void eof(bool b)
*/
BOOST_AUTO_TEST_CASE(input_stream_eof_2)
{
	test_stream ts;
	ts.push_eof();

	BOOST_CHECK(!ts.eof());
	BOOST_CHECK(ts.consume() == 0);
	BOOST_CHECK(ts.eof());
}

BOOST_AUTO_TEST_SUITE_END()
