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

	template <auto const& index>
	inline std::optional<uint32_t> get_index_code_point(uint16_t pointer)
	{
		if (index.size() <= pointer) return std::optional<uint32_t>{};
		uint32_t cp{ index[pointer] };
		if (cp == 4294967295u) return std::optional<uint32_t>{};
		return cp;
	}

	template <auto const& index_0, auto const& index_1>
	inline std::optional<uint16_t> get_index_pointer(uint32_t code_point)
	{
		auto it = std::lower_bound(index_0.cbegin(), index_0.cend(), code_point);

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
		// 表引きできなかった時のための追加コード
		if (it == index_code_point_gb18030_ranges.cend())
		{
			assert(false);
			return std::optional<uint32_t>{};
		}

		if (it->first != pointer) --it;
		uint32_t offset{ it->first };
		uint32_t code_point_offset{ it->second };
		// 4.
		return static_cast<uint32_t>(code_point_offset + (pointer - offset));
	}

	/*!
git pull origin master

	ranges_pointerに限り32ビットの戻り値が必要。
	*/
	inline std::optional<uint32_t> get_index_gb18030_ranges_pointer(uint32_t code_point)
	{
		// 1.
		if (code_point == 0xE7C7u) return 7457u;

		// 2.
		std::multimap<uint32_t, uint32_t>::const_iterator it = index_pointer_gb18030_ranges.lower_bound(code_point);
		// 表引きできなかった時のための追加コード
		if (it == index_pointer_gb18030_ranges.cend())
		{
			assert(false);
			return std::optional<uint32_t>{};
		}

		if (it->first != code_point) --it;
		uint32_t offset{ it->first };
		uint32_t pointer_offset{ it->second };

		// 3.
		return static_cast<uint32_t>(pointer_offset + (code_point - offset));
	}

	inline std::optional<uint16_t> get_index_shift_jis_pointer(uint32_t code_point)
	{
		return get_index_pointer<index_pointer_Shift_JIS_0, index_pointer_Shift_JIS_1>(code_point);
	}

	inline std::optional<uint16_t> get_index_big5_pointer(uint32_t code_point)
	{
		return get_index_pointer<index_pointer_big5_0, index_pointer_big5_1>(code_point);
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
	template <auto const& index>
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
	template <auto const& index_0, auto const& index_1>
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
					input.prepend_of({ static_cast<uint8_t>(gb18030_second), static_cast<uint8_t>(gb18030_third), byte });
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
				if (0x81u <= byte && byte <= 0xFEu)
				{
					gb18030_third = byte;
					return result_continue{};
				}

				input.prepend_of({ static_cast<uint8_t>(gb18030_second), byte });
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
				std::optional<uint32_t> pointer{};
				gb18030_first = 0;
				uint32_t offset = (byte < 0x7Fu) ? 0x40u : 0x41u;
				if ((0x40u <= byte && byte <= 0x7Eu) || (0x80u <= byte && byte <= 0xFEu)) pointer = (lead - 0x81) * 190 + (byte - offset);
				std::optional<uint32_t> cp{};
				if (pointer) cp = get_index_code_point<index_code_point_gb18030>(pointer.value());
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
			// 表引きできなかった時のための追加コード
			if (!pointer.has_value())
			{
				assert(false);
				return result_error{};
			}

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
			return result_bytes_4{
				static_cast<uint8_t>(byte1 + 0x81u),
				static_cast<uint8_t>(byte2 + 0x30u),
				static_cast<uint8_t>(byte3 + 0x81u),
				static_cast<uint8_t>(byte4 + 0x30u) };
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
		Big5_decoder() : big5_lead{ 0 } {}

		template <typename Stream, typename Token>
		result_value run(Stream& input, Token token)
		{
			if (!token.has_value())
			{
			// 1.
				if (big5_lead != 0)
				{
					big5_lead = 0;
					return result_error{};
				}
			// 2.
				return result_finished{};
			}

			uint8_t byte = token.value();

			// 3.
			if (big5_lead != 0)
			{
				uint32_t lead{ big5_lead };
				std::optional<uint16_t> pointer{};
				big5_lead = 0;
				// 3.1.
				uint32_t offset{ byte < 0x7Fu ? 0x40u : 0x62u };
				// 3.2.
				if (0x40u <= byte && byte <= 0x7Eu)
				{
					pointer = (lead - 0x81u) * 157 + (byte - offset);
				// 3.3.
					switch (pointer.value())
					{
					case 1133u: return result_code_points_2{ 0xCAu, 0x304u };
					case 1135u: return result_code_points_2{ 0xCAu, 0x30Cu };
					case 1164u: return result_code_points_2{ 0xEAu, 0x304u };
					case 1166u: return result_code_points_2{ 0xEAu, 0x30Cu };
					}
				}
				// 3.4.
				std::optional<uint32_t> cp{};
				if (pointer.has_value()) cp = get_index_code_point<index_code_point_big5>(pointer.value());
				// 3.5.
				if (cp.has_value()) return result_code_point{ cp.value() };
				// 3.6.
				if (is_ascii_byte(byte)) input.prepend(byte);
				// 3.7.
				return result_error{};
			}
			// 4.
			if (is_ascii_byte(byte)) return result_code_point{ byte };
			// 5.
			if (0x81u <= byte && byte <= 0xFEu)
			{
				big5_lead = byte;
				return result_continue{};
			}
			// 6.
			return result_error{};
		}

	private:
		uint32_t big5_lead;
	};

	class Big5_encoder : public encoder
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
			std::optional<uint16_t> pointer{ get_index_big5_pointer(cp) };
			// 4.
			if (!pointer.has_value()) return result_error{ cp };
			// 5.
			uint8_t lead{ static_cast<uint8_t>(pointer.value() / 157 + 0x81u) };
			// 6.
			uint8_t trail{ static_cast<uint8_t>(pointer.value() % 157) };
			// 7.
			uint8_t offset{ static_cast<uint8_t>(trail < 0x3Fu ? 0x40u : 0x62u) };
			// 8.
			return result_bytes_2{ lead, static_cast<uint8_t>(trail + offset) };
		}
	};

	// Legacy multi-byte Japanese encodings -----------------------------------

	// EUC_JP
	class EUC_JP_decoder : public decoder
	{
	public:
		EUC_JP_decoder() : EUC_JP_jis0212_flag{ false }, EUC_JP_lead{ 0 }{}

		template <typename Stream, typename Token>
		result_value run(Stream& input, Token token)
		{
			if (!token.has_value())
			{
			// 1.
				if (EUC_JP_lead != 0)
				{
					EUC_JP_lead = 0;
					return result_error{};
				}
			// 2.
				return result_finished{};
			}

			uint8_t byte{ static_cast<uint8_t>(token.value()) };
			
			// 3.
			if (EUC_JP_lead == 0x8Eu && (0xA1u <= byte && byte <= 0xDFu))
			{
				EUC_JP_lead = 0;
				return result_code_point{ 0xFF61u - 0xA1u + byte };
			}
			// 4.
			if (EUC_JP_lead == 0x8Fu && (0xA1u <= byte && byte <= 0xFEu))
			{
				EUC_JP_jis0212_flag = true;
				EUC_JP_lead = byte;
				return result_continue{};
			}
			// 5.
			if (EUC_JP_lead != 0)
			{
				uint8_t lead{ EUC_JP_lead };
				EUC_JP_lead = 0;
				// 5.1.
				std::optional<uint32_t> cp{};
				// 5.2.
				if ((0xA1u <= lead && lead <= 0xFEu) && (0xA1u <= byte && byte <= 0xFEu))
				{
					uint16_t pointer{ static_cast<uint16_t>((lead - 0xA1u) * 94 + byte - 0xA1u) };
					if (EUC_JP_jis0212_flag) cp = get_index_code_point<index_code_point_jis0212>(pointer);
					else cp = get_index_code_point<index_code_point_jis0208>(pointer);
				}
				// 5.3.
				EUC_JP_jis0212_flag = false;
				// 5.4.
				if (cp.has_value()) return result_code_point{ cp.value() };
				// 5.5.
				if (is_ascii_byte(byte)) input.prepend(byte);
				// 5.6.
				return result_error{};
			}
			// 6.
			if (is_ascii_byte(byte)) return result_code_point{ byte };
			// 7.
			if (byte == 0x8Eu || byte == 0x8Fu || (0xA1u <= byte && byte <= 0xFEu))
			{
				EUC_JP_lead = byte;
				return result_continue{};
			}
			// 8.
			return result_error{};
		}

	private:
		bool EUC_JP_jis0212_flag;
		uint8_t EUC_JP_lead;
	};
	class EUC_JP_encoder : public encoder
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
			if (cp == 0xA5u) return result_byte{ 0x5Cu };
			// 4.
			if (cp == 0x203Eu) return result_byte{ 0x7Eu };
			// 5.
			if (0xFF61u <= cp && cp <= 0xFF9Fu) return result_bytes_2{ 0x8Eu, static_cast<uint8_t>(cp - 0xFF61u + 0xA1u) };
			// 6.
			if (cp == 0x2212u) cp = 0xFF0Du;
			// 7.
			std::optional<uint16_t> pointer{ get_index_pointer<index_pointer_jis0208_0, index_pointer_jis0208_1>(cp) };
			// 8.
			if (!pointer.has_value()) return result_error{ cp };
			// 9.
			uint8_t lead{ static_cast<uint8_t>(pointer.value() / 94 + 0xA1u) };
			// 10.
			uint8_t trail{ static_cast<uint8_t>(pointer.value() % 94 + 0xA1u) };
			// 11.
			return result_bytes_2{ lead, trail };
		}
	};

	// ISO_2022_JP
	class ISO_2022_JP_decoder : public decoder
	{
	private:
		enum state : uint8_t
		{
			ASCII, Roman, katakana, Lead_byte, Trail_byte, Escape_start, Escape
		};

	public:
		ISO_2022_JP_decoder()
			: ISO_2022_JP_decoder_state{ state::ASCII }
			, ISO_2022_JP_decoder_output_state{ state::ASCII }
			, ISO_2022_JP_lead{ 0 }
			, ISO_2022_JP_output_flag{ false }
		{
		}

		template <typename Stream, typename Token>
		result_value run(Stream& input, Token token)
		{
			uint8_t byte{};

			switch (ISO_2022_JP_decoder_state)
			{
			case ASCII:
				if (!token.has_value()) return result_finished{};
				byte = static_cast<uint8_t>(token.value());
				if (byte == 0x1Bu)
				{
					ISO_2022_JP_decoder_state = state::Escape_start;
					return result_continue{};
				}
				else if (byte <= 0x7Fu && byte != 0x0Eu && byte != 0x0Fu && byte != 0x1Bu)
				{
					ISO_2022_JP_output_flag = false;
					return result_code_point{ byte };
				}
				ISO_2022_JP_output_flag = false;
				return result_error{};
				break;

			case state::Roman:
				if (!token.has_value()) return result_finished{};
				byte = static_cast<uint8_t>(token.value());
				if (byte == 0x1Bu)
				{
					ISO_2022_JP_decoder_state = state::Escape_start;
					return result_continue{};
				}
				else if (byte == 0x5Cu)
				{
					ISO_2022_JP_output_flag = false;
					return result_code_point{ 0xA5u };
				}
				else if (byte == 0x7Eu)
				{
					ISO_2022_JP_output_flag = false;
					return result_code_point{ 0x203Eu };
				}
				else if (byte <= 0x7Fu && byte != 0x0Eu && byte != 0x0Fu && byte != 0x1Bu && byte != 0x5Cu && byte != 0x7Eu)
				{
					ISO_2022_JP_output_flag = false;
					return result_code_point{ byte };
				}
				else
				{
					ISO_2022_JP_output_flag = false;
					return result_error{};
				}
				break;

			case state::katakana:
				if (!token.has_value()) return result_finished{};
				byte = static_cast<uint8_t>(token.value());
				if (byte == 0x1Bu)
				{
					ISO_2022_JP_decoder_state = state::Escape_start;
					return result_continue{};
				}
				else if (0x21u <= byte && byte <= 0x5Fu)
				{
					ISO_2022_JP_output_flag = false;
					return result_code_point{ 0xFF61u - 0x21u + byte };
				}
				else
				{
					ISO_2022_JP_output_flag = false;
					return result_error{};
				}
				break;

			case state::Lead_byte:
				if (!token.has_value()) return result_finished{};
				byte = static_cast<uint8_t>(token.value());
				if (byte == 0x1Bu)
				{
					ISO_2022_JP_decoder_state = state::Escape_start;
					return result_continue{};
				}
				else if (0x21u <= byte && byte <= 0x7Eu)
				{
					ISO_2022_JP_output_flag = false;
					ISO_2022_JP_lead = byte;
					ISO_2022_JP_decoder_state = state::Trail_byte;
					return result_continue{};
				}
				else
				{
					ISO_2022_JP_output_flag = false;
					return result_error{};
				}
				break;

			case state::Trail_byte:
				if (!token.has_value())
				{
					ISO_2022_JP_decoder_state = state::Lead_byte;
					// input.prepend(byte); 仕様でbyteはEOSのはず。
					return result_error{};
				}
				byte = static_cast<uint8_t>(token.value());
				if (byte == 0x1Bu)
				{
					ISO_2022_JP_decoder_state = state::Escape_start;
					return result_error{};
				}
				else if (0x21u <= byte && byte <= 0x7Eu)
				{
					ISO_2022_JP_decoder_state = state::Lead_byte;
					std::optional<uint16_t> pointer{ static_cast<uint16_t>((ISO_2022_JP_lead - 0x21u) * 94 + byte - 0x21u) };
					std::optional<uint32_t> cp{ get_index_code_point<index_code_point_jis0208>(pointer.value()) };
					if (!cp.has_value()) return result_error{};
					return result_code_point{ cp.value() };
				}
				else
				{
					ISO_2022_JP_decoder_state = state::Lead_byte;
					return result_error{};
				}
				break;

			case state::Escape_start:
				if (token.has_value())
				{
					byte = static_cast<uint8_t>(token.value());
					if (byte == 0x24u || byte == 0x28u)
					{
						ISO_2022_JP_lead = byte;
						ISO_2022_JP_decoder_state = state::Escape;
						return result_continue{};
					}
					input.prepend(byte);
				}
				ISO_2022_JP_output_flag = false;
				ISO_2022_JP_decoder_state = ISO_2022_JP_decoder_output_state;
				return result_error{};
				break;

			case state::Escape:
			{
				// state::Escape になるときは必ずbyteがprependされている。
				byte = static_cast<uint8_t>(token.value());
				uint8_t lead{ ISO_2022_JP_lead };
				ISO_2022_JP_lead = 0;
				std::optional<state> st{};
				if (lead == 0x28u && byte == 0x42u) st = state::ASCII;
				else if (lead == 0x28u && byte == 0x4Au) st = state::Roman;
				else if (lead == 0x28u && byte == 0x49u) st = state::katakana;
				else if (lead == 0x24u && (byte == 0x40u || byte == 0x42u)) st = state::Lead_byte;
				if (st.has_value())
				{
					ISO_2022_JP_decoder_state = st.value();
					ISO_2022_JP_decoder_output_state = st.value();
					bool output_flag = ISO_2022_JP_output_flag;
					ISO_2022_JP_output_flag = true;
					if (output_flag == false) return result_continue{};
					else return result_error{};
				}
				input.prepend_of({ lead, byte });
				ISO_2022_JP_output_flag = false;
				ISO_2022_JP_decoder_state = ISO_2022_JP_decoder_output_state;
				return result_error{};
			}
				break;
			}

			assert(false);
			return result_error{};
		}

	private:
		state   ISO_2022_JP_decoder_state;
		state   ISO_2022_JP_decoder_output_state;
		uint8_t ISO_2022_JP_lead;
		bool    ISO_2022_JP_output_flag;
	};

	class ISO_2022_JP_encoder : public encoder
	{
	private:
		enum state : uint8_t
		{
			ASCII, Roman, jis0208
		};

	public:
		ISO_2022_JP_encoder()
			: ISO_2022_JP_encoder_state{ state::ASCII }
		{
		}

		template <typename Stream, typename Token>
		result_value run(Stream& input, Token token)
		{
			// 1.
			if (!token.has_value() && ISO_2022_JP_encoder_state != state::ASCII)
			{
				ISO_2022_JP_encoder_state = state::ASCII;
				return result_bytes_3{ static_cast<uint8_t>(0x1Bu), static_cast<uint8_t>(0x28u), static_cast<uint8_t>(0x42u) };
			}
			// 2.
			if (!token.has_value() && ISO_2022_JP_encoder_state == state::ASCII) return result_finished{};

			uint32_t cp{ token.value() };

			// 3.
			if ((ISO_2022_JP_encoder_state == state::ASCII || ISO_2022_JP_encoder_state == state::Roman)
				&& (cp == 0xEu || cp == 0xFu || cp == 0x1Bu)) return result_error{ static_cast<uint32_t>(0xFFFDu) };
			// 4.
			if (ISO_2022_JP_encoder_state == state::ASCII && is_ascii_code_point(cp)) return result_byte{ static_cast<uint8_t>(cp) };
			// 5.
			if(ISO_2022_JP_encoder_state == state::Roman
				&& ((is_ascii_code_point(cp) && cp != 0x5Cu && cp != 0x7Eu) || (cp == 0xA5u || cp == 0x203Eu)))
			{
				if (is_ascii_code_point(cp)) return result_byte{ static_cast<uint8_t>(cp) };
				if (cp == 0xA5u) return result_byte{ static_cast<uint8_t>(0x5Cu) };
				if (cp == 0x203Eu) return result_byte{ static_cast<uint8_t>(0x7Eu) };
			}
			// 6.
			if (is_ascii_code_point(cp) && ISO_2022_JP_encoder_state != state::ASCII)
			{
				input.prepend(cp);
				ISO_2022_JP_encoder_state = state::ASCII;
				return result_bytes_3{ static_cast<uint8_t>(0x1Bu), static_cast<uint8_t>(0x28u), static_cast<uint8_t>(0x42u) };
			}
			// 7.
			if ((cp == 0xA5u || cp == 0x203Eu) && ISO_2022_JP_encoder_state != state::Roman)
			{
				input.prepend(cp);
				ISO_2022_JP_encoder_state = state::Roman;
				return result_bytes_3{ static_cast<uint8_t>(0x1Bu), static_cast<uint8_t>(0x28u), static_cast<uint8_t>(0x42u) };
			}
			// 8.
			if (cp == 0x2212u) cp = 0xFF0Du;
			// 9.
			if (0xFF61u <= cp && cp <= 0xFF9Fu) cp = get_index_code_point<index_code_point_iso_2022_jp_katakana>(cp - 0xFF61u).value();
			// 10.
			std::optional<uint16_t> pointer{ get_index_pointer<index_pointer_jis0208_0, index_pointer_jis0208_1>(cp) };
			// 11.
			if (!pointer.has_value()) return result_error{ cp };
			// 12.
			if (ISO_2022_JP_encoder_state != state::jis0208)
			{
				input.prepend(cp);
				ISO_2022_JP_encoder_state = state::jis0208;
				return result_bytes_3{ static_cast<uint8_t>(0x1Bu), static_cast<uint8_t>(0x24u), static_cast<uint8_t>(0x42u) };
			}
			// 13.
			uint8_t lead{ static_cast<uint8_t>(pointer.value() / 94 + 0x21u) };
			// 14.
			uint8_t trail{ static_cast<uint8_t>(pointer.value() % 94 + 0x21u) };
			// 15.
			return result_bytes_2{ lead, trail };
		}

	private:
		state ISO_2022_JP_encoder_state;
	};

	// Shift_JIS
	class Shift_JIS_decoder : public decoder
	{
	public:
		Shift_JIS_decoder()
			: Shift_JIS_lead(0)
		{
		}

		template <typename Stream, typename Token>
		result_value run(Stream& input, Token token)
		{
			// 1.
			if (!token.has_value() && Shift_JIS_lead != 0)
			{
				Shift_JIS_lead = 0;
				return result_error{};
			}
			// 2.
			if (!token.has_value() && Shift_JIS_lead == 0) return result_finished{};

			uint8_t byte{ static_cast<uint8_t>(token.value()) };

			// 3.
			if (Shift_JIS_lead != 0)
			{
				uint8_t lead{ Shift_JIS_lead };
				std::optional<uint16_t> pointer{};
				Shift_JIS_lead = 0;
				// 3.1.
				uint8_t offset{ byte < 0x7Fu ? 0x40u : 0x41u };
				// 3.2.
				uint8_t lead_offset{ lead < 0xA0u ? 0x81u : 0xC1u };
				// 3.3.
				if ((0x40u <= byte && byte <= 0x7Eu) || (0x80u <= byte && byte <= 0xFCu))
					pointer = (lead - lead_offset) * 188 + byte - offset;
				// 3.4.
				if (pointer.has_value() && (8836 <= pointer.value() && pointer.value() <= 10715))
					return result_code_point{ 0xE000u - 8836 + pointer.value() };
				// 3.5.
				std::optional<uint32_t> cp{};
				if (pointer.has_value()) cp = get_index_code_point<index_code_point_jis0208>(pointer.value());
				// 3.6.
				if (cp.has_value()) return result_code_point{ cp.value() };
				// 3.7.
				if (is_ascii_byte(byte)) input.prepend(byte);
				// 3.8.
				return result_error{};
			}
			// 4.
			if (is_ascii_byte(byte) || byte == 0x80u) return result_code_point{ byte };
			// 5.
			if (0xA1u <= byte && byte <= 0xDFu) return result_code_point{ 0xFF61u - 0xA1u + byte };
			// 6.
			if ((0x81u <= byte && byte <= 0x9Fu) || (0xE0u <= byte && byte <= 0xFCu))
			{
				Shift_JIS_lead = byte;
				return result_continue{};
			}
			// 7.
			return result_error{};
		}

	private:
		uint8_t Shift_JIS_lead;
	};

	class Shift_JIS_encoder : public encoder
	{
	public:
		template <typename Stream, typename Token>
		result_value run(Stream& input, Token token)
		{
			// 1.
			if (!token.has_value()) return result_finished{};

			uint32_t cp{ token.value() };

			// 2.
			if (is_ascii_code_point(cp) || cp == 0x80u) return result_byte{ static_cast<uint8_t>(cp) };
			// 3.
			if (cp == 0xA5u) return result_byte{ 0x5Cu };
			// 4.
			if (cp == 0x203Eu) return result_byte{ 0x7Eu };
			// 5.
			if (0xFF61u <= cp && cp <= 0xFF9Fu) return result_byte{ static_cast<uint8_t>(cp - 0xFF61u + 0xA1u) };
			// 6.
			if (cp == 0x2212u) cp = 0xFF0Du;
			// 7.
			std::optional<uint16_t> pointer = get_index_shift_jis_pointer(cp);
			// 8.
			if (!pointer.has_value()) return result_error{ cp };
			// 9.
			uint8_t lead{ static_cast<uint8_t>(pointer.value() / 188) };
			// 10.
			uint8_t lead_offset{ lead < 0x1Fu ? 0x81u : 0xC1u };
			// 11.
			uint8_t trail{ static_cast<uint8_t>(pointer.value() % 188) };
			// 12.
			uint8_t offset{ trail < 0x3Fu ? 0x40u : 0x41u };
			// 13.
			return result_bytes_2{ static_cast<uint8_t>(lead + lead_offset), static_cast<uint8_t>(trail + offset) };
		}
	};

	// Legacy multi - byte Korean encodings -----------------------------------

	// EUC_KR
	class EUC_KR_decoder : public decoder
	{
	public:
		EUC_KR_decoder()
			: EUC_KR_lead{ 0 }
		{
		}

		template <typename Stream, typename Token>
		result_value run(Stream& input, Token token)
		{
			// 1.
			if (!token.has_value() && EUC_KR_lead != 0)
			{
				EUC_KR_lead = 0;
				return result_error{};
			}
			// 2.
			if (!token.has_value() && EUC_KR_lead == 0) return result_finished{};

			uint8_t byte{ static_cast<uint8_t>(token.value()) };

			// 3.
			if (EUC_KR_lead != 0)
			{
				uint8_t lead{ EUC_KR_lead };
				std::optional<uint16_t> pointer{};
				EUC_KR_lead = 0;
				// 3.1.
				if (0x41u <= byte && byte <= 0xFEu) pointer = (lead - 0x81u) * 190 + (byte - 0x41u);
				// 3.2.
				std::optional<uint32_t> cp{};
				if (pointer.has_value()) cp = get_index_code_point<index_code_point_euc_kr>(pointer.value());
				// 3.3.
				if (cp.has_value()) return result_code_point{ cp.value() };
				// 3.4.
				if (is_ascii_byte(byte)) input.prepend(byte);
				// 3.5.
				return result_error{};
			}
			// 4.
			if (is_ascii_byte(byte)) return result_code_point{ byte };
			// 5.
			if (0x81u <= byte && byte <= 0xFEu)
			{
				EUC_KR_lead = byte;
				return result_continue{};
			}
			// 6.
			return result_error{};
		}

	private:
		uint8_t EUC_KR_lead;
	};

	class EUC_KR_encoder : public encoder
	{
	public:
		template <typename Stream, typename Token>
		result_value run(Stream& input, Token token)
		{
			// 1.
			if (!token.has_value()) return result_finished{};

			uint32_t cp{ token.has_value() };

			// 2.
			if (is_ascii_code_point(cp)) return result_byte{ static_cast<uint8_t>(cp) };
			// 3.
			std::optional<uint16_t> pointer = get_index_pointer<index_pointer_euc_kr_0, index_pointer_euc_kr_1>(cp);
			// 4.
			if (!pointer.has_value()) return result_error{ cp };
			// 5.
			uint8_t lead{ static_cast<uint8_t>(pointer.value() / 190 + 0x81u) };
			// 6.
			uint8_t trail{ static_cast<uint8_t>(pointer.value() % 190 + 0x41u) };
			// 7.
			return result_bytes_2{ lead, trail };
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
