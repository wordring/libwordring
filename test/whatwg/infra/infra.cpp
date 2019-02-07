
#include <boost/test/unit_test.hpp>

#include <algorithm>
#include <deque>
#include <string_view>

#include <wordring/whatwg/infra/infra.hpp>

BOOST_AUTO_TEST_SUITE(whatwg_infra_test)

// 4.3. Bytes -------------------------------------------------------------

BOOST_AUTO_TEST_CASE(whatwg_infra__is_ascii_byte)
{
	for (unsigned char i = 0; i <= 127; i++)
	{
		BOOST_CHECK(wordring::whatwg::infra::is_ascii_byte(i) == true);
	}
	for (unsigned int i = 128; i <= 255; i++)
	{
		unsigned char j = static_cast<unsigned char>(i);
		BOOST_CHECK(wordring::whatwg::infra::is_ascii_byte(j) == false);
	}
}

// 4.4. Byte sequences ----------------------------------------------------

BOOST_AUTO_TEST_CASE(whatwg_infra__to_byte_lowercase)
{
	char const * lower = "abcdefghijklmnopqrstuvwxyz0123456789";
	char const * upper = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	char const * mix = "abcdEFGhijklmnopqrstuvwxyz0123456789";

	std::string_view lower_a{ lower };
	std::string_view upper_a{ upper };
	std::string_view mix_a{ mix };

	std::deque<char> lower_b{ lower_a.begin(), lower_a.end() };
	std::deque<char> upper_b{ upper_a.begin(), upper_a.end() };
	std::deque<char> mix_b{ mix_a.begin(), mix_a.end() };

	std::deque<char> result{};

	wordring::whatwg::infra::to_byte_lowercase(mix_b.begin() + 4, mix_b.begin() + 7, mix_b.begin() + 4);
	BOOST_CHECK(mix_b == lower_b);

	wordring::whatwg::infra::to_byte_lowercase(upper_a, std::back_inserter(result));
	BOOST_CHECK(result == lower_b);

	result.clear();
	wordring::whatwg::infra::to_byte_lowercase(upper, std::back_inserter(result));
	BOOST_CHECK(result == lower_b);

	result = wordring::whatwg::infra::to_byte_lowercase(upper_b);
	BOOST_CHECK(result == lower_b);
}

BOOST_AUTO_TEST_CASE(whatwg_infra__to_byte_uppercase)
{
	char const * lower = "abcdefghijklmnopqrstuvwxyz0123456789";
	char const * upper = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	char const * mix = "ABCDefgHIJKLMNOPQRSTUVWXYZ0123456789";

	std::string_view lower_a{ lower };
	std::string_view upper_a{ upper };
	std::string_view mix_a{ mix };

	std::deque<char> lower_b{ lower_a.begin(), lower_a.end() };
	std::deque<char> upper_b{ upper_a.begin(), upper_a.end() };
	std::deque<char> mix_b{ mix_a.begin(), mix_a.end() };

	std::deque<char> result{};

	wordring::whatwg::infra::to_byte_uppercase(mix_b.begin() + 4, mix_b.begin() + 7, mix_b.begin() + 4);
	BOOST_CHECK(mix_b == upper_b);

	wordring::whatwg::infra::to_byte_uppercase(lower_a, std::back_inserter(result));
	BOOST_CHECK(result == upper_b);

	result.clear();
	wordring::whatwg::infra::to_byte_uppercase(lower, std::back_inserter(result));
	BOOST_CHECK(result == upper_b);

	result = wordring::whatwg::infra::to_byte_uppercase(lower_b);
	BOOST_CHECK(result == upper_b);
}

BOOST_AUTO_TEST_CASE(whatwg_infra__is_byte_case_insensitive_match)
{
	char const * lower = "abcdefghijklmnopqrstuvwxyz0123456789";
	char const * upper = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	char const * fail = "ABCD";

	std::string_view lower_a{ lower };
	std::string_view upper_a{ upper };
	std::string_view fail_a{ fail };

	std::deque<char> lower_b{ lower_a.begin(), lower_a.end() };
	std::deque<char> upper_b{ upper_a.begin(), upper_a.end() };
	std::deque<char> fail_b{ fail_a.begin(), fail_a.end() };

	BOOST_CHECK(wordring::whatwg::infra::is_byte_case_insensitive_match(lower_a.begin(), lower_a.end(), upper_b.begin(), upper_b.end()) == true);

	BOOST_CHECK(wordring::whatwg::infra::is_byte_case_insensitive_match(lower_a, upper_b) == true);

	BOOST_CHECK(wordring::whatwg::infra::is_byte_case_insensitive_match(lower_a, upper) == true);

	BOOST_CHECK(wordring::whatwg::infra::is_byte_case_insensitive_match(lower, upper_b) == true);

	BOOST_CHECK(wordring::whatwg::infra::is_byte_case_insensitive_match(lower, fail_b) == false);
}

