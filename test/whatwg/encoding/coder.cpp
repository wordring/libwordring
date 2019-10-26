// test/whatwg/encoding/decoder.cpp

#include <boost/test/unit_test.hpp>

#include <iterator>
#include <string>

#include <wordring/whatwg/encoding/coder.hpp>
#include <wordring/whatwg/encoding/encoding.hpp>
#include <wordring/whatwg/encoding/stream.hpp>

BOOST_AUTO_TEST_SUITE(whatwg_encoding__decoder_test)

BOOST_AUTO_TEST_CASE(whatwg_encoding__UTF_8_decoder)
{
	using namespace wordring::whatwg::encoding;

	std::u32string const correct{ U"ABCDEあいうえお𠀋𡈽𡌛𡑮𡢽AÀⱥ𐊠" };
	std::string const in{ u8"ABCDEあいうえお𠀋𡈽𡌛𡑮𡢽AÀⱥ𐊠" };
	std::u32string out{};

	UTF_8_decoder coder{};
	stream<std::string::const_iterator> stream_8{ in.cbegin(), in.cend() };

	run(coder, stream_8, std::back_inserter(out));
	BOOST_CHECK(out == correct);

	stream_8 = stream<std::string::const_iterator>{ in.cbegin(), in.cend() };
	out.clear();
	run_decoder(name::UTF_8, stream_8, std::back_inserter(out));
	BOOST_CHECK(out == correct);
}

BOOST_AUTO_TEST_CASE(whatwg_encoding__UTF_8_encoder)
{
	using namespace wordring::whatwg::encoding;

	std::string const correct{ u8"ABCDEあいうえお𠀋𡈽𡌛𡑮𡢽AÀⱥ𐊠" };
	std::u32string const in{ U"ABCDEあいうえお𠀋𡈽𡌛𡑮𡢽AÀⱥ𐊠" };
	std::string out{};

	UTF_8_encoder coder{};
	stream<std::u32string::const_iterator> stream_32{ in.cbegin(), in.cend() };

	run(coder, stream_32, std::back_inserter(out));
	BOOST_CHECK(out == correct);

	stream_32 = stream<std::u32string::const_iterator>{ in.cbegin(), in.cend() };
	out.clear();
	run_encoder(name::UTF_8, stream_32, std::back_inserter(out));
	BOOST_CHECK(out == correct);
}

BOOST_AUTO_TEST_CASE(whatwg_encoding__single_byte_decoder)
{
	using namespace wordring::whatwg::encoding;

	std::u32string const correct{ U"ABCDE€ÿ" };
	std::string const in{ "ABCDE\x80\xFF" };
	std::u32string out{};

	single_byte_decoder<index_code_point_windows_1252> coder{};
	stream<std::string::const_iterator> stream_8{ in.cbegin(), in.cend() };

	run(coder, stream_8, std::back_inserter(out));
	BOOST_CHECK(out == correct);
}

BOOST_AUTO_TEST_CASE(whatwg_encoding__single_byte_encoder)
{
	using namespace wordring::whatwg::encoding;

	std::string const correct{ "ABCDE\x80\xFF" };
	std::u32string const in{ U"ABCDE€ÿ" };
	std::string out{};

	single_byte_encoder<index_pointer_windows_1252_0, index_pointer_windows_1252_1> coder{};
	stream<std::u32string::const_iterator> stream_32{ in.cbegin(), in.cend() };

	run(coder, stream_32, std::back_inserter(out));
	BOOST_CHECK(out == correct);
}

BOOST_AUTO_TEST_CASE(whatwg_encoding__IBM866_coder)
{
	using namespace wordring::whatwg::encoding;

	std::string in{};
	std::u32string u32{};
	std::string out{};
	for (uint16_t i = 0; i <= 0xFFu; i++) in.push_back(static_cast<uint8_t>(i));

	IBM866_decoder decoder_0{};
	IBM866_encoder encoder_0{};

	stream<std::string::const_iterator> stream_8{ in.cbegin(), in.cend() };
	run(decoder_0, stream_8, std::back_inserter(u32));
	stream<std::u32string::const_iterator> stream_32{ u32.cbegin(), u32.cend() };
	run(encoder_0, stream_32, std::back_inserter(out));
	BOOST_CHECK(out.size() == 256);
	BOOST_CHECK(in == out);
}

BOOST_AUTO_TEST_CASE(whatwg_encoding__IBM866_run)
{
	using namespace wordring::whatwg::encoding;

	std::string in{};
	std::u32string u32{};
	std::string out{};
	for (uint16_t i = 0; i <= 0xFFu; i++) in.push_back(static_cast<uint8_t>(i));

	stream<std::string::const_iterator> stream_8{ in.cbegin(), in.cend() };
	run_decoder(name::IBM866, stream_8, std::back_inserter(u32));
	stream<std::u32string::const_iterator> stream_32{ u32.cbegin(), u32.cend() };
	run_encoder(name::IBM866, stream_32, std::back_inserter(out));
	BOOST_CHECK(out.size() == 256);
	BOOST_CHECK(in == out);
}

BOOST_AUTO_TEST_CASE(whatwg_encoding__ISO_8859_2_coder)
{
	using namespace wordring::whatwg::encoding;

	std::string in{};
	std::u32string u32{};
	std::string out{};
	for (uint16_t i = 0; i <= 0xFFu; i++) in.push_back(static_cast<uint8_t>(i));

	ISO_8859_2_decoder decoder_0{};
	ISO_8859_2_encoder encoder_0{};

	stream<std::string::const_iterator> stream_8{ in.cbegin(), in.cend() };
	run(decoder_0, stream_8, std::back_inserter(u32));
	stream<std::u32string::const_iterator> stream_32{ u32.cbegin(), u32.cend() };
	run(encoder_0, stream_32, std::back_inserter(out));
	BOOST_CHECK(out.size() == 256);
	BOOST_CHECK(in == out);
}

BOOST_AUTO_TEST_CASE(whatwg_encoding__ISO_8859_2_run)
{
	using namespace wordring::whatwg::encoding;

	std::string in{};
	std::u32string u32{};
	std::string out{};
	for (uint16_t i = 0; i <= 0xFFu; i++) in.push_back(static_cast<uint8_t>(i));

	stream<std::string::const_iterator> stream_8{ in.cbegin(), in.cend() };
	run_decoder(name::ISO_8859_2, stream_8, std::back_inserter(u32));
	stream<std::u32string::const_iterator> stream_32{ u32.cbegin(), u32.cend() };
	run_encoder(name::ISO_8859_2, stream_32, std::back_inserter(out));
	BOOST_CHECK(out.size() == 256);
	BOOST_CHECK(in == out);
}

