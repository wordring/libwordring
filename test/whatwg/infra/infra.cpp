
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


// 4.3. Bytes -----------------------------------------------------------------

BOOST_AUTO_TEST_CASE(is_ascii_byte__1)
{
	using namespace wordring::whatwg;

	for (unsigned char i = 0; i <= 127; i++) BOOST_CHECK(is_ascii_byte(i) == true);
	for (unsigned int i = 128; i <= 255; i++) BOOST_CHECK(is_ascii_byte(static_cast<unsigned char>(i)) == false);
}

// 4.4. Byte sequences --------------------------------------------------------

BOOST_AUTO_TEST_CASE(to_byte_lowercase__1)
{
	using namespace wordring::whatwg;

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

	to_byte_lowercase(mix_b.begin() + 4, mix_b.begin() + 7, mix_b.begin() + 4);
	BOOST_CHECK(mix_b == lower_b);
}

BOOST_AUTO_TEST_CASE(to_byte_uppercase__1)
{
	using namespace wordring::whatwg;

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

	to_byte_uppercase(mix_b.begin() + 4, mix_b.begin() + 7, mix_b.begin() + 4);
	BOOST_CHECK(mix_b == upper_b);
}

BOOST_AUTO_TEST_CASE(is_byte_case_insensitive_match__1)
{
	using namespace wordring::whatwg;

	char const * lower = "abcdefghijklmnopqrstuvwxyz0123456789";
	char const * upper = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	char const * fail = "ABCD";

	std::string_view lower_a{ lower };
	std::string_view upper_a{ upper };
	std::string_view fail_a{ fail };

	std::deque<char> lower_b{ lower_a.begin(), lower_a.end() };
	std::deque<char> upper_b{ upper_a.begin(), upper_a.end() };
	std::deque<char> fail_b{ fail_a.begin(), fail_a.end() };

	BOOST_CHECK(is_byte_case_insensitive_match(lower_a.begin(), lower_a.end(), upper_b.begin(), upper_b.end()) == true);
}

BOOST_AUTO_TEST_CASE(isomorphic_decode__1)
{
	using namespace wordring::whatwg;

	char const * lower = "abcdefghijklmnopqrstuvwxyz0123456789";
	std::string_view lower_a{ lower };
	std::deque<char> lower_b{ lower_a.begin(), lower_a.end() };
	
	std::deque<char32_t> result{};

	isomorphic_decode(lower_b.begin(), lower_b.end(), std::back_inserter(result));
	BOOST_CHECK(std::equal(lower_b.begin(), lower_b.end(), result.begin(), result.end()));
}

// 4.5. Code points -----------------------------------------------------------

BOOST_AUTO_TEST_CASE(is_surrogate__1)
{
	using namespace wordring::whatwg;

	BOOST_CHECK(is_surrogate(U'\u0000') == false);
	BOOST_CHECK(is_surrogate(U'\uD7FF') == false);

	BOOST_CHECK(is_surrogate(static_cast<char32_t>(0xD800)) == true);
	BOOST_CHECK(is_surrogate(static_cast<char32_t>(0xDFFF)) == true);

	BOOST_CHECK(is_surrogate(U'\uE000') == false);
	BOOST_CHECK(is_surrogate(static_cast<char32_t>(0xFFFFFFFF)) == false);
}

BOOST_AUTO_TEST_CASE(is_scalar_value__1)
{
	using namespace wordring::whatwg;

	BOOST_CHECK(is_scalar_value(U'\u0000') == true);
	BOOST_CHECK(is_scalar_value(U'\uD7FF') == true);

	BOOST_CHECK(is_scalar_value(static_cast<char32_t>(0xD800)) == false);
	BOOST_CHECK(is_scalar_value(static_cast<char32_t>(0xDFFF)) == false);

	BOOST_CHECK(is_scalar_value(U'\uE000') == true);
	BOOST_CHECK(is_scalar_value(static_cast<char32_t>(0xFFFFFFFF)) == true);
}

BOOST_AUTO_TEST_CASE(is_noncharacter__1)
{
	using namespace wordring::whatwg;

	BOOST_CHECK(is_noncharacter(U'\uFDD0') == true);
	BOOST_CHECK(is_noncharacter(U'\uFDEF') == true);
}

