// test/whatwg/encoding/coder.cpp

#include <boost/test/unit_test.hpp>

#include <wordring/whatwg/encoding/coder.hpp>
#include <wordring/whatwg/encoding/encoding.hpp>
#include <wordring/whatwg/encoding/terminology.hpp>

#include <array>
#include <iterator>
#include <string>

namespace
{
	// omit はテストデータに含まれない文字の集合
	bool cyclic_test(wordring::whatwg::encoding::encoding_name name, std::string omit = "")
	{
		// 循環テスト　※本来はテスト値を用意したい
		using namespace wordring::whatwg::encoding;

		// テストデータ作成
		io_queue<char> in_q;
		for (std::uint16_t i = 0; i <= 0xFFu; i++)
		{
			if (omit.find(static_cast<std::uint8_t>(i)) != std::string::npos) continue;
			in_q.push(static_cast<std::uint8_t>(i));
		}
		in_q.push(io_item<char>{ '\0', true });

		// 比較用に入力を文字列化しておく
		io_queue<char> in_tmp_q = in_q;
		std::string s1;
		from_io_queue_convert(in_tmp_q, std::back_inserter(s1));

		// デコード
		io_queue<char32_t> tmp_q;
		auto rv1 = decode(in_q, name, tmp_q);
		BOOST_CHECK(std::holds_alternative<result_finished>(rv1));

		// エンコード
		io_queue<char> out_q;
		auto rv2 = encode(tmp_q, name, out_q);
		BOOST_CHECK(std::holds_alternative<result_finished>(rv2));

		// 比較用に出力を文字列化する
		std::string s2;
		from_io_queue_convert(out_q, std::back_inserter(s2));

		return s1 == s2;
	}

	// data はテストデータ
	bool cyclic_test(wordring::whatwg::encoding::encoding_name name, std::u32string data)
	{
		using namespace wordring::whatwg::encoding;

		io_queue<char32_t> in_q = to_io_queue_convert(data.begin(), data.end());
		in_q.push(io_item<char32_t>{ U'\0', true });

		// 比較用に入力を文字列化しておく
		io_queue<char32_t> in_tmp_q = in_q;
		std::u32string s1;
		from_io_queue_convert(in_tmp_q, std::back_inserter(s1));

		// エンコード
		io_queue<char> tmp_q;
		auto rv1 = encode(in_q, name, tmp_q);
		BOOST_CHECK(std::holds_alternative<result_finished>(rv1));

		// デコード
		io_queue<char32_t> out_q;
		auto rv2 = decode(tmp_q, name, out_q);
		BOOST_CHECK(std::holds_alternative<result_finished>(rv2));

		// 比較用に出力を文字列化する
		std::u32string s2;
		from_io_queue_convert(out_q, std::back_inserter(s2));

		return s1 == s2;
	}
}

BOOST_AUTO_TEST_SUITE(coder_test)

BOOST_AUTO_TEST_CASE(coder_UTF_8_decoder_1)
{
	using namespace wordring::whatwg::encoding;

	// 1-4バイト迄の文字を一文字ずつ並べた。
	std::string_view sv = "\x41\xC3\x80\xE3\x81\x82\xF0\x90\x8A\x80";
	io_queue<char> in = to_io_queue_convert(sv.begin(), sv.end());

	io_queue<char32_t> out;
	auto rv = decode(in, encoding_name::UTF_8, out);
	BOOST_CHECK(std::holds_alternative<result_finished>(rv));

	std::u32string s;
	from_io_queue_convert(out, std::back_inserter(s));
	BOOST_CHECK(s == U"\U00000041\U000000C0\U00003042\U00010280");
}

BOOST_AUTO_TEST_CASE(coder_UTF_8_encoder_1)
{
	using namespace wordring::whatwg::encoding;

	// 1-4バイト迄の文字を一文字ずつ並べた。
	std::u32string_view sv = U"\U00000041\U000000C0\U00003042\U00010280";
	io_queue<char32_t> in = to_io_queue_convert(sv.begin(), sv.end());

	io_queue<char32_t> out;
	auto rv = encode(in, encoding_name::UTF_8, out);
	BOOST_CHECK(std::holds_alternative<result_finished>(rv));

	std::string s;
	from_io_queue_convert(out, std::back_inserter(s));
	BOOST_CHECK(s == "\x41\xC3\x80\xE3\x81\x82\xF0\x90\x8A\x80");
}

