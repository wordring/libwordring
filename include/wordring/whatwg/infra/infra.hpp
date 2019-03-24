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
#include <wordring/whatwg/infra/scalar_value.hpp>

namespace wordring::whatwg
{
	// ------------------------------------------------------------------------

	/*!
	@brief 同値にキャストする。

	@tparam Result 戻り値の型
	@tparam Character 引数の型

	@param [in] ch Character 型の整数

	@return 引数 ch と同じ値を持つ Result 型の整数

	@details
		Character 型の引数 ch を Result 型にキャストして返す。

	@par 使い方
	@code
		char32_t ch1{ U'a' };
		char ch2 = isomorphic_cast<unsigned char>(ch1);

		scalar_value sv1{ U'b' };
		char ch3 = isomorphic_cast<unsigned char>(sv1);
	@endcode

	@internal
		- 単純にstatic_cast<>するのみだが、scalar_valueの変換演算子がimplicitなので明示的にオーバーロードする。
		- 呼び出し側のテンプレート解決を単純化するために用意した。
		- ライブラリ内部からの呼び出しを想定する。
	*/
	template <typename Result, typename Character>
	constexpr Result isomorphic_cast(Character ch) noexcept { return static_cast<Result>(ch); }

	/*!
	@brief 同値にキャストする。

	@tparam Result 戻り値の型

	@param [in] ch ユニコード・スカラー値

	@return 引数 ch と同じ値を持つ Result 型の整数

	@sa isomorphic_cast(Character)

	@overload
	*/
	template <typename Result>
	constexpr Result isomorphic_cast(scalar_value ch) noexcept { return static_cast<Result>(static_cast<char32_t>(ch)); }

	// ------------------------------------------------------------------------

	/*!
	@brief 文字列からコードポイントを一つ読み取る。

	@tparam InputIterator 入力イテレータの型

	@param [in,out] first
		読み取りの開始位置を示すイテレータ。
		読み取った分だけ前進する。
	@param [in] last 読み取りの終端を示すイテレータ。

	@return ユニコード・コードポイント。

	@details
		この関数は、必要な分だけ first を前進し、一つのコードポイントを返す。
		例として、 utf-8 文字列では、一つのコードポイントを得るために、1～4バイトを必要とする。

		この関数は、ライブラリ内部で使用するために作成された。
		入力は、コードポイント列から作成されたクリーンな文字列の必要がある。
		エラーチェックは行われない。

	@internal
		読み取った位置を通知する必要があるので、コンテナ・バージョンは用意しない。
	*/
	template <typename InputIterator,
		typename std::enable_if_t<std::is_same_v<typename std::iterator_traits<InputIterator>::value_type, char_utf8_type>, std::nullptr_t> = nullptr>
		inline uint32_t to_codepoint(InputIterator & first, InputIterator last)
	{
		assert(first != last);
		unsigned char ch1 = static_cast<unsigned char>(*first++);
		if ((ch1 & 0x80u) == 0u) return static_cast<uint32_t>(ch1);

		assert(first != last);
		unsigned char ch2 = static_cast<unsigned char>(*first++);
		assert((ch2 & 0xC0u) == 0x80u);
		if ((ch1 & 0xE0u) == 0xC0u) return static_cast<uint32_t>(((ch1 & 0x1Fu) << 6) + (ch2 & 0x3Fu));

		assert(first != last);
		unsigned char ch3 = static_cast<unsigned char>(*first++);
		assert((ch3 & 0xC0u) == 0x80u);
		if ((ch1 & 0xF0u) == 0xE0u) return static_cast<uint32_t>(((ch1 & 0xFu) << 12) + ((ch2 & 0x3Fu) << 6) + (ch3 & 0x3Fu));

		assert(first != last);
		unsigned char ch4 = static_cast<unsigned char>(*first++);
		assert((ch4 & 0xC0u) == 0x80u);
		if ((ch1 & 0xF8u) == 0xF0u) return static_cast<uint32_t>(((ch1 & 0x7u) << 18) + ((ch2 & 0x3Fu) << 12) + ((ch3 & 0x3Fu) << 6) + (ch4 & 0x3Fu));

		assert(false);
		return 0;
	}

	template <typename InputIterator,
		typename std::enable_if_t<std::is_same_v<typename std::iterator_traits<InputIterator>::value_type, char16_t>, nullptr_t> = nullptr>
		inline uint32_t	to_codepoint(InputIterator & first, InputIterator last)
	{
		assert(first != last);
		uint16_t ch1 = *first++;
		if ((ch1 & 0xFC00u) != 0xD800u) return static_cast<uint32_t>(ch1);

		assert(first != last);
		uint16_t ch2 = *first++;
		assert((ch2 & 0xFC00u) == 0xDC00u);
		return 0x10000u + (ch1 - 0xD800u) * 0x400u + (ch2 - 0xDC00u);
	}

	template <typename InputIterator,
		typename std::enable_if_t<std::is_same_v<typename std::iterator_traits<InputIterator>::value_type, char32_t>, nullptr_t> = nullptr>
		inline uint32_t to_codepoint(InputIterator & first, InputIterator last)
	{
		assert(first != last);
		return *first++;
	}

