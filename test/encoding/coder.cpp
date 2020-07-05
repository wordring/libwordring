// test/html/coder.cpp

#include <boost/test/unit_test.hpp>

#include <wordring/encoding/coder.hpp>

#include <wordring/compatibility.hpp>

#include <iterator>
#include <string>

BOOST_AUTO_TEST_SUITE(encoding_coder_test)

BOOST_AUTO_TEST_CASE(corder_decoder_1)
{
	using namespace wordring;
	using namespace wordring::encoding;

	std::u8string const in = u8R"*(ABCDEあいうえお𠀋𡈽𡌛𡑮𡢽AÀⱥ𐊠)*";
	std::u32string const s = UR"*(ABCDEあいうえお𠀋𡈽𡌛𡑮𡢽AÀⱥ𐊠)*";
	std::u32string out;

	auto dec = decoder(encoding_name::UTF_8);
	for (auto c : in) dec.push(c, std::back_inserter(out));

	BOOST_CHECK(out == s);
}

BOOST_AUTO_TEST_CASE(corder_encoder_1)
{
	using namespace wordring;
	using namespace wordring::encoding;

	std::u32string const in = UR"*(ABCDEあいうえお𠀋𡈽𡌛𡑮𡢽AÀⱥ𐊠)*";
	std::u8string const s = u8R"*(ABCDEあいうえお𠀋𡈽𡌛𡑮𡢽AÀⱥ𐊠)*";
	std::u8string out;

	auto enc = encoder(encoding_name::UTF_8);
	for (auto c : in) enc.push(c, std::back_inserter(out));

	BOOST_CHECK(out == s);
}

BOOST_AUTO_TEST_SUITE_END()
