#pragma once
// whatwg/encoding/encoding.hpp

// https://encoding.spec.whatwg.org/
// https://triple-underscore.github.io/Encoding-ja.html

#include <cassert>
#include <charconv>
#include <cstdint>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <variant>

#include <wordring/algorithm.hpp>
#include <wordring/compatibility.hpp>
#include <wordring/whatwg/encoding/coder.hpp>
#include <wordring/whatwg/encoding/encoding_defs.hpp>
//#include <wordring/whatwg/encoding/indexes.hpp>
#include <wordring/whatwg/encoding/stream.hpp>
#include <wordring/whatwg/infra/infra.hpp>

namespace wordring::whatwg::encoding
{
	// 4.1. Encoders and decoders ---------------------------------------------

	template <typename Token, typename Coder, typename InputStream, typename OutputIterator>
	result_value process_token(
		Token token, Coder& coder, InputStream& input, OutputIterator output, error_mode mode)
	{
		result_value result = coder.run(input, token);
		switch (result.index())
		{
		case 0: // result_finished
		case 1: // result_continue
			return result;
		case 2: // result_code_point
		{
			result_code_point* cp = std::get_if<2>(&result);
			assert(cp != nullptr);
			*output++ = *cp;
			return result_continue{};
		}
		case 3: // result_code_points_2
		{
			std::array<uint32_t, 2>* array_2 = std::get_if<3>(&result);
			assert(array_2 != nullptr);
			output = std::copy(array_2->begin(), array_2->end(), output);
			return result_continue{};
		}
		case 4: // result_byte
		{
			uint8_t* byte = std::get_if<4>(&result);
			assert(byte != nullptr);
			*output++ = *byte;
			return result_continue{};
		}
		case 5: // result_bytes_2
		{
			std::array<uint8_t, 2>* array_2 = std::get_if<5>(&result);
			assert(array_2 != nullptr);
			output = std::copy(array_2->begin(), array_2->end(), output);
			return result_continue{};
		}
		case 6: // result_bytes_3
		{
			std::array<uint8_t, 3>* array_3 = std::get_if<6>(&result);
			assert(array_3 != nullptr);
			output = std::copy(array_3->begin(), array_3->end(), output);
			return result_continue{};
		}
		case 7: // result_bytes_4
		{
			std::array<uint8_t, 4>* array_4 = std::get_if<7>(&result);
			assert(array_4 != nullptr);
			output = std::copy(array_4->begin(), array_4->end(), output);
			return result_continue{};
		}
		case 8: // result_error
		{
			result_error* error = std::get_if<8>(&result);
			assert(error != nullptr);

			if (mode == error_mode::Replacement)
			{
				if constexpr (std::is_base_of_v<decoder, Coder>)* output++ = 0xFFFDu;
			}
			else if (mode == error_mode::Html)
			{
				if constexpr (std::is_base_of_v<encoder, Coder>)
				{
					std::array<char, 13> array{ 0x26u, 0x23u };
					auto [p, ec] = std::to_chars(array.data() + 2, array.data() + array.size() + 2, error->code_point);
					*p++ = 0x3Bu;
					output = std::copy(array.data(), p, output);
				}
			}
			else if (mode == error_mode::Fatal) return result;
		}
		}
		return result_continue{};
	}

	template <typename Token, typename Coder, typename InputStream, typename OutputIterator>
	result_value process_token(
		Token token, Coder & coder, InputStream & input, OutputIterator output)
	{
		error_mode mode{ error_mode::Replacement };
		if constexpr (std::is_base_of_v<encoder, Coder>) mode = error_mode::Fatal;
		return process_token(token, coder, input, output, mode);
	}

	template <typename Coder, typename InputStream, typename OutputIterator>
	result_value run(Coder & coder, InputStream & input, OutputIterator output, error_mode mode)
	{
		while (true)
		{
			result_value result = process_token(input.read(), coder, input, output, mode);
			if (result.index() != 1) return result;
		}
		assert(false);
		return result_finished{};
	}

	template <typename Coder, typename InputStream, typename OutputIterator>
	result_value run(Coder & coder, InputStream & input, OutputIterator output)
	{
		error_mode mode{ error_mode::Replacement };
		if constexpr (std::is_base_of_v<encoder, Coder>) mode = error_mode::Fatal;
		return run(coder, input, output, mode);
	}

