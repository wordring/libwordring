// test/html/simple_html_sample_edits.cpp

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

BOOST_AUTO_TEST_SUITE(simple_html_sample_edits_test)

/*
4.7.1 The ins element

https://html.spec.whatwg.org/multipage/edits.html#the-ins-element
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_edits_1)
{
	char32_t const* in = UR"(<aside>
 <ins>
  <p> I like fruit. </p>
 </ins>
</aside>
<aside>
 <ins>
  Apples are <em>tasty</em>.
 </ins>
 <ins>
  So are pears.
 </ins>
</aside>)";

	char32_t const* s = UR"(<html><head></head><body><aside>
 <ins>
  <p> I like fruit. </p>
 </ins>
</aside>
<aside>
 <ins>
  Apples are <em>tasty</em>.
 </ins>
 <ins>
  So are pears.
 </ins>
</aside></body></html>)";

	BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_edits_2)
{
	char32_t const* in = UR"(<aside>
 <!-- don't do this -->
 <ins datetime="2005-03-16 00:00Z">
  <p> I like fruit. </p>
  Apples are <em>tasty</em>.
 </ins>
 <ins datetime="2007-12-19 00:00Z">
  So are pears.
 </ins>
</aside>
<aside>
 <ins datetime="2005-03-16 00:00Z">
  <p> I like fruit. </p>
 </ins>
 <ins datetime="2005-03-16 00:00Z">
  Apples are <em>tasty</em>.
 </ins>
 <ins datetime="2007-12-19 00:00Z">
  So are pears.
 </ins>
</aside>)";

	char32_t const* s = UR"(<html><head></head><body><aside>
 <!-- don't do this -->
 <ins datetime="2005-03-16 00:00Z">
  <p> I like fruit. </p>
  Apples are <em>tasty</em>.
 </ins>
 <ins datetime="2007-12-19 00:00Z">
  So are pears.
 </ins>
</aside>
<aside>
 <ins datetime="2005-03-16 00:00Z">
  <p> I like fruit. </p>
 </ins>
 <ins datetime="2005-03-16 00:00Z">
  Apples are <em>tasty</em>.
 </ins>
 <ins datetime="2007-12-19 00:00Z">
  So are pears.
 </ins>
</aside></body></html>)";

	BOOST_CHECK(parse(in) == s);
}

/*
4.7.2 The del element

https://html.spec.whatwg.org/multipage/edits.html#the-del-element
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_edits_3)
{
	char32_t const* in = UR"(<h1>To Do</h1>
<ul>
 <li>Empty the dishwasher</li>
 <li><del datetime="2009-10-11T01:25-07:00">Watch Walter Lewin's lectures</del></li>
 <li><del datetime="2009-10-10T23:38-07:00">Download more tracks</del></li>
 <li>Buy a printer</li>
</ul>)";

	char32_t const* s = UR"(<html><head></head><body><h1>To Do</h1>
<ul>
 <li>Empty the dishwasher</li>
 <li><del datetime="2009-10-11T01:25-07:00">Watch Walter Lewin's lectures</del></li>
 <li><del datetime="2009-10-10T23:38-07:00">Download more tracks</del></li>
 <li>Buy a printer</li>
</ul></body></html>)";

	BOOST_CHECK(parse(in) == s);
}

/*
4.7.4 Edits and paragraphs

https://html.spec.whatwg.org/multipage/edits.html#edits-and-paragraphs
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_edits_4)
{
	char32_t const* in = UR"(<section>
 <ins>
  <p>
   This is a paragraph that was inserted.
  </p>
  This is another paragraph whose first sentence was inserted
  at the same time as the paragraph above.
 </ins>
 This is a second sentence, which was there all along.
</section>)";

	char32_t const* s = UR"(<html><head></head><body><section>
 <ins>
  <p>
   This is a paragraph that was inserted.
  </p>
  This is another paragraph whose first sentence was inserted
  at the same time as the paragraph above.
 </ins>
 This is a second sentence, which was there all along.
</section></body></html>)";

	BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_edits_5)
{
	char32_t const* in = UR"(<section>
 This is the first paragraph. <ins>This sentence was
 inserted.
 <p>This second paragraph was inserted.</p>
 This sentence was inserted too.</ins> This is the
 third paragraph in this example.
 <!-- (don't do this) -->
</section>)";

	char32_t const* s = UR"(<html><head></head><body><section>
 This is the first paragraph. <ins>This sentence was
 inserted.
 <p>This second paragraph was inserted.</p>
 This sentence was inserted too.</ins> This is the
 third paragraph in this example.
 <!-- (don't do this) -->
</section></body></html>)";

	BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_edits_6)
{
	char32_t const* in = UR"(<section>
 <p>This is the first paragraph. <del>This sentence was
 deleted.</del></p>
 <p><del>This sentence was deleted too.</del> That
 sentence needed a separate &lt;del&gt; element.</p>
</section>)";

	char32_t const* s = UR"(<html><head></head><body><section>
 <p>This is the first paragraph. <del>This sentence was
 deleted.</del></p>
 <p><del>This sentence was deleted too.</del> That
 sentence needed a separate &lt;del&gt; element.</p>
</section></body></html>)";

	BOOST_CHECK(parse(in) == s);
}

/*
4.7.5 Edits and lists

https://html.spec.whatwg.org/multipage/edits.html#edits-and-lists
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_edits_7)
{
	char32_t const* in = UR"(<h1>Stop-ship bugs</h1>
<ol>
 <li><ins datetime="2008-02-12T15:20Z">Bug 225:
 Rain detector doesn't work in snow</ins></li>
 <li><del datetime="2008-03-01T20:22Z"><ins datetime="2008-02-14T12:02Z">Bug 228:
 Water buffer overflows in April</ins></del></li>
 <li><ins datetime="2008-02-16T13:50Z">Bug 230:
 Water heater doesn't use renewable fuels</ins></li>
 <li><del datetime="2008-02-20T21:15Z"><ins datetime="2008-02-16T14:25Z">Bug 232:
 Carbon dioxide emissions detected after startup</ins></del></li>
</ol>)";

	char32_t const* s = UR"(<html><head></head><body><h1>Stop-ship bugs</h1>
<ol>
 <li><ins datetime="2008-02-12T15:20Z">Bug 225:
 Rain detector doesn't work in snow</ins></li>
 <li><del datetime="2008-03-01T20:22Z"><ins datetime="2008-02-14T12:02Z">Bug 228:
 Water buffer overflows in April</ins></del></li>
 <li><ins datetime="2008-02-16T13:50Z">Bug 230:
 Water heater doesn't use renewable fuels</ins></li>
 <li><del datetime="2008-02-20T21:15Z"><ins datetime="2008-02-16T14:25Z">Bug 232:
 Carbon dioxide emissions detected after startup</ins></del></li>
</ol></body></html>)";

	BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_edits_8)
{
	char32_t const* in = UR"(<h1>List of <del>fruits</del><ins>colors</ins></h1>
<ul>
 <li><del>Lime</del><ins>Green</ins></li>
 <li><del>Apple</del></li>
 <li>Orange</li>
 <li><del>Pear</del></li>
 <li><ins>Teal</ins></li>
 <li><del>Lemon</del><ins>Yellow</ins></li>
 <li>Olive</li>
 <li><ins>Purple</ins></li>
</ul>)";

	char32_t const* s = UR"(<html><head></head><body><h1>List of <del>fruits</del><ins>colors</ins></h1>
<ul>
 <li><del>Lime</del><ins>Green</ins></li>
 <li><del>Apple</del></li>
 <li>Orange</li>
 <li><del>Pear</del></li>
 <li><ins>Teal</ins></li>
 <li><del>Lemon</del><ins>Yellow</ins></li>
 <li>Olive</li>
 <li><ins>Purple</ins></li>
</ul></body></html>)";

	BOOST_CHECK(parse(in) == s);
}

/*
4.7.6 Edits and tables

https://html.spec.whatwg.org/multipage/edits.html#edits-and-tables
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_edits_9)
{
	char32_t const* in = UR"(<table>
 <thead>
  <tr> <th> Game name           <th> Game publisher   <th> Verdict
 <tbody>
  <tr> <td> Diablo 2            <td> Blizzard         <td> 8/10
  <tr> <td> Portal              <td> Valve            <td> 10/10
  <tr> <td> <ins>Portal 2</ins> <td> <ins>Valve</ins> <td> <ins>10/10</ins>
</table>
<table>
 <thead>
  <tr> <th> Game name           <th> Game publisher   <th> <del cite="/edits/r192" datetime="2011-05-02 14:23Z">Verdict</del>
 <tbody>
  <tr> <td> Diablo 2            <td> Blizzard         <td> <del cite="/edits/r192" datetime="2011-05-02 14:23Z">8/10</del>
  <tr> <td> Portal              <td> Valve            <td> <del cite="/edits/r192" datetime="2011-05-02 14:23Z">10/10</del>
  <tr> <td> Portal 2            <td> Valve            <td> <del cite="/edits/r192" datetime="2011-05-02 14:23Z">10/10</del>
</table>)";

	char32_t const* s = UR"(<html><head></head><body><table>
 <thead>
  <tr> <th> Game name           </th><th> Game publisher   </th><th> Verdict
 </th></tr></thead><tbody>
  <tr> <td> Diablo 2            </td><td> Blizzard         </td><td> 8/10
  </td></tr><tr> <td> Portal              </td><td> Valve            </td><td> 10/10
  </td></tr><tr> <td> <ins>Portal 2</ins> </td><td> <ins>Valve</ins> </td><td> <ins>10/10</ins>
</td></tr></tbody></table>
<table>
 <thead>
  <tr> <th> Game name           </th><th> Game publisher   </th><th> <del cite="/edits/r192" datetime="2011-05-02 14:23Z">Verdict</del>
 </th></tr></thead><tbody>
  <tr> <td> Diablo 2            </td><td> Blizzard         </td><td> <del cite="/edits/r192" datetime="2011-05-02 14:23Z">8/10</del>
  </td></tr><tr> <td> Portal              </td><td> Valve            </td><td> <del cite="/edits/r192" datetime="2011-05-02 14:23Z">10/10</del>
  </td></tr><tr> <td> Portal 2            </td><td> Valve            </td><td> <del cite="/edits/r192" datetime="2011-05-02 14:23Z">10/10</del>
</td></tr></tbody></table></body></html>)";

	BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_SUITE_END()
