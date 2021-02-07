#pragma once

// --------------------------------------------------------------------------------------------
// 1. Infrastructure
// 
// https://url.spec.whatwg.org/#infrastructure
// https://triple-underscore.github.io/URL-ja.html#infrastructure
// --------------------------------------------------------------------------------------------

#include <algorithm>
#include <array>
#include <charconv>
#include <iterator>
#include <string>
#include <system_error>
#include <type_traits>

#include <wordring/whatwg/infra/infra.hpp>
#include <wordring/whatwg/encoding/encoding.hpp>

namespace wordring::whatwg
{

	// --------------------------------------------------------------------------------------------
	// 1. Infrastructure
	//
	// https://url.spec.whatwg.org/#infrastructure
	// https://triple-underscore.github.io/URL-ja.html#infrastructure
	// --------------------------------------------------------------------------------------------

	/*! @brief 整数を直列化する
	* 
	* @sa https://triple-underscore.github.io/URL-ja.html#serialize-an-integer
	*/
	template <typename OutputIterator>
	inline void serialize_integer(char32_t cp, OutputIterator out)
	{
		char bytes[10];
		std::to_chars_result ret = std::to_chars(bytes, bytes + 10, static_cast<unsigned>(cp), 10);
		assert(ret.ec == std::errc{});
		std::copy(bytes, ret.ptr, out);
	}

	// --------------------------------------------------------------------------------------------
	// 1.3. Percent-encoded bytes
	//
	// https://url.spec.whatwg.org/#percent-encoded-bytes
	// https://triple-underscore.github.io/URL-ja.html#percent-encoded-bytes
	// --------------------------------------------------------------------------------------------

	/*! @brief [first, last) がパーセント・エンコード・バイトから始まる場合、 true を返す
	* 
	* @sa https://triple-underscore.github.io/URL-ja.html#percent-encoded-byte
	*/
	template <typename InputIterator>
	inline bool starts_with_percent_encoded_byte(InputIterator first, InputIterator last)
	{
		if (std::distance(first, last) < 3) return false;
		if (*first++ != 0x25u || !is_ascii_hex_digit(*first++) || !is_ascii_hex_digit(*first++)) return false;
		return true;
	}

	/*! [first, last) 全体がパーセント・エンコード・バイトの場合、 true を返す
	* 
	* @sa https://triple-underscore.github.io/URL-ja.html#percent-encoded-byte
	*/
	template <typename InputIterator>
	inline bool is_percent_encoded_byte(InputIterator first, InputIterator last)
	{
		if (std::distance(first, last) != 3) return false;
		return starts_with_percent_encoded_byte(first, last);
	}

	/*! 一つのバイトをパーセント・エンコードする
	* 
	* @sa https://triple-underscore.github.io/URL-ja.html#percent-encode
	*/
	template <typename OutputIterator>
	inline void percent_encode(uint8_t byte, OutputIterator output)
	{
		char bytes[3]{ '%', '0', '0' };
		std::to_chars_result ret = std::to_chars(bytes + 1, bytes + 3, byte, 16);
		assert(ret.ec == std::errc{});
		if (ret.ptr != bytes + 3) std::swap(bytes[1], bytes[2]);
		to_ascii_uppercase(bytes, bytes + 3, output);
	}

	/*! バイト列をバイト列にパーセント・デコードする
	* 
	* std::string → std::string
	* *
	* @sa https://triple-underscore.github.io/URL-ja.html#percent-decode
	*/
	template <typename InputIterator, typename OutputIterator>
	inline void percent_decode(InputIterator first, InputIterator last, OutputIterator output)
	{
		static_assert(sizeof(*first) == 1);

		while (first != last)
		{
			if (starts_with_percent_encoded_byte(first, last))
			{
				std::uint8_t val{};
				char bytes[3]{};
				for (std::uint32_t i = 0; i < 3; i++) bytes[i] = static_cast<unsigned char>(*first++);
				[[maybe_unused]] std::from_chars_result ret = std::from_chars(bytes + 1, bytes + 3, val, 16);
				assert(ret.ec == std::errc{});
				*output++ = val;
				continue;
			}
			*output++ = *first++;
		}
	}

