
#include <boost/test/unit_test.hpp>

#include <algorithm>
#include <deque>
#include <string>
#include <string_view>

#include <wordring/whatwg/infra/infra.hpp>

#ifndef __cpp_lib_char8_t
using char8_t = char;
namespace std { using u8string = string; }
#endif

BOOST_AUTO_TEST_SUITE(whatwg_infra_test)

// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(whatwg_infra__isomorphic_cast)
{
}

// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(whatwg_infra__to_codepoint)
{
	std::u32string const s32_correct{ U"ABCDEあいうえお𠀋𡈽𡌛𡑮𡢽AÀⱥ𐊠" };
	std::u32string s32_result{};

	// utf-8
	std::u8string const s8{ u8"ABCDEあいうえお𠀋𡈽𡌛𡑮𡢽AÀⱥ𐊠" };
	auto it8 = s8.begin();
	while(it8 != s8.end()) s32_result.push_back(wordring::whatwg::to_codepoint(it8, s8.end()));
	BOOST_CHECK(s32_result == s32_correct);

	// utf-16
	std::u16string const s16{ u"ABCDEあいうえお𠀋𡈽𡌛𡑮𡢽AÀⱥ𐊠" };
	auto it16 = s16.begin();
	s32_result.clear();
	while (it16 != s16.end()) s32_result.push_back(wordring::whatwg::to_codepoint(it16, s16.end()));
	BOOST_CHECK(s32_result == s32_correct);

	// utf-32
	std::u32string const s32{ U"ABCDEあいうえお𠀋𡈽𡌛𡑮𡢽AÀⱥ𐊠" };
	auto it32 = s32.begin();
	s32_result.clear();
	while (it32 != s32.end()) s32_result.push_back(wordring::whatwg::to_codepoint(it32, s32.end()));
	BOOST_CHECK(s32_result == s32_correct);

	// scalar_value
	wordring::whatwg::usv_string const usv{ wordring::whatwg::make_usv_string(U"ABCDEあいうえお𠀋𡈽𡌛𡑮𡢽AÀⱥ𐊠") };
	auto it_usv = usv.begin();
	s32_result.clear();
	while (it_usv != usv.end()) s32_result.push_back(wordring::whatwg::to_codepoint(it_usv, usv.end()));
	BOOST_CHECK(s32_result == s32_correct);
}

// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(whatwg_infra__to_string)
{
	std::u32string const s32_original{ U"ABCDEあいうえお𠀋𡈽𡌛𡑮𡢽AÀⱥ𐊠" };

	// utf-8
	std::u8string const s8_correct{ u8"ABCDEあいうえお𠀋𡈽𡌛𡑮𡢽AÀⱥ𐊠" };
	std::u8string s8{};

	for (auto ch : s32_original) wordring::whatwg::to_string(ch, std::back_inserter(s8));
	BOOST_CHECK(s8 == s8_correct);

	s8.clear();
	for (auto ch : s32_original) wordring::whatwg::to_string(ch, s8);
	BOOST_CHECK(s8 == s8_correct);

	// utf-16
	std::u16string const s16_correct{ u"ABCDEあいうえお𠀋𡈽𡌛𡑮𡢽AÀⱥ𐊠" };
	std::u16string s16{};

	for (auto ch : s32_original) wordring::whatwg::to_string(ch, std::back_inserter(s16));
	BOOST_CHECK(s16 == s16_correct);

	s16.clear();
	for (auto ch : s32_original) wordring::whatwg::to_string(ch, s16);
	BOOST_CHECK(s16 == s16_correct);

	// ut-32
	std::u32string const s32_correct{ U"ABCDEあいうえお𠀋𡈽𡌛𡑮𡢽AÀⱥ𐊠" };
	std::u32string s32{};

	for (auto ch : s32_original) wordring::whatwg::to_string(ch, std::back_inserter(s32));
	BOOST_CHECK(s32 == s32_correct);

	s32.clear();
	for (auto ch : s32_original) wordring::whatwg::to_string(ch, s32);
	BOOST_CHECK(s32 == s32_correct);

	// scalar_value
	wordring::whatwg::usv_string const usv_correct{ wordring::whatwg::make_usv_string(U"ABCDEあいうえお𠀋𡈽𡌛𡑮𡢽AÀⱥ𐊠") };
	wordring::whatwg::usv_string usv{};

	for (auto ch : s32_original) wordring::whatwg::to_string(ch, std::back_inserter(usv));
	BOOST_CHECK(usv == usv_correct);

	usv.clear();
	for (auto ch : s32_original) wordring::whatwg::to_string(ch, usv);
	BOOST_CHECK(usv == usv_correct);
}

