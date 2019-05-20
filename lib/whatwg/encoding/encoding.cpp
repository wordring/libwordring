// whatwg/encoding/encoding.cpp

#include <string>
#include <unordered_map>

#include <wordring/whatwg/encoding/encoding.hpp>

using namespace wordring::whatwg::encoding;

static std::unordered_map<std::u32string, name> const encoding_label_map {
	// The Encoding
	{ U"unicode-1-1-utf-8", name::UTF_8 },
	{ U"utf-8",             name::UTF_8 },
	{ U"utf8",              name::UTF_8 },

	// Legacy single-byte encodings
	{ U"866",      name::IBM866 },
	{ U"cp866",    name::IBM866 },
	{ U"csibm866", name::IBM866 },
	{ U"ibm866",   name::IBM866 },

	{ U"csisolatin2",     name::ISO_8859_2 },
	{ U"iso-8859-2",      name::ISO_8859_2 },
	{ U"iso-ir-101",      name::ISO_8859_2 },
	{ U"iso8859-2",       name::ISO_8859_2 },
	{ U"iso88592",        name::ISO_8859_2 },
	{ U"iso_8859-2",      name::ISO_8859_2 },
	{ U"iso_8859-2:1987", name::ISO_8859_2 },
	{ U"l2",              name::ISO_8859_2 },
	{ U"latin2",          name::ISO_8859_2 },

	{ U"csisolatin3",     name::ISO_8859_3 },
	{ U"iso-8859-3",      name::ISO_8859_3 },
	{ U"iso-ir-109",      name::ISO_8859_3 },
	{ U"iso8859-3",       name::ISO_8859_3 },
	{ U"iso88593",        name::ISO_8859_3 },
	{ U"iso_8859-3",      name::ISO_8859_3 },
	{ U"iso_8859-3:1988", name::ISO_8859_3 },
	{ U"l3",              name::ISO_8859_3 },
	{ U"latin3",          name::ISO_8859_3 },

	{ U"csisolatin4",     name::ISO_8859_4 },
	{ U"iso-8859-4",      name::ISO_8859_4 },
	{ U"iso-ir-110",      name::ISO_8859_4 },
	{ U"iso8859-4",       name::ISO_8859_4 },
	{ U"iso88594",        name::ISO_8859_4 },
	{ U"iso_8859-4",      name::ISO_8859_4 },
	{ U"iso_8859-4:1988", name::ISO_8859_4 },
	{ U"l4",              name::ISO_8859_4 },
	{ U"latin4",          name::ISO_8859_4 },

	{ U"csisolatincyrillic", name::ISO_8859_5 },
	{ U"cyrillic",           name::ISO_8859_5 },
	{ U"iso-8859-5",         name::ISO_8859_5 },
	{ U"iso-ir-144",         name::ISO_8859_5 },
	{ U"iso8859-5",          name::ISO_8859_5 },
	{ U"iso88595",           name::ISO_8859_5 },
	{ U"iso_8859-5",         name::ISO_8859_5 },
	{ U"iso_8859-5:1988",    name::ISO_8859_5 },

	{ U"arabic",           name::ISO_8859_6 },
	{ U"asmo-708",         name::ISO_8859_6 },
	{ U"csiso88596e",      name::ISO_8859_6 },
	{ U"csiso88596i",      name::ISO_8859_6 },
	{ U"csisolatinarabic", name::ISO_8859_6 },
	{ U"ecma-114",         name::ISO_8859_6 },
	{ U"iso-8859-6",       name::ISO_8859_6 },
	{ U"iso-8859-6-e",     name::ISO_8859_6 },
	{ U"iso-8859-6-i",     name::ISO_8859_6 },
	{ U"iso-ir-127",       name::ISO_8859_6 },
	{ U"iso8859-6",        name::ISO_8859_6 },
	{ U"iso88596",         name::ISO_8859_6 },
	{ U"iso_8859-6",       name::ISO_8859_6 },
	{ U"iso_8859-6:1987",  name::ISO_8859_6 },

	{ U"csisolatingreek", name::ISO_8859_7 },
	{ U"ecma-118",        name::ISO_8859_7 },
	{ U"elot_928",        name::ISO_8859_7 },
	{ U"greek",           name::ISO_8859_7 },
	{ U"greek8",          name::ISO_8859_7 },
	{ U"iso-8859-7",      name::ISO_8859_7 },
	{ U"iso-ir-126",      name::ISO_8859_7 },
	{ U"iso8859-7",       name::ISO_8859_7 },
	{ U"iso88597",        name::ISO_8859_7 },
	{ U"iso_8859-7",      name::ISO_8859_7 },
	{ U"iso_8859-7:1987", name::ISO_8859_7 },
	{ U"sun_eu_greek",    name::ISO_8859_7 },

	{ U"csiso88598e",      name::ISO_8859_8 },
	{ U"csisolatinhebrew", name::ISO_8859_8 },
	{ U"hebrew",           name::ISO_8859_8 },
	{ U"iso-8859-8",       name::ISO_8859_8 },
	{ U"iso-8859-8-e",     name::ISO_8859_8 },
	{ U"iso-ir-138",       name::ISO_8859_8 },
	{ U"iso8859-8",        name::ISO_8859_8 },
	{ U"iso88598",         name::ISO_8859_8 },
	{ U"iso_8859-8",       name::ISO_8859_8 },
	{ U"iso_8859-8:1988",  name::ISO_8859_8 },
	{ U"visual",           name::ISO_8859_8 },

	{ U"csiso88598i",  name::ISO_8859_8_I },
	{ U"iso-8859-8-i", name::ISO_8859_8_I },
	{ U"logical",      name::ISO_8859_8_I },

	{ U"csisolatin6", name::ISO_8859_10 },
	{ U"iso-8859-10", name::ISO_8859_10 },
	{ U"iso-ir-157",  name::ISO_8859_10 },
	{ U"iso8859-10",  name::ISO_8859_10 },
	{ U"iso885910",   name::ISO_8859_10 },
	{ U"l6",          name::ISO_8859_10 },
	{ U"latin6",      name::ISO_8859_10 },

	{ U"iso-8859-13", name::ISO_8859_13 },
	{ U"iso8859-13",  name::ISO_8859_13 },
	{ U"iso885913",   name::ISO_8859_13 },

	{ U"iso-8859-14", name::ISO_8859_14 },
	{ U"iso8859-14",  name::ISO_8859_14 },
	{ U"iso885914",   name::ISO_8859_14 },

	{ U"csisolatin9", name::ISO_8859_15 },
	{ U"iso-8859-15", name::ISO_8859_15 },
	{ U"iso8859-15",  name::ISO_8859_15 },
	{ U"iso885915",   name::ISO_8859_15 },
	{ U"iso_8859-15", name::ISO_8859_15 },
	{ U"l9",          name::ISO_8859_15 },

	{ U"iso-8859-16", name::ISO_8859_16 },

	{ U"cskoi8r", name::KOI8_R },
	{ U"koi",     name::KOI8_R },
	{ U"koi8",    name::KOI8_R },
	{ U"koi8-r",  name::KOI8_R },
	{ U"koi8_r",  name::KOI8_R },

	{ U"koi8-ru", name::KOI8_U },
	{ U"koi8-u",  name::KOI8_U },

	{ U"csmacintosh", name::macintosh },
	{ U"mac",         name::macintosh },
	{ U"macintosh",   name::macintosh },
	{ U"x-mac-roman", name::macintosh },

	{ U"dos-874",     name::windows_874 },
	{ U"iso-8859-11", name::windows_874 },
	{ U"iso8859-11",  name::windows_874 },
	{ U"iso885911",   name::windows_874 },
	{ U"tis-620",     name::windows_874 },
	{ U"windows-874", name::windows_874 },

	{ U"cp1250",       name::windows_1250 },
	{ U"windows-1250", name::windows_1250 },
	{ U"x-cp1250",     name::windows_1250 },

	{ U"cp1251",       name::windows_1251 },
	{ U"windows-1251", name::windows_1251 },
	{ U"x-cp1251",     name::windows_1251 },

	{ U"ansi_x3.4-1968",  name::windows_1252 },
	{ U"ascii",           name::windows_1252 },
	{ U"cp1252",          name::windows_1252 },
	{ U"cp819",           name::windows_1252 },
	{ U"csisolatin1",     name::windows_1252 },
	{ U"ibm819",          name::windows_1252 },
	{ U"iso-8859-1",      name::windows_1252 },
	{ U"iso-ir-100",      name::windows_1252 },
	{ U"iso8859-1",       name::windows_1252 },
	{ U"iso88591",        name::windows_1252 },
	{ U"iso_8859-1",      name::windows_1252 },
	{ U"iso_8859-1:1987", name::windows_1252 },
	{ U"l1",              name::windows_1252 },
	{ U"latin1",          name::windows_1252 },
	{ U"us-ascii",        name::windows_1252 },
	{ U"windows-1252",    name::windows_1252 },
	{ U"x-cp1252",        name::windows_1252 },

	{ U"cp1253",       name::windows_1253 },
	{ U"windows-1253", name::windows_1253 },
	{ U"x-cp1253",     name::windows_1253 },

	{ U"cp1254",          name::windows_1254 },
	{ U"csisolatin5",     name::windows_1254 },
	{ U"iso-8859-9",      name::windows_1254 },
	{ U"iso-ir-148",      name::windows_1254 },
	{ U"iso8859-9",       name::windows_1254 },
	{ U"iso88599",        name::windows_1254 },
	{ U"iso_8859-9",      name::windows_1254 },
	{ U"iso_8859-9:1989", name::windows_1254 },
	{ U"l5",              name::windows_1254 },
	{ U"latin5",          name::windows_1254 },
	{ U"windows-1254",    name::windows_1254 },
	{ U"x-cp1254",        name::windows_1254 },

	{ U"cp1255",       name::windows_1255 },
	{ U"windows-1255", name::windows_1255 },
	{ U"x-cp1255",     name::windows_1255 },

	{ U"cp1256",       name::windows_1256 },
	{ U"windows-1256", name::windows_1256 },
	{ U"x-cp1256",     name::windows_1256 },

	{ U"cp1257",       name::windows_1257 },
	{ U"windows-1257", name::windows_1257 },
	{ U"x-cp1257",     name::windows_1257 },

	{ U"cp1258",       name::windows_1258 },
	{ U"windows-1258", name::windows_1258 },
	{ U"x-cp1258",     name::windows_1258 },

	{ U"x-mac-cyrillic",  name::x_mac_cyrillic },
	{ U"x-mac-ukrainian", name::x_mac_cyrillic },

	// Legacy multi-byte Chinese (simplified) encodings
	{ U"chinese",         name::GBK },
	{ U"csgb2312",        name::GBK },
	{ U"csiso58gb231280", name::GBK },
	{ U"gb2312",          name::GBK },
	{ U"gb_2312",         name::GBK },
	{ U"gb_2312-80",      name::GBK },
	{ U"gbk",             name::GBK },
	{ U"iso-ir-58",       name::GBK },
	{ U"x-gbk",           name::GBK },

	{ U"gb18030", name::gb18030 },

	// Legacy multi-byte Chinese (traditional) encodings
	{ U"big5",       name::Big5 },
	{ U"big5-hkscs", name::Big5 },
	{ U"cn-big5",    name::Big5 },
	{ U"csbig5",     name::Big5 },
	{ U"x-x-big5",   name::Big5 },

	// Legacy multi-byte Japanese encodings
	{ U"cseucpkdfmtjapanese", name::EUC_JP },
	{ U"euc-jp",              name::EUC_JP },
	{ U"x-euc-jp",            name::EUC_JP },

	{ U"csiso2022jp", name::ISO_2022_JP },
	{ U"iso-2022-jp", name::ISO_2022_JP },

	{ U"csshiftjis",  name::Shift_JIS },
	{ U"ms932",       name::Shift_JIS },
	{ U"ms_kanji",    name::Shift_JIS },
	{ U"shift-jis",   name::Shift_JIS },
	{ U"shift_jis",   name::Shift_JIS },
	{ U"sjis",        name::Shift_JIS },
	{ U"windows-31j", name::Shift_JIS },
	{ U"x-sjis",      name::Shift_JIS },

	// Legacy multi-byte Korean encodings
	{ U"cseuckr",        name::EUC_KR },
	{ U"csksc56011987",  name::EUC_KR },
	{ U"euc-kr",         name::EUC_KR },
	{ U"iso-ir-149",     name::EUC_KR },
	{ U"korean",         name::EUC_KR },
	{ U"ks_c_5601-1987", name::EUC_KR },
	{ U"ks_c_5601-1989", name::EUC_KR },
	{ U"ksc5601",        name::EUC_KR },
	{ U"ksc_5601",       name::EUC_KR },
	{ U"windows-949",    name::EUC_KR },

	// Legacy miscellaneous encodings
	{ U"csiso2022kr",     name::replacement },
	{ U"hz-gb-2312",      name::replacement },
	{ U"iso-2022-cn",     name::replacement },
	{ U"iso-2022-cn-ext", name::replacement },
	{ U"iso-2022-kr",     name::replacement },
	{ U"replacement",     name::replacement },

	{ U"utf-16be", name::UTF_16BE },

	{ U"utf-16",   name::UTF_16LE },
	{ U"utf-16le", name::UTF_16LE },

	{ U"x-user-defined", name::x_user_defined }
};

name wordring::whatwg::encoding::get_name(std::u32string_view label)
{
	using namespace wordring::whatwg;

	std::u32string label_name{ label };
	label_name = strip_leading_and_trailing_ascii_whitespace(label_name.begin(), label_name.end());
	label_name = to_ascii_lowercase(label_name.begin(), label_name.end());
	auto it = encoding_label_map.find(label_name);
	return it == encoding_label_map.end() ? name{ 0 } : it->second;
}

std::string_view BOM_UTF_8{ "\xEF\xBB\xBF" };
std::string_view BOM_UTF_16BE{ "\xFE\xFF" };
std::string_view BOM_UTF_16LE{ "\xFF\xFE" };