	/*! 文字列をバイト列にパーセント・デコードする
	* 
	* std::u32string → std::string
	* 
	* @sa https://triple-underscore.github.io/URL-ja.html#string-percent-decode
	*/
	template <typename InputIterator, typename OutputIterator>
	inline void string_percent_decode(InputIterator first, InputIterator last, OutputIterator output)
	{
		static_assert(sizeof(*first) == 4);

		encoding::io_queue<char32_t> in_q = encoding::to_io_queue_convert(first, last);
		encoding::io_queue<char> out_q;
		encoding::utf8_encode(in_q, out_q);

		std::u8string tmp;
		encoding::from_io_queue_convert(out_q, std::back_inserter(tmp));
		percent_decode(tmp.begin(), tmp.end(), output);
	}

	/*! @brief C0 制御文字 % エンコーディング集合
	* 
	* @sa https://triple-underscore.github.io/URL-ja.html#c0-control-percent-encode-set
	*/
	inline bool is_c0_control_percent_encode_set(char32_t cp)
	{
		return is_c0_control(cp) || 0x7Eu < cp;
	}

	/*! @brief フラグメント % エンコーディング集合
	* 
	* @sa https://triple-underscore.github.io/URL-ja.html#fragment-percent-encode-set
	*/
	inline bool is_fragment_percent_encode_set(char32_t cp)
	{
		if (is_c0_control_percent_encode_set(cp)) return true;
		switch (cp)
		{
		case U'\x20':
		case U'\x22':
		case U'\x3C':
		case U'\x3E':
		case U'\x60':
			return true;
		}
		return false;
	}

	/*! @brief QUERY % エンコーディング集合
	* 
	* @sa https://triple-underscore.github.io/URL-ja.html#query-percent-encode-set
	*/
	inline bool is_query_percent_encode_set(char32_t cp)
	{
		if (is_c0_control_percent_encode_set(cp)) return true;
		switch (cp)
		{
		case U'\x20':
		case U'\x22':
		case U'\x23':
		case U'\x3C':
		case U'\x3E':
			return true;
		}
		return false;
	}

	/*! @brief 特別 QUERY % エンコーディング集合
	*
	* @sa https://triple-underscore.github.io/URL-ja.html#special-query-percent-encode-set
	*/
	inline bool is_special_query_percent_encode_set(char32_t cp)
	{
		if (is_query_percent_encode_set(cp) || cp == U'\x27') return true;
		return false;
	}

	/*! @brief PATH % エンコーディング集合
	* 
	* @sa https://triple-underscore.github.io/URL-ja.html#path-percent-encode-set
	*/
	inline bool is_path_percent_encode_set(char32_t cp)
	{
		if (is_fragment_percent_encode_set(cp)) return true;
		switch (cp)
		{
		case U'\x23':
		case U'\x3F':
		case U'\x7B':
		case U'\x7D':
			return true;
		}
		return false;
	}

	/*! @brie USERINFO % エンコーディング集合
	* 
	* @sa https://triple-underscore.github.io/URL-ja.html#userinfo-percent-encode-set
	*/
	inline bool is_userinfo_percent_encode_set(char32_t cp)
	{
		if (is_path_percent_encode_set(cp)) return true;
		switch (cp)
		{
		case U'\x2F':
		case U'\x3A':
		case U'\x3B':
		case U'\x3D':
		case U'\x40':
		case U'\x5B':
		case U'\x5C':
		case U'\x5D':
		case U'\x5E':
		case U'\x7C':
			return true;
		}
		return false;
	}

