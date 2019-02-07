#pragma once

// https://encoding.spec.whatwg.org/
// https://triple-underscore.github.io/Encoding-ja.html

#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <deque>
#include <iterator>
#include <memory>
#include <string>
#include <string_view>
#include <variant>

#include <boost/container/static_vector.hpp>
#include <boost/algorithm/string.hpp>

//#include <wordring/string/numeric.hpp>
//#include <wordring/whatwg/encoding/encoding_atom.hpp>
//#include <wordring/whatwg/infra.hpp>

namespace wordring
{
namespace whatwg
{
namespace encoding
{
	// 3. Terminology ---------------------------------------------------------

	using stream    = std::deque<char>;
	using u32stream = std::deque<char32_t>;

	// 4.1. Encoders and decoders ---------------------------------------------

	//using 
	enum class error_mode : uint32_t
	{
		Replacement = 1,
		Fatal       = 2,
		Html        = 3
	};

	// 4.2. Names and labels --------------------------------------------------

	enum class name : uint32_t
	{
		UTF_8,

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

	name get_encoding(std::string_view label)
	{
		return name{ 0 };
	}
}
}
}