BOOST_AUTO_TEST_CASE(whatwg_encoding__ISO_8859_3_coder)
{
	using namespace wordring::whatwg::encoding;

	std::string in{};
	std::u32string u32{};
	std::string out{};
	for (uint16_t i = 0; i <= 0xFFu; i++)
	{
		switch (i)
		{
		case 0x80 + 37:
		case 0x80 + 46:
		case 0x80 + 62:
		case 0x80 + 67:
		case 0x80 + 80:
		case 0x80 + 99:
		case 0x80 + 112:
			continue;
		}
		in.push_back(static_cast<uint8_t>(i));
	}

	ISO_8859_3_decoder decoder_0{};
	ISO_8859_3_encoder encoder_0{};

	stream<std::string::const_iterator> stream_8{ in.cbegin(), in.cend() };
	run(decoder_0, stream_8, std::back_inserter(u32));
	stream<std::u32string::const_iterator> stream_32{ u32.cbegin(), u32.cend() };
	run(encoder_0, stream_32, std::back_inserter(out));
	BOOST_CHECK(out.size() == 128 + 121);
	BOOST_CHECK(in == out);
}

BOOST_AUTO_TEST_CASE(whatwg_encoding__ISO_8859_3_run)
{
	using namespace wordring::whatwg::encoding;

	std::string in{};
	std::u32string u32{};
	std::string out{};
	for (uint16_t i = 0; i <= 0xFFu; i++)
	{
		switch (i)
		{
		case 0x80 + 37:
		case 0x80 + 46:
		case 0x80 + 62:
		case 0x80 + 67:
		case 0x80 + 80:
		case 0x80 + 99:
		case 0x80 + 112:
			continue;
		}
		in.push_back(static_cast<uint8_t>(i));
	}

	stream<std::string::const_iterator> stream_8{ in.cbegin(), in.cend() };
	run_decoder(name::ISO_8859_3, stream_8, std::back_inserter(u32));
	stream<std::u32string::const_iterator> stream_32{ u32.cbegin(), u32.cend() };
	run_encoder(name::ISO_8859_3, stream_32, std::back_inserter(out));
	BOOST_CHECK(out.size() == 128 + 121);
	BOOST_CHECK(in == out);
}

BOOST_AUTO_TEST_CASE(whatwg_encoding__ISO_8859_4_coder)
{
	using namespace wordring::whatwg::encoding;

	std::string in{};
	std::u32string u32{};
	std::string out{};
	for (uint16_t i = 0; i <= 0xFFu; i++) in.push_back(static_cast<uint8_t>(i));

	ISO_8859_4_decoder decoder_0{};
	ISO_8859_4_encoder encoder_0{};

	stream<std::string::const_iterator> stream_8{ in.cbegin(), in.cend() };
	run(decoder_0, stream_8, std::back_inserter(u32));
	stream<std::u32string::const_iterator> stream_32{ u32.cbegin(), u32.cend() };
	run(encoder_0, stream_32, std::back_inserter(out));
	BOOST_CHECK(out.size() == 256);
	BOOST_CHECK(in == out);
}

BOOST_AUTO_TEST_CASE(whatwg_encoding__ISO_8859_4_run)
{
	using namespace wordring::whatwg::encoding;

	std::string in{};
	std::u32string u32{};
	std::string out{};
	for (uint16_t i = 0; i <= 0xFFu; i++) in.push_back(static_cast<uint8_t>(i));

	stream<std::string::const_iterator> stream_8{ in.cbegin(), in.cend() };
	run_decoder(name::ISO_8859_4, stream_8, std::back_inserter(u32));
	stream<std::u32string::const_iterator> stream_32{ u32.cbegin(), u32.cend() };
	run_encoder(name::ISO_8859_4, stream_32, std::back_inserter(out));
	BOOST_CHECK(out.size() == 256);
	BOOST_CHECK(in == out);
}

BOOST_AUTO_TEST_CASE(whatwg_encoding__ISO_8859_5_coder)
{
	using namespace wordring::whatwg::encoding;

	std::string in{};
	std::u32string u32{};
	std::string out{};
	for (uint16_t i = 0; i <= 0xFFu; i++) in.push_back(static_cast<uint8_t>(i));

	ISO_8859_5_decoder decoder_0{};
	ISO_8859_5_encoder encoder_0{};

	stream<std::string::const_iterator> stream_8{ in.cbegin(), in.cend() };
	run(decoder_0, stream_8, std::back_inserter(u32));
	stream<std::u32string::const_iterator> stream_32{ u32.cbegin(), u32.cend() };
	run(encoder_0, stream_32, std::back_inserter(out));
	BOOST_CHECK(out.size() == 256);
	BOOST_CHECK(in == out);
}

BOOST_AUTO_TEST_CASE(whatwg_encoding__ISO_8859_5_run)
{
	using namespace wordring::whatwg::encoding;

	std::string in{};
	std::u32string u32{};
	std::string out{};
	for (uint16_t i = 0; i <= 0xFFu; i++) in.push_back(static_cast<uint8_t>(i));

	stream<std::string::const_iterator> stream_8{ in.cbegin(), in.cend() };
	run_decoder(name::ISO_8859_5, stream_8, std::back_inserter(u32));
	stream<std::u32string::const_iterator> stream_32{ u32.cbegin(), u32.cend() };
	run_encoder(name::ISO_8859_5, stream_32, std::back_inserter(out));
	BOOST_CHECK(out.size() == 256);
	BOOST_CHECK(in == out);
}

BOOST_AUTO_TEST_CASE(whatwg_encoding__ISO_8859_6_coder)
{
	using namespace wordring::whatwg::encoding;

	std::string in{};
	std::u32string u32{};
	std::string out{};
	for (uint16_t i = 0; i <= 0xFFu; i++)
	{
		if (0x80 + 33 <= i && i <= 0x80 + 35) continue;
		if (0x80 + 37 <= i && i <= 0x80 + 43) continue;
		if (0x80 + 46 <= i && i <= 0x80 + 58) continue;
		if (0x80 + 60 <= i && i <= 0x80 + 62) continue;
		if (i == 0x80 + 64) continue;
		if (0x80 + 91 <= i && i <= 0x80 + 95) continue;
		if (0x80 + 115 <= i && i <= 0x80 + 127) continue;
		in.push_back(static_cast<uint8_t>(i));
	}

	ISO_8859_6_decoder decoder_0{};
	ISO_8859_6_encoder encoder_0{};

	stream<std::string::const_iterator> stream_8{ in.cbegin(), in.cend() };
	run(decoder_0, stream_8, std::back_inserter(u32));
	stream<std::u32string::const_iterator> stream_32{ u32.cbegin(), u32.cend() };
	run(encoder_0, stream_32, std::back_inserter(out));
	BOOST_CHECK(out.size() == 128 + 83);
	BOOST_CHECK(in == out);
}

