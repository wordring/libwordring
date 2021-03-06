﻿
#include <iostream>
#include <iterator>
#include <string>

#include <boost/test/unit_test.hpp>

#include <wordring/whatwg/encoding/coder.hpp>
#include <wordring/whatwg/encoding/encoding.hpp>
#include <wordring/whatwg/encoding/terminology.hpp>

BOOST_AUTO_TEST_SUITE(whatwg_encoding_encoding_test)

BOOST_AUTO_TEST_CASE(encoding_process_utf8_decode_1)
{
	using namespace wordring::whatwg::encoding;

	// 1-4バイト迄の文字を一文字ずつ並べた。
	std::string_view sv = "\x41\xC3\x80\xE3\x81\x82\xF0\x90\x8A\x80";
	io_queue<char> in;
	in.push(sv.begin(), sv.end());

	io_queue<char32_t> out;
	auto r = utf8_decode(in, out);
	BOOST_CHECK(std::holds_alternative<result_continue>(r));
	
	std::u32string s;
	from_io_queue_convert(out, std::back_inserter(s));
	BOOST_CHECK(s == U"\U00000041\U000000C0\U00003042\U00010280");
}

BOOST_AUTO_TEST_CASE(encoding_process_utf8_decode_2)
{
	using namespace wordring::whatwg::encoding;

	// 空。
	std::string_view sv = "";
	io_queue<char> in = to_io_queue_convert(sv.begin(), sv.end());

	io_queue<char32_t> out;
	auto r = utf8_decode(in, out);
	BOOST_CHECK(std::holds_alternative<result_finished>(r));

	std::u32string s;
	from_io_queue_convert(out, std::back_inserter(s));
	BOOST_CHECK(s == U"");
}

BOOST_AUTO_TEST_CASE(encoding_bom_sniff_1)
{
	using namespace wordring::whatwg::encoding;

	std::string_view sv = "\xEF\xBB\xBF";
	io_queue<char> in = to_io_queue_convert(sv.begin(), sv.end());
	
	auto r = bom_sniff(in);
	BOOST_CHECK(r.m_value == encoding_name::UTF_8);
}

BOOST_AUTO_TEST_CASE(encoding_bom_sniff_2)
{
	using namespace wordring::whatwg::encoding;

	std::string_view sv = "\xFE\xFF";
	io_queue<char> in = to_io_queue_convert(sv.begin(), sv.end());
	
	auto r = bom_sniff(in);
	BOOST_CHECK(r.m_value == encoding_name::UTF_16BE);
}

BOOST_AUTO_TEST_CASE(encoding_bom_sniff_3)
{
	using namespace wordring::whatwg::encoding;

	std::string_view sv = "\xFF\xFE";
	io_queue<char> in = to_io_queue_convert(sv.begin(), sv.end());
	
	auto r = bom_sniff(in);
	BOOST_CHECK(r.m_value == encoding_name::UTF_16LE);
}

BOOST_AUTO_TEST_CASE(encoding_bom_sniff_4)
{
	using namespace wordring::whatwg::encoding;

	std::string_view sv = "\xEF\xBB\xBF";
	io_queue<char> in;
	in.push(sv.begin(), sv.end());
	
	auto r = bom_sniff(in);
	BOOST_CHECK(r.m_value == encoding_name::UTF_8);
}

BOOST_AUTO_TEST_CASE(encoding_bom_sniff_5)
{
	using namespace wordring::whatwg::encoding;

	std::string_view sv = "\xFE\xFF";
	io_queue<char> in;
	in.push(sv.begin(), sv.end());
	
	auto r = bom_sniff(in);
	// EOQ 無しの場合、規格上、待機状態で戻る。
	BOOST_CHECK(r.m_wait == true);
}

BOOST_AUTO_TEST_CASE(encoding_bom_sniff_6)
{
	using namespace wordring::whatwg::encoding;

	io_queue<char> in;
	auto r = bom_sniff(in);

	BOOST_CHECK(r.m_wait == true);
}