// 4.3. Bytes -----------------------------------------------------------------

BOOST_AUTO_TEST_CASE(whatwg_infra__is_ascii_byte)
{
	for (unsigned char i = 0; i <= 127; i++)
	{
		BOOST_CHECK(wordring::whatwg::is_ascii_byte(i) == true);
	}
	for (unsigned int i = 128; i <= 255; i++)
	{
		unsigned char j = static_cast<unsigned char>(i);
		BOOST_CHECK(wordring::whatwg::is_ascii_byte(j) == false);
	}
}

// 4.4. Byte sequences --------------------------------------------------------

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

	wordring::whatwg::to_byte_lowercase(mix_b.begin() + 4, mix_b.begin() + 7, mix_b.begin() + 4);
	BOOST_CHECK(mix_b == lower_b);

	wordring::whatwg::to_byte_lowercase(upper_a, std::back_inserter(result));
	BOOST_CHECK(result == lower_b);

	result.clear();
	wordring::whatwg::to_byte_lowercase(upper, std::back_inserter(result));
	BOOST_CHECK(result == lower_b);

	result = wordring::whatwg::to_byte_lowercase(upper_b);
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

	wordring::whatwg::to_byte_uppercase(mix_b.begin() + 4, mix_b.begin() + 7, mix_b.begin() + 4);
	BOOST_CHECK(mix_b == upper_b);

	wordring::whatwg::to_byte_uppercase(lower_a, std::back_inserter(result));
	BOOST_CHECK(result == upper_b);

	result.clear();
	wordring::whatwg::to_byte_uppercase(lower, std::back_inserter(result));
	BOOST_CHECK(result == upper_b);

	result = wordring::whatwg::to_byte_uppercase(lower_b);
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

	BOOST_CHECK(wordring::whatwg::is_byte_case_insensitive_match(lower_a.begin(), lower_a.end(), upper_b.begin(), upper_b.end()) == true);

	BOOST_CHECK(wordring::whatwg::is_byte_case_insensitive_match(lower_a, upper_b) == true);

	BOOST_CHECK(wordring::whatwg::is_byte_case_insensitive_match(lower_a, upper) == true);

	BOOST_CHECK(wordring::whatwg::is_byte_case_insensitive_match(lower, upper_b) == true);

	BOOST_CHECK(wordring::whatwg::is_byte_case_insensitive_match(lower, fail_b) == false);
}

BOOST_AUTO_TEST_CASE(whatwg_infra__isomorphic_decode)
{
	char const * lower = "abcdefghijklmnopqrstuvwxyz0123456789";
	std::string_view lower_a{ lower };
	std::deque<char> lower_b{ lower_a.begin(), lower_a.end() };
	
	std::deque<char32_t> result{};

	wordring::whatwg::isomorphic_decode(lower_b.begin(), lower_b.end(), std::back_inserter(result));
	BOOST_CHECK(std::equal(lower_b.begin(), lower_b.end(), result.begin(), result.end()));

	result.clear();
	wordring::whatwg::isomorphic_decode(lower_b.begin(), lower_b.end(), result);
	BOOST_CHECK(std::equal(lower_b.begin(), lower_b.end(), result.begin(), result.end()));
}

// 4.5. Code points -----------------------------------------------------------

BOOST_AUTO_TEST_CASE(whatwg_infra__is_surrogate)
{
	BOOST_CHECK(wordring::whatwg::is_surrogate(U'\u0000') == false);
	BOOST_CHECK(wordring::whatwg::is_surrogate(U'\uD7FF') == false);

	BOOST_CHECK(wordring::whatwg::is_surrogate(static_cast<char32_t>(0xD800)) == true);
	BOOST_CHECK(wordring::whatwg::is_surrogate(static_cast<char32_t>(0xDFFF)) == true);

	BOOST_CHECK(wordring::whatwg::is_surrogate(U'\uE000') == false);
	BOOST_CHECK(wordring::whatwg::is_surrogate(static_cast<char32_t>(0xFFFFFFFF)) == false);
}