BOOST_AUTO_TEST_CASE(whatwg_encoding__ISO_8859_6_run)
{
	using namespace wordring::whatwg::encoding;

	std::string in{};
	std::u32string u32{};
	std::string out{};
	for (uint16_t i = 0; i <= 0xFFu; i++)
	{
		if (0x80 + 33 <= i && i <= 0x80 + 35) continue;
		if (0x80 + 37 <= i && i <= 0x80 + 43) continue;
		if (0x80 + 46 <= i && i <= 0x80 + 58) continue;
		if (0x80 + 60 <= i && i <= 0x80 + 62) continue;
		if (i == 0x80 + 64) continue;
		if (0x80 + 91 <= i && i <= 0x80 + 95) continue;
		if (0x80 + 115 <= i && i <= 0x80 + 127) continue;
		in.push_back(static_cast<uint8_t>(i));
	}

	stream<std::string::const_iterator> stream_8{ in.cbegin(), in.cend() };
	run_decoder(name::ISO_8859_6, stream_8, std::back_inserter(u32));
	stream<std::u32string::const_iterator> stream_32{ u32.cbegin(), u32.cend() };
	run_encoder(name::ISO_8859_6, stream_32, std::back_inserter(out));
	BOOST_CHECK(out.size() == 128 + 83);
	BOOST_CHECK(in == out);
}

BOOST_AUTO_TEST_CASE(whatwg_encoding__ISO_8859_7_coder)
{
	using namespace wordring::whatwg::encoding;

	std::string in{};
	std::u32string u32{};
	std::string out{};
	for (uint16_t i = 0; i <= 0xFFu; i++)
	{
		if (i == 0x80 + 46) continue;
		if (i == 0x80 + 82) continue;
		if (i == 0x80 + 127) continue;
		in.push_back(static_cast<uint8_t>(i));
	}

	ISO_8859_7_decoder decoder_0{};
	ISO_8859_7_encoder encoder_0{};

	stream<std::string::const_iterator> stream_8{ in.cbegin(), in.cend() };
	run(decoder_0, stream_8, std::back_inserter(u32));
	stream<std::u32string::const_iterator> stream_32{ u32.cbegin(), u32.cend() };
	run(encoder_0, stream_32, std::back_inserter(out));
	BOOST_CHECK(out.size() == 128 + 125);
	BOOST_CHECK(in == out);
}

BOOST_AUTO_TEST_CASE(whatwg_encoding__ISO_8859_7_run)
{
	using namespace wordring::whatwg::encoding;

	std::string in{};
	std::u32string u32{};
	std::string out{};
	for (uint16_t i = 0; i <= 0xFFu; i++)
	{
		if (i == 0x80 + 46) continue;
		if (i == 0x80 + 82) continue;
		if (i == 0x80 + 127) continue;
		in.push_back(static_cast<uint8_t>(i));
	}

	stream<std::string::const_iterator> stream_8{ in.cbegin(), in.cend() };
	run_decoder(name::ISO_8859_7, stream_8, std::back_inserter(u32));
	stream<std::u32string::const_iterator> stream_32{ u32.cbegin(), u32.cend() };
	run_encoder(name::ISO_8859_7, stream_32, std::back_inserter(out));
	BOOST_CHECK(out.size() == 128 + 125);
	BOOST_CHECK(in == out);
}

BOOST_AUTO_TEST_CASE(whatwg_encoding__ISO_8859_8_coder)
{
	using namespace wordring::whatwg::encoding;

	std::string in{};
	std::u32string u32{};
	std::string out{};
	for (uint16_t i = 0; i <= 0xFFu; i++)
	{
		if (i == 0x80 + 33) continue;
		if (0x80 + 63 <= i && i <= 0x80 + 94) continue;
		if (i == 0x80 + 123) continue;
		if (i == 0x80 + 124) continue;
		if (i == 0x80 + 127) continue;
		in.push_back(static_cast<uint8_t>(i));
	}

	ISO_8859_8_decoder decoder_0{};
	ISO_8859_8_encoder encoder_0{};

	stream<std::string::const_iterator> stream_8{ in.cbegin(), in.cend() };
	run(decoder_0, stream_8, std::back_inserter(u32));
	stream<std::u32string::const_iterator> stream_32{ u32.cbegin(), u32.cend() };
	run(encoder_0, stream_32, std::back_inserter(out));
	BOOST_CHECK(out.size() == 128 + 92);
	BOOST_CHECK(in == out);
}

BOOST_AUTO_TEST_CASE(whatwg_encoding__ISO_8859_8_run)
{
	using namespace wordring::whatwg::encoding;

	std::string in{};
	std::u32string u32{};
	std::string out{};
	for (uint16_t i = 0; i <= 0xFFu; i++)
	{
		if (i == 0x80 + 33) continue;
		if (0x80 + 63 <= i && i <= 0x80 + 94) continue;
		if (i == 0x80 + 123) continue;
		if (i == 0x80 + 124) continue;
		if (i == 0x80 + 127) continue;
		in.push_back(static_cast<uint8_t>(i));
	}

	stream<std::string::const_iterator> stream_8{ in.cbegin(), in.cend() };
	run_decoder(name::ISO_8859_8, stream_8, std::back_inserter(u32));
	stream<std::u32string::const_iterator> stream_32{ u32.cbegin(), u32.cend() };
	run_encoder(name::ISO_8859_8, stream_32, std::back_inserter(out));
	BOOST_CHECK(out.size() == 128 + 92);
	BOOST_CHECK(in == out);
}

BOOST_AUTO_TEST_CASE(whatwg_encoding__ISO_8859_10_coder)
{
	using namespace wordring::whatwg::encoding;

	std::string in{};
	std::u32string u32{};
	std::string out{};
	for (uint16_t i = 0; i <= 0xFFu; i++) in.push_back(static_cast<uint8_t>(i));

	ISO_8859_10_decoder decoder_0{};
	ISO_8859_10_encoder encoder_0{};

	stream<std::string::const_iterator> stream_8{ in.cbegin(), in.cend() };
	run(decoder_0, stream_8, std::back_inserter(u32));
	stream<std::u32string::const_iterator> stream_32{ u32.cbegin(), u32.cend() };
	run(encoder_0, stream_32, std::back_inserter(out));
	BOOST_CHECK(out.size() == 256);
	BOOST_CHECK(in == out);
}

BOOST_AUTO_TEST_CASE(whatwg_encoding__ISO_8859_10_run)
{
	using namespace wordring::whatwg::encoding;

	std::string in{};
	std::u32string u32{};
	std::string out{};
	for (uint16_t i = 0; i <= 0xFFu; i++) in.push_back(static_cast<uint8_t>(i));

	stream<std::string::const_iterator> stream_8{ in.cbegin(), in.cend() };
	run_decoder(name::ISO_8859_10, stream_8, std::back_inserter(u32));
	stream<std::u32string::const_iterator> stream_32{ u32.cbegin(), u32.cend() };
	run_encoder(name::ISO_8859_10, stream_32, std::back_inserter(out));
	BOOST_CHECK(out.size() == 256);
	BOOST_CHECK(in == out);
}