/*
// 復号器が result_finished を返した場合
BOOST_AUTO_TEST_CASE(process_token_decoder_0)
{
	namespace e = wordring::whatwg::encoding;

	test_decoder_1 decoder{};
	std::u32string output{};
	std::string input{ "0" };
	e::stream<std::string::iterator> stream{ input.begin(), input.end() };
	e::result_value r = e::process_token(stream.read(), decoder, stream, std::back_inserter(output));
	BOOST_CHECK(r.index() == 0);
}

// 復号器が result_continue を返した場合
BOOST_AUTO_TEST_CASE(process_token_decoder_1)
{
	namespace e = wordring::whatwg::encoding;

	test_decoder_1 decoder{};
	std::u32string output{};
	std::string input{ "1" };
	e::stream<std::string::iterator> stream{ input.begin(), input.end() };
	e::result_value r = e::process_token(stream.read(), decoder, stream, std::back_inserter(output));
	BOOST_CHECK(r.index() == 1);
}

// 復号器が 1個のコードポイント を返した場合
BOOST_AUTO_TEST_CASE(process_token_decoder_2)
{
	namespace e = wordring::whatwg::encoding;

	test_decoder_1 decoder{};
	std::u32string output{};
	std::string input{ "2" };
	e::stream<std::string::iterator> stream{ input.begin(), input.end() };
	e::result_value r = e::process_token(stream.read(), decoder, stream, std::back_inserter(output));
	BOOST_CHECK(r.index() == 1);
	BOOST_CHECK(output == U"A");
}

// 復号器が 2個のコードポイント を返した場合
BOOST_AUTO_TEST_CASE(process_token_decoder_3)
{
	namespace e = wordring::whatwg::encoding;

	test_decoder_1 decoder{};
	std::u32string output{};
	std::string input{ "3" };
	e::stream<std::string::iterator> stream{ input.begin(), input.end() };
	e::result_value r = e::process_token(stream.read(), decoder, stream, std::back_inserter(output));
	BOOST_CHECK(r.index() == 1);
	BOOST_CHECK(output == U"12");
}

// error_mode が Replacement で復号器が result_error を返した場合
BOOST_AUTO_TEST_CASE(process_token_decoder_8_1)
{
	namespace e = wordring::whatwg::encoding;

	test_decoder_1 decoder{};
	std::u32string output{};
	std::string input{ "8" };
	e::stream<std::string::iterator> stream{ input.begin(), input.end() };
	e::result_value r = e::process_token(stream.read(), decoder, stream, std::back_inserter(output));
	BOOST_CHECK(r.index() == 1);
	BOOST_CHECK(output.front() == 0xFFFD);
}

// error_mode が Fatal で復号器が result_error を返した場合
BOOST_AUTO_TEST_CASE(process_token_decoder_8_2)
{
	namespace e = wordring::whatwg::encoding;

	test_decoder_1 decoder{};
	std::u32string output{};
	std::string input{ "9" };
	e::stream<std::string::iterator> stream{ input.begin(), input.end() };
	e::result_value r = e::process_token(stream.read(), decoder, stream, std::back_inserter(output), e::error_mode_name::Fatal);
	BOOST_CHECK(r.index() == 8);
	BOOST_CHECK(std::get<8>(r).code_point == U'A');
}

// 符号化器が result_finished を返した場合
BOOST_AUTO_TEST_CASE(process_token_encoder_0)
{
	namespace e = wordring::whatwg::encoding;

	test_encoder_1 encoder{};
	std::string output{};
	std::u32string input{ U"0" };
	e::stream<std::u32string::iterator> stream{ input.begin(), input.end() };
	e::result_value r = e::process_token(stream.read(), encoder, stream, std::back_inserter(output));
	BOOST_CHECK(r.index() == 0);
}

// 符号化器が result_continue を返した場合
BOOST_AUTO_TEST_CASE(process_token_encoder_1)
{
	namespace e = wordring::whatwg::encoding;

	test_encoder_1 encoder{};
	std::string output{};
	std::u32string input{ U"1" };
	e::stream<std::u32string::iterator> stream{ input.begin(), input.end() };
	e::result_value r = e::process_token(stream.read(), encoder, stream, std::back_inserter(output));
	BOOST_CHECK(r.index() == 1);
}

// 符号化器が 1個のバイト を返した場合
BOOST_AUTO_TEST_CASE(process_token_encoder_4)
{
	namespace e = wordring::whatwg::encoding;

	test_encoder_1 encoder{};
	std::string output{};
	std::u32string input{ U"4" };
	e::stream<std::u32string::iterator> stream{ input.begin(), input.end() };
	e::result_value r = e::process_token(stream.read(), encoder, stream, std::back_inserter(output));
	BOOST_CHECK(r.index() == 1);
	BOOST_CHECK(output == "1");
}

// 符号化器が 2個のバイト を返した場合
BOOST_AUTO_TEST_CASE(process_token_encoder_5)
{
	namespace e = wordring::whatwg::encoding;

	test_encoder_1 encoder{};
	std::string output{};
	std::u32string input{ U"5" };
	e::stream<std::u32string::iterator> stream{ input.begin(), input.end() };
	e::result_value r = e::process_token(stream.read(), encoder, stream, std::back_inserter(output));
	BOOST_CHECK(r.index() == 1);
	BOOST_CHECK(output == "12");
}

// 符号化器が 3個のバイト を返した場合
BOOST_AUTO_TEST_CASE(process_token_encoder_6)
{
	namespace e = wordring::whatwg::encoding;

	test_encoder_1 encoder{};
	std::string output{};
	std::u32string input{ U"6" };
	e::stream<std::u32string::iterator> stream{ input.begin(), input.end() };
	e::result_value r = e::process_token(stream.read(), encoder, stream, std::back_inserter(output));
	BOOST_CHECK(r.index() == 1);
	BOOST_CHECK(output == "123");
}

// 符号化器が 4個のバイト を返した場合
BOOST_AUTO_TEST_CASE(process_token_encoder_7)
{
	namespace e = wordring::whatwg::encoding;

	test_encoder_1 encoder{};
	std::string output{};
	std::u32string input{ U"7" };
	e::stream<std::u32string::iterator> stream{ input.begin(), input.end() };
	e::result_value r = e::process_token(stream.read(), encoder, stream, std::back_inserter(output));
	BOOST_CHECK(r.index() == 1);
	BOOST_CHECK(output == "1234");
}

// error_mode が Fatal で符号化器が result_error を返した場合
BOOST_AUTO_TEST_CASE(process_token_encoder_8_1)
{
	namespace e = wordring::whatwg::encoding;

	test_encoder_1 encoder{};
	std::string output{};
	std::u32string input{ U"8" };
	e::stream<std::u32string::iterator> stream{ input.begin(), input.end() };
	e::result_value r = e::process_token(stream.read(), encoder, stream, std::back_inserter(output));
	BOOST_CHECK(r.index() == 8);
}

// error_mode が Html で符号化器が result_error を返した場合
BOOST_AUTO_TEST_CASE(process_token_encoder_8_2)
{
	namespace e = wordring::whatwg::encoding;

	test_encoder_1 encoder{};
	std::string output{};
	std::u32string input{ U"9" };
	e::stream<std::u32string::iterator> stream{ input.begin(), input.end() };
	e::result_value r = e::process_token(stream.read(), encoder, stream, std::back_inserter(output), e::error_mode_name::Html);
	BOOST_CHECK(r.index() == 1);
	BOOST_CHECK(output == "");

	while (stream) output.push_back(static_cast<char8_t>(*stream.read()));
	BOOST_CHECK(output == "&#1234;");
}

BOOST_AUTO_TEST_CASE(get_name_1)
{
	using namespace wordring::whatwg::encoding;
	BOOST_CHECK(get_name(U"  Unicode-1-1-utf-8  ") == name::UTF_8);
}

BOOST_AUTO_TEST_CASE(get_name_2)
{
	using namespace wordring::whatwg::encoding;
	BOOST_CHECK(get_name(std::u8string(u8"  Unicode-1-1-utf-8  ")) == name::UTF_8);
}

BOOST_AUTO_TEST_CASE(get_name_3)
{
	using namespace wordring::whatwg::encoding;
	BOOST_CHECK(get_name(std::u16string(u"  Unicode-1-1-utf-8  ")) == name::UTF_8);
}

// BOM UTF-8
BOOST_AUTO_TEST_CASE(decode_1)
{
	using namespace wordring::whatwg::encoding;

	std::string s{ "\xEF\xBB\xBF\xE3\x81\x82" };
	std::u32string out{};
	decode(s.begin(), s.end(), name::Big5, std::back_inserter(out));

	BOOST_CHECK(out == U"あ");
}

// BOM UTF-16BE
BOOST_AUTO_TEST_CASE(decode_2)
{
	using namespace wordring::whatwg::encoding;

	std::string s{ "\xFE\xFF\x30\x42" };
	std::u32string out{};
	decode(s.begin(), s.end(), name::Big5, std::back_inserter(out));

	BOOST_CHECK(out == U"あ");
}

// BOM UTF-16LE
BOOST_AUTO_TEST_CASE(decode_3)
{
	using namespace wordring::whatwg::encoding;

	std::string s{ "\xFF\xFE\x42\x30" };
	std::u32string out{};
	decode(s.begin(), s.end(), name::Big5, std::back_inserter(out));

	BOOST_CHECK(out == U"あ");
}

// BOM 無し
BOOST_AUTO_TEST_CASE(decode_4)
{
	using namespace wordring::whatwg::encoding;

	std::string s{ u8"あ" };
	std::u32string out{};
	decode(s.begin(), s.end(), name::UTF_8, std::back_inserter(out));

	BOOST_CHECK(out == U"あ");
}

// BOM UTF-8
BOOST_AUTO_TEST_CASE(utf8_decode_1)
{
	using namespace wordring::whatwg::encoding;

	std::string s{ "\xEF\xBB\xBF\xE3\x81\x82" };
	std::u32string out{};
	utf8_decode(s.begin(), s.end(), std::back_inserter(out));

	BOOST_CHECK(out == U"あ");
}

// BOM 無し
BOOST_AUTO_TEST_CASE(utf8_decode_2)
{
	using namespace wordring::whatwg::encoding;

	std::string s{ "\xE3\x81\x82" };
	std::u32string out{};
	utf8_decode(s.begin(), s.end(), std::back_inserter(out));

	BOOST_CHECK(out == U"あ");
}

BOOST_AUTO_TEST_CASE(utf8_decode_3)
{
	using namespace wordring::whatwg::encoding;

	std::string s{ "a" };
	std::u32string out{};
	utf8_decode(s.begin(), s.end(), std::back_inserter(out));

	BOOST_CHECK(out == U"a");
}

BOOST_AUTO_TEST_CASE(utf8_decode_4)
{
	using namespace wordring::whatwg::encoding;

	std::string s{ "abcde" };
	std::u32string out{};
	utf8_decode(s.begin(), s.end(), std::back_inserter(out));

	BOOST_CHECK(out == U"abcde");
}

// BOM UTF-8
BOOST_AUTO_TEST_CASE(utf8_decode_without_bom_1)
{
	using namespace wordring::whatwg::encoding;

	std::string s{ "\xEF\xBB\xBF\xE3\x81\x82" };
	std::u32string out{};
	utf8_decode_without_bom(s.begin(), s.end(), std::back_inserter(out));

	BOOST_CHECK(out == U"\uFEFF\u3042");
}

// BOM 無し
BOOST_AUTO_TEST_CASE(utf8_decode_without_bom_2)
{
	using namespace wordring::whatwg::encoding;

	std::string s{ u8"あ" };
	std::u32string out{};
	utf8_decode_without_bom(s.begin(), s.end(), std::back_inserter(out));

	BOOST_CHECK(out == U"あ");
}

// BOM UTF-8
BOOST_AUTO_TEST_CASE(utf8_decode_without_bom_or_fail_1)
{
	using namespace wordring::whatwg::encoding;

	std::string s{ "\xEF\xBB\xBF\xE3\x81\x82" };
	std::u32string out{};
	bool ret = utf8_decode_without_bom_or_fail(s.begin(), s.end(), std::back_inserter(out));

	BOOST_CHECK(ret);
	BOOST_CHECK(out == U"\uFEFF\u3042");
}

// BOM 無し 失敗させた場合
BOOST_AUTO_TEST_CASE(utf8_decode_without_bom_or_fail_2)
{
	using namespace wordring::whatwg::encoding;

	std::string s{ "あいうえお\xC3" };
	std::u32string out{};
	bool ret = utf8_decode_without_bom_or_fail(s.begin(), s.end(), std::back_inserter(out));

	BOOST_CHECK(ret == false);
	BOOST_CHECK(out == U"");
}

BOOST_AUTO_TEST_CASE(encode_1)
{
	using namespace wordring::whatwg::encoding;

	std::u32string s{ U"あいうえお" };
	std::string out{};
	encode(s.begin(), s.end(), name::UTF_8, std::back_inserter(out));

	BOOST_CHECK(out == u8"あいうえお");
}

BOOST_AUTO_TEST_CASE(utf8_encode_1)
{
	using namespace wordring::whatwg::encoding;

	std::u32string s{ U"あいうえお" };
	std::string out{};
	utf8_encode(s.begin(), s.end(), std::back_inserter(out));

	BOOST_CHECK(out == u8"あいうえお");
}
*/
BOOST_AUTO_TEST_SUITE_END()

