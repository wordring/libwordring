
#include <iostream>
#include <iterator>
#include <string>

#include <boost/test/unit_test.hpp>

#include <wordring/whatwg/encoding/coder.hpp>
#include <wordring/whatwg/encoding/encoding.hpp>
#include <wordring/whatwg/encoding/stream.hpp>

// 試験用復号器
class test_decoder_1 : public wordring::whatwg::encoding::decoder
{
public:
	template <typename Stream, typename Token>
	wordring::whatwg::encoding::result_value run(Stream& input, Token byte)
	{
		switch (byte.value())
		{
		case '0': return wordring::whatwg::encoding::result_finished{};
		case '1': return wordring::whatwg::encoding::result_continue{};
		case '2': return U'A';
		case '3': return std::array<uint32_t, 2>{ U'1', U'2' };
		case '8': return wordring::whatwg::encoding::result_error{};
		case '9': return wordring::whatwg::encoding::result_error{ U'A' };
		}
		return wordring::whatwg::encoding::result_finished{};
	}
};

// 試験用符号化器
class test_encoder_1 : public wordring::whatwg::encoding::encoder
{
public:
	template <typename Stream, typename Token>
	wordring::whatwg::encoding::result_value run(Stream& input, Token cp)
	{
		switch (cp.value())
		{
		case '0': return wordring::whatwg::encoding::result_finished{};
		case '1': return wordring::whatwg::encoding::result_continue{};
		case '4': return static_cast<uint8_t>('1');
		case '5': return std::array<uint8_t, 2>{ '1', '2' };
		case '6': return std::array<uint8_t, 3>{ '1', '2', '3' };
		case '7': return std::array<uint8_t, 4>{ '1', '2', '3', '4' };
		case '8': return wordring::whatwg::encoding::result_error{};
		case '9': return wordring::whatwg::encoding::result_error{ 1234 };
		}
		return wordring::whatwg::encoding::result_finished{};
	}
};

BOOST_AUTO_TEST_SUITE(whatwg_encoding_test)

// 復号器が result_finished を返した場合
BOOST_AUTO_TEST_CASE(whatwg_encoding__process_token__decoder_0)
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
BOOST_AUTO_TEST_CASE(whatwg_encoding__process_token__decoder_1)
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
BOOST_AUTO_TEST_CASE(whatwg_encoding__process_token__decoder_2)
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
BOOST_AUTO_TEST_CASE(whatwg_encoding__process_token__decoder_3)
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
BOOST_AUTO_TEST_CASE(whatwg_encoding__process_token__decoder_8_1)
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
BOOST_AUTO_TEST_CASE(whatwg_encoding__process_token__decoder_8_2)
{
	namespace e = wordring::whatwg::encoding;

	test_decoder_1 decoder{};
	std::u32string output{};
	std::string input{ "9" };
	e::stream<std::string::iterator> stream{ input.begin(), input.end() };
	e::result_value r = e::process_token(stream.read(), decoder, stream, std::back_inserter(output), e::error_mode::Fatal);
	BOOST_CHECK(r.index() == 8);
	BOOST_CHECK(std::get<8>(r).code_point == U'A');
}

// 符号化器が result_finished を返した場合
BOOST_AUTO_TEST_CASE(whatwg_encoding__process_token__encoder_0)
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
BOOST_AUTO_TEST_CASE(whatwg_encoding__process_token__encoder_1)
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
BOOST_AUTO_TEST_CASE(whatwg_encoding__process_token__encoder_4)
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
BOOST_AUTO_TEST_CASE(whatwg_encoding__process_token__encoder_5)
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
BOOST_AUTO_TEST_CASE(whatwg_encoding__process_token__encoder_6)
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
BOOST_AUTO_TEST_CASE(whatwg_encoding__process_token__encoder_7)
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
BOOST_AUTO_TEST_CASE(whatwg_encoding__process_token__encoder_8_1)
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
BOOST_AUTO_TEST_CASE(whatwg_encoding__process_token__encoder_8_2)
{
	namespace e = wordring::whatwg::encoding;

	test_encoder_1 encoder{};
	std::string output{};
	std::u32string input{ U"9" };
	e::stream<std::u32string::iterator> stream{ input.begin(), input.end() };
	e::result_value r = e::process_token(stream.read(), encoder, stream, std::back_inserter(output), e::error_mode::Html);
	BOOST_CHECK(r.index() == 1);
	BOOST_CHECK(output == "&#1234;");
}

BOOST_AUTO_TEST_CASE(whatwg_encoding__get_encoding_name)
{
	std::cout << sizeof(wordring::whatwg::encoding::result_continue) << std::endl;
	
	BOOST_CHECK(wordring::whatwg::encoding::get_name(std::u32string_view{ U"  Unicode-1-1-utf-8  " }) == wordring::whatwg::encoding::name::UTF_8);
}

BOOST_AUTO_TEST_SUITE_END()
