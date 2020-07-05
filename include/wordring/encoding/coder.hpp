#pragma once

#include <wordring/encoding/encoding_defs.hpp>

#include <wordring/whatwg/encoding/coder.hpp>
#include <wordring/whatwg/infra/unicode.hpp>

#include <cstdint>
#include <deque>
#include <optional>
#include <variant>

namespace wordring::encoding::detail
{
	using decoder_holder = std::variant <
		wordring::whatwg::encoding::UTF_8_decoder,
		// Legacy single - byte encodings
		wordring::whatwg::encoding::IBM866_decoder,
		wordring::whatwg::encoding::ISO_8859_2_decoder,
		wordring::whatwg::encoding::ISO_8859_3_decoder,
		wordring::whatwg::encoding::ISO_8859_4_decoder,
		wordring::whatwg::encoding::ISO_8859_5_decoder,
		wordring::whatwg::encoding::ISO_8859_6_decoder,
		wordring::whatwg::encoding::ISO_8859_7_decoder,
		wordring::whatwg::encoding::ISO_8859_8_decoder,
		//wordring::whatwg::encoding::ISO_8859_8_I_decoder,
		wordring::whatwg::encoding::ISO_8859_10_decoder,
		wordring::whatwg::encoding::ISO_8859_13_decoder,
		wordring::whatwg::encoding::ISO_8859_14_decoder,
		wordring::whatwg::encoding::ISO_8859_15_decoder,
		wordring::whatwg::encoding::ISO_8859_16_decoder,
		wordring::whatwg::encoding::KOI8_R_decoder,
		wordring::whatwg::encoding::KOI8_U_decoder,
		wordring::whatwg::encoding::macintosh_decoder,
		wordring::whatwg::encoding::windows_874_decoder,
		wordring::whatwg::encoding::windows_1250_decoder,
		wordring::whatwg::encoding::windows_1251_decoder,
		wordring::whatwg::encoding::windows_1252_decoder,
		wordring::whatwg::encoding::windows_1253_decoder,
		wordring::whatwg::encoding::windows_1254_decoder,
		wordring::whatwg::encoding::windows_1255_decoder,
		wordring::whatwg::encoding::windows_1256_decoder,
		wordring::whatwg::encoding::windows_1257_decoder,
		wordring::whatwg::encoding::windows_1258_decoder,
		wordring::whatwg::encoding::x_mac_cyrillic_decoder,
		// Legacy multi - byte Chinese(simplified) encodings
		//wordring::whatwg::encoding::GBK_decoder,
		wordring::whatwg::encoding::gb18030_decoder,
		// Legacy multi - byte Chinese(traditional) encodings
		wordring::whatwg::encoding::Big5_decoder,
		// Legacy multi - byte Japanese encodings
		wordring::whatwg::encoding::EUC_JP_decoder,
		wordring::whatwg::encoding::ISO_2022_JP_decoder,
		wordring::whatwg::encoding::Shift_JIS_decoder,
		// Legacy multi - byte Korean encodings
		wordring::whatwg::encoding::EUC_KR_decoder,
		// Legacy miscellaneous encodings
		wordring::whatwg::encoding::replacement_decoder,
		wordring::whatwg::encoding::UTF_16BE_decoder,
		wordring::whatwg::encoding::UTF_16LE_decoder,
		wordring::whatwg::encoding::x_user_defined_decoder>;

