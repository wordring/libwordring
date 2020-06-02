// test/html/simple_html_sample_sections.cpp

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

BOOST_AUTO_TEST_SUITE(simple_html_sample_sections_test)

BOOST_AUTO_TEST_CASE(simple_html_sample_sections_1)
{
	char32_t const* in = UR"()";

	char32_t const* s = UR"()";

	BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_sections_2)
{
	char32_t const* in = UR"()";

	char32_t const* s = UR"()";

	BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_sections_3)
{
	char32_t const* in = UR"()";

	char32_t const* s = UR"()";

	BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_sections_4)
{
	char32_t const* in = UR"()";

	char32_t const* s = UR"()";

	BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_sections_5)
{
	char32_t const* in = UR"()";

	char32_t const* s = UR"()";

	BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_sections_6)
{
	char32_t const* in = UR"()";

	char32_t const* s = UR"()";

	BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_sections_7)
{
	char32_t const* in = UR"()";

	char32_t const* s = UR"()";

	BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_sections_8)
{
	char32_t const* in = UR"()";

	char32_t const* s = UR"()";

	BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_sections_9)
{
	char32_t const* in = UR"()";

	char32_t const* s = UR"()";

	BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_SUITE_END()