BOOST_AUTO_TEST_CASE(whatwg_infra__is_scalar_value)
{
	BOOST_CHECK(wordring::whatwg::is_scalar_value(U'\u0000') == true);
	BOOST_CHECK(wordring::whatwg::is_scalar_value(U'\uD7FF') == true);

	BOOST_CHECK(wordring::whatwg::is_scalar_value(static_cast<char32_t>(0xD800)) == false);
	BOOST_CHECK(wordring::whatwg::is_scalar_value(static_cast<char32_t>(0xDFFF)) == false);

	BOOST_CHECK(wordring::whatwg::is_scalar_value(U'\uE000') == true);
	BOOST_CHECK(wordring::whatwg::is_scalar_value(static_cast<char32_t>(0xFFFFFFFF)) == true);
}

BOOST_AUTO_TEST_CASE(whatwg_infra__is_noncharacter)
{
	BOOST_CHECK(wordring::whatwg::is_noncharacter(U'\uFDD0') == true);
	BOOST_CHECK(wordring::whatwg::is_noncharacter(U'\uFDEF') == true);
}

BOOST_AUTO_TEST_CASE(whatwg_infra__is_ascii_char)
{
	BOOST_CHECK(wordring::whatwg::is_ascii_char(U'\u0000') == true);
	BOOST_CHECK(wordring::whatwg::is_ascii_char(U'\u007F') == true);

	BOOST_CHECK(wordring::whatwg::is_ascii_char(U'\u0080') == false);
	BOOST_CHECK(wordring::whatwg::is_ascii_char(static_cast<char32_t>(0xFFFFFFFF)) == false);
}

BOOST_AUTO_TEST_CASE(whatwg_infra__is_ascii_tab_or_newline)
{
	BOOST_CHECK(wordring::whatwg::is_ascii_tab_or_newline(U'\u0009') == true);
	BOOST_CHECK(wordring::whatwg::is_ascii_tab_or_newline(U'\u000A') == true);
	BOOST_CHECK(wordring::whatwg::is_ascii_tab_or_newline(U'\u000D') == true);

	BOOST_CHECK(wordring::whatwg::is_ascii_tab_or_newline(U'a') == false);
}

BOOST_AUTO_TEST_CASE(whatwg_infra__is_ascii_white_space)
{
	BOOST_CHECK(wordring::whatwg::is_ascii_white_space(U'\u0009') == true);
	BOOST_CHECK(wordring::whatwg::is_ascii_white_space(U'\u000A') == true);
	BOOST_CHECK(wordring::whatwg::is_ascii_white_space(U'\u000C') == true);
	BOOST_CHECK(wordring::whatwg::is_ascii_white_space(U'\u000D') == true);
	BOOST_CHECK(wordring::whatwg::is_ascii_white_space(U'\u0020') == true);

	BOOST_CHECK(wordring::whatwg::is_ascii_white_space(U'a') == false);
}

BOOST_AUTO_TEST_CASE(whatwg_infra__is_c0_control)
{
	BOOST_CHECK(wordring::whatwg::is_c0_control(U'\u0000') == true);
	BOOST_CHECK(wordring::whatwg::is_c0_control(U'\u001F') == true);

	BOOST_CHECK(wordring::whatwg::is_c0_control(U'\u0020') == false);
	BOOST_CHECK(wordring::whatwg::is_c0_control(static_cast<char32_t>(0xFFFFFFFF)) == false);
}

BOOST_AUTO_TEST_CASE(whatwg_infra__is_ascii_digit)
{
	BOOST_CHECK(wordring::whatwg::is_ascii_digit(U'\u0000') == false);
	BOOST_CHECK(wordring::whatwg::is_ascii_digit(U'\u0029') == false);

	BOOST_CHECK(wordring::whatwg::is_ascii_digit(U'0') == true);
	BOOST_CHECK(wordring::whatwg::is_ascii_digit(U'9') == true);

	BOOST_CHECK(wordring::whatwg::is_ascii_digit(U'\u003A') == false);
	BOOST_CHECK(wordring::whatwg::is_ascii_digit(static_cast<char32_t>(0xFFFFFFFF)) == false);
}

