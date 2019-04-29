
#include <boost/test/unit_test.hpp>

#include <string>

#include <wordring/whatwg/encoding/encoding_stream.hpp>

BOOST_AUTO_TEST_SUITE(whatwg_encoding_stream_test)

BOOST_AUTO_TEST_CASE(whatwg_encoding__encoding_stream)
{
	std::u32string s32_original{ U" hello " };
	wordring::whatwg::encoding_stream<std::u32string::iterator> s32_stream{ s32_original.begin(), s32_original.end() };
	std::u32string s32_prepend{ U"&#128169;" };
	s32_stream.prepend(s32_prepend.begin(), s32_prepend.end());
	std::u32string s32_push{ U"world" };
	s32_stream.push(s32_push.begin(), s32_push.end());
	BOOST_CHECK(s32_stream.read() == U'&');
	std::u32string s32_result{ U"&" };
	while (s32_stream) s32_result.push_back(s32_stream.read());
	BOOST_CHECK(s32_result == U"&#128169; hello world");
	BOOST_CHECK(!s32_stream);
}

BOOST_AUTO_TEST_SUITE_END()
