
#include <boost/test/unit_test.hpp>

#include <iterator>
#include <string>

#include <wordring/whatwg/encoding/coder.hpp>
#include <wordring/whatwg/encoding/encoding.hpp>
#include <wordring/whatwg/encoding/stream.hpp>

BOOST_AUTO_TEST_SUITE(whatwg_encoding__coder_test)

BOOST_AUTO_TEST_CASE(whatwg_encoding__UTF_8_decoder)
{
	namespace e = wordring::whatwg::encoding;

	std::u32string correct{ U"ABCDEあいうえお𠀋𡈽𡌛𡑮𡢽AÀⱥ𐊠" };
	std::string in{ u8"ABCDEあいうえお𠀋𡈽𡌛𡑮𡢽AÀⱥ𐊠" };
	std::u32string out{};

	e::run(e::UTF_8_decoder{}, e::stream<std::string::iterator>{ in.begin(), in.end() }, std::back_inserter(out));
	BOOST_CHECK(out == correct);
}

BOOST_AUTO_TEST_CASE(whatwg_encoding__UTF_8_encoder)
{
	namespace e = wordring::whatwg::encoding;

	std::string correct{ u8"ABCDEあいうえお𠀋𡈽𡌛𡑮𡢽AÀⱥ𐊠" };
	std::u32string in{ U"ABCDEあいうえお𠀋𡈽𡌛𡑮𡢽AÀⱥ𐊠" };
	std::string out{};

	e::run(e::UTF_8_encoder{}, e::stream<std::u32string::iterator>{ in.begin(), in.end() }, std::back_inserter(out));
	BOOST_CHECK(out == correct);
}

BOOST_AUTO_TEST_SUITE_END()