BOOST_AUTO_TEST_CASE(whatwg_infra__is_ascii_upper_hex_digit)
{
	BOOST_CHECK(wordring::whatwg::is_ascii_upper_hex_digit(U'\u0000') == false);

	BOOST_CHECK(wordring::whatwg::is_ascii_upper_hex_digit(U'\u0029') == false);
	BOOST_CHECK(wordring::whatwg::is_ascii_upper_hex_digit(U'0') == true);
	BOOST_CHECK(wordring::whatwg::is_ascii_upper_hex_digit(U'9') == true);
	BOOST_CHECK(wordring::whatwg::is_ascii_upper_hex_digit(U'\u003A') == false);

	BOOST_CHECK(wordring::whatwg::is_ascii_upper_hex_digit(U'\u0040') == false);
	BOOST_CHECK(wordring::whatwg::is_ascii_upper_hex_digit(U'A') == true);
	BOOST_CHECK(wordring::whatwg::is_ascii_upper_hex_digit(U'F') == true);
	BOOST_CHECK(wordring::whatwg::is_ascii_upper_hex_digit(U'\u0047') == false);

	BOOST_CHECK(wordring::whatwg::is_ascii_upper_hex_digit(static_cast<char32_t>(0xFFFFFFFF)) == false);
}

BOOST_AUTO_TEST_CASE(whatwg_infra__is_ascii_lower_hex_digit)
{
	BOOST_CHECK(wordring::whatwg::is_ascii_lower_hex_digit(U'\u0000') == false);

	BOOST_CHECK(wordring::whatwg::is_ascii_lower_hex_digit(U'\u0029') == false);
	BOOST_CHECK(wordring::whatwg::is_ascii_lower_hex_digit(U'0') == true);
	BOOST_CHECK(wordring::whatwg::is_ascii_lower_hex_digit(U'9') == true);
	BOOST_CHECK(wordring::whatwg::is_ascii_lower_hex_digit(U'\u003A') == false);

	BOOST_CHECK(wordring::whatwg::is_ascii_lower_hex_digit(U'\u0060') == false);
	BOOST_CHECK(wordring::whatwg::is_ascii_lower_hex_digit(U'a') == true);
	BOOST_CHECK(wordring::whatwg::is_ascii_lower_hex_digit(U'f') == true);
	BOOST_CHECK(wordring::whatwg::is_ascii_lower_hex_digit(U'\u0067') == false);

	BOOST_CHECK(wordring::whatwg::is_ascii_lower_hex_digit(static_cast<char32_t>(0xFFFFFFFF)) == false);
}

BOOST_AUTO_TEST_CASE(whatwg_infra__is_ascii_hex_digit)
{
	BOOST_CHECK(wordring::whatwg::is_ascii_hex_digit(U'\u0000') == false);

	BOOST_CHECK(wordring::whatwg::is_ascii_hex_digit(U'\u0029') == false);
	BOOST_CHECK(wordring::whatwg::is_ascii_hex_digit(U'0') == true);
	BOOST_CHECK(wordring::whatwg::is_ascii_hex_digit(U'9') == true);
	BOOST_CHECK(wordring::whatwg::is_ascii_hex_digit(U'\u003A') == false);

	BOOST_CHECK(wordring::whatwg::is_ascii_hex_digit(U'\u0040') == false);
	BOOST_CHECK(wordring::whatwg::is_ascii_hex_digit(U'A') == true);
	BOOST_CHECK(wordring::whatwg::is_ascii_hex_digit(U'F') == true);
	BOOST_CHECK(wordring::whatwg::is_ascii_hex_digit(U'\u0047') == false);

	BOOST_CHECK(wordring::whatwg::is_ascii_hex_digit(U'\u0060') == false);
	BOOST_CHECK(wordring::whatwg::is_ascii_hex_digit(U'a') == true);
	BOOST_CHECK(wordring::whatwg::is_ascii_hex_digit(U'f') == true);
	BOOST_CHECK(wordring::whatwg::is_ascii_hex_digit(U'\u0067') == false);

	BOOST_CHECK(wordring::whatwg::is_ascii_hex_digit(static_cast<char32_t>(0xFFFFFFFF)) == false);
}

