// test/html/simple_html_sample_embedded.cpp

#include <boost/test/unit_test.hpp>

#include <wordring/html/simple_html.hpp>

#include <wordring/compatibility.hpp>
#include <wordring/tree/tree.hpp>

#include <iterator>
#include <string>

namespace
{
	inline std::u32string parse(std::u32string_view in)
	{
		using namespace wordring::html;

		simple_parser<wordring::tree<simple_node<std::u8string>>> sp;
		for (char32_t cp : in) sp.push_code_point(cp);
		sp.push_eof();
		std::u32string out;
		to_string(sp.get_document(), std::back_inserter(out));

		std::cout << wordring::encoding_cast<std::u8string>(out);

		return out;
	}
}

BOOST_AUTO_TEST_SUITE(simple_html_sample_embedded_test)

/*
4.8.1 The picture element

https://html.spec.whatwg.org/multipage/embedded-content.html#the-picture-element
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_embedded_1)
{
	char32_t const* in = UR"()";

	char32_t const* s = UR"()";

	BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_embedded_2)
{
	char32_t const* in = UR"()";

	char32_t const* s = UR"()";

	BOOST_CHECK(parse(in) == s);
}


BOOST_AUTO_TEST_CASE(simple_html_sample_embedded_9)
{
	char32_t const* in = UR"()";

	char32_t const* s = UR"()";

	BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_SUITE_END()
