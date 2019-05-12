// whtawg/encoding/coder.hpp

#pragma once

// https://encoding.spec.whatwg.org/
// https://triple-underscore.github.io/Encoding-ja.html

#include <optional>

#include <wordring/whatwg/encoding/encoding.hpp>
#include <wordring/whatwg/encoding/indexes.hpp>
#include <wordring/whatwg/infra/infra.hpp>

namespace wordring::whatwg::encoding
{
	// 8.1.1. UTF-8 decoder
	class UTF_8_decoder : public decoder
	{
	public:
		UTF_8_decoder()
			: UTF_8_code_point{ 0 }
			, UTF_8_bytes_seen{ 0 }
			, UTF_8_bytes_needed{ 0 }
			, UTF_8_lower_boundary{ 0x80 }
			, UTF_8_upper_boundary{ 0xBF }
		{
		}

		template <typename Stream, typename Token>
		result_value run(Stream& input, Token token)
		{
			// 1.
			if ((!token.has_value()) && (UTF_8_bytes_needed != 0)) return result_error{};
			// 2.
			if ((!token.has_value())) return result_finished{};

			uint8_t byte{ static_cast<uint8_t>(token.value()) };
			// 3.
			if (UTF_8_bytes_needed == 0)
			{
				if (0 <= byte && byte <= 0x7F) return static_cast<uint32_t>(byte);
				if (0xC2 <= byte && byte <= 0xDF)
				{
					UTF_8_bytes_needed = 1;
					UTF_8_code_point = byte & 0x1F;
				}
				else if (0xE0 <= byte && byte <= 0xEF)
				{
					if (byte == 0xE0) UTF_8_lower_boundary = 0xA0;
					else if (byte == 0xED) UTF_8_upper_boundary = 0x9F;
					UTF_8_bytes_needed = 2;
					UTF_8_code_point = byte & 0xF;
				}
				else if (0xF0 <= byte && byte <= 0xF4)
				{
					if (byte == 0xF0) UTF_8_lower_boundary = 0x90;
					else if (byte == 0xF4) UTF_8_upper_boundary = 0x8F;
					UTF_8_bytes_needed = 3;
					UTF_8_code_point = byte & 0x7;
				}
				else return result_error{};

				return result_continue{};
			}

			// 4.
			if (!(UTF_8_lower_boundary <= byte && byte <= UTF_8_upper_boundary))
			{
				UTF_8_code_point = 0;
				UTF_8_bytes_seen = 0;
				UTF_8_bytes_needed = 0;

				UTF_8_lower_boundary = 0x80;
				UTF_8_upper_boundary = 0xBF;

				input.prepend(byte);

				return result_error{};
			}

			// 5.
			UTF_8_lower_boundary = 0x80;
			UTF_8_upper_boundary = 0xBF;

			// 6.
			UTF_8_code_point = (UTF_8_code_point << 6) | (byte & 0x3F);

			// 7.
			++UTF_8_bytes_seen;

			// 8.
			if (UTF_8_bytes_seen != UTF_8_bytes_needed) return result_continue{};

			// 9.
			uint32_t cp{ UTF_8_code_point };

			// 10.
			UTF_8_code_point = 0;
			UTF_8_bytes_seen = 0;
			UTF_8_bytes_needed = 0;

			// 11.
			return cp;
		}

	private:
		uint32_t UTF_8_code_point;
		uint32_t UTF_8_bytes_seen;
		uint32_t UTF_8_bytes_needed;
		uint32_t UTF_8_lower_boundary;
		uint32_t UTF_8_upper_boundary;
	};

	class UTF_8_encoder : public encoder
	{
	private:
		template <typename Array>
		Array run(uint32_t cp)
		{
			Array result{};
			//3.
			uint32_t count{ result.size() - 1 };
			uint32_t offset{};
			if (count == 1) offset = 0xC0;
			else if (count == 2) offset = 0xE0;
			else if (count == 3) offset = 0xF0;

			// 4.
			result[0] = (cp >> (6 * count)) + offset;

			// 5.
			uint32_t i = 1;
			while (0 < count)
			{
				uint32_t temp{ cp >> (6 * (count - 1)) };
				result[i] = 0x80 | (temp & 0x3F);
				--count;
				++i;
			}

			// 6.
			return result;
		}

	public:
		template <typename Stream, typename Token>
		result_value run(Stream& input, Token token)
		{
			// 1.
			if (!token.has_value()) return result_finished{};

			uint32_t cp{ token.value() };

			// 2.
			if (is_ascii_code_point(cp)) return static_cast<uint8_t>(cp);

			// 3.
			if (0x80 <= cp && cp <= 0x7FF) return run<std::array<uint8_t, 2>>(cp);
			else if (0x800 <= cp && cp <= 0xFFFF) return run<std::array<uint8_t, 3>>(cp);
			else if (0x10000 <= cp && cp <= 0x10FFFF) return run<std::array<uint8_t, 4>>(cp);

			assert(false);
			return result_error{};
		}
	};

	// Legacy single - byte encodings -----------------------------------------

	class single_byte_decoder : public decoder
	{

	};

	//IBM866
	// ISO_8859_2
	// ISO_8859_3
	// ISO_8859_4
	// ISO_8859_5
	// ISO_8859_6
	// ISO_8859_7
	// ISO_8859_8
	// ISO_8859_8_I
	// ISO_8859_10
	// ISO_8859_13
	// ISO_8859_14
	// ISO_8859_15
	// ISO_8859_16
	// KOI8_R
	// KOI8_U
	// macintosh
	// windows_874
	// windows_1250
	// windows_1251
	// windows_1252
	// windows_1253
	// windows_1254
	// windows_1255
	// windows_1256
	// windows_1257
	// windows_1258
	// x_mac_cyrillic

	// Legacy multi - byte Chinese(simplified) encodings ----------------------

	// GBK
	class GBK_decoder : public decoder {};
	class GBK_encoder : public encoder {};

	// gb18030
	class gb18030_decoder : public decoder {};
	class gb18030_encoder : public encoder {};

	// Legacy multi - byte Chinese(traditional) encodings ---------------------

	// Big5
	class Big5_decoder : public decoder {};
	class Big5_encoder : public encoder {};

	// Legacy multi - byte Japanese encodings ---------------------------------

	// EUC_JP
	class EUC_JP_decoder : public decoder {};
	class EUC_JP_encoder : public encoder {};

	// ISO_2022_JP
	class ISO_2022_JP_decoder : public decoder {};
	class ISO_2022_JP_encoder : public encoder {};

	// Shift_JIS
	class Shift_JIS_decoder : public decoder {};
	class Shift_JIS_encoder : public encoder {};

	// Legacy multi - byte Korean encodings -----------------------------------

	// EUC_KR
	class EUC_KR_decoder : public decoder {};
	class EUC_KR_encoder : public encoder {};

	// Legacy miscellaneous encodings -----------------------------------------

	// replacement
	class replacement_decoder : public decoder {};

	// UTF_16
	class shared_UTF_16_decoder : public decoder
	{
	};

	// UTF_16BE
	class UTF_16BE_decoder : public shared_UTF_16_decoder
	{
	};
	// UTF_16LE
	class UTF_16LE_decoder : public shared_UTF_16_decoder {};

	// x_user_defined
	class x_user_defined_decoder : public decoder {};
	class x_user_defined_encoder : public encoder {};
}