BOOST_AUTO_TEST_CASE(whatwg_infra__to_isomorphic_decode)
{
	char const * lower = "abcdefghijklmnopqrstuvwxyz0123456789";
	std::string_view lower_a{ lower };
	std::deque<char> lower_b{ lower_a.begin(), lower_a.end() };
	
	std::deque<char32_t> result{};

	wordring::whatwg::infra::to_isomorphic_decode(lower_b.begin(), lower_b.end(), std::back_inserter(result));

	BOOST_CHECK(std::equal(lower_b.begin(), lower_b.end(), result.begin(), result.end()));
}

// 4.5. Code points -------------------------------------------------------

BOOST_AUTO_TEST_CASE(whatwg_infra__is_surrogate)
{
	BOOST_CHECK(wordring::whatwg::infra::is_surrogate(U'\u0000') == false);
	BOOST_CHECK(wordring::whatwg::infra::is_surrogate(U'\uD7FF') == false);

	BOOST_CHECK(wordring::whatwg::infra::is_surrogate(static_cast<char32_t>(0xD800)) == true);
	BOOST_CHECK(wordring::whatwg::infra::is_surrogate(static_cast<char32_t>(0xDFFF)) == true);

	BOOST_CHECK(wordring::whatwg::infra::is_surrogate(U'\uE000') == false);
	BOOST_CHECK(wordring::whatwg::infra::is_surrogate(static_cast<char32_t>(0xFFFFFFFF)) == false);
}

BOOST_AUTO_TEST_CASE(whatwg_infra__is_scalar_value)
{
	BOOST_CHECK(wordring::whatwg::infra::is_scalar_value(U'\u0000') == true);
	BOOST_CHECK(wordring::whatwg::infra::is_scalar_value(U'\uD7FF') == true);

	BOOST_CHECK(wordring::whatwg::infra::is_scalar_value(static_cast<char32_t>(0xD800)) == false);
	BOOST_CHECK(wordring::whatwg::infra::is_scalar_value(static_cast<char32_t>(0xDFFF)) == false);

	BOOST_CHECK(wordring::whatwg::infra::is_scalar_value(U'\uE000') == true);
	BOOST_CHECK(wordring::whatwg::infra::is_scalar_value(static_cast<char32_t>(0xFFFFFFFF)) == true);
}

BOOST_AUTO_TEST_CASE(whatwg_infra__is_noncharacter)
{
	BOOST_CHECK(wordring::whatwg::infra::is_noncharacter(U'\uFDD0') == true);
	BOOST_CHECK(wordring::whatwg::infra::is_noncharacter(U'\uFDEF') == true);
}

BOOST_AUTO_TEST_CASE(whatwg_infra__is_ascii_char)
{
	BOOST_CHECK(wordring::whatwg::infra::is_ascii_char(U'\u0000') == true);
	BOOST_CHECK(wordring::whatwg::infra::is_ascii_char(U'\u007F') == true);

	BOOST_CHECK(wordring::whatwg::infra::is_ascii_char(U'\u0080') == false);
	BOOST_CHECK(wordring::whatwg::infra::is_ascii_char(static_cast<char32_t>(0xFFFFFFFF)) == false);
}

BOOST_AUTO_TEST_CASE(whatwg_infra__is_ascii_tab_or_newline)
{
	BOOST_CHECK(wordring::whatwg::infra::is_ascii_tab_or_newline(U'\u0009') == true);
	BOOST_CHECK(wordring::whatwg::infra::is_ascii_tab_or_newline(U'\u000A') == true);
	BOOST_CHECK(wordring::whatwg::infra::is_ascii_tab_or_newline(U'\u000D') == true);

	BOOST_CHECK(wordring::whatwg::infra::is_ascii_tab_or_newline(U'a') == false);
}