BOOST_AUTO_TEST_CASE(whatwg_infra__is_ascii_upper_alpha)
{
	BOOST_CHECK(wordring::whatwg::is_ascii_upper_alpha(U'\u0000') == false);
	BOOST_CHECK(wordring::whatwg::is_ascii_upper_alpha(U'\u0040') == false);

	BOOST_CHECK(wordring::whatwg::is_ascii_upper_alpha(U'A') == true);
	BOOST_CHECK(wordring::whatwg::is_ascii_upper_alpha(U'Z') == true);

	BOOST_CHECK(wordring::whatwg::is_ascii_upper_alpha(U'\u005B') == false);
	BOOST_CHECK(wordring::whatwg::is_ascii_upper_alpha(static_cast<char32_t>(0xFFFFFFFF)) == false);
}

BOOST_AUTO_TEST_CASE(whatwg_infra__is_ascii_lower_alpha)
{
	BOOST_CHECK(wordring::whatwg::is_ascii_lower_alpha(U'\u0000') == false);
	BOOST_CHECK(wordring::whatwg::is_ascii_lower_alpha(U'\u0060') == false);

	BOOST_CHECK(wordring::whatwg::is_ascii_lower_alpha(U'a') == true);
	BOOST_CHECK(wordring::whatwg::is_ascii_lower_alpha(U'z') == true);

	BOOST_CHECK(wordring::whatwg::is_ascii_lower_alpha(U'\u007B') == false);
	BOOST_CHECK(wordring::whatwg::is_ascii_lower_alpha(static_cast<char32_t>(0xFFFFFFFF)) == false);
}

BOOST_AUTO_TEST_CASE(whatwg_infra__is_ascii_alpha)
{
	BOOST_CHECK(wordring::whatwg::is_ascii_alpha(U'\u0000') == false);

	BOOST_CHECK(wordring::whatwg::is_ascii_alpha(U'\u0040') == false);
	BOOST_CHECK(wordring::whatwg::is_ascii_alpha(U'A') == true);
	BOOST_CHECK(wordring::whatwg::is_ascii_alpha(U'Z') == true);
	BOOST_CHECK(wordring::whatwg::is_ascii_alpha(U'\u005B') == false);

	BOOST_CHECK(wordring::whatwg::is_ascii_alpha(U'\u0060') == false);
	BOOST_CHECK(wordring::whatwg::is_ascii_alpha(U'a') == true);
	BOOST_CHECK(wordring::whatwg::is_ascii_alpha(U'z') == true);
	BOOST_CHECK(wordring::whatwg::is_ascii_alpha(U'\u007B') == false);

	BOOST_CHECK(wordring::whatwg::is_ascii_alpha(static_cast<char32_t>(0xFFFFFFFF)) == false);
}

BOOST_AUTO_TEST_CASE(whatwg_infra__is_ascii_alphanumeric)
{
	BOOST_CHECK(wordring::whatwg::is_ascii_alphanumeric(U'\u0000') == false);

	BOOST_CHECK(wordring::whatwg::is_ascii_alphanumeric(U'\u0029') == false);
	BOOST_CHECK(wordring::whatwg::is_ascii_alphanumeric(U'0') == true);
	BOOST_CHECK(wordring::whatwg::is_ascii_alphanumeric(U'9') == true);
	BOOST_CHECK(wordring::whatwg::is_ascii_alphanumeric(U'\u003A') == false);

	BOOST_CHECK(wordring::whatwg::is_ascii_alphanumeric(U'\u0040') == false);
	BOOST_CHECK(wordring::whatwg::is_ascii_alphanumeric(U'A') == true);
	BOOST_CHECK(wordring::whatwg::is_ascii_alphanumeric(U'Z') == true);
	BOOST_CHECK(wordring::whatwg::is_ascii_alphanumeric(U'\u005B') == false);

	BOOST_CHECK(wordring::whatwg::is_ascii_alphanumeric(U'\u0060') == false);
	BOOST_CHECK(wordring::whatwg::is_ascii_alphanumeric(U'a') == true);
	BOOST_CHECK(wordring::whatwg::is_ascii_alphanumeric(U'z') == true);
	BOOST_CHECK(wordring::whatwg::is_ascii_alphanumeric(U'\u007B') == false);

	BOOST_CHECK(wordring::whatwg::is_ascii_alphanumeric(static_cast<char32_t>(0xFFFFFFFF)) == false);
}

