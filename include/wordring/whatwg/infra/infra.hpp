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

namespace wordring::whatwg::infra
{
	// 4.3. Bytes -------------------------------------------------------------

	inline bool is_ascii_byte(char ch)
	{
		return static_cast<unsigned char>(ch) <= 0x7F;
	}

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
		static_assert(std::is_same_v<typename std::iterator_traits<InputIterator>::value_type, char>);

		while (first != last)
		{
			unsigned char ch = *first++;
			if (0x41 <= ch && ch <= 0x5A) ch += 0x20;
			*output++ = ch;
		}
	}

	template <typename ByteSequence, typename OutputIterator>
	inline void to_byte_lowercase(ByteSequence const & bytes, OutputIterator output)
	{
		static_assert(std::is_same_v<typename ByteSequence::value_type, char>);

		to_byte_lowercase(bytes.begin(), bytes.end(), output);
	}

	template <typename OutputIterator>
	inline void to_byte_lowercase(char const * bytes, OutputIterator output)
	{
		to_byte_lowercase(std::string_view{ bytes }, output);
	}

	template <typename ByteSequence>
	inline ByteSequence to_byte_lowercase(ByteSequence const & bytes)
	{
		static_assert(std::is_same_v<typename ByteSequence::value_type, char>);

		ByteSequence result{};
		to_byte_lowercase(bytes, std::back_inserter(result));

		return result;
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
		static_assert(std::is_same_v<typename std::iterator_traits<InputIterator>::value_type, char>);

		while (first != last)
		{
			unsigned char ch = *first++;
			if (0x61 <= ch && ch <= 0x7A) ch -= 0x20;
			*output++ = ch;
		}
	}

	template <typename ByteSequence, typename OutputIterator>
	inline void to_byte_uppercase(ByteSequence const & bytes, OutputIterator output)
	{
		static_assert(std::is_same_v<typename ByteSequence::value_type, char>);

		to_byte_uppercase(bytes.begin(), bytes.end(), output);
	}

	template <typename OutputIterator>
	inline void to_byte_uppercase(char const * bytes, OutputIterator output)
	{
		to_byte_uppercase(std::string_view{ bytes }, output);
	}

	template <typename ByteSequence>
	inline ByteSequence to_byte_uppercase(ByteSequence const & bytes)
	{
		static_assert(std::is_same_v<typename ByteSequence::value_type, char>);

		ByteSequence result{};
		to_byte_uppercase(bytes, std::back_inserter(result));

		return result;
	}

	/*! 大文字と小文字を区別しない比較。
	*/
	template <typename InputIterator1, typename InputIterator2>
	inline bool is_byte_case_insensitive_match(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2)
	{
		static_assert(std::is_same_v<typename std::iterator_traits<InputIterator1>::value_type, char>);
		static_assert(std::is_same_v<typename std::iterator_traits<InputIterator2>::value_type, char>);

		std::string lower1{};
		to_byte_lowercase(first1, last1, std::back_inserter(lower1));
		std::string lower2{};
		to_byte_lowercase(first2, last2, std::back_inserter(lower2));

		return lower1 == lower2;
	}

	template <typename ByteSequence1, typename ByteSequence2>
	inline bool is_byte_case_insensitive_match(ByteSequence1 const & bytes1, ByteSequence2 const & bytes2)
	{
		static_assert(std::is_same_v<typename ByteSequence1::value_type, char>);
		static_assert(std::is_same_v<typename ByteSequence2::value_type, char>);

		return is_byte_case_insensitive_match(bytes1.begin(), bytes1.end(), bytes2.begin(), bytes2.end());
	}

	template <typename ByteSequence>
	inline bool is_byte_case_insensitive_match(ByteSequence const & bytes1, char const * bytes2)
	{
		static_assert(std::is_same_v<typename ByteSequence::value_type, char>);

		return is_byte_case_insensitive_match(bytes1, std::string_view{ bytes2 });
	}

	template <typename ByteSequence>
	inline bool is_byte_case_insensitive_match(char const * bytes1, ByteSequence const & bytes2)
	{
		static_assert(std::is_same_v<typename ByteSequence::value_type, char>);

		return is_byte_case_insensitive_match(std::string_view{ bytes1 }, bytes2);
	}

	/*! 同型にデコード。

	出力の範囲チェックは行わないため、十分な容量が確保されているか、あるいは動的に確保される必要がある。
	*/
	template <typename InputIterator, typename OutputIterator>
	inline typename std::enable_if_t<std::is_same_v<typename InputIterator::value_type, char>, void>
	to_isomorphic_decoded(InputIterator first, InputIterator last, OutputIterator output)
	{
		static_assert(std::is_same_v<typename std::iterator_traits<InputIterator>::value_type, char>);

		while (first != last) *output++ = static_cast<unsigned char>(*first++);
	}

	// 4.5. Code points -------------------------------------------------------

	inline bool is_surrogate(uint32_t cp)
	{
		return 0xD800 <= cp && cp <= 0xDFFF;
	}

	inline bool is_scalar_value(uint32_t cp) { return ! is_surrogate(cp); }

	inline bool is_noncharacter(uint32_t cp)
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

	inline bool is_ascii_char(uint32_t cp)
	{
		return 0 <= cp && cp <= 0x7F;
	}

	inline bool is_ascii_tab_or_newline(uint32_t cp)
	{
		return cp == 0x9 || cp == 0xA || cp == 0xD;
	}

	inline bool is_ascii_white_space(uint32_t cp)
	{
		return cp == 0x9 || cp == 0xA || cp == 0xC || cp == 0xD || cp == 0x20;
	}

	inline bool is_c0_control(uint32_t cp)
	{
		return cp <= 0x1F;
	}

	inline bool is_c0_control_or_space(uint32_t cp)
	{
		return is_c0_control(cp) || cp == 0x20;
	}

	inline bool is_control(uint32_t cp)
	{
		return is_c0_control(cp) || (0x7F <= cp && cp <= 0x9F);
	}

	inline bool is_ascii_digit(uint32_t cp)
	{
		return 0x30 <= cp && cp <= 0x39;
	}

	inline bool is_ascii_upper_hex_digit(uint32_t cp)
	{
		return is_ascii_digit(cp) || (0x41 <= cp && cp <= 0x46);
	}

	inline bool is_ascii_lower_hex_digit(uint32_t cp)
	{
		return is_ascii_digit(cp) || (0x61 <= cp && cp <= 0x66);
	}

	inline bool is_ascii_hex_digit(uint32_t cp)
	{
		return is_ascii_upper_hex_digit(cp) || is_ascii_lower_hex_digit(cp);
	}

	inline bool is_ascii_upper_alpha(uint32_t cp)
	{
		return 0x41 <= cp && cp <= 0x5A;
	}

	inline bool is_ascii_lower_alpha(uint32_t cp)
	{
		return 0x61 <= cp && cp <= 0x7A;
	}

	inline bool is_ascii_alpha(uint32_t cp)
	{
		return is_ascii_upper_alpha(cp) || is_ascii_lower_alpha(cp);
	}

	inline bool is_ascii_alphanumeric(uint32_t cp)
	{
		return is_ascii_digit(cp) || is_ascii_alpha(cp);
	}

	// 4.6. Strings -----------------------------------------------------------

	template <typename InputIterator, typename OutputIterator>
	inline auto to_isomorphic_encoded(InputIterator first, InputIterator last, OutputIterator output)
		-> typename std::enable_if_t<std::conjunction_v<
			std::is_same<typename std::iterator_traits<InputIterator>::value_type, char32_t>,
			std::disjunction<
				std::is_same<typename std::iterator_traits<OutputIterator>::value_type, char>,
				std::is_same<typename std::iterator_traits<OutputIterator>::value_type, void>>>, void>
	{
		static_assert(std::is_same_v<typename std::iterator_traits<InputIterator>::value_type, char32_t>);
		static_assert(std::is_same_v<typename std::iterator_traits<OutputIterator>::value_type, char>
			|| std::is_same_v<typename std::iterator_traits<OutputIterator>::value_type, void>); // std::back_inserter用

		while (first != last)
		{
			assert(*first <= 0xFF);
			*output++ = static_cast<unsigned char>(*first++);
		}
	}

	template <typename InputIterator, typename Container>
	inline auto to_isomorphic_encoded(InputIterator first, InputIterator last, Container & output)
		-> typename std::enable_if_t<std::conjunction_v<
			std::is_same<typename std::iterator_traits<InputIterator>::value_type, char32_t>,
			std::is_same<typename Container::value_type, char>>, void>
	{
		static_assert(std::is_same_v<typename std::iterator_traits<InputIterator>::value_type, char32_t>);
		static_assert(std::is_same_v<typename Container::value_type, char>);

		while (first != last)
		{
			assert(*first <= 0xFF);
			output.push_back(static_cast<unsigned char>(*first++));
		}
	}

	template <typename InputIterator, typename OutputIterator>
	inline auto to_isomorphic_encoded(InputIterator first, InputIterator last, OutputIterator output)
		-> typename std::enable_if_t< std::conjunction_v<
			std::is_same<typename std::iterator_traits<InputIterator>::value_type, char>,
			std::disjunction<
				std::is_same<typename std::iterator_traits<OutputIterator>::value_type, char>,
				std::is_same<typename std::iterator_traits<OutputIterator>::value_type, void>>>, void>
	{
		static_assert(std::is_same_v<typename std::iterator_traits<InputIterator>::value_type, char>);
		static_assert(std::is_same_v<typename std::iterator_traits<OutputIterator>::value_type, char>
			|| std::is_same_v<typename std::iterator_traits<OutputIterator>::value_type, void>); // std::back_inserter用

		while (first != last) *output++ = static_cast<unsigned char>(*first++);
	}

	template <typename InputIterator, typename Container>
	inline auto to_isomorphic_encoded(InputIterator first, InputIterator last, Container & output)
		-> typename std::enable_if_t<std::conjunction_v<
			std::is_same<typename std::iterator_traits<InputIterator>::value_type, char>,
			std::is_same<typename Container::value_type, char>>, void>
	{
		static_assert(std::is_same_v<typename std::iterator_traits<InputIterator>::value_type, char>);
		static_assert(std::is_same_v<typename Container::value_type, char>);

		while (first != last)
		{
			assert(*first <= 0xFF);
			output.push_back(*first++);
		}
	}

	template <typename InputIterator>
	inline auto	is_ascii_string(InputIterator first, InputIterator last)
		-> typename std::enable_if_t<std::is_same_v<typename InputIterator::value_type, char32_t>, bool>
	{
		static_assert(std::is_same_v<typename std::iterator_traits<InputIterator>::value_type, char32_t>);

		while (first != last) if (!is_ascii_char(*first++)) return false;
		return true;
	}

	template <typename InputIterator>
	inline auto is_ascii_string(InputIterator first, InputIterator last)
		-> typename std::enable_if_t<std::is_same_v<typename InputIterator::value_type, char>, bool>
	{
		static_assert(std::is_same_v<typename std::iterator_traits<InputIterator>::value_type, char>);

		while (first != last) if (!is_ascii_char(static_cast<unsigned char>(*first++))) return false;
		return true;
	}

	template <typename InputIterator, typename OutputIterator>
	inline auto to_ascii_lowercase()
		-> typename std::enable_if_t<std::is_same_v<typename std::iterator_traits<InputIterator>::value_type, char32_t>, void>
	{

	}
}