BOOST_AUTO_TEST_CASE(coder_windows_1252_decoder_1)
{
	using namespace wordring::whatwg::encoding;

	std::string_view sv = "A\x80\xFF";
	io_queue<char> in = to_io_queue_convert(sv.begin(), sv.end());

	io_queue<char32_t> out;
	auto rv = decode(in, encoding_name::windows_1252, out);
	BOOST_CHECK(std::holds_alternative<result_finished>(rv));

	std::u32string s;
	from_io_queue_convert(out, std::back_inserter(s));
	BOOST_CHECK(s == U"A€ÿ");
}

BOOST_AUTO_TEST_CASE(coder_windows_1252_encoder_1)
{
	using namespace wordring::whatwg::encoding;

	std::u32string_view sv = U"A€ÿ";
	io_queue<char32_t> in = to_io_queue_convert(sv.begin(), sv.end());

	io_queue<char32_t> out;
	auto rv = encode(in, encoding_name::windows_1252, out);
	BOOST_CHECK(std::holds_alternative<result_finished>(rv));

	std::string s;
	from_io_queue_convert(out, std::back_inserter(s));
	BOOST_CHECK(s == "A\x80\xFF");
}

BOOST_AUTO_TEST_CASE(coder_IBM866_coder_1)
{
	using namespace wordring::whatwg::encoding;
	BOOST_CHECK(cyclic_test(encoding_name::IBM866));
}

BOOST_AUTO_TEST_CASE(coder_ISO_8859_2_coder_1)
{
	using namespace wordring::whatwg::encoding;
	BOOST_CHECK(cyclic_test(encoding_name::ISO_8859_2));
}

BOOST_AUTO_TEST_CASE(coder_ISO_8859_3_coder_1)
{
	using namespace wordring::whatwg::encoding;
	std::string omit = "\xA5\xAE\xBE\xC3\xD0\xE3\xF0"; // 0x80 + [ 37, 46, 62, 67, 80, 99, 112 ]
	BOOST_CHECK(cyclic_test(encoding_name::ISO_8859_3, omit));
}

BOOST_AUTO_TEST_CASE(coder_ISO_8859_4_coder_1)
{
	using namespace wordring::whatwg::encoding;
	BOOST_CHECK(cyclic_test(encoding_name::ISO_8859_4));
}

BOOST_AUTO_TEST_CASE(coder_ISO_8859_5_coder_1)
{
	using namespace wordring::whatwg::encoding;
	BOOST_CHECK(cyclic_test(encoding_name::ISO_8859_5));
}

BOOST_AUTO_TEST_CASE(coder_ISO_8859_6_coder_1)
{
	using namespace wordring::whatwg::encoding;
	std::string omit =
		"\xA1\xA2\xA3\xA5\xA6\xA7\xA8\xA9\xAA\xAB\xAE\xAF\xB0\xB1\xB2\xB3\xB4\xB5\xB6\xB7\xB8\xB9"
		"\xBA\xBC\xBD\xBE\xC0\xDB\xDC\xDD\xDE\xDF\xF3\xF4\xF5\xF6\xF7\xF8\xF9\xFA\xFB\xFC\xFD\xFE"
		"\xFF";
	BOOST_CHECK(cyclic_test(encoding_name::ISO_8859_6, omit));
}

BOOST_AUTO_TEST_CASE(coder_ISO_8859_7_coder_1)
{
	using namespace wordring::whatwg::encoding;
	std::string omit = "\xAE\xD2\xFF";
	BOOST_CHECK(cyclic_test(encoding_name::ISO_8859_7, omit));
}

BOOST_AUTO_TEST_CASE(coder_ISO_8859_8_coder_1)
{
	using namespace wordring::whatwg::encoding;
	std::string omit = 
		"\xA1\xBF\xC0\xC1\xC2\xC3\xC4\xC5\xC6\xC7\xC8\xC9\xCA\xCB\xCC\xCD\xCE\xCF\xD0\xD1\xD2\xD3"
		"\xD4\xD5\xD6\xD7\xD8\xD9\xDA\xDB\xDC\xDD\xDE\xFB\xFC\xFF\x80";
	BOOST_CHECK(cyclic_test(encoding_name::ISO_8859_8, omit));
}

