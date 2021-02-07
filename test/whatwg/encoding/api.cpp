// test/whatwg/encoding/api.cpp

#include <boost/test/unit_test.hpp>

#include <wordring/whatwg/encoding/api.hpp>

#include <array>
#include <iterator>
#include <string>

#include <wordring/whatwg/encoding/api.hpp>

BOOST_AUTO_TEST_SUITE(coder_api_test)

BOOST_AUTO_TEST_CASE(api_text_decoder_1)
{
	using namespace wordring::whatwg::encoding;

	text_decoder td;
	td.decode();
	BOOST_CHECK(td.encoding() == U"utf-8");
}

BOOST_AUTO_TEST_CASE(api_text_decoder_2)
{
	using namespace wordring::whatwg::encoding;

	std::string in = u8"ABCDE";
	text_decoder td;
	std::u32string out = td.decode(in.begin(), in.end());
	BOOST_CHECK(out == U"ABCDE");
}

BOOST_AUTO_TEST_SUITE_END()
