#pragma once
// whtawg/encoding/coder.hpp

// https://encoding.spec.whatwg.org/
// https://triple-underscore.github.io/Encoding-ja.html

#include <algorithm>
#include <optional>

#include <wordring/whatwg/encoding/encoding_defs.hpp>
#include <wordring/whatwg/encoding/indexes.hpp>
#include <wordring/whatwg/infra/infra.hpp>

namespace wordring::whatwg::encoding
{

	// 5. Indexes

	template <std::vector<uint32_t> const& index>
	inline std::optional<uint32_t> get_index_code_point(uint16_t pointer)
	{
		uint32_t cp{ index.at(pointer) };

		if (cp == 4294967295u) return std::optional<uint32_t>{};
		return cp;
	}

	template <std::vector<uint32_t> const& index_0, std::vector<uint16_t> const& index_1>
	inline std::optional<uint16_t> get_index_pointer(uint32_t code_point)
	{
		std::vector<uint32_t>::const_iterator it = std::lower_bound(index_0.cbegin(), index_0.cend(), code_point);

		if (it != index_0.end() && *it == code_point) return *(index_1.cbegin() + (it - index_0.cbegin()));
		return std::optional<uint16_t>{};
	}

	inline std::optional<uint32_t> get_index_gb18030_ranges_code_point(uint32_t pointer)
	{
		// 1.
		if ((39419u < pointer && pointer < 189000u) || 1237575u < pointer) return std::optional<uint32_t>{};
		// 2.
		if (pointer == 7457u) return static_cast<uint32_t>(0xE7C7u);
		// 3.
		std::multimap<uint32_t, uint32_t>::const_iterator it = index_code_point_gb18030_ranges.lower_bound(pointer);
		assert(it != index_code_point_gb18030_ranges.cend());
		if (it->first != pointer) --it;
		uint32_t offset{ it->first };
		uint32_t code_point_offset{ it->second };
		// 4.
		return static_cast<uint32_t>(code_point_offset + pointer - offset);
	}

	inline std::optional<uint32_t> get_index_gb18030_ranges_pointer(uint32_t code_point)
	{
		// 1.
		if (code_point == 0xE7C7u) return 7457u;

		// 2.
		std::multimap<uint32_t, uint32_t>::const_iterator it = index_pointer_gb18030_ranges.lower_bound(code_point);
		if (it != index_pointer_gb18030_ranges.cend())
		{
			assert(false);
			return std::optional<uint32_t>{};
		}
		uint32_t offset{ it->first };
		uint32_t pointer_offset{ it->second };

		// 3.
		return static_cast<uint32_t>(pointer_offset + code_point - offset);
	}

	// 8.1.1. UTF-8 decoder
	class UTF_8_decoder : public decoder
	{
	public:
		UTF_8_decoder()
			: UTF_8_code_point{ 0 }
			, UTF_8_bytes_seen{ 0 }
			, UTF_8_bytes_needed{ 0 }
			, UTF_8_lower_boundary{ 0x80u }
			, UTF_8_upper_boundary{ 0xBFu }
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
				if (0 <= byte && byte <= 0x7Fu) return static_cast<uint32_t>(byte);
				if (0xC2u <= byte && byte <= 0xDFu)
				{
					UTF_8_bytes_needed = 1;
					UTF_8_code_point = byte & 0x1Fu;
				}
				else if (0xE0u <= byte && byte <= 0xEFu)
				{
					if (byte == 0xE0u) UTF_8_lower_boundary = 0xA0u;
					else if (byte == 0xEDu) UTF_8_upper_boundary = 0x9Fu;
					UTF_8_bytes_needed = 2;
					UTF_8_code_point = byte & 0xFu;
				}
				else if (0xF0u <= byte && byte <= 0xF4u)
				{
					if (byte == 0xF0u) UTF_8_lower_boundary = 0x90u;
					else if (byte == 0xF4u) UTF_8_upper_boundary = 0x8Fu;
					UTF_8_bytes_needed = 3;
					UTF_8_code_point = byte & 0x7u;
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

				UTF_8_lower_boundary = 0x80u;
				UTF_8_upper_boundary = 0xBFu;

				input.prepend(byte);

				return result_error{};
			}

			// 5.
			UTF_8_lower_boundary = 0x80u;
			UTF_8_upper_boundary = 0xBFu;

			// 6.
			UTF_8_code_point = (UTF_8_code_point << 6) | (byte & 0x3Fu);

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
			if (count == 1) offset = 0xC0u;
			else if (count == 2) offset = 0xE0u;
			else if (count == 3) offset = 0xF0u;

