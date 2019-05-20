
#include <boost/test/unit_test.hpp>

#include <iterator>
#include <string>

#include <wordring/whatwg/encoding/coder.hpp>
#include <wordring/whatwg/encoding/encoding.hpp>
#include <wordring/whatwg/encoding/stream.hpp>

BOOST_AUTO_TEST_SUITE(whatwg_encoding__coder_test)

BOOST_AUTO_TEST_CASE(whatwg_encoding__UTF_8_decoder)
{
	namespace e = wordring::whatwg::encoding;

	std::u32string correct{ U"ABCDEあいうえお𠀋𡈽𡌛𡑮𡢽AÀⱥ𐊠" };
	std::string in{ u8"ABCDEあいうえお𠀋𡈽𡌛𡑮𡢽AÀⱥ𐊠" };
	std::u32string out{};

	e::run(e::UTF_8_decoder{}, e::stream<std::string::iterator>{ in.begin(), in.end() }, std::back_inserter(out));
	BOOST_CHECK(out == correct);
}

BOOST_AUTO_TEST_CASE(whatwg_encoding__UTF_8_encoder)
{
	namespace e = wordring::whatwg::encoding;

	std::string correct{ u8"ABCDEあいうえお𠀋𡈽𡌛𡑮𡢽AÀⱥ𐊠" };
	std::u32string in{ U"ABCDEあいうえお𠀋𡈽𡌛𡑮𡢽AÀⱥ𐊠" };
	std::string out{};

	e::run(e::UTF_8_encoder{}, e::stream<std::u32string::iterator>{ in.begin(), in.end() }, std::back_inserter(out));
	BOOST_CHECK(out == correct);
}

BOOST_AUTO_TEST_CASE(whatwg_encoding__single_byte_decoder)
{
	namespace e = wordring::whatwg::encoding;

	std::u32string correct{ U"ABCDE€ÿ" };
	std::string in{ "ABCDE\x80\xFF" };
	std::u32string out{};

	e::run(e::single_byte_decoder<e::index_code_point_windows_1252>{}, e::stream<std::string::iterator>{ in.begin(), in.end() }, std::back_inserter(out));
	BOOST_CHECK(out == correct);
}

BOOST_AUTO_TEST_CASE(whatwg_encoding__single_byte_encoder)
{
	namespace e = wordring::whatwg::encoding;

	std::string correct{ "ABCDE\x80\xFF" };
	std::u32string in{ U"ABCDE€ÿ" };
	std::string out{};

	e::run(e::single_byte_encoder<e::index_pointer_windows_1252_0, e::index_pointer_windows_1252_1>{}, e::stream<std::u32string::iterator>{ in.begin(), in.end() }, std::back_inserter(out));
	BOOST_CHECK(out == correct);
}