BOOST_AUTO_TEST_CASE(coder_ISO_8859_10_coder_1)
{
	using namespace wordring::whatwg::encoding;
	BOOST_CHECK(cyclic_test(encoding_name::ISO_8859_10));
}

BOOST_AUTO_TEST_CASE(coder_ISO_8859_13_coder_1)
{
	using namespace wordring::whatwg::encoding;
	BOOST_CHECK(cyclic_test(encoding_name::ISO_8859_13));
}

BOOST_AUTO_TEST_CASE(coder_ISO_8859_14_coder_1)
{
	using namespace wordring::whatwg::encoding;
	BOOST_CHECK(cyclic_test(encoding_name::ISO_8859_14));
}

BOOST_AUTO_TEST_CASE(coder_ISO_8859_15_coder_1)
{
	using namespace wordring::whatwg::encoding;
	BOOST_CHECK(cyclic_test(encoding_name::ISO_8859_15));
}

BOOST_AUTO_TEST_CASE(coder_ISO_8859_16_coder_1)
{
	using namespace wordring::whatwg::encoding;
	BOOST_CHECK(cyclic_test(encoding_name::ISO_8859_16));
}

BOOST_AUTO_TEST_CASE(coder_KOI8_R_coder_1)
{
	using namespace wordring::whatwg::encoding;
	BOOST_CHECK(cyclic_test(encoding_name::KOI8_R));
}

BOOST_AUTO_TEST_CASE(coder_KOI8_U_coder_1)
{
	using namespace wordring::whatwg::encoding;
	BOOST_CHECK(cyclic_test(encoding_name::KOI8_U));
}

BOOST_AUTO_TEST_CASE(coder_macintosh_coder_1)
{
	using namespace wordring::whatwg::encoding;
	BOOST_CHECK(cyclic_test(encoding_name::macintosh));
}

BOOST_AUTO_TEST_CASE(coder_windows_874_coder_1)
{
	using namespace wordring::whatwg::encoding;
	std::string omit = "\xDB\xDC\xDD\xDE\xFC\xFD\xFE\xFF";
	BOOST_CHECK(cyclic_test(encoding_name::windows_874, omit));
}

BOOST_AUTO_TEST_CASE(coder_windows_1250_coder_1)
{
	using namespace wordring::whatwg::encoding;
	BOOST_CHECK(cyclic_test(encoding_name::windows_1250));
}

BOOST_AUTO_TEST_CASE(coder_windows_1251_coder_1)
{
	using namespace wordring::whatwg::encoding;
	BOOST_CHECK(cyclic_test(encoding_name::windows_1251));
}

BOOST_AUTO_TEST_CASE(coder_windows_1252_coder_1)
{
	using namespace wordring::whatwg::encoding;
	BOOST_CHECK(cyclic_test(encoding_name::windows_1252));
}

BOOST_AUTO_TEST_CASE(coder_windows_1253_coder_1)
{
	using namespace wordring::whatwg::encoding;
	std::string omit = "\xAA\xD2\xFF";
	BOOST_CHECK(cyclic_test(encoding_name::windows_1253, omit));
}

BOOST_AUTO_TEST_CASE(coder_windows_1254_coder_1)
{
	using namespace wordring::whatwg::encoding;
	BOOST_CHECK(cyclic_test(encoding_name::windows_1254));
}

BOOST_AUTO_TEST_CASE(coder_windows_1255_coder_1)
{
	using namespace wordring::whatwg::encoding;
	std::string omit = "\xD9\xDA\xDB\xDC\xDD\xDE\xDF\xFB\xFC\xFF";
	BOOST_CHECK(cyclic_test(encoding_name::windows_1255, omit));
}

BOOST_AUTO_TEST_CASE(coder_windows_1256_coder_1)
{
	using namespace wordring::whatwg::encoding;
	BOOST_CHECK(cyclic_test(encoding_name::windows_1256));
}