			// 4.
			result[0] = (cp >> (6 * count)) + offset;

			// 5.
			uint32_t i = 1;
			while (0 < count)
			{
				uint32_t temp{ cp >> (6 * (count - 1)) };
				result[i] = 0x80u | (temp & 0x3Fu);
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
			if (0x80u <= cp && cp <= 0x7FFu) return run<std::array<uint8_t, 2>>(cp);
			else if (0x800u <= cp && cp <= 0xFFFFu) return run<std::array<uint8_t, 3>>(cp);
			else if (0x10000u <= cp && cp <= 0x10FFFFu) return run<std::array<uint8_t, 4>>(cp);

			assert(false);
			return result_error{};
		}
	};

	// Legacy single - byte encodings -----------------------------------------

	// 9.1. single-byte decoder
	template <std::vector<uint32_t> const& index>
	class single_byte_decoder : public decoder
	{
	public:
		template <typename Stream, typename Token>
		result_value run(Stream& input, Token token)
		{
			// 1.
			if ((!token.has_value())) return result_finished{};

			uint8_t byte{ static_cast<uint8_t>(token.value()) };
			// 2.
			if (is_ascii_byte(byte)) return static_cast<uint32_t>(byte);

			// 3.
			std::optional<uint32_t> cp{ get_index_code_point<index>(byte - 0x80u) };

			// 4.
			if (!cp) return result_error{};

			// 5.
			return cp.value();
		}
	};

	// 9.2. single-byte encoder
	template <std::vector<uint32_t> const& index_0, std::vector<uint16_t> const& index_1>
	class single_byte_encoder : public encoder
	{
	public:
		template <typename Stream, typename Token>
		result_value run(Stream& input, Token token)
		{
			// 1.
			if ((!token.has_value())) return result_finished{};

			uint32_t cp{ token.value() };
			// 2.
			if (is_ascii_code_point(cp)) return static_cast<uint8_t>(cp);

			// 3.
			std::optional<uint16_t> byte{ get_index_pointer<index_0, index_1>(cp) };

			// 4.
			if(!byte) return result_error{ cp };

			// 5.
			return static_cast<uint8_t>(byte.value() + 0x80u);
		}
	};

	// IBM866
	using IBM866_decoder = single_byte_decoder<index_code_point_ibm866>;
	using IBM866_encoder = single_byte_encoder<index_pointer_ibm866_0, index_pointer_ibm866_1>;

	// ISO_8859_2
	using ISO_8859_2_decoder = single_byte_decoder<index_code_point_iso_8859_2>;
	using ISO_8859_2_encoder = single_byte_encoder<index_pointer_iso_8859_2_0, index_pointer_iso_8859_2_1>;

	// ISO_8859_3
	using ISO_8859_3_decoder = single_byte_decoder<index_code_point_iso_8859_3>;
	using ISO_8859_3_encoder = single_byte_encoder<index_pointer_iso_8859_3_0, index_pointer_iso_8859_3_1>;

	// ISO_8859_4
	using ISO_8859_4_decoder = single_byte_decoder<index_code_point_iso_8859_4>;
	using ISO_8859_4_encoder = single_byte_encoder<index_pointer_iso_8859_4_0, index_pointer_iso_8859_4_1>;

	// ISO_8859_5
	using ISO_8859_5_decoder = single_byte_decoder<index_code_point_iso_8859_5>;
	using ISO_8859_5_encoder = single_byte_encoder<index_pointer_iso_8859_5_0, index_pointer_iso_8859_5_1>;

	// ISO_8859_6
	using ISO_8859_6_decoder = single_byte_decoder<index_code_point_iso_8859_6>;
	using ISO_8859_6_encoder = single_byte_encoder<index_pointer_iso_8859_6_0, index_pointer_iso_8859_6_1>;

	// ISO_8859_7
	using ISO_8859_7_decoder = single_byte_decoder<index_code_point_iso_8859_7>;
	using ISO_8859_7_encoder = single_byte_encoder<index_pointer_iso_8859_7_0, index_pointer_iso_8859_7_1>;

	// ISO_8859_8
	using ISO_8859_8_decoder = single_byte_decoder<index_code_point_iso_8859_8>;
	using ISO_8859_8_encoder = single_byte_encoder<index_pointer_iso_8859_8_0, index_pointer_iso_8859_8_1>;

	// ISO_8859_8_I
	using ISO_8859_8_I_decoder = ISO_8859_8_decoder;
	using ISO_8859_8_I_encoder = ISO_8859_8_encoder;

