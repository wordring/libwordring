#pragma once

// https://infra.spec.whatwg.org/
// https://triple-underscore.github.io/infra-ja.html

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iterator>
#include <string>
#include <string_view>
#include <type_traits>

#include <wordring/algorithm.hpp>
#include <wordring/compatibility.hpp>
#include <wordring/constraint.hpp>
#include <wordring/whatwg/infra/unicode.hpp>

namespace wordring::whatwg
{

	// 4.3. Bytes -------------------------------------------------------------

	inline constexpr bool is_ascii_byte(char ch) noexcept { return static_cast<unsigned char>(ch) <= 0x7F; }

	// 4.4. Byte sequences ----------------------------------------------------

	/*! 小文字化

	@tparam InputIterator  value_typeがcharである入力イテレータ。
	@tparam OutputIterator 出力イテレータ。

	@param[in]  first  入力（半開区間）の先頭を示すイテレータ。
	@param[in]  last   入力（半開区間）の終端を示すイテレータ。
	@param[out] output 出力先を示すイテレータ。

	[first, last)を[A-Z]があれば[a-z]に置き換えたうえでoutputへ出力する。
	出力の範囲チェックは行わないため、十分な容量が確保されているか、あるいは動的に確保される必要がある。
	first == outputの場合は、[first, last)自身を小文字化する。

	- 入力と出力が重なっていても問題ないが、要素の追加によるイテレータの無効化、出力を入力が読み取ることによる無限連鎖に要注意。
	*/
	template <typename InputIterator, typename OutputIterator>
	inline void to_byte_lowercase(InputIterator first, InputIterator last, OutputIterator output)
	{
		while (first != last)
		{
			unsigned char ch = *first++;
			if (0x41u <= ch && ch <= 0x5Au) ch += 0x20u;
			*output++ = ch;
		}
	}

	/*! 大文字化

	@tparam InputIterator  value_typeがcharである入力イテレータ。
	@tparam OutputIterator 出力イテレータ。

	@param[in]  first  入力（半開区間）の先頭を示すイテレータ。
	@param[in]  last   入力（半開区間）の終端を示すイテレータ。
	@param[out] output 出力先を示すイテレータ。

	[first, last)を[a-z]があれば[A-Z]に置き換えたうえでoutputへ出力する。
	出力の範囲チェックは行わないため、十分な容量が確保されているか、あるいは動的に確保される必要がある。
	first == outputの場合は、[first, last)自身を大文字化する。

	- 入力と出力が重なっていても問題ないが、要素の追加によるイテレータの無効化、出力を入力が読み取ることによる無限連鎖に要注意。
	*/
	template <typename InputIterator, typename OutputIterator>
	inline void to_byte_uppercase(InputIterator first, InputIterator last, OutputIterator output)
	{
		while (first != last)
		{
			unsigned char ch = *first++;
			if (0x61u <= ch && ch <= 0x7Au) ch -= 0x20u;
			*output++ = ch;
		}
	}

	/*! 大文字と小文字を区別しない比較。
	*/
	template <typename InputIterator1, typename InputIterator2>
	inline bool is_byte_case_insensitive_match(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2)
	{
		std::string lower1{};
		to_byte_lowercase(first1, last1, std::back_inserter(lower1));
		std::string lower2{};
		to_byte_lowercase(first2, last2, std::back_inserter(lower2));

		return lower1 == lower2;
	}

	/*! 同型にデコード。

	出力の範囲チェックは行わないため、十分な容量が確保されているか、あるいは動的に確保される必要がある。
	*/
	template <typename InputIterator, typename OutputIterator>
	inline void isomorphic_decode(InputIterator first, InputIterator last, OutputIterator output)
	{
		while (first != last) *output++ = static_cast<unsigned char>(*first++);
	}

	// 4.5. Code points -------------------------------------------------------

	inline bool is_surrogate(uint32_t cp) noexcept
	{
		return 0xD800 <= cp && cp <= 0xDFFF;
	}

	inline bool is_scalar_value(uint32_t cp) noexcept { return !is_surrogate(cp); }

