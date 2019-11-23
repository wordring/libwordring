#include <boost/test/unit_test.hpp>

#include <iterator>
#include <string>

#include <wordring/compatibility.hpp>
#include <wordring/whatwg/url/infra.hpp>

BOOST_AUTO_TEST_SUITE(whatwg_url__infra_test)

BOOST_AUTO_TEST_CASE(starts_with_percent_encoded_byte__1)
{
	using namespace wordring::whatwg;
	std::u32string s{ U"%E3ABC" };
	BOOST_CHECK(starts_with_percent_encoded_byte(s.begin(), s.end()));
}

BOOST_AUTO_TEST_CASE(starts_with_percent_encoded_byte__2)
{
	using namespace wordring::whatwg;
	std::u32string s{ U"ABC" };
	BOOST_CHECK(starts_with_percent_encoded_byte(s.begin(), s.end()) == false);
}

BOOST_AUTO_TEST_CASE(is_percent_encoded_byte__1)
{
	using namespace wordring::whatwg;
	std::u32string s{ U"%E3" };
	BOOST_CHECK(is_percent_encoded_byte(s.begin(), s.end()));
}

BOOST_AUTO_TEST_CASE(is_percent_encoded_byte__2)
{
	using namespace wordring::whatwg;
	std::u32string s{ U"%E3%81%82" };
	BOOST_CHECK(is_percent_encoded_byte(s.begin(), s.end()) == false);
}

BOOST_AUTO_TEST_CASE(percent_encode__1)
{
	using namespace wordring::whatwg;
	std::string out{};
	percent_encode('A', std::back_inserter(out));
	BOOST_CHECK(out == "%41");
}

BOOST_AUTO_TEST_CASE(percent_encode__2)
{
	using namespace wordring::whatwg;
	std::string out{};
	percent_encode('\t', std::back_inserter(out));
	BOOST_CHECK(out == "%09");
}

BOOST_AUTO_TEST_CASE(percent_encode__3)
{
	using namespace wordring::whatwg;
	std::u32string out{};
	percent_encode('\t', std::back_inserter(out));
	BOOST_CHECK(out == U"%09");
}

BOOST_AUTO_TEST_CASE(percent_encode__4)
{
	using namespace wordring::whatwg;
	std::string out{};
	percent_encode(0xE3u, std::back_inserter(out));
	BOOST_CHECK(out == "%E3");
}

BOOST_AUTO_TEST_CASE(percent_decode__1)
{
	using namespace wordring::whatwg;
	std::string in{ "%E3%81%82a%E3%81%82" };
	std::string out{};
	percent_decode(in.begin(), in.end(), std::back_inserter(out));
	BOOST_CHECK(out == u8"あaあ");
}

BOOST_AUTO_TEST_CASE(string_percent_decode__1)
{
	using namespace wordring::whatwg;
	std::string in{ "%E3%81%82a%E3%81%82" };
	std::string out{};
	percent_decode(in.begin(), in.end(), std::back_inserter(out));
	BOOST_CHECK(out == u8"あaあ");
}

BOOST_AUTO_TEST_CASE(is_c0_control_percent_encode_set__1)
{
	using namespace wordring::whatwg;
	BOOST_CHECK(is_c0_control_percent_encode_set(0));
}

BOOST_AUTO_TEST_CASE(is_fragment_percent_encode_set__1)
{
	using namespace wordring::whatwg;
	BOOST_CHECK(is_fragment_percent_encode_set(0));

	BOOST_CHECK(is_fragment_percent_encode_set(0x20u));
	BOOST_CHECK(is_fragment_percent_encode_set(0x22u));
	BOOST_CHECK(is_fragment_percent_encode_set(0x3Cu));
	BOOST_CHECK(is_fragment_percent_encode_set(0x3Eu));
	BOOST_CHECK(is_fragment_percent_encode_set(0x60u));
}

BOOST_AUTO_TEST_CASE(is_path_percent_encode_set__1)
{
	using namespace wordring::whatwg;
	BOOST_CHECK(is_path_percent_encode_set(0x20u));

	BOOST_CHECK(is_path_percent_encode_set(0x23u));
	BOOST_CHECK(is_path_percent_encode_set(0x3Fu));
	BOOST_CHECK(is_path_percent_encode_set(0x7Du));
	BOOST_CHECK(is_path_percent_encode_set(0x7Bu));
}

BOOST_AUTO_TEST_CASE(is_userinfo_percent_encode_set__1)
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

BOOST_AUTO_TEST_CASE(utf_8_percent_encode__1)
{
	using namespace wordring::whatwg;

	std::string out{};
	utf_8_percent_encode(U'あ', std::back_inserter(out), is_c0_control_percent_encode_set);
	BOOST_CHECK(out == "%E3%81%82");

}

BOOST_AUTO_TEST_SUITE_END()