	// ISO_8859_10
	using ISO_8859_10_decoder = single_byte_decoder<index_code_point_iso_8859_10>;
	using ISO_8859_10_encoder = single_byte_encoder<index_pointer_iso_8859_10_0, index_pointer_iso_8859_10_1>;

	// ISO_8859_13
	using ISO_8859_13_decoder = single_byte_decoder<index_code_point_iso_8859_13>;
	using ISO_8859_13_encoder = single_byte_encoder<index_pointer_iso_8859_13_0, index_pointer_iso_8859_13_1>;

	// ISO_8859_14
	using ISO_8859_14_decoder = single_byte_decoder<index_code_point_iso_8859_14>;
	using ISO_8859_14_encoder = single_byte_encoder<index_pointer_iso_8859_14_0, index_pointer_iso_8859_14_1>;

	// ISO_8859_15
	using ISO_8859_15_decoder = single_byte_decoder<index_code_point_iso_8859_15>;
	using ISO_8859_15_encoder = single_byte_encoder<index_pointer_iso_8859_15_0, index_pointer_iso_8859_15_1>;

	// ISO_8859_16
	using ISO_8859_16_decoder = single_byte_decoder<index_code_point_iso_8859_16>;
	using ISO_8859_16_encoder = single_byte_encoder<index_pointer_iso_8859_16_0, index_pointer_iso_8859_16_1>;

	// KOI8_R
	using KOI8_R_decoder = single_byte_decoder<index_code_point_koi8_r>;
	using KOI8_R_encoder = single_byte_encoder<index_pointer_koi8_r_0, index_pointer_koi8_r_1>;

	// KOI8_U
	using KOI8_U_decoder = single_byte_decoder<index_code_point_koi8_u>;
	using KOI8_U_encoder = single_byte_encoder<index_pointer_koi8_u_0, index_pointer_koi8_u_1>;

	// macintosh
	using macintosh_decoder = single_byte_decoder<index_code_point_macintosh>;
	using macintosh_encoder = single_byte_encoder<index_pointer_macintosh_0, index_pointer_macintosh_1>;

	// windows_874
	using windows_874_decoder = single_byte_decoder<index_code_point_windows_874>;
	using windows_874_encoder = single_byte_encoder<index_pointer_windows_874_0, index_pointer_windows_874_1>;

	// windows_1250
	using windows_1250_decoder = single_byte_decoder<index_code_point_windows_1250>;
	using windows_1250_encoder = single_byte_encoder<index_pointer_windows_1250_0, index_pointer_windows_1250_1>;

	// windows_1251
	using windows_1251_decoder = single_byte_decoder<index_code_point_windows_1251>;
	using windows_1251_encoder = single_byte_encoder<index_pointer_windows_1251_0, index_pointer_windows_1251_1>;

	// windows_1252
	using windows_1252_decoder = single_byte_decoder<index_code_point_windows_1252>;
	using windows_1252_encoder = single_byte_encoder<index_pointer_windows_1252_0, index_pointer_windows_1252_1>;

	// windows_1253
	using windows_1253_decoder = single_byte_decoder<index_code_point_windows_1253>;
	using windows_1253_encoder = single_byte_encoder<index_pointer_windows_1253_0, index_pointer_windows_1253_1>;

	// windows_1254
	using windows_1254_decoder = single_byte_decoder<index_code_point_windows_1254>;
	using windows_1254_encoder = single_byte_encoder<index_pointer_windows_1254_0, index_pointer_windows_1254_1>;

	// windows_1255
	using windows_1255_decoder = single_byte_decoder<index_code_point_windows_1255>;
	using windows_1255_encoder = single_byte_encoder<index_pointer_windows_1255_0, index_pointer_windows_1255_1>;

	// windows_1256
	using windows_1256_decoder = single_byte_decoder<index_code_point_windows_1256>;
	using windows_1256_encoder = single_byte_encoder<index_pointer_windows_1256_0, index_pointer_windows_1256_1>;

	// windows_1257
	using windows_1257_decoder = single_byte_decoder<index_code_point_windows_1257>;
	using windows_1257_encoder = single_byte_encoder<index_pointer_windows_1257_0, index_pointer_windows_1257_1>;

	// windows_1258
	using windows_1258_decoder = single_byte_decoder<index_code_point_windows_1258>;
	using windows_1258_encoder = single_byte_encoder<index_pointer_windows_1258_0, index_pointer_windows_1258_1>;