	using encoder_holder = std::variant <
		wordring::whatwg::encoding::UTF_8_encoder,
		// Legacy single - byte encodings
		wordring::whatwg::encoding::IBM866_encoder,
		wordring::whatwg::encoding::ISO_8859_2_encoder,
		wordring::whatwg::encoding::ISO_8859_3_encoder,
		wordring::whatwg::encoding::ISO_8859_4_encoder,
		wordring::whatwg::encoding::ISO_8859_5_encoder,
		wordring::whatwg::encoding::ISO_8859_6_encoder,
		wordring::whatwg::encoding::ISO_8859_7_encoder,
		wordring::whatwg::encoding::ISO_8859_8_encoder,
		//wordring::whatwg::encoding::ISO_8859_8_I_encoder,
		wordring::whatwg::encoding::ISO_8859_10_encoder,
		wordring::whatwg::encoding::ISO_8859_13_encoder,
		wordring::whatwg::encoding::ISO_8859_14_encoder,
		wordring::whatwg::encoding::ISO_8859_15_encoder,
		wordring::whatwg::encoding::ISO_8859_16_encoder,
		wordring::whatwg::encoding::KOI8_R_encoder,
		wordring::whatwg::encoding::KOI8_U_encoder,
		wordring::whatwg::encoding::macintosh_encoder,
		wordring::whatwg::encoding::windows_874_encoder,
		wordring::whatwg::encoding::windows_1250_encoder,
		wordring::whatwg::encoding::windows_1251_encoder,
		wordring::whatwg::encoding::windows_1252_encoder,
		wordring::whatwg::encoding::windows_1253_encoder,
		wordring::whatwg::encoding::windows_1254_encoder,
		wordring::whatwg::encoding::windows_1255_encoder,
		wordring::whatwg::encoding::windows_1256_encoder,
		wordring::whatwg::encoding::windows_1257_encoder,
		wordring::whatwg::encoding::windows_1258_encoder,
		wordring::whatwg::encoding::x_mac_cyrillic_encoder,
		// Legacy multi - byte Chinese(simplified) encodings
		wordring::whatwg::encoding::GBK_encoder,
		wordring::whatwg::encoding::gb18030_encoder,
		// Legacy multi - byte Chinese(traditional) encodings
		wordring::whatwg::encoding::Big5_encoder,
		// Legacy multi - byte Japanese encodings
		wordring::whatwg::encoding::EUC_JP_encoder,
		wordring::whatwg::encoding::ISO_2022_JP_encoder,
		wordring::whatwg::encoding::Shift_JIS_encoder,
		// Legacy multi - byte Korean encodings
		wordring::whatwg::encoding::EUC_KR_encoder,
		// Legacy miscellaneous encodings
		wordring::whatwg::encoding::x_user_defined_encoder>;

	struct decoder_stream
	{
		std::deque<std::uint8_t> m_buffer;

		void prepend(std::uint8_t token) { m_buffer.push_front(token); }

		template <typename InputIterator>
		void prepend(InputIterator first, InputIterator last) { m_buffer.insert(m_buffer.begin(), first, last); }

		template <typename Value>
		void prepend(std::initializer_list<Value> tokens) { prepend(tokens.begin(), tokens.end()); }

	};

	struct encoder_stream
	{
		std::deque<std::uint32_t> m_buffer;

		void prepend(std::uint32_t token) { m_buffer.push_front(token); }

		template <typename InputIterator>
		void prepend(InputIterator first, InputIterator last) { m_buffer.insert(m_buffer.begin(), first, last); }

		template <typename Value>
		void prepend(std::initializer_list<Value> tokens) { prepend(tokens.begin(), tokens.end()); }

	};