	inline bool is_noncharacter(uint32_t cp) noexcept
	{
		if (0xFDD0 <= cp && cp <= 0xFDEF) return true;

		switch (cp)
		{
		case 0xFFFE:
		case 0xFFFF:
		case 0x1FFFE:
		case 0x1FFFF:
		case 0x2FFFE:
		case 0x2FFFF:
		case 0x3FFFE:
		case 0x3FFFF:
		case 0x4FFFE:
		case 0x4FFFF:
		case 0x5FFFE:
		case 0x5FFFF:
		case 0x6FFFE:
		case 0x6FFFF:
		case 0x7FFFE:
		case 0x7FFFF:
		case 0x8FFFE:
		case 0x8FFFF:
		case 0x9FFFE:
		case 0x9FFFF:
		case 0xAFFFE:
		case 0xAFFFF:
		case 0xBFFFE:
		case 0xBFFFF:
		case 0xCFFFE:
		case 0xCFFFF:
		case 0xDFFFE:
		case 0xDFFFF:
		case 0xEFFFE:
		case 0xEFFFF:
		case 0xFFFFE:
		case 0xFFFFF:
		case 0x10FFFE:
		case 0x10FFFF:
			return true;
		}

		return false;
	}

	inline bool is_ascii_code_point(uint32_t cp) noexcept
	{
		return 0 <= cp && cp <= 0x7F;
	}

	inline bool is_ascii_tab_or_newline(uint32_t cp) noexcept
	{
		return cp == 0x9 || cp == 0xA || cp == 0xD;
	}

	inline bool is_ascii_white_space(uint32_t cp) noexcept
	{
		return cp == 0x9 || cp == 0xA || cp == 0xC || cp == 0xD || cp == 0x20;
	}

	inline bool is_c0_control(uint32_t cp) noexcept
	{
		return cp <= 0x1F;
	}

	inline bool is_c0_control_or_space(uint32_t cp) noexcept
	{
		return is_c0_control(cp) || cp == 0x20;
	}

	inline bool is_control(uint32_t cp) noexcept
	{
		return is_c0_control(cp) || (0x7F <= cp && cp <= 0x9F);
	}

	inline bool is_ascii_digit(uint32_t cp) noexcept
	{
		return 0x30 <= cp && cp <= 0x39;
	}

	inline bool is_ascii_upper_hex_digit(uint32_t cp) noexcept
	{
		return is_ascii_digit(cp) || (0x41 <= cp && cp <= 0x46);
	}

	inline bool is_ascii_lower_hex_digit(uint32_t cp) noexcept
	{
		return is_ascii_digit(cp) || (0x61 <= cp && cp <= 0x66);
	}

	inline bool is_ascii_hex_digit(uint32_t cp) noexcept
	{
		return is_ascii_upper_hex_digit(cp) || is_ascii_lower_hex_digit(cp);
	}

	inline bool is_ascii_upper_alpha(uint32_t cp) noexcept
	{
		return 0x41 <= cp && cp <= 0x5A;
	}

	inline bool is_ascii_lower_alpha(uint32_t cp) noexcept
	{
		return 0x61 <= cp && cp <= 0x7A;
	}

	inline bool is_ascii_alpha(uint32_t cp) noexcept
	{
		return is_ascii_upper_alpha(cp) || is_ascii_lower_alpha(cp);
	}

	inline bool is_ascii_alphanumeric(uint32_t cp) noexcept
	{
		return is_ascii_digit(cp) || is_ascii_alpha(cp);
	}

	// 4.6. Strings -----------------------------------------------------------

	template <typename InputIterator, typename OutputIterator>
	inline void isomorphic_encode(InputIterator first, InputIterator last, OutputIterator output)
	{
		while (first != last)
		{
			assert(*first <= 0xFFu);
			*output++ = static_cast<unsigned char>(*first++);
		}
	}

	template <typename InputIterator>
	inline bool	is_ascii_string(InputIterator first, InputIterator last)
	{
		uint32_t cp{};
		while (first != last)
		{
			first = to_code_point(first, last, cp);
			if (0x7Fu < cp) return false;
		}
		return true;
	}

	/*!
	@brief 小文字化する。
	*/
	template <typename InputIterator, typename OutputIterator>
	inline void to_ascii_lowercase(InputIterator first, InputIterator last, OutputIterator output)
	{
		uint32_t cp{};
		while (first != last)
		{
			first = to_code_point(first, last, cp);
			if (is_ascii_upper_alpha(cp)) cp += 0x20u;
			to_string(cp, output);
		}
	}

