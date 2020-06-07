// test/html/simple_html_sample_text.cpp

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

BOOST_AUTO_TEST_SUITE(simple_html_sample_text_test)

/*
4.5.1 The a element

https://html.spec.whatwg.org/multipage/text-level-semantics.html#the-a-element
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_text_1)
{
	char32_t const* in = UR"(<nav>
 <ul>
  <li> <a href="/">Home</a> </li>
  <li> <a href="/news">News</a> </li>
  <li> <a>Examples</a> </li>
  <li> <a href="/legal">Legal</a> </li>
 </ul>
</nav>)";

	char32_t const* s = UR"(<html><head></head><body><nav>
 <ul>
  <li> <a href="/">Home</a> </li>
  <li> <a href="/news">News</a> </li>
  <li> <a>Examples</a> </li>
  <li> <a href="/legal">Legal</a> </li>
 </ul>
</nav></body></html>)";

	BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_text_2)
{
	char32_t const* in = UR"(<aside class="advertising">
 <h1>Advertising</h1>
 <a href="https://ad.example.com/?adid=1929&amp;pubid=1422">
  <section>
   <h1>Mellblomatic 9000!</h1>
   <p>Turn all your widgets into mellbloms!</p>
   <p>Only $9.99 plus shipping and handling.</p>
  </section>
 </a>
 <a href="https://ad.example.com/?adid=375&amp;pubid=1422">
  <section>
   <h1>The Mellblom Browser</h1>
   <p>Web browsing at the speed of light.</p>
   <p>No other browser goes faster!</p>
  </section>
 </a>
</aside>)";

	char32_t const* s = UR"(<html><head></head><body><aside class="advertising">
 <h1>Advertising</h1>
 <a href="https://ad.example.com/?adid=1929&amp;pubid=1422">
  <section>
   <h1>Mellblomatic 9000!</h1>
   <p>Turn all your widgets into mellbloms!</p>
   <p>Only $9.99 plus shipping and handling.</p>
  </section>
 </a>
 <a href="https://ad.example.com/?adid=375&amp;pubid=1422">
  <section>
   <h1>The Mellblom Browser</h1>
   <p>Web browsing at the speed of light.</p>
   <p>No other browser goes faster!</p>
  </section>
 </a>
</aside></body></html>)";

	BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_text_3)
{
	char32_t const* in = UR"(<table>
 <tr>
  <th>Position
  <th>Team
  <th>Location
 <tr>
  <td><a href="/jobs/manager">Manager</a>
  <td>Remotees
  <td>Remote
 <tr>
  <td><a href="/jobs/director">Director</a>
  <td>Remotees
  <td>Remote
 <tr>
  <td><a href="/jobs/astronaut">Astronaut</a>
  <td>Architecture
  <td>Remote
</table>
<script>
document.querySelector("table").onclick = ({ target }) => {
  if (target.parentElement.localName === "tr") {
    const link = target.parentElement.querySelector("a");
    if (link) {
      link.click();
    }
  }
}
</script>)";

	char32_t const* s = UR"(<html><head></head><body><table>
 <tbody><tr>
  <th>Position
  </th><th>Team
  </th><th>Location
 </th></tr><tr>
  <td><a href="/jobs/manager">Manager</a>
  </td><td>Remotees
  </td><td>Remote
 </td></tr><tr>
  <td><a href="/jobs/director">Director</a>
  </td><td>Remotees
  </td><td>Remote
 </td></tr><tr>
  <td><a href="/jobs/astronaut">Astronaut</a>
  </td><td>Architecture
  </td><td>Remote
</td></tr></tbody></table>
<script>
document.querySelector("table").onclick = ({ target }) => {
  if (target.parentElement.localName === "tr") {
    const link = target.parentElement.querySelector("a");
    if (link) {
      link.click();
    }
  }
}
</script></body></html>)";

	BOOST_CHECK(parse(in) == s);
}

/*
4.5.2 The em element

https://html.spec.whatwg.org/multipage/text-level-semantics.html#the-em-element
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_text_4)
{
	char32_t const* in = UR"(<p>Cats are cute animals.</p>)";

	char32_t const* s = UR"(<html><head></head><body><p>Cats are cute animals.</p></body></html>)";

	BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_text_5)
{
	char32_t const* in = UR"(<p>Cats are cute animals.</p>)";

	char32_t const* s = UR"(<html><head></head><body><p>Cats are cute animals.</p></body></html>)";

	BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_text_6)
{
	char32_t const* in = UR"(<p><em>Cats</em> are cute animals.</p>)";

	char32_t const* s = UR"(<html><head></head><body><p><em>Cats</em> are cute animals.</p></body></html>)";

	BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_text_7)
{
	char32_t const* in = UR"(<p>Cats <em>are</em> cute animals.</p>)";

	char32_t const* s = UR"(<html><head></head><body><p>Cats <em>are</em> cute animals.</p></body></html>)";

	BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_text_8)
{
	char32_t const* in = UR"(<p>Cats are <em>cute</em> animals.</p>)";

	char32_t const* s = UR"(<html><head></head><body><p>Cats are <em>cute</em> animals.</p></body></html>)";

	BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_text_9)
{
	char32_t const* in = UR"(<p>Cats are cute <em>animals</em>.</p>)";

	char32_t const* s = UR"(<html><head></head><body><p>Cats are cute <em>animals</em>.</p></body></html>)";

	BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_text_10)
{
	char32_t const* in = UR"(<p><em>Cats are cute animals!</em></p>)";

	char32_t const* s = UR"(<html><head></head><body><p><em>Cats are cute animals!</em></p></body></html>)";

	BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_text_11)
{
	char32_t const* in = UR"(<p><em>Cats are <em>cute</em> animals!</em></p>)";

	char32_t const* s = UR"(<html><head></head><body><p><em>Cats are <em>cute</em> animals!</em></p></body></html>)";

	BOOST_CHECK(parse(in) == s);
}

/*
4.5.3 The strong element

https://html.spec.whatwg.org/multipage/text-level-semantics.html#the-strong-element
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_text_12)
{
	char32_t const* in = UR"(<h1>Chapter 1: <strong>The Praxis</strong></h1>)";

	char32_t const* s = UR"(<html><head></head><body><h1>Chapter 1: <strong>The Praxis</strong></h1></body></html>)";

	BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_text_13)
{
	char32_t const* in = UR"(<figcaption>Figure 1. <strong>Ant colony dynamics</strong>. The ants in this colony are
affected by the heat source (upper left) and the food source (lower right).</figcaption>)";

	char32_t const* s = UR"(<html><head></head><body><figcaption>Figure 1. <strong>Ant colony dynamics</strong>. The ants in this colony are
affected by the heat source (upper left) and the food source (lower right).</figcaption></body></html>)";

	BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_text_14)
{
	char32_t const* in = UR"(<h1><strong>Flowers, Bees, and Honey</strong> and other things I don't understand</h1>)";

	char32_t const* s = UR"(<html><head></head><body><h1><strong>Flowers, Bees, and Honey</strong> and other things I don't understand</h1></body></html>)";

	BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_text_15)
{
	char32_t const* in = UR"(<p><strong>Warning.</strong> This dungeon is dangerous.
<strong>Avoid the ducks.</strong> Take any gold you find.
<strong><strong>Do not take any of the diamonds</strong>,
they are explosive and <strong>will destroy anything within
ten meters.</strong></strong> You have been warned.</p>
)";

	char32_t const* s = UR"(<html><head></head><body><p><strong>Warning.</strong> This dungeon is dangerous.
<strong>Avoid the ducks.</strong> Take any gold you find.
<strong><strong>Do not take any of the diamonds</strong>,
they are explosive and <strong>will destroy anything within
ten meters.</strong></strong> You have been warned.</p>
</body></html>)";

	BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_text_16)
{
	char32_t const* in = UR"(<p>Welcome to Remy, the reminder system.</p>
<p>Your tasks for today:</p>
<ul>
 <li><p><strong>Turn off the oven.</strong></p></li>
 <li><p>Put out the trash.</p></li>
 <li><p>Do the laundry.</p></li>
</ul>)";

	char32_t const* s = UR"(<html><head></head><body><p>Welcome to Remy, the reminder system.</p>
<p>Your tasks for today:</p>
<ul>
 <li><p><strong>Turn off the oven.</strong></p></li>
 <li><p>Put out the trash.</p></li>
 <li><p>Do the laundry.</p></li>
</ul></body></html>)";

	BOOST_CHECK(parse(in) == s);
}

/*
4.5.4 The small element

https://html.spec.whatwg.org/multipage/text-level-semantics.html#the-small-element
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_text_17)
{
	char32_t const* in = UR"(<dl>
 <dt>Single room
 <dd>199 € <small>breakfast included, VAT not included</small>
 <dt>Double room
 <dd>239 € <small>breakfast included, VAT not included</small>
</dl>)";

	char32_t const* s = UR"(<html><head></head><body><dl>
 <dt>Single room
 </dt><dd>199 € <small>breakfast included, VAT not included</small>
 </dd><dt>Double room
 </dt><dd>239 € <small>breakfast included, VAT not included</small>
</dd></dl></body></html>)";

	BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_text_18)
{
	char32_t const* in = UR"(<p>Example Corp today announced record profits for the
second quarter <small>(Full Disclosure: Foo News is a subsidiary of
Example Corp)</small>, leading to speculation about a third quarter
merger with Demo Group.</p>)";

	char32_t const* s = UR"(<html><head></head><body><p>Example Corp today announced record profits for the
second quarter <small>(Full Disclosure: Foo News is a subsidiary of
Example Corp)</small>, leading to speculation about a third quarter
merger with Demo Group.</p></body></html>)";

	BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_text_19)
{
	char32_t const* in = UR"(<aside>
 <h1>Example Corp</h1>
 <p>This company mostly creates small software and Web
 sites.</p>
 <p>The Example Corp company mission is "To provide entertainment
 and news on a sample basis".</p>
 <p><small>Information obtained from <a
 href="https://example.com/about.html">example.com</a> home
 page.</small></p>
</aside>)";

	char32_t const* s = UR"(<html><head></head><body><aside>
 <h1>Example Corp</h1>
 <p>This company mostly creates small software and Web
 sites.</p>
 <p>The Example Corp company mission is "To provide entertainment
 and news on a sample basis".</p>
 <p><small>Information obtained from <a href="https://example.com/about.html">example.com</a> home
 page.</small></p>
</aside></body></html>)";

	BOOST_CHECK(parse(in) == s);
}

/*
4.5.5 The s element

https://html.spec.whatwg.org/multipage/text-level-semantics.html#the-s-element
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_text_20)
{
	char32_t const* in = UR"(<p><strong><small>Continued use of this service will result in a kiss.</small></strong></p>)";

	char32_t const* s = UR"(<html><head></head><body><p><strong><small>Continued use of this service will result in a kiss.</small></strong></p></body></html>)";

	BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_text_21)
{
	char32_t const* in = UR"(<p>Buy our Iced Tea and Lemonade!</p>
<p><s>Recommended retail price: $3.99 per bottle</s></p>
<p><strong>Now selling for just $2.99 a bottle!</strong></p>)";

	char32_t const* s = UR"(<html><head></head><body><p>Buy our Iced Tea and Lemonade!</p>
<p><s>Recommended retail price: $3.99 per bottle</s></p>
<p><strong>Now selling for just $2.99 a bottle!</strong></p></body></html>)";

	BOOST_CHECK(parse(in) == s);
}

/*
4.5.6 The cite element

https://html.spec.whatwg.org/multipage/text-level-semantics.html#the-cite-element
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_text_22)
{
	char32_t const* in = UR"(<p>My favorite book is <cite>The Reality Dysfunction</cite> by
Peter F. Hamilton. My favorite comic is <cite>Pearls Before
Swine</cite> by Stephan Pastis. My favorite track is <cite>Jive
Samba</cite> by the Cannonball Adderley Sextet.</p>)";

	char32_t const* s = UR"(<html><head></head><body><p>My favorite book is <cite>The Reality Dysfunction</cite> by
Peter F. Hamilton. My favorite comic is <cite>Pearls Before
Swine</cite> by Stephan Pastis. My favorite track is <cite>Jive
Samba</cite> by the Cannonball Adderley Sextet.</p></body></html>)";

	BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_text_23)
{
	char32_t const* in = UR"(<p>According to the Wikipedia article <cite>HTML</cite>, as it
stood in mid-February 2008, leaving attribute values unquoted is
unsafe. This is obviously an over-simplification.</p>
<!-- do not copy this example, it is an example of bad usage! -->
<p>According to <cite>the Wikipedia article on HTML</cite>, as it
stood in mid-February 2008, leaving attribute values unquoted is
unsafe. This is obviously an over-simplification.</p>)";

	char32_t const* s = UR"(<html><head></head><body><p>According to the Wikipedia article <cite>HTML</cite>, as it
stood in mid-February 2008, leaving attribute values unquoted is
unsafe. This is obviously an over-simplification.</p>
<!-- do not copy this example, it is an example of bad usage! -->
<p>According to <cite>the Wikipedia article on HTML</cite>, as it
stood in mid-February 2008, leaving attribute values unquoted is
unsafe. This is obviously an over-simplification.</p></body></html>)";

	BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_text_24)
{
	char32_t const* in = UR"(<p><cite>Universal Declaration of Human Rights</cite>, United Nations,
December 1948. Adopted by General Assembly resolution 217 A (III).</p>)";

	char32_t const* s = UR"(<html><head></head><body><p><cite>Universal Declaration of Human Rights</cite>, United Nations,
December 1948. Adopted by General Assembly resolution 217 A (III).</p></body></html>)";

	BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_text_25)
{
	char32_t const* in = UR"(<p><cite>This is wrong!</cite>, said Ian.</p>
<p><q>This is still wrong!</q>, said <cite>Ian</cite>.</p>
<p><q>This is correct</q>, said Ian.</p>
<p>And then <b>Ian</b> said <q>this might be right, in a
gossip column, maybe!</q>.</p>)";

	char32_t const* s = UR"(<html><head></head><body><p><cite>This is wrong!</cite>, said Ian.</p>
<p><q>This is still wrong!</q>, said <cite>Ian</cite>.</p>
<p><q>This is correct</q>, said Ian.</p>
<p>And then <b>Ian</b> said <q>this might be right, in a
gossip column, maybe!</q>.</p></body></html>)";

	BOOST_CHECK(parse(in) == s);
}

/*
4.5.7 The q element

https://html.spec.whatwg.org/multipage/text-level-semantics.html#the-q-element
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_text_26)
{
	char32_t const* in = UR"(<p>The man said <q>Things that are impossible just take
longer</q>. I disagreed with him.</p>)";

	char32_t const* s = UR"(<html><head></head><body><p>The man said <q>Things that are impossible just take
longer</q>. I disagreed with him.</p></body></html>)";

	BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_text_27)
{
	char32_t const* in = UR"(<p>The W3C page <cite>About W3C</cite> says the W3C's
mission is <q cite="https://www.w3.org/Consortium/">To lead the
World Wide Web to its full potential by developing protocols and
guidelines that ensure long-term growth for the Web</q>. I
disagree with this mission.</p>)";

	char32_t const* s = UR"(<html><head></head><body><p>The W3C page <cite>About W3C</cite> says the W3C's
mission is <q cite="https://www.w3.org/Consortium/">To lead the
World Wide Web to its full potential by developing protocols and
guidelines that ensure long-term growth for the Web</q>. I
disagree with this mission.</p></body></html>)";

	BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_text_28)
{
	char32_t const* in = UR"(<p>In <cite>Example One</cite>, he writes <q>The man
said <q>Things that are impossible just take longer</q>. I
disagreed with him</q>. Well, I disagree even more!</p>)";

	char32_t const* s = UR"(<html><head></head><body><p>In <cite>Example One</cite>, he writes <q>The man
said <q>Things that are impossible just take longer</q>. I
disagreed with him</q>. Well, I disagree even more!</p></body></html>)";

	BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_text_29)
{
    char32_t const* in = UR"(<p>His best argument was ❝I disagree❞, which
I thought was laughable.</p>)";

    char32_t const* s = UR"(<html><head></head><body><p>His best argument was ❝I disagree❞, which
I thought was laughable.</p></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_text_30)
{
    char32_t const* in = UR"(<p>The word "ineffable" could have been used to describe the disaster
resulting from the campaign's mismanagement.</p>)";

    char32_t const* s = UR"(<html><head></head><body><p>The word "ineffable" could have been used to describe the disaster
resulting from the campaign's mismanagement.</p></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

/*
4.5.8 The dfn element

https://html.spec.whatwg.org/multipage/text-level-semantics.html#the-dfn-element
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_text_31)
{
    char32_t const* in = UR"(<p>The <dfn><abbr title="Garage Door Opener">GDO</abbr></dfn>
is a device that allows off-world teams to open the iris.</p>
<!-- ... later in the document: -->
<p>Teal'c activated his <abbr title="Garage Door Opener">GDO</abbr>
and so Hammond ordered the iris to be opened.</p>
<p>The <dfn id=gdo><abbr title="Garage Door Opener">GDO</abbr></dfn>
is a device that allows off-world teams to open the iris.</p>
<!-- ... later in the document: -->
<p>Teal'c activated his <a href=#gdo><abbr title="Garage Door Opener">GDO</abbr></a>
and so Hammond ordered the iris to be opened.</p>)";

    char32_t const* s = UR"(<html><head></head><body><p>The <dfn><abbr title="Garage Door Opener">GDO</abbr></dfn>
is a device that allows off-world teams to open the iris.</p>
<!-- ... later in the document: -->
<p>Teal'c activated his <abbr title="Garage Door Opener">GDO</abbr>
and so Hammond ordered the iris to be opened.</p>
<p>The <dfn id="gdo"><abbr title="Garage Door Opener">GDO</abbr></dfn>
is a device that allows off-world teams to open the iris.</p>
<!-- ... later in the document: -->
<p>Teal'c activated his <a href="#gdo"><abbr title="Garage Door Opener">GDO</abbr></a>
and so Hammond ordered the iris to be opened.</p></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

/*
4.5.9 The abbr element

https://html.spec.whatwg.org/multipage/text-level-semantics.html#the-abbr-element
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_text_32)
{
    char32_t const* in = UR"(<p>The <dfn id=whatwg><abbr
title="Web Hypertext Application Technology Working Group">WHATWG</abbr></dfn>
is a loose unofficial collaboration of Web browser manufacturers and
interested parties who wish to develop new technologies designed to
allow authors to write and deploy Applications over the World Wide
Web.</p>
<p>The <dfn id=whatwg>Web Hypertext Application Technology
Working Group</dfn> (<abbr
title="Web Hypertext Application Technology Working Group">WHATWG</abbr>)
is a loose unofficial collaboration of Web browser manufacturers and
interested parties who wish to develop new technologies designed to
allow authors to write and deploy Applications over the World Wide
Web.</p>)";

    char32_t const* s = UR"(<html><head></head><body><p>The <dfn id="whatwg"><abbr title="Web Hypertext Application Technology Working Group">WHATWG</abbr></dfn>
is a loose unofficial collaboration of Web browser manufacturers and
interested parties who wish to develop new technologies designed to
allow authors to write and deploy Applications over the World Wide
Web.</p>
<p>The <dfn id="whatwg">Web Hypertext Application Technology
Working Group</dfn> (<abbr title="Web Hypertext Application Technology Working Group">WHATWG</abbr>)
is a loose unofficial collaboration of Web browser manufacturers and
interested parties who wish to develop new technologies designed to
allow authors to write and deploy Applications over the World Wide
Web.</p></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_text_33)
{
    char32_t const* in = UR"(<p>The
<abbr title="Web Hypertext Application Technology Working Group">WHATWG</abbr>
started working on HTML5 in 2004.</p>)";

    char32_t const* s = UR"(<html><head></head><body><p>The
<abbr title="Web Hypertext Application Technology Working Group">WHATWG</abbr>
started working on HTML5 in 2004.</p></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_text_34)
{
    char32_t const* in = UR"(<p>The <a href="#whatwg"><abbr
title="Web Hypertext Application Technology Working Group">WHATWG</abbr></a>
community does not have much representation from Asia.</p>)";

    char32_t const* s = UR"(<html><head></head><body><p>The <a href="#whatwg"><abbr title="Web Hypertext Application Technology Working Group">WHATWG</abbr></a>
community does not have much representation from Asia.</p></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_text_35)
{
    char32_t const* in = UR"(<p>Philip` and Dashiva both denied that they were going to
get the issue counts from past revisions of the specification to
backfill the <abbr>WHATWG</abbr> issue graph.</p>)";

    char32_t const* s = UR"(<html><head></head><body><p>Philip` and Dashiva both denied that they were going to
get the issue counts from past revisions of the specification to
backfill the <abbr>WHATWG</abbr> issue graph.</p></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_text_36)
{
    char32_t const* in = UR"(<p>Two <abbr title="Working Group">WG</abbr>s worked on
this specification: the <abbr>WHATWG</abbr> and the
<abbr>HTMLWG</abbr>.</p>
<p>Two <abbr title="Working Groups">WGs</abbr> worked on
this specification: the <abbr>WHATWG</abbr> and the
<abbr>HTMLWG</abbr>.</p>)";

    char32_t const* s = UR"(<html><head></head><body><p>Two <abbr title="Working Group">WG</abbr>s worked on
this specification: the <abbr>WHATWG</abbr> and the
<abbr>HTMLWG</abbr>.</p>
<p>Two <abbr title="Working Groups">WGs</abbr> worked on
this specification: the <abbr>WHATWG</abbr> and the
<abbr>HTMLWG</abbr>.</p></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

/*
4.5.10 The ruby element

https://html.spec.whatwg.org/multipage/text-level-semantics.html#the-ruby-element
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_text_37)
{
    char32_t const* in = UR"(<ruby>B<rt>annotation</ruby>)";

    char32_t const* s = UR"(<html><head></head><body><ruby>B<rt>annotation</rt></ruby></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_text_38)
{
    char32_t const* in = UR"(<ruby>君<rt>くん</ruby><ruby>子<rt>し</ruby>は<ruby>和<rt>わ</ruby>して<ruby>同<rt>どう</ruby>ぜず。
<ruby>君<rt>くん</rt>子<rt>し</ruby>は<ruby>和<rt>わ</ruby>して<ruby>同<rt>どう</ruby>ぜず。)";

    char32_t const* s = UR"(<html><head></head><body><ruby>君<rt>くん</rt></ruby><ruby>子<rt>し</rt></ruby>は<ruby>和<rt>わ</rt></ruby>して<ruby>同<rt>どう</rt></ruby>ぜず。
<ruby>君<rt>くん</rt>子<rt>し</rt></ruby>は<ruby>和<rt>わ</rt></ruby>して<ruby>同<rt>どう</rt></ruby>ぜず。</body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_text_39)
{
    char32_t const* in = UR"(<ruby>B<rt>annotation</rt>B<rt>annotation</ruby>)";

    char32_t const* s = UR"(<html><head></head><body><ruby>B<rt>annotation</rt>B<rt>annotation</rt></ruby></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_text_40)
{
    char32_t const* in = UR"(<ruby>鬼<rt>き</rt>門<rt>もん</rt></ruby>の<ruby>方<rt>ほう</rt>角<rt>がく</rt></ruby>を<ruby>凝<rt>ぎょう</rt>視<rt>し</rt></ruby>する)";

    char32_t const* s = UR"(<html><head></head><body><ruby>鬼<rt>き</rt>門<rt>もん</rt></ruby>の<ruby>方<rt>ほう</rt>角<rt>がく</rt></ruby>を<ruby>凝<rt>ぎょう</rt>視<rt>し</rt></ruby>する</body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_text_41)
{
    char32_t const* in = UR"(<ruby>鬼<rt>き</rt>門<rt>もん</rt></ruby>の<ruby>方<rt>ほう</rt>角<rt>がく</rt></ruby>を<ruby>凝<rt>ぎょう</rt>視<rt>し</rt></ruby>する)";

    char32_t const* s = UR"(<html><head></head><body><ruby>鬼<rt>き</rt>門<rt>もん</rt></ruby>の<ruby>方<rt>ほう</rt>角<rt>がく</rt></ruby>を<ruby>凝<rt>ぎょう</rt>視<rt>し</rt></ruby>する</body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_text_42)
{
    char32_t const* in = UR"(<ruby>BASE<rt>annotation</ruby>)";

    char32_t const* s = UR"(<html><head></head><body><ruby>BASE<rt>annotation</rt></ruby></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_text_43)
{
    char32_t const* in = UR"(<ruby>境界面<rt>インターフェース</ruby>)";

    char32_t const* s = UR"(<html><head></head><body><ruby>境界面<rt>インターフェース</rt></ruby></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_text_44)
{
    char32_t const* in = UR"(<ruby lang="ja">編集者<rt lang="en">editor</ruby>)";

    char32_t const* s = UR"(<html><head></head><body><ruby lang="ja">編集者<rt lang="en">editor</rt></ruby></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_text_45)
{
    char32_t const* in = UR"(<ruby>紫陽花<rt>あじさい</ruby>)";

    char32_t const* s = UR"(<html><head></head><body><ruby>紫陽花<rt>あじさい</rt></ruby></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_text_46)
{
    char32_t const* in = UR"(<ruby>BASE<rt>annotation 1<rt>annotation 2</ruby>)";

    char32_t const* s = UR"(<html><head></head><body><ruby>BASE<rt>annotation 1</rt><rt>annotation 2</rt></ruby></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_text_47)
{
    char32_t const* in = UR"(<ruby>B<rt>a<rt>a</ruby><ruby>A<rt>a<rt>a</ruby><ruby>S<rt>a<rt>a</ruby><ruby>E<rt>a<rt>a</ruby>)";

    char32_t const* s = UR"(<html><head></head><body><ruby>B<rt>a</rt><rt>a</rt></ruby><ruby>A<rt>a</rt><rt>a</rt></ruby><ruby>S<rt>a</rt><rt>a</rt></ruby><ruby>E<rt>a</rt><rt>a</rt></ruby></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_text_48)
{
    char32_t const* in = UR"(<ruby>
 ♥ <rt> Heart <rt lang=fr> Cœur </rt>
 ☘ <rt> Shamrock <rt lang=fr> Trèfle </rt>
 ✶ <rt> Star <rt lang=fr> Étoile </rt>
</ruby>)";

    char32_t const* s = UR"(<html><head></head><body><ruby>
 ♥ <rt> Heart </rt><rt lang="fr"> Cœur </rt>
 ☘ <rt> Shamrock </rt><rt lang="fr"> Trèfle </rt>
 ✶ <rt> Star </rt><rt lang="fr"> Étoile </rt>
</ruby></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_text_49)
{
    char32_t const* in = UR"(<ruby><ruby>東<rt>とう</rt>南<rt>なん</rt></ruby><rt>たつみ</rt></ruby>の方角)";

    char32_t const* s = UR"(<html><head></head><body><ruby><ruby>東<rt>とう</rt>南<rt>なん</rt></ruby><rt>たつみ</rt></ruby>の方角</body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_text_50)
{
    char32_t const* in = UR"(<ruby><ruby>東<rt>とう</rt>南<rt>なん</rt></ruby><rt lang=en>Southeast</rt></ruby>の方角)";

    char32_t const* s = UR"(<html><head></head><body><ruby><ruby>東<rt>とう</rt>南<rt>なん</rt></ruby><rt lang="en">Southeast</rt></ruby>の方角</body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_text_51)
{
    char32_t const* in = UR"(...
<ruby>漢<rt>かん</rt>字<rt>じ</rt></ruby>
...)";

    char32_t const* s = UR"(<html><head></head><body>...
<ruby>漢<rt>かん</rt>字<rt>じ</rt></ruby>
...</body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_text_52)
{
    char32_t const* in = UR"(<ruby>漢<rt>ㄏㄢˋ</rt>字<rt>ㄗˋ</rt></ruby>)";

    char32_t const* s = UR"(<html><head></head><body><ruby>漢<rt>ㄏㄢˋ</rt>字<rt>ㄗˋ</rt></ruby></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_text_53)
{
    char32_t const* in = UR"(...<ruby>汉<rt>hàn</rt>字<rt>zì</rt></ruby>...)";

    char32_t const* s = UR"(<html><head></head><body>...<ruby>汉<rt>hàn</rt>字<rt>zì</rt></ruby>...</body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_text_54)
{
    char32_t const* in = UR"(<ruby>
 <ruby>HT<rt>Hypertext</rt>M<rt>Markup</rt>L<rt>Language</rt></ruby>
 <rt>An abstract language for describing documents and applications
</ruby>)";

    char32_t const* s = UR"(<html><head></head><body><ruby>
 <ruby>HT<rt>Hypertext</rt>M<rt>Markup</rt>L<rt>Language</rt></ruby>
 <rt>An abstract language for describing documents and applications
</rt></ruby></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

/*
4.5.12 The rp element

https://html.spec.whatwg.org/multipage/text-level-semantics.html#the-rp-element
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_text_55)
{
    char32_t const* in = UR"(...
<ruby>漢<rp>（</rp><rt>かん</rt><rp>）</rp>字<rp>（</rp><rt>じ</rt><rp>）</rp></ruby>
...)";

    char32_t const* s = UR"(<html><head></head><body>...
<ruby>漢<rp>（</rp><rt>かん</rt><rp>）</rp>字<rp>（</rp><rt>じ</rt><rp>）</rp></ruby>
...</body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_text_56)
{
    char32_t const* in = UR"(<ruby>
♥<rp>: </rp><rt>Heart</rt><rp>, </rp><rt lang=fr>Cœur</rt><rp>.</rp>
☘<rp>: </rp><rt>Shamrock</rt><rp>, </rp><rt lang=fr>Trèfle</rt><rp>.</rp>
✶<rp>: </rp><rt>Star</rt><rp>, </rp><rt lang=fr>Étoile</rt><rp>.</rp>
</ruby>)";

    char32_t const* s = UR"(<html><head></head><body><ruby>
♥<rp>: </rp><rt>Heart</rt><rp>, </rp><rt lang="fr">Cœur</rt><rp>.</rp>
☘<rp>: </rp><rt>Shamrock</rt><rp>, </rp><rt lang="fr">Trèfle</rt><rp>.</rp>
✶<rp>: </rp><rt>Star</rt><rp>, </rp><rt lang="fr">Étoile</rt><rp>.</rp>
</ruby></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

/*
4.5.13 The data element

https://html.spec.whatwg.org/multipage/text-level-semantics.html#the-data-element
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_text_57)
{
    char32_t const* in = UR"(<script src="sortable.js"></script>
<table class="sortable">
 <thead> <tr> <th> Game <th> Corporations <th> Map Size
 <tbody>
  <tr> <td> 1830 <td> <data value="8">Eight</data> <td> <data value="93">19+74 hexes (93 total)</data>
  <tr> <td> 1856 <td> <data value="11">Eleven</data> <td> <data value="99">12+87 hexes (99 total)</data>
  <tr> <td> 1870 <td> <data value="10">Ten</data> <td> <data value="149">4+145 hexes (149 total)</data>
</table>)";

    char32_t const* s = UR"(<html><head><script src="sortable.js"></script>
</head><body><table class="sortable">
 <thead> <tr> <th> Game </th><th> Corporations </th><th> Map Size
 </th></tr></thead><tbody>
  <tr> <td> 1830 </td><td> <data value="8">Eight</data> </td><td> <data value="93">19+74 hexes (93 total)</data>
  </td></tr><tr> <td> 1856 </td><td> <data value="11">Eleven</data> </td><td> <data value="99">12+87 hexes (99 total)</data>
  </td></tr><tr> <td> 1870 </td><td> <data value="10">Ten</data> </td><td> <data value="149">4+145 hexes (149 total)</data>
</td></tr></tbody></table></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

/*
4.5.14 The time element

https://html.spec.whatwg.org/multipage/text-level-semantics.html#the-time-element
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_text_58)
{
    char32_t const* in = UR"(<time>2011-11</time>)";

    char32_t const* s = UR"(<html><head></head><body><time>2011-11</time></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_text_59)
{
    char32_t const* in = UR"(<div class="vevent">
 <a class="url" href="http://www.web2con.com/">http://www.web2con.com/</a>
 <span class="summary">Web 2.0 Conference</span>:
 <time class="dtstart" datetime="2005-10-05">October 5</time> -
 <time class="dtend" datetime="2005-10-07">7</time>,
 at the <span class="location">Argent Hotel, San Francisco, CA</span>
</div>)";

    char32_t const* s = UR"(<html><head></head><body><div class="vevent">
 <a class="url" href="http://www.web2con.com/">http://www.web2con.com/</a>
 <span class="summary">Web 2.0 Conference</span>:
 <time class="dtstart" datetime="2005-10-05">October 5</time> -
 <time class="dtend" datetime="2005-10-07">7</time>,
 at the <span class="location">Argent Hotel, San Francisco, CA</span>
</div></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_text_60)
{
    char32_t const* in = UR"(<article itemscope itemtype="https://n.example.org/rfc4287">
 <h1 itemprop="title">Big tasks</h1>
 <footer>Published <time itemprop="published" datetime="2009-08-29">two days ago</time>.</footer>
 <p itemprop="content">Today, I went out and bought a bike for my kid.</p>
</article>)";

    char32_t const* s = UR"(<html><head></head><body><article itemscope="" itemtype="https://n.example.org/rfc4287">
 <h1 itemprop="title">Big tasks</h1>
 <footer>Published <time itemprop="published" datetime="2009-08-29">two days ago</time>.</footer>
 <p itemprop="content">Today, I went out and bought a bike for my kid.</p>
</article></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_text_61)
{
    char32_t const* in = UR"(<article itemscope itemtype="http://schema.org/BlogPosting">
 <h1 itemprop="headline">Small tasks</h1>
 <footer>Published <time itemprop="datePublished" datetime="2009-08-30">yesterday</time>.</footer>
 <p itemprop="articleBody">I put a bike bell on her bike.</p>
</article>)";

    char32_t const* s = UR"(<html><head></head><body><article itemscope="" itemtype="http://schema.org/BlogPosting">
 <h1 itemprop="headline">Small tasks</h1>
 <footer>Published <time itemprop="datePublished" datetime="2009-08-30">yesterday</time>.</footer>
 <p itemprop="articleBody">I put a bike bell on her bike.</p>
</article></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_text_62)
{
    char32_t const* in = UR"(<p>Our first date was <time datetime="2006-09-23">a Saturday</time>.</p>
<p>We stopped talking at <time datetime="2006-09-24T05:00-07:00">5am the next morning</time>.</p>)";

    char32_t const* s = UR"(<html><head></head><body><p>Our first date was <time datetime="2006-09-23">a Saturday</time>.</p>
<p>We stopped talking at <time datetime="2006-09-24T05:00-07:00">5am the next morning</time>.</p></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_text_63)
{
    char32_t const* in = UR"(Today is <time datetime="2011-11-18">Friday</time>.)";

    char32_t const* s = UR"(<html><head></head><body>Today is <time datetime="2011-11-18">Friday</time>.</body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_text_64)
{
    char32_t const* in = UR"(Your next meeting is at <time datetime="2011-11-18T15:00-08:00">3pm</time>.)";

    char32_t const* s = UR"(<html><head></head><body>Your next meeting is at <time datetime="2011-11-18T15:00-08:00">3pm</time>.</body></html>)";

    BOOST_CHECK(parse(in) == s);
}

/*
4.5.15 The code element

https://html.spec.whatwg.org/multipage/text-level-semantics.html#the-code-element
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_text_65)
{
    char32_t const* in = UR"(<p>The <code>code</code> element represents a fragment of computer
code.</p>

<p>When you call the <code>activate()</code> method on the
<code>robotSnowman</code> object, the eyes glow.</p>

<p>The example below uses the <code>begin</code> keyword to indicate
the start of a statement block. It is paired with an <code>end</code>
keyword, which is followed by the <code>.</code> punctuation character
(full stop) to indicate the end of the program.</p>)";

    char32_t const* s = UR"(<html><head></head><body><p>The <code>code</code> element represents a fragment of computer
code.</p>

<p>When you call the <code>activate()</code> method on the
<code>robotSnowman</code> object, the eyes glow.</p>

<p>The example below uses the <code>begin</code> keyword to indicate
the start of a statement block. It is paired with an <code>end</code>
keyword, which is followed by the <code>.</code> punctuation character
(full stop) to indicate the end of the program.</p></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_text_66)
{
    char32_t const* in = UR"(<pre><code class="language-pascal">var i: Integer;
begin
   i := 1;
end.</code></pre>)";

    char32_t const* s = UR"(<html><head></head><body><pre><code class="language-pascal">var i: Integer;
begin
   i := 1;
end.</code></pre></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

/*
4.5.16 The var element

https://html.spec.whatwg.org/multipage/text-level-semantics.html#the-var-element
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_text_67)
{
    char32_t const* in = UR"(<p>If there are <var>n</var> pipes leading to the ice
cream factory then I expect at <em>least</em> <var>n</var>
flavors of ice cream to be available for purchase!</p>)";

    char32_t const* s = UR"(<html><head></head><body><p>If there are <var>n</var> pipes leading to the ice
cream factory then I expect at <em>least</em> <var>n</var>
flavors of ice cream to be available for purchase!</p></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_text_68)
{
    char32_t const* in = UR"(<figure>
 <math>
  <mi>a</mi>
  <mo>=</mo>
  <msqrt>
   <msup><mi>b</mi><mn>2</mn></msup>
   <mi>+</mi>
   <msup><mi>c</mi><mn>2</mn></msup>
  </msqrt>
 </math>
 <figcaption>
  Using Pythagoras' theorem to solve for the hypotenuse <var>a</var> of
  a triangle with sides <var>b</var> and <var>c</var>
 </figcaption>
</figure>)";

    char32_t const* s = UR"(<html><head></head><body><figure>
 <math>
  <mi>a</mi>
  <mo>=</mo>
  <msqrt>
   <msup><mi>b</mi><mn>2</mn></msup>
   <mi>+</mi>
   <msup><mi>c</mi><mn>2</mn></msup>
  </msqrt>
 </math>
 <figcaption>
  Using Pythagoras' theorem to solve for the hypotenuse <var>a</var> of
  a triangle with sides <var>b</var> and <var>c</var>
 </figcaption>
</figure></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_text_69)
{
    char32_t const* in = UR"(<p>Then she turned to the blackboard and picked up the chalk. After a few moment's
thought, she wrote <var>E</var> = <var>m</var> <var>c</var><sup>2</sup>. The teacher
looked pleased.</p>)";

    char32_t const* s = UR"(<html><head></head><body><p>Then she turned to the blackboard and picked up the chalk. After a few moment's
thought, she wrote <var>E</var> = <var>m</var> <var>c</var><sup>2</sup>. The teacher
looked pleased.</p></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

/*
4.5.17 The samp element

https://html.spec.whatwg.org/multipage/text-level-semantics.html#the-samp-element
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_text_70)
{
    char32_t const* in = UR"(<p>The computer said <samp>Too much cheese in tray
two</samp> but I didn't know what that meant.</p>)";

    char32_t const* s = UR"(<html><head></head><body><p>The computer said <samp>Too much cheese in tray
two</samp> but I didn't know what that meant.</p></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_text_71)
{
    char32_t const* in = UR"(<pre><samp><span class="prompt">jdoe@mowmow:~$</span> <kbd>ssh demo.example.com</kbd>
Last login: Tue Apr 12 09:10:17 2005 from mowmow.example.com on pts/1
Linux demo 2.6.10-grsec+gg3+e+fhs6b+nfs+gr0501+++p3+c4a+gr2b-reslog-v6.189 #1 SMP Tue Feb 1 11:22:36 PST 2005 i686 unknown

<span class="prompt">jdoe@demo:~$</span> <span class="cursor">_</span></samp></pre>)";

    char32_t const* s = UR"(<html><head></head><body><pre><samp><span class="prompt">jdoe@mowmow:~$</span> <kbd>ssh demo.example.com</kbd>
Last login: Tue Apr 12 09:10:17 2005 from mowmow.example.com on pts/1
Linux demo 2.6.10-grsec+gg3+e+fhs6b+nfs+gr0501+++p3+c4a+gr2b-reslog-v6.189 #1 SMP Tue Feb 1 11:22:36 PST 2005 i686 unknown

<span class="prompt">jdoe@demo:~$</span> <span class="cursor">_</span></samp></pre></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_text_72)
{
    char32_t const* in = UR"(<pre>
<code class="language-javascript">console.log(2.3 + 2.4)</code>
<samp>4.699999999999999</samp>
</pre>)";

    char32_t const* s = UR"(<html><head></head><body><pre><code class="language-javascript">console.log(2.3 + 2.4)</code>
<samp>4.699999999999999</samp>
</pre></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

/*
4.5.18 The kbd element

https://html.spec.whatwg.org/multipage/text-level-semantics.html#the-kbd-element
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_text_73)
{
    char32_t const* in = UR"(<p>To make George eat an apple, press <kbd><kbd>Shift</kbd>+<kbd>F3</kbd></kbd></p>
<p>To make George eat an apple, select
    <kbd><kbd><samp>File</samp></kbd>|<kbd><samp>Eat Apple...</samp></kbd></kbd>
</p>
<p>To make George eat an apple, select <kbd>File | Eat Apple...</kbd></p>)";

    char32_t const* s = UR"(<html><head></head><body><p>To make George eat an apple, press <kbd><kbd>Shift</kbd>+<kbd>F3</kbd></kbd></p>
<p>To make George eat an apple, select
    <kbd><kbd><samp>File</samp></kbd>|<kbd><samp>Eat Apple...</samp></kbd></kbd>
</p>
<p>To make George eat an apple, select <kbd>File | Eat Apple...</kbd></p></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

/*
4.5.19 The sub and sup elements

https://html.spec.whatwg.org/multipage/text-level-semantics.html#the-sub-and-sup-elements
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_text_74)
{
    char32_t const* in = UR"(<p>Their names are
<span lang="fr"><abbr>M<sup>lle</sup></abbr> Gwendoline</span> and
<span lang="fr"><abbr>M<sup>me</sup></abbr> Denise</span>.</p>)";

    char32_t const* s = UR"(<html><head></head><body><p>Their names are
<span lang="fr"><abbr>M<sup>lle</sup></abbr> Gwendoline</span> and
<span lang="fr"><abbr>M<sup>me</sup></abbr> Denise</span>.</p></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_text_75)
{
    char32_t const* in = UR"(<p>The coordinate of the <var>i</var>th point is
(<var>x<sub><var>i</var></sub></var>, <var>y<sub><var>i</var></sub></var>).
For example, the 10th point has coordinate
(<var>x<sub>10</sub></var>, <var>y<sub>10</sub></var>).</p>)";

    char32_t const* s = UR"(<html><head></head><body><p>The coordinate of the <var>i</var>th point is
(<var>x<sub><var>i</var></sub></var>, <var>y<sub><var>i</var></sub></var>).
For example, the 10th point has coordinate
(<var>x<sub>10</sub></var>, <var>y<sub>10</sub></var>).</p></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_text_76)
{
    char32_t const* in = UR"(<var>E</var>=<var>m</var><var>c</var><sup>2</sup>
f(<var>x</var>, <var>n</var>) = log<sub>4</sub><var>x</var><sup><var>n</var></sup>)";

    char32_t const* s = UR"(<html><head></head><body><var>E</var>=<var>m</var><var>c</var><sup>2</sup>
f(<var>x</var>, <var>n</var>) = log<sub>4</sub><var>x</var><sup><var>n</var></sup></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

/*
4.5.20 The i element

https://html.spec.whatwg.org/multipage/text-level-semantics.html#the-i-element
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_text_77)
{
    char32_t const* in = UR"(<p>The <i class="taxonomy">Felis silvestris catus</i> is cute.</p>
<p>The term <i>prose content</i> is defined above.</p>
<p>There is a certain <i lang="fr">je ne sais quoi</i> in the air.</p>
<p>Raymond tried to sleep.</p>
<p><i>The ship sailed away on Thursday</i>, he
dreamt. <i>The ship had many people aboard, including a beautiful
princess called Carey. He watched her, day-in, day-out, hoping she
would notice him, but she never did.</i></p>
<p><i>Finally one night he picked up the courage to speak with
her—</i></p>
<p>Raymond woke with a start as the fire alarm rang out.</p>)";

    char32_t const* s = UR"(<html><head></head><body><p>The <i class="taxonomy">Felis silvestris catus</i> is cute.</p>
<p>The term <i>prose content</i> is defined above.</p>
<p>There is a certain <i lang="fr">je ne sais quoi</i> in the air.</p>
<p>Raymond tried to sleep.</p>
<p><i>The ship sailed away on Thursday</i>, he
dreamt. <i>The ship had many people aboard, including a beautiful
princess called Carey. He watched her, day-in, day-out, hoping she
would notice him, but she never did.</i></p>
<p><i>Finally one night he picked up the courage to speak with
her—</i></p>
<p>Raymond woke with a start as the fire alarm rang out.</p></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

/*
4.5.21 The b element

https://html.spec.whatwg.org/multipage/text-level-semantics.html#the-b-element
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_text_78)
{
    char32_t const* in = UR"(<p>The <b>frobonitor</b> and <b>barbinator</b> components are fried.</p>)";

    char32_t const* s = UR"(<html><head></head><body><p>The <b>frobonitor</b> and <b>barbinator</b> components are fried.</p></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_text_79)
{
    char32_t const* in = UR"(<p>You enter a small room. Your <b>sword</b> glows
brighter. A <b>rat</b> scurries past the corner wall.</p>)";

    char32_t const* s = UR"(<html><head></head><body><p>You enter a small room. Your <b>sword</b> glows
brighter. A <b>rat</b> scurries past the corner wall.</p></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_text_80)
{
    char32_t const* in = UR"(<article>
 <h2>Kittens 'adopted' by pet rabbit</h2>
 <p><b class="lede">Six abandoned kittens have found an
 unexpected new mother figure — a pet rabbit.</b></p>
 <p>Veterinary nurse Melanie Humble took the three-week-old
 kittens to her Aberdeen home.</p>
[...])";

    char32_t const* s = UR"(<html><head></head><body><article>
 <h2>Kittens 'adopted' by pet rabbit</h2>
 <p><b class="lede">Six abandoned kittens have found an
 unexpected new mother figure — a pet rabbit.</b></p>
 <p>Veterinary nurse Melanie Humble took the three-week-old
 kittens to her Aberdeen home.</p>
[...]</article></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_text_81)
{
    char32_t const* in = UR"(<p><b>WARNING!</b> Do not frob the barbinator!</p>)";

    char32_t const* s = UR"(<html><head></head><body><p><b>WARNING!</b> Do not frob the barbinator!</p></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

/*
4.5.22 The u element

https://html.spec.whatwg.org/multipage/text-level-semantics.html#the-u-element
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_text_82)
{
    char32_t const* in = UR"(<p>The <u>see</u> is full of fish.</p>
)";

    char32_t const* s = UR"(<html><head></head><body><p>The <u>see</u> is full of fish.</p>
</body></html>)";

    BOOST_CHECK(parse(in) == s);
}

/*
4.5.23 The mark element

https://html.spec.whatwg.org/multipage/text-level-semantics.html#the-mark-element
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_text_83)
{
    char32_t const* in = UR"(<p lang="en-US">Consider the following quote:</p>
<blockquote lang="en-GB">
 <p>Look around and you will find, no-one's really
 <mark>colour</mark> blind.</p>
</blockquote>
<p lang="en-US">As we can tell from the <em>spelling</em> of the word,
the person writing this quote is clearly not American.</p>)";

    char32_t const* s = UR"(<html><head></head><body><p lang="en-US">Consider the following quote:</p>
<blockquote lang="en-GB">
 <p>Look around and you will find, no-one's really
 <mark>colour</mark> blind.</p>
</blockquote>
<p lang="en-US">As we can tell from the <em>spelling</em> of the word,
the person writing this quote is clearly not American.</p></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_text_84)
{
    char32_t const* in = UR"(<p>I also have some <mark>kitten</mark>s who are visiting me
these days. They're really cute. I think they like my garden! Maybe I
should adopt a <mark>kitten</mark>.</p>)";

    char32_t const* s = UR"(<html><head></head><body><p>I also have some <mark>kitten</mark>s who are visiting me
these days. They're really cute. I think they like my garden! Maybe I
should adopt a <mark>kitten</mark>.</p></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_text_85)
{
    char32_t const* in = UR"(<p>The highlighted part below is where the error lies:</p>
<pre><code>var i: Integer;
begin
   i := <mark>1.1</mark>;
end.</code></pre>
<p>The highlighted part below is where the error lies:</p>
<pre><code><span class=keyword>var</span> <span class=ident>i</span>: <span class=type>Integer</span>;
<span class=keyword>begin</span>
   <span class=ident>i</span> := <span class=literal><mark>1.1</mark></span>;
<span class=keyword>end</span>.</code></pre>)";

    char32_t const* s = UR"(<html><head></head><body><p>The highlighted part below is where the error lies:</p>
<pre><code>var i: Integer;
begin
   i := <mark>1.1</mark>;
end.</code></pre>
<p>The highlighted part below is where the error lies:</p>
<pre><code><span class="keyword">var</span> <span class="ident">i</span>: <span class="type">Integer</span>;
<span class="keyword">begin</span>
   <span class="ident">i</span> := <span class="literal"><mark>1.1</mark></span>;
<span class="keyword">end</span>.</code></pre></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_text_86)
{
    char32_t const* in = UR"(<style>
 blockquote mark, q mark {
   font: inherit; font-style: italic;
   text-decoration: none;
   background: transparent; color: inherit;
 }
 .bubble em {
   font: inherit; font-size: larger;
   text-decoration: underline;
 }
</style>
<article>
 <h1>She knew</h1>
 <p>Did you notice the subtle joke in the joke on panel 4?</p>
 <blockquote>
  <p class="bubble">I didn't <em>want</em> to believe. <mark>Of course
  on some level I realized it was a known-plaintext attack.</mark> But I
  couldn't admit it until I saw for myself.</p>
 </blockquote>
 <p>(Emphasis mine.) I thought that was great. It's so pedantic, yet it
 explains everything neatly.</p>
</article>)";

    char32_t const* s = UR"(<html><head><style>
 blockquote mark, q mark {
   font: inherit; font-style: italic;
   text-decoration: none;
   background: transparent; color: inherit;
 }
 .bubble em {
   font: inherit; font-size: larger;
   text-decoration: underline;
 }
</style>
</head><body><article>
 <h1>She knew</h1>
 <p>Did you notice the subtle joke in the joke on panel 4?</p>
 <blockquote>
  <p class="bubble">I didn't <em>want</em> to believe. <mark>Of course
  on some level I realized it was a known-plaintext attack.</mark> But I
  couldn't admit it until I saw for myself.</p>
 </blockquote>
 <p>(Emphasis mine.) I thought that was great. It's so pedantic, yet it
 explains everything neatly.</p>
</article></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_text_87)
{
    char32_t const* in = UR"(<h3>Wormhole Physics Introduction</h3>

<p><mark>A wormhole in normal conditions can be held open for a
maximum of just under 39 minutes.</mark> Conditions that can increase
the time include a powerful energy source coupled to one or both of
the gates connecting the wormhole, and a large gravity well (such as a
black hole).</p>

<p><mark>Momentum is preserved across the wormhole. Electromagnetic
radiation can travel in both directions through a wormhole,
but matter cannot.</mark></p>

<p>When a wormhole is created, a vortex normally forms.
<strong>Warning: The vortex caused by the wormhole opening will
annihilate anything in its path.</strong> Vortexes can be avoided when
using sufficiently advanced dialing technology.</p>

<p><mark>An obstruction in a gate will prevent it from accepting a
wormhole connection.</mark></p>)";

    char32_t const* s = UR"(<html><head></head><body><h3>Wormhole Physics Introduction</h3>

<p><mark>A wormhole in normal conditions can be held open for a
maximum of just under 39 minutes.</mark> Conditions that can increase
the time include a powerful energy source coupled to one or both of
the gates connecting the wormhole, and a large gravity well (such as a
black hole).</p>

<p><mark>Momentum is preserved across the wormhole. Electromagnetic
radiation can travel in both directions through a wormhole,
but matter cannot.</mark></p>

<p>When a wormhole is created, a vortex normally forms.
<strong>Warning: The vortex caused by the wormhole opening will
annihilate anything in its path.</strong> Vortexes can be avoided when
using sufficiently advanced dialing technology.</p>

<p><mark>An obstruction in a gate will prevent it from accepting a
wormhole connection.</mark></p></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

/*
4.5.24 The bdi element

https://html.spec.whatwg.org/multipage/text-level-semantics.html#the-bdi-element
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_text_88)
{
    char32_t const* in = UR"(<ul>
 <li>User <bdi>jcranmer</bdi>: 12 posts.
 <li>User <bdi>hober</bdi>: 5 posts.
 <li>User <bdi>إيان</bdi>: 3 posts.
</ul>)";

    char32_t const* s = UR"(<html><head></head><body><ul>
 <li>User <bdi>jcranmer</bdi>: 12 posts.
 </li><li>User <bdi>hober</bdi>: 5 posts.
 </li><li>User <bdi>إيان</bdi>: 3 posts.
</li></ul></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

/*
4.5.26 The span element

https://html.spec.whatwg.org/multipage/text-level-semantics.html#the-span-element
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_text_89)
{
    char32_t const* in = UR"(<pre><code class="lang-c"><span class="keyword">for</span> (<span class="ident">j</span> = 0; <span class="ident">j</span> &lt; 256; <span class="ident">j</span>++) {
  <span class="ident">i_t3</span> = (<span class="ident">i_t3</span> & 0x1ffff) | (<span class="ident">j</span> &lt;&lt; 17);
  <span class="ident">i_t6</span> = (((((((<span class="ident">i_t3</span> >> 3) ^ <span class="ident">i_t3</span>) >> 1) ^ <span class="ident">i_t3</span>) >> 8) ^ <span class="ident">i_t3</span>) >> 5) & 0xff;
  <span class="keyword">if</span> (<span class="ident">i_t6</span> == <span class="ident">i_t1</span>)
    <span class="keyword">break</span>;
}</code></pre>)";

    char32_t const* s = UR"(<html><head></head><body><pre><code class="lang-c"><span class="keyword">for</span> (<span class="ident">j</span> = 0; <span class="ident">j</span> &lt; 256; <span class="ident">j</span>++) {
  <span class="ident">i_t3</span> = (<span class="ident">i_t3</span> &amp; 0x1ffff) | (<span class="ident">j</span> &lt;&lt; 17);
  <span class="ident">i_t6</span> = (((((((<span class="ident">i_t3</span> &gt;&gt; 3) ^ <span class="ident">i_t3</span>) &gt;&gt; 1) ^ <span class="ident">i_t3</span>) &gt;&gt; 8) ^ <span class="ident">i_t3</span>) &gt;&gt; 5) &amp; 0xff;
  <span class="keyword">if</span> (<span class="ident">i_t6</span> == <span class="ident">i_t1</span>)
    <span class="keyword">break</span>;
}</code></pre></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

/*
4.5.27 The br element

https://html.spec.whatwg.org/multipage/text-level-semantics.html#the-br-element
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_text_90)
{
    char32_t const* in = UR"(<p>P. Sherman<br>
42 Wallaby Way<br>
Sydney</p>)";

    char32_t const* s = UR"(<html><head></head><body><p>P. Sherman<br>
42 Wallaby Way<br>
Sydney</p></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_text_91)
{
    char32_t const* in = UR"(<p><a ...>34 comments.</a><br>
<a ...>Add a comment.</a></p>
<p><label>Name: <input name="name"></label><br>
<label>Address: <input name="address"></label></p>
<p><a ...>34 comments.</a></p>
<p><a ...>Add a comment.</a></p>
<p><label>Name: <input name="name"></label></p>
<p><label>Address: <input name="address"></label></p>)";

    char32_t const* s = UR"(<html><head></head><body><p><a ...="">34 comments.</a><br>
<a ...="">Add a comment.</a></p>
<p><label>Name: <input name="name"></label><br>
<label>Address: <input name="address"></label></p>
<p><a ...="">34 comments.</a></p>
<p><a ...="">Add a comment.</a></p>
<p><label>Name: <input name="name"></label></p>
<p><label>Address: <input name="address"></label></p></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

/*
4.5.28 The wbr element

https://html.spec.whatwg.org/multipage/text-level-semantics.html#the-wbr-element
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_text_92)
{
    char32_t const* in = UR"(<p>So then she pointed at the tiger and screamed
"there<wbr>is<wbr>no<wbr>way<wbr>you<wbr>are<wbr>ever<wbr>going<wbr>to<wbr>catch<wbr>me"!</p>)";

    char32_t const* s = UR"(<html><head></head><body><p>So then she pointed at the tiger and screamed
"there<wbr>is<wbr>no<wbr>way<wbr>you<wbr>are<wbr>ever<wbr>going<wbr>to<wbr>catch<wbr>me"!</p></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

/*
4.5.29 Usage summary

https://html.spec.whatwg.org/multipage/text-level-semantics.html#usage-summary
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_text_93)
{
    char32_t const* in = UR"(Visit my <a href="drinks.html">drinks</a> page.
I must say I <em>adore</em> lemonade.
This tea is <strong>very hot</strong>.
These grapes are made into wine. <small>Alcohol is addictive.</small>
Price: <s>£4.50</s> £2.00!
The case <cite>Hugo v. Danielle</cite> is relevant here.
The judge said <q>You can drink water from the fish tank</q> but advised against it.
The term <dfn>organic food</dfn> refers to food produced without synthetic chemicals.
Organic food in Ireland is certified by the <abbr title="Irish Organic Farmers and Growers Association">IOFGA</abbr>.
<ruby> OJ <rp>(<rt>Orange Juice<rp>)</ruby>
Available starting today! <data value="UPC:022014640201">North Coast Organic Apple Cider</data>
Available starting on <time datetime="2011-11-18">November 18th</time>!
The <code>fruitdb</code> program can be used for tracking fruit production.
If there are <var>n</var> fruit in the bowl, at least <var>n</var>÷2 will be ripe.
The computer said <samp>Unknown error -3</samp>.
Hit <kbd>F1</kbd> to continue.
Water is H<sub>2</sub>O.
The Hydrogen in heavy water is usually <sup>2</sup>H.
Lemonade consists primarily of <i>Citrus limon</i>.
Take a <b>lemon</b> and squeeze it with a <b>juicer</b>.
The mixture of apple juice and <u class="spelling">eldeflower</u> juice is very pleasant.
Elderflower cordial, with one <mark>part</mark> cordial to ten <mark>part</mark>s water, stands a<mark>part</mark> from the rest.
The recommended restaurant is <bdi lang="">My Juice Café (At The Beach)</bdi>.
The proposal is to write English, but in reverse order. "Juice" would become "<bdo dir=rtl>Juice</bdo>">
In French we call it <span lang="fr">sirop de sureau</span>.
Simply Orange Juice Company<br>Apopka, FL 32703<br>U.S.A.
www.simply<wbr>orange<wbr>juice.com)";

    char32_t const* s = UR"(<html><head></head><body>Visit my <a href="drinks.html">drinks</a> page.
I must say I <em>adore</em> lemonade.
This tea is <strong>very hot</strong>.
These grapes are made into wine. <small>Alcohol is addictive.</small>
Price: <s>£4.50</s> £2.00!
The case <cite>Hugo v. Danielle</cite> is relevant here.
The judge said <q>You can drink water from the fish tank</q> but advised against it.
The term <dfn>organic food</dfn> refers to food produced without synthetic chemicals.
Organic food in Ireland is certified by the <abbr title="Irish Organic Farmers and Growers Association">IOFGA</abbr>.
<ruby> OJ <rp>(</rp><rt>Orange Juice</rt><rp>)</rp></ruby>
Available starting today! <data value="UPC:022014640201">North Coast Organic Apple Cider</data>
Available starting on <time datetime="2011-11-18">November 18th</time>!
The <code>fruitdb</code> program can be used for tracking fruit production.
If there are <var>n</var> fruit in the bowl, at least <var>n</var>÷2 will be ripe.
The computer said <samp>Unknown error -3</samp>.
Hit <kbd>F1</kbd> to continue.
Water is H<sub>2</sub>O.
The Hydrogen in heavy water is usually <sup>2</sup>H.
Lemonade consists primarily of <i>Citrus limon</i>.
Take a <b>lemon</b> and squeeze it with a <b>juicer</b>.
The mixture of apple juice and <u class="spelling">eldeflower</u> juice is very pleasant.
Elderflower cordial, with one <mark>part</mark> cordial to ten <mark>part</mark>s water, stands a<mark>part</mark> from the rest.
The recommended restaurant is <bdi lang="">My Juice Café (At The Beach)</bdi>.
The proposal is to write English, but in reverse order. "Juice" would become "<bdo dir="rtl">Juice</bdo>"&gt;
In French we call it <span lang="fr">sirop de sureau</span>.
Simply Orange Juice Company<br>Apopka, FL 32703<br>U.S.A.
www.simply<wbr>orange<wbr>juice.com</body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_SUITE_END()