// 4.6. Strings ---------------------------------------------------------------

BOOST_AUTO_TEST_CASE(whatwg_infra__isomorphic_encode)
{
	std::u32string a1{ U"abcdefg" };
	std::string b1{ "abcdefg" };
	std::vector<char> o1;

	wordring::whatwg::isomorphic_encode(a1.begin(), a1.end(), std::back_inserter(o1));
	BOOST_CHECK(std::equal(b1.begin(), b1.end(), o1.begin(), o1.end()) == true);

	o1.clear();
	wordring::whatwg::isomorphic_encode(b1.begin(), b1.end(), std::back_inserter(o1));
	BOOST_CHECK(std::equal(b1.begin(), b1.end(), o1.begin(), o1.end()) == true);

	o1.clear();
	wordring::whatwg::isomorphic_encode(a1.begin(), a1.end(), o1);
	BOOST_CHECK(std::equal(a1.begin(), a1.end(), o1.begin(), o1.end()) == true);

}

BOOST_AUTO_TEST_CASE(whatwg_infra__is_ascii_string)
{
	// utf-8
	std::u8string const s8_true{ u8"abcdefg" };
	std::u8string const s8_false{ u8"abcdefgあいうえお" };
	BOOST_CHECK(wordring::whatwg::is_ascii_string(s8_true.begin(), s8_true.end()) == true);
	BOOST_CHECK(wordring::whatwg::is_ascii_string(s8_false.begin(), s8_false.end()) == false);

	BOOST_CHECK(wordring::whatwg::is_ascii_string(s8_true) == true);
	BOOST_CHECK(wordring::whatwg::is_ascii_string(s8_false) == false);

	// utf-16
	std::u16string const s16_true{ u"abcdefg" };
	std::u16string const s16_false{ u"abcdefgあいうえお" };
	BOOST_CHECK(wordring::whatwg::is_ascii_string(s16_true.begin(), s16_true.end()) == true);
	BOOST_CHECK(wordring::whatwg::is_ascii_string(s16_false.begin(), s16_false.end()) == false);

	BOOST_CHECK(wordring::whatwg::is_ascii_string(s16_true) == true);
	BOOST_CHECK(wordring::whatwg::is_ascii_string(s16_false) == false);

	// utf-32
	std::u32string const s32_true{ U"abcdefg" };
	std::u32string const s32_false{ U"abcdefgあいうえお" };
	BOOST_CHECK(wordring::whatwg::is_ascii_string(s32_true.begin(), s32_true.end()) == true);
	BOOST_CHECK(wordring::whatwg::is_ascii_string(s32_false.begin(), s32_false.end()) == false);

	BOOST_CHECK(wordring::whatwg::is_ascii_string(s32_true) == true);
	BOOST_CHECK(wordring::whatwg::is_ascii_string(s32_false) == false);

	// scalar value
	wordring::whatwg::usv_string const usv_true{ wordring::whatwg::make_usv_string(U"abcdefg") };
	wordring::whatwg::usv_string const usv_false{ wordring::whatwg::make_usv_string(U"abcdefgあいうえお") };
	BOOST_CHECK(wordring::whatwg::is_ascii_string(usv_true.begin(), usv_true.end()) == true);
	BOOST_CHECK(wordring::whatwg::is_ascii_string(usv_false.begin(), usv_false.end()) == false);

	BOOST_CHECK(wordring::whatwg::is_ascii_string(usv_true) == true);
	BOOST_CHECK(wordring::whatwg::is_ascii_string(usv_false) == false);
}