BOOST_AUTO_TEST_CASE(is_ascii_code_point__1)
{
	using namespace wordring::whatwg;

	BOOST_CHECK(is_ascii_code_point(U'\u0000') == true);
	BOOST_CHECK(is_ascii_code_point(U'\u007F') == true);

	BOOST_CHECK(is_ascii_code_point(U'\u0080') == false);
	BOOST_CHECK(is_ascii_code_point(static_cast<char32_t>(0xFFFFFFFF)) == false);
}

BOOST_AUTO_TEST_CASE(is_ascii_tab_or_newline__1)
{
	using namespace wordring::whatwg;

	BOOST_CHECK(is_ascii_tab_or_newline(U'\u0009') == true);
	BOOST_CHECK(is_ascii_tab_or_newline(U'\u000A') == true);
	BOOST_CHECK(is_ascii_tab_or_newline(U'\u000D') == true);

	BOOST_CHECK(is_ascii_tab_or_newline(U'a') == false);
}

BOOST_AUTO_TEST_CASE(is_ascii_white_space__1)
{
	using namespace wordring::whatwg;

	BOOST_CHECK(is_ascii_white_space(U'\u0009') == true);
	BOOST_CHECK(is_ascii_white_space(U'\u000A') == true);
	BOOST_CHECK(is_ascii_white_space(U'\u000C') == true);
	BOOST_CHECK(is_ascii_white_space(U'\u000D') == true);
	BOOST_CHECK(is_ascii_white_space(U'\u0020') == true);

	BOOST_CHECK(is_ascii_white_space(U'a') == false);
}

BOOST_AUTO_TEST_CASE(is_c0_control__1)
{
	using namespace wordring::whatwg;

	BOOST_CHECK(is_c0_control(U'\u0000') == true);
	BOOST_CHECK(is_c0_control(U'\u001F') == true);

	BOOST_CHECK(is_c0_control(U'\u0020') == false);
	BOOST_CHECK(is_c0_control(static_cast<char32_t>(0xFFFFFFFF)) == false);
}

BOOST_AUTO_TEST_CASE(is_ascii_digit__1)
{
	using namespace wordring::whatwg;

	BOOST_CHECK(is_ascii_digit(U'\u0000') == false);
	BOOST_CHECK(is_ascii_digit(U'\u0029') == false);

	BOOST_CHECK(is_ascii_digit(U'0') == true);
	BOOST_CHECK(is_ascii_digit(U'9') == true);

	BOOST_CHECK(is_ascii_digit(U'\u003A') == false);
	BOOST_CHECK(is_ascii_digit(static_cast<char32_t>(0xFFFFFFFF)) == false);
}

BOOST_AUTO_TEST_CASE(is_ascii_upper_hex_digit__1)
{
	using namespace wordring::whatwg;

	BOOST_CHECK(is_ascii_upper_hex_digit(U'\u0000') == false);

	BOOST_CHECK(is_ascii_upper_hex_digit(U'\u0029') == false);
	BOOST_CHECK(is_ascii_upper_hex_digit(U'0') == true);
	BOOST_CHECK(is_ascii_upper_hex_digit(U'9') == true);
	BOOST_CHECK(is_ascii_upper_hex_digit(U'\u003A') == false);

	BOOST_CHECK(is_ascii_upper_hex_digit(U'\u0040') == false);
	BOOST_CHECK(is_ascii_upper_hex_digit(U'A') == true);
	BOOST_CHECK(is_ascii_upper_hex_digit(U'F') == true);
	BOOST_CHECK(is_ascii_upper_hex_digit(U'\u0047') == false);

	BOOST_CHECK(is_ascii_upper_hex_digit(static_cast<char32_t>(0xFFFFFFFF)) == false);
}

BOOST_AUTO_TEST_CASE(is_ascii_lower_hex_digit__1)
{
	using namespace wordring::whatwg;

	BOOST_CHECK(is_ascii_lower_hex_digit(U'\u0000') == false);

	BOOST_CHECK(is_ascii_lower_hex_digit(U'\u0029') == false);
	BOOST_CHECK(is_ascii_lower_hex_digit(U'0') == true);
	BOOST_CHECK(is_ascii_lower_hex_digit(U'9') == true);
	BOOST_CHECK(is_ascii_lower_hex_digit(U'\u003A') == false);

	BOOST_CHECK(is_ascii_lower_hex_digit(U'\u0060') == false);
	BOOST_CHECK(is_ascii_lower_hex_digit(U'a') == true);
	BOOST_CHECK(is_ascii_lower_hex_digit(U'f') == true);
	BOOST_CHECK(is_ascii_lower_hex_digit(U'\u0067') == false);

	BOOST_CHECK(is_ascii_lower_hex_digit(static_cast<char32_t>(0xFFFFFFFF)) == false);
}