BOOST_AUTO_TEST_CASE(coder_windows_1257_coder_1)
{
	using namespace wordring::whatwg::encoding;
	std::string omit = "\xA1\xA5";
	BOOST_CHECK(cyclic_test(encoding_name::windows_1257, omit));
}

BOOST_AUTO_TEST_CASE(coder_windows_1258_coder_1)
{
	using namespace wordring::whatwg::encoding;
	BOOST_CHECK(cyclic_test(encoding_name::windows_1258));
}

BOOST_AUTO_TEST_CASE(coder_x_mac_cyrillic_coder_1)
{
	using namespace wordring::whatwg::encoding;
	BOOST_CHECK(cyclic_test(encoding_name::x_mac_cyrillic));
}

BOOST_AUTO_TEST_CASE(coder_gb18030_coder_1)
{
	using namespace wordring::whatwg::encoding;

	// テストデータ
	std::u32string sv =
		U"成我戒戓戔戕或戗战戙戚戛戜戝戞戟abcdefghijklmno㐀㐁㐂㐃㐄㐅㐆㐇㐈㐉㐊㐋㐌㐍㐎㐏€亐亖亗亙亜亝"
		"亞⺁⺄⺈⺋⺌⺗⺧⺪⺮⺳⺶⺷⺻⻊㖞㘚㘎⺗㥮㤘㧏㧟㩳㧐㭎㱮㳠郎凉秊裏隣兀嗀﨎﨏ⅠⅡⅢⅣⅤⅰⅱⅲⅳⅴ"
		"ᠠᠡᠢᠣᠤᠥᠦᠧᠨᠩꀇꀈꀉꀊꀋꀌꀍꀎꀏ༐༑༒༓༔༕༖༗༘༙بةتثجحخد狢狣狤狥狦狧狪狫㊣通桐酮瞳同铜彤童﹔﹕﹖﹗";
	BOOST_CHECK(cyclic_test(encoding_name::gb18030, sv));
}

BOOST_AUTO_TEST_CASE(coder_Big5_coder_1)
{
	using namespace wordring::whatwg::encoding;

	// テストデータ
	std::u32string s =
		U"·＼ｗ一世共作杓咖昇陂拯耐哦浬虔娼毫莆婷溉詔媳睹辟愿罰劇瑾輥濃錐瞧駿鞭願護讖すЛ乂汌杙坨泒哃柜"
		"穾唊毨笄酎崰淐耞釫惲湨罦軹媷毹稛觡凘榠禗裰噚澍膞踔噳澢蕀錋檕蕷鞞璸蹛徿譑嚵鏼蠩糴讌纘";
	BOOST_CHECK(cyclic_test(encoding_name::Big5, s));
}

BOOST_AUTO_TEST_CASE(coder_EUC_JP_coder_1)
{
	using namespace wordring::whatwg::encoding;

	// テストデータ
	std::u32string s =
		U"｡｢｣､･ｦｧｨｩｪｫｬｭｮｯｰｱｲｳｴｵｶｷｸｹｺｻｼｽｾｿﾀﾁﾂﾃﾄﾅﾆﾇﾈﾉﾊﾋﾌﾍﾎﾏﾐﾑﾒﾓﾔﾕﾖﾗﾘﾙﾚﾛﾜﾝﾞﾟ0123456789ぁあぃいぅうぇ"
		"えぉお";
	BOOST_CHECK(cyclic_test(encoding_name::EUC_JP, s));
}

BOOST_AUTO_TEST_CASE(coder_ISO_2022_JP_coder_1)
{
	using namespace wordring::whatwg::encoding;

	// テストデータ
	std::u32string s = U"ABCDEｱｲｳｴｵあいうえお漢字";
	io_queue<char32_t> in_q = to_io_queue_convert(s.begin(), s.end());
	in_q.push(io_item<char32_t>{ U'\0', true });

	// エンコード
	io_queue<char> tmp_q;
	auto rv1 = encode(in_q, encoding_name::ISO_2022_JP, tmp_q);
	BOOST_CHECK(std::holds_alternative<result_finished>(rv1));

	// デコード
	io_queue<char32_t> out_q;
	auto rv2 = decode(tmp_q, encoding_name::ISO_2022_JP, out_q);
	BOOST_CHECK(std::holds_alternative<result_finished>(rv2));

	// 比較用に出力を文字列化する
	std::u32string s1;
	from_io_queue_convert(out_q, std::back_inserter(s1));

	// 半角カナは全角に変わる。
	std::u32string s2 = U"ABCDEアイウエオあいうえお漢字";
	BOOST_CHECK(s1 == s2);
}