BOOST_AUTO_TEST_CASE(whatwg_encoding__ISO_8859_13_coder)
{
	using namespace wordring::whatwg::encoding;

	std::string in{};
	std::u32string u32{};
	std::string out{};
	for (uint16_t i = 0; i <= 0xFFu; i++) in.push_back(static_cast<uint8_t>(i));

	ISO_8859_13_decoder decoder_0{};
	ISO_8859_13_encoder encoder_0{};

	stream<std::string::const_iterator> stream_8{ in.cbegin(), in.cend() };
	run(decoder_0, stream_8, std::back_inserter(u32));
	stream<std::u32string::const_iterator> stream_32{ u32.cbegin(), u32.cend() };
	run(encoder_0, stream_32, std::back_inserter(out));
	BOOST_CHECK(out.size() == 256);
	BOOST_CHECK(in == out);
}

BOOST_AUTO_TEST_CASE(whatwg_encoding__ISO_8859_13_run)
{
	using namespace wordring::whatwg::encoding;

	std::string in{};
	std::u32string u32{};
	std::string out{};
	for (uint16_t i = 0; i <= 0xFFu; i++) in.push_back(static_cast<uint8_t>(i));

	stream<std::string::const_iterator> stream_8{ in.cbegin(), in.cend() };
	run_decoder(name::ISO_8859_13, stream_8, std::back_inserter(u32));
	stream<std::u32string::const_iterator> stream_32{ u32.cbegin(), u32.cend() };
	run_encoder(name::ISO_8859_13, stream_32, std::back_inserter(out));
	BOOST_CHECK(out.size() == 256);
	BOOST_CHECK(in == out);
}

BOOST_AUTO_TEST_CASE(whatwg_encoding__ISO_8859_14_coder)
{
	using namespace wordring::whatwg::encoding;

	std::string in{};
	std::u32string u32{};
	std::string out{};
	for (uint16_t i = 0; i <= 0xFFu; i++) in.push_back(static_cast<uint8_t>(i));

	ISO_8859_14_decoder decoder_0{};
	ISO_8859_14_encoder encoder_0{};

	stream<std::string::const_iterator> stream_8{ in.cbegin(), in.cend() };
	run(decoder_0, stream_8, std::back_inserter(u32));
	stream<std::u32string::const_iterator> stream_32{ u32.cbegin(), u32.cend() };
	run(encoder_0, stream_32, std::back_inserter(out));
	BOOST_CHECK(out.size() == 256);
	BOOST_CHECK(in == out);
}

BOOST_AUTO_TEST_CASE(whatwg_encoding__ISO_8859_14_run)
{
	using namespace wordring::whatwg::encoding;

	std::string in{};
	std::u32string u32{};
	std::string out{};
	for (uint16_t i = 0; i <= 0xFFu; i++) in.push_back(static_cast<uint8_t>(i));

	stream<std::string::const_iterator> stream_8{ in.cbegin(), in.cend() };
	run_decoder(name::ISO_8859_14, stream_8, std::back_inserter(u32));
	stream<std::u32string::const_iterator> stream_32{ u32.cbegin(), u32.cend() };
	run_encoder(name::ISO_8859_14, stream_32, std::back_inserter(out));
	BOOST_CHECK(out.size() == 256);
	BOOST_CHECK(in == out);
}

BOOST_AUTO_TEST_CASE(whatwg_encoding__ISO_8859_15_coder)
{
	using namespace wordring::whatwg::encoding;

	std::string in{};
	std::u32string u32{};
	std::string out{};
	for (uint16_t i = 0; i <= 0xFFu; i++) in.push_back(static_cast<uint8_t>(i));

	ISO_8859_15_decoder decoder_0{};
	ISO_8859_15_encoder encoder_0{};

	stream<std::string::const_iterator> stream_8{ in.cbegin(), in.cend() };
	run(decoder_0, stream_8, std::back_inserter(u32));
	stream<std::u32string::const_iterator> stream_32{ u32.cbegin(), u32.cend() };
	run(encoder_0, stream_32, std::back_inserter(out));
	BOOST_CHECK(out.size() == 256);
	BOOST_CHECK(in == out);
}

BOOST_AUTO_TEST_CASE(whatwg_encoding__ISO_8859_15_run)
{
	using namespace wordring::whatwg::encoding;

	std::string in{};
	std::u32string u32{};
	std::string out{};
	for (uint16_t i = 0; i <= 0xFFu; i++) in.push_back(static_cast<uint8_t>(i));

	stream<std::string::const_iterator> stream_8{ in.cbegin(), in.cend() };
	run_decoder(name::ISO_8859_15, stream_8, std::back_inserter(u32));
	stream<std::u32string::const_iterator> stream_32{ u32.cbegin(), u32.cend() };
	run_encoder(name::ISO_8859_15, stream_32, std::back_inserter(out));
	BOOST_CHECK(out.size() == 256);
	BOOST_CHECK(in == out);
}

BOOST_AUTO_TEST_CASE(whatwg_encoding__ISO_8859_16_coder)
{
	using namespace wordring::whatwg::encoding;

	std::string in{};
	std::u32string u32{};
	std::string out{};
	for (uint16_t i = 0; i <= 0xFFu; i++) in.push_back(static_cast<uint8_t>(i));

	ISO_8859_16_decoder decoder_0{};
	ISO_8859_16_encoder encoder_0{};

	stream<std::string::const_iterator> stream_8{ in.cbegin(), in.cend() };
	run(decoder_0, stream_8, std::back_inserter(u32));
	stream<std::u32string::const_iterator> stream_32{ u32.cbegin(), u32.cend() };
	run(encoder_0, stream_32, std::back_inserter(out));
	BOOST_CHECK(out.size() == 256);
	BOOST_CHECK(in == out);
}

BOOST_AUTO_TEST_CASE(whatwg_encoding__ISO_8859_16_run)
{
	using namespace wordring::whatwg::encoding;

	std::string in{};
	std::u32string u32{};
	std::string out{};
	for (uint16_t i = 0; i <= 0xFFu; i++) in.push_back(static_cast<uint8_t>(i));

	stream<std::string::const_iterator> stream_8{ in.cbegin(), in.cend() };
	run_decoder(name::ISO_8859_16, stream_8, std::back_inserter(u32));
	stream<std::u32string::const_iterator> stream_32{ u32.cbegin(), u32.cend() };
	run_encoder(name::ISO_8859_16, stream_32, std::back_inserter(out));
	BOOST_CHECK(out.size() == 256);
	BOOST_CHECK(in == out);
}

