// test/html/simple_html_sample_images.cpp

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

BOOST_AUTO_TEST_SUITE(simple_html_sample_images_test)

/*
4.8.4.1 Introduction

https://html.spec.whatwg.org/multipage/images.html#introduction-3
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_images_1)
{
	char32_t const* in = UR"(<h2>From today's featured article</h2>
<img src="/uploads/100-marie-lloyd.jpg" alt="" width="100" height="150">
<p><b><a href="/wiki/Marie_Lloyd">Marie Lloyd</a></b> (1870–1922)
was an English <a href="/wiki/Music_hall">music hall</a> singer, ...)";

	char32_t const* s = UR"(<html><head></head><body><h2>From today's featured article</h2>
<img src="/uploads/100-marie-lloyd.jpg" alt="" width="100" height="150">
<p><b><a href="/wiki/Marie_Lloyd">Marie Lloyd</a></b> (1870–1922)
was an English <a href="/wiki/Music_hall">music hall</a> singer, ...</p></body></html>)";

	BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_images_2)
{
	char32_t const* in = UR"(<h2>From today's featured article</h2>
<img src="/uploads/100-marie-lloyd.jpg"
     srcset="/uploads/150-marie-lloyd.jpg 1.5x, /uploads/200-marie-lloyd.jpg 2x"
     alt="" width="100" height="150">
<p><b><a href="/wiki/Marie_Lloyd">Marie Lloyd</a></b> (1870–1922)
was an English <a href="/wiki/Music_hall">music hall</a> singer, ...)";

	char32_t const* s = UR"(<html><head></head><body><h2>From today's featured article</h2>
<img src="/uploads/100-marie-lloyd.jpg" srcset="/uploads/150-marie-lloyd.jpg 1.5x, /uploads/200-marie-lloyd.jpg 2x" alt="" width="100" height="150">
<p><b><a href="/wiki/Marie_Lloyd">Marie Lloyd</a></b> (1870–1922)
was an English <a href="/wiki/Music_hall">music hall</a> singer, ...</p></body></html>)";

	BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_images_3)
{
	char32_t const* in = UR"(<h1><img sizes="100vw" srcset="wolf-400.jpg 400w, wolf-800.jpg 800w, wolf-1600.jpg 1600w"
     src="wolf-400.jpg" alt="The rad wolf"></h1>)";

	char32_t const* s = UR"(<html><head></head><body><h1><img sizes="100vw" srcset="wolf-400.jpg 400w, wolf-800.jpg 800w, wolf-1600.jpg 1600w" src="wolf-400.jpg" alt="The rad wolf"></h1></body></html>)";

	BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_images_4)
{
	char32_t const* in = UR"*(<img sizes="(max-width: 30em) 100vw, (max-width: 50em) 50vw, calc(33vw - 100px)"
     srcset="swing-200.jpg 200w, swing-400.jpg 400w, swing-800.jpg 800w, swing-1600.jpg 1600w"
     src="swing-400.jpg" alt="Kettlebell Swing">)*";

	char32_t const* s = UR"*(<html><head></head><body><img sizes="(max-width: 30em) 100vw, (max-width: 50em) 50vw, calc(33vw - 100px)" srcset="swing-200.jpg 200w, swing-400.jpg 400w, swing-800.jpg 800w, swing-1600.jpg 1600w" src="swing-400.jpg" alt="Kettlebell Swing"></body></html>)*";

	BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_images_5)
{
	char32_t const* in = UR"*(<picture>
  <source media="(min-width: 45em)" srcset="large.jpg">
  <source media="(min-width: 32em)" srcset="med.jpg">
  <img src="small.jpg" alt="The wolf runs through the snow.">
</picture>)*";

	char32_t const* s = UR"*(<html><head></head><body><picture>
  <source media="(min-width: 45em)" srcset="large.jpg">
  <source media="(min-width: 32em)" srcset="med.jpg">
  <img src="small.jpg" alt="The wolf runs through the snow.">
</picture></body></html>)*";

	BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_images_6)
{
	char32_t const* in = UR"*(<h1>
 <picture>
  <source media="(max-width: 500px)" srcset="banner-phone.jpeg, banner-phone-HD.jpeg 2x">
  <img src="banner.jpeg" srcset="banner-HD.jpeg 2x" alt="The Breakfast Combo">
 </picture>
</h1>)*";

	char32_t const* s = UR"*(<html><head></head><body><h1>
 <picture>
  <source media="(max-width: 500px)" srcset="banner-phone.jpeg, banner-phone-HD.jpeg 2x">
  <img src="banner.jpeg" srcset="banner-HD.jpeg 2x" alt="The Breakfast Combo">
 </picture>
</h1></body></html>)*";

	BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_images_7)
{
	char32_t const* in = UR"(<h2>From today's featured article</h2>
<picture>
 <source srcset="/uploads/100-marie-lloyd.webp" type="image/webp">
 <source srcset="/uploads/100-marie-lloyd.jxr" type="image/vnd.ms-photo">
 <img src="/uploads/100-marie-lloyd.jpg" alt="" width="100" height="150">
</picture>
<p><b><a href="/wiki/Marie_Lloyd">Marie Lloyd</a></b> (1870–1922)
was an English <a href="/wiki/Music_hall">music hall</a> singer, ...)";

	char32_t const* s = UR"(<html><head></head><body><h2>From today's featured article</h2>
<picture>
 <source srcset="/uploads/100-marie-lloyd.webp" type="image/webp">
 <source srcset="/uploads/100-marie-lloyd.jxr" type="image/vnd.ms-photo">
 <img src="/uploads/100-marie-lloyd.jpg" alt="" width="100" height="150">
</picture>
<p><b><a href="/wiki/Marie_Lloyd">Marie Lloyd</a></b> (1870–1922)
was an English <a href="/wiki/Music_hall">music hall</a> singer, ...</p></body></html>)";

	BOOST_CHECK(parse(in) == s);
}

/*
4.8.4.1.1 Adaptive images

https://html.spec.whatwg.org/multipage/images.html#adaptive-images
*/

BOOST_AUTO_TEST_SUITE_END()
