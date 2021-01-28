// test/html/simple_html_sample_links.cpp

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

		std::cout << wordring::encoding_cast<std::u8string>(out);

		return out;
	}
}

BOOST_AUTO_TEST_SUITE(simple_html_sample_links_test)

/*
4.6.6.1 Link type "alternate"

https://html.spec.whatwg.org/multipage/links.html#rel-alternate
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_links_1)
{
	char32_t const* in = UR"(<!-- a persistent style sheet -->
<link rel="stylesheet" href="default.css">

<!-- the preferred alternate style sheet -->
<link rel="stylesheet" href="green.css" title="Green styles">

<!-- some alternate style sheets -->
<link rel="alternate stylesheet" href="contrast.css" title="High contrast">
<link rel="alternate stylesheet" href="big.css" title="Big fonts">
<link rel="alternate stylesheet" href="wide.css" title="Wide screen">)";

	char32_t const* s = UR"(<!-- a persistent style sheet --><html><head><link rel="stylesheet" href="default.css">

<!-- the preferred alternate style sheet -->
<link rel="stylesheet" href="green.css" title="Green styles">

<!-- some alternate style sheets -->
<link rel="alternate stylesheet" href="contrast.css" title="High contrast">
<link rel="alternate stylesheet" href="big.css" title="Big fonts">
<link rel="alternate stylesheet" href="wide.css" title="Wide screen"></head><body></body></html>)";

	BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_links_2)
{
	char32_t const* in = UR"(<link rel="alternate" type="application/atom+xml" href="posts.xml" title="Cool Stuff Blog">
<link rel="alternate" type="application/atom+xml" href="posts.xml?category=robots" title="Cool Stuff Blog: robots category">
<link rel="alternate" type="application/atom+xml" href="comments.xml" title="Cool Stuff Blog: Comments">
<p>You can access the planets database using Atom feeds:</p>
<ul>
 <li><a href="recently-visited-planets.xml" rel="alternate" type="application/atom+xml">Recently Visited Planets</a></li>
 <li><a href="known-bad-planets.xml" rel="alternate" type="application/atom+xml">Known Bad Planets</a></li>
 <li><a href="unexplored-planets.xml" rel="alternate" type="application/atom+xml">Unexplored Planets</a></li>
</ul>)";

	char32_t const* s = UR"(<html><head><link rel="alternate" type="application/atom+xml" href="posts.xml" title="Cool Stuff Blog">
<link rel="alternate" type="application/atom+xml" href="posts.xml?category=robots" title="Cool Stuff Blog: robots category">
<link rel="alternate" type="application/atom+xml" href="comments.xml" title="Cool Stuff Blog: Comments">
</head><body><p>You can access the planets database using Atom feeds:</p>
<ul>
 <li><a href="recently-visited-planets.xml" rel="alternate" type="application/atom+xml">Recently Visited Planets</a></li>
 <li><a href="known-bad-planets.xml" rel="alternate" type="application/atom+xml">Known Bad Planets</a></li>
 <li><a href="unexplored-planets.xml" rel="alternate" type="application/atom+xml">Unexplored Planets</a></li>
</ul></body></html>)";

	BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_links_3)
{
	char32_t const* in = UR"*(<link rel=alternate href="/en/html" hreflang=en type=text/html title="English HTML">
<link rel=alternate href="/fr/html" hreflang=fr type=text/html title="French HTML">
<link rel=alternate href="/en/html/print" hreflang=en type=text/html media=print title="English HTML (for printing)">
<link rel=alternate href="/fr/html/print" hreflang=fr type=text/html media=print title="French HTML (for printing)">
<link rel=alternate href="/en/pdf" hreflang=en type=application/pdf title="English PDF">
<link rel=alternate href="/fr/pdf" hreflang=fr type=application/pdf title="French PDF">)*";

	char32_t const* s = UR"*(<html><head><link rel="alternate" href="/en/html" hreflang="en" type="text/html" title="English HTML">
<link rel="alternate" href="/fr/html" hreflang="fr" type="text/html" title="French HTML">
<link rel="alternate" href="/en/html/print" hreflang="en" type="text/html" media="print" title="English HTML (for printing)">
<link rel="alternate" href="/fr/html/print" hreflang="fr" type="text/html" media="print" title="French HTML (for printing)">
<link rel="alternate" href="/en/pdf" hreflang="en" type="application/pdf" title="English PDF">
<link rel="alternate" href="/fr/pdf" hreflang="fr" type="application/pdf" title="French PDF"></head><body></body></html>)*";

	BOOST_CHECK(parse(in) == s);
}

/*
4.6.6.3 Link type "bookmark"

https://html.spec.whatwg.org/multipage/links.html#link-type-bookmark
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_links_4)
{
	char32_t const* in = UR"(...
 <body>
  <h1>Example of permalinks</h1>
  <div id="a">
   <h2>First example</h2>
   <p><a href="a.html" rel="bookmark">This permalink applies to
   only the content from the first H2 to the second H2</a>. The DIV isn't
   exactly that section, but it roughly corresponds to it.</p>
  </div>
  <h2>Second example</h2>
  <article id="b">
   <p><a href="b.html" rel="bookmark">This permalink applies to
   the outer ARTICLE element</a> (which could be, e.g., a blog post).</p>
   <article id="c">
    <p><a href="c.html" rel="bookmark">This permalink applies to
    the inner ARTICLE element</a> (which could be, e.g., a blog comment).</p>
   </article>
  </article>
 </body>
 ...)";

	char32_t const* s = UR"(<html><head></head><body>...
 
  <h1>Example of permalinks</h1>
  <div id="a">
   <h2>First example</h2>
   <p><a href="a.html" rel="bookmark">This permalink applies to
   only the content from the first H2 to the second H2</a>. The DIV isn't
   exactly that section, but it roughly corresponds to it.</p>
  </div>
  <h2>Second example</h2>
  <article id="b">
   <p><a href="b.html" rel="bookmark">This permalink applies to
   the outer ARTICLE element</a> (which could be, e.g., a blog post).</p>
   <article id="c">
    <p><a href="c.html" rel="bookmark">This permalink applies to
    the inner ARTICLE element</a> (which could be, e.g., a blog comment).</p>
   </article>
  </article>
 
 ...</body></html>)";

	BOOST_CHECK(parse(in) == s);
}

/*
4.6.6.7 Link type "help"

https://html.spec.whatwg.org/multipage/links.html#link-type-help
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_links_5)
{
	char32_t const* in = UR"(<p><label> Topic: <input name=topic> <a href="help/topic.html" rel="help">(Help)</a></label></p>)";

	char32_t const* s = UR"(<html><head></head><body><p><label> Topic: <input name="topic"> <a href="help/topic.html" rel="help">(Help)</a></label></p></body></html>)";

	BOOST_CHECK(parse(in) == s);
}

/*
4.6.6.8 Link type "icon"

https://html.spec.whatwg.org/multipage/links.html#rel-icon
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_links_6)
{
	char32_t const* in = UR"(<!DOCTYPE HTML>
<html lang="en">
 <head>
  <title>lsForums — Inbox</title>
  <link rel=icon href=favicon.png sizes="16x16" type="image/png">
  <link rel=icon href=windows.ico sizes="32x32 48x48" type="image/vnd.microsoft.icon">
  <link rel=icon href=mac.icns sizes="128x128 512x512 8192x8192 32768x32768">
  <link rel=icon href=iphone.png sizes="57x57" type="image/png">
  <link rel=icon href=gnome.svg sizes="any" type="image/svg+xml">
  <link rel=stylesheet href=lsforums.css>
  <script src=lsforums.js></script>
  <meta name=application-name content="lsForums">
 </head>
 <body>
  ...)";

	char32_t const* s = UR"(<!DOCTYPE html><html lang="en"><head>
  <title>lsForums — Inbox</title>
  <link rel="icon" href="favicon.png" sizes="16x16" type="image/png">
  <link rel="icon" href="windows.ico" sizes="32x32 48x48" type="image/vnd.microsoft.icon">
  <link rel="icon" href="mac.icns" sizes="128x128 512x512 8192x8192 32768x32768">
  <link rel="icon" href="iphone.png" sizes="57x57" type="image/png">
  <link rel="icon" href="gnome.svg" sizes="any" type="image/svg+xml">
  <link rel="stylesheet" href="lsforums.css">
  <script src="lsforums.js"></script>
  <meta name="application-name" content="lsForums">
 </head>
 <body>
  ...</body></html>)";

	BOOST_CHECK(parse(in) == s);
}

/*
4.6.6.9 Link type "license"

https://html.spec.whatwg.org/multipage/links.html#link-type-license
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_links_7)
{
	char32_t const* in = UR"(<!DOCTYPE HTML>
<html lang="en">
 <head>
  <title>Exampl Pictures: Kissat</title>
  <link rel="stylesheet" href="/style/default">
 </head>
 <body>
  <h1>Kissat</h1>
  <nav>
   <a href="../">Return to photo index</a>
  </nav>
  <figure>
   <img src="/pix/39627052_fd8dcd98b5.jpg">
   <figcaption>Kissat</figcaption>
  </figure>
  <p>One of them has six toes!</p>
  <p><small><a rel="license" href="http://www.opensource.org/licenses/mit-license.php">MIT Licensed</a></small></p>
  <footer>
   <a href="/">Home</a> | <a href="../">Photo index</a>
   <p><small>© copyright 2009 Exampl Pictures. All Rights Reserved.</small></p>
  </footer>
 </body>
</html>)";

	char32_t const* s = UR"(<!DOCTYPE html><html lang="en"><head>
  <title>Exampl Pictures: Kissat</title>
  <link rel="stylesheet" href="/style/default">
 </head>
 <body>
  <h1>Kissat</h1>
  <nav>
   <a href="../">Return to photo index</a>
  </nav>
  <figure>
   <img src="/pix/39627052_fd8dcd98b5.jpg">
   <figcaption>Kissat</figcaption>
  </figure>
  <p>One of them has six toes!</p>
  <p><small><a rel="license" href="http://www.opensource.org/licenses/mit-license.php">MIT Licensed</a></small></p>
  <footer>
   <a href="/">Home</a> | <a href="../">Photo index</a>
   <p><small>© copyright 2009 Exampl Pictures. All Rights Reserved.</small></p>
  </footer>
 
</body></html>)";

	BOOST_CHECK(parse(in) == s);
}

/*
4.6.6.10 Link type "modulepreload"

https://html.spec.whatwg.org/multipage/links.html#link-type-modulepreload
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_links_8)
{
	char32_t const* in = UR"(<!DOCTYPE html>
<html lang="en">
<title>IRCFog</title>

<link rel="modulepreload" href="app.mjs">
<link rel="modulepreload" href="helpers.mjs">
<link rel="modulepreload" href="irc.mjs">
<link rel="modulepreload" href="fog-machine.mjs">

<script type="module" src="app.mjs">
...)";

	char32_t const* s = UR"(<!DOCTYPE html><html lang="en"><head><title>IRCFog</title>

<link rel="modulepreload" href="app.mjs">
<link rel="modulepreload" href="helpers.mjs">
<link rel="modulepreload" href="irc.mjs">
<link rel="modulepreload" href="fog-machine.mjs">

<script type="module" src="app.mjs">
...</script></head><body></body></html>)";

	BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_links_9)
{
	char32_t const* in = UR"*(<link rel="modulepreload" href="awesome-viewer.mjs">

<button onclick="import('./awesome-viewer.mjs').then(m => m.view())">
  View awesome thing
</button>)*";

	char32_t const* s = UR"*(<html><head><link rel="modulepreload" href="awesome-viewer.mjs">

</head><body><button onclick="import('./awesome-viewer.mjs').then(m => m.view())">
  View awesome thing
</button></body></html>)*";

	BOOST_CHECK(parse(in) == s);
}

/*
4.6.6.12 Link type "noopener"

https://html.spec.whatwg.org/multipage/links.html#link-type-noopener
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_links_10)
{
	char32_t const* in = UR"(<a href=help.html target=example>Help!</a>
<a href=help.html target=example rel=noopener>Help!</a>
<a href=index.html target=_parent>Home</a>
<a href=index.html target=_parent rel=noopener>Home</a>)";

	char32_t const* s = UR"(<html><head></head><body><a href="help.html" target="example">Help!</a>
<a href="help.html" target="example" rel="noopener">Help!</a>
<a href="index.html" target="_parent">Home</a>
<a href="index.html" target="_parent" rel="noopener">Home</a></body></html>)";

	BOOST_CHECK(parse(in) == s);
}

/*
4.6.6.13 Link type "noreferrer"

https://html.spec.whatwg.org/multipage/links.html#link-type-noreferrer
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_links_11)
{
	char32_t const* in = UR"(<a href="..." rel="noreferrer" target="_blank"> has the same behavior as <a href="..." rel="noreferrer noopener" target="_blank">.)";

	char32_t const* s = UR"(<html><head></head><body><a href="..." rel="noreferrer" target="_blank"> has the same behavior as </a><a href="..." rel="noreferrer noopener" target="_blank">.</a></body></html>)";

	BOOST_CHECK(parse(in) == s);
}

/*
4.6.6.14 Link type "opener"

https://html.spec.whatwg.org/multipage/links.html#link-type-opener
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_links_12)
{
	char32_t const* in = UR"(<a href="..." rel=opener target=_blank>Help!</a>)";

	char32_t const* s = UR"(<html><head></head><body><a href="..." rel="opener" target="_blank">Help!</a></body></html>)";

	BOOST_CHECK(parse(in) == s);
}

/*
4.6.6.22 Link type "tag"

https://html.spec.whatwg.org/multipage/links.html#link-type-tag
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_links_13)
{
	char32_t const* in = UR"(<!DOCTYPE HTML>
<html lang="en">
 <head>
  <title>My Precious</title>
 </head>
 <body>
  <header><h1>My precious</h1> <p>Summer 2012</p></header>
  <p>Recently I managed to dispose of a red gem that had been
  bothering me. I now have a much nicer blue sapphire.</p>
  <p>The red gem had been found in a bauxite stone while I was digging
  out the office level, but nobody was willing to haul it away. The
  same red gem stayed there for literally years.</p>
  <footer>
   Tags: <a rel=tag href="https://en.wikipedia.org/wiki/Gemstone">Gemstone</a>
  </footer>
 </body>
</html>)";

	char32_t const* s = UR"(<!DOCTYPE html><html lang="en"><head>
  <title>My Precious</title>
 </head>
 <body>
  <header><h1>My precious</h1> <p>Summer 2012</p></header>
  <p>Recently I managed to dispose of a red gem that had been
  bothering me. I now have a much nicer blue sapphire.</p>
  <p>The red gem had been found in a bauxite stone while I was digging
  out the office level, but nobody was willing to haul it away. The
  same red gem stayed there for literally years.</p>
  <footer>
   Tags: <a rel="tag" href="https://en.wikipedia.org/wiki/Gemstone">Gemstone</a>
  </footer>
 
</body></html>)";

	BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_links_14)
{
	char32_t const* in = UR"(<!DOCTYPE HTML>
<html lang="en">
 <head>
  <title>Gem 4/4</title>
 </head>
 <body>
  <article>
   <h1>801: Steinbock</h1>
   <p>The number 801 Gem 4/4 electro-diesel has an ibex and was rebuilt in 2002.</p>
  </article>
  <article>
   <h1>802: Murmeltier</h1>
   <figure>
    <img src="https://upload.wikimedia.org/wikipedia/commons/b/b0/Trains_de_la_Bernina_en_hiver_2.jpg"
         alt="The 802 was red with pantographs and tall vents on the side.">
    <figcaption>The 802 in the 1980s, above Lago Bianco.</figcaption>
   </figure>
   <p>The number 802 Gem 4/4 electro-diesel has a marmot and was rebuilt in 2003.</p>
  </article>
  <p class="topic"><a rel=tag href="https://en.wikipedia.org/wiki/Rhaetian_Railway_Gem_4/4">Gem 4/4</a></p>
 </body>
</html>)";

	char32_t const* s = UR"(<!DOCTYPE html><html lang="en"><head>
  <title>Gem 4/4</title>
 </head>
 <body>
  <article>
   <h1>801: Steinbock</h1>
   <p>The number 801 Gem 4/4 electro-diesel has an ibex and was rebuilt in 2002.</p>
  </article>
  <article>
   <h1>802: Murmeltier</h1>
   <figure>
    <img src="https://upload.wikimedia.org/wikipedia/commons/b/b0/Trains_de_la_Bernina_en_hiver_2.jpg" alt="The 802 was red with pantographs and tall vents on the side.">
    <figcaption>The 802 in the 1980s, above Lago Bianco.</figcaption>
   </figure>
   <p>The number 802 Gem 4/4 electro-diesel has a marmot and was rebuilt in 2003.</p>
  </article>
  <p class="topic"><a rel="tag" href="https://en.wikipedia.org/wiki/Rhaetian_Railway_Gem_4/4">Gem 4/4</a></p>
 
</body></html>)";

	BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_SUITE_END()