BOOST_AUTO_TEST_CASE(is_ascii_hex_digit__1)
{
	using namespace wordring::whatwg;

	BOOST_CHECK(is_ascii_hex_digit(U'\u0000') == false);

	BOOST_CHECK(is_ascii_hex_digit(U'\u0029') == false);
	BOOST_CHECK(is_ascii_hex_digit(U'0') == true);
	BOOST_CHECK(is_ascii_hex_digit(U'9') == true);
	BOOST_CHECK(is_ascii_hex_digit(U'\u003A') == false);

	BOOST_CHECK(is_ascii_hex_digit(U'\u0040') == false);
	BOOST_CHECK(is_ascii_hex_digit(U'A') == true);
	BOOST_CHECK(is_ascii_hex_digit(U'F') == true);
	BOOST_CHECK(is_ascii_hex_digit(U'\u0047') == false);

	BOOST_CHECK(is_ascii_hex_digit(U'\u0060') == false);
	BOOST_CHECK(is_ascii_hex_digit(U'a') == true);
	BOOST_CHECK(is_ascii_hex_digit(U'f') == true);
	BOOST_CHECK(is_ascii_hex_digit(U'\u0067') == false);

	BOOST_CHECK(is_ascii_hex_digit(static_cast<char32_t>(0xFFFFFFFF)) == false);
}

BOOST_AUTO_TEST_CASE(is_ascii_upper_alpha__1)
{
	using namespace wordring::whatwg;

	BOOST_CHECK(is_ascii_upper_alpha(U'\u0000') == false);
	BOOST_CHECK(is_ascii_upper_alpha(U'\u0040') == false);

	BOOST_CHECK(is_ascii_upper_alpha(U'A') == true);
	BOOST_CHECK(is_ascii_upper_alpha(U'Z') == true);

	BOOST_CHECK(is_ascii_upper_alpha(U'\u005B') == false);
	BOOST_CHECK(is_ascii_upper_alpha(static_cast<char32_t>(0xFFFFFFFF)) == false);
}

BOOST_AUTO_TEST_CASE(is_ascii_lower_alpha__1)
{
	using namespace wordring::whatwg;

	BOOST_CHECK(is_ascii_lower_alpha(U'\u0000') == false);
	BOOST_CHECK(is_ascii_lower_alpha(U'\u0060') == false);

	BOOST_CHECK(is_ascii_lower_alpha(U'a') == true);
	BOOST_CHECK(is_ascii_lower_alpha(U'z') == true);

	BOOST_CHECK(is_ascii_lower_alpha(U'\u007B') == false);
	BOOST_CHECK(is_ascii_lower_alpha(static_cast<char32_t>(0xFFFFFFFF)) == false);
}

BOOST_AUTO_TEST_CASE(is_ascii_alpha__1)
{
	using namespace wordring::whatwg;

	BOOST_CHECK(is_ascii_alpha(U'\u0000') == false);

	BOOST_CHECK(is_ascii_alpha(U'\u0040') == false);
	BOOST_CHECK(is_ascii_alpha(U'A') == true);
	BOOST_CHECK(is_ascii_alpha(U'Z') == true);
	BOOST_CHECK(is_ascii_alpha(U'\u005B') == false);

	BOOST_CHECK(is_ascii_alpha(U'\u0060') == false);
	BOOST_CHECK(is_ascii_alpha(U'a') == true);
	BOOST_CHECK(is_ascii_alpha(U'z') == true);
	BOOST_CHECK(is_ascii_alpha(U'\u007B') == false);

	BOOST_CHECK(is_ascii_alpha(static_cast<char32_t>(0xFFFFFFFF)) == false);
}

