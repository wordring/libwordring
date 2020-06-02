// test/html/simple_html_sample_parsing.cpp

#include <boost/test/unit_test.hpp>

#include <wordring/html/simple_html.hpp>

#include <wordring/tree/tree.hpp>

#include <iterator>
#include <string>

BOOST_AUTO_TEST_SUITE(simple_html_sample_parsing_test)

/*
invalid-first-character-of-tag-name

https://html.spec.whatwg.org/multipage/parsing.html#parse-error-invalid-first-character-of-tag-name
https://triple-underscore.github.io/HTML-parsing-ja.html#parse-error-invalid-first-character-of-tag-name
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_parsing_1)
{
	using namespace wordring::html;

	std::u32string in = U"<42></42>";

	simple_parser<wordring::tree<simple_node<std::u8string>>> sp;
	for (char32_t cp : in) sp.push_code_point(cp);
	sp.push_eof();
	std::u8string out;
	to_string(sp.get_document(), std::back_inserter(out));

	std::u8string s = u8"<html><head></head><body>&lt;42&gt;<!--42--></body></html>";

	BOOST_CHECK(out == s);
}

/*
non-void-html-element-start-tag-with-trailing-solidus

https://html.spec.whatwg.org/multipage/parsing.html#parse-error-non-void-html-element-start-tag-with-trailing-solidus
https://triple-underscore.github.io/HTML-parsing-ja.html#parse-error-non-void-html-element-start-tag-with-trailing-solidus
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_parsing_2)
{
	using namespace wordring::html;

	std::u32string in = U"<div/><span></span><span></span>";

	simple_parser<wordring::tree<simple_node<std::u8string>>> sp;
	for (char32_t cp : in) sp.push_code_point(cp);
	sp.push_eof();
	std::u8string out;
	to_string(sp.get_document(), std::back_inserter(out));

	std::u8string s = u8"<html><head></head><body><div><span></span><span></span></div></body></html>";

	BOOST_CHECK(out == s);
}

/*
unexpected-character-in-attribute-name

https://html.spec.whatwg.org/multipage/parsing.html#parse-error-unexpected-character-in-attribute-name
https://triple-underscore.github.io/HTML-parsing-ja.html#parse-error-unexpected-character-in-attribute-name
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_parsing_3)
{
	using namespace wordring::html;

	std::u32string in = U"<div foo<div>";

	simple_parser<wordring::tree<simple_node<std::u8string>>> sp;
	for (char32_t cp : in) sp.push_code_point(cp);
	sp.push_eof();
	std::u8string out;
	to_string(sp.get_document(), std::back_inserter(out));

	std::u8string s = u8"<html><head></head><body><div foo<div=\"\"></div></body></html>";

	BOOST_CHECK(out == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_parsing_4)
{
	using namespace wordring::html;

	std::u32string in = U"<div id'bar'>";

	simple_parser<wordring::tree<simple_node<std::u8string>>> sp;
	for (char32_t cp : in) sp.push_code_point(cp);
	sp.push_eof();
	std::u8string out;
	to_string(sp.get_document(), std::back_inserter(out));

	std::u8string s = u8"<html><head></head><body><div id'bar'=\"\"></div></body></html>";

	BOOST_CHECK(out == s);
}

/*
unexpected-character-in-unquoted-attribute-value

https://html.spec.whatwg.org/multipage/parsing.html#parse-error-unexpected-character-in-unquoted-attribute-value
https://triple-underscore.github.io/HTML-parsing-ja.html#parse-error-unexpected-character-in-unquoted-attribute-value
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_parsing_5)
{
	using namespace wordring::html;

	std::u32string in = U"<div foo=b'ar'>";

	simple_parser<wordring::tree<simple_node<std::u8string>>> sp;
	for (char32_t cp : in) sp.push_code_point(cp);
	sp.push_eof();
	std::u8string out;
	to_string(sp.get_document(), std::back_inserter(out));

	// Chromeは属性値内の単引用符を&#39;に置き換えているが本実装は仕様通りの動作としている。
	// 【参考】Chromeの出力
	// <html><head></head><body><div foo=\"b&#39;ar&#39;\"></div></body></html>
	std::u8string s = u8"<html><head></head><body><div foo=\"b'ar'\"></div></body></html>";

	BOOST_CHECK(out == s);
}

/*
unexpected-equals-sign-before-attribute-name

https://html.spec.whatwg.org/multipage/parsing.html#parse-error-unexpected-equals-sign-before-attribute-name
https://triple-underscore.github.io/HTML-parsing-ja.html#parse-error-unexpected-equals-sign-before-attribute-name
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_parsing_6)
{
	using namespace wordring::html;

	std::u32string in = U"<div foo=\"bar\" =\"baz\">";

	simple_parser<wordring::tree<simple_node<std::u8string>>> sp;
	for (char32_t cp : in) sp.push_code_point(cp);
	sp.push_eof();
	std::u8string out;
	to_string(sp.get_document(), std::back_inserter(out));

	std::u8string s = u8"<html><head></head><body><div foo=\"bar\" =\"baz\"=\"\"></div></body></html>";

	BOOST_CHECK(out == s);
}

/*
unexpected-question-mark-instead-of-tag-name

https://html.spec.whatwg.org/multipage/parsing.html#parse-error-unexpected-question-mark-instead-of-tag-name
https://triple-underscore.github.io/HTML-parsing-ja.html#parse-error-unexpected-question-mark-instead-of-tag-name
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_parsing_7)
{
	using namespace wordring::html;

	std::u32string in = U"<?xml-stylesheet type=\"text / css\" href=\"style.css\"?>";

	simple_parser<wordring::tree<simple_node<std::u8string>>> sp;
	for (char32_t cp : in) sp.push_code_point(cp);
	sp.push_eof();
	std::u8string out;
	to_string(sp.get_document(), std::back_inserter(out));

	// 仕様によると、文書直下のコメントは許される。
	// https://triple-underscore.github.io/HTML-writing-ja.html#writing
	// しかしながら、Chromeではコメントは消去されていた。
	std::u8string s = u8"<!--?xml-stylesheet type=\"text / css\" href=\"style.css\"?--><html><head></head><body></body></html>";

	BOOST_CHECK(out == s);
}

/*
insert a character

 https://html.spec.whatwg.org/multipage/parsing.html#insert-a-character
 https://triple-underscore.github.io/HTML-parsing-ja.html#insert-a-character
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_parsing_8)
{
	using namespace wordring::html;

	std::u32string in = U"A<table>B<tr>C</tr>D</table>";

	simple_parser<wordring::tree<simple_node<std::u8string>>> sp;
	for (char32_t cp : in) sp.push_code_point(cp);
	sp.push_eof();
	std::u8string out;
	to_string(sp.get_document(), std::back_inserter(out));

	std::u8string s = u8"<html><head></head><body>ABCD<table><tbody><tr></tr></tbody></table></body></html>";

	BOOST_CHECK(out == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_parsing_9)
{
	using namespace wordring::html;

	std::u32string in = U"A<table><tr> B</tr> C</table>";

	simple_parser<wordring::tree<simple_node<std::u8string>>> sp;
	for (char32_t cp : in) sp.push_code_point(cp);
	sp.push_eof();
	std::u8string out;
	to_string(sp.get_document(), std::back_inserter(out));

	std::u8string s = u8"<html><head></head><body>A B C<table><tbody><tr></tr></tbody></table></body></html>";

	BOOST_CHECK(out == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_parsing_10)
{
	using namespace wordring::html;

	std::u32string in = U"A<table><tr> B</tr> </em>C</table>";

	simple_parser<wordring::tree<simple_node<std::u8string>>> sp;
	for (char32_t cp : in) sp.push_code_point(cp);
	sp.push_eof();
	std::u8string out;
	to_string(sp.get_document(), std::back_inserter(out));

	std::u8string s = u8"<html><head></head><body>A BC<table><tbody><tr></tr> </tbody></table></body></html>";

	BOOST_CHECK(out == s);
}

/*
12.2.6.4.7 The "in body" insertion mode

https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-inbody
https://triple-underscore.github.io/HTML-parsing-ja.html#parsing-main-inbody
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_parsing_11)
{
	using namespace wordring::html;

	std::u32string in = U"<a href=\"a\">a<table><a href=\"b\">b</table>x";

	simple_parser<wordring::tree<simple_node<std::u8string>>> sp;
	for (char32_t cp : in) sp.push_code_point(cp);
	sp.push_eof();
	std::u8string out;
	to_string(sp.get_document(), std::back_inserter(out));

	std::u8string s = u8"<html><head></head><body><a href=\"a\">a<a href=\"b\">b</a><table></table></a><a href=\"b\">x</a></body></html>";

	BOOST_CHECK(out == s);
}

/*
12.2.9.1 Misnested tags: <b><i></b></i>

https://html.spec.whatwg.org/multipage/parsing.html#misnested-tags:-b-i-/b-/i
https://triple-underscore.github.io/HTML-parsing-ja.html#misnested-tags:-b-i-/b-/i
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_parsing_12)
{
	using namespace wordring::html;

	std::u32string in = U"<p>1<b>2<i>3</b>4</i>5</p>";

	simple_parser<wordring::tree<simple_node<std::u8string>>> sp;
	for (char32_t cp : in) sp.push_code_point(cp);
	sp.push_eof();
	std::u8string out;
	to_string(sp.get_document(), std::back_inserter(out));

	std::u8string s = u8"<html><head></head><body><p>1<b>2<i>3</i></b><i>4</i>5</p></body></html>";

	BOOST_CHECK(out == s);
}

/*
12.2.9.2 Misnested tags: <b><p></b></p>

https://html.spec.whatwg.org/multipage/parsing.html#misnested-tags:-b-p-/b-/p
https://triple-underscore.github.io/HTML-parsing-ja.html#misnested-tags:-b-p-/b-/p
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_parsing_13)
{
	using namespace wordring::html;

	std::u32string in = U"<b>1<p>2</b>3</p>";

	simple_parser<wordring::tree<simple_node<std::u8string>>> sp;
	for (char32_t cp : in) sp.push_code_point(cp);
	sp.push_eof();
	std::u8string out;
	to_string(sp.get_document(), std::back_inserter(out));

	std::u8string s = u8"<html><head></head><body><b>1</b><p><b>2</b>3</p></body></html>";

	BOOST_CHECK(out == s);
}

/*
12.2.9.3 Unexpected markup in tables

https://html.spec.whatwg.org/multipage/parsing.html#unexpected-markup-in-tables
https://triple-underscore.github.io/HTML-parsing-ja.html#unexpected-markup-in-tables
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_parsing_14)
{
	using namespace wordring::html;

	std::u32string in = U"<table><b><tr><td>aaa</td></tr>bbb</table>ccc";

	simple_parser<wordring::tree<simple_node<std::u8string>>> sp;
	for (char32_t cp : in) sp.push_code_point(cp);
	sp.push_eof();
	std::u8string out;
	to_string(sp.get_document(), std::back_inserter(out));

	std::u8string s = u8"<html><head></head><body><b></b><b>bbb</b><table><tbody><tr><td>aaa</td></tr></tbody></table><b>ccc</b></body></html>";

	BOOST_CHECK(out == s);
}

/*
12.2.9.6 Unclosed formatting elements

https://html.spec.whatwg.org/multipage/parsing.html#unclosed-formatting-elements
https://triple-underscore.github.io/HTML-parsing-ja.html#unclosed-formatting-elements
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_parsing_15)
{
	using namespace wordring::html;

	std::u32string in =
		U"<!DOCTYPE html>\n"
		U"<p><b class=x><b class=x><b><b class=x><b class=x><b>X\n"
		U"<p>X\n"
		U"<p><b><b class=x><b>X\n"
		U"<p></b></b></b></b></b></b>X";

	simple_parser<wordring::tree<simple_node<std::u8string>>> sp;
	for (char32_t cp : in) sp.push_code_point(cp);
	sp.push_eof();
	std::u8string out;
	to_string(sp.get_document(), std::back_inserter(out));

	std::u8string s =
		u8"<!DOCTYPE html><html><head></head><body><p><b class=\"x\"><b class=\"x\"><b><b class=\"x\"><b class=\"x\"><b>X\n"
		u8"</b></b></b></b></b></b></p><p><b class=\"x\"><b><b class=\"x\"><b class=\"x\"><b>X\n"
		u8"</b></b></b></b></b></p><p><b class=\"x\"><b><b class=\"x\"><b class=\"x\"><b><b><b class=\"x\"><b>X\n"
		u8"</b></b></b></b></b></b></b></b></p><p>X</p></body></html>";

	BOOST_CHECK(out == s);
}

BOOST_AUTO_TEST_SUITE_END()