	// x_mac_cyrillic
	using x_mac_cyrillic_decoder = single_byte_decoder<index_code_point_x_mac_cyrillic>;
	using x_mac_cyrillic_encoder = single_byte_encoder<index_pointer_x_mac_cyrillic_0, index_pointer_x_mac_cyrillic_1>;

	// Legacy multi - byte Chinese(simplified) encodings ----------------------

	// gb18030
	class gb18030_decoder : public decoder
	{
	public:
		gb18030_decoder()
			: gb18030_first{ 0 }
			, gb18030_second{ 0 }
			, gb18030_third{ 0 }
		{
		}

		template <typename Stream, typename Token>
		result_value run(Stream& input, Token token)
		{
			// 1.
			if ((!token.has_value()))
			{
				if (gb18030_first == 0 && gb18030_second == 0 && gb18030_third == 0) return result_finished{};
			// 2.
				reset();
				return result_error{};
			}

			uint8_t byte = token.value();
			// 3.
			if (gb18030_third != 0)
			{
				if (!(0x30u <= byte && byte <= 0x39u))
				{
					input.prepend({ static_cast<uint8_t>(gb18030_second), static_cast<uint8_t>(gb18030_third), byte });
					reset();
					return result_error{};
				}

				std::optional<uint32_t> cp = get_index_gb18030_ranges_code_point(
					((gb18030_first - 0x81u) * 12600) + ((gb18030_second - 0x30u) * 1260) + ((gb18030_third - 0x81u) * 10) + byte - 0x30u);
				reset();
				if (!cp) return result_error{};
				return cp.value();
			}
			// 4.
			if (gb18030_second != 0)
			{
				if (0x30u <= byte && byte <= 0x39u)
				{
					gb18030_second = byte;
					return result_continue{};
				}

				input.prepend({ static_cast<uint8_t>(gb18030_second), byte });
				gb18030_first = 0;
				gb18030_second = 0;
				return result_error{};
			}
			// 5.
			if (gb18030_first != 0)
			{
				if (0x30u <= byte && byte <= 0x39u)
				{
					gb18030_second = byte;
					return result_continue{};
				}
				uint32_t lead{ gb18030_first };
				uint32_t offset = (byte < 0x7Fu) ? 0x40u : 0x41u;
				std::optional<uint32_t> pointer{};
				if ((0x40u <= byte && byte <= 0x7Eu) || (0x80u <= byte && byte <= 0xFEu)) pointer = (lead - 0x81) * 190 + (byte - offset);
				std::optional<uint32_t> cp{};
				if (pointer) cp = get_index_gb18030_ranges_code_point(pointer.value());
				if (cp) return cp.value();
				if (is_ascii_byte(byte)) input.prepend(byte);
				return result_error{};
			}
			// 6.
			if (is_ascii_byte(byte)) return static_cast<uint32_t>(byte);
			// 7.
			if (byte == 0x80u) return static_cast<uint32_t>(0x20ACu);
			// 8.
			if (0x81u <= byte && byte <= 0xFEu)
			{
				gb18030_first = byte;
				return result_continue{};
			}
			// 9.
			return result_error{};
		}

	private:
		void reset() noexcept
		{
			gb18030_first = 0;
			gb18030_second = 0;
			gb18030_third = 0;
		}

	private:
		uint32_t gb18030_first;
		uint32_t gb18030_second;
		uint32_t gb18030_third;
	};

	template <bool GBK_flag>
	class basic_gb18030_encoder : public encoder
	{
	public:
		template <typename Stream, typename Token>
		result_value run(Stream& input, Token token)
		{
			// 1.
			if (!token.has_value()) return result_finished{};

			uint32_t cp{ token.value() };
			// 2.
			if (is_ascii_code_point(cp)) return result_byte{ static_cast<uint8_t>(cp) };
			// 3.
			if (cp == 0xE5E5u) return result_error{ cp };
			// 4.
			if constexpr (GBK_flag) { if (cp == 0x20ACu) return result_byte{ 0x80u }; };
			// 5.
			std::optional<uint32_t> pointer{ get_index_pointer<index_pointer_gb18030_0, index_pointer_gb18030_1>(cp) };
			// 6.
			if (pointer.has_value())
			{
				uint32_t lead{ pointer.value() / 190 + 0x81u };
				uint32_t trail{ pointer.value() % 190 };
				uint32_t offset{ trail < 0x3Fu ? 0x40u : 0x41u };
				return result_bytes_2{ static_cast<uint8_t>(lead), static_cast<uint8_t>(trail + offset) };
			}
			// 7.
			if constexpr (GBK_flag) return result_error{ cp };
			// 8.
			pointer = get_index_gb18030_ranges_pointer(cp);
			// 9.
			uint8_t byte1{ static_cast<uint8_t>(pointer.value() / (10 * 126 * 10)) };
			// 10.
			pointer = pointer.value() % (10 * 126 * 10);
			// 11.
			uint8_t byte2{ static_cast<uint8_t>(pointer.value() / (10 * 126)) };
			// 12.
			pointer = pointer.value() % (10 * 126);
			// 13.
			uint8_t byte3{ static_cast<uint8_t>(pointer.value() / 10) };
			// 14.
			uint8_t byte4{ static_cast<uint8_t>(pointer.value() % 10) };
			// 15.
			return result_bytes_4{ byte1 + 0x81u, byte2 + 0x30u, byte3 + 0x81u, byte4 + 0x30u };
		}
	};