BOOST_AUTO_TEST_CASE(whatwg_encoding__KOI8_R_coder)
{
	using namespace wordring::whatwg::encoding;

	std::string in{};
	std::u32string u32{};
	std::string out{};
	for (uint16_t i = 0; i <= 0xFFu; i++) in.push_back(static_cast<uint8_t>(i));

	KOI8_R_decoder decoder_0{};
	KOI8_R_encoder encoder_0{};

	stream<std::string::const_iterator> stream_8{ in.cbegin(), in.cend() };
	run(decoder_0, stream_8, std::back_inserter(u32));
	stream<std::u32string::const_iterator> stream_32{ u32.cbegin(), u32.cend() };
	run(encoder_0, stream_32, std::back_inserter(out));
	BOOST_CHECK(out.size() == 256);
	BOOST_CHECK(in == out);
}

BOOST_AUTO_TEST_CASE(whatwg_encoding__KOI8_R_run)
{
	using namespace wordring::whatwg::encoding;

	std::string in{};
	std::u32string u32{};
	std::string out{};
	for (uint16_t i = 0; i <= 0xFFu; i++) in.push_back(static_cast<uint8_t>(i));

	stream<std::string::const_iterator> stream_8{ in.cbegin(), in.cend() };
	run_decoder(name::KOI8_R, stream_8, std::back_inserter(u32));
	stream<std::u32string::const_iterator> stream_32{ u32.cbegin(), u32.cend() };
	run_encoder(name::KOI8_R, stream_32, std::back_inserter(out));
	BOOST_CHECK(out.size() == 256);
	BOOST_CHECK(in == out);
}

BOOST_AUTO_TEST_CASE(whatwg_encoding__KOI8_U_coder)
{
	using namespace wordring::whatwg::encoding;

	std::string in{};
	std::u32string u32{};
	std::string out{};
	for (uint16_t i = 0; i <= 0xFFu; i++) in.push_back(static_cast<uint8_t>(i));

	KOI8_U_decoder decoder_0{};
	KOI8_U_encoder encoder_0{};

	stream<std::string::const_iterator> stream_8{ in.cbegin(), in.cend() };
	run(decoder_0, stream_8, std::back_inserter(u32));
	stream<std::u32string::const_iterator> stream_32{ u32.cbegin(), u32.cend() };
	run(encoder_0, stream_32, std::back_inserter(out));
	BOOST_CHECK(out.size() == 256);
	BOOST_CHECK(in == out);
}

BOOST_AUTO_TEST_CASE(whatwg_encoding__KOI8_U_run)
{
	using namespace wordring::whatwg::encoding;

	std::string in{};
	std::u32string u32{};
	std::string out{};
	for (uint16_t i = 0; i <= 0xFFu; i++) in.push_back(static_cast<uint8_t>(i));

	stream<std::string::const_iterator> stream_8{ in.cbegin(), in.cend() };
	run_decoder(name::KOI8_U, stream_8, std::back_inserter(u32));
	stream<std::u32string::const_iterator> stream_32{ u32.cbegin(), u32.cend() };
	run_encoder(name::KOI8_U, stream_32, std::back_inserter(out));
	BOOST_CHECK(out.size() == 256);
	BOOST_CHECK(in == out);
}

BOOST_AUTO_TEST_CASE(whatwg_encoding__macintosh_coder)
{
	using namespace wordring::whatwg::encoding;

	std::string in{};
	std::u32string u32{};
	std::string out{};
	for (uint16_t i = 0; i <= 0xFFu; i++) in.push_back(static_cast<uint8_t>(i));

	macintosh_decoder decoder_0{};
	macintosh_encoder encoder_0{};

	stream<std::string::const_iterator> stream_8{ in.cbegin(), in.cend() };
	run(decoder_0, stream_8, std::back_inserter(u32));
	stream<std::u32string::const_iterator> stream_32{ u32.cbegin(), u32.cend() };
	run(encoder_0, stream_32, std::back_inserter(out));
	BOOST_CHECK(out.size() == 256);
	BOOST_CHECK(in == out);
}

BOOST_AUTO_TEST_CASE(whatwg_encoding__macintosh_run)
{
	using namespace wordring::whatwg::encoding;

	std::string in{};
	std::u32string u32{};
	std::string out{};
	for (uint16_t i = 0; i <= 0xFFu; i++) in.push_back(static_cast<uint8_t>(i));

	stream<std::string::const_iterator> stream_8{ in.cbegin(), in.cend() };
	run_decoder(name::macintosh, stream_8, std::back_inserter(u32));
	stream<std::u32string::const_iterator> stream_32{ u32.cbegin(), u32.cend() };
	run_encoder(name::macintosh , stream_32, std::back_inserter(out));
	BOOST_CHECK(out.size() == 256);
	BOOST_CHECK(in == out);
}

BOOST_AUTO_TEST_CASE(whatwg_encoding__windows_874_coder)
{
	using namespace wordring::whatwg::encoding;

	std::string in{};
	std::u32string u32{};
	std::string out{};
	for (uint16_t i = 0; i <= 0xFFu; i++)
	{
		if (0x80 + 91 <= i && i <= 0x80 + 94) continue;
		if (0x80 + 124 <= i && i <= 0x80 + 127) continue;
		in.push_back(static_cast<uint8_t>(i));
	}

	windows_874_decoder decoder_0{};
	windows_874_encoder encoder_0{};

	stream<std::string::const_iterator> stream_8{ in.cbegin(), in.cend() };
	run(decoder_0, stream_8, std::back_inserter(u32));
	stream<std::u32string::const_iterator> stream_32{ u32.cbegin(), u32.cend() };
	run(encoder_0, stream_32, std::back_inserter(out));
	BOOST_CHECK(out.size() == 128 + 120);
	BOOST_CHECK(in == out);
}

BOOST_AUTO_TEST_CASE(whatwg_encoding__windows_874_run)
{
	using namespace wordring::whatwg::encoding;

	std::string in{};
	std::u32string u32{};
	std::string out{};
	for (uint16_t i = 0; i <= 0xFFu; i++)
	{
		if (0x80 + 91 <= i && i <= 0x80 + 94) continue;
		if (0x80 + 124 <= i && i <= 0x80 + 127) continue;
		in.push_back(static_cast<uint8_t>(i));
	}

	stream<std::string::const_iterator> stream_8{ in.cbegin(), in.cend() };
	run_decoder(name::windows_874, stream_8, std::back_inserter(u32));
	stream<std::u32string::const_iterator> stream_32{ u32.cbegin(), u32.cend() };
	run_encoder(name::windows_874, stream_32, std::back_inserter(out));
	BOOST_CHECK(out.size() == 128 + 120);
	BOOST_CHECK(in == out);
}

