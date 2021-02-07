#pragma once

// https://encoding.spec.whatwg.org/
// https://triple-underscore.github.io/Encoding-ja.html

#include <array>
#include <cstdint>
#include <variant>

namespace wordring::whatwg::encoding
{
	// 4.1. Encoders and decoders ---------------------------------------------

	class encoder {};
	class decoder {};

	class result_finished {};
	class result_continue {};

	using result_code_point = uint32_t;
	using result_code_points_2 = std::array<uint32_t, 2>;

	using result_byte = uint8_t;
	using result_bytes_2 = std::array<uint8_t, 2>;
	using result_bytes_3 = std::array<uint8_t, 3>;
	using result_bytes_4 = std::array<uint8_t, 4>;

	struct result_error
	{
		result_error() : code_point{ 0xFFFFFFFFu } {}
		result_error(uint32_t cp) : code_point{ cp } {}
		uint32_t code_point;
	};

	using result_value = std::variant<
		result_finished,      // 0.
		result_continue,      // 1.
		result_code_point,    // 2.
		result_code_points_2, // 3.
		result_byte,          // 4.
		result_bytes_2,       // 5.
		result_bytes_3,       // 6.
		result_bytes_4,       // 7.
		result_error          // 8.
	>;

	enum class error_mode_name : uint32_t
	{
		None = 0,
		Replacement = 1,
		Fatal = 2,
		Html = 3
	};

	// 4.2. Names and labels --------------------------------------------------

	enum class encoding_name : uint32_t
	{
		UTF_8 = 1,

		// Legacy single - byte encodings
		IBM866,
		ISO_8859_2,
		ISO_8859_3,
		ISO_8859_4,
		ISO_8859_5,
		ISO_8859_6,
		ISO_8859_7,
		ISO_8859_8,
		ISO_8859_8_I,
		ISO_8859_10,
		ISO_8859_13,
		ISO_8859_14,
		ISO_8859_15,
		ISO_8859_16,
		KOI8_R,
		KOI8_U,
		macintosh,
		windows_874,
		windows_1250,
		windows_1251,
		windows_1252,
		windows_1253,
		windows_1254,
		windows_1255,
		windows_1256,
		windows_1257,
		windows_1258,
		x_mac_cyrillic,

		// Legacy multi - byte Chinese(simplified) encodings
		GBK,
		gb18030,

		// Legacy multi - byte Chinese(traditional) encodings
		Big5,

		// Legacy multi - byte Japanese encodings
		EUC_JP,
		ISO_2022_JP,
		Shift_JIS,

		// Legacy multi - byte Korean encodings
		EUC_KR,

		// Legacy miscellaneous encodings
		replacement,
		UTF_16BE,
		UTF_16LE,
		x_user_defined,
	};
}

namespace wordring::whatwg
{
	using encoding_name = encoding::encoding_name;
}
