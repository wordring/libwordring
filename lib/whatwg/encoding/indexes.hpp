#pragma once
// wordring_cpp/generator/whatwg/encoding/encoding.cpp によって作成された。

#include <array>
#include <unordered_map>

namespace wordring::whatwg::encoding
{
	extern std::array<uint32_t, 19782> const index_code_point_big5;
	extern std::array<uint32_t, 23940> const index_code_point_euc_kr;
	extern std::array<uint32_t, 23940> const index_code_point_gb18030;
	extern std::array<uint32_t, 11280> const index_code_point_jis0208;
	extern std::array<uint32_t, 8836> const index_code_point_jis0212;
	extern std::array<uint32_t, 128> const index_code_point_ibm866;
	extern std::array<uint32_t, 63> const index_code_point_iso_2022_jp_katakana;
	extern std::array<uint32_t, 128> const index_code_point_iso_8859_2;
	extern std::array<uint32_t, 128> const index_code_point_iso_8859_3;
	extern std::array<uint32_t, 128> const index_code_point_iso_8859_4;
	extern std::array<uint32_t, 128> const index_code_point_iso_8859_5;
	extern std::array<uint32_t, 128> const index_code_point_iso_8859_6;
	extern std::array<uint32_t, 128> const index_code_point_iso_8859_7;
	extern std::array<uint32_t, 128> const index_code_point_iso_8859_8;
	extern std::array<uint32_t, 128> const index_code_point_iso_8859_10;
	extern std::array<uint32_t, 128> const index_code_point_iso_8859_13;
	extern std::array<uint32_t, 128> const index_code_point_iso_8859_14;
	extern std::array<uint32_t, 128> const index_code_point_iso_8859_15;
	extern std::array<uint32_t, 128> const index_code_point_iso_8859_16;
	extern std::array<uint32_t, 128> const index_code_point_koi8_r;
	extern std::array<uint32_t, 128> const index_code_point_koi8_u;
	extern std::array<uint32_t, 128> const index_code_point_macintosh;
	extern std::array<uint32_t, 128> const index_code_point_windows_874;
	extern std::array<uint32_t, 128> const index_code_point_windows_1250;
	extern std::array<uint32_t, 128> const index_code_point_windows_1251;
	extern std::array<uint32_t, 128> const index_code_point_windows_1252;
	extern std::array<uint32_t, 128> const index_code_point_windows_1253;
	extern std::array<uint32_t, 128> const index_code_point_windows_1254;
	extern std::array<uint32_t, 128> const index_code_point_windows_1255;
	extern std::array<uint32_t, 128> const index_code_point_windows_1256;
	extern std::array<uint32_t, 128> const index_code_point_windows_1257;
	extern std::array<uint32_t, 128> const index_code_point_windows_1258;
	extern std::array<uint32_t, 128> const index_code_point_x_mac_cyrillic;

	extern std::array<std::pair<uint32_t, uint32_t>, 207> const index_code_point_gb18030_ranges;

}

