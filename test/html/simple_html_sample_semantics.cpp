// test/html/simple_html_sample_semantics.cpp

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

BOOST_AUTO_TEST_SUITE(simple_html_sample_semantics_test)

/*
3.1.4 DOM tree accessors

https://html.spec.whatwg.org/multipage/dom.html#dom-tree-accessors
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_semantics_1)
{
	char32_t const* in = UR"(<svg xmlns="http://www.w3.org/2000/svg">
 <script>
  // document.body = document.createElementNS("http://www.w3.org/1999/xhtml", "body");
  console.assert(document.body === null);
 </script>
</svg>)";

	char32_t const* s = UR"(<html><head></head><body><svg xmlns="http://www.w3.org/2000/svg">
 <script>
  // document.body = document.createElementNS("http://www.w3.org/1999/xhtml", "body");
  console.assert(document.body === null);
 </script>
</svg></body></html>)";

	BOOST_CHECK(parse(in) == s);
}

/*
3.2.1 Semantics

https://html.spec.whatwg.org/multipage/dom.html#semantics-2
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_semantics_2)
{
	char32_t const* in = UR"(<!DOCTYPE HTML>
<html lang="en">
 <head>
  <title>My Page</title>
 </head>
 <body>
  <h1>Welcome to my page</h1>
  <p>I like cars and lorries and have a big Jeep!</p>
  <h2>Where I live</h2>
  <p>I live in a small hut on a mountain!</p>
 </body>
</html>)";

	char32_t const* s = UR"(<!DOCTYPE html><html lang="en"><head>
  <title>My Page</title>
 </head>
 <body>
  <h1>Welcome to my page</h1>
  <p>I like cars and lorries and have a big Jeep!</p>
  <h2>Where I live</h2>
  <p>I live in a small hut on a mountain!</p>
 
</body></html>)";

	BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_semantics_3)
{
	char32_t const* in = UR"(<body>
 <h1>ACME Corporation</h1>
 <h2>The leaders in arbitrary fast delivery since 1920</h2>)";

	char32_t const* s = UR"(<html><head></head><body>
 <h1>ACME Corporation</h1>
 <h2>The leaders in arbitrary fast delivery since 1920</h2></body></html>)";

	BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_semantics_4)
{
	char32_t const* in = UR"(<body>
 <hgroup>
  <h1>ACME Corporation</h1>
  <h2>The leaders in arbitrary fast delivery since 1920</h2>
 </hgroup>)";

	char32_t const* s = UR"(<html><head></head><body>
 <hgroup>
  <h1>ACME Corporation</h1>
  <h2>The leaders in arbitrary fast delivery since 1920</h2>
 </hgroup></body></html>)";

	BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_semantics_5)
{
	char32_t const* in = UR"(<!DOCTYPE HTML>
<html lang="en-GB">
 <head> <title> Demonstration </title> </head>
 <body>
  <table>
   <tr> <td> My favourite animal is the cat. </td> </tr>
   <tr>
    <td>
     —<a href="https://example.org/~ernest/"><cite>Ernest</cite></a>,
     in an essay from 1992
    </td>
   </tr>
  </table>
 </body>
</html>)";

	char32_t const* s = UR"(<!DOCTYPE html><html lang="en-GB"><head> <title> Demonstration </title> </head>
 <body>
  <table>
   <tbody><tr> <td> My favourite animal is the cat. </td> </tr>
   <tr>
    <td>
     —<a href="https://example.org/~ernest/"><cite>Ernest</cite></a>,
     in an essay from 1992
    </td>
   </tr>
  </tbody></table>
 
</body></html>)";

	BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_semantics_6)
{
	char32_t const* in = UR"(<!DOCTYPE HTML>
<html lang="en-GB">
 <head> <title> Demonstration </title> </head>
 <body>
  <blockquote>
   <p> My favourite animal is the cat. </p>
  </blockquote>
  <p>
   —<a href="https://example.org/~ernest/">Ernest</a>,
   in an essay from 1992
  </p>
 </body>
</html>)";

	char32_t const* s = UR"(<!DOCTYPE html><html lang="en-GB"><head> <title> Demonstration </title> </head>
 <body>
  <blockquote>
   <p> My favourite animal is the cat. </p>
  </blockquote>
  <p>
   —<a href="https://example.org/~ernest/">Ernest</a>,
   in an essay from 1992
  </p>
 
</body></html>)";

	BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_semantics_7)
{
	char32_t const* in = UR"(<label>Carpet: <input type="carpet" name="c" texture="deep pile"></label>)";

	char32_t const* s = UR"(<html><head></head><body><label>Carpet: <input type="carpet" name="c" texture="deep pile"></label></body></html>)";

	BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_semantics_8)
{
	char32_t const* in = UR"(<label>Carpet: <input type="text" class="carpet" name="c" data-texture="deep pile"></label>)";

	char32_t const* s = UR"(<html><head></head><body><label>Carpet: <input type="text" class="carpet" name="c" data-texture="deep pile"></label></body></html>)";

	BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_semantics_9)
{
    char32_t const* in = UR"(<template>
 <article>
  <img src="{{src}}" alt="{{alt}}">
  <h1></h1>
 </article>
</template>)";

    // template 内のコンテンツは本来無くなるが、本実装はノードとして挿入する。
    char32_t const* s = UR"(<html><head><template>
 <article>
  <img src="{{src}}" alt="{{alt}}">
  <h1></h1>
 </article>
</template></head><body></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

/*
3.2.2 Elements in the DOM

https://html.spec.whatwg.org/multipage/dom.html#elements-in-the-dom
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_semantics_10)
{
    char32_t const* in = UR"(<figure id="module-script-graph">
  <img src="module-script-graph.svg"
       alt="Module A depends on module B, which depends
            on modules C and D.">
  <figcaption>Figure 27: a simple module graph</figcaption>
</figure>)";

    char32_t const* s = UR"(<html><head></head><body><figure id="module-script-graph">
  <img src="module-script-graph.svg" alt="Module A depends on module B, which depends
            on modules C and D.">
  <figcaption>Figure 27: a simple module graph</figcaption>
</figure></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_semantics_11)
{
    char32_t const* in = UR"(As we can see in <a href="#module-script-graph">figure 27</a>)";

    char32_t const* s = UR"(<html><head></head><body>As we can see in <a href="#module-script-graph">figure 27</a></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

/*
3.2.5.2.1 Metadata content

https://html.spec.whatwg.org/multipage/dom.html#metadata-content
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_semantics_12)
{
    char32_t const* in = UR"(<html xmlns="http://www.w3.org/1999/xhtml"
      xmlns:r="http://www.w3.org/1999/02/22-rdf-syntax-ns#" xml:lang="en">
 <head>
  <title>Hedral's Home Page</title>
  <r:RDF>
   <Person xmlns="http://www.w3.org/2000/10/swap/pim/contact#"
           r:about="https://hedral.example.com/#">
    <fullName>Cat Hedral</fullName>
    <mailbox r:resource="mailto:hedral@damowmow.com"/>
    <personalTitle>Sir</personalTitle>
   </Person>
  </r:RDF>
 </head>
 <body>
  <h1>My home page</h1>
  <p>I like playing with string, I guess. Sister says squirrels are fun
  too so sometimes I follow her to play with them.</p>
 </body>
</html>)";

    char32_t const* s = UR"(<html xmlns="http://www.w3.org/1999/xhtml" xmlns:r="http://www.w3.org/1999/02/22-rdf-syntax-ns#" xml:lang="en"><head>
  <title>Hedral's Home Page</title>
  </head><body><r:rdf>
   <person xmlns="http://www.w3.org/2000/10/swap/pim/contact#" r:about="https://hedral.example.com/#">
    <fullname>Cat Hedral</fullname>
    <mailbox r:resource="mailto:hedral@damowmow.com">
    <personaltitle>Sir</personaltitle>
   </mailbox></person>
  </r:rdf>
 
 
  <h1>My home page</h1>
  <p>I like playing with string, I guess. Sister says squirrels are fun
  too so sometimes I follow her to play with them.</p>
 
</body></html>)";

    BOOST_CHECK(parse(in) == s);
}

/*
3.2.5.3 Transparent content models

https://html.spec.whatwg.org/multipage/dom.html#transparent-content-models
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_semantics_13)
{
    char32_t const* in = UR"(<p><object><param><ins><map><a href="/">Apples</a></map></ins></object></p>)";

    char32_t const* s = UR"(<html><head></head><body><p><object><param><ins><map><a href="/">Apples</a></map></ins></object></p></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

/*
3.2.5.4 Paragraphs

https://html.spec.whatwg.org/multipage/dom.html#paragraphs
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_semantics_14)
{
    char32_t const* in = UR"(<section>
  <h1>Example of paragraphs</h1>
  This is the <em>first</em> paragraph in this example.
  <p>This is the second.</p>
  <!-- This is not a paragraph. -->
</section>)";

    char32_t const* s = UR"(<html><head></head><body><section>
  <h1>Example of paragraphs</h1>
  This is the <em>first</em> paragraph in this example.
  <p>This is the second.</p>
  <!-- This is not a paragraph. -->
</section></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_semantics_15)
{
    char32_t const* in = UR"(<section>
  <ins><h1>Example of paragraphs</h1>
  This is the <em>first</em> paragraph in</ins> this example<del>.
  <p>This is the second.</p></del>
  <!-- This is not a paragraph. -->
</section>)";

    char32_t const* s = UR"(<html><head></head><body><section>
  <ins><h1>Example of paragraphs</h1>
  This is the <em>first</em> paragraph in</ins> this example<del>.
  <p>This is the second.</p></del>
  <!-- This is not a paragraph. -->
</section></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_semantics_16)
{
    char32_t const* in = UR"(<header>
 Welcome!
 <a href="about.html">
  This is home of...
  <h1>The Falcons!</h1>
  The Lockheed Martin multirole jet fighter aircraft!
 </a>
 This page discusses the F-16 Fighting Falcon's innermost secrets.
</header>)";

    char32_t const* s = UR"(<html><head></head><body><header>
 Welcome!
 <a href="about.html">
  This is home of...
  <h1>The Falcons!</h1>
  The Lockheed Martin multirole jet fighter aircraft!
 </a>
 This page discusses the F-16 Fighting Falcon's innermost secrets.
</header></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_semantics_17)
{
    char32_t const* in = UR"(<header>
 <p>Welcome! <a href="about.html">This is home of...</a></p>
 <h1><a href="about.html">The Falcons!</a></h1>
 <p><a href="about.html">The Lockheed Martin multirole jet
 fighter aircraft!</a> This page discusses the F-16 Fighting
 Falcon's innermost secrets.</p>
</header>)";

    char32_t const* s = UR"(<html><head></head><body><header>
 <p>Welcome! <a href="about.html">This is home of...</a></p>
 <h1><a href="about.html">The Falcons!</a></h1>
 <p><a href="about.html">The Lockheed Martin multirole jet
 fighter aircraft!</a> This page discusses the F-16 Fighting
 Falcon's innermost secrets.</p>
</header></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_semantics_18)
{
    char32_t const* in = UR"(<section>
 <h1>My Cats</h1>
 You can play with my cat simulator.
 <object data="cats.sim">
  To see the cat simulator, use one of the following links:
  <ul>
   <li><a href="cats.sim">Download simulator file</a>
   <li><a href="https://sims.example.com/watch?v=LYds5xY4INU">Use online simulator</a>
  </ul>
  Alternatively, upgrade to the Mellblom Browser.
 </object>
 I'm quite proud of it.
</section>)";

    char32_t const* s = UR"(<html><head></head><body><section>
 <h1>My Cats</h1>
 You can play with my cat simulator.
 <object data="cats.sim">
  To see the cat simulator, use one of the following links:
  <ul>
   <li><a href="cats.sim">Download simulator file</a>
   </li><li><a href="https://sims.example.com/watch?v=LYds5xY4INU">Use online simulator</a>
  </li></ul>
  Alternatively, upgrade to the Mellblom Browser.
 </object>
 I'm quite proud of it.
</section></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_semantics_19)
{
    char32_t const* in = UR"(<section>
 <h1>My Cats</h1>
 <p>You can play with my cat simulator.</p>
 <object data="cats.sim">
  <p>To see the cat simulator, use one of the following links:</p>
  <ul>
   <li><a href="cats.sim">Download simulator file</a>
   <li><a href="https://sims.example.com/watch?v=LYds5xY4INU">Use online simulator</a>
  </ul>
  <p>Alternatively, upgrade to the Mellblom Browser.</p>
 </object>
 <p>I'm quite proud of it.</p>
</section>)";

    char32_t const* s = UR"(<html><head></head><body><section>
 <h1>My Cats</h1>
 <p>You can play with my cat simulator.</p>
 <object data="cats.sim">
  <p>To see the cat simulator, use one of the following links:</p>
  <ul>
   <li><a href="cats.sim">Download simulator file</a>
   </li><li><a href="https://sims.example.com/watch?v=LYds5xY4INU">Use online simulator</a>
  </li></ul>
  <p>Alternatively, upgrade to the Mellblom Browser.</p>
 </object>
 <p>I'm quite proud of it.</p>
</section></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

/*
3.2.6 Global attributes

https://html.spec.whatwg.org/multipage/dom.html#global-attributes
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_semantics_20)
{
    char32_t const* in = UR"(<div xmlns="http://www.w3.org/1999/xhtml" dir="rtl">
 <bogus xmlns="https://example.net/ns" dir="ltr">
  <span xmlns="http://www.w3.org/1999/xhtml">
  </span>
 </bogus>
</div>)";

    char32_t const* s = UR"(<html><head></head><body><div xmlns="http://www.w3.org/1999/xhtml" dir="rtl">
 <bogus xmlns="https://example.net/ns" dir="ltr">
  <span xmlns="http://www.w3.org/1999/xhtml">
  </span>
 </bogus>
</div></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

/*
3.2.6.1 The title attribute

https://html.spec.whatwg.org/multipage/dom.html#the-title-attribute
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_semantics_21)
{
    char32_t const* in = UR"(<p>My logs show that there was some interest in <abbr title="Hypertext
Transport Protocol">HTTP</abbr> today.</p>)";

    char32_t const* s = UR"(<html><head></head><body><p>My logs show that there was some interest in <abbr title="Hypertext
Transport Protocol">HTTP</abbr> today.</p></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

/*
3.2.6.3 The translate attribute

https://html.spec.whatwg.org/multipage/dom.html#the-translate-attribute
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_semantics_22)
{
    char32_t const* in = UR"(<!DOCTYPE HTML>
<html lang=en> <!-- default on the document element is translate=yes -->
 <head>
  <title>The Bee Game</title> <!-- implied translate=yes inherited from ancestors -->
 </head>
 <body>
  <p>The Bee Game is a text adventure game in English.</p>
  <p>When the game launches, the first thing you should do is type
  <kbd translate=no>eat honey</kbd>. The game will respond with:</p>
  <pre><samp translate=no>Yum yum! That was some good honey!</samp></pre>
 </body>
</html>)";

    char32_t const* s = UR"(<!DOCTYPE html><html lang="en"><!-- default on the document element is translate=yes --><head>
  <title>The Bee Game</title> <!-- implied translate=yes inherited from ancestors -->
 </head>
 <body>
  <p>The Bee Game is a text adventure game in English.</p>
  <p>When the game launches, the first thing you should do is type
  <kbd translate="no">eat honey</kbd>. The game will respond with:</p>
  <pre><samp translate="no">Yum yum! That was some good honey!</samp></pre>
 
</body></html>)";

    BOOST_CHECK(parse(in) == s);
}

/*
3.2.6.4 The dir attribute

https://html.spec.whatwg.org/multipage/dom.html#the-dir-attribute
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_semantics_23)
{
    char32_t const* in = UR"(<p dir=auto class="u1"><b><bdi>Student</bdi>:</b> How do you write "What's your name?" in Arabic?</p>
<p dir=auto class="u2"><b><bdi>Teacher</bdi>:</b> ما اسمك؟</p>
<p dir=auto class="u1"><b><bdi>Student</bdi>:</b> Thanks.</p>
<p dir=auto class="u2"><b><bdi>Teacher</bdi>:</b> That's written "شكرًا".</p>
<p dir=auto class="u2"><b><bdi>Teacher</bdi>:</b> Do you know how to write "Please"?</p>
<p dir=auto class="u1"><b><bdi>Student</bdi>:</b> "من فضلك", right?</p>)";

    char32_t const* s = UR"(<html><head></head><body><p dir="auto" class="u1"><b><bdi>Student</bdi>:</b> How do you write "What's your name?" in Arabic?</p>
<p dir="auto" class="u2"><b><bdi>Teacher</bdi>:</b> ما اسمك؟</p>
<p dir="auto" class="u1"><b><bdi>Student</bdi>:</b> Thanks.</p>
<p dir="auto" class="u2"><b><bdi>Teacher</bdi>:</b> That's written "شكرًا".</p>
<p dir="auto" class="u2"><b><bdi>Teacher</bdi>:</b> Do you know how to write "Please"?</p>
<p dir="auto" class="u1"><b><bdi>Student</bdi>:</b> "من فضلك", right?</p></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

/*
3.2.6.5 The style attribute

https://html.spec.whatwg.org/multipage/dom.html#the-style-attribute
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_semantics_24)
{
    char32_t const* in = UR"(<p>My sweat suit is <span style="color: green; background:
transparent">green</span> and my eyes are <span style="color: blue;
background: transparent">blue</span>.</p>)";

    char32_t const* s = UR"(<html><head></head><body><p>My sweat suit is <span style="color: green; background:
transparent">green</span> and my eyes are <span style="color: blue;
background: transparent">blue</span>.</p></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_semantics_25)
{
    char32_t const* in = UR"(<ol>
 <li data-length="2m11s">Beyond The Sea</li>
 ...
</ol>)";

    char32_t const* s = UR"(<html><head></head><body><ol>
 <li data-length="2m11s">Beyond The Sea</li>
 ...
</ol></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_semantics_26)
{
    char32_t const* in = UR"(<p>The third <span data-mytrans-de="Anspruch">claim</span> covers the case of <span
translate="no">HTML</span> markup.</p>)";

    char32_t const* s = UR"(<html><head></head><body><p>The third <span data-mytrans-de="Anspruch">claim</span> covers the case of <span translate="no">HTML</span> markup.</p></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_semantics_27)
{
    char32_t const* in = UR"(<script>
 if ('PaymentRequest' in window) {
   document.documentElement.dataset.hasPaymentRequest = '';
 }
</script>)";

    char32_t const* s = UR"(<html><head><script>
 if ('PaymentRequest' in window) {
   document.documentElement.dataset.hasPaymentRequest = '';
 }
</script></head><body></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_semantics_28)
{
    char32_t const* in = UR"*(<div class="spaceship" data-ship-id="92432"
     data-weapons="laser 2" data-shields="50%"
     data-x="30" data-y="10" data-z="90">
 <button class="fire"
         onclick="spaceships[this.parentNode.dataset.shipId].fire()">
  Fire
 </button>
</div>)*";

    char32_t const* s = UR"*(<html><head></head><body><div class="spaceship" data-ship-id="92432" data-weapons="laser 2" data-shields="50%" data-x="30" data-y="10" data-z="90">
 <button class="fire" onclick="spaceships[this.parentNode.dataset.shipId].fire()">
  Fire
 </button>
</div></body></html>)*";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_semantics_29)
{
    char32_t const* in = UR"(<img class="tower" id="tower5" data-x="12" data-y="5"
     data-ai="robotarget" data-hp="46" data-ability="flames"
     src="towers/rocket.png" alt="Rocket Tower">)";

    char32_t const* s = UR"(<html><head></head><body><img class="tower" id="tower5" data-x="12" data-y="5" data-ai="robotarget" data-hp="46" data-ability="flames" src="towers/rocket.png" alt="Rocket Tower"></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_SUITE_END()