BOOST_AUTO_TEST_CASE(whatwg_infra__to_ascii_lowercase)
{
	// utf-8
	std::u8string const s8_original{ u8"ABCDEあいうえお𠀋𡈽𡌛𡑮𡢽Àⱥ𐊠" };
	std::u8string const s8_correct{ u8"abcdeあいうえお𠀋𡈽𡌛𡑮𡢽Àⱥ𐊠" };
	std::u8string s8_result{};
	wordring::whatwg::to_ascii_lowercase(s8_original.begin(), s8_original.end(), std::back_inserter(s8_result));
	BOOST_CHECK(s8_correct == s8_result);

	// utf-16
	std::u16string const s16_original{ u"ABCDEあいうえお𠀋𡈽𡌛𡑮𡢽Àⱥ𐊠" };
	std::u16string const s16_correct{ u"abcdeあいうえお𠀋𡈽𡌛𡑮𡢽Àⱥ𐊠" };
	std::u16string s16_result{};
	wordring::whatwg::to_ascii_lowercase(s16_original.begin(), s16_original.end(), std::back_inserter(s16_result));
	BOOST_CHECK(s16_correct == s16_result);

	// utf-32
	std::u32string const s32_original{ U"ABCDEあいうえお𠀋𡈽𡌛𡑮𡢽Àⱥ𐊠" };
	std::u32string const s32_correct{ U"abcdeあいうえお𠀋𡈽𡌛𡑮𡢽Àⱥ𐊠" };
	std::u32string s32_result{};
	wordring::whatwg::to_ascii_lowercase(s32_original.begin(), s32_original.end(), std::back_inserter(s32_result));
	BOOST_CHECK(s32_correct == s32_result);

	// scalar value
	wordring::whatwg::usv_string const usv_original{ wordring::whatwg::make_usv_string(U"ABCDEあいうえお𠀋𡈽𡌛𡑮𡢽Àⱥ𐊠") };
	wordring::whatwg::usv_string const usv_correct{ wordring::whatwg::make_usv_string(U"abcdeあいうえお𠀋𡈽𡌛𡑮𡢽Àⱥ𐊠") };
	wordring::whatwg::usv_string usv_result{};
	wordring::whatwg::to_ascii_lowercase(usv_original.begin(), usv_original.end(), std::back_inserter(usv_result));
	BOOST_CHECK(usv_correct == usv_result);
}

BOOST_AUTO_TEST_CASE(whatwg_infra__to_ascii_uppercase)
{
	// utf-8
	std::u8string const s8_original{ u8"abcdeあいうえお𠀋𡈽𡌛𡑮𡢽Àⱥ𐊠" };
	std::u8string const s8_correct{ u8"ABCDEあいうえお𠀋𡈽𡌛𡑮𡢽Àⱥ𐊠" };
	std::u8string s8_result{};
	wordring::whatwg::to_ascii_uppercase(s8_original.begin(), s8_original.end(), std::back_inserter(s8_result));
	BOOST_CHECK(s8_correct == s8_result);

	// utf-16
	std::u16string const s16_original{ u"abcdeあいうえお𠀋𡈽𡌛𡑮𡢽Àⱥ𐊠" };
	std::u16string const s16_correct{ u"ABCDEあいうえお𠀋𡈽𡌛𡑮𡢽Àⱥ𐊠" };
	std::u16string s16_result{};
	wordring::whatwg::to_ascii_uppercase(s16_original.begin(), s16_original.end(), std::back_inserter(s16_result));
	BOOST_CHECK(s16_correct == s16_result);

	// utf-32
	std::u32string const s32_original{ U"abcdeあいうえお𠀋𡈽𡌛𡑮𡢽Àⱥ𐊠" };
	std::u32string const s32_correct{ U"ABCDEあいうえお𠀋𡈽𡌛𡑮𡢽Àⱥ𐊠" };
	std::u32string s32_result{};
	wordring::whatwg::to_ascii_uppercase(s32_original.begin(), s32_original.end(), std::back_inserter(s32_result));
	BOOST_CHECK(s32_correct == s32_result);

	// scalar value
	wordring::whatwg::usv_string const usv_original{ wordring::whatwg::make_usv_string(U"abcdeあいうえお𠀋𡈽𡌛𡑮𡢽Àⱥ𐊠") };
	wordring::whatwg::usv_string const usv_correct{ wordring::whatwg::make_usv_string(U"ABCDEあいうえお𠀋𡈽𡌛𡑮𡢽Àⱥ𐊠") };
	wordring::whatwg::usv_string usv_result{};
	wordring::whatwg::to_ascii_uppercase(usv_original.begin(), usv_original.end(), std::back_inserter(usv_result));
	BOOST_CHECK(usv_correct == usv_result);
}