	/*! @brief コンポーネント % エンコーディング集合
	*
	* @sa https://triple-underscore.github.io/URL-ja.html#component-percent-encode-set
	*/
	inline bool is_component_percent_encode_set(char32_t cp)
	{
		if (is_userinfo_percent_encode_set(cp)) return true;
		switch (cp)
		{
		case U'\x24':
		case U'\x25':
		case U'\x26':
		case U'\x2B':
		case U'\x2C':
			return true;
		}
		return false;
	}

	/*! @brief FORM URL エンコード % エンコーディング集合
	* 
	* @sa https://triple-underscore.github.io/URL-ja.html#application-x-www-form-urlencoded-percent-encode-set
	*/
	inline bool application_x_www_form_urlencoded_percent_encode_set(char32_t cp)
	{
		if (is_component_percent_encode_set(cp)) return true;
		switch (cp)
		{
		case U'\x21':
		case U'\x27':
		case U'\x28':
		case U'\x29':
		case U'\x7E':
			return true;
		}
		return false;
	}

	/*! @brief 文字列をエンコーディング後、 % エンコーディングする
	* 
	* @sa https://triple-underscore.github.io/URL-ja.html#string-percent-encode-after-encoding
	*/
	template <typename PercentEncodeSet>
	inline std::u32string percent_encode_after_encoding(
		encoding_name name, std::u32string const& in, PercentEncodeSet set, bool spaceAsPlus = false)
	{
		using namespace wordring::whatwg::encoding;
		// 1.
		coder c = get_encoder(name);
		// 2.
		io_queue<char32_t> inputQueue = to_io_queue_convert(in.begin(), in.end());
		// 3.
		std::u32string output;
		// 4.
		result_value potentialError = result_error{};
		// 5.
		while (std::holds_alternative<result_error>(potentialError))
		{
			// 5.1.
			io_queue<char> encodeOutput;
			// 5.2.
			potentialError = encode_or_fail(inputQueue, c, encodeOutput); // result_error{} あるいは null を意味する result_finished{} 
			// 5.3.
			std::string s;
			from_io_queue_convert(encodeOutput, std::back_inserter(s));
			for (std::uint8_t byte : s)
			{
				// 1.
				if (spaceAsPlus && byte == '\x20')
				{
					output.push_back('\x2B');
					continue;
				}
				// 2.
				char32_t isomorph = byte;
				// 3.
				// 4.
				if (!set(isomorph)) output.push_back(static_cast<std::uint8_t>(isomorph));
				// 5.
				else
				{
					std::string pe;
					percent_encode(byte, std::back_inserter(pe));
					output.insert(output.end(), pe.begin(), pe.end());
				}
			}
			// 5.4.
			if (std::holds_alternative<result_error>(potentialError))
			{
				std::string_view sv1 = "%26%23";
				output.insert(output.end(), sv1.begin(), sv1.end());
				char32_t cp = std::get_if<result_error>(&potentialError)->code_point;
				serialize_integer(cp, std::back_inserter(output));
				std::string_view sv2 = "%3B";
				output.insert(output.end(), sv2.begin(), sv2.end());
			}
		}
		// 6.
		return output;
	}

	/*! コードポイントを UTF-8 パーセント・エンコードする
	* 
	* @sa https://url.spec.whatwg.org/#utf-8-percent-encode
	*/
	template <typename PercentEncodeSet>
	inline std::u32string utf8_percent_encode(char32_t cp, PercentEncodeSet set)
	{
		return percent_encode_after_encoding(encoding_name::UTF_8, std::u32string(1, cp), set);
	}

	/*! 文字列を UTF-8 パーセント・エンコードする
	*
	* @sa https://url.spec.whatwg.org/#utf-8-percent-encode
	*/
	template <typename PercentEncodeSet>
	inline std::u32string utf8_percent_encode(std::u32string const& s, PercentEncodeSet set)
	{
		return percent_encode_after_encoding(encoding_name::UTF_8, s, set);
	}
}