BOOST_AUTO_TEST_CASE(whatwg_infra__is_ascii_white_space)
{
	BOOST_CHECK(wordring::whatwg::infra::is_ascii_white_space(U'\u0009') == true);
	BOOST_CHECK(wordring::whatwg::infra::is_ascii_white_space(U'\u000A') == true);
	BOOST_CHECK(wordring::whatwg::infra::is_ascii_white_space(U'\u000C') == true);
	BOOST_CHECK(wordring::whatwg::infra::is_ascii_white_space(U'\u000D') == true);
	BOOST_CHECK(wordring::whatwg::infra::is_ascii_white_space(U'\u0020') == true);

	BOOST_CHECK(wordring::whatwg::infra::is_ascii_white_space(U'a') == false);
}

BOOST_AUTO_TEST_CASE(whatwg_infra__is_c0_control)
{
	BOOST_CHECK(wordring::whatwg::infra::is_c0_control(U'\u0000') == true);
	BOOST_CHECK(wordring::whatwg::infra::is_c0_control(U'\u001F') == true);

	BOOST_CHECK(wordring::whatwg::infra::is_c0_control(U'\u0020') == false);
	BOOST_CHECK(wordring::whatwg::infra::is_c0_control(static_cast<char32_t>(0xFFFFFFFF)) == false);
}

BOOST_AUTO_TEST_CASE(whatwg_infra__is_ascii_digit)
{
	BOOST_CHECK(wordring::whatwg::infra::is_ascii_digit(U'\u0000') == false);
	BOOST_CHECK(wordring::whatwg::infra::is_ascii_digit(U'\u0029') == false);

	BOOST_CHECK(wordring::whatwg::infra::is_ascii_digit(U'0') == true);
	BOOST_CHECK(wordring::whatwg::infra::is_ascii_digit(U'9') == true);

	BOOST_CHECK(wordring::whatwg::infra::is_ascii_digit(U'\u003A') == false);
	BOOST_CHECK(wordring::whatwg::infra::is_ascii_digit(static_cast<char32_t>(0xFFFFFFFF)) == false);
}

BOOST_AUTO_TEST_CASE(whatwg_infra__is_ascii_upper_hex_digit)
{
	BOOST_CHECK(wordring::whatwg::infra::is_ascii_upper_hex_digit(U'\u0000') == false);

	BOOST_CHECK(wordring::whatwg::infra::is_ascii_upper_hex_digit(U'\u0029') == false);
	BOOST_CHECK(wordring::whatwg::infra::is_ascii_upper_hex_digit(U'0') == true);
	BOOST_CHECK(wordring::whatwg::infra::is_ascii_upper_hex_digit(U'9') == true);
	BOOST_CHECK(wordring::whatwg::infra::is_ascii_upper_hex_digit(U'\u003A') == false);

	BOOST_CHECK(wordring::whatwg::infra::is_ascii_upper_hex_digit(U'\u0040') == false);
	BOOST_CHECK(wordring::whatwg::infra::is_ascii_upper_hex_digit(U'A') == true);
	BOOST_CHECK(wordring::whatwg::infra::is_ascii_upper_hex_digit(U'F') == true);
	BOOST_CHECK(wordring::whatwg::infra::is_ascii_upper_hex_digit(U'\u0047') == false);

	BOOST_CHECK(wordring::whatwg::infra::is_ascii_upper_hex_digit(static_cast<char32_t>(0xFFFFFFFF)) == false);
}

BOOST_AUTO_TEST_CASE(whatwg_infra__is_ascii_lower_hex_digit)
{
	BOOST_CHECK(wordring::whatwg::infra::is_ascii_lower_hex_digit(U'\u0000') == false);

	BOOST_CHECK(wordring::whatwg::infra::is_ascii_lower_hex_digit(U'\u0029') == false);
	BOOST_CHECK(wordring::whatwg::infra::is_ascii_lower_hex_digit(U'0') == true);
	BOOST_CHECK(wordring::whatwg::infra::is_ascii_lower_hex_digit(U'9') == true);
	BOOST_CHECK(wordring::whatwg::infra::is_ascii_lower_hex_digit(U'\u003A') == false);

	BOOST_CHECK(wordring::whatwg::infra::is_ascii_lower_hex_digit(U'\u0060') == false);
	BOOST_CHECK(wordring::whatwg::infra::is_ascii_lower_hex_digit(U'a') == true);
	BOOST_CHECK(wordring::whatwg::infra::is_ascii_lower_hex_digit(U'f') == true);
	BOOST_CHECK(wordring::whatwg::infra::is_ascii_lower_hex_digit(U'\u0067') == false);

	BOOST_CHECK(wordring::whatwg::infra::is_ascii_lower_hex_digit(static_cast<char32_t>(0xFFFFFFFF)) == false);
}

