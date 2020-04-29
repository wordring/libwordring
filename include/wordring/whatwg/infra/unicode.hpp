#pragma once

// https://infra.spec.whatwg.org/
// https://triple-underscore.github.io/infra-ja.html

#include <cassert>
#include <cstdint>
#include <iterator>
#include <string>
#include <string_view>
#include <type_traits>

#include <wordring/compatibility.hpp>

namespace wordring::whatwg
{
	// ------------------------------------------------------------------------

	/*!
	@brief 文字列からコードポイントを一つ読み取る。

	@tparam InputIterator 入力イテレータの型

	@param [in]  first  読み取りの開始位置を示すイテレータ。
	@param [in]  last   読み取りの終端を示すイテレータ。
	@param [out] output ユニコード・コードポイント。

	@return
		文字を読み取った分だけ前進させたイテレータを返す。
		通常、返されたイテレータは、次の読み取り開始位置となる。
		例として、 utf-8 文字列では、一つのコードポイントを得るために、1～4バイトを必要とする。
		戻り値はfirstをその分だけ前進したものとなる。

	@details
		この関数は、ライブラリ内部で使用するために作成された。
		入力は、コードポイント列から作成されたクリーンな文字列の必要がある。
		エラーチェックは行われない。
	*/
	template <typename InputIterator,
		typename std::enable_if_t<std::is_same_v<typename std::iterator_traits<InputIterator>::value_type, char8_t>, std::nullptr_t> = nullptr>
	inline InputIterator to_code_point(InputIterator first, InputIterator last, char32_t& output)
	{
		assert(first != last);
		unsigned char ch1 = static_cast<unsigned char>(*first++);
		if ((ch1 & 0x80u) == 0u)
		{
			output = static_cast<char32_t>(ch1);
			return first;
		}

		assert(first != last);
		unsigned char ch2 = static_cast<unsigned char>(*first++);
		assert((ch2 & 0xC0u) == 0x80u);
		if ((ch1 & 0xE0u) == 0xC0u)
		{
			output = static_cast<char32_t>(((ch1 & 0x1Fu) << 6) + (ch2 & 0x3Fu));
			return first;
		}

		assert(first != last);
		unsigned char ch3 = static_cast<unsigned char>(*first++);
		assert((ch3 & 0xC0u) == 0x80u);
		if ((ch1 & 0xF0u) == 0xE0u)
		{
			output = static_cast<char32_t>(((ch1 & 0xFu) << 12) + ((ch2 & 0x3Fu) << 6) + (ch3 & 0x3Fu));
			return first;
		}

		assert(first != last);
		unsigned char ch4 = static_cast<unsigned char>(*first++);
		assert((ch4 & 0xC0u) == 0x80u);
		if ((ch1 & 0xF8u) == 0xF0u)
		{
			output = static_cast<char32_t>(((ch1 & 0x7u) << 18) + ((ch2 & 0x3Fu) << 12) + ((ch3 & 0x3Fu) << 6) + (ch4 & 0x3Fu));
			return first;
		}

		assert(false);
		return last;
	}

	template <typename InputIterator,
		typename std::enable_if_t<std::is_same_v<typename std::iterator_traits<InputIterator>::value_type, char16_t>, nullptr_t> = nullptr>
	inline InputIterator to_code_point(InputIterator first, InputIterator last, char32_t& output)
	{
		assert(first != last);
		uint16_t ch1 = *first++;
		if ((ch1 & 0xFC00u) != 0xD800u)
		{
			output = static_cast<char32_t>(ch1);
			return first;
		}

		assert(first != last);
		uint16_t ch2 = *first++;
		assert((ch2 & 0xFC00u) == 0xDC00u);
		output = 0x10000u + (ch1 - 0xD800u) * 0x400u + (ch2 - 0xDC00u);
		return first;
	}

	template <typename InputIterator,
		typename std::enable_if_t<std::is_same_v<typename std::iterator_traits<InputIterator>::value_type, char32_t>, nullptr_t> = nullptr>
	inline InputIterator to_code_point(InputIterator first, InputIterator last, char32_t& output)
	{
		assert(first != last);
		output = *first++;
		return first;
	}

	// ------------------------------------------------------------------------