	inline decoder_holder get_decoder(encoding_name name)
	{
		using namespace wordring::whatwg::encoding;

		switch (name)
		{
		case encoding_name::UTF_8:          return UTF_8_decoder();
			// Legacy single - byte encodings
		case encoding_name::IBM866:         return IBM866_decoder();
		case encoding_name::ISO_8859_2:     return ISO_8859_2_decoder();
		case encoding_name::ISO_8859_3:     return ISO_8859_3_decoder();
		case encoding_name::ISO_8859_4:     return ISO_8859_4_decoder();
		case encoding_name::ISO_8859_5:     return ISO_8859_5_decoder();
		case encoding_name::ISO_8859_6:     return ISO_8859_6_decoder();
		case encoding_name::ISO_8859_7:     return ISO_8859_7_decoder();
		case encoding_name::ISO_8859_8:     return ISO_8859_8_decoder();
		case encoding_name::ISO_8859_8_I:   return ISO_8859_8_I_decoder();
		case encoding_name::ISO_8859_10:    return ISO_8859_10_decoder();
		case encoding_name::ISO_8859_13:    return ISO_8859_13_decoder();
		case encoding_name::ISO_8859_14:    return ISO_8859_14_decoder();
		case encoding_name::ISO_8859_15:    return ISO_8859_15_decoder();
		case encoding_name::ISO_8859_16:    return ISO_8859_16_decoder();
		case encoding_name::KOI8_R:         return KOI8_R_decoder();
		case encoding_name::KOI8_U:         return KOI8_U_decoder();
		case encoding_name::macintosh:      return macintosh_decoder();
		case encoding_name::windows_874:    return windows_874_decoder();
		case encoding_name::windows_1250:   return windows_1250_decoder();
		case encoding_name::windows_1251:   return windows_1251_decoder();
		case encoding_name::windows_1252:   return windows_1252_decoder();
		case encoding_name::windows_1253:   return windows_1253_decoder();
		case encoding_name::windows_1254:   return windows_1254_decoder();
		case encoding_name::windows_1255:   return windows_1255_decoder();
		case encoding_name::windows_1256:   return windows_1256_decoder();
		case encoding_name::windows_1257:   return windows_1257_decoder();
		case encoding_name::windows_1258:   return windows_1258_decoder();
		case encoding_name::x_mac_cyrillic: return x_mac_cyrillic_decoder();
			// Legacy multi - byte Chinese(simplified) encodings
		case encoding_name::GBK:            return GBK_decoder();
		case encoding_name::gb18030:        return gb18030_decoder();
			// Legacy multi - byte Chinese(traditional) encodings
		case encoding_name::Big5:           return Big5_decoder();
			// Legacy multi - byte Japanese encodings
		case encoding_name::EUC_JP:         return EUC_JP_decoder();
		case encoding_name::ISO_2022_JP:    return ISO_2022_JP_decoder();
		case encoding_name::Shift_JIS:      return Shift_JIS_decoder();
			// Legacy multi - byte Korean encodings
		case encoding_name::EUC_KR:         return EUC_KR_decoder();
			// Legacy miscellaneous encodings
		case encoding_name::replacement:    return replacement_decoder();
		case encoding_name::UTF_16BE:       return UTF_16BE_decoder();
		case encoding_name::UTF_16LE:       return UTF_16LE_decoder();
		case encoding_name::x_user_defined: return x_user_defined_decoder();
		default: break;
		}

		return decoder_holder();
	}