BOOST_AUTO_TEST_CASE(is_ascii_alphanumeric__1)
{
	using namespace wordring::whatwg;

	BOOST_CHECK(is_ascii_alphanumeric(U'\u0000') == false);

	BOOST_CHECK(is_ascii_alphanumeric(U'\u0029') == false);
	BOOST_CHECK(is_ascii_alphanumeric(U'0') == true);
	BOOST_CHECK(is_ascii_alphanumeric(U'9') == true);
	BOOST_CHECK(is_ascii_alphanumeric(U'\u003A') == false);

	BOOST_CHECK(is_ascii_alphanumeric(U'\u0040') == false);
	BOOST_CHECK(is_ascii_alphanumeric(U'A') == true);
	BOOST_CHECK(is_ascii_alphanumeric(U'Z') == true);
	BOOST_CHECK(is_ascii_alphanumeric(U'\u005B') == false);

	BOOST_CHECK(is_ascii_alphanumeric(U'\u0060') == false);
	BOOST_CHECK(is_ascii_alphanumeric(U'a') == true);
	BOOST_CHECK(is_ascii_alphanumeric(U'z') == true);
	BOOST_CHECK(is_ascii_alphanumeric(U'\u007B') == false);

	BOOST_CHECK(is_ascii_alphanumeric(static_cast<char32_t>(0xFFFFFFFF)) == false);
}

// 4.6. Strings ---------------------------------------------------------------

BOOST_AUTO_TEST_CASE(isomorphic_encode__1)
{
	using namespace wordring::whatwg;

	std::u32string a1{ U"abcdefg" };
	std::string b1{ "abcdefg" };
	std::vector<char> o1;

	isomorphic_encode(a1.begin(), a1.end(), std::back_inserter(o1));
	BOOST_CHECK(std::equal(b1.begin(), b1.end(), o1.begin(), o1.end()) == true);

	o1.clear();
	isomorphic_encode(b1.begin(), b1.end(), std::back_inserter(o1));
	BOOST_CHECK(std::equal(b1.begin(), b1.end(), o1.begin(), o1.end()) == true);
}

BOOST_AUTO_TEST_CASE(is_ascii_string__1)
{
	using namespace wordring::whatwg;

	// utf-8
	std::u8string const s8_true{ u8"abcdefg" };
	std::u8string const s8_false{ u8"abcdefgあいうえお" };
	BOOST_CHECK(is_ascii_string(s8_true.begin(), s8_true.end()) == true);
	BOOST_CHECK(is_ascii_string(s8_false.begin(), s8_false.end()) == false);

	// utf-16
	std::u16string const s16_true{ u"abcdefg" };
	std::u16string const s16_false{ u"abcdefgあいうえお" };
	BOOST_CHECK(is_ascii_string(s16_true.begin(), s16_true.end()) == true);
	BOOST_CHECK(is_ascii_string(s16_false.begin(), s16_false.end()) == false);

	// utf-32
	std::u32string const s32_true{ U"abcdefg" };
	std::u32string const s32_false{ U"abcdefgあいうえお" };
	BOOST_CHECK(is_ascii_string(s32_true.begin(), s32_true.end()) == true);
	BOOST_CHECK(is_ascii_string(s32_false.begin(), s32_false.end()) == false);
}

BOOST_AUTO_TEST_CASE(to_ascii_lowercase__1)
{
	using namespace wordring::whatwg;

	// utf-8
	std::u8string const s8_original{ u8"ABCDEあいうえお𠀋𡈽𡌛𡑮𡢽Àⱥ𐊠" };
	std::u8string const s8_correct{ u8"abcdeあいうえお𠀋𡈽𡌛𡑮𡢽Àⱥ𐊠" };
	std::u8string s8_result{};
	to_ascii_lowercase(s8_original.begin(), s8_original.end(), std::back_inserter(s8_result));
	BOOST_CHECK(s8_correct == s8_result);

	// utf-16
	std::u16string const s16_original{ u"ABCDEあいうえお𠀋𡈽𡌛𡑮𡢽Àⱥ𐊠" };
	std::u16string const s16_correct{ u"abcdeあいうえお𠀋𡈽𡌛𡑮𡢽Àⱥ𐊠" };
	std::u16string s16_result{};
	to_ascii_lowercase(s16_original.begin(), s16_original.end(), std::back_inserter(s16_result));
	BOOST_CHECK(s16_correct == s16_result);

	// utf-32
	std::u32string const s32_original{ U"ABCDEあいうえお𠀋𡈽𡌛𡑮𡢽Àⱥ𐊠" };
	std::u32string const s32_correct{ U"abcdeあいうえお𠀋𡈽𡌛𡑮𡢽Àⱥ𐊠" };
	std::u32string s32_result{};
	to_ascii_lowercase(s32_original.begin(), s32_original.end(), std::back_inserter(s32_result));
	BOOST_CHECK(s32_correct == s32_result);
}

