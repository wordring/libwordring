// test/html/simple_html_sample_introduction.cpp

#include <boost/test/unit_test.hpp>

#include <wordring/html/simple_html.hpp>

#include <wordring/compatibility.hpp>
#include <wordring/tree/tree.hpp>

#include <iostream>
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

BOOST_AUTO_TEST_SUITE(simple_html_sample_introduction_test)

/*
1.10 A quick introduction to HTML

https://html.spec.whatwg.org/multipage/introduction.html#a-quick-introduction-to-html
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_introduction_1)
{
	char32_t const* in = UR"(<!DOCTYPE html>
<html lang="en">
 <head>
  <title>Sample page</title>
 </head>
 <body>
  <h1>Sample page</h1>
  <p>This is a <a href="demo.html">simple</a> sample.</p>
  <!-- this is a comment -->
 </body>
</html>)";

	char32_t const* s = UR"(<!DOCTYPE html><html lang="en"><head>
  <title>Sample page</title>
 </head>
 <body>
  <h1>Sample page</h1>
  <p>This is a <a href="demo.html">simple</a> sample.</p>
  <!-- this is a comment -->
 
</body></html>)";

	BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_introduction_2)
{
	char32_t const* in = UR"(<p>This is <em>very <strong>wrong</em>!</strong></p>)";

	char32_t const* s = UR"(<html><head></head><body><p>This is <em>very <strong>wrong</strong></em><strong>!</strong></p></body></html>)";

	BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_introduction_3)
{
	char32_t const* in = UR"(<p>This <em>is <strong>correct</strong>.</em></p>)";

	char32_t const* s = UR"(<html><head></head><body><p>This <em>is <strong>correct</strong>.</em></p></body></html>)";

	BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_introduction_4)
{
	char32_t const* in = UR"(<a href="demo.html">simple</a>)";

	char32_t const* s = UR"(<html><head></head><body><a href="demo.html">simple</a></body></html>)";

	BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_introduction_5)
{
	char32_t const* in = UR"(<!-- empty attributes -->
<input name=address disabled>
<input name=address disabled="">

<!-- attributes with a value -->
<input name=address maxlength=200>
<input name=address maxlength='200'>
<input name=address maxlength="200">)";

	char32_t const* s = UR"(<!-- empty attributes --><html><head></head><body><input name="address" disabled="">
<input name="address" disabled="">

<!-- attributes with a value -->
<input name="address" maxlength="200">
<input name="address" maxlength="200">
<input name="address" maxlength="200"></body></html>)";

	BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_introduction_6)
{
	char32_t const* in = UR"(<form name="main">
 Result: <output name="result"></output>
 <script>
  document.forms.main.elements.result.value = 'Hello World';
 </script>
</form>)";

	char32_t const* s = UR"(<html><head></head><body><form name="main">
 Result: <output name="result"></output>
 <script>
  document.forms.main.elements.result.value = 'Hello World';
 </script>
</form></body></html>)";

	BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_introduction_7)
{
	char32_t const* in = UR"(<!DOCTYPE html>
<html lang="en">
 <head>
  <title>Sample styled page</title>
  <style>
   body { background: navy; color: yellow; }
  </style>
 </head>
 <body>
  <h1>Sample styled page</h1>
  <p>This page is just a demo.</p>
 </body>
</html>)";

	char32_t const* s = UR"(<!DOCTYPE html><html lang="en"><head>
  <title>Sample styled page</title>
  <style>
   body { background: navy; color: yellow; }
  </style>
 </head>
 <body>
  <h1>Sample styled page</h1>
  <p>This page is just a demo.</p>
 
</body></html>)";

	BOOST_CHECK(parse(in) == s);
}


/*
1.10.1 Writing secure applications with HTML

https://html.spec.whatwg.org/multipage/introduction.html#writing-secure-applications-with-html
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_introduction_8)
{
	char32_t const* in = UR"(<ul>
 <li><a href="message.cgi?say=Hello">Say Hello</a>
 <li><a href="message.cgi?say=Welcome">Say Welcome</a>
 <li><a href="message.cgi?say=Kittens">Say Kittens</a>
</ul>)";

	char32_t const* s = UR"(<html><head></head><body><ul>
 <li><a href="message.cgi?say=Hello">Say Hello</a>
 </li><li><a href="message.cgi?say=Welcome">Say Welcome</a>
 </li><li><a href="message.cgi?say=Kittens">Say Kittens</a>
</li></ul></body></html>)";

	BOOST_CHECK(parse(in) == s);
}

/*
1.10.2 Common pitfalls to avoid when using the scripting APIs

https://html.spec.whatwg.org/multipage/introduction.html#common-pitfalls-to-avoid-when-using-the-scripting-apis
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_introduction_9)
{
	char32_t const* in = UR"*(<img src="games.png" alt="Games" onload="gamesLogoHasLoaded(event)">)*";

	char32_t const* s = UR"*(<html><head></head><body><img src="games.png" alt="Games" onload="gamesLogoHasLoaded(event)"></body></html>)*";

	BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_introduction_10)
{
    char32_t const* in = UR"(<script>
 var img = new Image();
 img.src = 'games.png';
 img.alt = 'Games';
 img.onload = gamesLogoHasLoaded;
 // img.addEventListener('load', gamesLogoHasLoaded, false); // would work also
</script>)";

    char32_t const* s = UR"(<html><head><script>
 var img = new Image();
 img.src = 'games.png';
 img.alt = 'Games';
 img.onload = gamesLogoHasLoaded;
 // img.addEventListener('load', gamesLogoHasLoaded, false); // would work also
</script></head><body></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_introduction_11)
{
    char32_t const* in = UR"(<!-- Do not use this style, it has a race condition! -->
 <img id="games" src="games.png" alt="Games">
 <!-- the 'load' event might fire here while the parser is taking a
      break, in which case you will not see it! -->
 <script>
  var img = document.getElementById('games');
  img.onload = gamesLogoHasLoaded; // might never fire!
 </script>)";

    char32_t const* s = UR"(<!-- Do not use this style, it has a race condition! --><html><head></head><body><img id="games" src="games.png" alt="Games">
 <!-- the 'load' event might fire here while the parser is taking a
      break, in which case you will not see it! -->
 <script>
  var img = document.getElementById('games');
  img.onload = gamesLogoHasLoaded; // might never fire!
 </script></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

/*
1.11.2 Syntax errors

https://html.spec.whatwg.org/multipage/introduction.html#syntax-errors
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_introduction_12)
{
    char32_t const* in = UR"(<table><hr>)";

    char32_t const* s = UR"(<html><head></head><body><hr><table></table></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_introduction_13)
{
    char32_t const* in = UR"(<p><i>She dreamt.
<p><i>She dreamt that she ate breakfast.
<p><i>Then lunch.
<p><i>And finally dinner.)";

    char32_t const* s = UR"(<html><head></head><body><p><i>She dreamt.
</i></p><p><i><i>She dreamt that she ate breakfast.
</i></i></p><p><i><i><i>Then lunch.
</i></i></i></p><p><i><i><i><i>And finally dinner.</i></i></i></i></p></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_introduction_14)
{
    char32_t const* in = UR"(<a href="?bill&ted">Bill and Ted</a>)";

    char32_t const* s = UR"(<html><head></head><body><a href="?bill&amp;ted">Bill and Ted</a></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_introduction_15)
{
    char32_t const* in = UR"(<a href="?art&copy">Art and Copy</a>)";

    char32_t const* s = UR"(<html><head></head><body><a href="?art©">Art and Copy</a></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_introduction_16)
{
    char32_t const* in = UR"(<a href="?bill&ted">Bill and Ted</a> <!-- &ted is ok, since it's not a named character reference -->)";

    char32_t const* s = UR"(<html><head></head><body><a href="?bill&amp;ted">Bill and Ted</a> <!-- &ted is ok, since it's not a named character reference --></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_introduction_17)
{
    char32_t const* in = UR"(<a href="?art&amp;copy">Art and Copy</a> <!-- the & has to be escaped, since &copy is a named character reference -->)";

    char32_t const* s = UR"(<html><head></head><body><a href="?art&amp;copy">Art and Copy</a> <!-- the & has to be escaped, since &copy is a named character reference --></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_introduction_18)
{
    char32_t const* in = UR"(<h1>Contact details</h2>)";

    char32_t const* s = UR"(<html><head></head><body><h1>Contact details</h1></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

/*
1.11.3 Restrictions on content models and on attribute values

https://html.spec.whatwg.org/multipage/introduction.html#restrictions-on-content-models-and-on-attribute-values
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_introduction_19)
{
    char32_t const* in = UR"(<hr role="cell">)";

    char32_t const* s = UR"(<html><head></head><body><hr role="cell"></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_introduction_20)
{
    char32_t const* in = UR"(<input type=radio role=progressbar>)";

    char32_t const* s = UR"(<html><head></head><body><input type="radio" role="progressbar"></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_introduction_21)
{
    char32_t const* in = UR"(<p>Welcome. <form><label>Name:</label> <input></form>)";

    char32_t const* s = UR"(<html><head></head><body><p>Welcome. </p><form><label>Name:</label> <input></form></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_introduction_22)
{
    char32_t const* in = UR"(<p>Welcome. </p><form><label>Name:</label> <input></form>)";

    char32_t const* s = UR"(<html><head></head><body><p>Welcome. </p><form><label>Name:</label> <input></form></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_SUITE_END()