	inline encoder_holder get_encoder(encoding_name name)
	{
		using namespace wordring::whatwg::encoding;

		switch (name)
		{
		case encoding_name::UTF_8:          return UTF_8_encoder();
			// Legacy single - byte encodings
		case encoding_name::IBM866:         return IBM866_encoder();
		case encoding_name::ISO_8859_2:     return ISO_8859_2_encoder();
		case encoding_name::ISO_8859_3:     return ISO_8859_3_encoder();
		case encoding_name::ISO_8859_4:     return ISO_8859_4_encoder();
		case encoding_name::ISO_8859_5:     return ISO_8859_5_encoder();
		case encoding_name::ISO_8859_6:     return ISO_8859_6_encoder();
		case encoding_name::ISO_8859_7:     return ISO_8859_7_encoder();
		case encoding_name::ISO_8859_8:     return ISO_8859_8_encoder();
		case encoding_name::ISO_8859_8_I:   return ISO_8859_8_I_encoder();
		case encoding_name::ISO_8859_10:    return ISO_8859_10_encoder();
		case encoding_name::ISO_8859_13:    return ISO_8859_13_encoder();
		case encoding_name::ISO_8859_14:    return ISO_8859_14_encoder();
		case encoding_name::ISO_8859_15:    return ISO_8859_15_encoder();
		case encoding_name::ISO_8859_16:    return ISO_8859_16_encoder();
		case encoding_name::KOI8_R:         return KOI8_R_encoder();
		case encoding_name::KOI8_U:         return KOI8_U_encoder();
		case encoding_name::macintosh:      return macintosh_encoder();
		case encoding_name::windows_874:    return windows_874_encoder();
		case encoding_name::windows_1250:   return windows_1250_encoder();
		case encoding_name::windows_1251:   return windows_1251_encoder();
		case encoding_name::windows_1252:   return windows_1252_encoder();
		case encoding_name::windows_1253:   return windows_1253_encoder();
		case encoding_name::windows_1254:   return windows_1254_encoder();
		case encoding_name::windows_1255:   return windows_1255_encoder();
		case encoding_name::windows_1256:   return windows_1256_encoder();
		case encoding_name::windows_1257:   return windows_1257_encoder();
		case encoding_name::windows_1258:   return windows_1258_encoder();
		case encoding_name::x_mac_cyrillic: return x_mac_cyrillic_encoder();
			// Legacy multi - byte Chinese(simplified) encodings
		case encoding_name::GBK:            return GBK_encoder();
		case encoding_name::gb18030:        return gb18030_encoder();
			// Legacy multi - byte Chinese(traditional) encodings
		case encoding_name::Big5:           return Big5_encoder();
			// Legacy multi - byte Japanese encodings
		case encoding_name::EUC_JP:         return EUC_JP_encoder();
		case encoding_name::ISO_2022_JP:    return ISO_2022_JP_encoder();
		case encoding_name::Shift_JIS:      return Shift_JIS_encoder();
			// Legacy multi - byte Korean encodings
		case encoding_name::EUC_KR:         return EUC_KR_encoder();
			// Legacy miscellaneous encodings
		case encoding_name::x_user_defined: return x_user_defined_encoder();
		default: break;
		}

		return encoder_holder();
	}
};

namespace wordring::encoding
{
	class decoder
	{
	public:
		decoder(encoding_name name, error_mode_name mode = error_mode_name::Replacement)
			: m_decoder(detail::get_decoder(name))
			, m_mode(mode)
		{
		}

		template <typename OutputIterator>
		result_value push(std::uint8_t ch, OutputIterator out)
		{
			using wordring::whatwg::encoding::process_token;

			auto token = std::optional<uint8_t>(ch);

			result_value ret = std::visit([token, out, this](auto& dec) { return process_token(token, dec, m_stream, out, m_mode); }, m_decoder);
			if (!std::holds_alternative<result_continue>(ret)) return ret;

			while (!m_stream.m_buffer.empty())
			{
				token = m_stream.m_buffer.front();
				m_stream.m_buffer.pop_front();
				ret = std::visit([token, out, this](auto& dec) { return process_token(token, dec, m_stream, out, m_mode); }, m_decoder);
				if (!std::holds_alternative<result_continue>(ret)) return ret;
			}

			return result_continue();
		}

	protected:
		detail::decoder_holder m_decoder;
		detail::decoder_stream m_stream;
		error_mode_name m_mode;
	};

	class encoder
	{
	public:
		encoder(encoding_name name, error_mode_name mode = error_mode_name::Fatal)
			: m_encoder(detail::get_encoder(name))
			, m_mode(mode)
		{
		}

		template <typename OutputIterator>
		result_value push(std::uint32_t cp, OutputIterator out)
		{
			using wordring::whatwg::encoding::process_token;

			auto token = std::optional<uint32_t>(cp);

			result_value ret = std::visit([token, out, this](auto& dec) { return process_token(token, dec, m_stream, out, m_mode); }, m_encoder);
			if (!std::holds_alternative<result_continue>(ret)) return ret;

			while (!m_stream.m_buffer.empty())
			{
				token = m_stream.m_buffer.front();
				m_stream.m_buffer.pop_front();
				ret = std::visit([token, out, this](auto& dec) { return process_token(token, dec, m_stream, out, m_mode); }, m_encoder);
				if (!std::holds_alternative<result_continue>(ret)) return ret;
			}

			return result_continue();
		}

	protected:
		detail::encoder_holder m_encoder;
		detail::encoder_stream m_stream;
		error_mode_name m_mode;
	};
}
