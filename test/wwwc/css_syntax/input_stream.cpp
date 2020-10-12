// test/wwwc/css_syntax/input_stream.cpp

#include <boost/test/unit_test.hpp>

#include <wordring/wwwc/css_syntax/input_stream.hpp>

#include <algorithm>
#include <iterator>
#include <string>

BOOST_AUTO_TEST_SUITE(css_syntax_input_stream_test)

BOOST_AUTO_TEST_CASE(input_stream_decode_1)
{
	using namespace wordring::wwwc;

	std::string in = "@charset \"shift-jis\";\x82\xA0\x82\xA2\x82\xA4\x82\xA6\x82\xA8";
	std::u32string s = U"@charset \"shift-jis\";あいうえお";
	std::u32string out;

	decode(in.begin(), in.end(), std::back_inserter(out));

	BOOST_CHECK(s == out);
}

BOOST_AUTO_TEST_CASE(input_stream_decode_2)
{
	using namespace wordring::wwwc;

	std::string in = "\xEF\xBB\xBF@charset \"shift-jis\";\xE3\x81\x82\xE3\x81\x84\xE3\x81\x86\xE3\x81\x88\xE3\x81\x8A";
	std::u32string s = U"@charset \"shift-jis\";あいうえお";
	std::u32string out;

	decode(in.begin(), in.end(), std::back_inserter(out));

	BOOST_CHECK(s == out);
}

BOOST_AUTO_TEST_CASE(input_stream_determine_fallback_encoding_1)
{
	using namespace wordring::wwwc;

	std::string in = "@charset \"shift-jis\";";
	std::u32string out;

	wordring::encoding_name enc = determine_fallback_encoding(in.begin(), in.end());

	BOOST_CHECK(enc == wordring::encoding_name::Shift_JIS);
}

BOOST_AUTO_TEST_CASE(input_stream_determine_fallback_encoding_2)
{
	using namespace wordring::wwwc;

	std::string in = "ABCD@charset \"shift-jis\";";
	std::u32string out;

	wordring::encoding_name enc = determine_fallback_encoding(in.begin(), in.end());

	BOOST_CHECK(enc == wordring::encoding_name::UTF_8);
}

BOOST_AUTO_TEST_CASE(input_stream_filter_code_points_1)
{
	using namespace wordring::wwwc;

	std::u32string in{ U'\r', U'\n', U'\r', U'\x0C', U'\x00', U'\xD800', U'A' };
	std::u32string s{ U'\n', U'\n', U'\n', U'\xFFFD', U'\xFFFD', U'A' };

	in.erase(filter_code_points(in.begin(), in.end()), in.end());

	BOOST_CHECK(in == s);
}

BOOST_AUTO_TEST_SUITE_END()