	/*!
	@brief 大文字化する。
	*/
	template <typename InputIterator, typename OutputIterator>
	inline void to_ascii_uppercase(InputIterator first, InputIterator last, OutputIterator output)
	{
		uint32_t cp{};
		while (first != last)
		{
			first = to_code_point(first, last, cp);
			if (is_ascii_lower_alpha(cp)) cp -= 0x20u;
			to_string(cp, output);
		}
	}

	template <typename InputIterator1, typename InputIterator2>
	inline bool is_ascii_case_insensitive_match(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2)
	{
		uint32_t cp1{}, cp2{};
		while (first1 != last1 && first2 != last2)
		{
			first1 = to_code_point(first1, last1, cp1);
			if (is_ascii_upper_alpha(cp1)) cp1 += 0x20u;

			first2 = to_code_point(first2, last2, cp2);
			if (is_ascii_upper_alpha(cp2)) cp2 += 0x20u;

			if (cp1 != cp2) return false;
		}

		return first1 == last1 && first2 == last2;
	}

	template <typename InputIterator, typename OutputIterator>
	inline void ascii_encode(InputIterator first, InputIterator last, OutputIterator output)
	{
		assert(is_ascii_string(first, last));

		isomorphic_encode(first, last, output);
	}

	template <typename InputIterator, typename OutputIterator>
	inline void ascii_decode(InputIterator first, InputIterator last, OutputIterator output)
	{
		assert([](InputIterator it1, InputIterator it2)->bool
		{
			while (it1 != it2) if (!is_ascii_byte(static_cast<unsigned char>(*it1++))) return false;
			return true;
		}(first, last));

		isomorphic_decode(first, last, output);
	}

	template <typename InputIterator, typename OutputIterator>
	inline void strip_newlines(InputIterator first, InputIterator last, OutputIterator output)
	{
		uint32_t cp{};
		while (first != last)
		{
			first = to_code_point(first, last, cp);
			if (cp != 0xA && cp != 0xD) to_string(cp, output);
		}
	}

	/*!
	- CR(0xD), LF(0xA)
	- CRLF -> LF
	- LF -> LF
	- CR -> LF
	*/
	template <typename InputIterator, typename OutputIterator>
	inline void normalize_newlines(InputIterator first, InputIterator last, OutputIterator output)
	{
		enum class states : uint32_t { Start, Cr };

		states state{ states::Start };
		uint32_t cp{};
		while (first != last)
		{
			first = to_code_point(first, last, cp);
			switch (state)
			{
			case states::Start:
				if (cp == 0xDu) state = states::Cr;
				else if (cp == 0xAu) goto LineBreak;
				else goto Output;
				break;
			case states::Cr:
				if (cp == 0xDu) *output++ = 0xAu;
				else if (cp == 0xAu) goto LineBreak;
				else
				{
					*output++ = 0xAu;
					goto Output;
				}
				break;
			LineBreak:
				cp = 0xAu;
			Output:
				*output++ = cp;
				state = states::Start;
				break;
			}
		}
	}

	/*!
	@brief 前後のASCII空白文字を剥ぎ取る。
	*/
	template <typename ForwardIterator, typename OutputIterator>
	inline void strip_leading_and_trailing_ascii_whitespace(ForwardIterator first, ForwardIterator last, OutputIterator output)
	{
		static_assert(std::is_base_of_v<std::forward_iterator_tag, typename std::iterator_traits<ForwardIterator>::iterator_category>);

		std::basic_string<uint32_t> tmp{};
		uint32_t cp{};
		while (first != last)
		{
			first = to_code_point(first, last, cp);
			tmp.push_back(cp);
		}

		auto left = std::find_if_not(tmp.begin(), tmp.end(), is_ascii_white_space);
		auto right = std::find_if_not(tmp.crbegin(), tmp.crend(), is_ascii_white_space).base();

		while (left != right) to_string(*left++, output);
	}

