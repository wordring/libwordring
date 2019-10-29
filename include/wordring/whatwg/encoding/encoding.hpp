#pragma once
// whatwg/encoding/encoding.hpp

// https://encoding.spec.whatwg.org/
// https://triple-underscore.github.io/Encoding-ja.html

#include <cassert>
#include <charconv>
#include <cstdint>
#include <string_view>
#include <type_traits>
#include <variant>

//#include <wordring/algorithm.hpp>
//#include <wordring/compatibility.hpp>
#include <wordring/whatwg/encoding/coder.hpp>
#include <wordring/whatwg/encoding/encoding_defs.hpp>
#include <wordring/whatwg/encoding/stream.hpp>
//#include <wordring/whatwg/infra/infra.hpp>

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

	template <typename InputStream, typename OutputIterator>
	result_value run_decoder(name encoding_name, InputStream& input, OutputIterator output, error_mode mode = error_mode::Replacement)
	{
		switch (encoding_name)
		{
		case name::UTF_8:
		{
			UTF_8_decoder c{};
			return run(c, input, output, mode);
		}
		// Legacy single - byte encodings
		case name::IBM866:
		{
			IBM866_decoder c{};
			return run(c, input, output, mode);
		}
		case name::ISO_8859_2:
		{
			ISO_8859_2_decoder c{};
			return run(c, input, output, mode);
		}
		case name::ISO_8859_3:
		{
			ISO_8859_3_decoder c{};
			return run(c, input, output, mode);
		}
		case name::ISO_8859_4:
		{
			ISO_8859_4_decoder c{};
			return run(c, input, output, mode);
		}
		case name::ISO_8859_5:
		{
			ISO_8859_5_decoder c{};
			return run(c, input, output, mode);
		}
		case name::ISO_8859_6:
		{
			ISO_8859_6_decoder c{};
			return run(c, input, output, mode);
		}
		case name::ISO_8859_7:
		{
			ISO_8859_7_decoder c{};
			return run(c, input, output, mode);
		}
		case name::ISO_8859_8:
		{
			ISO_8859_8_decoder c{};
			return run(c, input, output, mode);
		}
		case name::ISO_8859_8_I:
		{
			ISO_8859_8_I_decoder c{};
			return run(c, input, output, mode);
		}
		case name::ISO_8859_10:
		{
			ISO_8859_10_decoder c{};
			return run(c, input, output, mode);
		}
		case name::ISO_8859_13:
		{
			ISO_8859_13_decoder c{};
			return run(c, input, output, mode);
		}
		case name::ISO_8859_14:
		{
			ISO_8859_14_decoder c{};
			return run(c, input, output, mode);
		}
		case name::ISO_8859_15:
		{
			ISO_8859_15_decoder c{};
			return run(c, input, output, mode);
		}
		case name::ISO_8859_16:
		{
			ISO_8859_16_decoder c{};
			return run(c, input, output, mode);
		}
		case name::KOI8_R:
		{
			KOI8_R_decoder c{};
			return run(c, input, output, mode);
		}
		case name::KOI8_U:
		{
			KOI8_U_decoder c{};
			return run(c, input, output, mode);
		}
		case name::macintosh:
		{
			macintosh_decoder c{};
			return run(c, input, output, mode);
		}
		case name::windows_874:
		{
			windows_874_decoder c{};
			return run(c, input, output, mode);
		}
		case name::windows_1250:
		{
			windows_1250_decoder c{};
			return run(c, input, output, mode);
		}
		case name::windows_1251:
		{
			windows_1251_decoder c{};
			return run(c, input, output, mode);
		}
		case name::windows_1252:
		{
			windows_1252_decoder c{};
			return run(c, input, output, mode);
		}
		case name::windows_1253:
		{
			windows_1253_decoder c{};
			return run(c, input, output, mode);
		}
		case name::windows_1254:
		{
			windows_1254_decoder c{};
			return run(c, input, output, mode);
		}
		case name::windows_1255:	
		{
			windows_1255_decoder c{};
			return run(c, input, output, mode);
		}
		case name::windows_1256:
		{
			windows_1256_decoder c{};
			return run(c, input, output, mode);
		}
		case name::windows_1257:
		{
			windows_1257_decoder c{};
			return run(c, input, output, mode);
		}
		case name::windows_1258:
		{
			windows_1258_decoder c{};
			return run(c, input, output, mode);
		}
		case name::x_mac_cyrillic:
		{
			x_mac_cyrillic_decoder c{};
			return run(c, input, output, mode);
		}
		// Legacy multi - byte Chinese(simplified) encodings
		case name::GBK:
		{
			GBK_decoder c{};
			return run(c, input, output, mode);
		}
		case name::gb18030:
		{
			gb18030_decoder c{};
			return run(c, input, output, mode);
		}
		// Legacy multi - byte Chinese(traditional) encodings
		case name::Big5:
		{
			Big5_decoder c{};
			return run(c, input, output, mode);
		}
		// Legacy multi - byte Japanese encodings
		case name::EUC_JP:
		{
			EUC_JP_decoder c{};
			return run(c, input, output, mode);
		}
		case name::ISO_2022_JP:
		{
			ISO_2022_JP_decoder c{};
			return run(c, input, output, mode);
		}
		case name::Shift_JIS:
		{
			Shift_JIS_decoder c{};
			return run(c, input, output, mode);
		}
		// Legacy multi - byte Korean encodings
		case name::EUC_KR:
		{
			EUC_KR_decoder c{};
			return run(c, input, output, mode);
		}
		// Legacy miscellaneous encodings
		case name::replacement:
		{
			replacement_decoder c{};
			return run(c, input, output, mode);
		}
		case name::UTF_16BE:
		{
			UTF_16BE_decoder c{};
			return run(c, input, output, mode);
		}
		case name::UTF_16LE:
		{
			UTF_16LE_decoder c{};
			return run(c, input, output, mode);
		}
		case name::x_user_defined:
		{
			x_user_defined_decoder c{};
			return run(c, input, output, mode);
		}
		}
		assert(false);
		return result_error{};
	}

	template <typename InputStream, typename OutputIterator>
	result_value run_encoder(name encoding_name, InputStream& input, OutputIterator output, error_mode mode = error_mode::Fatal)
	{
		switch (encoding_name)
		{
		case name::UTF_8:
		{
			UTF_8_encoder c{};
			return run(c, input, output, mode);
		}
		// Legacy single - byte encodings
		case name::IBM866:
		{
			IBM866_encoder c{};
			return run(c, input, output, mode);
		}
		case name::ISO_8859_2:
		{
			ISO_8859_2_encoder c{};
			return run(c, input, output, mode);
		}
		case name::ISO_8859_3:
		{
			ISO_8859_3_encoder c{};
			return run(c, input, output, mode);
		}
		case name::ISO_8859_4:
		{
			ISO_8859_4_encoder c{};
			return run(c, input, output, mode);
		}
		case name::ISO_8859_5:
		{
			ISO_8859_5_encoder c{};
			return run(c, input, output, mode);
		}
		case name::ISO_8859_6:
		{
			ISO_8859_6_encoder c{};
			return run(c, input, output, mode);
		}
		case name::ISO_8859_7:
		{
			ISO_8859_7_encoder c{};
			return run(c, input, output, mode);
		}
		case name::ISO_8859_8:
		{
			ISO_8859_8_encoder c{};
			return run(c, input, output, mode);
		}
		case name::ISO_8859_8_I:
		{
			ISO_8859_8_I_encoder c{};
			return run(c, input, output, mode);
		}
		case name::ISO_8859_10:
		{
			ISO_8859_10_encoder c{};
			return run(c, input, output, mode);
		}
		case name::ISO_8859_13:
		{
			ISO_8859_13_encoder c{};
			return run(c, input, output, mode);
		}
		case name::ISO_8859_14:
		{
			ISO_8859_14_encoder c{};
			return run(c, input, output, mode);
		}
		case name::ISO_8859_15:
		{
			ISO_8859_15_encoder c{};
			return run(c, input, output, mode);
		}
		case name::ISO_8859_16:
		{
			ISO_8859_16_encoder c{};
			return run(c, input, output, mode);
		}
		case name::KOI8_R:
		{
			KOI8_R_encoder c{};
			return run(c, input, output, mode);
		}
		case name::KOI8_U:
		{
			KOI8_U_encoder c{};
			return run(c, input, output, mode);
		}
		case name::macintosh:
		{
			macintosh_encoder c{};
			return run(c, input, output, mode);
		}
		case name::windows_874:
		{
			windows_874_encoder c{};
			return run(c, input, output, mode);
		}
		case name::windows_1250:
		{
			windows_1250_encoder c{};
			return run(c, input, output, mode);
		}
		case name::windows_1251:
		{
			windows_1251_encoder c{};
			return run(c, input, output, mode);
		}
		case name::windows_1252:
		{
			windows_1252_encoder c{};
			return run(c, input, output, mode);
		}
		case name::windows_1253:
		{
			windows_1253_encoder c{};
			return run(c, input, output, mode);
		}
		case name::windows_1254:
		{
			windows_1254_encoder c{};
			return run(c, input, output, mode);
		}
		case name::windows_1255:
		{
			windows_1255_encoder c{};
			return run(c, input, output, mode);
		}
		case name::windows_1256:
		{
			windows_1256_encoder c{};
			return run(c, input, output, mode);
		}
		case name::windows_1257:
		{
			windows_1257_encoder c{};
			return run(c, input, output, mode);
		}
		case name::windows_1258:
		{
			windows_1258_encoder c{};
			return run(c, input, output, mode);
		}
		case name::x_mac_cyrillic:
		{
			x_mac_cyrillic_encoder c{};
			return run(c, input, output, mode);
		}
		// Legacy multi - byte Chinese(simplified) encodings
		case name::GBK:
		{
			GBK_encoder c{};
			return run(c, input, output, mode);
		}
		case name::gb18030:
		{
			gb18030_encoder c{};
			return run(c, input, output, mode);
		}
		// Legacy multi - byte Chinese(traditional) encodings
		case name::Big5:
		{
			Big5_encoder c{};
			return run(c, input, output, mode);
		}
		// Legacy multi - byte Japanese encodings
		case name::EUC_JP:
		{
			EUC_JP_encoder c{};
			return run(c, input, output, mode);
		}
		case name::ISO_2022_JP:
		{
			ISO_2022_JP_encoder c{};
			return run(c, input, output, mode);
		}
		case name::Shift_JIS:
		{
			Shift_JIS_encoder c{};
			return run(c, input, output, mode);
		}
		// Legacy multi - byte Korean encodings
		case name::EUC_KR:
		{
			EUC_KR_encoder c{};
			return run(c, input, output, mode);
		}
		// Legacy miscellaneous encodings
		case name::x_user_defined:
		{
			x_user_defined_encoder c{};
			return run(c, input, output, mode);
		}
		}
		assert(false);
		return result_error{};
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

	template <typename InputIterator, typename OutputIterator>
	inline void decode(InputIterator first, InputIterator last, name fallback, OutputIterator output)
	{
		std::string_view BOM_UTF_8{ "\xEF\xBB\xBF" };
		std::string_view BOM_UTF_16BE{ "\xFE\xFF" };
		std::string_view BOM_UTF_16LE{ "\xFF\xFE" };

		stream<InputIterator> input{ first, last };
		name encoding_name = fallback;

		// 1.
		std::string buffer{};
		// 2.
		bool BOM_seen_flag{ true };
		// 3.
		for (uint32_t i = 0; i < 3 && input; i++) buffer.push_back(input.read().value());
		// 4.
		if (BOM_UTF_8.size() <= buffer.size() && std::equal(BOM_UTF_8.begin(), BOM_UTF_8.end(), buffer.begin()))
			encoding_name = name::UTF_8;
		else if (BOM_UTF_16BE.size() <= buffer.size() && std::equal(BOM_UTF_16BE.begin(), BOM_UTF_16BE.end(), buffer.begin()))
			encoding_name = name::UTF_16BE;
		else if (BOM_UTF_16LE.size() <= buffer.size() && std::equal(BOM_UTF_16LE.begin(), BOM_UTF_16LE.end(), buffer.begin()))
			encoding_name = name::UTF_16LE;
		else BOM_seen_flag = false;
		// 5. 6.
		if (BOM_seen_flag == false) input.prepend(buffer.begin(), buffer.end());
		else if (encoding_name != name::UTF_8 && buffer.size() == 3) input.prepend(buffer.back());
		// 7.
		// 8. 9.
		run_decoder(encoding_name, input, output);
	}

	template <typename InputIterator, typename OutputIterator>
	inline void utf_8_decode(InputIterator first, InputIterator last, OutputIterator output)
	{
		std::string_view BOM_UTF_8{ "\xEF\xBB\xBF" };
		stream<InputIterator> input{ first, last };

		// 1.
		std::string buffer{};
		// 2.
		for (uint32_t i = 0; i < 3 && input; i++) buffer.push_back(input.read().value());
		// 3.
		if (!(BOM_UTF_8.size() <= buffer.size()) || !std::equal(BOM_UTF_8.begin(), BOM_UTF_8.end(), buffer.begin()))
			input.prepend(buffer.begin(), buffer.end());
		// 5.
		run_decoder(name::UTF_8, input, output);
	}

	template <typename InputIterator, typename OutputIterator>
	inline void utf_8_decode_without_bom(InputIterator first, InputIterator last, OutputIterator output)
	{
		stream<InputIterator> input{ first, last };
		run_decoder(name::UTF_8, input, output);
	}

	template <typename InputIterator, typename OutputIterator>
	inline bool utf_8_decode_without_bom_or_fail(InputIterator first, InputIterator last, OutputIterator output)
	{
		stream<InputIterator> input{ first, last };
		std::u32string buffer{};
		result_value ret = run_decoder(name::UTF_8, input, std::back_inserter(buffer), error_mode::Fatal);
		if (ret.index() == 0) std::copy(buffer.begin(), buffer.end(), output);
		return ret.index() == 0;
	}

	template <typename InputIterator, typename OutputIterator>
	inline void encode(InputIterator first, InputIterator last, name encoding_name, OutputIterator output)
	{
		// 1.
		assert(encoding_name != name::replacement);
		assert(encoding_name != name::UTF_16BE);
		assert(encoding_name != name::UTF_16LE);

		stream<InputIterator> input{ first, last };

		run_encoder(encoding_name, input, output, error_mode::Html);
	}

	template <typename InputIterator, typename OutputIterator>
	inline void utf_8_encode(InputIterator first, InputIterator last, OutputIterator output)
	{
		encode(first, last, name::UTF_8, output);
	}
}
