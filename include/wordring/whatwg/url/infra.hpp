#pragma once

// https://url.spec.whatwg.org/
// https://triple-underscore.github.io/URL-ja.html

#include <algorithm>
#include <charconv>
#include <iterator>
#include <system_error>
#include <type_traits>

#include <wordring/whatwg/infra/infra.hpp>
#include <wordring/whatwg/encoding/encoding.hpp>

namespace wordring::whatwg
{
	/*! [first, last)がパーセント・エンコード・バイトから始まる場合、trueを返す
	*/
	template <typename InputIterator>
	inline bool starts_with_percent_encoded_byte(InputIterator first, InputIterator last)
	{
		if (std::distance(first, last) < 3) return false;
		if (*first++ != 0x25u || !is_ascii_hex_digit(*first++) || !is_ascii_hex_digit(*first++)) return false;
		return true;
	}

	/*! [first, last)が完全にパーセント・エンコード・バイトの場合、trueを返す
	*/
	template <typename InputIterator>
	inline bool is_percent_encoded_byte(InputIterator first, InputIterator last)
	{
		if (std::distance(first, last) != 3) return false;
		return starts_with_percent_encoded_byte(first, last);
	}

	/*! 一つのバイトをパーセント・エンコードする
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

	/*! バイト列をバイト列にパーセント・デーコードする
	*/
	template <typename InputIterator, typename OutputIterator>
	inline void percent_decode(InputIterator first, InputIterator last, OutputIterator output)
	{
		while (first != last)
		{
			if (starts_with_percent_encoded_byte(first, last))
			{
				uint8_t val{};
				char bytes[3]{};
				for (uint32_t i = 0; i < 3; i++) bytes[i] = static_cast<unsigned char>(*first++);
				[[maybe_unused]] std::from_chars_result ret = std::from_chars(bytes + 1, bytes + 3, val, 16);
				assert(ret.ec == std::errc{});
				*output++ = val;
				continue;
			}
			*output++ = *first++;
		}
	}

	/*! 文字列をバイト列にパーセント・デーコードする
	*/
	template <typename InputIterator, typename OutputIterator>
	inline void string_percent_decode(InputIterator first, InputIterator last, OutputIterator output)
	{
		std::u8string tmp{};
		encoding::utf8_encode(first, last, std::back_inserter(tmp));
		percent_decode(tmp.begin(), tmp.end(), output);
	}

	inline bool is_c0_control_percent_encode_set(uint32_t cp)
	{
		return is_c0_control(cp) || 0x7Eu < cp;
	}

	inline bool is_fragment_percent_encode_set(uint32_t cp)
	{
		if (is_c0_control_percent_encode_set(cp)) return true;
		switch (cp)
		{
		case 0x20u:
		case 0x22u:
		case 0x3Cu:
		case 0x3Eu:
		case 0x60u:
			return true;
		}
		return false;
	}

	inline bool is_path_percent_encode_set(uint32_t cp)
	{
		if (is_fragment_percent_encode_set(cp)) return true;
		switch (cp)
		{
		case 0x23u:
		case 0x3Fu:
		case 0x7Bu:
		case 0x7Du:
			return true;
		}
		return false;
	}

	inline bool is_userinfo_percent_encode_set(uint32_t cp)
	{
		if (is_path_percent_encode_set(cp)) return true;
		switch (cp)
		{
		case 0x2Fu:
		case 0x3Au:
		case 0x3Bu:
		case 0x3Du:
		case 0x40u:
		case 0x5Bu:
		case 0x5Cu:
		case 0x5Du:
		case 0x5Eu:
		case 0x7Cu:
			return true;
		}
		return false;
	}

	/*! 文字列をパーセント・エンコードする
	*/
	template <typename OutputIterator, typename Condition>
	inline void utf_8_percent_encode(uint32_t cp, OutputIterator output, Condition condition)
	{
		if (!condition(cp))
		{
			*output++ = cp;
			return;
		}
		std::string tmp;
		encoding::utf8_encode(&cp, &cp + 1, std::back_inserter(tmp));
		for(uint8_t byte : tmp) percent_encode(byte, output);
	}
}