	template <typename Coder, typename InputStream, typename OutputIterator>
	result_value run_decoder(name encoding_name, InputStream& input, OutputIterator output, error_mode mode)
	{
		switch (encoding_name)
		{
		case name::UTF_8: return run(UTF_8_decoder{}, input, output, mode);

			// Legacy single - byte encodings
		case name::IBM866:         return run(IBM866_decoder{}, input, output, mode);
		case name::ISO_8859_2:     return run(ISO_8859_2_decoder{}, input, output, mode);
		case name::ISO_8859_3:     return run(ISO_8859_3_decoder{}, input, output, mode);
		case name::ISO_8859_4:     return run(ISO_8859_4_decoder{}, input, output, mode);
		case name::ISO_8859_5:     return run(ISO_8859_5_decoder{}, input, output, mode);
		case name::ISO_8859_6:     return run(ISO_8859_6_decoder{}, input, output, mode);
		case name::ISO_8859_7:     return run(ISO_8859_7_decoder{}, input, output, mode);
		case name::ISO_8859_8:     return run(ISO_8859_8_decoder{}, input, output, mode);
		case name::ISO_8859_8_I:   return run(ISO_8859_8_I_decoder{}, input, output, mode);
		case name::ISO_8859_10:    return run(ISO_8859_10_decoder{}, input, output, mode);
		case name::ISO_8859_13:    return run(ISO_8859_13_decoder{}, input, output, mode);
		case name::ISO_8859_14:    return run(ISO_8859_14_decoder{}, input, output, mode);
		case name::ISO_8859_15:    return run(ISO_8859_15_decoder{}, input, output, mode);
		case name::ISO_8859_16:    return run(ISO_8859_16_decoder{}, input, output, mode);
		case name::KOI8_R:         return run(KOI8_R_decoder{}, input, output, mode);
		case name::KOI8_U:         return run(KOI8_U_decoder{}, input, output, mode);
		case name::macintosh:      return run(macintosh_decoder{}, input, output, mode);
		case name::windows_874:    return run(windows_874_decoder{}, input, output, mode);
		case name::windows_1250:   return run(windows_1250_decoder{}, input, output, mode);
		case name::windows_1251:   return run(windows_1251_decoder{}, input, output, mode);
		case name::windows_1252:   return run(windows_1252_decoder{}, input, output, mode);
		case name::windows_1253:   return run(windows_1253_decoder{}, input, output, mode);
		case name::windows_1254:   return run(windows_1254_decoder{}, input, output, mode);
		case name::windows_1255:   return run(windows_1255_decoder{}, input, output, mode);
		case name::windows_1256:   return run(windows_1256_decoder{}, input, output, mode);
		case name::windows_1257:   return run(windows_1257_decoder{}, input, output, mode);
		case name::windows_1258:   return run(windows_1258_decoder{}, input, output, mode);
		case name::x_mac_cyrillic: return run(x_mac_cyrillic_decoder{}, input, output, mode);

			// Legacy multi - byte Chinese(simplified) encodings
		case name::GBK:     return run(GBK_decoder{}, input, output, mode);
		case name::gb18030: return run(gb18030_decoder{}, input, output, mode);

			// Legacy multi - byte Chinese(traditional) encodings
		case name::Big5: return run(Big5_decoder{}, input, output, mode);

			// Legacy multi - byte Japanese encodings
		case name::EUC_JP:      return run(EUC_JP_decoder{}, input, output, mode);
		case name::ISO_2022_JP: return run(ISO_2022_JP_decoder{}, input, output, mode);
		case name::Shift_JIS:   return run(Shift_JIS_decoder{}, input, output, mode);

			// Legacy multi - byte Korean encodings
		case name::EUC_KR: return run(EUC_KR_decoder{}, input, output, mode);

			// Legacy miscellaneous encodings
		case name::replacement:    return run(replacement_decoder{}, input, output, mode);
		case name::UTF_16BE:       return run(UTF_16BE_decoder{}, input, output, mode);
		case name::UTF_16LE:       return run(UTF_16LE_decoder{}, input, output, mode);
		case name::x_user_defined: return run(x_user_defined_decoder{}, input, output, mode);
		}

		return result_error{};
	}

	template <typename InputStream, typename OutputIterator>
	result_value run_decoder(name encoding_name, InputStream& input, OutputIterator output)
	{
		error_mode mode{ error_mode::Replacement };
		return run(encoding_name, input, output, mode);
	}

	// 4.2. Names and labels --------------------------------------------------

	name get_name(std::u32string_view label);

	template <typename String>
	inline name get_name(String string) { return get_name(to_u32string(string)); }

	// 4.3. Output encodings --------------------------------------------------

	inline name get_output_encoding(name encoding)
	{
		switch (encoding)
		{
		case name::replacement:
		case name::UTF_16BE:
		case name::UTF_16LE:
			return name::UTF_8;
		}
		return encoding;
	}

	// 6. Hooks for standards -------------------------------------------------

	extern std::string_view BOM_UTF_8;
	extern std::string_view BOM_UTF_16BE;
	extern std::string_view BOM_UTF_16LE;

	template <typename InputStream, typename OutputIterator>
	inline void decode(InputStream input, name fallback, OutputIterator output)
	{
		static_assert(std::is_same_v<std::iterator_traits<OutputIterator>::value_type, uint32_t>);

		name encoding_name = fallback;

		// 1.
		std::array<uint8_t, 3> buffer{};
		uint32_t buffer_length{ 0 };
		// 2.
		bool BOM_seen_flag{ false };
		// 3.
		while (input && buffer_length < 3) buffer[buffer_length++] = input.read().value();
		// 4.
		if (std::equal(buffer.data(), buffer.data() + buffer_length, BOM_UTF_8.begin(), BOM_UTF_8.end()))
		{
			encoding_name = name::UTF_8;
			BOM_seen_flag = true;
		}
		else if (std::equal(buffer.data(), buffer.data() + buffer_length, BOM_UTF_8.begin(), BOM_UTF_16BE.end()))
		{
			encoding_name = name::UTF_16BE;
			BOM_seen_flag = true;
		}
		else if (std::equal(buffer.data(), buffer.data() + buffer_length, BOM_UTF_8.begin(), BOM_UTF_16LE.end()))
		{
			encoding_name = name::UTF_16LE;
			BOM_seen_flag = true;
		}
		// 5. 6.
		if (BOM_seen_flag == false) input.prepend(buffer.data(), buffer.data() + buffer_length);
		// 7.
		// 8. 9.
		run_decoder(encoding_name, input, output);
	}
}