BOOST_AUTO_TEST_CASE(whatwg_encoding__windows_1250_coder)
{
	using namespace wordring::whatwg::encoding;

	std::string in{};
	std::u32string u32{};
	std::string out{};
	for (uint16_t i = 0; i <= 0xFFu; i++) in.push_back(static_cast<uint8_t>(i));

	windows_1250_decoder decoder_0{};
	windows_1250_encoder encoder_0{};

	stream<std::string::const_iterator> stream_8{ in.cbegin(), in.cend() };
	run(decoder_0, stream_8, std::back_inserter(u32));
	stream<std::u32string::const_iterator> stream_32{ u32.cbegin(), u32.cend() };
	run(encoder_0, stream_32, std::back_inserter(out));
	BOOST_CHECK(out.size() == 256);
	BOOST_CHECK(in == out);
}

BOOST_AUTO_TEST_CASE(whatwg_encoding__windows_1250_run)
{
	using namespace wordring::whatwg::encoding;

	std::string in{};
	std::u32string u32{};
	std::string out{};
	for (uint16_t i = 0; i <= 0xFFu; i++) in.push_back(static_cast<uint8_t>(i));

	stream<std::string::const_iterator> stream_8{ in.cbegin(), in.cend() };
	run_decoder(name::windows_1250, stream_8, std::back_inserter(u32));
	stream<std::u32string::const_iterator> stream_32{ u32.cbegin(), u32.cend() };
	run_encoder(name::windows_1250, stream_32, std::back_inserter(out));
	BOOST_CHECK(out.size() == 256);
	BOOST_CHECK(in == out);
}

BOOST_AUTO_TEST_CASE(whatwg_encoding__windows_1251_coder)
{
	using namespace wordring::whatwg::encoding;

	std::string in{};
	std::u32string u32{};
	std::string out{};
	for (uint16_t i = 0; i <= 0xFFu; i++) in.push_back(static_cast<uint8_t>(i));

	windows_1251_decoder decoder_0{};
	windows_1251_encoder encoder_0{};

	stream<std::string::const_iterator> stream_8{ in.cbegin(), in.cend() };
	run(decoder_0, stream_8, std::back_inserter(u32));
	stream<std::u32string::const_iterator> stream_32{ u32.cbegin(), u32.cend() };
	run(encoder_0, stream_32, std::back_inserter(out));
	BOOST_CHECK(out.size() == 256);
	BOOST_CHECK(in == out);
}

BOOST_AUTO_TEST_CASE(whatwg_encoding__windows_1251_run)
{
	using namespace wordring::whatwg::encoding;

	std::string in{};
	std::u32string u32{};
	std::string out{};
	for (uint16_t i = 0; i <= 0xFFu; i++) in.push_back(static_cast<uint8_t>(i));

	stream<std::string::const_iterator> stream_8{ in.cbegin(), in.cend() };
	run_decoder(name::windows_1251, stream_8, std::back_inserter(u32));
	stream<std::u32string::const_iterator> stream_32{ u32.cbegin(), u32.cend() };
	run_encoder(name::windows_1251, stream_32, std::back_inserter(out));
	BOOST_CHECK(out.size() == 256);
	BOOST_CHECK(in == out);
}

BOOST_AUTO_TEST_CASE(whatwg_encoding__windows_1252_coder)
{
	using namespace wordring::whatwg::encoding;

	std::string in{};
	std::u32string u32{};
	std::string out{};
	for (uint16_t i = 0; i <= 0xFFu; i++) in.push_back(static_cast<uint8_t>(i));

	windows_1252_decoder decoder_0{};
	windows_1252_encoder encoder_0{};

	stream<std::string::const_iterator> stream_8{ in.cbegin(), in.cend() };
	run(decoder_0, stream_8, std::back_inserter(u32));
	stream<std::u32string::const_iterator> stream_32{ u32.cbegin(), u32.cend() };
	run(encoder_0, stream_32, std::back_inserter(out));
	BOOST_CHECK(out.size() == 256);
	BOOST_CHECK(in == out);
}

BOOST_AUTO_TEST_CASE(whatwg_encoding__windows_1252_run)
{
	using namespace wordring::whatwg::encoding;

	std::string in{};
	std::u32string u32{};
	std::string out{};
	for (uint16_t i = 0; i <= 0xFFu; i++) in.push_back(static_cast<uint8_t>(i));

	stream<std::string::const_iterator> stream_8{ in.cbegin(), in.cend() };
	run_decoder(name::windows_1252, stream_8, std::back_inserter(u32));
	stream<std::u32string::const_iterator> stream_32{ u32.cbegin(), u32.cend() };
	run_encoder(name::windows_1252, stream_32, std::back_inserter(out));
	BOOST_CHECK(out.size() == 256);
	BOOST_CHECK(in == out);
}

BOOST_AUTO_TEST_CASE(whatwg_encoding__windows_1253_coder)
{
	using namespace wordring::whatwg::encoding;

	std::string in{};
	std::u32string u32{};
	std::string out{};
	for (uint16_t i = 0; i <= 0xFFu; i++)
	{
		if (i == 0x80 + 42) continue;
		if (i == 0x80 + 82) continue;
		if (i == 0x80 + 127) continue;
		in.push_back(static_cast<uint8_t>(i));
	}

	windows_1253_decoder decoder_0{};
	windows_1253_encoder encoder_0{};

	stream<std::string::const_iterator> stream_8{ in.cbegin(), in.cend() };
	run(decoder_0, stream_8, std::back_inserter(u32));
	stream<std::u32string::const_iterator> stream_32{ u32.cbegin(), u32.cend() };
	run(encoder_0, stream_32, std::back_inserter(out));
	BOOST_CHECK(out.size() == 128 + 125);
	BOOST_CHECK(in == out);
}

BOOST_AUTO_TEST_CASE(whatwg_encoding__windows_1253_run)
{
	using namespace wordring::whatwg::encoding;

	std::string in{};
	std::u32string u32{};
	std::string out{};
	for (uint16_t i = 0; i <= 0xFFu; i++)
	{
		if (i == 0x80 + 42) continue;
		if (i == 0x80 + 82) continue;
		if (i == 0x80 + 127) continue;
		in.push_back(static_cast<uint8_t>(i));
	}

	stream<std::string::const_iterator> stream_8{ in.cbegin(), in.cend() };
	run_decoder(name::windows_1253, stream_8, std::back_inserter(u32));
	stream<std::u32string::const_iterator> stream_32{ u32.cbegin(), u32.cend() };
	run_encoder(name::windows_1253, stream_32, std::back_inserter(out));
	BOOST_CHECK(out.size() == 128 + 125);
	BOOST_CHECK(in == out);
}