BOOST_AUTO_TEST_CASE(coder_Shift_JIS_coder_1)
{
	using namespace wordring::whatwg::encoding;

	// テストデータ
	std::u32string s = U"ABCDEｱｲｳｴｵアイウエオ漾漓滷澆潺纊褜鍈銈蓜";
	BOOST_CHECK(cyclic_test(encoding_name::Shift_JIS, s));
}

BOOST_AUTO_TEST_CASE(coder_EUC_KR_coder_1)
{
	using namespace wordring::whatwg::encoding;

	// テストデータ
	std::u32string s =
		U"!、⇒！ㄱⅰ─㎕ÆæぁァА가괌깹끝뇟덧땀래륫묀벙빨샥숯쐴에웩점징찼치큄퉤퐈혤伽匣瞼棨科區鬼朞納丹棹"
		"蘿煉遼立蔑汶發碧孚脾傘胥聖戍嵩沈櫻旅簾烏窈運濡議立障煎靜踪咫鏶責椒贅鐸阪品行形禍爻";
	BOOST_CHECK(cyclic_test(encoding_name::EUC_KR, s));
}

BOOST_AUTO_TEST_CASE(coder_replacement_coder_1)
{
	// 一度だけエラーを返し、その後は result_finished{} を返し続けるデコーダ
	using namespace wordring::whatwg::encoding;

	// テストデータ
	std::string s = "ABCDE";
	io_queue<char> in_q = to_io_queue_convert(s.begin(), s.end());
	in_q.push(io_item<char>{ '\0', true });

	// デコード
	io_queue<char32_t> out_q;
	auto rv = decode(in_q, encoding_name::replacement, out_q);
	BOOST_CHECK(std::holds_alternative<result_finished>(rv));
}

BOOST_AUTO_TEST_CASE(coder_replacement_coder_2)
{
	// 一度だけエラーを返し、その後は result_finished{} を返し続けるデコーダ
	using namespace wordring::whatwg::encoding;

	// テストデータ
	io_queue<char> in_q;
	in_q.push(io_item<char>{ 'A', false });

	// デコード
	replacement_decoder c;
	result_value rv = c.run(in_q, in_q.read().m_value);
	BOOST_CHECK(std::holds_alternative<result_error>(rv));
}

BOOST_AUTO_TEST_CASE(coder_UTF_16BE_decoder_1)
{
	// エンコーダは無い
	using namespace wordring::whatwg::encoding;

	// テストデータ
	std::string s = {
		'\x00', '\x21', '\x10', '\x00', '\x20', '\x10', '\x30', '\x01', '\x40', '\x00', '\x50',
		'\x00', '\x60', '\x00', '\x70', '\x00', '\x80', '\x00', '\x90', '\x00', '\xA0', '\x00',
		'\xB0', '\x00', '\xC0', '\x00', '\xD0', '\x00', '\xD8', '\x0C', '\xDC', '\x00', '\xD8',
		'\x35', '\xDC', '\x00', '\xD8', '\x3C', '\xDC', '\x00', '\xD8', '\x40', '\xDC', '\x00',
		'\xD8', '\x44', '\xDC', '\x00', '\xD8', '\x48', '\xDC', '\x00', '\xD8', '\x4C', '\xDC',
		'\x00', '\xD8', '\x50', '\xDC', '\x00', '\xD8', '\x54', '\xDC', '\x00', '\xD8', '\x58',
		'\xDC', '\x00', '\xD8', '\x5C', '\xDC', '\x00', '\xD8', '\x60', '\xDC', '\x00', '\xD8',
		'\x64', '\xDC', '\x00', '\xD8', '\x68', '\xDC', '\x00', '\xD8', '\x6C', '\xDC', '\x00',
		'\xD8', '\x70', '\xDC', '\x29', '\xD8', '\x42', '\xDF', '\xB7' };

	io_queue<char> in_q = to_io_queue_convert(s.begin(), s.end());
	in_q.push(io_item<char>{ '\0', true });

	// デコード
	io_queue<char32_t> out_q;
	auto rv2 = decode(in_q, encoding_name::UTF_16BE, out_q);
	BOOST_CHECK(std::holds_alternative<result_finished>(rv2));

	// 比較用に出力を文字列化する
	std::u32string s1;
	from_io_queue_convert(out_q, std::back_inserter(s1));

	std::u32string s2 =
		U"\U00000021\U00001000\U00002010\U00003001\U00004000\U00005000\U00006000\U00007000\U00008000"
		"\U00009000\U0000A000\U0000B000\U0000C000\U0000D000\U00013000\U0001D400\U0001F000\U00020000"
		"\U00021000\U00022000\U00023000\U00024000\U00025000\U00026000\U00027000\U00028000\U00029000"
		"\U0002A000\U0002B000\U0002C029\U00020BB7";

	BOOST_CHECK(s1 == s2);
}