BOOST_AUTO_TEST_CASE(whatwg_encoding__IBM866_coder)
{
	using namespace wordring::whatwg::encoding;

	std::string in{};
	std::u32string u32{};
	std::string out{};
	for (uint16_t i = 0; i <= 0xFFu; i++) in.push_back(static_cast<uint8_t>(i));

	run(IBM866_decoder{}, stream<std::string::iterator>{ in.begin(), in.end() }, std::back_inserter(u32));
	run(IBM866_encoder{}, stream<std::u32string::iterator>{ u32.begin(), u32.end() }, std::back_inserter(out));
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

	run(ISO_8859_2_decoder{}, stream<std::string::iterator>{ in.begin(), in.end() }, std::back_inserter(u32));
	run(ISO_8859_2_encoder{}, stream<std::u32string::iterator>{ u32.begin(), u32.end() }, std::back_inserter(out));
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

	run(ISO_8859_3_decoder{}, stream<std::string::iterator>{ in.begin(), in.end() }, std::back_inserter(u32));
	run(ISO_8859_3_encoder{}, stream<std::u32string::iterator>{ u32.begin(), u32.end() }, std::back_inserter(out));
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

	run(ISO_8859_4_decoder{}, stream<std::string::iterator>{ in.begin(), in.end() }, std::back_inserter(u32));
	run(ISO_8859_4_encoder{}, stream<std::u32string::iterator>{ u32.begin(), u32.end() }, std::back_inserter(out));
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

	run(ISO_8859_5_decoder{}, stream<std::string::iterator>{ in.begin(), in.end() }, std::back_inserter(u32));
	run(ISO_8859_5_encoder{}, stream<std::u32string::iterator>{ u32.begin(), u32.end() }, std::back_inserter(out));
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

	run(ISO_8859_6_decoder{}, stream<std::string::iterator>{ in.begin(), in.end() }, std::back_inserter(u32));
	run(ISO_8859_6_encoder{}, stream<std::u32string::iterator>{ u32.begin(), u32.end() }, std::back_inserter(out));
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

	run(ISO_8859_7_decoder{}, stream<std::string::iterator>{ in.begin(), in.end() }, std::back_inserter(u32));
	run(ISO_8859_7_encoder{}, stream<std::u32string::iterator>{ u32.begin(), u32.end() }, std::back_inserter(out));
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

	run(ISO_8859_8_decoder{}, stream<std::string::iterator>{ in.begin(), in.end() }, std::back_inserter(u32));
	run(ISO_8859_8_encoder{}, stream<std::u32string::iterator>{ u32.begin(), u32.end() }, std::back_inserter(out));
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

	run(ISO_8859_10_decoder{}, stream<std::string::iterator>{ in.begin(), in.end() }, std::back_inserter(u32));
	run(ISO_8859_10_encoder{}, stream<std::u32string::iterator>{ u32.begin(), u32.end() }, std::back_inserter(out));
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

	run(ISO_8859_13_decoder{}, stream<std::string::iterator>{ in.begin(), in.end() }, std::back_inserter(u32));
	run(ISO_8859_13_encoder{}, stream<std::u32string::iterator>{ u32.begin(), u32.end() }, std::back_inserter(out));
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

	run(ISO_8859_14_decoder{}, stream<std::string::iterator>{ in.begin(), in.end() }, std::back_inserter(u32));
	run(ISO_8859_14_encoder{}, stream<std::u32string::iterator>{ u32.begin(), u32.end() }, std::back_inserter(out));
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

	run(ISO_8859_15_decoder{}, stream<std::string::iterator>{ in.begin(), in.end() }, std::back_inserter(u32));
	run(ISO_8859_15_encoder{}, stream<std::u32string::iterator>{ u32.begin(), u32.end() }, std::back_inserter(out));
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

	run(ISO_8859_16_decoder{}, stream<std::string::iterator>{ in.begin(), in.end() }, std::back_inserter(u32));
	run(ISO_8859_16_encoder{}, stream<std::u32string::iterator>{ u32.begin(), u32.end() }, std::back_inserter(out));
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

	run(KOI8_R_decoder{}, stream<std::string::iterator>{ in.begin(), in.end() }, std::back_inserter(u32));
	run(KOI8_R_encoder{}, stream<std::u32string::iterator>{ u32.begin(), u32.end() }, std::back_inserter(out));
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

	run(KOI8_U_decoder{}, stream<std::string::iterator>{ in.begin(), in.end() }, std::back_inserter(u32));
	run(KOI8_U_encoder{}, stream<std::u32string::iterator>{ u32.begin(), u32.end() }, std::back_inserter(out));
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

	run(macintosh_decoder{}, stream<std::string::iterator>{ in.begin(), in.end() }, std::back_inserter(u32));
	run(macintosh_encoder{}, stream<std::u32string::iterator>{ u32.begin(), u32.end() }, std::back_inserter(out));
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

	run(windows_874_decoder{}, stream<std::string::iterator>{ in.begin(), in.end() }, std::back_inserter(u32));
	run(windows_874_encoder{}, stream<std::u32string::iterator>{ u32.begin(), u32.end() }, std::back_inserter(out));
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

	run(windows_1250_decoder{}, stream<std::string::iterator>{ in.begin(), in.end() }, std::back_inserter(u32));
	run(windows_1250_encoder{}, stream<std::u32string::iterator>{ u32.begin(), u32.end() }, std::back_inserter(out));
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

	run(windows_1251_decoder{}, stream<std::string::iterator>{ in.begin(), in.end() }, std::back_inserter(u32));
	run(windows_1251_encoder{}, stream<std::u32string::iterator>{ u32.begin(), u32.end() }, std::back_inserter(out));
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

	run(windows_1252_decoder{}, stream<std::string::iterator>{ in.begin(), in.end() }, std::back_inserter(u32));
	run(windows_1252_encoder{}, stream<std::u32string::iterator>{ u32.begin(), u32.end() }, std::back_inserter(out));
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

	run(windows_1253_decoder{}, stream<std::string::iterator>{ in.begin(), in.end() }, std::back_inserter(u32));
	run(windows_1253_encoder{}, stream<std::u32string::iterator>{ u32.begin(), u32.end() }, std::back_inserter(out));
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

	run(windows_1254_decoder{}, stream<std::string::iterator>{ in.begin(), in.end() }, std::back_inserter(u32));
	run(windows_1254_encoder{}, stream<std::u32string::iterator>{ u32.begin(), u32.end() }, std::back_inserter(out));
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

	run(windows_1255_decoder{}, stream<std::string::iterator>{ in.begin(), in.end() }, std::back_inserter(u32));
	run(windows_1255_encoder{}, stream<std::u32string::iterator>{ u32.begin(), u32.end() }, std::back_inserter(out));
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

	run(windows_1256_decoder{}, stream<std::string::iterator>{ in.begin(), in.end() }, std::back_inserter(u32));
	run(windows_1256_encoder{}, stream<std::u32string::iterator>{ u32.begin(), u32.end() }, std::back_inserter(out));
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

	run(windows_1257_decoder{}, stream<std::string::iterator>{ in.begin(), in.end() }, std::back_inserter(u32));
	run(windows_1257_encoder{}, stream<std::u32string::iterator>{ u32.begin(), u32.end() }, std::back_inserter(out));
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

	run(windows_1258_decoder{}, stream<std::string::iterator>{ in.begin(), in.end() }, std::back_inserter(u32));
	run(windows_1258_encoder{}, stream<std::u32string::iterator>{ u32.begin(), u32.end() }, std::back_inserter(out));
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

	run(x_mac_cyrillic_decoder{}, stream<std::string::iterator>{ in.begin(), in.end() }, std::back_inserter(u32));
	run(x_mac_cyrillic_encoder{}, stream<std::u32string::iterator>{ u32.begin(), u32.end() }, std::back_inserter(out));
	BOOST_CHECK(out.size() == 256);
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

	run({}, stream<std::string::iterator>{ in.begin(), in.end() }, std::back_inserter(u32));
	run({}, stream<std::u32string::iterator>{ u32.begin(), u32.end() }, std::back_inserter(out));
	BOOST_CHECK(out.size() == 256);
	BOOST_CHECK(in == out);
}
*/

BOOST_AUTO_TEST_SUITE_END()