BOOST_AUTO_TEST_CASE(whatwg_infra__is_ascii_hex_digit)
{
	BOOST_CHECK(wordring::whatwg::infra::is_ascii_hex_digit(U'\u0000') == false);

	BOOST_CHECK(wordring::whatwg::infra::is_ascii_hex_digit(U'\u0029') == false);
	BOOST_CHECK(wordring::whatwg::infra::is_ascii_hex_digit(U'0') == true);
	BOOST_CHECK(wordring::whatwg::infra::is_ascii_hex_digit(U'9') == true);
	BOOST_CHECK(wordring::whatwg::infra::is_ascii_hex_digit(U'\u003A') == false);

	BOOST_CHECK(wordring::whatwg::infra::is_ascii_hex_digit(U'\u0040') == false);
	BOOST_CHECK(wordring::whatwg::infra::is_ascii_hex_digit(U'A') == true);
	BOOST_CHECK(wordring::whatwg::infra::is_ascii_hex_digit(U'F') == true);
	BOOST_CHECK(wordring::whatwg::infra::is_ascii_hex_digit(U'\u0047') == false);

	BOOST_CHECK(wordring::whatwg::infra::is_ascii_hex_digit(U'\u0060') == false);
	BOOST_CHECK(wordring::whatwg::infra::is_ascii_hex_digit(U'a') == true);
	BOOST_CHECK(wordring::whatwg::infra::is_ascii_hex_digit(U'f') == true);
	BOOST_CHECK(wordring::whatwg::infra::is_ascii_hex_digit(U'\u0067') == false);

	BOOST_CHECK(wordring::whatwg::infra::is_ascii_hex_digit(static_cast<char32_t>(0xFFFFFFFF)) == false);
}

BOOST_AUTO_TEST_CASE(whatwg_infra__is_ascii_upper_alpha)
{
	BOOST_CHECK(wordring::whatwg::infra::is_ascii_upper_alpha(U'\u0000') == false);
	BOOST_CHECK(wordring::whatwg::infra::is_ascii_upper_alpha(U'\u0040') == false);

	BOOST_CHECK(wordring::whatwg::infra::is_ascii_upper_alpha(U'A') == true);
	BOOST_CHECK(wordring::whatwg::infra::is_ascii_upper_alpha(U'Z') == true);

	BOOST_CHECK(wordring::whatwg::infra::is_ascii_upper_alpha(U'\u005B') == false);
	BOOST_CHECK(wordring::whatwg::infra::is_ascii_upper_alpha(static_cast<char32_t>(0xFFFFFFFF)) == false);
}

BOOST_AUTO_TEST_CASE(whatwg_infra__is_ascii_lower_alpha)
{
	BOOST_CHECK(wordring::whatwg::infra::is_ascii_lower_alpha(U'\u0000') == false);
	BOOST_CHECK(wordring::whatwg::infra::is_ascii_lower_alpha(U'\u0060') == false);

	BOOST_CHECK(wordring::whatwg::infra::is_ascii_lower_alpha(U'a') == true);
	BOOST_CHECK(wordring::whatwg::infra::is_ascii_lower_alpha(U'z') == true);

	BOOST_CHECK(wordring::whatwg::infra::is_ascii_lower_alpha(U'\u007B') == false);
	BOOST_CHECK(wordring::whatwg::infra::is_ascii_lower_alpha(static_cast<char32_t>(0xFFFFFFFF)) == false);
}

BOOST_AUTO_TEST_CASE(whatwg_infra__is_ascii_alpha)
{
	BOOST_CHECK(wordring::whatwg::infra::is_ascii_alpha(U'\u0000') == false);

	BOOST_CHECK(wordring::whatwg::infra::is_ascii_alpha(U'\u0040') == false);
	BOOST_CHECK(wordring::whatwg::infra::is_ascii_alpha(U'A') == true);
	BOOST_CHECK(wordring::whatwg::infra::is_ascii_alpha(U'Z') == true);
	BOOST_CHECK(wordring::whatwg::infra::is_ascii_alpha(U'\u005B') == false);

	BOOST_CHECK(wordring::whatwg::infra::is_ascii_alpha(U'\u0060') == false);
	BOOST_CHECK(wordring::whatwg::infra::is_ascii_alpha(U'a') == true);
	BOOST_CHECK(wordring::whatwg::infra::is_ascii_alpha(U'z') == true);
	BOOST_CHECK(wordring::whatwg::infra::is_ascii_alpha(U'\u007B') == false);

	BOOST_CHECK(wordring::whatwg::infra::is_ascii_alpha(static_cast<char32_t>(0xFFFFFFFF)) == false);
}

