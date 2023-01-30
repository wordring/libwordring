#include <boost/test/unit_test.hpp>

#include <iterator>
#include <string>

#include <wordring/compatibility.hpp>
#include <wordring/whatwg/url/infra.hpp>

BOOST_AUTO_TEST_SUITE(whatwg_url_infra_test)

BOOST_AUTO_TEST_CASE(infra_starts_with_percent_encoded_byte_1)
{
	using namespace wordring::whatwg;
	std::u32string s{ U"%E3ABC" };
	BOOST_CHECK(starts_with_percent_encoded_byte(s.begin(), s.end()));
}

BOOST_AUTO_TEST_CASE(infra_starts_with_percent_encoded_byte_2)
{
	using namespace wordring::whatwg;
	std::u32string s{ U"ABC" };
	BOOST_CHECK(starts_with_percent_encoded_byte(s.begin(), s.end()) == false);
}

BOOST_AUTO_TEST_CASE(infra_is_percent_encoded_byte_1)
{
	using namespace wordring::whatwg;
	std::u32string s{ U"%E3" };
	BOOST_CHECK(is_percent_encoded_byte(s.begin(), s.end()));
}

BOOST_AUTO_TEST_CASE(infra_is_percent_encoded_byte_2)
{
	using namespace wordring::whatwg;
	std::u32string s{ U"%E3%81%82" };
	BOOST_CHECK(is_percent_encoded_byte(s.begin(), s.end()) == false);
}

BOOST_AUTO_TEST_CASE(infra_percent_encode_1)
{
	using namespace wordring::whatwg;
	std::string out;
	percent_encode('A', std::back_inserter(out));
	BOOST_CHECK(out == "%41");
}

BOOST_AUTO_TEST_CASE(infra_percent_encode_2)
{
	using namespace wordring::whatwg;
	std::string out;
	percent_encode('\t', std::back_inserter(out));
	BOOST_CHECK(out == "%09");
}

BOOST_AUTO_TEST_CASE(infra_percent_encode_3)
{
	using namespace wordring::whatwg;
	std::u32string out;
	percent_encode('\t', std::back_inserter(out));
	BOOST_CHECK(out == U"%09");
}

BOOST_AUTO_TEST_CASE(infra_percent_encode_4)
{
	using namespace wordring::whatwg;
	std::string out;
	percent_encode(0xE3u, std::back_inserter(out));
	BOOST_CHECK(out == "%E3");
}

BOOST_AUTO_TEST_CASE(infra_percent_encode_5)
{
	using namespace wordring::whatwg;
	std::string out;
	percent_encode(0x23u, std::back_inserter(out));
	BOOST_CHECK(out == "%23");
}

BOOST_AUTO_TEST_CASE(infra_percent_encode_6)
{
	using namespace wordring::whatwg;
	std::string out;
	percent_encode(0x7Fu, std::back_inserter(out));
	BOOST_CHECK(out == "%7F");
}

BOOST_AUTO_TEST_CASE(infra_percent_decode_1)
{
	using namespace wordring::whatwg;
	std::string in{ "%E3%81%82a%E3%81%82" };
	std::u8string out{};
	percent_decode(in.begin(), in.end(), std::back_inserter(out));
	BOOST_CHECK(out == u8"あaあ");
}

BOOST_AUTO_TEST_CASE(infra_percent_decode_2)
{
	using namespace wordring::whatwg;
	std::string in{ "%25%s%1G" };
	std::string out{};
	percent_decode(in.begin(), in.end(), std::back_inserter(out));
	BOOST_CHECK(out == "%%s%1G");
}

BOOST_AUTO_TEST_CASE(infra_percent_decode_3)
{
	using namespace wordring::whatwg;
	std::string in{ "\xE2\x80\xBD%25%2E" };
	std::string out;
	percent_decode(in.begin(), in.end(), std::back_inserter(out));
	BOOST_CHECK(out == "\xE2\x80\xBD\x25\x2E");
}

BOOST_AUTO_TEST_CASE(infra_string_percent_decode_1)
{
	using namespace wordring::whatwg;
	std::string in{ "%E3%81%82a%E3%81%82" };
	std::u8string out{};
	percent_decode(in.begin(), in.end(), std::back_inserter(out));
	BOOST_CHECK(out == u8"あaあ");
}

BOOST_AUTO_TEST_CASE(infra_is_c0_control_percent_encode_set_1)
{
	using namespace wordring::whatwg;
	BOOST_CHECK(is_c0_control_percent_encode_set(0));
}