BOOST_AUTO_TEST_CASE(coder_UTF_16LE_decoder_1)
{
	// エンコーダは無い
	using namespace wordring::whatwg::encoding;

	// テストデータ
	std::string s = {
		'\x21', '\x00', '\x00', '\x10', '\x10', '\x20', '\x01', '\x30', '\x00', '\x40', '\x00',
		'\x50', '\x00', '\x60', '\x00', '\x70', '\x00', '\x80', '\x00', '\x90', '\x00', '\xA0',
		'\x00', '\xB0', '\x00', '\xC0', '\x00', '\xD0', '\x0C', '\xD8', '\x00', '\xDC', '\x35',
		'\xD8', '\x00', '\xDC', '\x3C', '\xD8', '\x00', '\xDC', '\x40', '\xD8', '\x00', '\xDC',
		'\x44', '\xD8', '\x00', '\xDC', '\x48', '\xD8', '\x00', '\xDC', '\x4C', '\xD8', '\x00',
		'\xDC', '\x50', '\xD8', '\x00', '\xDC', '\x54', '\xD8', '\x00', '\xDC', '\x58', '\xD8',
		'\x00', '\xDC', '\x5C', '\xD8', '\x00', '\xDC', '\x60', '\xD8', '\x00', '\xDC', '\x64',
		'\xD8', '\x00', '\xDC', '\x68', '\xD8', '\x00', '\xDC', '\x6C', '\xD8', '\x00', '\xDC',
		'\x70', '\xD8', '\x29', '\xDC', '\x42', '\xD8', '\xB7', '\xDF' };

	io_queue<char> in_q = to_io_queue_convert(s.begin(), s.end());
	in_q.push(io_item<char>{ '\0', true });

	// デコード
	io_queue<char32_t> out_q;
	auto rv2 = decode(in_q, encoding_name::UTF_16LE, out_q);
	BOOST_CHECK(std::holds_alternative<result_finished>(rv2));

	// 比較用に出力を文字列化する
	std::u32string s1;
	from_io_queue_convert(out_q, std::back_inserter(s1));

	std::u32string s2 =
		U"\U00000021\U00001000\U00002010\U00003001\U00004000\U00005000\U00006000\U00007000\U00008000"
		"\U00009000\U0000A000\U0000B000\U0000C000\U0000D000\U00013000\U0001D400\U0001F000\U00020000"
		"\U00021000\U00022000\U00023000\U00024000\U00025000\U00026000\U00027000\U00028000\U00029000"
		"\U0002A000\U0002B000\U0002C029\U00020BB7";

	BOOST_CHECK(s1 == s2);
}

BOOST_AUTO_TEST_CASE(coder_x_user_defined_coder_1)
{
	using namespace wordring::whatwg::encoding;

	// テストデータ
	std::u32string s = U"\x00\xFF";
	BOOST_CHECK(cyclic_test(encoding_name::x_user_defined, s));
}

BOOST_AUTO_TEST_SUITE_END()
