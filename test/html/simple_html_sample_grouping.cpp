// test/html/simple_html_sample_grouping.cpp

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

BOOST_AUTO_TEST_SUITE(simple_html_sample_grouping_test)

/*
4.4.1 The p element

https://html.spec.whatwg.org/multipage/grouping-content.html#the-p-element
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_grouping_1)
{
	char32_t const* in = UR"(<p>The little kitten gently seated herself on a piece of
carpet. Later in her life, this would be referred to as the time the
cat sat on the mat.</p>)";

	char32_t const* s = UR"(<html><head></head><body><p>The little kitten gently seated herself on a piece of
carpet. Later in her life, this would be referred to as the time the
cat sat on the mat.</p></body></html>)";

	BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_grouping_2)
{
	char32_t const* in = UR"(<fieldset>
 <legend>Personal information</legend>
 <p>
   <label>Name: <input name="n"></label>
   <label><input name="anon" type="checkbox"> Hide from other users</label>
 </p>
 <p><label>Address: <textarea name="a"></textarea></label></p>
</fieldset>)";

	char32_t const* s = UR"(<html><head></head><body><fieldset>
 <legend>Personal information</legend>
 <p>
   <label>Name: <input name="n"></label>
   <label><input name="anon" type="checkbox"> Hide from other users</label>
 </p>
 <p><label>Address: <textarea name="a"></textarea></label></p>
</fieldset></body></html>)";

	BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_grouping_3)
{
	char32_t const* in = UR"(<p>There was once an example from Femley,<br>
Whose markup was of dubious quality.<br>
The validator complained,<br>
So the author was pained,<br>
To move the error from the markup to the rhyming.</p>)";

	char32_t const* s = UR"(<html><head></head><body><p>There was once an example from Femley,<br>
Whose markup was of dubious quality.<br>
The validator complained,<br>
So the author was pained,<br>
To move the error from the markup to the rhyming.</p></body></html>)";

	BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_grouping_4)
{
	char32_t const* in = UR"(<section>
 <!-- ... -->
 <p>Last modified: 2001-04-23</p>
 <p>Author: fred@example.com</p>
</section>)";

	char32_t const* s = UR"(<html><head></head><body><section>
 <!-- ... -->
 <p>Last modified: 2001-04-23</p>
 <p>Author: fred@example.com</p>
</section></body></html>)";

	BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_grouping_5)
{
	char32_t const* in = UR"(<section>
 <!-- ... -->
 <footer>Last modified: 2001-04-23</footer>
 <address>Author: fred@example.com</address>
</section>)";

	char32_t const* s = UR"(<html><head></head><body><section>
 <!-- ... -->
 <footer>Last modified: 2001-04-23</footer>
 <address>Author: fred@example.com</address>
</section></body></html>)";

	BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_grouping_6)
{
	char32_t const* in = UR"(<section>
 <!-- ... -->
 <footer>
  <p>Last modified: 2001-04-23</p>
  <address>Author: fred@example.com</address>
 </footer>
</section>)";

	char32_t const* s = UR"(<html><head></head><body><section>
 <!-- ... -->
 <footer>
  <p>Last modified: 2001-04-23</p>
  <address>Author: fred@example.com</address>
 </footer>
</section></body></html>)";

	BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_grouping_7)
{
	char32_t const* in = UR"(<p>For instance, this fantastic sentence has bullets relating to</p>
<ul>
 <li>wizards,
 <li>faster-than-light travel, and
 <li>telepathy,
</ul>
<p>and is further discussed below.</p>)";

	char32_t const* s = UR"(<html><head></head><body><p>For instance, this fantastic sentence has bullets relating to</p>
<ul>
 <li>wizards,
 </li><li>faster-than-light travel, and
 </li><li>telepathy,
</li></ul>
<p>and is further discussed below.</p></body></html>)";

	BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_grouping_8)
{
	char32_t const* in = UR"(<div>For instance, this fantastic sentence has bullets relating to
<ul>
 <li>wizards,
 <li>faster-than-light travel, and
 <li>telepathy,
</ul>
and is further discussed below.</div>)";

	char32_t const* s = UR"(<html><head></head><body><div>For instance, this fantastic sentence has bullets relating to
<ul>
 <li>wizards,
 </li><li>faster-than-light travel, and
 </li><li>telepathy,
</li></ul>
and is further discussed below.</div></body></html>)";

	BOOST_CHECK(parse(in) == s);
}

/*
4.4.2 The hr element

https://html.spec.whatwg.org/multipage/grouping-content.html#the-hr-element
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_grouping_9)
{
    char32_t const* in = UR"(<section>
 <h1>Communication</h1>
 <p>There are various methods of communication. This section
 covers a few of the important ones used by the project.</p>
 <hr>
 <p>Communication stones seem to come in pairs and have mysterious
 properties:</p>
 <ul>
  <li>They can transfer thoughts in two directions once activated
  if used alone.</li>
  <li>If used with another device, they can transfer one's
  consciousness to another body.</li>
  <li>If both stones are used with another device, the
  consciousnesses switch bodies.</li>
 </ul>
 <hr>
 <p>Radios use the electromagnetic spectrum in the meter range and
 longer.</p>
 <hr>
 <p>Signal flares use the electromagnetic spectrum in the
 nanometer range.</p>
</section>
<section>
 <h1>Food</h1>
 <p>All food at the project is rationed:</p>
 <dl>
  <dt>Potatoes</dt>
  <dd>Two per day</dd>
  <dt>Soup</dt>
  <dd>One bowl per day</dd>
 </dl>
 <hr>
 <p>Cooking is done by the chefs on a set rotation.</p>
</section>)";

    char32_t const* s = UR"(<html><head></head><body><section>
 <h1>Communication</h1>
 <p>There are various methods of communication. This section
 covers a few of the important ones used by the project.</p>
 <hr>
 <p>Communication stones seem to come in pairs and have mysterious
 properties:</p>
 <ul>
  <li>They can transfer thoughts in two directions once activated
  if used alone.</li>
  <li>If used with another device, they can transfer one's
  consciousness to another body.</li>
  <li>If both stones are used with another device, the
  consciousnesses switch bodies.</li>
 </ul>
 <hr>
 <p>Radios use the electromagnetic spectrum in the meter range and
 longer.</p>
 <hr>
 <p>Signal flares use the electromagnetic spectrum in the
 nanometer range.</p>
</section>
<section>
 <h1>Food</h1>
 <p>All food at the project is rationed:</p>
 <dl>
  <dt>Potatoes</dt>
  <dd>Two per day</dd>
  <dt>Soup</dt>
  <dd>One bowl per day</dd>
 </dl>
 <hr>
 <p>Cooking is done by the chefs on a set rotation.</p>
</section></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_grouping_10)
{
    char32_t const* in = UR"(<p>Dudley was ninety-two, in his second life, and fast approaching
time for another rejuvenation. Despite his body having the physical
age of a standard fifty-year-old, the prospect of a long degrading
campaign within academia was one he regarded with dread. For a
supposedly advanced civilization, the Intersolar Commonwealth could be
appallingly backward at times, not to mention cruel.</p>
<p><i>Maybe it won't be that bad</i>, he told himself. The lie was
comforting enough to get him through the rest of the night's
shift.</p>
<hr>
<p>The Carlton AllLander drove Dudley home just after dawn. Like the
astronomer, the vehicle was old and worn, but perfectly capable of
doing its job. It had a cheap diesel engine, common enough on a
semi-frontier world like Gralmond, although its drive array was a
thoroughly modern photoneural processor. With its high suspension and
deep-tread tyres it could plough along the dirt track to the
observatory in all weather and seasons, including the metre-deep snow
of Gralmond's winters.</p>)";

    char32_t const* s = UR"(<html><head></head><body><p>Dudley was ninety-two, in his second life, and fast approaching
time for another rejuvenation. Despite his body having the physical
age of a standard fifty-year-old, the prospect of a long degrading
campaign within academia was one he regarded with dread. For a
supposedly advanced civilization, the Intersolar Commonwealth could be
appallingly backward at times, not to mention cruel.</p>
<p><i>Maybe it won't be that bad</i>, he told himself. The lie was
comforting enough to get him through the rest of the night's
shift.</p>
<hr>
<p>The Carlton AllLander drove Dudley home just after dawn. Like the
astronomer, the vehicle was old and worn, but perfectly capable of
doing its job. It had a cheap diesel engine, common enough on a
semi-frontier world like Gralmond, although its drive array was a
thoroughly modern photoneural processor. With its high suspension and
deep-tread tyres it could plough along the dirt track to the
observatory in all weather and seasons, including the metre-deep snow
of Gralmond's winters.</p></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

/*
4.4.3 The pre element

https://html.spec.whatwg.org/multipage/grouping-content.html#the-pre-element
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_grouping_11)
{
    char32_t const* in = UR"(<p>This is the <code>Panel</code> constructor:</p>
<pre><code>function Panel(element, canClose, closeHandler) {
  this.element = element;
  this.canClose = canClose;
  this.closeHandler = function () { if (closeHandler) closeHandler() };
}</code></pre>)";

    char32_t const* s = UR"(<html><head></head><body><p>This is the <code>Panel</code> constructor:</p>
<pre><code>function Panel(element, canClose, closeHandler) {
  this.element = element;
  this.canClose = canClose;
  this.closeHandler = function () { if (closeHandler) closeHandler() };
}</code></pre></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_grouping_12)
{
    char32_t const* in = UR"(<pre><samp>You are in an open field west of a big white house with a boarded
front door.
There is a small mailbox here.

></samp> <kbd>open mailbox</kbd>

<samp>Opening the mailbox reveals:
A leaflet.

></samp></pre>)";

    char32_t const* s = UR"(<html><head></head><body><pre><samp>You are in an open field west of a big white house with a boarded
front door.
There is a small mailbox here.

&gt;</samp> <kbd>open mailbox</kbd>

<samp>Opening the mailbox reveals:
A leaflet.

&gt;</samp></pre></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_grouping_13)
{
    char32_t const* in = UR"(<pre>                maxling

it is with a          heart
               heavy

that i admit loss of a feline
        so           loved

a friend lost to the
        unknown
                                (night)

~cdr 11dec07</pre>)";

    char32_t const* s = UR"(<html><head></head><body><pre>                maxling

it is with a          heart
               heavy

that i admit loss of a feline
        so           loved

a friend lost to the
        unknown
                                (night)

~cdr 11dec07</pre></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

/*
4.4.4 The blockquote element

https://html.spec.whatwg.org/multipage/grouping-content.html#the-blockquote-element
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_grouping_14)
{
    char32_t const* in = UR"(<blockquote>
 <p>[Jane] then said she liked [...] fish.</p>
</blockquote>)";

    char32_t const* s = UR"(<html><head></head><body><blockquote>
 <p>[Jane] then said she liked [...] fish.</p>
</blockquote></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_grouping_15)
{
    char32_t const* in = UR"(<blockquote>
 <p>I contend that we are both atheists. I just believe in one fewer
 god than you do. When you understand why you dismiss all the other
 possible gods, you will understand why I dismiss yours.</p>
</blockquote>
<p>— Stephen Roberts</p>)";

    char32_t const* s = UR"(<html><head></head><body><blockquote>
 <p>I contend that we are both atheists. I just believe in one fewer
 god than you do. When you understand why you dismiss all the other
 possible gods, you will understand why I dismiss yours.</p>
</blockquote>
<p>— Stephen Roberts</p></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_grouping_16)
{
    char32_t const* in = UR"(<figure>
 <blockquote>
  <p>The truth may be puzzling. It may take some work to grapple with.
  It may be counterintuitive. It may contradict deeply held
  prejudices. It may not be consonant with what we desperately want to
  be true. But our preferences do not determine what's true. We have a
  method, and that method helps us to reach not absolute truth, only
  asymptotic approaches to the truth — never there, just closer
  and closer, always finding vast new oceans of undiscovered
  possibilities. Cleverly designed experiments are the key.</p>
 </blockquote>
 <figcaption>Carl Sagan, in "<cite>Wonder and Skepticism</cite>", from
 the <cite>Skeptical Inquirer</cite> Volume 19, Issue 1 (January-February
 1995)</figcaption>
</figure>)";

    char32_t const* s = UR"(<html><head></head><body><figure>
 <blockquote>
  <p>The truth may be puzzling. It may take some work to grapple with.
  It may be counterintuitive. It may contradict deeply held
  prejudices. It may not be consonant with what we desperately want to
  be true. But our preferences do not determine what's true. We have a
  method, and that method helps us to reach not absolute truth, only
  asymptotic approaches to the truth — never there, just closer
  and closer, always finding vast new oceans of undiscovered
  possibilities. Cleverly designed experiments are the key.</p>
 </blockquote>
 <figcaption>Carl Sagan, in "<cite>Wonder and Skepticism</cite>", from
 the <cite>Skeptical Inquirer</cite> Volume 19, Issue 1 (January-February
 1995)</figcaption>
</figure></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_grouping_17)
{
    char32_t const* in = UR"(<p>His next piece was the aptly named <cite>Sonnet 130</cite>:</p>
<blockquote cite="https://quotes.example.org/s/sonnet130.html">
  <p>My mistress' eyes are nothing like the sun,<br>
  Coral is far more red, than her lips red,<br>
  ...)";

    char32_t const* s = UR"(<html><head></head><body><p>His next piece was the aptly named <cite>Sonnet 130</cite>:</p>
<blockquote cite="https://quotes.example.org/s/sonnet130.html">
  <p>My mistress' eyes are nothing like the sun,<br>
  Coral is far more red, than her lips red,<br>
  ...</p></blockquote></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_grouping_18)
{
    char32_t const* in = UR"(<article>
 <h1><a href="https://bacon.example.com/?blog=109431">Bacon on a crowbar</a></h1>
 <article>
  <header><strong>t3yw</strong> 12 points 1 hour ago</header>
  <p>I bet a narwhal would love that.</p>
  <footer><a href="?pid=29578">permalink</a></footer>
  <article>
   <header><strong>greg</strong> 8 points 1 hour ago</header>
   <blockquote><p>I bet a narwhal would love that.</p></blockquote>
   <p>Dude narwhals don't eat bacon.</p>
   <footer><a href="?pid=29579">permalink</a></footer>
   <article>
    <header><strong>t3yw</strong> 15 points 1 hour ago</header>
    <blockquote>
     <blockquote><p>I bet a narwhal would love that.</p></blockquote>
     <p>Dude narwhals don't eat bacon.</p>
    </blockquote>
    <p>Next thing you'll be saying they don't get capes and wizard
    hats either!</p>
    <footer><a href="?pid=29580">permalink</a></footer>
    <article>
     <article>
      <header><strong>boing</strong> -5 points 1 hour ago</header>
      <p>narwhals are worse than ceiling cat</p>
      <footer><a href="?pid=29581">permalink</a></footer>
     </article>
    </article>
   </article>
  </article>
  <article>
   <header><strong>fred</strong> 1 points 23 minutes ago</header>
   <blockquote><p>I bet a narwhal would love that.</p></blockquote>
   <p>I bet they'd love to peel a banana too.</p>
   <footer><a href="?pid=29582">permalink</a></footer>
  </article>
 </article>
</article>)";

    char32_t const* s = UR"(<html><head></head><body><article>
 <h1><a href="https://bacon.example.com/?blog=109431">Bacon on a crowbar</a></h1>
 <article>
  <header><strong>t3yw</strong> 12 points 1 hour ago</header>
  <p>I bet a narwhal would love that.</p>
  <footer><a href="?pid=29578">permalink</a></footer>
  <article>
   <header><strong>greg</strong> 8 points 1 hour ago</header>
   <blockquote><p>I bet a narwhal would love that.</p></blockquote>
   <p>Dude narwhals don't eat bacon.</p>
   <footer><a href="?pid=29579">permalink</a></footer>
   <article>
    <header><strong>t3yw</strong> 15 points 1 hour ago</header>
    <blockquote>
     <blockquote><p>I bet a narwhal would love that.</p></blockquote>
     <p>Dude narwhals don't eat bacon.</p>
    </blockquote>
    <p>Next thing you'll be saying they don't get capes and wizard
    hats either!</p>
    <footer><a href="?pid=29580">permalink</a></footer>
    <article>
     <article>
      <header><strong>boing</strong> -5 points 1 hour ago</header>
      <p>narwhals are worse than ceiling cat</p>
      <footer><a href="?pid=29581">permalink</a></footer>
     </article>
    </article>
   </article>
  </article>
  <article>
   <header><strong>fred</strong> 1 points 23 minutes ago</header>
   <blockquote><p>I bet a narwhal would love that.</p></blockquote>
   <p>I bet they'd love to peel a banana too.</p>
   <footer><a href="?pid=29582">permalink</a></footer>
  </article>
 </article>
</article></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_grouping_19)
{
    char32_t const* in = UR"(<p>He began his list of "lessons" with the following:</p>
<blockquote>One should never assume that his side of
the issue will be recognized, let alone that it will
be conceded to have merits.</blockquote>
<p>He continued with a number of similar points, ending with:</p>
<blockquote>Finally, one should be prepared for the threat
of breakdown in negotiations at any given moment and not
be cowed by the possibility.</blockquote>
<p>We shall now discuss these points...)";

    char32_t const* s = UR"(<html><head></head><body><p>He began his list of "lessons" with the following:</p>
<blockquote>One should never assume that his side of
the issue will be recognized, let alone that it will
be conceded to have merits.</blockquote>
<p>He continued with a number of similar points, ending with:</p>
<blockquote>Finally, one should be prepared for the threat
of breakdown in negotiations at any given moment and not
be cowed by the possibility.</blockquote>
<p>We shall now discuss these points...</p></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

/*
4.4.5 The ol element

https://html.spec.whatwg.org/multipage/grouping-content.html#the-ol-element
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_grouping_20)
{
    char32_t const* in = UR"(<p>I have lived in the following countries (given in the order of when
I first lived there):</p>
<ol>
 <li>Switzerland
 <li>United Kingdom
 <li>United States
 <li>Norway
</ol>)";

    char32_t const* s = UR"(<html><head></head><body><p>I have lived in the following countries (given in the order of when
I first lived there):</p>
<ol>
 <li>Switzerland
 </li><li>United Kingdom
 </li><li>United States
 </li><li>Norway
</li></ol></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_grouping_21)
{
    char32_t const* in = UR"(<p>I have lived in the following countries (given in the order of when
I first lived there):</p>
<ol>
 <li>United Kingdom
 <li>Switzerland
 <li>United States
 <li>Norway
</ol>)";

    char32_t const* s = UR"(<html><head></head><body><p>I have lived in the following countries (given in the order of when
I first lived there):</p>
<ol>
 <li>United Kingdom
 </li><li>Switzerland
 </li><li>United States
 </li><li>Norway
</li></ol></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

/*
4.4.6 The ul element

https://html.spec.whatwg.org/multipage/grouping-content.html#the-ul-element
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_grouping_22)
{
    char32_t const* in = UR"(<p>I have lived in the following countries:</p>
<ul>
 <li>Norway
 <li>Switzerland
 <li>United Kingdom
 <li>United States
</ul>)";

    char32_t const* s = UR"(<html><head></head><body><p>I have lived in the following countries:</p>
<ul>
 <li>Norway
 </li><li>Switzerland
 </li><li>United Kingdom
 </li><li>United States
</li></ul></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_grouping_23)
{
    char32_t const* in = UR"(<p>I have lived in the following countries:</p>
<ul>
 <li>Switzerland
 <li>Norway
 <li>United Kingdom
 <li>United States
</ul>)";

    char32_t const* s = UR"(<html><head></head><body><p>I have lived in the following countries:</p>
<ul>
 <li>Switzerland
 </li><li>Norway
 </li><li>United Kingdom
 </li><li>United States
</li></ul></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

/*
4.4.7 The menu element

https://html.spec.whatwg.org/multipage/grouping-content.html#the-menu-element
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_grouping_24)
{
    char32_t const* in = UR"*(<menu>
 <li><button onclick="copy()"><img src="copy.svg" alt="Copy"></button></li>
 <li><button onclick="cut()"><img src="cut.svg" alt="Cut"></button></li>
 <li><button onclick="paste()"><img src="paste.svg" alt="Paste"></button></li>
</menu>)*";

    char32_t const* s = UR"*(<html><head></head><body><menu>
 <li><button onclick="copy()"><img src="copy.svg" alt="Copy"></button></li>
 <li><button onclick="cut()"><img src="cut.svg" alt="Cut"></button></li>
 <li><button onclick="paste()"><img src="paste.svg" alt="Paste"></button></li>
</menu></body></html>)*";

    BOOST_CHECK(parse(in) == s);
}

/*
4.4.8 The li element

https://html.spec.whatwg.org/multipage/grouping-content.html#the-li-element
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_grouping_25)
{
    char32_t const* in = UR"(<ol>
 <li>Item 1
 <li value="3">Item 3
 <li>Item 4
</ol>)";

    char32_t const* s = UR"(<html><head></head><body><ol>
 <li>Item 1
 </li><li value="3">Item 3
 </li><li>Item 4
</li></ol></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_grouping_26)
{
    char32_t const* in = UR"(<figure>
 <figcaption>The top 10 movies of all time</figcaption>
 <ol>
  <li value="10"><cite>Josie and the Pussycats</cite>, 2001</li>
  <li value="9"><cite lang="sh">Црна мачка, бели мачор</cite>, 1998</li>
  <li value="8"><cite>A Bug's Life</cite>, 1998</li>
  <li value="7"><cite>Toy Story</cite>, 1995</li>
  <li value="6"><cite>Monsters, Inc</cite>, 2001</li>
  <li value="5"><cite>Cars</cite>, 2006</li>
  <li value="4"><cite>Toy Story 2</cite>, 1999</li>
  <li value="3"><cite>Finding Nemo</cite>, 2003</li>
  <li value="2"><cite>The Incredibles</cite>, 2004</li>
  <li value="1"><cite>Ratatouille</cite>, 2007</li>
 </ol>
</figure>)";

    char32_t const* s = UR"(<html><head></head><body><figure>
 <figcaption>The top 10 movies of all time</figcaption>
 <ol>
  <li value="10"><cite>Josie and the Pussycats</cite>, 2001</li>
  <li value="9"><cite lang="sh">Црна мачка, бели мачор</cite>, 1998</li>
  <li value="8"><cite>A Bug's Life</cite>, 1998</li>
  <li value="7"><cite>Toy Story</cite>, 1995</li>
  <li value="6"><cite>Monsters, Inc</cite>, 2001</li>
  <li value="5"><cite>Cars</cite>, 2006</li>
  <li value="4"><cite>Toy Story 2</cite>, 1999</li>
  <li value="3"><cite>Finding Nemo</cite>, 2003</li>
  <li value="2"><cite>The Incredibles</cite>, 2004</li>
  <li value="1"><cite>Ratatouille</cite>, 2007</li>
 </ol>
</figure></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_grouping_27)
{
    char32_t const* in = UR"(<figure>
 <figcaption>The top 10 movies of all time</figcaption>
 <ol reversed>
  <li><cite>Josie and the Pussycats</cite>, 2001</li>
  <li><cite lang="sh">Црна мачка, бели мачор</cite>, 1998</li>
  <li><cite>A Bug's Life</cite>, 1998</li>
  <li><cite>Toy Story</cite>, 1995</li>
  <li><cite>Monsters, Inc</cite>, 2001</li>
  <li><cite>Cars</cite>, 2006</li>
  <li><cite>Toy Story 2</cite>, 1999</li>
  <li><cite>Finding Nemo</cite>, 2003</li>
  <li><cite>The Incredibles</cite>, 2004</li>
  <li><cite>Ratatouille</cite>, 2007</li>
 </ol>
</figure>)";

    char32_t const* s = UR"(<html><head></head><body><figure>
 <figcaption>The top 10 movies of all time</figcaption>
 <ol reversed="">
  <li><cite>Josie and the Pussycats</cite>, 2001</li>
  <li><cite lang="sh">Црна мачка, бели мачор</cite>, 1998</li>
  <li><cite>A Bug's Life</cite>, 1998</li>
  <li><cite>Toy Story</cite>, 1995</li>
  <li><cite>Monsters, Inc</cite>, 2001</li>
  <li><cite>Cars</cite>, 2006</li>
  <li><cite>Toy Story 2</cite>, 1999</li>
  <li><cite>Finding Nemo</cite>, 2003</li>
  <li><cite>The Incredibles</cite>, 2004</li>
  <li><cite>Ratatouille</cite>, 2007</li>
 </ol>
</figure></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

/*
4.4.9 The dl element

https://html.spec.whatwg.org/multipage/grouping-content.html#the-dl-element
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_grouping_28)
{
    char32_t const* in = UR"(<dl>
 <dt> Authors
 <dd> John
 <dd> Luke
 <dt> Editor
 <dd> Frank
</dl>)";

    char32_t const* s = UR"(<html><head></head><body><dl>
 <dt> Authors
 </dt><dd> John
 </dd><dd> Luke
 </dd><dt> Editor
 </dt><dd> Frank
</dd></dl></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_grouping_29)
{
    char32_t const* in = UR"(<dl>
 <dt lang="en-US"> <dfn>color</dfn> </dt>
 <dt lang="en-GB"> <dfn>colour</dfn> </dt>
 <dd> A sensation which (in humans) derives from the ability of
 the fine structure of the eye to distinguish three differently
 filtered analyses of a view. </dd>
</dl>)";

    char32_t const* s = UR"(<html><head></head><body><dl>
 <dt lang="en-US"> <dfn>color</dfn> </dt>
 <dt lang="en-GB"> <dfn>colour</dfn> </dt>
 <dd> A sensation which (in humans) derives from the ability of
 the fine structure of the eye to distinguish three differently
 filtered analyses of a view. </dd>
</dl></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_grouping_30)
{
    char32_t const* in = UR"(<dl>
 <div>
  <dt> Last modified time </dt>
  <dd> 2004-12-23T23:33Z </dd>
 </div>
 <div>
  <dt> Recommended update interval </dt>
  <dd> 60s </dd>
 </div>
 <div>
  <dt> Authors </dt>
  <dt> Editors </dt>
  <dd> Robert Rothman </dd>
  <dd> Daniel Jackson </dd>
 </div>
</dl>)";

    char32_t const* s = UR"(<html><head></head><body><dl>
 <div>
  <dt> Last modified time </dt>
  <dd> 2004-12-23T23:33Z </dd>
 </div>
 <div>
  <dt> Recommended update interval </dt>
  <dd> 60s </dd>
 </div>
 <div>
  <dt> Authors </dt>
  <dt> Editors </dt>
  <dd> Robert Rothman </dd>
  <dd> Daniel Jackson </dd>
 </div>
</dl></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_grouping_31)
{
    char32_t const* in = UR"(<p>Determine the victory points as follows (use the
first matching case):</p>
<dl>
 <dt> If you have exactly five gold coins </dt>
 <dd> You get five victory points </dd>
 <dt> If you have one or more gold coins, and you have one or more silver coins </dt>
 <dd> You get two victory points </dd>
 <dt> If you have one or more silver coins </dt>
 <dd> You get one victory point </dd>
 <dt> Otherwise </dt>
 <dd> You get no victory points </dd>
</dl>)";

    char32_t const* s = UR"(<html><head></head><body><p>Determine the victory points as follows (use the
first matching case):</p>
<dl>
 <dt> If you have exactly five gold coins </dt>
 <dd> You get five victory points </dd>
 <dt> If you have one or more gold coins, and you have one or more silver coins </dt>
 <dd> You get two victory points </dd>
 <dt> If you have one or more silver coins </dt>
 <dd> You get one victory point </dd>
 <dt> Otherwise </dt>
 <dd> You get no victory points </dd>
</dl></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_grouping_32)
{
    char32_t const* in = UR"(<dl>
 <dt><dfn>Apartment</dfn>, n.</dt>
 <dd>An execution context grouping one or more threads with one or
 more COM objects.</dd>
 <dt><dfn>Flat</dfn>, n.</dt>
 <dd>A deflated tire.</dd>
 <dt><dfn>Home</dfn>, n.</dt>
 <dd>The user's login directory.</dd>
</dl>)";

    char32_t const* s = UR"(<html><head></head><body><dl>
 <dt><dfn>Apartment</dfn>, n.</dt>
 <dd>An execution context grouping one or more threads with one or
 more COM objects.</dd>
 <dt><dfn>Flat</dfn>, n.</dt>
 <dd>A deflated tire.</dd>
 <dt><dfn>Home</dfn>, n.</dt>
 <dd>The user's login directory.</dd>
</dl></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_grouping_33)
{
    char32_t const* in = UR"(<dl>
 <div itemscope itemtype="http://schema.org/Product">
  <dt itemprop="name">Café ou Chocolat Liégeois
  <dd itemprop="offers" itemscope itemtype="http://schema.org/Offer">
   <span itemprop="price">3.50</span>
   <data itemprop="priceCurrency" value="EUR">€</data>
  <dd itemprop="description">
   2 boules Café ou Chocolat, 1 boule Vanille, sause café ou chocolat, chantilly
 </div>

 <div itemscope itemtype="http://schema.org/Product">
  <dt itemprop="name">Américaine
  <dd itemprop="offers" itemscope itemtype="http://schema.org/Offer">
   <span itemprop="price">3.50</span>
   <data itemprop="priceCurrency" value="EUR">€</data>
  <dd itemprop="description">
   1 boule Crème brûlée, 1 boule Vanille, 1 boule Caramel, chantilly
 </div>
</dl>)";

    char32_t const* s = UR"(<html><head></head><body><dl>
 <div itemscope="" itemtype="http://schema.org/Product">
  <dt itemprop="name">Café ou Chocolat Liégeois
  </dt><dd itemprop="offers" itemscope="" itemtype="http://schema.org/Offer">
   <span itemprop="price">3.50</span>
   <data itemprop="priceCurrency" value="EUR">€</data>
  </dd><dd itemprop="description">
   2 boules Café ou Chocolat, 1 boule Vanille, sause café ou chocolat, chantilly
 </dd></div>

 <div itemscope="" itemtype="http://schema.org/Product">
  <dt itemprop="name">Américaine
  </dt><dd itemprop="offers" itemscope="" itemtype="http://schema.org/Offer">
   <span itemprop="price">3.50</span>
   <data itemprop="priceCurrency" value="EUR">€</data>
  </dd><dd itemprop="description">
   1 boule Crème brûlée, 1 boule Vanille, 1 boule Caramel, chantilly
 </dd></div>
</dl></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_grouping_34)
{
    char32_t const* in = UR"(<html><head></head><body><dl>
 <div itemscope="" itemtype="http://schema.org/Product">
  <dt itemprop="name">Café ou Chocolat Liégeois
  </dt><dd itemprop="offers" itemscope="" itemtype="http://schema.org/Offer">
   <span itemprop="price">3.50</span>
   <data itemprop="priceCurrency" value="EUR">€</data>
  </dd><dd itemprop="description">
   2 boules Café ou Chocolat, 1 boule Vanille, sause café ou chocolat, chantilly
 </dd></div>

 <div itemscope="" itemtype="http://schema.org/Product">
  <dt itemprop="name">Américaine
  </dt><dd itemprop="offers" itemscope="" itemtype="http://schema.org/Offer">
   <span itemprop="price">3.50</span>
   <data itemprop="priceCurrency" value="EUR">€</data>
  </dd><dd itemprop="description">
   1 boule Crème brûlée, 1 boule Vanille, 1 boule Caramel, chantilly
 </dd></div>
</dl></body></html>)";

    char32_t const* s = UR"(<html><head></head><body><dl>
 <div itemscope="" itemtype="http://schema.org/Product">
  <dt itemprop="name">Café ou Chocolat Liégeois
  </dt><dd itemprop="offers" itemscope="" itemtype="http://schema.org/Offer">
   <span itemprop="price">3.50</span>
   <data itemprop="priceCurrency" value="EUR">€</data>
  </dd><dd itemprop="description">
   2 boules Café ou Chocolat, 1 boule Vanille, sause café ou chocolat, chantilly
 </dd></div>

 <div itemscope="" itemtype="http://schema.org/Product">
  <dt itemprop="name">Américaine
  </dt><dd itemprop="offers" itemscope="" itemtype="http://schema.org/Offer">
   <span itemprop="price">3.50</span>
   <data itemprop="priceCurrency" value="EUR">€</data>
  </dd><dd itemprop="description">
   1 boule Crème brûlée, 1 boule Vanille, 1 boule Caramel, chantilly
 </dd></div>
</dl></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_grouping_35)
{
    char32_t const* in = UR"(<dl>
 <dt itemscope itemtype="http://schema.org/Product" itemref="1-offer 1-description">
  <span itemprop="name">Café ou Chocolat Liégeois</span>
 <dd id="1-offer" itemprop="offers" itemscope itemtype="http://schema.org/Offer">
  <span itemprop="price">3.50</span>
  <data itemprop="priceCurrency" value="EUR">€</data>
 <dd id="1-description" itemprop="description">
  2 boules Café ou Chocolat, 1 boule Vanille, sause café ou chocolat, chantilly

 <dt itemscope itemtype="http://schema.org/Product" itemref="2-offer 2-description">
  <span itemprop="name">Américaine</span>
 <dd id="2-offer" itemprop="offers" itemscope itemtype="http://schema.org/Offer">
  <span itemprop="price">3.50</span>
  <data itemprop="priceCurrency" value="EUR">€</data>
 <dd id="2-description" itemprop="description">
  1 boule Crème brûlée, 1 boule Vanille, 1 boule Caramel, chantilly
</dl>)";

    char32_t const* s = UR"(<html><head></head><body><dl>
 <dt itemscope="" itemtype="http://schema.org/Product" itemref="1-offer 1-description">
  <span itemprop="name">Café ou Chocolat Liégeois</span>
 </dt><dd id="1-offer" itemprop="offers" itemscope="" itemtype="http://schema.org/Offer">
  <span itemprop="price">3.50</span>
  <data itemprop="priceCurrency" value="EUR">€</data>
 </dd><dd id="1-description" itemprop="description">
  2 boules Café ou Chocolat, 1 boule Vanille, sause café ou chocolat, chantilly

 </dd><dt itemscope="" itemtype="http://schema.org/Product" itemref="2-offer 2-description">
  <span itemprop="name">Américaine</span>
 </dt><dd id="2-offer" itemprop="offers" itemscope="" itemtype="http://schema.org/Offer">
  <span itemprop="price">3.50</span>
  <data itemprop="priceCurrency" value="EUR">€</data>
 </dd><dd id="2-description" itemprop="description">
  1 boule Crème brûlée, 1 boule Vanille, 1 boule Caramel, chantilly
</dd></dl></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

/*
4.4.10 The dt element

https://html.spec.whatwg.org/multipage/grouping-content.html#the-dt-element
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_grouping_36)
{
    char32_t const* in = UR"(<article>
 <h1>FAQ</h1>
 <dl>
  <dt>What do we want?</dt>
  <dd>Our data.</dd>
  <dt>When do we want it?</dt>
  <dd>Now.</dd>
  <dt>Where is it?</dt>
  <dd>We are not sure.</dd>
 </dl>
</article>)";

    char32_t const* s = UR"(<html><head></head><body><article>
 <h1>FAQ</h1>
 <dl>
  <dt>What do we want?</dt>
  <dd>Our data.</dd>
  <dt>When do we want it?</dt>
  <dd>Now.</dd>
  <dt>Where is it?</dt>
  <dd>We are not sure.</dd>
 </dl>
</article></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

/*
4.4.11 The dd element

https://html.spec.whatwg.org/multipage/grouping-content.html#the-dd-element
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_grouping_37)
{
    char32_t const* in = UR"(<dl>
 <dt><dfn>happiness</dfn></dt>
 <dd class="pronunciation">/'hæ p. nes/</dd>
 <dd class="part-of-speech"><i><abbr>n.</abbr></i></dd>
 <dd>The state of being happy.</dd>
 <dd>Good fortune; success. <q>Oh <b>happiness</b>! It worked!</q></dd>
 <dt><dfn>rejoice</dfn></dt>
 <dd class="pronunciation">/ri jois'/</dd>
 <dd><i class="part-of-speech"><abbr>v.intr.</abbr></i> To be delighted oneself.</dd>
 <dd><i class="part-of-speech"><abbr>v.tr.</abbr></i> To cause one to be delighted.</dd>
</dl>)";

    char32_t const* s = UR"(<html><head></head><body><dl>
 <dt><dfn>happiness</dfn></dt>
 <dd class="pronunciation">/'hæ p. nes/</dd>
 <dd class="part-of-speech"><i><abbr>n.</abbr></i></dd>
 <dd>The state of being happy.</dd>
 <dd>Good fortune; success. <q>Oh <b>happiness</b>! It worked!</q></dd>
 <dt><dfn>rejoice</dfn></dt>
 <dd class="pronunciation">/ri jois'/</dd>
 <dd><i class="part-of-speech"><abbr>v.intr.</abbr></i> To be delighted oneself.</dd>
 <dd><i class="part-of-speech"><abbr>v.tr.</abbr></i> To cause one to be delighted.</dd>
</dl></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

/*
4.4.12 The figure element

https://html.spec.whatwg.org/multipage/grouping-content.html#the-figure-element
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_grouping_38)
{
    char32_t const* in = UR"(<p>In <a href="#l4">listing 4</a> we see the primary core interface
API declaration.</p>
<figure id="l4">
 <figcaption>Listing 4. The primary core interface API declaration.</figcaption>
 <pre><code>interface PrimaryCore {
 boolean verifyDataLine();
 void sendData(in sequence&lt;byte> data);
 void initSelfDestruct();
}</code></pre>
</figure>
<p>The API is designed to use UTF-8.</p>)";

    char32_t const* s = UR"(<html><head></head><body><p>In <a href="#l4">listing 4</a> we see the primary core interface
API declaration.</p>
<figure id="l4">
 <figcaption>Listing 4. The primary core interface API declaration.</figcaption>
 <pre><code>interface PrimaryCore {
 boolean verifyDataLine();
 void sendData(in sequence&lt;byte&gt; data);
 void initSelfDestruct();
}</code></pre>
</figure>
<p>The API is designed to use UTF-8.</p></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_grouping_39)
{
    char32_t const* in = UR"(<!DOCTYPE HTML>
<html lang="en">
<title>Bubbles at work — My Gallery™</title>
<figure>
 <img src="bubbles-work.jpeg"
      alt="Bubbles, sitting in his office chair, works on his
           latest project intently.">
 <figcaption>Bubbles at work</figcaption>
</figure>
<nav><a href="19414.html">Prev</a> — <a href="19416.html">Next</a></nav>)";

    char32_t const* s = UR"(<!DOCTYPE html><html lang="en"><head><title>Bubbles at work — My Gallery™</title>
</head><body><figure>
 <img src="bubbles-work.jpeg" alt="Bubbles, sitting in his office chair, works on his
           latest project intently.">
 <figcaption>Bubbles at work</figcaption>
</figure>
<nav><a href="19414.html">Prev</a> — <a href="19416.html">Next</a></nav></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_grouping_40)
{
    char32_t const* in = UR"(<h2>Malinko's comics</h2>

<p>This case centered on some sort of "intellectual property"
infringement related to a comic (see Exhibit A). The suit started
after a trailer ending with these words:

<blockquote>
 <img src="promblem-packed-action.png" alt="ROUGH COPY! Promblem-Packed Action!">
</blockquote>

<p>...was aired. A lawyer, armed with a Bigger Notebook, launched a
preemptive strike using snowballs. A complete copy of the trailer is
included with Exhibit B.

<figure>
 <img src="ex-a.png" alt="Two squiggles on a dirty piece of paper.">
 <figcaption>Exhibit A. The alleged <cite>rough copy</cite> comic.</figcaption>
</figure>

<figure>
 <video src="ex-b.mov"></video>
 <figcaption>Exhibit B. The <cite>Rough Copy</cite> trailer.</figcaption>
</figure>

<p>The case was resolved out of court.)";

    char32_t const* s = UR"(<html><head></head><body><h2>Malinko's comics</h2>

<p>This case centered on some sort of "intellectual property"
infringement related to a comic (see Exhibit A). The suit started
after a trailer ending with these words:

</p><blockquote>
 <img src="promblem-packed-action.png" alt="ROUGH COPY! Promblem-Packed Action!">
</blockquote>

<p>...was aired. A lawyer, armed with a Bigger Notebook, launched a
preemptive strike using snowballs. A complete copy of the trailer is
included with Exhibit B.

</p><figure>
 <img src="ex-a.png" alt="Two squiggles on a dirty piece of paper.">
 <figcaption>Exhibit A. The alleged <cite>rough copy</cite> comic.</figcaption>
</figure>

<figure>
 <video src="ex-b.mov"></video>
 <figcaption>Exhibit B. The <cite>Rough Copy</cite> trailer.</figcaption>
</figure>

<p>The case was resolved out of court.</p></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_grouping_41)
{
    char32_t const* in = UR"(<figure>
 <p>'Twas brillig, and the slithy toves<br>
 Did gyre and gimble in the wabe;<br>
 All mimsy were the borogoves,<br>
 And the mome raths outgrabe.</p>
 <figcaption><cite>Jabberwocky</cite> (first verse). Lewis Carroll, 1832-98</figcaption>
</figure>)";

    char32_t const* s = UR"(<html><head></head><body><figure>
 <p>'Twas brillig, and the slithy toves<br>
 Did gyre and gimble in the wabe;<br>
 All mimsy were the borogoves,<br>
 And the mome raths outgrabe.</p>
 <figcaption><cite>Jabberwocky</cite> (first verse). Lewis Carroll, 1832-98</figcaption>
</figure></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_grouping_42)
{
    char32_t const* in = UR"(<figure>
 <figcaption>The castle through the ages: 1423, 1858, and 1999 respectively.</figcaption>
 <figure>
  <figcaption>Etching. Anonymous, ca. 1423.</figcaption>
  <img src="castle1423.jpeg" alt="The castle has one tower, and a tall wall around it.">
 </figure>
 <figure>
  <figcaption>Oil-based paint on canvas. Maria Towle, 1858.</figcaption>
  <img src="castle1858.jpeg" alt="The castle now has two towers and two walls.">
 </figure>
 <figure>
  <figcaption>Film photograph. Peter Jankle, 1999.</figcaption>
  <img src="castle1999.jpeg" alt="The castle lies in ruins, the original tower all that remains in one piece.">
 </figure>
</figure>)";

    char32_t const* s = UR"(<html><head></head><body><figure>
 <figcaption>The castle through the ages: 1423, 1858, and 1999 respectively.</figcaption>
 <figure>
  <figcaption>Etching. Anonymous, ca. 1423.</figcaption>
  <img src="castle1423.jpeg" alt="The castle has one tower, and a tall wall around it.">
 </figure>
 <figure>
  <figcaption>Oil-based paint on canvas. Maria Towle, 1858.</figcaption>
  <img src="castle1858.jpeg" alt="The castle now has two towers and two walls.">
 </figure>
 <figure>
  <figcaption>Film photograph. Peter Jankle, 1999.</figcaption>
  <img src="castle1999.jpeg" alt="The castle lies in ruins, the original tower all that remains in one piece.">
 </figure>
</figure></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_grouping_43)
{
    char32_t const* in = UR"(<figure>
 <img src="castle1423.jpeg" title="Etching. Anonymous, ca. 1423."
      alt="The castle has one tower, and a tall wall around it.">
 <img src="castle1858.jpeg" title="Oil-based paint on canvas. Maria Towle, 1858."
      alt="The castle now has two towers and two walls.">
 <img src="castle1999.jpeg" title="Film photograph. Peter Jankle, 1999."
      alt="The castle lies in ruins, the original tower all that remains in one piece.">
 <figcaption>The castle through the ages: 1423, 1858, and 1999 respectively.</figcaption>
</figure>)";

    char32_t const* s = UR"(<html><head></head><body><figure>
 <img src="castle1423.jpeg" title="Etching. Anonymous, ca. 1423." alt="The castle has one tower, and a tall wall around it.">
 <img src="castle1858.jpeg" title="Oil-based paint on canvas. Maria Towle, 1858." alt="The castle now has two towers and two walls.">
 <img src="castle1999.jpeg" title="Film photograph. Peter Jankle, 1999." alt="The castle lies in ruins, the original tower all that remains in one piece.">
 <figcaption>The castle through the ages: 1423, 1858, and 1999 respectively.</figcaption>
</figure></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_grouping_44)
{
    char32_t const* in = UR"(<article>
 <h1>Fiscal negotiations stumble in Congress as deadline nears</h1>
 <figure>
  <img src="obama-reid.jpeg" alt="Obama and Reid sit together smiling in the Oval Office.">
  <figcaption>Barack Obama and Harry Reid. White House press photograph.</figcaption>
 </figure>
 <p>Negotiations in Congress to end the fiscal impasse sputtered on Tuesday, leaving both chambers
 grasping for a way to reopen the government and raise the country's borrowing authority with a
 Thursday deadline drawing near.</p>
 ...
</article>)";

    char32_t const* s = UR"(<html><head></head><body><article>
 <h1>Fiscal negotiations stumble in Congress as deadline nears</h1>
 <figure>
  <img src="obama-reid.jpeg" alt="Obama and Reid sit together smiling in the Oval Office.">
  <figcaption>Barack Obama and Harry Reid. White House press photograph.</figcaption>
 </figure>
 <p>Negotiations in Congress to end the fiscal impasse sputtered on Tuesday, leaving both chambers
 grasping for a way to reopen the government and raise the country's borrowing authority with a
 Thursday deadline drawing near.</p>
 ...
</article></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

/*
4.4.13 The figcaption element

https://html.spec.whatwg.org/multipage/grouping-content.html#the-figcaption-element
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_grouping_45)
{
    char32_t const* in = UR"(<figcaption>
 <p>A duck.</p>
 <p><small>Photograph courtesy of 🌟 News.</small></p>
</figcaption>)";

    char32_t const* s = UR"(<html><head></head><body><figcaption>
 <p>A duck.</p>
 <p><small>Photograph courtesy of 🌟 News.</small></p>
</figcaption></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_grouping_46)
{
    char32_t const* in = UR"(<figcaption>
 <p>Average rent for 3-room apartments, excluding non-profit apartments</p>
 <p>Zürich’s Statistics Office — <time datetime=2017-11-14>14 November 2017</time></p>
</figcaption>)";

    char32_t const* s = UR"(<html><head></head><body><figcaption>
 <p>Average rent for 3-room apartments, excluding non-profit apartments</p>
 <p>Zürich’s Statistics Office — <time datetime="2017-11-14">14 November 2017</time></p>
</figcaption></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

/*
4.4.14 The main element

https://html.spec.whatwg.org/multipage/grouping-content.html#the-main-element
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_grouping_47)
{
    char32_t const* in = UR"(<!DOCTYPE html>
<html lang="en">
<title>RPG System 17</title>
<style>
 header, nav, aside, main, footer {
   margin: 0.5em; border: thin solid; padding: 0.5em;
   background: #EFF; color: black; box-shadow: 0 0 0.25em #033;
 }
 h1, h2, p { margin: 0; }
 nav, main { float: left; }
 aside { float: right; }
 footer { clear: both; }
</style>
<header>
 <h1>System Eighteen</h1>
</header>
<nav>
 <a href="../16/">← System 17</a>
 <a href="../18/">RPXIX →</a>
</nav>
<aside>
 <p>This system has no HP mechanic, so there's no healing.
</aside>
<main>
 <h2>Character creation</h2>
 <p>Attributes (magic, strength, agility) are purchased at the cost of one point per level.</p>
 <h2>Rolls</h2>
 <p>Each encounter, roll the dice for all your skills. If you roll more than the opponent, you win.</p>
</main>
<footer>
 <p>Copyright © 2013
</footer>
</html>)";

    char32_t const* s = UR"(<!DOCTYPE html><html lang="en"><head><title>RPG System 17</title>
<style>
 header, nav, aside, main, footer {
   margin: 0.5em; border: thin solid; padding: 0.5em;
   background: #EFF; color: black; box-shadow: 0 0 0.25em #033;
 }
 h1, h2, p { margin: 0; }
 nav, main { float: left; }
 aside { float: right; }
 footer { clear: both; }
</style>
</head><body><header>
 <h1>System Eighteen</h1>
</header>
<nav>
 <a href="../16/">← System 17</a>
 <a href="../18/">RPXIX →</a>
</nav>
<aside>
 <p>This system has no HP mechanic, so there's no healing.
</p></aside>
<main>
 <h2>Character creation</h2>
 <p>Attributes (magic, strength, agility) are purchased at the cost of one point per level.</p>
 <h2>Rolls</h2>
 <p>Each encounter, roll the dice for all your skills. If you roll more than the opponent, you win.</p>
</main>
<footer>
 <p>Copyright © 2013
</p></footer>
</body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_grouping_48)
{
    char32_t const* in = UR"(<!doctype html>
<html lang=en-CA>
<meta charset=utf-8>
<title> … </title>
<link rel=stylesheet href=spa.css>
<script src=spa.js async></script>
<nav>
 <a href=/>Home</a>
 <a href=/about>About</a>
 <a href=/contact>Contact</a>
</nav>
<main>
 <h1>Home</h1>
 …
</main>
<main hidden>
 <h1>About</h1>
 …
</main>
<main hidden>
 <h1>Contact</h1>
 …
</main>
<footer>Made with ❤️ by <a href=https://example.com/>Example 👻</a>.</footer>)";

    char32_t const* s = UR"(<!DOCTYPE html><html lang="en-CA"><head><meta charset="utf-8">
<title> … </title>
<link rel="stylesheet" href="spa.css">
<script src="spa.js" async=""></script>
</head><body><nav>
 <a href="/">Home</a>
 <a href="/about">About</a>
 <a href="/contact">Contact</a>
</nav>
<main>
 <h1>Home</h1>
 …
</main>
<main hidden="">
 <h1>About</h1>
 …
</main>
<main hidden="">
 <h1>Contact</h1>
 …
</main>
<footer>Made with ❤️ by <a href="https://example.com/">Example 👻</a>.</footer></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

/*
4.4.15 The div element

https://html.spec.whatwg.org/multipage/grouping-content.html#the-div-element
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_grouping_49)
{
    char32_t const* in = UR"(<article lang="en-US">
 <h1>My use of language and my cats</h1>
 <p>My cat's behavior hasn't changed much since her absence, except
 that she plays her new physique to the neighbors regularly, in an
 attempt to get pets.</p>
 <div lang="en-GB">
  <p>My other cat, coloured black and white, is a sweetie. He followed
  us to the pool today, walking down the pavement with us. Yesterday
  he apparently visited our neighbours. I wonder if he recognises that
  their flat is a mirror image of ours.</p>
  <p>Hm, I just noticed that in the last paragraph I used British
  English. But I'm supposed to write in American English. So I
  shouldn't say "pavement" or "flat" or "colour"...</p>
 </div>
 <p>I should say "sidewalk" and "apartment" and "color"!</p>
</article>)";

    char32_t const* s = UR"(<html><head></head><body><article lang="en-US">
 <h1>My use of language and my cats</h1>
 <p>My cat's behavior hasn't changed much since her absence, except
 that she plays her new physique to the neighbors regularly, in an
 attempt to get pets.</p>
 <div lang="en-GB">
  <p>My other cat, coloured black and white, is a sweetie. He followed
  us to the pool today, walking down the pavement with us. Yesterday
  he apparently visited our neighbours. I wonder if he recognises that
  their flat is a mirror image of ours.</p>
  <p>Hm, I just noticed that in the last paragraph I used British
  English. But I'm supposed to write in American English. So I
  shouldn't say "pavement" or "flat" or "colour"...</p>
 </div>
 <p>I should say "sidewalk" and "apartment" and "color"!</p>
</article></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_SUITE_END()
