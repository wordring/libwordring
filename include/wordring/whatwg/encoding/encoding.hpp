#pragma once

// https://encoding.spec.whatwg.org/
// https://triple-underscore.github.io/Encoding-ja.html

#include <cassert>
#include <string_view>
#include <unordered_map>

#include <wordring/compatibility.hpp>
#include <wordring/whatwg/encoding/encoding_stream.hpp>
#include <wordring/whatwg/infra/infra.hpp>

namespace wordring::whatwg
{
	// 4.1. Encoders and decoders ---------------------------------------------

	enum class encoding_handler_result
	{
		None     = 0,
		Finished = 1,
		Error    = 2,
		Continue = 3
	};

	enum class encoding_error_mode : uint32_t
	{
		None        = 0,
		Replacement = 1,
		Fatal       = 2,
		Html        = 3
	};

	encoding_handler_result process_encoding_token()
	{
		return encoding_handler_result::None;
	}

	template <typename InputStream, typename OutputStream>
	encoding_handler_result run_encoding_coder(
		InputStream input, OutputStream output, encoding_error_mode error_mode = encoding_error_mode::None)
	{

	}


	// 4.2. Names and labels --------------------------------------------------

	enum class encoding_name : uint32_t
	{
		None = 0,

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

	template <typename String>
	encoding_name get_encoding_name(String label)
	{
		static std::unordered_map<std::u32string, encoding_name> const map = {
			// The Encoding
			{ U"unicode-1-1-utf-8", encoding_name::UTF_8 },
			{ U"utf-8",             encoding_name::UTF_8 },
			{ U"utf8",              encoding_name::UTF_8 },

			// Legacy single-byte encodings
			{ U"866",      encoding_name::IBM866 },
			{ U"cp866",    encoding_name::IBM866 },
			{ U"csibm866", encoding_name::IBM866 },
			{ U"ibm866",   encoding_name::IBM866 },

			{ U"csisolatin2",     encoding_name::ISO_8859_2 },
			{ U"iso-8859-2",      encoding_name::ISO_8859_2 },
			{ U"iso-ir-101",      encoding_name::ISO_8859_2 },
			{ U"iso8859-2",       encoding_name::ISO_8859_2 },
			{ U"iso88592",        encoding_name::ISO_8859_2 },
			{ U"iso_8859-2",      encoding_name::ISO_8859_2 },
			{ U"iso_8859-2:1987", encoding_name::ISO_8859_2 },
			{ U"l2",              encoding_name::ISO_8859_2 },
			{ U"latin2",          encoding_name::ISO_8859_2 },

			{ U"csisolatin3",     encoding_name::ISO_8859_3 },
			{ U"iso-8859-3",      encoding_name::ISO_8859_3 },
			{ U"iso-ir-109",      encoding_name::ISO_8859_3 },
			{ U"iso8859-3",       encoding_name::ISO_8859_3 },
			{ U"iso88593",        encoding_name::ISO_8859_3 },
			{ U"iso_8859-3",      encoding_name::ISO_8859_3 },
			{ U"iso_8859-3:1988", encoding_name::ISO_8859_3 },
			{ U"l3",              encoding_name::ISO_8859_3 },
			{ U"latin3",          encoding_name::ISO_8859_3 },

			{ U"csisolatin4",     encoding_name::ISO_8859_4 },
			{ U"iso-8859-4",      encoding_name::ISO_8859_4 },
			{ U"iso-ir-110",      encoding_name::ISO_8859_4 },
			{ U"iso8859-4",       encoding_name::ISO_8859_4 },
			{ U"iso88594",        encoding_name::ISO_8859_4 },
			{ U"iso_8859-4",      encoding_name::ISO_8859_4 },
			{ U"iso_8859-4:1988", encoding_name::ISO_8859_4 },
			{ U"l4",              encoding_name::ISO_8859_4 },
			{ U"latin4",          encoding_name::ISO_8859_4 },

			{ U"csisolatincyrillic", encoding_name::ISO_8859_5 },
			{ U"cyrillic",           encoding_name::ISO_8859_5 },
			{ U"iso-8859-5",         encoding_name::ISO_8859_5 },
			{ U"iso-ir-144",         encoding_name::ISO_8859_5 },
			{ U"iso8859-5",          encoding_name::ISO_8859_5 },
			{ U"iso88595",           encoding_name::ISO_8859_5 },
			{ U"iso_8859-5",         encoding_name::ISO_8859_5 },
			{ U"iso_8859-5:1988",    encoding_name::ISO_8859_5 },

			{ U"arabic",           encoding_name::ISO_8859_6 },
			{ U"asmo-708",         encoding_name::ISO_8859_6 },
			{ U"csiso88596e",      encoding_name::ISO_8859_6 },
			{ U"csiso88596i",      encoding_name::ISO_8859_6 },
			{ U"csisolatinarabic", encoding_name::ISO_8859_6 },
			{ U"ecma-114",         encoding_name::ISO_8859_6 },
			{ U"iso-8859-6",       encoding_name::ISO_8859_6 },
			{ U"iso-8859-6-e",     encoding_name::ISO_8859_6 },
			{ U"iso-8859-6-i",     encoding_name::ISO_8859_6 },
			{ U"iso-ir-127",       encoding_name::ISO_8859_6 },
			{ U"iso8859-6",        encoding_name::ISO_8859_6 },
			{ U"iso88596",         encoding_name::ISO_8859_6 },
			{ U"iso_8859-6",       encoding_name::ISO_8859_6 },
			{ U"iso_8859-6:1987",  encoding_name::ISO_8859_6 },

			{ U"csisolatingreek", encoding_name::ISO_8859_7 },
			{ U"ecma-118",        encoding_name::ISO_8859_7 },
			{ U"elot_928",        encoding_name::ISO_8859_7 },
			{ U"greek",           encoding_name::ISO_8859_7 },
			{ U"greek8",          encoding_name::ISO_8859_7 },
			{ U"iso-8859-7",      encoding_name::ISO_8859_7 },
			{ U"iso-ir-126",      encoding_name::ISO_8859_7 },
			{ U"iso8859-7",       encoding_name::ISO_8859_7 },
			{ U"iso88597",        encoding_name::ISO_8859_7 },
			{ U"iso_8859-7",      encoding_name::ISO_8859_7 },
			{ U"iso_8859-7:1987", encoding_name::ISO_8859_7 },
			{ U"sun_eu_greek",    encoding_name::ISO_8859_7 },

			{ U"csiso88598e",      encoding_name::ISO_8859_8 },
			{ U"csisolatinhebrew", encoding_name::ISO_8859_8 },
			{ U"hebrew",           encoding_name::ISO_8859_8 },
			{ U"iso-8859-8",       encoding_name::ISO_8859_8 },
			{ U"iso-8859-8-e",     encoding_name::ISO_8859_8 },
			{ U"iso-ir-138",       encoding_name::ISO_8859_8 },
			{ U"iso8859-8",        encoding_name::ISO_8859_8 },
			{ U"iso88598",         encoding_name::ISO_8859_8 },
			{ U"iso_8859-8",       encoding_name::ISO_8859_8 },
			{ U"iso_8859-8:1988",  encoding_name::ISO_8859_8 },
			{ U"visual",           encoding_name::ISO_8859_8 },

			{ U"csiso88598i",  encoding_name::ISO_8859_8_I },
			{ U"iso-8859-8-i", encoding_name::ISO_8859_8_I },
			{ U"logical",      encoding_name::ISO_8859_8_I },

			{ U"csisolatin6", encoding_name::ISO_8859_10 },
			{ U"iso-8859-10", encoding_name::ISO_8859_10 },
			{ U"iso-ir-157",  encoding_name::ISO_8859_10 },
			{ U"iso8859-10",  encoding_name::ISO_8859_10 },
			{ U"iso885910",   encoding_name::ISO_8859_10 },
			{ U"l6",          encoding_name::ISO_8859_10 },
			{ U"latin6",      encoding_name::ISO_8859_10 },

			{ U"iso-8859-13", encoding_name::ISO_8859_13 },
			{ U"iso8859-13",  encoding_name::ISO_8859_13 },
			{ U"iso885913",   encoding_name::ISO_8859_13 },

			{ U"iso-8859-14", encoding_name::ISO_8859_14 },
			{ U"iso8859-14",  encoding_name::ISO_8859_14 },
			{ U"iso885914",   encoding_name::ISO_8859_14 },

			{ U"csisolatin9", encoding_name::ISO_8859_15 },
			{ U"iso-8859-15", encoding_name::ISO_8859_15 },
			{ U"iso8859-15",  encoding_name::ISO_8859_15 },
			{ U"iso885915",   encoding_name::ISO_8859_15 },
			{ U"iso_8859-15", encoding_name::ISO_8859_15 },
			{ U"l9",          encoding_name::ISO_8859_15 },

			{ U"iso-8859-16", encoding_name::ISO_8859_16 },

			{ U"cskoi8r", encoding_name::KOI8_R },
			{ U"koi",     encoding_name::KOI8_R },
			{ U"koi8",    encoding_name::KOI8_R },
			{ U"koi8-r",  encoding_name::KOI8_R },
			{ U"koi8_r",  encoding_name::KOI8_R },

			{ U"koi8-ru", encoding_name::KOI8_U },
			{ U"koi8-u",  encoding_name::KOI8_U },

			{ U"csmacintosh", encoding_name::macintosh },
			{ U"mac",         encoding_name::macintosh },
			{ U"macintosh",   encoding_name::macintosh },
			{ U"x-mac-roman", encoding_name::macintosh },

			{ U"dos-874",     encoding_name::windows_874 },
			{ U"iso-8859-11", encoding_name::windows_874 },
			{ U"iso8859-11",  encoding_name::windows_874 },
			{ U"iso885911",   encoding_name::windows_874 },
			{ U"tis-620",     encoding_name::windows_874 },
			{ U"windows-874", encoding_name::windows_874 },

			{ U"cp1250",       encoding_name::windows_1250 },
			{ U"windows-1250", encoding_name::windows_1250 },
			{ U"x-cp1250",     encoding_name::windows_1250 },

			{ U"cp1251",       encoding_name::windows_1251 },
			{ U"windows-1251", encoding_name::windows_1251 },
			{ U"x-cp1251",     encoding_name::windows_1251 },

			{ U"ansi_x3.4-1968",  encoding_name::windows_1252 },
			{ U"ascii",           encoding_name::windows_1252 },
			{ U"cp1252",          encoding_name::windows_1252 },
			{ U"cp819",           encoding_name::windows_1252 },
			{ U"csisolatin1",     encoding_name::windows_1252 },
			{ U"ibm819",          encoding_name::windows_1252 },
			{ U"iso-8859-1",      encoding_name::windows_1252 },
			{ U"iso-ir-100",      encoding_name::windows_1252 },
			{ U"iso8859-1",       encoding_name::windows_1252 },
			{ U"iso88591",        encoding_name::windows_1252 },
			{ U"iso_8859-1",      encoding_name::windows_1252 },
			{ U"iso_8859-1:1987", encoding_name::windows_1252 },
			{ U"l1",              encoding_name::windows_1252 },
			{ U"latin1",          encoding_name::windows_1252 },
			{ U"us-ascii",        encoding_name::windows_1252 },
			{ U"windows-1252",    encoding_name::windows_1252 },
			{ U"x-cp1252",        encoding_name::windows_1252 },

			{ U"cp1253",       encoding_name::windows_1253 },
			{ U"windows-1253", encoding_name::windows_1253 },
			{ U"x-cp1253",     encoding_name::windows_1253 },

			{ U"cp1254",          encoding_name::windows_1254 },
			{ U"csisolatin5",     encoding_name::windows_1254 },
			{ U"iso-8859-9",      encoding_name::windows_1254 },
			{ U"iso-ir-148",      encoding_name::windows_1254 },
			{ U"iso8859-9",       encoding_name::windows_1254 },
			{ U"iso88599",        encoding_name::windows_1254 },
			{ U"iso_8859-9",      encoding_name::windows_1254 },
			{ U"iso_8859-9:1989", encoding_name::windows_1254 },
			{ U"l5",              encoding_name::windows_1254 },
			{ U"latin5",          encoding_name::windows_1254 },
			{ U"windows-1254",    encoding_name::windows_1254 },
			{ U"x-cp1254",        encoding_name::windows_1254 },

			{ U"cp1255",       encoding_name::windows_1255 },
			{ U"windows-1255", encoding_name::windows_1255 },
			{ U"x-cp1255",     encoding_name::windows_1255 },

			{ U"cp1256",       encoding_name::windows_1256 },
			{ U"windows-1256", encoding_name::windows_1256 },
			{ U"x-cp1256",     encoding_name::windows_1256 },

			{ U"cp1257",       encoding_name::windows_1257 },
			{ U"windows-1257", encoding_name::windows_1257 },
			{ U"x-cp1257",     encoding_name::windows_1257 },

			{ U"cp1258",       encoding_name::windows_1258 },
			{ U"windows-1258", encoding_name::windows_1258 },
			{ U"x-cp1258",     encoding_name::windows_1258 },

			{ U"x-mac-cyrillic",  encoding_name::x_mac_cyrillic },
			{ U"x-mac-ukrainian", encoding_name::x_mac_cyrillic },

			// Legacy multi-byte Chinese (simplified) encodings
			{ U"chinese",         encoding_name::GBK },
			{ U"csgb2312",        encoding_name::GBK },
			{ U"csiso58gb231280", encoding_name::GBK },
			{ U"gb2312",          encoding_name::GBK },
			{ U"gb_2312",         encoding_name::GBK },
			{ U"gb_2312-80",      encoding_name::GBK },
			{ U"gbk",             encoding_name::GBK },
			{ U"iso-ir-58",       encoding_name::GBK },
			{ U"x-gbk",           encoding_name::GBK },

			{ U"gb18030", encoding_name::gb18030 },

			// Legacy multi-byte Chinese (traditional) encodings
			{ U"big5",       encoding_name::Big5 },
			{ U"big5-hkscs", encoding_name::Big5 },
			{ U"cn-big5",    encoding_name::Big5 },
			{ U"csbig5",     encoding_name::Big5 },
			{ U"x-x-big5",   encoding_name::Big5 },

			// Legacy multi-byte Japanese encodings
			{ U"cseucpkdfmtjapanese", encoding_name::EUC_JP },
			{ U"euc-jp",              encoding_name::EUC_JP },
			{ U"x-euc-jp",            encoding_name::EUC_JP },

			{ U"csiso2022jp", encoding_name::ISO_2022_JP },
			{ U"iso-2022-jp", encoding_name::ISO_2022_JP },

			{ U"csshiftjis",  encoding_name::Shift_JIS },
			{ U"ms932",       encoding_name::Shift_JIS },
			{ U"ms_kanji",    encoding_name::Shift_JIS },
			{ U"shift-jis",   encoding_name::Shift_JIS },
			{ U"shift_jis",   encoding_name::Shift_JIS },
			{ U"sjis",        encoding_name::Shift_JIS },
			{ U"windows-31j", encoding_name::Shift_JIS },
			{ U"x-sjis",      encoding_name::Shift_JIS },

			// Legacy multi-byte Korean encodings
			{ U"cseuckr",        encoding_name::EUC_KR },
			{ U"csksc56011987",  encoding_name::EUC_KR },
			{ U"euc-kr",         encoding_name::EUC_KR },
			{ U"iso-ir-149",     encoding_name::EUC_KR },
			{ U"korean",         encoding_name::EUC_KR },
			{ U"ks_c_5601-1987", encoding_name::EUC_KR },
			{ U"ks_c_5601-1989", encoding_name::EUC_KR },
			{ U"ksc5601",        encoding_name::EUC_KR },
			{ U"ksc_5601",       encoding_name::EUC_KR },
			{ U"windows-949",    encoding_name::EUC_KR },

			// Legacy miscellaneous encodings
			{ U"csiso2022kr",     encoding_name::replacement },
			{ U"hz-gb-2312",      encoding_name::replacement },
			{ U"iso-2022-cn",     encoding_name::replacement },
			{ U"iso-2022-cn-ext", encoding_name::replacement },
			{ U"iso-2022-kr",     encoding_name::replacement },
			{ U"replacement",     encoding_name::replacement },

			{ U"utf-16be", encoding_name::UTF_16BE },

			{ U"utf-16",   encoding_name::UTF_16LE },
			{ U"utf-16le", encoding_name::UTF_16LE },

			{ U"x-user-defined", encoding_name::x_user_defined }
		};
		
		std::u32string label_name{ to_u32string(label) };
		label_name = strip_leading_and_trailing_ascii_whitespace(label_name.begin(), label_name.end());
		label_name = to_ascii_lowercase(label_name.begin(), label_name.end());
		auto it = map.find(label_name);
		return it == map.end() ? encoding_name::None : it->second;
	}

	// 4.3. Output encodings --------------------------------------------------

	encoding_name get_output_encoding(encoding_name encoding)
	{
		switch (encoding)
		{
		case encoding_name::replacement:
		case encoding_name::UTF_16BE:
		case encoding_name::UTF_16LE:
			return encoding_name::UTF_8;
		}
		return encoding;
	}

	// 5. Indexes

}