BOOST_AUTO_TEST_CASE(to_ascii_uppercase__1)
{
	using namespace wordring::whatwg;

	// utf-8
	std::u8string const s8_original{ u8"abcdeあいうえお𠀋𡈽𡌛𡑮𡢽Àⱥ𐊠" };
	std::u8string const s8_correct{ u8"ABCDEあいうえお𠀋𡈽𡌛𡑮𡢽Àⱥ𐊠" };
	std::u8string s8_result{};
	to_ascii_uppercase(s8_original.begin(), s8_original.end(), std::back_inserter(s8_result));
	BOOST_CHECK(s8_correct == s8_result);

	// utf-16
	std::u16string const s16_original{ u"abcdeあいうえお𠀋𡈽𡌛𡑮𡢽Àⱥ𐊠" };
	std::u16string const s16_correct{ u"ABCDEあいうえお𠀋𡈽𡌛𡑮𡢽Àⱥ𐊠" };
	std::u16string s16_result{};
	to_ascii_uppercase(s16_original.begin(), s16_original.end(), std::back_inserter(s16_result));
	BOOST_CHECK(s16_correct == s16_result);

	// utf-32
	std::u32string const s32_original{ U"abcdeあいうえお𠀋𡈽𡌛𡑮𡢽Àⱥ𐊠" };
	std::u32string const s32_correct{ U"ABCDEあいうえお𠀋𡈽𡌛𡑮𡢽Àⱥ𐊠" };
	std::u32string s32_result{};
	to_ascii_uppercase(s32_original.begin(), s32_original.end(), std::back_inserter(s32_result));
	BOOST_CHECK(s32_correct == s32_result);
}

BOOST_AUTO_TEST_CASE(is_ascii_case_insensitive_match__1)
{
	using namespace wordring::whatwg;

	// utf-8
	std::u8string const s8_original{ u8"ABCDEあいうえお𠀋𡈽𡌛𡑮𡢽Àⱥ𐊠" };
	std::u8string const s8_correct{ u8"abcdeあいうえお𠀋𡈽𡌛𡑮𡢽Àⱥ𐊠" };
	std::u8string const s8_incorrect_1{ u8"abcdeあいうえお𠀋𡈽𡌛𡑮𡢽Àⱥ𐊠abcde" };
	std::u8string const s8_incorrect_2{ u8"12345あいうえお𠀋𡈽𡌛𡑮𡢽Àⱥ𐊠" };
	BOOST_CHECK(is_ascii_case_insensitive_match(s8_original.begin(), s8_original.end(), s8_correct.begin(), s8_correct.end()) == true);
	BOOST_CHECK(is_ascii_case_insensitive_match(s8_original.begin(), s8_original.end(), s8_incorrect_1.begin(), s8_incorrect_1.end()) == false);
	BOOST_CHECK(is_ascii_case_insensitive_match(s8_original.begin(), s8_original.end(), s8_incorrect_2.begin(), s8_incorrect_2.end()) == false);
}

BOOST_AUTO_TEST_CASE(ascii_encode__1)
{
	using namespace wordring::whatwg;

	std::u16string const s16_original{ u"ABCDE" };
	std::u16string const s16_correct{ u"ABCDE" };
	std::u8string s8_result{};
	ascii_encode(s16_original.begin(), s16_original.end(), std::back_inserter(s8_result));
	BOOST_CHECK(std::equal(s16_correct.begin(), s16_correct.end(), s8_result.begin(), s8_result.end()));
}

BOOST_AUTO_TEST_CASE(ascii_decode__1)
{
	using namespace wordring::whatwg;

	std::string const s_original{ "ABCDE" };
	std::string const s_correct{ "ABCDE" };
	std::u16string s16_result{};
	ascii_decode(s_original.begin(), s_original.end(), std::back_inserter(s16_result));
	BOOST_CHECK(std::equal(s_correct.begin(), s_correct.end(), s16_result.begin(), s16_result.end()));
}