BOOST_AUTO_TEST_CASE(whatwg_encoding__windows_1254_coder)
{
	using namespace wordring::whatwg::encoding;

	std::string in{};
	std::u32string u32{};
	std::string out{};
	for (uint16_t i = 0; i <= 0xFFu; i++) in.push_back(static_cast<uint8_t>(i));

	windows_1254_decoder decoder_0{};
	windows_1254_encoder encoder_0{};

	stream<std::string::const_iterator> stream_8{ in.cbegin(), in.cend() };
	run(decoder_0, stream_8, std::back_inserter(u32));
	stream<std::u32string::const_iterator> stream_32{ u32.cbegin(), u32.cend() };
	run(encoder_0, stream_32, std::back_inserter(out));
	BOOST_CHECK(out.size() == 256);
	BOOST_CHECK(in == out);
}

BOOST_AUTO_TEST_CASE(whatwg_encoding__windows_1254_run)
{
	using namespace wordring::whatwg::encoding;

	std::string in{};
	std::u32string u32{};
	std::string out{};
	for (uint16_t i = 0; i <= 0xFFu; i++) in.push_back(static_cast<uint8_t>(i));

	stream<std::string::const_iterator> stream_8{ in.cbegin(), in.cend() };
	run_decoder(name::windows_1254, stream_8, std::back_inserter(u32));
	stream<std::u32string::const_iterator> stream_32{ u32.cbegin(), u32.cend() };
	run_encoder(name::windows_1254, stream_32, std::back_inserter(out));
	BOOST_CHECK(out.size() == 256);
	BOOST_CHECK(in == out);
}

BOOST_AUTO_TEST_CASE(whatwg_encoding__windows_1255_coder)
{
	using namespace wordring::whatwg::encoding;

	std::string in{};
	std::u32string u32{};
	std::string out{};
	for (uint16_t i = 0; i <= 0xFFu; i++)
	{
		if (0x80 + 89 <= i && i <= 0x80 + 95) continue;
		if (0x80 + 123 <= i && i <= 0x80 + 124) continue;
		if (i == 0x80 + 127) continue;
		in.push_back(static_cast<uint8_t>(i));
	}

	windows_1255_decoder decoder_0{};
	windows_1255_encoder encoder_0{};

	stream<std::string::const_iterator> stream_8{ in.cbegin(), in.cend() };
	run(decoder_0, stream_8, std::back_inserter(u32));
	stream<std::u32string::const_iterator> stream_32{ u32.cbegin(), u32.cend() };
	run(encoder_0, stream_32, std::back_inserter(out));
	BOOST_CHECK(out.size() == 128 + 118);
	BOOST_CHECK(in == out);
}

BOOST_AUTO_TEST_CASE(whatwg_encoding__windows_1255_run)
{
	using namespace wordring::whatwg::encoding;

	std::string in{};
	std::u32string u32{};
	std::string out{};
	for (uint16_t i = 0; i <= 0xFFu; i++)
	{
		if (0x80 + 89 <= i && i <= 0x80 + 95) continue;
		if (0x80 + 123 <= i && i <= 0x80 + 124) continue;
		if (i == 0x80 + 127) continue;
		in.push_back(static_cast<uint8_t>(i));
	}

	stream<std::string::const_iterator> stream_8{ in.cbegin(), in.cend() };
	run_decoder(name::windows_1255, stream_8, std::back_inserter(u32));
	stream<std::u32string::const_iterator> stream_32{ u32.cbegin(), u32.cend() };
	run_encoder(name::windows_1255, stream_32, std::back_inserter(out));
	BOOST_CHECK(out.size() == 128 + 118);
	BOOST_CHECK(in == out);
}

BOOST_AUTO_TEST_CASE(whatwg_encoding__windows_1256_coder)
{
	using namespace wordring::whatwg::encoding;

	std::string in{};
	std::u32string u32{};
	std::string out{};
	for (uint16_t i = 0; i <= 0xFFu; i++) in.push_back(static_cast<uint8_t>(i));

	windows_1256_decoder decoder_0{};
	windows_1256_encoder encoder_0{};

	stream<std::string::const_iterator> stream_8{ in.cbegin(), in.cend() };
	run(decoder_0, stream_8, std::back_inserter(u32));
	stream<std::u32string::const_iterator> stream_32{ u32.cbegin(), u32.cend() };
	run(encoder_0, stream_32, std::back_inserter(out));
	BOOST_CHECK(out.size() == 256);
	BOOST_CHECK(in == out);
}

BOOST_AUTO_TEST_CASE(whatwg_encoding__windows_1256_run)
{
	using namespace wordring::whatwg::encoding;

	std::string in{};
	std::u32string u32{};
	std::string out{};
	for (uint16_t i = 0; i <= 0xFFu; i++) in.push_back(static_cast<uint8_t>(i));

	stream<std::string::const_iterator> stream_8{ in.cbegin(), in.cend() };
	run_decoder(name::windows_1256, stream_8, std::back_inserter(u32));
	stream<std::u32string::const_iterator> stream_32{ u32.cbegin(), u32.cend() };
	run_encoder(name::windows_1256, stream_32, std::back_inserter(out));
	BOOST_CHECK(out.size() == 256);
	BOOST_CHECK(in == out);
}

BOOST_AUTO_TEST_CASE(whatwg_encoding__windows_1257_coder)
{
	using namespace wordring::whatwg::encoding;

	std::string in{};
	std::u32string u32{};
	std::string out{};
	for (uint16_t i = 0; i <= 0xFFu; i++)
	{
		if (i == 0x80 + 33) continue;
		if (i == 0x80 + 37) continue;
		in.push_back(static_cast<uint8_t>(i));
	}

	windows_1257_decoder decoder_0{};
	windows_1257_encoder encoder_0{};

	stream<std::string::const_iterator> stream_8{ in.cbegin(), in.cend() };
	run(decoder_0, stream_8, std::back_inserter(u32));
	stream<std::u32string::const_iterator> stream_32{ u32.cbegin(), u32.cend() };
	run(encoder_0, stream_32, std::back_inserter(out));
	BOOST_CHECK(out.size() == 128 + 126);
	BOOST_CHECK(in == out);
}

BOOST_AUTO_TEST_CASE(whatwg_encoding__windows_1257_run)
{
	using namespace wordring::whatwg::encoding;

	std::string in{};
	std::u32string u32{};
	std::string out{};
	for (uint16_t i = 0; i <= 0xFFu; i++)
	{
		if (i == 0x80 + 33) continue;
		if (i == 0x80 + 37) continue;
		in.push_back(static_cast<uint8_t>(i));
	}

	stream<std::string::const_iterator> stream_8{ in.cbegin(), in.cend() };
	run_decoder(name::windows_1257, stream_8, std::back_inserter(u32));
	stream<std::u32string::const_iterator> stream_32{ u32.cbegin(), u32.cend() };
	run_encoder(name::windows_1257, stream_32, std::back_inserter(out));
	BOOST_CHECK(out.size() == 128 + 126);
	BOOST_CHECK(in == out);
}