	using gb18030_encoder = basic_gb18030_encoder<false>;

	// GBK
	using GBK_decoder = gb18030_decoder;
	using GBK_encoder = basic_gb18030_encoder<true>;

	// Legacy multi - byte Chinese(traditional) encodings ---------------------

	// Big5
	class Big5_decoder : public decoder
	{
	public:
		template <typename Stream, typename Token>
		result_value run(Stream& input, Token token)
		{
			return result_error{};
		}
	};
	class Big5_encoder : public encoder
	{
	public:
		template <typename Stream, typename Token>
		result_value run(Stream& input, Token token)
		{
			return result_error{};
		}
	};

	// Legacy multi - byte Japanese encodings ---------------------------------

	// EUC_JP
	class EUC_JP_decoder : public decoder
	{
	public:
		template <typename Stream, typename Token>
		result_value run(Stream& input, Token token)
		{
			return result_error{};
		}
	};
	class EUC_JP_encoder : public encoder
	{
	public:
		template <typename Stream, typename Token>
		result_value run(Stream& input, Token token)
		{
			return result_error{};
		}
	};

	// ISO_2022_JP
	class ISO_2022_JP_decoder : public decoder
	{
	public:
		template <typename Stream, typename Token>
		result_value run(Stream& input, Token token)
		{
			return result_error{};
		}
	};
	class ISO_2022_JP_encoder : public encoder
	{
	public:
		template <typename Stream, typename Token>
		result_value run(Stream& input, Token token)
		{
			return result_error{};
		}
	};

	// Shift_JIS
	class Shift_JIS_decoder : public decoder
	{
	public:
		template <typename Stream, typename Token>
		result_value run(Stream& input, Token token)
		{
			return result_error{};
		}
	};
	class Shift_JIS_encoder : public encoder
	{
	public:
		template <typename Stream, typename Token>
		result_value run(Stream& input, Token token)
		{
			return result_error{};
		}
	};

	// Legacy multi - byte Korean encodings -----------------------------------

	// EUC_KR
	class EUC_KR_decoder : public decoder
	{
	public:
		template <typename Stream, typename Token>
		result_value run(Stream& input, Token token)
		{
			return result_error{};
		}
	};
	class EUC_KR_encoder : public encoder
	{
	public:
		template <typename Stream, typename Token>
		result_value run(Stream& input, Token token)
		{
			return result_error{};
		}
	};

	// Legacy miscellaneous encodings -----------------------------------------

	// replacement
	class replacement_decoder : public decoder
	{
	public:
		template <typename Stream, typename Token>
		result_value run(Stream& input, Token token)
		{
			return result_error{};
		}
	};

	// UTF_16
	class shared_UTF_16_decoder : public decoder
	{
	public:
		template <typename Stream, typename Token>
		result_value run(Stream& input, Token token)
		{
			return result_error{};
		}
	};

	// UTF_16BE
	class UTF_16BE_decoder : public shared_UTF_16_decoder
	{
	public:
		template <typename Stream, typename Token>
		result_value run(Stream& input, Token token)
		{
			return result_error{};
		}
	};
	// UTF_16LE
	class UTF_16LE_decoder : public shared_UTF_16_decoder
	{
	public:
		template <typename Stream, typename Token>
		result_value run(Stream& input, Token token)
		{
			return result_error{};
		}
	};

	// x_user_defined
	class x_user_defined_decoder : public decoder
	{
	public:
		template <typename Stream, typename Token>
		result_value run(Stream& input, Token token)
		{
			return result_error{};
		}
	};
	class x_user_defined_encoder : public encoder
	{
	public:
		template <typename Stream, typename Token>
		result_value run(Stream& input, Token token)
		{
			return result_error{};
		}
	};
}