BOOST_AUTO_TEST_CASE(whatwg_infra__is_ascii_alphanumeric)
{
	BOOST_CHECK(wordring::whatwg::infra::is_ascii_alphanumeric(U'\u0000') == false);

	BOOST_CHECK(wordring::whatwg::infra::is_ascii_alphanumeric(U'\u0029') == false);
	BOOST_CHECK(wordring::whatwg::infra::is_ascii_alphanumeric(U'0') == true);
	BOOST_CHECK(wordring::whatwg::infra::is_ascii_alphanumeric(U'9') == true);
	BOOST_CHECK(wordring::whatwg::infra::is_ascii_alphanumeric(U'\u003A') == false);

	BOOST_CHECK(wordring::whatwg::infra::is_ascii_alphanumeric(U'\u0040') == false);
	BOOST_CHECK(wordring::whatwg::infra::is_ascii_alphanumeric(U'A') == true);
	BOOST_CHECK(wordring::whatwg::infra::is_ascii_alphanumeric(U'Z') == true);
	BOOST_CHECK(wordring::whatwg::infra::is_ascii_alphanumeric(U'\u005B') == false);

	BOOST_CHECK(wordring::whatwg::infra::is_ascii_alphanumeric(U'\u0060') == false);
	BOOST_CHECK(wordring::whatwg::infra::is_ascii_alphanumeric(U'a') == true);
	BOOST_CHECK(wordring::whatwg::infra::is_ascii_alphanumeric(U'z') == true);
	BOOST_CHECK(wordring::whatwg::infra::is_ascii_alphanumeric(U'\u007B') == false);

	BOOST_CHECK(wordring::whatwg::infra::is_ascii_alphanumeric(static_cast<char32_t>(0xFFFFFFFF)) == false);
}

// 4.6. Strings -----------------------------------------------------------

BOOST_AUTO_TEST_CASE(whatwg_infra__to_isomorphic_encode)
{
	std::u32string a1{ U"abcdefg" };
	std::string b1{ "abcdefg" };
	std::vector<char> o1;

	wordring::whatwg::infra::to_isomorphic_encode(a1.begin(), a1.end(), std::back_inserter(o1));
	BOOST_CHECK(std::equal(b1.begin(), b1.end(), o1.begin(), o1.end()) == true);

	o1.clear();
	wordring::whatwg::infra::to_isomorphic_encode(b1.begin(), b1.end(), std::back_inserter(o1));
	BOOST_CHECK(std::equal(b1.begin(), b1.end(), o1.begin(), o1.end()) == true);
}

BOOST_AUTO_TEST_CASE(whatwg_infra__is_ascii_string)
{
	std::u32string a1{ U"abcdefg" };
	std::u32string a2{ U"あいうえお" };
	std::string b1{ "abcdefg" };
	std::string b2{ u8"あいうえお" };

	BOOST_CHECK(wordring::whatwg::infra::is_ascii_string(a1.begin(), a1.end()) == true);
	BOOST_CHECK(wordring::whatwg::infra::is_ascii_string(a2.begin(), a2.end()) == false);

	BOOST_CHECK(wordring::whatwg::infra::is_ascii_string(b1.begin(), b1.end()) == true);
	BOOST_CHECK(wordring::whatwg::infra::is_ascii_string(b2.begin(), b2.end()) == false);
}

/*
BOOST_AUTO_TEST_CASE(whatwg_infra__)
{
	BOOST_CHECK(wordring::whatwg::infra::() == true);
}
BOOST_AUTO_TEST_CASE(whatwg_infra__)
{
	BOOST_CHECK(wordring::whatwg::infra::() == true);
}
BOOST_AUTO_TEST_CASE(whatwg_infra__)
{
	BOOST_CHECK(wordring::whatwg::infra::() == true);
}
BOOST_AUTO_TEST_CASE(whatwg_infra__)
{
	BOOST_CHECK(wordring::whatwg::infra::() == true);
}
*/

BOOST_AUTO_TEST_SUITE_END()
