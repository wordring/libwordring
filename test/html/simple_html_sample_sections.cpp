// test/html/simple_html_sample_sections.cpp

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

BOOST_AUTO_TEST_SUITE(simple_html_sample_sections_test)

/*
4.3.1 The body element

https://html.spec.whatwg.org/multipage/sections.html#the-body-element
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_sections_1)
{
	char32_t const* in = UR"*(<!DOCTYPE HTML>
<html lang="en">
 <head>
  <title>Online or offline?</title>
  <script>
   function update(online) {
     //document.getElementById('status').textContent =
       //online ? 'Online' : 'Offline';
   }
  </script>
 </head>
 <body ononline="update(true)"
       onoffline="update(false)"
       onload="update(navigator.onLine)">
  <p>You are: <span id="status">(Unknown)</span></p>
 </body>
</html>)*";

	char32_t const* s = UR"*(<!DOCTYPE html><html lang="en"><head>
  <title>Online or offline?</title>
  <script>
   function update(online) {
     //document.getElementById('status').textContent =
       //online ? 'Online' : 'Offline';
   }
  </script>
 </head>
 <body ononline="update(true)" onoffline="update(false)" onload="update(navigator.onLine)">
  <p>You are: <span id="status">(Unknown)</span></p>
 
</body></html>)*";

	BOOST_CHECK(parse(in) == s);
}

/*
4.3.2 The article element

https://html.spec.whatwg.org/multipage/sections.html#the-article-element
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_sections_2)
{
	char32_t const* in = UR"(<article itemscope itemtype="http://schema.org/BlogPosting">
 <header>
  <h1 itemprop="headline">The Very First Rule of Life</h1>
  <p><time itemprop="datePublished" datetime="2009-10-09">3 days ago</time></p>
  <link itemprop="url" href="?comments=0">
 </header>
 <p>If there's a microphone anywhere near you, assume it's hot and
 sending whatever you're saying to the world. Seriously.</p>
 <p>...</p>
 <footer>
  <a itemprop="discussionUrl" href="?comments=1">Show comments...</a>
 </footer>
</article>)";

	char32_t const* s = UR"(<html><head></head><body><article itemscope="" itemtype="http://schema.org/BlogPosting">
 <header>
  <h1 itemprop="headline">The Very First Rule of Life</h1>
  <p><time itemprop="datePublished" datetime="2009-10-09">3 days ago</time></p>
  <link itemprop="url" href="?comments=0">
 </header>
 <p>If there's a microphone anywhere near you, assume it's hot and
 sending whatever you're saying to the world. Seriously.</p>
 <p>...</p>
 <footer>
  <a itemprop="discussionUrl" href="?comments=1">Show comments...</a>
 </footer>
</article></body></html>)";

	BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_sections_3)
{
	char32_t const* in = UR"(<article itemscope itemtype="http://schema.org/BlogPosting">
 <header>
  <h1 itemprop="headline">The Very First Rule of Life</h1>
  <p><time itemprop="datePublished" datetime="2009-10-09">3 days ago</time></p>
  <link itemprop="url" href="?comments=0">
 </header>
 <p>If there's a microphone anywhere near you, assume it's hot and
 sending whatever you're saying to the world. Seriously.</p>
 <p>...</p>
 <section>
  <h1>Comments</h1>
  <article itemprop="comment" itemscope itemtype="http://schema.org/UserComments" id="c1">
   <link itemprop="url" href="#c1">
   <footer>
    <p>Posted by: <span itemprop="creator" itemscope itemtype="http://schema.org/Person">
     <span itemprop="name">George Washington</span>
    </span></p>
    <p><time itemprop="commentTime" datetime="2009-10-10">15 minutes ago</time></p>
   </footer>
   <p>Yeah! Especially when talking about your lobbyist friends!</p>
  </article>
  <article itemprop="comment" itemscope itemtype="http://schema.org/UserComments" id="c2">
   <link itemprop="url" href="#c2">
   <footer>
    <p>Posted by: <span itemprop="creator" itemscope itemtype="http://schema.org/Person">
     <span itemprop="name">George Hammond</span>
    </span></p>
    <p><time itemprop="commentTime" datetime="2009-10-10">5 minutes ago</time></p>
   </footer>
   <p>Hey, you have the same first name as me.</p>
  </article>
 </section>
</article>)";

	char32_t const* s = UR"(<html><head></head><body><article itemscope="" itemtype="http://schema.org/BlogPosting">
 <header>
  <h1 itemprop="headline">The Very First Rule of Life</h1>
  <p><time itemprop="datePublished" datetime="2009-10-09">3 days ago</time></p>
  <link itemprop="url" href="?comments=0">
 </header>
 <p>If there's a microphone anywhere near you, assume it's hot and
 sending whatever you're saying to the world. Seriously.</p>
 <p>...</p>
 <section>
  <h1>Comments</h1>
  <article itemprop="comment" itemscope="" itemtype="http://schema.org/UserComments" id="c1">
   <link itemprop="url" href="#c1">
   <footer>
    <p>Posted by: <span itemprop="creator" itemscope="" itemtype="http://schema.org/Person">
     <span itemprop="name">George Washington</span>
    </span></p>
    <p><time itemprop="commentTime" datetime="2009-10-10">15 minutes ago</time></p>
   </footer>
   <p>Yeah! Especially when talking about your lobbyist friends!</p>
  </article>
  <article itemprop="comment" itemscope="" itemtype="http://schema.org/UserComments" id="c2">
   <link itemprop="url" href="#c2">
   <footer>
    <p>Posted by: <span itemprop="creator" itemscope="" itemtype="http://schema.org/Person">
     <span itemprop="name">George Hammond</span>
    </span></p>
    <p><time itemprop="commentTime" datetime="2009-10-10">5 minutes ago</time></p>
   </footer>
   <p>Hey, you have the same first name as me.</p>
  </article>
 </section>
</article></body></html>)";

	BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_sections_4)
{
	char32_t const* in = UR"*(<!DOCTYPE HTML>
<html lang=en>
<title>eHome Portal</title>
<script src="/scripts/widgets.js"></script>
<link rel=stylesheet href="/styles/main.css">
<article is="stock-widget">
 <h1>Stocks</h1>
 <table>
  <thead> <tr> <th> Stock <th> Value <th> Delta
  <tbody> <template> <tr> <td> <td> <td> </template>
 </table>
 <p> <input type=button value="Refresh" onclick="this.parentElement.refresh()">
</article>
<article is="news-widget">
 <h1>News</h1>
 <ul>
  <template>
   <li>
    <p><img> <strong></strong>
    <p>
  </template>
 </ul>
 <p> <input type=button value="Refresh" onclick="this.parentElement.refresh()">
</article>)*";

	char32_t const* s = UR"*(<!DOCTYPE html><html lang="en"><head><title>eHome Portal</title>
<script src="/scripts/widgets.js"></script>
<link rel="stylesheet" href="/styles/main.css">
</head><body><article is="stock-widget">
 <h1>Stocks</h1>
 <table>
  <thead> <tr> <th> Stock </th><th> Value </th><th> Delta
  </th></tr></thead><tbody> <template> <tr> <td> </td><td> </td><td> </td></tr></template>
 </tbody></table>
 <p> <input type="button" value="Refresh" onclick="this.parentElement.refresh()">
</p></article>
<article is="news-widget">
 <h1>News</h1>
 <ul>
  <template>
   <li>
    <p><img> <strong></strong>
    </p><p>
  </p></li></template>
 </ul>
 <p> <input type="button" value="Refresh" onclick="this.parentElement.refresh()">
</p></article></body></html>)*";

	BOOST_CHECK(parse(in) == s);
}

/*
4.3.3 The section element

https://html.spec.whatwg.org/multipage/sections.html#the-section-element
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_sections_5)
{
	char32_t const* in = UR"(<article>
 <hgroup>
  <h1>Apples</h1>
  <h2>Tasty, delicious fruit!</h2>
 </hgroup>
 <p>The apple is the pomaceous fruit of the apple tree.</p>
 <section>
  <h1>Red Delicious</h1>
  <p>These bright red apples are the most common found in many
  supermarkets.</p>
 </section>
 <section>
  <h1>Granny Smith</h1>
  <p>These juicy, green apples make a great filling for
  apple pies.</p>
 </section>
</article>)";

	char32_t const* s = UR"(<html><head></head><body><article>
 <hgroup>
  <h1>Apples</h1>
  <h2>Tasty, delicious fruit!</h2>
 </hgroup>
 <p>The apple is the pomaceous fruit of the apple tree.</p>
 <section>
  <h1>Red Delicious</h1>
  <p>These bright red apples are the most common found in many
  supermarkets.</p>
 </section>
 <section>
  <h1>Granny Smith</h1>
  <p>These juicy, green apples make a great filling for
  apple pies.</p>
 </section>
</article></body></html>)";

	BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_sections_6)
{
	char32_t const* in = UR"(<!DOCTYPE Html>
<Html Lang=En
 ><Head
   ><Title
     >Graduation Ceremony Summer 2022</Title
   ></Head
 ><Body
   ><H1
     >Graduation</H1
   ><Section
     ><H1
       >Ceremony</H1
     ><P
       >Opening Procession</P
     ><P
       >Speech by Validactorian</P
     ><P
       >Speech by Class President</P
     ><P
       >Presentation of Diplomas</P
     ><P
       >Closing Speech by Headmaster</P
   ></Section
   ><Section
     ><H1
       >Graduates</H1
     ><Ul
       ><Li
         >Molly Carpenter</Li
       ><Li
         >Anastasia Luccio</Li
       ><Li
         >Ebenezar McCoy</Li
       ><Li
         >Karrin Murphy</Li
       ><Li
         >Thomas Raith</Li
       ><Li
         >Susan Rodriguez</Li
     ></Ul
   ></Section
 ></Body
></Html>)";

	char32_t const* s = UR"(<!DOCTYPE html><html lang="En"><head><title>Graduation Ceremony Summer 2022</title></head><body><h1>Graduation</h1><section><h1>Ceremony</h1><p>Opening Procession</p><p>Speech by Validactorian</p><p>Speech by Class President</p><p>Presentation of Diplomas</p><p>Closing Speech by Headmaster</p></section><section><h1>Graduates</h1><ul><li>Molly Carpenter</li><li>Anastasia Luccio</li><li>Ebenezar McCoy</li><li>Karrin Murphy</li><li>Thomas Raith</li><li>Susan Rodriguez</li></ul></section></body></html>)";

	BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_sections_7)
{
	char32_t const* in = UR"(<style>
 section { border: double medium; margin: 2em; }
 section.chapter h1 { font: 2em Roboto, Helvetica Neue, sans-serif; }
 section.appendix h1 { font: small-caps 2em Roboto, Helvetica Neue, sans-serif; }
</style>
<header>
 <hgroup>
  <h1>My Book</h1>
  <h2>A sample with not much content</h2>
 </hgroup>
 <p><small>Published by Dummy Publicorp Ltd.</small></p>
</header>
<section class="chapter">
 <h1>My First Chapter</h1>
 <p>This is the first of my chapters. It doesn't say much.</p>
 <p>But it has two paragraphs!</p>
</section>
<section class="chapter">
 <h1>It Continues: The Second Chapter</h1>
 <p>Bla dee bla, dee bla dee bla. Boom.</p>
</section>
<section class="chapter">
 <h1>Chapter Three: A Further Example</h1>
 <p>It's not like a battle between brightness and earthtones would go
 unnoticed.</p>
 <p>But it might ruin my story.</p>
</section>
<section class="appendix">
 <h1>Appendix A: Overview of Examples</h1>
 <p>These are demonstrations.</p>
</section>
<section class="appendix">
 <h1>Appendix B: Some Closing Remarks</h1>
 <p>Hopefully this long example shows that you <em>can</em> style
 sections, so long as they are used to indicate actual sections.</p>
</section>)";

	char32_t const* s = UR"(<html><head><style>
 section { border: double medium; margin: 2em; }
 section.chapter h1 { font: 2em Roboto, Helvetica Neue, sans-serif; }
 section.appendix h1 { font: small-caps 2em Roboto, Helvetica Neue, sans-serif; }
</style>
</head><body><header>
 <hgroup>
  <h1>My Book</h1>
  <h2>A sample with not much content</h2>
 </hgroup>
 <p><small>Published by Dummy Publicorp Ltd.</small></p>
</header>
<section class="chapter">
 <h1>My First Chapter</h1>
 <p>This is the first of my chapters. It doesn't say much.</p>
 <p>But it has two paragraphs!</p>
</section>
<section class="chapter">
 <h1>It Continues: The Second Chapter</h1>
 <p>Bla dee bla, dee bla dee bla. Boom.</p>
</section>
<section class="chapter">
 <h1>Chapter Three: A Further Example</h1>
 <p>It's not like a battle between brightness and earthtones would go
 unnoticed.</p>
 <p>But it might ruin my story.</p>
</section>
<section class="appendix">
 <h1>Appendix A: Overview of Examples</h1>
 <p>These are demonstrations.</p>
</section>
<section class="appendix">
 <h1>Appendix B: Some Closing Remarks</h1>
 <p>Hopefully this long example shows that you <em>can</em> style
 sections, so long as they are used to indicate actual sections.</p>
</section></body></html>)";

	BOOST_CHECK(parse(in) == s);
}

/*
4.3.4 The nav element

https://html.spec.whatwg.org/multipage/sections.html#the-nav-element
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_sections_8)
{
	char32_t const* in = UR"(<body>
 <h1>The Wiki Center Of Exampland</h1>
 <nav>
  <ul>
   <li><a href="/">Home</a></li>
   <li><a href="/events">Current Events</a></li>
   ...more...
  </ul>
 </nav>
 <article>
  <header>
   <h1>Demos in Exampland</h1>
   <p>Written by A. N. Other.</p>
  </header>
  <nav>
   <ul>
    <li><a href="#public">Public demonstrations</a></li>
    <li><a href="#destroy">Demolitions</a></li>
    ...more...
   </ul>
  </nav>
  <div>
   <section id="public">
    <h1>Public demonstrations</h1>
    <p>...more...</p>
   </section>
   <section id="destroy">
    <h1>Demolitions</h1>
    <p>...more...</p>
   </section>
   ...more...
  </div>
  <footer>
   <p><a href="?edit">Edit</a> | <a href="?delete">Delete</a> | <a href="?Rename">Rename</a></p>
  </footer>
 </article>
 <footer>
  <p><small>© copyright 1998 Exampland Emperor</small></p>
 </footer>
</body>)";

	char32_t const* s = UR"(<html><head></head><body>
 <h1>The Wiki Center Of Exampland</h1>
 <nav>
  <ul>
   <li><a href="/">Home</a></li>
   <li><a href="/events">Current Events</a></li>
   ...more...
  </ul>
 </nav>
 <article>
  <header>
   <h1>Demos in Exampland</h1>
   <p>Written by A. N. Other.</p>
  </header>
  <nav>
   <ul>
    <li><a href="#public">Public demonstrations</a></li>
    <li><a href="#destroy">Demolitions</a></li>
    ...more...
   </ul>
  </nav>
  <div>
   <section id="public">
    <h1>Public demonstrations</h1>
    <p>...more...</p>
   </section>
   <section id="destroy">
    <h1>Demolitions</h1>
    <p>...more...</p>
   </section>
   ...more...
  </div>
  <footer>
   <p><a href="?edit">Edit</a> | <a href="?delete">Delete</a> | <a href="?Rename">Rename</a></p>
  </footer>
 </article>
 <footer>
  <p><small>© copyright 1998 Exampland Emperor</small></p>
 </footer>
</body></html>)";

	BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_sections_9)
{
    char32_t const* in = UR"(<body itemscope itemtype="http://schema.org/Blog">
 <header>
  <h1>Wake up sheeple!</h1>
  <p><a href="news.html">News</a> -
     <a href="blog.html">Blog</a> -
     <a href="forums.html">Forums</a></p>
  <p>Last Modified: <span itemprop="dateModified">2009-04-01</span></p>
  <nav>
   <h1>Navigation</h1>
   <ul>
    <li><a href="articles.html">Index of all articles</a></li>
    <li><a href="today.html">Things sheeple need to wake up for today</a></li>
    <li><a href="successes.html">Sheeple we have managed to wake</a></li>
   </ul>
  </nav>
 </header>
 <main>
  <article itemprop="blogPosts" itemscope itemtype="http://schema.org/BlogPosting">
   <header>
    <h1 itemprop="headline">My Day at the Beach</h1>
   </header>
   <div itemprop="articleBody">
    <p>Today I went to the beach and had a lot of fun.</p>
    ...more content...
   </div>
   <footer>
    <p>Posted <time itemprop="datePublished" datetime="2009-10-10">Thursday</time>.</p>
   </footer>
  </article>
  ...more blog posts...
 </main>
 <footer>
  <p>Copyright ©
   <span itemprop="copyrightYear">2010</span>
   <span itemprop="copyrightHolder">The Example Company</span>
  </p>
  <p><a href="about.html">About</a> -
     <a href="policy.html">Privacy Policy</a> -
     <a href="contact.html">Contact Us</a></p>
 </footer>
</body>)";

    char32_t const* s = UR"(<html><head></head><body itemscope="" itemtype="http://schema.org/Blog">
 <header>
  <h1>Wake up sheeple!</h1>
  <p><a href="news.html">News</a> -
     <a href="blog.html">Blog</a> -
     <a href="forums.html">Forums</a></p>
  <p>Last Modified: <span itemprop="dateModified">2009-04-01</span></p>
  <nav>
   <h1>Navigation</h1>
   <ul>
    <li><a href="articles.html">Index of all articles</a></li>
    <li><a href="today.html">Things sheeple need to wake up for today</a></li>
    <li><a href="successes.html">Sheeple we have managed to wake</a></li>
   </ul>
  </nav>
 </header>
 <main>
  <article itemprop="blogPosts" itemscope="" itemtype="http://schema.org/BlogPosting">
   <header>
    <h1 itemprop="headline">My Day at the Beach</h1>
   </header>
   <div itemprop="articleBody">
    <p>Today I went to the beach and had a lot of fun.</p>
    ...more content...
   </div>
   <footer>
    <p>Posted <time itemprop="datePublished" datetime="2009-10-10">Thursday</time>.</p>
   </footer>
  </article>
  ...more blog posts...
 </main>
 <footer>
  <p>Copyright ©
   <span itemprop="copyrightYear">2010</span>
   <span itemprop="copyrightHolder">The Example Company</span>
  </p>
  <p><a href="about.html">About</a> -
     <a href="policy.html">Privacy Policy</a> -
     <a href="contact.html">Contact Us</a></p>
 </footer>
</body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_sections_10)
{
    char32_t const* in = UR"(<nav>
 <h1>Navigation</h1>
 <p>You are on my home page. To the north lies <a href="/blog">my
 blog</a>, from whence the sounds of battle can be heard. To the east
 you can see a large mountain, upon which many <a
 href="/school">school papers</a> are littered. Far up thus mountain
 you can spy a little figure who appears to be me, desperately
 scribbling a <a href="/school/thesis">thesis</a>.</p>
 <p>To the west are several exits. One fun-looking exit is labeled <a
 href="https://games.example.com/">"games"</a>. Another more
 boring-looking exit is labeled <a
 href="https://isp.example.net/">ISP™</a>.</p>
 <p>To the south lies a dark and dank <a href="/about">contacts
 page</a>. Cobwebs cover its disused entrance, and at one point you
 see a rat run quickly out of the page.</p>
</nav>)";

    char32_t const* s = UR"(<html><head></head><body><nav>
 <h1>Navigation</h1>
 <p>You are on my home page. To the north lies <a href="/blog">my
 blog</a>, from whence the sounds of battle can be heard. To the east
 you can see a large mountain, upon which many <a href="/school">school papers</a> are littered. Far up thus mountain
 you can spy a little figure who appears to be me, desperately
 scribbling a <a href="/school/thesis">thesis</a>.</p>
 <p>To the west are several exits. One fun-looking exit is labeled <a href="https://games.example.com/">"games"</a>. Another more
 boring-looking exit is labeled <a href="https://isp.example.net/">ISP™</a>.</p>
 <p>To the south lies a dark and dank <a href="/about">contacts
 page</a>. Cobwebs cover its disused entrance, and at one point you
 see a rat run quickly out of the page.</p>
</nav></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_sections_11)
{
    char32_t const* in = UR"*(<p><input type=button value="Compose" onclick="compose()"></p>
<nav>
 <h1>Folders</h1>
 <ul>
  <li> <a href="/inbox" onclick="return openFolder(this.href)">Inbox</a> <span class=count></span>
  <li> <a href="/sent" onclick="return openFolder(this.href)">Sent</a>
  <li> <a href="/drafts" onclick="return openFolder(this.href)">Drafts</a>
  <li> <a href="/trash" onclick="return openFolder(this.href)">Trash</a>
  <li> <a href="/customers" onclick="return openFolder(this.href)">Customers</a>
 </ul>
</nav>)*";

    char32_t const* s = UR"*(<html><head></head><body><p><input type="button" value="Compose" onclick="compose()"></p>
<nav>
 <h1>Folders</h1>
 <ul>
  <li> <a href="/inbox" onclick="return openFolder(this.href)">Inbox</a> <span class="count"></span>
  </li><li> <a href="/sent" onclick="return openFolder(this.href)">Sent</a>
  </li><li> <a href="/drafts" onclick="return openFolder(this.href)">Drafts</a>
  </li><li> <a href="/trash" onclick="return openFolder(this.href)">Trash</a>
  </li><li> <a href="/customers" onclick="return openFolder(this.href)">Customers</a>
 </li></ul>
</nav></body></html>)*";

    BOOST_CHECK(parse(in) == s);
}

/*
4.3.5 The aside element

https://html.spec.whatwg.org/multipage/sections.html#the-aside-element
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_sections_12)
{
    char32_t const* in = UR"(<aside>
 <h1>Switzerland</h1>
 <p>Switzerland, a land-locked country in the middle of geographic
 Europe, has not joined the geopolitical European Union, though it is
 a signatory to a number of European treaties.</p>
</aside>)";

    char32_t const* s = UR"(<html><head></head><body><aside>
 <h1>Switzerland</h1>
 <p>Switzerland, a land-locked country in the middle of geographic
 Europe, has not joined the geopolitical European Union, though it is
 a signatory to a number of European treaties.</p>
</aside></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_sections_13)
{
    char32_t const* in = UR"(<p>He later joined a large company, continuing on the same work.
<q>I love my job. People ask me what I do for fun when I'm not at
work. But I'm paid to do my hobby, so I never know what to
answer. Some people wonder what they would do if they didn't have to
work... but I know what I would do, because I was unemployed for a
year, and I filled that time doing exactly what I do now.</q></p>

<aside>
 <q> People ask me what I do for fun when I'm not at work. But I'm
 paid to do my hobby, so I never know what to answer. </q>
</aside>

<p>Of course his work — or should that be hobby? —
isn't his only passion. He also enjoys other pleasures.</p>

...)";

    char32_t const* s = UR"(<html><head></head><body><p>He later joined a large company, continuing on the same work.
<q>I love my job. People ask me what I do for fun when I'm not at
work. But I'm paid to do my hobby, so I never know what to
answer. Some people wonder what they would do if they didn't have to
work... but I know what I would do, because I was unemployed for a
year, and I filled that time doing exactly what I do now.</q></p>

<aside>
 <q> People ask me what I do for fun when I'm not at work. But I'm
 paid to do my hobby, so I never know what to answer. </q>
</aside>

<p>Of course his work — or should that be hobby? —
isn't his only passion. He also enjoys other pleasures.</p>

...</body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_sections_14)
{
    char32_t const* in = UR"(<body>
 <header>
  <h1>My wonderful blog</h1>
  <p>My tagline</p>
 </header>
 <aside>
  <!-- this aside contains two sections that are tangentially related
  to the page, namely, links to other blogs, and links to blog posts
  from this blog -->
  <nav>
   <h1>My blogroll</h1>
   <ul>
    <li><a href="https://blog.example.com/">Example Blog</a>
   </ul>
  </nav>
  <nav>
   <h1>Archives</h1>
   <ol reversed>
    <li><a href="/last-post">My last post</a>
    <li><a href="/first-post">My first post</a>
   </ol>
  </nav>
 </aside>
 <aside>
  <!-- this aside is tangentially related to the page also, it
  contains twitter messages from the blog author -->
  <h1>Twitter Feed</h1>
  <blockquote cite="https://twitter.example.net/t31351234">
   I'm on vacation, writing my blog.
  </blockquote>
  <blockquote cite="https://twitter.example.net/t31219752">
   I'm going to go on vacation soon.
  </blockquote>
 </aside>
 <article>
  <!-- this is a blog post -->
  <h1>My last post</h1>
  <p>This is my last post.</p>
  <footer>
   <p><a href="/last-post" rel=bookmark>Permalink</a>
  </footer>
 </article>
 <article>
  <!-- this is also a blog post -->
  <h1>My first post</h1>
  <p>This is my first post.</p>
  <aside>
   <!-- this aside is about the blog post, since it's inside the
   <article> element; it would be wrong, for instance, to put the
   blogroll here, since the blogroll isn't really related to this post
   specifically, only to the page as a whole -->
   <h1>Posting</h1>
   <p>While I'm thinking about it, I wanted to say something about
   posting. Posting is fun!</p>
  </aside>
  <footer>
   <p><a href="/first-post" rel=bookmark>Permalink</a>
  </footer>
 </article>
 <footer>
  <p><a href="/archives">Archives</a> -
   <a href="/about">About me</a> -
   <a href="/copyright">Copyright</a></p>
 </footer>
</body>)";

    char32_t const* s = UR"(<html><head></head><body>
 <header>
  <h1>My wonderful blog</h1>
  <p>My tagline</p>
 </header>
 <aside>
  <!-- this aside contains two sections that are tangentially related
  to the page, namely, links to other blogs, and links to blog posts
  from this blog -->
  <nav>
   <h1>My blogroll</h1>
   <ul>
    <li><a href="https://blog.example.com/">Example Blog</a>
   </li></ul>
  </nav>
  <nav>
   <h1>Archives</h1>
   <ol reversed="">
    <li><a href="/last-post">My last post</a>
    </li><li><a href="/first-post">My first post</a>
   </li></ol>
  </nav>
 </aside>
 <aside>
  <!-- this aside is tangentially related to the page also, it
  contains twitter messages from the blog author -->
  <h1>Twitter Feed</h1>
  <blockquote cite="https://twitter.example.net/t31351234">
   I'm on vacation, writing my blog.
  </blockquote>
  <blockquote cite="https://twitter.example.net/t31219752">
   I'm going to go on vacation soon.
  </blockquote>
 </aside>
 <article>
  <!-- this is a blog post -->
  <h1>My last post</h1>
  <p>This is my last post.</p>
  <footer>
   <p><a href="/last-post" rel="bookmark">Permalink</a>
  </p></footer>
 </article>
 <article>
  <!-- this is also a blog post -->
  <h1>My first post</h1>
  <p>This is my first post.</p>
  <aside>
   <!-- this aside is about the blog post, since it's inside the
   <article> element; it would be wrong, for instance, to put the
   blogroll here, since the blogroll isn't really related to this post
   specifically, only to the page as a whole -->
   <h1>Posting</h1>
   <p>While I'm thinking about it, I wanted to say something about
   posting. Posting is fun!</p>
  </aside>
  <footer>
   <p><a href="/first-post" rel="bookmark">Permalink</a>
  </p></footer>
 </article>
 <footer>
  <p><a href="/archives">Archives</a> -
   <a href="/about">About me</a> -
   <a href="/copyright">Copyright</a></p>
 </footer>
</body></html>)";

    BOOST_CHECK(parse(in) == s);
}

/*
4.3.6 The h1, h2, h3, h4, h5, and h6 elements

https://html.spec.whatwg.org/multipage/sections.html#the-h1,-h2,-h3,-h4,-h5,-and-h6-elements
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_sections_15)
{
    char32_t const* in = UR"(<body>
<h1>Let's call it a draw(ing surface)</h1>
<h2>Diving in</h2>
<h2>Simple shapes</h2>
<h2>Canvas coordinates</h2>
<h3>Canvas coordinates diagram</h3>
<h2>Paths</h2>
</body>
<body>
 <h1>Let's call it a draw(ing surface)</h1>
 <section>
  <h1>Diving in</h1>
 </section>
 <section>
  <h1>Simple shapes</h1>
 </section>
 <section>
  <h1>Canvas coordinates</h1>
  <section>
   <h1>Canvas coordinates diagram</h1>
  </section>
 </section>
 <section>
  <h1>Paths</h1>
 </section>
</body>)";

    char32_t const* s = UR"(<html><head></head><body>
<h1>Let's call it a draw(ing surface)</h1>
<h2>Diving in</h2>
<h2>Simple shapes</h2>
<h2>Canvas coordinates</h2>
<h3>Canvas coordinates diagram</h3>
<h2>Paths</h2>


 <h1>Let's call it a draw(ing surface)</h1>
 <section>
  <h1>Diving in</h1>
 </section>
 <section>
  <h1>Simple shapes</h1>
 </section>
 <section>
  <h1>Canvas coordinates</h1>
  <section>
   <h1>Canvas coordinates diagram</h1>
  </section>
 </section>
 <section>
  <h1>Paths</h1>
 </section>
</body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_sections_16)
{
    char32_t const* in = UR"(<body>
 <h1>Let's call it a draw(ing surface)</h1>
 <section>
  <h2>Diving in</h2>
 </section>
 <section>
  <h2>Simple shapes</h2>
 </section>
 <section>
  <h2>Canvas coordinates</h2>
  <section>
   <h3>Canvas coordinates diagram</h3>
  </section>
 </section>
 <section>
  <h2>Paths</h2>
 </section>
</body>)";

    char32_t const* s = UR"(<html><head></head><body>
 <h1>Let's call it a draw(ing surface)</h1>
 <section>
  <h2>Diving in</h2>
 </section>
 <section>
  <h2>Simple shapes</h2>
 </section>
 <section>
  <h2>Canvas coordinates</h2>
  <section>
   <h3>Canvas coordinates diagram</h3>
  </section>
 </section>
 <section>
  <h2>Paths</h2>
 </section>
</body></html>)";

    BOOST_CHECK(parse(in) == s);
}

/*
4.3.7 The hgroup element

https://html.spec.whatwg.org/multipage/sections.html#the-hgroup-element
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_sections_17)
{
    char32_t const* in = UR"(<hgroup>
 <h1>The reality dysfunction</h1>
 <h2>Space is not the only void</h2>
</hgroup>)";

    char32_t const* s = UR"(<html><head></head><body><hgroup>
 <h1>The reality dysfunction</h1>
 <h2>Space is not the only void</h2>
</hgroup></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_sections_18)
{
    char32_t const* in = UR"(<hgroup>
 <h1>Dr. Strangelove</h1>
 <h2>Or: How I Learned to Stop Worrying and Love the Bomb</h2>
</hgroup>)";

    char32_t const* s = UR"(<html><head></head><body><hgroup>
 <h1>Dr. Strangelove</h1>
 <h2>Or: How I Learned to Stop Worrying and Love the Bomb</h2>
</hgroup></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_sections_19)
{
    char32_t const* in = UR"(<hgroup>
 <h1>The Avengers</h1>
 <h1>Avengers Assemble</h1>
</hgroup>

<hgroup>
 <h1>Avengers Assemble</h1>
 <h1>The Avengers</h1>
</hgroup>)";

    char32_t const* s = UR"(<html><head></head><body><hgroup>
 <h1>The Avengers</h1>
 <h1>Avengers Assemble</h1>
</hgroup>

<hgroup>
 <h1>Avengers Assemble</h1>
 <h1>The Avengers</h1>
</hgroup></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_sections_20)
{
    char32_t const* in = UR"*(<dialog onclose="walletSetup.continue(this.returnValue)">
 <hgroup>
  <h1>Wallet Setup</h1>
  <h2>Configure your Wallet funding source</h2>
 </hgroup>
 <p>Your Wallet can be used to buy wands at the merchant in town, to buy potions from travelling
 salesmen you may find in the dungeons, and to pay for mercenaries.</p>
 <p>We support two payment sources:</p>
 <form method=dialog>
  <fieldset oninput="this.getElementsByTagName('input')[0].checked = true">
   <legend> <label> <input type=radio name=payment-type value=cc> Credit Card </label> </legend>
   <p><label>Name on card: <input name=cc1 autocomplete="section-cc cc-name" placeholder="Y. Name"></label>
   <p><label>Card number: <input name=cc2 inputmode=numeric autocomplete="section-cc cc-number" placeholder="6331 1019 9999 0016"></label>
   <p><label>Expiry Date: <input name=cc3 type=month autocomplete="section-cc cc-exp" placeholder="2020-02"></label>
   <p><label>Security Code: <input name=cc4 inputmode=numeric autocomplete="section-cc cc-csc" placeholder="246"></label>
  </fieldset>
  <fieldset oninput="this.getElementsByTagName('input')[0].checked = true">
   <legend> <label> <input type=radio name=payment-type value=bank> Checking Account </label> </legend>
   <p><label>Name on account: <input name=bank1 autocomplete="section-bank cc-name"></label>
   <p><label>Routing number: <input name=bank2 inputmode=numeric></label>
   <p><label>Account number: <input name=bank3 inputmode=numeric></label>
  </fieldset>
  <button type=submit value="back"> ← Back </button>
  <button type=submit value="next"> Next → </button>
 </form>
</dialog>)*";

    char32_t const* s = UR"*(<html><head></head><body><dialog onclose="walletSetup.continue(this.returnValue)">
 <hgroup>
  <h1>Wallet Setup</h1>
  <h2>Configure your Wallet funding source</h2>
 </hgroup>
 <p>Your Wallet can be used to buy wands at the merchant in town, to buy potions from travelling
 salesmen you may find in the dungeons, and to pay for mercenaries.</p>
 <p>We support two payment sources:</p>
 <form method="dialog">
  <fieldset oninput="this.getElementsByTagName('input')[0].checked = true">
   <legend> <label> <input type="radio" name="payment-type" value="cc"> Credit Card </label> </legend>
   <p><label>Name on card: <input name="cc1" autocomplete="section-cc cc-name" placeholder="Y. Name"></label>
   </p><p><label>Card number: <input name="cc2" inputmode="numeric" autocomplete="section-cc cc-number" placeholder="6331 1019 9999 0016"></label>
   </p><p><label>Expiry Date: <input name="cc3" type="month" autocomplete="section-cc cc-exp" placeholder="2020-02"></label>
   </p><p><label>Security Code: <input name="cc4" inputmode="numeric" autocomplete="section-cc cc-csc" placeholder="246"></label>
  </p></fieldset>
  <fieldset oninput="this.getElementsByTagName('input')[0].checked = true">
   <legend> <label> <input type="radio" name="payment-type" value="bank"> Checking Account </label> </legend>
   <p><label>Name on account: <input name="bank1" autocomplete="section-bank cc-name"></label>
   </p><p><label>Routing number: <input name="bank2" inputmode="numeric"></label>
   </p><p><label>Account number: <input name="bank3" inputmode="numeric"></label>
  </p></fieldset>
  <button type="submit" value="back"> ← Back </button>
  <button type="submit" value="next"> Next → </button>
 </form>
</dialog></body></html>)*";

    BOOST_CHECK(parse(in) == s);
}

/*
4.3.8 The header element

https://html.spec.whatwg.org/multipage/sections.html#the-header-element
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_sections_21)
{
    char32_t const* in = UR"(<header>
 <p>Welcome to...</p>
 <h1>Voidwars!</h1>
</header>)";

    char32_t const* s = UR"(<html><head></head><body><header>
 <p>Welcome to...</p>
 <h1>Voidwars!</h1>
</header></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_sections_22)
{
    char32_t const* in = UR"(<header>
 <hgroup>
  <h1>Fullscreen API</h1>
  <h2>Living Standard — Last Updated 19 October 2015</h2>
 </hgroup>
 <dl>
  <dt>Participate:</dt>
  <dd><a href="https://github.com/whatwg/fullscreen">GitHub whatwg/fullscreen</a></dd>
  <dt>Commits:</dt>
  <dd><a href="https://github.com/whatwg/fullscreen/commits">GitHub whatwg/fullscreen/commits</a></dd>
 </dl>
</header>)";

    char32_t const* s = UR"(<html><head></head><body><header>
 <hgroup>
  <h1>Fullscreen API</h1>
  <h2>Living Standard — Last Updated 19 October 2015</h2>
 </hgroup>
 <dl>
  <dt>Participate:</dt>
  <dd><a href="https://github.com/whatwg/fullscreen">GitHub whatwg/fullscreen</a></dd>
  <dt>Commits:</dt>
  <dd><a href="https://github.com/whatwg/fullscreen/commits">GitHub whatwg/fullscreen/commits</a></dd>
 </dl>
</header></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_sections_23)
{
    char32_t const* in = UR"(<body>
 <header>
  <h1>Little Green Guys With Guns</h1>
  <nav>
   <ul>
    <li><a href="/games">Games</a>
    <li><a href="/forum">Forum</a>
    <li><a href="/download">Download</a>
   </ul>
  </nav>
  <h2>Important News</h2> <!-- this starts a second subsection -->
  <!-- this is part of the subsection entitled "Important News" -->
  <p>To play today's games you will need to update your client.</p>
  <h2>Games</h2> <!-- this starts a third subsection -->
 </header>
 <p>You have three active games:</p>
 <!-- this is still part of the subsection entitled "Games" -->
 ...)";

    char32_t const* s = UR"(<html><head></head><body>
 <header>
  <h1>Little Green Guys With Guns</h1>
  <nav>
   <ul>
    <li><a href="/games">Games</a>
    </li><li><a href="/forum">Forum</a>
    </li><li><a href="/download">Download</a>
   </li></ul>
  </nav>
  <h2>Important News</h2> <!-- this starts a second subsection -->
  <!-- this is part of the subsection entitled "Important News" -->
  <p>To play today's games you will need to update your client.</p>
  <h2>Games</h2> <!-- this starts a third subsection -->
 </header>
 <p>You have three active games:</p>
 <!-- this is still part of the subsection entitled "Games" -->
 ...</body></html>)";

    BOOST_CHECK(parse(in) == s);
}

/*
4.3.9 The footer element

https://html.spec.whatwg.org/multipage/sections.html#the-footer-element
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_sections_24)
{
    char32_t const* in = UR"(<body>
 <footer><a href="../">Back to index...</a></footer>
 <hgroup>
  <h1>Lorem ipsum</h1>
  <h2>The ipsum of all lorems</h2>
 </hgroup>
 <p>A dolor sit amet, consectetur adipisicing elit, sed do eiusmod
 tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim
 veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex
 ea commodo consequat. Duis aute irure dolor in reprehenderit in
 voluptate velit esse cillum dolore eu fugiat nulla
 pariatur. Excepteur sint occaecat cupidatat non proident, sunt in
 culpa qui officia deserunt mollit anim id est laborum.</p>
 <footer><a href="../">Back to index...</a></footer>
</body>)";

    char32_t const* s = UR"(<html><head></head><body>
 <footer><a href="../">Back to index...</a></footer>
 <hgroup>
  <h1>Lorem ipsum</h1>
  <h2>The ipsum of all lorems</h2>
 </hgroup>
 <p>A dolor sit amet, consectetur adipisicing elit, sed do eiusmod
 tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim
 veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex
 ea commodo consequat. Duis aute irure dolor in reprehenderit in
 voluptate velit esse cillum dolore eu fugiat nulla
 pariatur. Excepteur sint occaecat cupidatat non proident, sunt in
 culpa qui officia deserunt mollit anim id est laborum.</p>
 <footer><a href="../">Back to index...</a></footer>
</body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_sections_25)
{
    char32_t const* in = UR"(<!DOCTYPE HTML>
<HTML LANG="en"><HEAD>
<TITLE>The Ramblings of a Scientist</TITLE>
<BODY>
<H1>The Ramblings of a Scientist</H1>
<ARTICLE>
 <H1>Episode 15</H1>
 <VIDEO SRC="/fm/015.ogv" CONTROLS PRELOAD>
  <P><A HREF="/fm/015.ogv">Download video</A>.</P>
 </VIDEO>
 <FOOTER> <!-- footer for article -->
  <P>Published <TIME DATETIME="2009-10-21T18:26-07:00">on 2009/10/21 at 6:26pm</TIME></P>
 </FOOTER>
</ARTICLE>
<ARTICLE>
 <H1>My Favorite Trains</H1>
 <P>I love my trains. My favorite train of all time is a Köf.</P>
 <P>It is fun to see them pull some coal cars because they look so
 dwarfed in comparison.</P>
 <FOOTER> <!-- footer for article -->
  <P>Published <TIME DATETIME="2009-09-15T14:54-07:00">on 2009/09/15 at 2:54pm</TIME></P>
 </FOOTER>
</ARTICLE>
<FOOTER> <!-- site wide footer -->
 <NAV>
  <P><A HREF="/credits.html">Credits</A> —
     <A HREF="/tos.html">Terms of Service</A> —
     <A HREF="/index.html">Blog Index</A></P>
 </NAV>
 <P>Copyright © 2009 Gordon Freeman</P>
</FOOTER>
</BODY>
</HTML>)";

    char32_t const* s = UR"(<!DOCTYPE html><html lang="en"><head>
<title>The Ramblings of a Scientist</title>
</head><body>
<h1>The Ramblings of a Scientist</h1>
<article>
 <h1>Episode 15</h1>
 <video src="/fm/015.ogv" controls="" preload="">
  <p><a href="/fm/015.ogv">Download video</a>.</p>
 </video>
 <footer> <!-- footer for article -->
  <p>Published <time datetime="2009-10-21T18:26-07:00">on 2009/10/21 at 6:26pm</time></p>
 </footer>
</article>
<article>
 <h1>My Favorite Trains</h1>
 <p>I love my trains. My favorite train of all time is a Köf.</p>
 <p>It is fun to see them pull some coal cars because they look so
 dwarfed in comparison.</p>
 <footer> <!-- footer for article -->
  <p>Published <time datetime="2009-09-15T14:54-07:00">on 2009/09/15 at 2:54pm</time></p>
 </footer>
</article>
<footer> <!-- site wide footer -->
 <nav>
  <p><a href="/credits.html">Credits</a> —
     <a href="/tos.html">Terms of Service</a> —
     <a href="/index.html">Blog Index</a></p>
 </nav>
 <p>Copyright © 2009 Gordon Freeman</p>
</footer>

</body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_sections_26)
{
    char32_t const* in = UR"(...
 <footer>
  <nav>
   <section>
    <h1>Articles</h1>
    <p><img src="images/somersaults.jpeg" alt=""> Go to the gym with
    our somersaults class! Our teacher Jim takes you through the paces
    in this two-part article. <a href="articles/somersaults/1">Part
    1</a> · <a href="articles/somersaults/2">Part 2</a></p>
    <p><img src="images/kindplus.jpeg"> Tired of walking on the edge of
    a clif<!-- sic -->? Our guest writer Lara shows you how to bumble
    your way through the bars. <a href="articles/kindplus/1">Read
    more...</a></p>
    <p><img src="images/crisps.jpeg"> The chips are down, now all
    that's left is a potato. What can you do with it? <a
    href="articles/crisps/1">Read more...</a></p>
   </section>
   <ul>
    <li> <a href="/about">About us...</a>
    <li> <a href="/feedback">Send feedback!</a>
    <li> <a href="/sitemap">Sitemap</a>
   </ul>
  </nav>
  <p><small>Copyright © 2015 The Snacker —
  <a href="/tos">Terms of Service</a></small></p>
 </footer>
</body>)";

    char32_t const* s = UR"(<html><head></head><body>...
 <footer>
  <nav>
   <section>
    <h1>Articles</h1>
    <p><img src="images/somersaults.jpeg" alt=""> Go to the gym with
    our somersaults class! Our teacher Jim takes you through the paces
    in this two-part article. <a href="articles/somersaults/1">Part
    1</a> · <a href="articles/somersaults/2">Part 2</a></p>
    <p><img src="images/kindplus.jpeg"> Tired of walking on the edge of
    a clif<!-- sic -->? Our guest writer Lara shows you how to bumble
    your way through the bars. <a href="articles/kindplus/1">Read
    more...</a></p>
    <p><img src="images/crisps.jpeg"> The chips are down, now all
    that's left is a potato. What can you do with it? <a href="articles/crisps/1">Read more...</a></p>
   </section>
   <ul>
    <li> <a href="/about">About us...</a>
    </li><li> <a href="/feedback">Send feedback!</a>
    </li><li> <a href="/sitemap">Sitemap</a>
   </li></ul>
  </nav>
  <p><small>Copyright © 2015 The Snacker —
  <a href="/tos">Terms of Service</a></small></p>
 </footer>
</body></html>)";

    BOOST_CHECK(parse(in) == s);
}

/*
4.3.10 The address element

https://html.spec.whatwg.org/multipage/sections.html#the-address-element
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_sections_27)
{
    char32_t const* in = UR"(<ADDRESS>
 <A href="../People/Raggett/">Dave Raggett</A>,
 <A href="../People/Arnaud/">Arnaud Le Hors</A>,
 contact persons for the <A href="Activity">W3C HTML Activity</A>
</ADDRESS>)";

    char32_t const* s = UR"(<html><head></head><body><address>
 <a href="../People/Raggett/">Dave Raggett</a>,
 <a href="../People/Arnaud/">Arnaud Le Hors</a>,
 contact persons for the <a href="Activity">W3C HTML Activity</a>
</address></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_sections_28)
{
    char32_t const* in = UR"(<ADDRESS>Last Modified: 1999/12/24 23:37:50</ADDRESS>)";

    char32_t const* s = UR"(<html><head></head><body><address>Last Modified: 1999/12/24 23:37:50</address></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_sections_29)
{
    char32_t const* in = UR"(<footer>
 <address>
  For more details, contact
  <a href="mailto:js@example.com">John Smith</a>.
 </address>
 <p><small>© copyright 2038 Example Corp.</small></p>
</footer>)";

    char32_t const* s = UR"(<html><head></head><body><footer>
 <address>
  For more details, contact
  <a href="mailto:js@example.com">John Smith</a>.
 </address>
 <p><small>© copyright 2038 Example Corp.</small></p>
</footer></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

/*
4.3.11 Headings and sections

https://html.spec.whatwg.org/multipage/sections.html#headings-and-sections
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_sections_30)
{
    char32_t const* in = UR"(<body>
 <h1>Foo</h1>
 <h2>Bar</h2>
 <blockquote>
  <h3>Bla</h3>
 </blockquote>
 <p>Baz</p>
 <h2>Quux</h2>
 <section>
  <h3>Thud</h3>
 </section>
 <p>Grunt</p>
</body>)";

    char32_t const* s = UR"(<html><head></head><body>
 <h1>Foo</h1>
 <h2>Bar</h2>
 <blockquote>
  <h3>Bla</h3>
 </blockquote>
 <p>Baz</p>
 <h2>Quux</h2>
 <section>
  <h3>Thud</h3>
 </section>
 <p>Grunt</p>
</body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_sections_31)
{
    char32_t const* in = UR"(<body>
 <h4>Apples</h4>
 <p>Apples are fruit.</p>
 <section>
  <h2>Taste</h2>
  <p>They taste lovely.</p>
  <h6>Sweet</h6>
  <p>Red apples are sweeter than green ones.</p>
  <h1>Color</h1>
  <p>Apples come in various colors.</p>
 </section>
</body>)";

    char32_t const* s = UR"(<html><head></head><body>
 <h4>Apples</h4>
 <p>Apples are fruit.</p>
 <section>
  <h2>Taste</h2>
  <p>They taste lovely.</p>
  <h6>Sweet</h6>
  <p>Red apples are sweeter than green ones.</p>
  <h1>Color</h1>
  <p>Apples come in various colors.</p>
 </section>
</body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_sections_32)
{
    char32_t const* in = UR"(<body>
 <h1>Apples</h1>
 <p>Apples are fruit.</p>
 <section>
  <h2>Taste</h2>
  <p>They taste lovely.</p>
  <section>
   <h3>Sweet</h3>
   <p>Red apples are sweeter than green ones.</p>
  </section>
 </section>
 <section>
  <h2>Color</h2>
  <p>Apples come in various colors.</p>
 </section>
</body>)";

    char32_t const* s = UR"(<html><head></head><body>
 <h1>Apples</h1>
 <p>Apples are fruit.</p>
 <section>
  <h2>Taste</h2>
  <p>They taste lovely.</p>
  <section>
   <h3>Sweet</h3>
   <p>Red apples are sweeter than green ones.</p>
  </section>
 </section>
 <section>
  <h2>Color</h2>
  <p>Apples come in various colors.</p>
 </section>
</body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_sections_33)
{
    char32_t const* in = UR"(<body>
 <h1>Apples</h1>
 <p>Apples are fruit.</p>
 <section>
  <h1>Taste</h1>
  <p>They taste lovely.</p>
  <section>
   <h1>Sweet</h1>
   <p>Red apples are sweeter than green ones.</p>
  </section>
 </section>
 <section>
  <h1>Color</h1>
  <p>Apples come in various colors.</p>
 </section>
</body>)";

    char32_t const* s = UR"(<html><head></head><body>
 <h1>Apples</h1>
 <p>Apples are fruit.</p>
 <section>
  <h1>Taste</h1>
  <p>They taste lovely.</p>
  <section>
   <h1>Sweet</h1>
   <p>Red apples are sweeter than green ones.</p>
  </section>
 </section>
 <section>
  <h1>Color</h1>
  <p>Apples come in various colors.</p>
 </section>
</body></html>)";

    BOOST_CHECK(parse(in) == s);
}

/*
4.3.11.1 Creating an outline

https://html.spec.whatwg.org/multipage/sections.html#outlines
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_sections_34)
{
    char32_t const* in = UR"(<body>
  <hgroup id="document-title">
    <h1>HTML</h1>
    <h2>Living Standard — Last Updated 12 August 2016</h2>
  </hgroup>
  <p>Some intro to the document.</p>
  <h2>Table of contents</h2>
  <ol id=toc>...</ol>
  <h2>First section</h2>
  <p>Some intro to the first section.</p>
</body>)";

    char32_t const* s = UR"(<html><head></head><body>
  <hgroup id="document-title">
    <h1>HTML</h1>
    <h2>Living Standard — Last Updated 12 August 2016</h2>
  </hgroup>
  <p>Some intro to the document.</p>
  <h2>Table of contents</h2>
  <ol id="toc">...</ol>
  <h2>First section</h2>
  <p>Some intro to the first section.</p>
</body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_sections_35)
{
    char32_t const* in = UR"(<body>
 <nav>
  <p><a href="/">Home</a></p>
 </nav>
 <p>Hello world.</p>
 <aside>
  <p>My cat is cute.</p>
 </aside>
</body>)";

    char32_t const* s = UR"(<html><head></head><body>
 <nav>
  <p><a href="/">Home</a></p>
 </nav>
 <p>Hello world.</p>
 <aside>
  <p>My cat is cute.</p>
 </aside>
</body></html>)";

    BOOST_CHECK(parse(in) == s);
}

/*
4.3.11.2 Sample outlines

https://html.spec.whatwg.org/multipage/sections.html#sample-outlines
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_sections_36)
{
    char32_t const* in = UR"(<!DOCTYPE HTML>
<html lang=en>
<title>The Tax Book (all in one page)</title>
<h1>The Tax Book</h1>
<h2>Earning money</h2>
<p>Earning money is good.</p>
<h3>Getting a job</h3>
<p>To earn money you typically need a job.</p>
<h2>Spending money</h2>
<p>Spending is what money is mainly used for.</p>
<h3>Cheap things</h3>
<p>Buying cheap things often not cost-effective.</p>
<h3>Expensive things</h3>
<p>The most expensive thing is often not the most cost-effective either.</p>
<h2>Investing money</h2>
<p>You can lend your money to other people.</p>
<h2>Losing money</h2>
<p>If you spend money or invest money, sooner or later you will lose money.
<h3>Poor judgement</h3>
<p>Usually if you lose money it's because you made a mistake.</p>)";

    char32_t const* s = UR"(<!DOCTYPE html><html lang="en"><head><title>The Tax Book (all in one page)</title>
</head><body><h1>The Tax Book</h1>
<h2>Earning money</h2>
<p>Earning money is good.</p>
<h3>Getting a job</h3>
<p>To earn money you typically need a job.</p>
<h2>Spending money</h2>
<p>Spending is what money is mainly used for.</p>
<h3>Cheap things</h3>
<p>Buying cheap things often not cost-effective.</p>
<h3>Expensive things</h3>
<p>The most expensive thing is often not the most cost-effective either.</p>
<h2>Investing money</h2>
<p>You can lend your money to other people.</p>
<h2>Losing money</h2>
<p>If you spend money or invest money, sooner or later you will lose money.
</p><h3>Poor judgement</h3>
<p>Usually if you lose money it's because you made a mistake.</p></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_sections_37)
{
    char32_t const* in = UR"(<!DOCTYPE HTML>
<html lang=en>
<title>The Tax Book (all in one page)</title>
<h1>The Tax Book</h1>
<section>
 <h1>Earning money</h1>
 <p>Earning money is good.</p>
 <section>
  <h1>Getting a job</h1>
  <p>To earn money you typically need a job.</p>
 </section>
</section>
<section>
 <h1>Spending money</h1>
 <p>Spending is what money is mainly used for.</p>
 <section>
  <h1>Cheap things</h1>
  <p>Buying cheap things often not cost-effective.</p>
 </section>
 <section>
  <h1>Expensive things</h1>
  <p>The most expensive thing is often not the most cost-effective either.</p>
 </section>
</section>
<section>
 <h1>Investing money</h1>
 <p>You can lend your money to other people.</p>
</section>
<section>
 <h1>Losing money</h1>
 <p>If you spend money or invest money, sooner or later you will lose money.
 <section>
  <h1>Poor judgement</h1>
  <p>Usually if you lose money it's because you made a mistake.</p>
 </section>
</section>)";

    char32_t const* s = UR"(<!DOCTYPE html><html lang="en"><head><title>The Tax Book (all in one page)</title>
</head><body><h1>The Tax Book</h1>
<section>
 <h1>Earning money</h1>
 <p>Earning money is good.</p>
 <section>
  <h1>Getting a job</h1>
  <p>To earn money you typically need a job.</p>
 </section>
</section>
<section>
 <h1>Spending money</h1>
 <p>Spending is what money is mainly used for.</p>
 <section>
  <h1>Cheap things</h1>
  <p>Buying cheap things often not cost-effective.</p>
 </section>
 <section>
  <h1>Expensive things</h1>
  <p>The most expensive thing is often not the most cost-effective either.</p>
 </section>
</section>
<section>
 <h1>Investing money</h1>
 <p>You can lend your money to other people.</p>
</section>
<section>
 <h1>Losing money</h1>
 <p>If you spend money or invest money, sooner or later you will lose money.
 </p><section>
  <h1>Poor judgement</h1>
  <p>Usually if you lose money it's because you made a mistake.</p>
 </section>
</section></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_sections_38)
{
    char32_t const* in = UR"(<!DOCTYPE HTML>
<html lang=en>
<title>Alphabetic Fruit</title>
<h1>Apples</h1>
<p>Pomaceous.</p>
<h1>Bananas</h1>
<p>Edible.</p>
<h1>Carambola</h1>
<p>Star.</p>)";

    char32_t const* s = UR"(<!DOCTYPE html><html lang="en"><head><title>Alphabetic Fruit</title>
</head><body><h1>Apples</h1>
<p>Pomaceous.</p>
<h1>Bananas</h1>
<p>Edible.</p>
<h1>Carambola</h1>
<p>Star.</p></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_sections_39)
{
    char32_t const* in = UR"(<!DOCTYPE HTML>
<html lang=en>
<title>Alphabetic Fruit</title>
<section>
 <h1>Apples</h1>
 <p>Pomaceous.</p>
 <h1>Bananas</h1>
 <p>Edible.</p>
 <h1>Carambola</h1>
 <p>Star.</p>
</section>)";

    char32_t const* s = UR"(<!DOCTYPE html><html lang="en"><head><title>Alphabetic Fruit</title>
</head><body><section>
 <h1>Apples</h1>
 <p>Pomaceous.</p>
 <h1>Bananas</h1>
 <p>Edible.</p>
 <h1>Carambola</h1>
 <p>Star.</p>
</section></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_sections_40)
{
    char32_t const* in = UR"(<!DOCTYPE HTML>
<html lang=en>
<title>Feathers on The Site of Encyclopedic Knowledge</title>
<section>
 <h1>A plea from our caretakers</h1>
 <p>Please, we beg of you, send help! We're stuck in the server room!</p>
</section>
<h1>Feathers</h1>
<p>Epidermal growths.</p>)";

    char32_t const* s = UR"(<!DOCTYPE html><html lang="en"><head><title>Feathers on The Site of Encyclopedic Knowledge</title>
</head><body><section>
 <h1>A plea from our caretakers</h1>
 <p>Please, we beg of you, send help! We're stuck in the server room!</p>
</section>
<h1>Feathers</h1>
<p>Epidermal growths.</p></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_sections_41)
{
    char32_t const* in = UR"(<!DOCTYPE HTML>
<html lang="en">
<title>We're adopting a child! — Ray's blog</title>
<h1>Ray's blog</h1>
<article>
 <header>
  <nav>
   <a href="?t=-1d">Yesterday</a>;
   <a href="?t=-7d">Last week</a>;
   <a href="?t=-1m">Last month</a>
  </nav>
  <h1>We're adopting a child!</h1>
 </header>
 <p>As of today, Janine and I have signed the papers to become
 the proud parents of baby Diane! We've been looking forward to
 this day for weeks.</p>
</article>
</html>)";

    char32_t const* s = UR"(<!DOCTYPE html><html lang="en"><head><title>We're adopting a child! — Ray's blog</title>
</head><body><h1>Ray's blog</h1>
<article>
 <header>
  <nav>
   <a href="?t=-1d">Yesterday</a>;
   <a href="?t=-7d">Last week</a>;
   <a href="?t=-1m">Last month</a>
  </nav>
  <h1>We're adopting a child!</h1>
 </header>
 <p>As of today, Janine and I have signed the papers to become
 the proud parents of baby Diane! We've been looking forward to
 this day for weeks.</p>
</article>
</body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_sections_42)
{
    char32_t const* in = UR"(<!DOCTYPE HTML>
<html lang="en">
<title>Chronotype: CS Student</title>
<hgroup>
 <h1> The morning </h1>
 <h2> 06:00 to 12:00 </h2>
</hgroup>
<p>We sleep.</p>
<hgroup>
 <h1> The afternoon </h1>
 <h2> 12:00 to 18:00 </h2>
</hgroup>
<p>We study.</p>
<hgroup>
 <h2>Additional Commentary</h2>
 <h3>Because not all this is necessarily true</h3>
 <h6>Ok it's almost certainly not true</h6>
</hgroup>
<p>Yeah we probably play, rather than study.</p>
<hgroup>
 <h1> The evening </h1>
 <h2> 18:00 to 00:00 </h2>
</hgroup>
<p>We play.</p>
<hgroup>
 <h1> The night </h1>
 <h2> 00:00 to 06:00 </h2>
</hgroup>
<p>We play some more.</p>
</html>)";

    char32_t const* s = UR"(<!DOCTYPE html><html lang="en"><head><title>Chronotype: CS Student</title>
</head><body><hgroup>
 <h1> The morning </h1>
 <h2> 06:00 to 12:00 </h2>
</hgroup>
<p>We sleep.</p>
<hgroup>
 <h1> The afternoon </h1>
 <h2> 12:00 to 18:00 </h2>
</hgroup>
<p>We study.</p>
<hgroup>
 <h2>Additional Commentary</h2>
 <h3>Because not all this is necessarily true</h3>
 <h6>Ok it's almost certainly not true</h6>
</hgroup>
<p>Yeah we probably play, rather than study.</p>
<hgroup>
 <h1> The evening </h1>
 <h2> 18:00 to 00:00 </h2>
</hgroup>
<p>We play.</p>
<hgroup>
 <h1> The night </h1>
 <h2> 00:00 to 06:00 </h2>
</hgroup>
<p>We play some more.</p>
</body></html>)";

    BOOST_CHECK(parse(in) == s);
}

/*
4.3.12 Usage summary

https://html.spec.whatwg.org/multipage/sections.html#usage-summary-2
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_sections_43)
{
    char32_t const* in = UR"(<!DOCTYPE HTML>
<html lang="en">
 <head> <title>Steve Hill's Home Page</title> </head>
 <body> <p>Hard Trance is My Life.</p> </body>
</html>)";

    char32_t const* s = UR"(<!DOCTYPE html><html lang="en"><head> <title>Steve Hill's Home Page</title> </head>
 <body> <p>Hard Trance is My Life.</p> 
</body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_sections_44)
{
    char32_t const* in = UR"(<article>
 <img src="/tumblr_masqy2s5yn1rzfqbpo1_500.jpg" alt="Yellow smiley face with the caption 'masif'">
 <p>My fave Masif tee so far!</p>
 <footer>Posted 2 days ago</footer>
</article>
<article>
 <img src="/tumblr_m9tf6wSr6W1rzfqbpo1_500.jpg" alt="">
 <p>Happy 2nd birthday Masif Saturdays!!!</p>
 <footer>Posted 3 weeks ago</footer>
</article>)";

    char32_t const* s = UR"(<html><head></head><body><article>
 <img src="/tumblr_masqy2s5yn1rzfqbpo1_500.jpg" alt="Yellow smiley face with the caption 'masif'">
 <p>My fave Masif tee so far!</p>
 <footer>Posted 2 days ago</footer>
</article>
<article>
 <img src="/tumblr_m9tf6wSr6W1rzfqbpo1_500.jpg" alt="">
 <p>Happy 2nd birthday Masif Saturdays!!!</p>
 <footer>Posted 3 weeks ago</footer>
</article></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_sections_45)
{
    char32_t const* in = UR"(<h1>Biography</h1>
<section>
 <h1>The facts</h1>
 <p>1500+ shows, 14+ countries</p>
</section>
<section>
 <h1>2010/2011 figures per year</h1>
 <p>100+ shows, 8+ countries</p>
</section>)";

    char32_t const* s = UR"(<html><head></head><body><h1>Biography</h1>
<section>
 <h1>The facts</h1>
 <p>1500+ shows, 14+ countries</p>
</section>
<section>
 <h1>2010/2011 figures per year</h1>
 <p>100+ shows, 8+ countries</p>
</section></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_sections_46)
{
    char32_t const* in = UR"(<nav>
 <p><a href="/">Home</a>
 <p><a href="/biog.html">Bio</a>
 <p><a href="/discog.html">Discog</a>
</nav>)";

    char32_t const* s = UR"(<html><head></head><body><nav>
 <p><a href="/">Home</a>
 </p><p><a href="/biog.html">Bio</a>
 </p><p><a href="/discog.html">Discog</a>
</p></nav></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_sections_47)
{
    char32_t const* in = UR"(<h1>Music</h1>
<p>As any burner can tell you, the event has a lot of trance.</p>
<aside>You can buy the music we played at our <a href="buy.html">playlist page</a>.</aside>
<p>This year we played a kind of trance that originated in Belgium, Germany, and the Netherlands in the mid 90s.</p>)";

    char32_t const* s = UR"(<html><head></head><body><h1>Music</h1>
<p>As any burner can tell you, the event has a lot of trance.</p>
<aside>You can buy the music we played at our <a href="buy.html">playlist page</a>.</aside>
<p>This year we played a kind of trance that originated in Belgium, Germany, and the Netherlands in the mid 90s.</p></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_sections_48)
{
    char32_t const* in = UR"(<h1>The Guide To Music On The Playa</h1>
<h2>The Main Stage</h2>
<p>If you want to play on a stage, you should bring one.</p>
<h2>Amplified Music</h2>
<p>Amplifiers up to 300W or 90dB are welcome.</p>)";

    char32_t const* s = UR"(<html><head></head><body><h1>The Guide To Music On The Playa</h1>
<h2>The Main Stage</h2>
<p>If you want to play on a stage, you should bring one.</p>
<h2>Amplified Music</h2>
<p>Amplifiers up to 300W or 90dB are welcome.</p></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_sections_49)
{
    char32_t const* in = UR"(<hgroup>
 <h1>Burning Music</h1>
 <h2>The Guide To Music On The Playa</h2>
</hgroup>
<section>
 <hgroup>
  <h1>Main Stage</h1>
  <h2>The Fiction Of A Music Festival</h2>
 </hgroup>
 <p>If you want to play on a stage, you should bring one.</p>
</section>
<section>
 <hgroup>
  <h1>Loudness!</h1>
  <h2>Questions About Amplified Music</h2>
 </hgroup>
 <p>Amplifiers up to 300W or 90dB are welcome.</p>
</section>)";

    char32_t const* s = UR"(<html><head></head><body><hgroup>
 <h1>Burning Music</h1>
 <h2>The Guide To Music On The Playa</h2>
</hgroup>
<section>
 <hgroup>
  <h1>Main Stage</h1>
  <h2>The Fiction Of A Music Festival</h2>
 </hgroup>
 <p>If you want to play on a stage, you should bring one.</p>
</section>
<section>
 <hgroup>
  <h1>Loudness!</h1>
  <h2>Questions About Amplified Music</h2>
 </hgroup>
 <p>Amplifiers up to 300W or 90dB are welcome.</p>
</section></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_sections_50)
{
    char32_t const* in = UR"(<article>
 <header>
  <h1>Hard Trance is My Life</h1>
  <p>By DJ Steve Hill and Technikal</p>
 </header>
 <p>The album with the amusing punctuation has red artwork.</p>
</article>)";

    char32_t const* s = UR"(<html><head></head><body><article>
 <header>
  <h1>Hard Trance is My Life</h1>
  <p>By DJ Steve Hill and Technikal</p>
 </header>
 <p>The album with the amusing punctuation has red artwork.</p>
</article></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_sections_51)
{
    char32_t const* in = UR"(<article>
 <h1>Hard Trance is My Life</h1>
 <p>The album with the amusing punctuation has red artwork.</p>
 <footer>
  <p>Artists: DJ Steve Hill and Technikal</p>
 </footer>
</article>)";

    char32_t const* s = UR"(<html><head></head><body><article>
 <h1>Hard Trance is My Life</h1>
 <p>The album with the amusing punctuation has red artwork.</p>
 <footer>
  <p>Artists: DJ Steve Hill and Technikal</p>
 </footer>
</article></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_SUITE_END()