BOOST_AUTO_TEST_CASE(infra_is_fragment_percent_encode_set_1)
{
	using namespace wordring::whatwg;
	BOOST_CHECK(is_fragment_percent_encode_set(0));

	BOOST_CHECK(is_fragment_percent_encode_set(0x20u));
	BOOST_CHECK(is_fragment_percent_encode_set(0x22u));
	BOOST_CHECK(is_fragment_percent_encode_set(0x3Cu));
	BOOST_CHECK(is_fragment_percent_encode_set(0x3Eu));
	BOOST_CHECK(is_fragment_percent_encode_set(0x60u));
}

BOOST_AUTO_TEST_CASE(infra_is_path_percent_encode_set_1)
{
	using namespace wordring::whatwg;
	BOOST_CHECK(is_path_percent_encode_set(0x20u));

	BOOST_CHECK(is_path_percent_encode_set(0x23u));
	BOOST_CHECK(is_path_percent_encode_set(0x3Fu));
	BOOST_CHECK(is_path_percent_encode_set(0x7Du));
	BOOST_CHECK(is_path_percent_encode_set(0x7Bu));
}

BOOST_AUTO_TEST_CASE(infra_is_userinfo_percent_encode_set_1)
{
	using namespace wordring::whatwg;
	BOOST_CHECK(is_userinfo_percent_encode_set(0x23u));

	BOOST_CHECK(is_userinfo_percent_encode_set(0x2Fu));
	BOOST_CHECK(is_userinfo_percent_encode_set(0x3Au));
	BOOST_CHECK(is_userinfo_percent_encode_set(0x3Bu));
	BOOST_CHECK(is_userinfo_percent_encode_set(0x3Du));
	BOOST_CHECK(is_userinfo_percent_encode_set(0x40u));
	BOOST_CHECK(is_userinfo_percent_encode_set(0x5Bu));
	BOOST_CHECK(is_userinfo_percent_encode_set(0x5Cu));
	BOOST_CHECK(is_userinfo_percent_encode_set(0x5Du));
	BOOST_CHECK(is_userinfo_percent_encode_set(0x5Eu));
	BOOST_CHECK(is_userinfo_percent_encode_set(0x7Cu));
}

BOOST_AUTO_TEST_CASE(infra_percent_encode_after_encoding_1)
{
	using namespace wordring::whatwg;
	std::u32string s = percent_encode_after_encoding(encoding_name::Shift_JIS, U" ", is_userinfo_percent_encode_set);
	BOOST_CHECK(s == U"%20");
}

BOOST_AUTO_TEST_CASE(infra_percent_encode_after_encoding_2)
{
	using namespace wordring::whatwg;
	std::u32string s = percent_encode_after_encoding(encoding_name::Shift_JIS, U"≡", is_userinfo_percent_encode_set);
	BOOST_CHECK(s == U"%81%DF");
}

BOOST_AUTO_TEST_CASE(infra_percent_encode_after_encoding_3)
{
	using namespace wordring::whatwg;
	std::u32string s = percent_encode_after_encoding(encoding_name::Shift_JIS, U"‽", is_userinfo_percent_encode_set);
	BOOST_CHECK(s == U"%26%238253%3B");
}

BOOST_AUTO_TEST_CASE(infra_percent_encode_after_encoding_4)
{
	using namespace wordring::whatwg;
	std::u32string s = percent_encode_after_encoding(encoding_name::ISO_2022_JP, U"¥", is_userinfo_percent_encode_set);
	
	// WHATG の例によると %5C は \ となっているが、is_userinfo_percent_encode_set に含まれるため、%エンコードされるはず。
	BOOST_CHECK(s == U"%1B(J%5C%1B(B");
}

BOOST_AUTO_TEST_CASE(infra_percent_encode_after_encoding_5)
{
	using namespace wordring::whatwg;
	std::u32string s = percent_encode_after_encoding(encoding_name::Shift_JIS, U"1+1 ≡ 2%20‽", is_userinfo_percent_encode_set, true);
	BOOST_CHECK(s == U"1+1+%81%DF+2%20%26%238253%3B");
}

BOOST_AUTO_TEST_CASE(infra_utf8_percent_encode_1)
{
	using namespace wordring::whatwg;

	std::u32string out = utf8_percent_encode(U'\U00002261', is_userinfo_percent_encode_set);
	BOOST_CHECK(out == U"%E2%89%A1");
}

BOOST_AUTO_TEST_CASE(infra_utf8_percent_encode_2)
{
	using namespace wordring::whatwg;

	std::u32string out = utf8_percent_encode(U'\U0000203D', is_userinfo_percent_encode_set);
	BOOST_CHECK(out == U"%E2%80%BD");
}

BOOST_AUTO_TEST_CASE(infra_utf8_percent_encode_3)
{
	using namespace wordring::whatwg;

	std::u32string out = utf8_percent_encode(U"Say what‽", is_userinfo_percent_encode_set);
	BOOST_CHECK(out == U"Say%20what%E2%80%BD");
}

BOOST_AUTO_TEST_SUITE_END()