BOOST_AUTO_TEST_CASE(whatwg_infra__is_ascii_case_insensitive_match)
{
	// utf-8
	std::u8string const s8_original{ u8"ABCDEあいうえお𠀋𡈽𡌛𡑮𡢽Àⱥ𐊠" };
	std::u8string const s8_correct{ u8"abcdeあいうえお𠀋𡈽𡌛𡑮𡢽Àⱥ𐊠" };
	std::u8string const s8_incorrect_1{ u8"abcdeあいうえお𠀋𡈽𡌛𡑮𡢽Àⱥ𐊠abcde" };
	std::u8string const s8_incorrect_2{ u8"12345あいうえお𠀋𡈽𡌛𡑮𡢽Àⱥ𐊠" };
	BOOST_CHECK(wordring::whatwg::is_ascii_case_insensitive_match(s8_original.begin(), s8_original.end(), s8_correct.begin(), s8_correct.end()) == true);
	BOOST_CHECK(wordring::whatwg::is_ascii_case_insensitive_match(s8_original.begin(), s8_original.end(), s8_incorrect_1.begin(), s8_incorrect_1.end()) == false);
	BOOST_CHECK(wordring::whatwg::is_ascii_case_insensitive_match(s8_original.begin(), s8_original.end(), s8_incorrect_2.begin(), s8_incorrect_2.end()) == false);
}

BOOST_AUTO_TEST_CASE(whatwg_infra__ascii_encode)
{
	std::u16string const s16_original{ u"ABCDE" };
	std::u16string const s16_correct{ u"ABCDE" };
	std::u8string s8_result{};
	wordring::whatwg::ascii_encode(s16_original.begin(), s16_original.end(), std::back_inserter(s8_result));
	BOOST_CHECK(std::equal(s16_correct.begin(), s16_correct.end(), s8_result.begin(), s8_result.end()));
}

BOOST_AUTO_TEST_CASE(whatwg_infra__ascii_decode)
{
	std::string const s_original{ "ABCDE" };
	std::string const s_correct{ "ABCDE" };
	std::u16string s16_result{};
	wordring::whatwg::ascii_decode(s_original.begin(), s_original.end(), std::back_inserter(s16_result));
	BOOST_CHECK(std::equal(s_correct.begin(), s_correct.end(), s16_result.begin(), s16_result.end()));
}

BOOST_AUTO_TEST_CASE(whatwg_infra__to_strip_newlines)
{
	std::u32string const s32_original{ U"ABCDE\nあいうえ\r\nお𠀋𡈽𡌛\r𡑮𡢽Àⱥ𐊠" };
	std::u32string const s32_correct{ U"ABCDEあいうえお𠀋𡈽𡌛𡑮𡢽Àⱥ𐊠" };
	std::u32string s32_result{};
	wordring::whatwg::to_strip_newlines(s32_original.begin(), s32_original.end(), std::back_inserter(s32_result));
	BOOST_CHECK(s32_result == s32_correct);
}

BOOST_AUTO_TEST_CASE(whatwg_infra__to_normalize_newlines)
{
	std::u32string const s32_original{ U"A\rA\nA\r\n\n\n\r\r\n" };
	std::u32string const s32_correct{ U"A\nA\nA\n\n\n\n\n" };
	std::u32string s32_result{};
	wordring::whatwg::to_normalize_newlines(s32_original.begin(), s32_original.end(), std::back_inserter(s32_result));
	BOOST_CHECK(s32_result == s32_correct);
}

/*
BOOST_AUTO_TEST_CASE(whatwg_infra__)
{
	BOOST_CHECK(wordring::whatwg::() == true);
}
BOOST_AUTO_TEST_CASE(whatwg_infra__)
{
	BOOST_CHECK(wordring::whatwg::() == true);
}
BOOST_AUTO_TEST_CASE(whatwg_infra__)
{
	BOOST_CHECK(wordring::whatwg::() == true);
}
*/

BOOST_AUTO_TEST_SUITE_END()