BOOST_AUTO_TEST_CASE(strip_newlines__1)
{
	using namespace wordring::whatwg;

	std::u32string const s32_original{ U"ABCDE\nあいうえ\r\nお𠀋𡈽𡌛\r𡑮𡢽Àⱥ𐊠" };
	std::u32string const s32_correct{ U"ABCDEあいうえお𠀋𡈽𡌛𡑮𡢽Àⱥ𐊠" };
	std::u32string s32_result{};
	strip_newlines(s32_original.begin(), s32_original.end(), std::back_inserter(s32_result));
	BOOST_CHECK(s32_result == s32_correct);
}

BOOST_AUTO_TEST_CASE(normalize_newlines__1)
{
	using namespace wordring::whatwg;

	std::u32string const s32_original{ U"A\rA\nA\r\n\n\n\r\r\n" };
	std::u32string const s32_correct{ U"A\nA\nA\n\n\n\n\n" };
	std::u32string s32_result{};
	normalize_newlines(s32_original.begin(), s32_original.end(), std::back_inserter(s32_result));
	BOOST_CHECK(s32_result == s32_correct);
}

BOOST_AUTO_TEST_CASE(strip_leading_and_trailing_ascii_whitespace__1)
{
	using namespace wordring::whatwg;

	std::u32string const s32_original_1{ U"  ABC 123  " };
	std::u32string const s32_correct_1{ U"ABC 123" };
	std::u32string s32_result{};
	strip_leading_and_trailing_ascii_whitespace(s32_original_1.begin(), s32_original_1.end(), std::back_inserter(s32_result));
	BOOST_CHECK(s32_result == s32_correct_1);

	std::u32string const s32_original_2{ U"ABC 123" };
	std::u32string const s32_correct_2{ U"ABC 123" };
	s32_result.clear();
	strip_leading_and_trailing_ascii_whitespace(s32_original_2.begin(), s32_original_2.end(), std::back_inserter(s32_result));
	BOOST_CHECK(s32_result == s32_correct_2);
}

BOOST_AUTO_TEST_CASE(strip_and_collapse_ascii_whitespace__1)
{
	using namespace wordring::whatwg;

	std::u32string const s32_original_1{ U"  ABC   123  " };
	std::u32string const s32_correct_1{ U"ABC 123" };
	std::u32string s32_result{};
	strip_and_collapse_ascii_whitespace(s32_original_1.begin(), s32_original_1.end(), std::back_inserter(s32_result));
	BOOST_CHECK(s32_result == s32_correct_1);

	std::u32string const s32_original_2{ U"ABC 123" };
	std::u32string const s32_correct_2{ U"ABC 123" };
	s32_result.clear();
	strip_and_collapse_ascii_whitespace(s32_original_2.begin(), s32_original_2.end(), std::back_inserter(s32_result));
	BOOST_CHECK(s32_result == s32_correct_2);
}

BOOST_AUTO_TEST_CASE(collect_a_sequence_of_code_points__1)
{
	using namespace wordring::whatwg;

	std::u32string const s32_original_1{ U"  ABC   123  " };
	std::u32string const s32_correct_1{ U"  " };
	std::u32string s32_result_1{};
	auto it_result_1 = collect_a_sequence_of_code_points(
		s32_original_1.begin(), s32_original_1.end(), std::back_inserter(s32_result_1), is_ascii_white_space);
	BOOST_CHECK(*it_result_1 == U'A');
	BOOST_CHECK(s32_result_1 == s32_correct_1);

	std::u32string const s32_original_2{ U"ABC   123  " };
	std::u32string const s32_correct_2{ U"" };
	std::u32string s32_result_2{};
	auto it_result_2 = collect_a_sequence_of_code_points(
		s32_original_2.begin(), s32_original_2.end(), std::back_inserter(s32_result_2), is_ascii_white_space);
	BOOST_CHECK(*it_result_2 == U'A');
	BOOST_CHECK(s32_result_2 == s32_correct_2);

	std::u32string const s32_original_3{ U"   " };
	std::u32string const s32_correct_3{ U"   " };
	std::u32string s32_result_3{};
	auto it_result_3 = collect_a_sequence_of_code_points(
		s32_original_3.begin(), s32_original_3.end(), std::back_inserter(s32_result_3), is_ascii_white_space);
	BOOST_CHECK(it_result_3 == s32_original_3.end());
	BOOST_CHECK(s32_result_3 == s32_correct_3);
}

