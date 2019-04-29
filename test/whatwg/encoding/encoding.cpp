
#include <boost/test/unit_test.hpp>

#include <wordring/whatwg/encoding/encoding.hpp>
#include <wordring/whatwg/encoding/encoding_stream.hpp>

BOOST_AUTO_TEST_SUITE(whatwg_encoding_test)

BOOST_AUTO_TEST_CASE(whatwg_encoding__get_encoding_name)
{
	BOOST_CHECK(wordring::whatwg::get_encoding_name(std::u32string_view{ U"  Unicode-1-1-utf-8  " }) == wordring::whatwg::encoding_name::UTF_8);
}

BOOST_AUTO_TEST_SUITE_END()