BOOST_AUTO_TEST_CASE(whatwg_encoding__windows_1258_coder)
{
	using namespace wordring::whatwg::encoding;

	std::string in{};
	std::u32string u32{};
	std::string out{};
	for (uint16_t i = 0; i <= 0xFFu; i++) in.push_back(static_cast<uint8_t>(i));

	windows_1258_decoder decoder_0{};
	windows_1258_encoder encoder_0{};

	stream<std::string::const_iterator> stream_8{ in.cbegin(), in.cend() };
	run(decoder_0, stream_8, std::back_inserter(u32));
	stream<std::u32string::const_iterator> stream_32{ u32.cbegin(), u32.cend() };
	run(encoder_0, stream_32, std::back_inserter(out));
	BOOST_CHECK(out.size() == 256);
	BOOST_CHECK(in == out);
}

BOOST_AUTO_TEST_CASE(whatwg_encoding__windows_1258_run)
{
	using namespace wordring::whatwg::encoding;

	std::string in{};
	std::u32string u32{};
	std::string out{};
	for (uint16_t i = 0; i <= 0xFFu; i++) in.push_back(static_cast<uint8_t>(i));

	stream<std::string::const_iterator> stream_8{ in.cbegin(), in.cend() };
	run_decoder(name::windows_1258, stream_8, std::back_inserter(u32));
	stream<std::u32string::const_iterator> stream_32{ u32.cbegin(), u32.cend() };
	run_encoder(name::windows_1258, stream_32, std::back_inserter(out));
	BOOST_CHECK(out.size() == 256);
	BOOST_CHECK(in == out);
}

BOOST_AUTO_TEST_CASE(whatwg_encoding__x_mac_cyrillic_coder)
{
	using namespace wordring::whatwg::encoding;

	std::string in{};
	std::u32string u32{};
	std::string out{};
	for (uint16_t i = 0; i <= 0xFFu; i++) in.push_back(static_cast<uint8_t>(i));

	x_mac_cyrillic_decoder decoder_0{};
	x_mac_cyrillic_encoder encoder_0{};

	stream<std::string::const_iterator> stream_8{ in.cbegin(), in.cend() };
	run(decoder_0, stream_8, std::back_inserter(u32));
	stream<std::u32string::const_iterator> stream_32{ u32.cbegin(), u32.cend() };
	run(encoder_0, stream_32, std::back_inserter(out));
	BOOST_CHECK(out.size() == 256);
	BOOST_CHECK(in == out);
}

BOOST_AUTO_TEST_CASE(whatwg_encoding__x_mac_cyrillic_run)
{
	using namespace wordring::whatwg::encoding;

	std::string in{};
	std::u32string u32{};
	std::string out{};
	for (uint16_t i = 0; i <= 0xFFu; i++) in.push_back(static_cast<uint8_t>(i));

	stream<std::string::const_iterator> stream_8{ in.cbegin(), in.cend() };
	run_decoder(name::x_mac_cyrillic, stream_8, std::back_inserter(u32));
	stream<std::u32string::const_iterator> stream_32{ u32.cbegin(), u32.cend() };
	run_encoder(name::x_mac_cyrillic, stream_32, std::back_inserter(out));
	BOOST_CHECK(out.size() == 256);
	BOOST_CHECK(in == out);
}

BOOST_AUTO_TEST_CASE(whatwg_encoding__gb18030_coder)
{
	using namespace wordring::whatwg::encoding;

	std::u32string in{ U"成我戒戓戔戕或戗战戙戚戛戜戝戞戟abcdefghijklmno㐀㐁㐂㐃㐄㐅㐆㐇㐈㐉㐊㐋㐌㐍㐎㐏€亐亖亗亙亜亝亞⺁⺄⺈⺋⺌⺗⺧⺪⺮⺳⺶⺷⺻⻊㖞㘚㘎⺗㥮㤘㧏㧟㩳㧐㭎㱮㳠郎凉秊裏隣兀嗀﨎﨏ⅠⅡⅢⅣⅤⅰⅱⅲⅳⅴᠠᠡᠢᠣᠤᠥᠦᠧᠨᠩꀇꀈꀉꀊꀋꀌꀍꀎꀏ༐༑༒༓༔༕༖༗༘༙بةتثجحخد狢狣狤狥狦狧狪狫㊣通桐酮瞳同铜彤童﹔﹕﹖﹗" };
	std::string str{};
	std::u32string out{};

	gb18030_encoder encoder_0{};
	gb18030_decoder decoder_0{};

	stream<std::u32string::const_iterator> stream_in{ in.cbegin(), in.cend() };
	run(encoder_0, stream_in, std::back_inserter(str));
	stream<std::string::const_iterator> stream_str{ str.cbegin(), str.cend() };
	run(decoder_0, stream_str, std::back_inserter(out));
	BOOST_CHECK(in == out);
}

BOOST_AUTO_TEST_CASE(whatwg_encoding__gb18030_run)
{
	using namespace wordring::whatwg::encoding;

	std::u32string in{ U"成我戒戓戔戕或戗战戙戚戛戜戝戞戟abcdefghijklmno㐀㐁㐂㐃㐄㐅㐆㐇㐈㐉㐊㐋㐌㐍㐎㐏€亐亖亗亙亜亝亞⺁⺄⺈⺋⺌⺗⺧⺪⺮⺳⺶⺷⺻⻊㖞㘚㘎⺗㥮㤘㧏㧟㩳㧐㭎㱮㳠郎凉秊裏隣兀嗀﨎﨏ⅠⅡⅢⅣⅤⅰⅱⅲⅳⅴᠠᠡᠢᠣᠤᠥᠦᠧᠨᠩꀇꀈꀉꀊꀋꀌꀍꀎꀏ༐༑༒༓༔༕༖༗༘༙بةتثجحخد狢狣狤狥狦狧狪狫㊣通桐酮瞳同铜彤童﹔﹕﹖﹗" };
	std::string str{};
	std::u32string out{};

	stream<std::u32string::const_iterator> stream_in{ in.cbegin(), in.cend() };
	run_encoder(name::gb18030, stream_in, std::back_inserter(str));
	stream<std::string::const_iterator> stream_str{ str.cbegin(), str.cend() };
	run_decoder(name::gb18030, stream_str, std::back_inserter(out));
	BOOST_CHECK(in == out);
}

/*
BOOST_AUTO_TEST_CASE(whatwg_encoding___coder)
{
	using namespace wordring::whatwg::encoding;

	std::string in{};
	std::u32string u32{};
	std::string out{};
	for (uint16_t i = 0; i <= 0xFFu; i++) in.push_back(static_cast<uint8_t>(i));

	run(, stream_8, std::back_inserter(u32));
	run(, stream_32, std::back_inserter(out));
	BOOST_CHECK(out.size() == 256);
	BOOST_CHECK(in == out);
}
*/

BOOST_AUTO_TEST_SUITE_END()
