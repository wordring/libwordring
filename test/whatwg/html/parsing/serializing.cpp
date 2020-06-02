// test/whatwg/html/parsing/serializing.cpp

#include <boost/test/unit_test.hpp>

#include <wordring/whatwg/html/parsing/serializing.hpp>

#include <wordring/html/simple_node.hpp>
#include <wordring/html/simple_parser.hpp>
#include <wordring/tree/tree.hpp>

#include <iterator>
#include <string>

BOOST_AUTO_TEST_SUITE(whatwg_html_serializing_test)

BOOST_AUTO_TEST_CASE(serializing_to_string_1)
{
	using namespace wordring::html;

	std::u32string in(U"<!DOCTYPE HTML><!-- Comment --><html><head></head><body>\r\n<p id='text1'>text&<br>text<br></p></body></html>");
	simple_parser<wordring::tree<simple_node<std::u8string>>> sp;
	for (char32_t cp : in) sp.push_code_point(cp);
	sp.push_eof();
	std::string out;
	wordring::whatwg::html::to_string(sp.get_document(), std::back_inserter(out));

	BOOST_CHECK(out == "<!DOCTYPE html><!-- Comment --><html><head></head><body>\n<p id=\"text1\">text&amp;<br>text<br></p></body></html>");
}

BOOST_AUTO_TEST_CASE(serializing_to_string_2)
{
	using namespace wordring::html;

	std::u32string in(U"");
	simple_parser<wordring::tree<simple_node<std::u8string>>> sp;
	for (char32_t cp : in) sp.push_code_point(cp);
	sp.push_eof();
	std::string out;
	wordring::whatwg::html::to_string(sp.get_document(), std::back_inserter(out));

	BOOST_CHECK(out == "<html><head></head><body></body></html>");
}

BOOST_AUTO_TEST_CASE(serializing_to_string_3)
{
	using namespace wordring::html;

	std::u32string in(U"<html><head></head><body><![CDATA[ text ]]></body></html>");
	simple_parser<wordring::tree<simple_node<std::u8string>>> sp;
	for (char32_t cp : in) sp.push_code_point(cp);
	sp.push_eof();
	std::string out;
	wordring::whatwg::html::to_string(sp.get_document(), std::back_inserter(out));

	BOOST_CHECK(out == "<html><head></head><body> text </body></html>");
}

BOOST_AUTO_TEST_SUITE_END()