	template <typename InputIterator,
		typename std::enable_if_t<std::is_same_v<typename std::iterator_traits<InputIterator>::value_type, scalar_value>, nullptr_t> = nullptr>
		inline uint32_t to_codepoint(InputIterator & first, InputIterator last)
	{
		assert(first != last);
		return static_cast<char32_t>(*first++);
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
				is_iterator<OutputIterator>,
				std::disjunction<
					std::is_same<typename std::iterator_traits<OutputIterator>::value_type, char_utf8_type>,
					std::is_same<typename OutputIterator::container_type::value_type, char_utf8_type>>>, nullptr_t> = nullptr>
		inline void to_string(uint32_t ch, OutputIterator output)
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
				is_iterator<OutputIterator>,
				std::disjunction<
					std::is_same<typename std::iterator_traits<OutputIterator>::value_type, char16_t>,
					std::is_same<typename OutputIterator::container_type::value_type, char16_t>>>, nullptr_t> = nullptr>
		inline void to_string(uint32_t ch, OutputIterator output)
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
				is_iterator<OutputIterator>,
				std::disjunction<
					std::is_same<typename std::iterator_traits<OutputIterator>::value_type, char32_t>,
					std::is_same<typename OutputIterator::container_type::value_type, char32_t>>>, nullptr_t> = nullptr>
		inline void to_string(uint32_t ch, OutputIterator output) { *output++ = ch; }

	template <typename OutputIterator,
		typename std::enable_if_t<
			std::conjunction_v<
				is_iterator<OutputIterator>,
				std::disjunction<
					std::is_same<typename std::iterator_traits<OutputIterator>::value_type, scalar_value>,
					std::is_same<typename OutputIterator::container_type::value_type, scalar_value>>>, nullptr_t> = nullptr>
		inline void to_string(uint32_t ch, OutputIterator output) { *output++ = scalar_value{ ch }; }

	/*!
	@brief ユニコード・コードポイントから文字列に変換する。

	@tparam Container 出力先となるコンテナの型。

	@param [in] ch ユニコード・コードポイント。
	@param [out] output 出力先のコンテナ。

	入力されたユニコード・コードポイントchに対応する文字列をoutputの末尾に追加する。
	この関数はライブラリ内部での利用を意図する。
	入力にエラーがある場合を考慮しない。

	Container::value_typeの型に応じて文字符号化が決定される。
	- char8_t/charの場合、utf-8。
	- char16_tの場合、utf-16。
	- char32_tの場合、utf-32。

	utf-8文字符号化は、処理系がchar8_tをサポートする場合char8_t、しない場合charで有効化される。

	@sa to_string(uint32_t, OutputIterator)
	*/
	template <typename Container,
		std::enable_if_t<is_container_v<Container>, nullptr_t> = nullptr>
		inline void to_string(uint32_t ch, Container & output)
	{
		to_string(ch, std::back_inserter(output));
	}


	// 4.3. Bytes -------------------------------------------------------------

	constexpr bool is_ascii_byte(char ch) noexcept { return static_cast<unsigned char>(ch) <= 0x7F; }

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
			if (0x41u <= ch && ch <= 0x5Au) ch += 0x20u;
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
			if (0x61u <= ch && ch <= 0x7Au) ch -= 0x20u;
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
	template <typename InputIterator, typename OutputIterator,
		typename std::enable_if_t<is_iterator_v<OutputIterator>, std::nullptr_t> = nullptr>
		inline void isomorphic_decode(InputIterator first, InputIterator last, OutputIterator output)
	{
		static_assert(std::is_same_v<typename std::iterator_traits<InputIterator>::value_type, char>);

		while (first != last) *output++ = static_cast<unsigned char>(*first++);
	}

	template <typename InputIterator, typename Container,
		typename std::enable_if_t<is_container_v<Container>, std::nullptr_t> = nullptr>
		inline void isomorphic_decode(InputIterator first, InputIterator last, Container & output)
	{
		static_assert(std::is_same_v<typename std::iterator_traits<InputIterator>::value_type, char>);

		while (first != last) output.push_back(static_cast<unsigned char>(*first++));
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

	inline bool is_ascii_char(uint32_t cp) noexcept
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

	template <typename InputIterator, typename OutputIterator,
		typename std::enable_if_t<is_iterator_v<OutputIterator>, std::nullptr_t> = nullptr>
		inline void isomorphic_encode(InputIterator first, InputIterator last, OutputIterator output)
	{
		while (first != last)
		{
			assert(*first <= 0xFFu);
			*output++ = isomorphic_cast<unsigned char>(*first++);
		}
	}

	template <typename InputIterator, typename Container,
		typename std::enable_if_t<is_container_v<Container>, std::nullptr_t> = nullptr>
		inline void isomorphic_encode(InputIterator first, InputIterator last, Container & output)
	{
		while (first != last)
		{
			assert(*first <= 0xFFu);
			output.push_back(isomorphic_cast<unsigned char>(*first++));
		}
	}

	template <typename InputIterator,
		typename std::enable_if_t<is_iterator_v<InputIterator>, std::nullptr_t> = nullptr>
		inline bool	is_ascii_string(InputIterator first, InputIterator last)
	{
		while (first != last) if (0x7Fu < to_codepoint(first, last)) return false;
		return true;
	}

	template <typename Container,
		typename std::enable_if_t<is_container_v<Container>, std::nullptr_t> = nullptr>
		inline bool	is_ascii_string(Container const & str)
	{
		return is_ascii_string(str.begin(), str.end());
	}

	template <typename InputIterator, typename OutputIterator,
		typename std::enable_if_t<std::conjunction_v<is_iterator<InputIterator>, is_iterator<OutputIterator>>, std::nullptr_t> = nullptr>
		inline void to_ascii_lowercase(InputIterator first, InputIterator last, OutputIterator output)
	{
		while (first != last)
		{
			char32_t ch = to_codepoint(first, last);
			if (is_ascii_upper_alpha(ch)) ch += 0x20u;
			to_string(ch, output);
		}
	}

	template <typename InputIterator, typename OutputIterator,
		typename std::enable_if_t<std::conjunction_v<is_iterator<InputIterator>, is_iterator<OutputIterator>>, std::nullptr_t> = nullptr>
		inline void to_ascii_uppercase(InputIterator first, InputIterator last, OutputIterator output)
	{
		while (first != last)
		{
			char32_t ch = to_codepoint(first, last);
			if (is_ascii_lower_alpha(ch)) ch -= 0x20u;
			to_string(ch, output);
		}
	}

	template <typename InputIterator1, typename InputIterator2,
		typename std::enable_if_t<std::conjunction_v<is_iterator<InputIterator1>, is_iterator<InputIterator2>>, std::nullptr_t> = nullptr>
		inline bool is_ascii_case_insensitive_match(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2)
	{
		while (first1 != last1 && first2 != last2)
		{
			uint32_t ch1 = to_codepoint(first1, last1);
			if (is_ascii_upper_alpha(ch1)) ch1 += 0x20u;

			uint32_t ch2 = to_codepoint(first2, last2);
			if (is_ascii_upper_alpha(ch2)) ch2 += 0x20u;

			if (ch1 != ch2) return false;
		}

		return first1 == last1 && first2 == last2;
	}

	template <typename InputIterator, typename OutputIterator>
	inline void ascii_encode(InputIterator first, InputIterator last, OutputIterator output,
		typename std::enable_if_t<is_iterator_v<InputIterator>, nullptr_t> = nullptr)
	{
		assert(is_ascii_string(first, last));

		isomorphic_encode(first, last, output);
	}

	template <typename InputIterator, typename OutputIterator>
	inline void ascii_decode(InputIterator first, InputIterator last, OutputIterator output,
		typename std::enable_if_t<is_iterator_v<InputIterator>, nullptr_t> = nullptr)
	{
		assert([](InputIterator it1, InputIterator it2)->bool
		{
			while (it1 != it2) if (!is_ascii_byte(static_cast<unsigned char>(*it1++))) return false;
			return true;
		}(first, last));

		isomorphic_decode(first, last, output);
	}

	template <typename InputIterator, typename OutputIterator,
		typename std::enable_if_t<is_iterator_v<InputIterator>, nullptr_t> = nullptr>
	inline void to_strip_newlines(InputIterator first, InputIterator last, OutputIterator output)
	{
		while (first != last)
		{
			uint32_t cp = to_codepoint(first, last);
			if (cp != 0xA && cp != 0xD) to_string(cp, output);
		}
	}

	template <typename Container,
		typename std::enable_if_t<is_container_v<Container>, nullptr_t> = nullptr>
	inline Container strip_newlines(Container const & str)
	{
		Container result{};
		to_strip_newlines(str.begin(), str.end(), std::back_inserter(result));
		return result;
	}

	/*!
	- CR(0xD), LF(0xA)
	- CRLF -> LF
	- LF -> LF
	- CR -> LF
	*/
	template <typename InputIterator, typename OutputIterator,
		typename std::enable_if_t<is_iterator_v<InputIterator>, nullptr_t> = nullptr>
	inline void to_normalize_newlines(InputIterator first, InputIterator last, OutputIterator output)
	{
		enum class states : uint32_t { Start, Cr };

		states state{ states::Start };
		while (first != last)
		{
			auto ch = *first++;
			switch (state)
			{
			case states::Start:
				if (ch == 0xDu) state = states::Cr;
				else if (ch == 0xAu) goto LineBreak;
				else goto Output;
				break;
			case states::Cr:
				if (ch == 0xDu) *output++ = static_cast<unsigned char>(0xAu);
				else if (ch == 0xAu) goto LineBreak;
				else
				{
					*output++ = static_cast<unsigned char>(0xAu);
					goto Output;
				}
				break;
			LineBreak:
				ch = static_cast<unsigned char>(0xAu);
			Output:
				*output++ = ch;
				state = states::Start;
				break;
			}
		}
	}


}
