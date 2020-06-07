// test/html/simple_html_sample_syntax.cpp

#include <boost/test/unit_test.hpp>

#include <wordring/html/simple_html.hpp>

#include <wordring/compatibility.hpp>
#include <wordring/tree/tree.hpp>

#include <iostream>
#include <iterator>
#include <string>

BOOST_AUTO_TEST_SUITE(simple_html_sample_syntax_test)

/*
12.1.2 Elements

https://html.spec.whatwg.org/multipage/syntax.html#elements-2
https://triple-underscore.github.io/HTML-writing-ja.html#elements-2
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_syntax_1)
{
	using namespace wordring::html;

	std::u32string in =
		U"<p>\r\n"
		U" <svg>\r\n"
		U"  <metadata>\r\n"
		U"   <!-- this is invalid -->\r\n"
		U"   <cdr:license xmlns:cdr=\"https://www.example.com/cdr/metadata\" name=\"MIT\"/>\r\n"
		U"  </metadata>\r\n"
		U" </svg>\r\n"
		U"</p>\r\n";

	simple_parser<wordring::tree<simple_node<std::u8string>>> sp;
	for (char32_t cp : in) sp.push_code_point(cp);
	sp.push_eof();
	std::u8string out;
	to_string(sp.get_document(), std::back_inserter(out));

	// 出力を見やすくコピーしただけで、規格上正しいのか不明。
	std::u8string s =
		u8"<html><head></head><body><p>\n"
		u8" <svg>\n"
		u8"  <metadata>\n"
		u8"   <!-- this is invalid -->\n"
		u8"   <cdr:license xmlns:cdr=\"https://www.example.com/cdr/metadata\" name=\"MIT\"></cdr:license>\n"
		u8"  </metadata>\n"
		u8" </svg>\n"
		u8"</p>\n"
		u8"</body></html>";

	BOOST_CHECK(out == s);
}

/*
12.1.2.4 Optional tags

https://html.spec.whatwg.org/multipage/syntax.html#optional-tags
https://triple-underscore.github.io/HTML-writing-ja.html#optional-tags
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_syntax_2)
{
	using namespace wordring::html;

	std::u32string in =
		U"<!DOCTYPE HTML>\n"
		U"\n"
		U"  <head>\n"
		U"    <title>Hello</title>\n"
		U"  </head>\n"
		U"  <body>\n"
		U"    <p>Welcome to this example.</p>\n"
		U"  </body>\n"
		U"</html>\n";

	simple_parser<wordring::tree<simple_node<std::u8string>>> sp;
	for (char32_t cp : in) sp.push_code_point(cp);
	sp.push_eof();
	std::u8string out;
	to_string(sp.get_document(), std::back_inserter(out));

	// 仕様において、<head>の前までは改行などの空白文字が無視される。
	// しかしながら、Chromeは直列化時に<!DOCTYPE html>の後に必ず改行を入れる。
	// 仕様において、</body>あるいは</html>の後は、文字の挿入位置が変更されない。
	// したがって、</body>の後の改行と</html>の後の改行が<body>内の末尾に挿入されている。
	// Chromeも本実装と同様の挿入位置となっている。
	std::u8string s =
		u8"<!DOCTYPE html><html><head>\n"
		u8"    <title>Hello</title>\n"
		u8"  </head>\n"
		u8"  <body>\n"
		u8"    <p>Welcome to this example.</p>\n"
		u8"  \n" // </body>の後の改行
		u8"\n"   // </html>の後の改行
		u8"</body></html>";

	BOOST_CHECK(out == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_syntax_3)
{
	using namespace wordring::html;

	std::u32string in =
		U"<!DOCTYPE HTML><head>\n"
		U"    <title>Hello</title>\n"
		U"  </head>\n"
		U"  <body>\n"
		U"    <p>Welcome to this example.</p>\n"
		U"  </body>\n"
		U"</html>\n";

	simple_parser<wordring::tree<simple_node<std::u8string>>> sp;
	for (char32_t cp : in) sp.push_code_point(cp);
	sp.push_eof();
	std::u8string out;
	to_string(sp.get_document(), std::back_inserter(out));

	std::u8string s =
		u8"<!DOCTYPE html><html><head>\n"
		u8"    <title>Hello</title>\n"
		u8"  </head>\n"
		u8"  <body>\n"
		u8"    <p>Welcome to this example.</p>\n"
		u8"  \n" // </body>の後の改行
		u8"\n"   // </html>の後の改行
		u8"</body></html>";

	BOOST_CHECK(out == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_syntax_4)
{
	using namespace wordring::html;

	// <html>を省略しない場合のコメント位置を確認。
	std::u32string in =
		U"<!DOCTYPE HTML>\n"
		U"<html>\n"
		U"  <!-- where is this comment in the DOM? -->\n"
		U"  <head>\n"
		U"    <title>Hello</title>\n"
		U"  </head>\n"
		U"  <body>\n"
		U"    <p>Welcome to this example.</p>\n"
		U"  </body>\n"
		U"</html>\n";

	simple_parser<wordring::tree<simple_node<std::u8string>>> sp;
	for (char32_t cp : in) sp.push_code_point(cp);
	sp.push_eof();
	std::u8string out;
	to_string(sp.get_document(), std::back_inserter(out));

	std::u8string s =
		u8"<!DOCTYPE html><html><!-- where is this comment in the DOM? --><head>\n"
		u8"    <title>Hello</title>\n"
		u8"  </head>\n"
		u8"  <body>\n"
		u8"    <p>Welcome to this example.</p>\n"
		u8"  \n" // </body>の後の改行
		u8"\n"   // </html>の後の改行
		u8"</body></html>";

	BOOST_CHECK(out == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_syntax_5)
{
	using namespace wordring::html;

	// <html>を省略した場合のコメント位置を確認。
	std::u32string in =
		U"<!DOCTYPE HTML>\n"
		U"\n"
		U"  <!-- where is this comment in the DOM? -->\n"
		U"  <head>\n"
		U"    <title>Hello</title>\n"
		U"  </head>\n"
		U"  <body>\n"
		U"    <p>Welcome to this example.</p>\n"
		U"  </body>\n"
		U"</html>\n";

	simple_parser<wordring::tree<simple_node<std::u8string>>> sp;
	for (char32_t cp : in) sp.push_code_point(cp);
	sp.push_eof();
	std::u8string out;
	to_string(sp.get_document(), std::back_inserter(out));

	std::u8string s =
		u8"<!DOCTYPE html><!-- where is this comment in the DOM? --><html><head>\n"
		u8"    <title>Hello</title>\n"
		u8"  </head>\n"
		u8"  <body>\n"
		u8"    <p>Welcome to this example.</p>\n"
		u8"  \n" // </body>の後の改行
		u8"\n"   // </html>の後の改行
		u8"</body></html>";

	BOOST_CHECK(out == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_syntax_6)
{
	using namespace wordring::html;

	std::u32string in = U"<!DOCTYPE HTML><title>Hello</title><p>Welcome to this example.</p>";

	simple_parser<wordring::tree<simple_node<std::u8string>>> sp;
	for (char32_t cp : in) sp.push_code_point(cp);
	sp.push_eof();
	std::u8string out;
	to_string(sp.get_document(), std::back_inserter(out));

	std::u8string s = u8"<!DOCTYPE html><html><head><title>Hello</title></head><body><p>Welcome to this example.</p></body></html>";

	BOOST_CHECK(out == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_syntax_7)
{
	using namespace wordring::html;

	std::u32string in =
		U"<!DOCTYPE HTML>\n"
		U"<title>Hello</title>\n"
		U"<p>Welcome to this example.</p>";

	simple_parser<wordring::tree<simple_node<std::u8string>>> sp;
	for (char32_t cp : in) sp.push_code_point(cp);
	sp.push_eof();
	std::u8string out;
	to_string(sp.get_document(), std::back_inserter(out));

	std::u8string s =
		u8"<!DOCTYPE html><html><head><title>Hello</title>\n"
		u8"</head><body><p>Welcome to this example.</p></body></html>";

	BOOST_CHECK(out == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_syntax_8)
{
	using namespace wordring::html;

	std::u32string in = U"<!DOCTYPE HTML><title>Hello</title><p>Welcome to this example.";

	simple_parser<wordring::tree<simple_node<std::u8string>>> sp;
	for (char32_t cp : in) sp.push_code_point(cp);
	sp.push_eof();
	std::u8string out;
	to_string(sp.get_document(), std::back_inserter(out));

	std::u8string s = u8"<!DOCTYPE html><html><head><title>Hello</title></head><body><p>Welcome to this example.</p></body></html>";

	BOOST_CHECK(out == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_syntax_9)
{
	using namespace wordring::html;

	std::u32string in =
		U"<table>\n"
		U" <caption>37547 TEE Electric Powered Rail Car Train Functions (Abbreviated)\n"
		U" <colgroup><col><col><col>\n"
		U" <thead>\n"
		U"  <tr>\n"
		U"   <th>Function\n"
		U"   <th>Control Unit\n"
		U"   <th>Central Station\n"
		U" <tbody>\n"
		U"  <tr>\n"
		U"   <td>Headlights\n"
		U"   <td>✔\n"
		U"   <td>✔\n"
		U"  <tr>\n"
		U"   <td>Interior Lights\n"
		U"   <td>✔\n"
		U"   <td>✔\n"
		U"  <tr>\n"
		U"   <td>Electric locomotive operating sounds\n"
		U"   <td>✔\n"
		U"   <td>✔\n"
		U"  <tr>\n"
		U"   <td>Engineer's cab lighting\n"
		U"   <td>\n"
		U"   <td>✔\n"
		U"  <tr>\n"
		U"   <td>Station Announcements - Swiss\n"
		U"   <td>\n"
		U"   <td>✔\n"
		U"</table>\n";

	simple_parser<wordring::tree<simple_node<std::u8string>>> sp;
	for (char32_t cp : in) sp.push_code_point(cp);
	sp.push_eof();
	std::u8string out;
	to_string(sp.get_document(), std::back_inserter(out));

	std::u8string s =
		u8"<html><head></head><body><table>\n"
		u8" <caption>37547 TEE Electric Powered Rail Car Train Functions (Abbreviated)\n"
		u8" </caption><colgroup><col><col><col>\n"
		u8" </colgroup><thead>\n"
		u8"  <tr>\n"
		u8"   <th>Function\n"
		u8"   </th><th>Control Unit\n"
		u8"   </th><th>Central Station\n"
		u8" </th></tr></thead><tbody>\n"
		u8"  <tr>\n"
		u8"   <td>Headlights\n"
		u8"   </td><td>✔\n"
		u8"   </td><td>✔\n"
		u8"  </td></tr><tr>\n"
		u8"   <td>Interior Lights\n"
		u8"   </td><td>✔\n"
		u8"   </td><td>✔\n"
		u8"  </td></tr><tr>\n"
		u8"   <td>Electric locomotive operating sounds\n"
		u8"   </td><td>✔\n"
		u8"   </td><td>✔\n"
		u8"  </td></tr><tr>\n"
		u8"   <td>Engineer's cab lighting\n"
		u8"   </td><td>\n"
		u8"   </td><td>✔\n"
		u8"  </td></tr><tr>\n"
		u8"   <td>Station Announcements - Swiss\n"
		u8"   </td><td>\n"
		u8"   </td><td>✔\n"
		u8"</td></tr></tbody></table>\n"
		u8"</body></html>";

	std::cout << out << std::endl;

	std::cout << wordring::encoding_cast<std::u8string>(s);

	BOOST_CHECK(out == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_syntax_10)
{
	using namespace wordring::html;

	std::u32string in =
		U"<table>\n"
		U" <caption>37547 TEE Electric Powered Rail Car Train Functions (Abbreviated)\n"
		U" <colgroup><col><col><col>\n"
		U" <thead>\n"
		U"  <tr> <th>Function                              <th>Control Unit     <th>Central Station\n"
		U" <tbody>\n"
		U"  <tr> <td>Headlights                            <td>✔                <td>✔\n"
		U"  <tr> <td>Interior Lights                       <td>✔                <td>✔\n"
		U"  <tr> <td>Electric locomotive operating sounds  <td>✔                <td>✔\n"
		U"  <tr> <td>Engineer's cab lighting               <td>                 <td>✔\n"
		U"  <tr> <td>Station Announcements - Swiss         <td>                 <td>✔\n"
		U"</table>\n";


	simple_parser<wordring::tree<simple_node<std::u8string>>> sp;
	for (char32_t cp : in) sp.push_code_point(cp);
	sp.push_eof();
	std::u8string out;
	to_string(sp.get_document(), std::back_inserter(out));

	std::u8string s =
		u8"<html><head></head><body><table>\n"
		u8" <caption>37547 TEE Electric Powered Rail Car Train Functions (Abbreviated)\n"
		u8" </caption><colgroup><col><col><col>\n"
		u8" </colgroup><thead>\n"
		u8"  <tr> <th>Function                              </th><th>Control Unit     </th><th>Central Station\n"
		u8" </th></tr></thead><tbody>\n"
		u8"  <tr> <td>Headlights                            </td><td>✔                </td><td>✔\n"
		u8"  </td></tr><tr> <td>Interior Lights                       </td><td>✔                </td><td>✔\n"
		u8"  </td></tr><tr> <td>Electric locomotive operating sounds  </td><td>✔                </td><td>✔\n"
		u8"  </td></tr><tr> <td>Engineer's cab lighting               </td><td>                 </td><td>✔\n"
		u8"  </td></tr><tr> <td>Station Announcements - Swiss         </td><td>                 </td><td>✔\n"
		u8"</td></tr></tbody></table>\n"
		u8"</body></html>";

	BOOST_CHECK(out == s);
}

/*
12.1.2.5 Restrictions on content models

https://html.spec.whatwg.org/multipage/syntax.html#element-restrictions
https://triple-underscore.github.io/HTML-writing-ja.html#element-restrictions
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_syntax_11)
{
	using namespace wordring::html;

	std::u32string in1 = U"<pre>Hello </pre>";

	simple_parser<wordring::tree<simple_node<std::u8string>>> sp1;
	for (char32_t cp : in1) sp1.push_code_point(cp);
	sp1.push_eof();
	std::string out1;
	to_string(sp1.get_document(), std::back_inserter(out1));

	std::u32string in2 =
		U"<pre>\n"
		U"Hello </pre>";

	simple_parser<wordring::tree<simple_node<std::u8string>>> sp2;
	for (char32_t cp : in2) sp2.push_code_point(cp);
	sp2.push_eof();
	std::string out2;
	to_string(sp2.get_document(), std::back_inserter(out2));

	BOOST_CHECK(out1 == out2);
}

/*
12.1.5 CDATA sections

https://html.spec.whatwg.org/multipage/syntax.html#cdata-sections
https://triple-underscore.github.io/HTML-writing-ja.html#cdata-sections
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_syntax_12)
{
	using namespace wordring::html;

	std::u32string in =
		U"<p>You can add a string to a number, but this stringifies the number:</p>\n"
		U"<math>\n"
		U" <ms><![CDATA[x<y]]></ms>\n"
		U" <mo>+</mo>\n"
		U" <mn>3</mn>\n"
		U" <mo>=</mo>\n"
		U" <ms><![CDATA[x<y3]]></ms>\n"
		U"</math>\n";

	simple_parser<wordring::tree<simple_node<std::u8string>>> sp;
	for (char32_t cp : in) sp.push_code_point(cp);
	sp.push_eof();
	std::u8string out;
	to_string(sp.get_document(), std::back_inserter(out));

	// 仕様において外来要素内ではCDATAが使えるように書かれているが、実際にはコメント化される。
	// トークン化がCDATAトークンをサポートせず、木構築段階でも回復する機構が無いので、正常な動作と考える。
	// Chromeも同様の結果となる。
	std::u8string s =
		u8"<html><head></head><body><p>You can add a string to a number, but this stringifies the number:</p>\n"
		u8"<math>\n"
		u8" <ms><!--[CDATA[x<y]]--></ms>\n"
		u8" <mo>+</mo>\n"
		u8" <mn>3</mn>\n"
		u8" <mo>=</mo>\n"
		u8" <ms><!--[CDATA[x<y3]]--></ms>\n"
		u8"</math>\n"
		u8"</body></html>";

	BOOST_CHECK(out == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_syntax_13)
{
	using namespace wordring::html;

	std::u32string in = U"<!--My favorite operators are > and <!-->";

	simple_parser<wordring::tree<simple_node<std::u8string>>> sp;
	for (char32_t cp : in) sp.push_code_point(cp);
	sp.push_eof();
	std::u8string out;
	to_string(sp.get_document(), std::back_inserter(out));

	// 仕様によると、文書直下のコメントは許される。
	// https://triple-underscore.github.io/HTML-writing-ja.html#writing
	// しかしながら、Chromeではコメントは消去されていた。
	std::u8string s = u8"<!--My favorite operators are > and <!--><html><head></head><body></body></html>";

	BOOST_CHECK(out == s);
}

BOOST_AUTO_TEST_SUITE_END()