BOOST_AUTO_TEST_CASE(skip_ascii_whitespace__1)
{
	using namespace wordring::whatwg;

	std::u32string const s32_original_1{ U"  ABC" };
	std::u32string const s32_correct_1{ U"ABC" };
	auto it_result_1 = skip_ascii_whitespace(s32_original_1.begin(), s32_original_1.end());
	std::u32string s32_result_1{ it_result_1, s32_original_1.end() };
	BOOST_CHECK(*it_result_1 == U'A');
	BOOST_CHECK(s32_result_1 == s32_correct_1);
}

BOOST_AUTO_TEST_CASE(strictly_split_on_a_particular_delimiter__1)
{
	using namespace wordring::whatwg;

	std::u32string const s32_original_1{ U"ABC,DEF,GHI" };
	std::vector<std::u32string> const s32_correct_1{ U"ABC", U"DEF", U"GHI" };
	std::vector<std::u32string> s32_result_1{};
	strictly_split_on_a_particular_delimiter(
		s32_original_1.begin(), s32_original_1.end(), std::back_inserter(s32_result_1), U',');
	BOOST_CHECK(s32_result_1 == s32_correct_1);

	std::u32string const s32_original_2{};
	std::vector<std::u32string> const s32_correct_2{ std::u32string{} };
	std::vector<std::u32string> s32_result_2{};
	strictly_split_on_a_particular_delimiter(
		s32_original_2.begin(), s32_original_2.end(), std::back_inserter(s32_result_2), U',');
	BOOST_CHECK(s32_result_2 == s32_correct_2);

	std::u32string const s32_original_3{ U"," };
	std::vector<std::u32string> const s32_correct_3{ std::u32string{}, std::u32string{} };
	std::vector<std::u32string> s32_result_3{};
	strictly_split_on_a_particular_delimiter(
		s32_original_3.begin(), s32_original_3.end(), std::back_inserter(s32_result_3), U',');
	BOOST_CHECK(s32_result_3 == s32_correct_3);
}

BOOST_AUTO_TEST_CASE(split_on_ascii_whitespace__1)
{
	using namespace wordring::whatwg;

	std::u32string const s32_original_1{ U" ABC DEF GHI " };
	std::vector<std::u32string> const s32_correct_1{ U"ABC", U"DEF", U"GHI" };
	std::vector<std::u32string> s32_result_1{};
	split_on_ascii_whitespace(
		s32_original_1.begin(), s32_original_1.end(), std::back_inserter(s32_result_1));
	BOOST_CHECK(s32_result_1 == s32_correct_1);
}

BOOST_AUTO_TEST_CASE(split_on_commas__1)
{
	using namespace wordring::whatwg;

	std::u32string const s32_original_1{ U"ABC,DEF,GHI" };
	std::vector<std::u32string> const s32_correct_1{ U"ABC", U"DEF", U"GHI" };
	std::vector<std::u32string> s32_result_1{};
	split_on_commas(
		s32_original_1.begin(), s32_original_1.end(), std::back_inserter(s32_result_1));
	BOOST_CHECK(s32_result_1 == s32_correct_1);

	std::u32string const s32_original_2{};
	std::vector<std::u32string> const s32_correct_2{};
	std::vector<std::u32string> s32_result_2{};
	split_on_commas(
		s32_original_2.begin(), s32_original_2.end(), std::back_inserter(s32_result_2));
	BOOST_CHECK(s32_result_2 == s32_correct_2);

	std::u32string const s32_original_3{ U"," };
	std::vector<std::u32string> const s32_correct_3{ std::u32string{} };
	std::vector<std::u32string> s32_result_3{};
	split_on_commas(
		s32_original_3.begin(), s32_original_3.end(), std::back_inserter(s32_result_3));
	BOOST_CHECK(s32_result_3 == s32_correct_3);
}

BOOST_AUTO_TEST_SUITE_END()