	template <typename InputIterator, typename OutputIterator>
	inline void strip_and_collapse_ascii_whitespace(InputIterator first, InputIterator last, OutputIterator output)
	{
		static_assert(std::is_base_of_v<std::input_iterator_tag, typename std::iterator_traits<InputIterator>::iterator_category>);

		enum class states : uint32_t { Start, Leading, Normal, Ws };

		states state{ states::Start };
		uint32_t cp{};
		while (first != last)
		{
			first = to_code_point(first, last, cp);
			switch (state)
			{
			case states::Start:
				if (is_ascii_white_space(cp)) goto Leading;
				goto Normal;
			case states::Leading:
				if (is_ascii_white_space(cp)) break;
				goto Normal;
			Leading:
				state = states::Leading;
				break;
			case states::Normal:
				if (is_ascii_white_space(cp)) goto Ws;
			Normal:
				state = states::Normal;
				to_string(cp, output);
				break;
			case states::Ws:
				if (is_ascii_white_space(cp)) break;
				to_string(0x20u, output);
				goto Normal;
			Ws:
				state = states::Ws;
				break;
			}
		}
	}

	template <typename InputIterator, typename OutputIterator, typename Condition>
	inline InputIterator collect_a_sequence_of_code_points(
		InputIterator first, InputIterator last, OutputIterator output, Condition condition)
	{
		InputIterator result{ first };

		uint32_t cp{};
		while (first != last)
		{
			first = to_code_point(first, last, cp);
			if (!condition(cp)) break;

			result = first;
			to_string(cp, output);
		}

		return result;
	}

	template <typename InputIterator>
	inline InputIterator skip_ascii_whitespace(InputIterator first, InputIterator last)
	{
		InputIterator result{ first };

		uint32_t cp{};
		while (first != last)
		{
			first = to_code_point(first, last, cp);
			if (!is_ascii_white_space(cp)) break;

			result = first;
		}

		return result;
	}

	/*
	特定の区切り文字で厳密に分割する。
	*/
	template <typename InputIterator, typename OutputIterator>
	inline void strictly_split_on_a_particular_delimiter(
		InputIterator first, InputIterator last, OutputIterator output, uint32_t delimiter)
	{
		using char_type = typename std::iterator_traits<InputIterator>::value_type;
		using string_type = typename std::basic_string<char_type>;

		string_type token{};
		first = collect_a_sequence_of_code_points(first, last, std::back_inserter(token), [delimiter](uint32_t cp)->bool { return cp != delimiter; });
		*output++ = token;
		token.clear();
		while (first != last)
		{
			assert([](InputIterator first, InputIterator last, uint32_t delimiter)->bool
			{
				uint32_t cp{};
				to_code_point(first, last, cp);
				return cp == delimiter;
			}(first, last, delimiter));

			++first;
			token.clear();
			first = collect_a_sequence_of_code_points(first, last, std::back_inserter(token), [delimiter](uint32_t cp)->bool { return cp != delimiter; });
			*output++ = token;
		}
	}

	template <typename InputIterator, typename OutputIterator>
	inline void split_on_ascii_whitespace(InputIterator first, InputIterator last, OutputIterator output)
	{
		using char_type = typename std::iterator_traits<InputIterator>::value_type;
		using string_type = typename std::basic_string<char_type>;

		string_type token{};
		first = skip_ascii_whitespace(first, last);
		while (first != last)
		{
			token.clear();
			first = collect_a_sequence_of_code_points(first, last, std::back_inserter(token), [](uint32_t cp)->bool { return !is_ascii_white_space(cp); });
			*output++ = token;
			first = skip_ascii_whitespace(first, last);
		}
	}

	template <typename InputIterator, typename OutputIterator>
	inline void split_on_commas(InputIterator first, InputIterator last, OutputIterator output)
	{
		using char_type = typename std::iterator_traits<InputIterator>::value_type;
		using string_type = typename std::basic_string<char_type>;

		while (first != last)
		{
			string_type tmp1{}, tmp2{};
			first = collect_a_sequence_of_code_points(first, last, std::back_inserter(tmp1), [](uint32_t cp)->bool { return cp != 0x2Cu; });
			strip_leading_and_trailing_ascii_whitespace(tmp1.begin(), tmp1.end(), std::back_inserter(tmp2));
			*output++ = tmp2;
			if (first != last)
			{
				assert(*first == 0x2Cu);
				++first;
			}
		}
	}

	inline void string_concatenate()
	{

	}
}
