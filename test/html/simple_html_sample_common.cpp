// test/html/simple_html_sample_common.cpp

#include <boost/test/unit_test.hpp>

#include <wordring/html/simple_html.hpp>

#include <wordring/compatibility.hpp>

#include <iostream>
#include <iterator>
#include <string>

namespace
{
	inline std::u32string parse(std::u32string_view in)
	{
		using namespace wordring::html;

		simple_parser<u8simple_tree> sp;
		for (char32_t cp : in) sp.push_code_point(cp);
		sp.push_eof();
		std::u32string out;
		to_string(sp.get_document(), std::back_inserter(out));

		// std::cout << wordring::encoding_cast<std::u8string>(out);

		return out;
	}
}

BOOST_AUTO_TEST_SUITE(simple_html_sample_common_test)

/*
2.4.2 Boolean attributes

https://html.spec.whatwg.org/multipage/common-microsyntaxes.html#boolean-attributes
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_common_1)
{
    char32_t const* in = UR"(<label><input type=checkbox checked name=cheese disabled> Cheese</label>)";

    char32_t const* s = UR"(<html><head></head><body><label><input type="checkbox" checked="" name="cheese" disabled=""> Cheese</label></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_common_2)
{
	char32_t const* in = UR"(<label><input type=checkbox checked=checked name=cheese disabled=disabled> Cheese</label>)";

	char32_t const* s = UR"(<html><head></head><body><label><input type="checkbox" checked="checked" name="cheese" disabled="disabled"> Cheese</label></body></html>)";

	BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_common_3)
{
	char32_t const* in = UR"(<label><input type='checkbox' checked name=cheese disabled=""> Cheese</label>)";

	char32_t const* s = UR"(<html><head></head><body><label><input type="checkbox" checked="" name="cheese" disabled=""> Cheese</label></body></html>)";

	BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_SUITE_END()