	/*!
	@brief ユニコード・コードポイントから文字列に変換する。

	@tparam OutputIterator 出力イテレータの型。

	@param [in] ch ユニコード・コードポイント。
	@param [out] output 出力先となる出力イテレータ。

	この関数はライブラリ内部での利用を意図する。
	入力にエラーがある場合を考慮しない。

	OutputIteratorのvalue_typeの型に応じて文字符号化が決定される。
	std::back_insert_iterator<>が与えられた場合、container_type::value_typeを参照する。
	- char8_t/charの場合、utf-8。
	- char16_tの場合、utf-16。
	- char32_t/scalar_valueの場合、utf-32。

	utf-8文字符号化は、処理系がchar8_tをサポートする場合char8_t、しない場合charで有効化される。
	*/
	template <typename OutputIterator,
		typename std::enable_if_t<
			std::conjunction_v<
				std::is_same<typename std::iterator_traits<OutputIterator>::iterator_category, std::output_iterator_tag>,
				std::disjunction<
					std::is_same<typename std::iterator_traits<OutputIterator>::value_type, char8_t>,
					std::is_same<typename OutputIterator::container_type::value_type, char8_t>>>, nullptr_t> = nullptr>
	inline void to_string(char32_t ch, OutputIterator output)
	{
		if ((ch & 0xFFFFFF80u) == 0) *output++ = static_cast<unsigned char>(ch); // 一バイト文字
		else if ((ch & 0xFFFFF800u) == 0) // 二バイト文字
		{
			*output++ = static_cast<unsigned char>(0xC0u + (ch >> 6));
			*output++ = static_cast<unsigned char>(0x80u + (ch & 0x3Fu));
		}
		else if ((ch & 0xFFFF0000u) == 0) // 三バイト文字
		{
			*output++ = static_cast<unsigned char>(0xE0u + (ch >> 12));
			*output++ = static_cast<unsigned char>(0x80u + ((ch & 0xFC0u) >> 6));
			*output++ = static_cast<unsigned char>(0x80u + (ch & 0x3Fu));
		}
		else if ((ch & 0xFFE00000u) == 0) // 四バイト文字
		{
			*output++ = static_cast<unsigned char>(0xF0u + (ch >> 18));
			*output++ = static_cast<unsigned char>(0x80u + ((ch & 0x3F000u) >> 12));
			*output++ = static_cast<unsigned char>(0x80u + ((ch & 0xFC0u) >> 6));
			*output++ = static_cast<unsigned char>(0x80u + (ch & 0x3Fu));
		}
		else assert(false);
	}

	template <typename OutputIterator,
		typename std::enable_if_t<
			std::conjunction_v<
				std::is_same<typename std::iterator_traits<OutputIterator>::iterator_category, std::output_iterator_tag>,
				std::disjunction<
					std::is_same<typename std::iterator_traits<OutputIterator>::value_type, char16_t>,
					std::is_same<typename OutputIterator::container_type::value_type, char16_t>>>, nullptr_t> = nullptr>
	inline void to_string(char32_t ch, OutputIterator output)
	{
		if ((ch & 0xFFFF0000u) == 0) *output++ = static_cast<char16_t>(ch);
		else if ((ch & 0xFFE00000u) == 0)
		{
			*output++ = static_cast<char16_t>((ch - 0x10000u) / 0x400u + 0xD800u);
			*output++ = static_cast<char16_t>((ch - 0x10000u) % 0x400u + 0xDC00u);
		}
		else assert(false);
	}

	template <typename OutputIterator,
		typename std::enable_if_t<
			std::conjunction_v<
				std::is_same<typename std::iterator_traits<OutputIterator>::iterator_category, std::output_iterator_tag>,
				std::disjunction<
					std::is_same<typename std::iterator_traits<OutputIterator>::value_type, char32_t>,
					std::is_same<typename OutputIterator::container_type::value_type, char32_t>>>, nullptr_t> = nullptr>
	inline void to_string(char32_t ch, OutputIterator output) { *output++ = ch; }

	// ------------------------------------------------------------------------

	template <typename String, typename Container>
	inline void encoding_cast(String const& in, std::back_insert_iterator<Container> out)
	{
		auto it = std::begin(in);
		while (it != std::end(in))
		{
			// 文字列リテラルの場合
			if constexpr (std::is_array_v<std::remove_cv_t<std::remove_reference_t<String>>>) if (*it == 0) break;

			char32_t cp;

			if constexpr (std::is_same_v<std::remove_cv_t<std::remove_reference_t<String>>, std::u32string>) cp = *it++;
			else it = to_code_point(it, std::end(in), cp);

			if constexpr (std::is_same_v<Container, std::u32string>) *out++ = cp;
			else to_string(cp, out);
		}
	}

	template <typename Result, typename String, std::enable_if_t<std::negation_v<std::is_same<Result, String>>, std::nullptr_t> = nullptr>
	inline Result encoding_cast(String const& in)
	{
		Result s;
		encoding_cast(in, std::back_inserter(s));
		return s;
	}

	template <typename Result, typename String, std::enable_if_t<std::is_same_v<Result, String>, std::nullptr_t> = nullptr>
	inline Result encoding_cast(String in)
	{
		return std::forward<String>(in);
	}
}
