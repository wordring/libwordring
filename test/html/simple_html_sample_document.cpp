// test/html/simple_html_sample_document.cpp

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

BOOST_AUTO_TEST_SUITE(simple_html_sample_document_test)

/*
4.1.1 The html element

https://html.spec.whatwg.org/multipage/semantics.html#the-html-element
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_document_1)
{
	char32_t const* in = UR"(<!DOCTYPE html>
<html lang="en">
<head>
<title>Swapping Songs</title>
</head>
<body>
<h1>Swapping Songs</h1>
<p>Tonight I swapped some of the songs I wrote with some friends, who
gave me some of the songs they wrote. I love sharing my music.</p>
</body>
</html>)";

	char32_t const* s = UR"(<!DOCTYPE html><html lang="en"><head>
<title>Swapping Songs</title>
</head>
<body>
<h1>Swapping Songs</h1>
<p>Tonight I swapped some of the songs I wrote with some friends, who
gave me some of the songs they wrote. I love sharing my music.</p>

</body></html>)";

	BOOST_CHECK(parse(in) == s);
}

/*
4.2.1 The head element

https://html.spec.whatwg.org/multipage/semantics.html#the-head-element
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_document_2)
{
	char32_t const* in = UR"(<!doctype html>
<html lang=en>
 <head>
  <title>A document with a short head</title>
 </head>
 <body>
 ...)";

	char32_t const* s = UR"(<!DOCTYPE html><html lang="en"><head>
  <title>A document with a short head</title>
 </head>
 <body>
 ...</body></html>)";

	BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_document_3)
{
	char32_t const* in = UR"(<!DOCTYPE HTML>
<HTML LANG="EN">
 <HEAD>
  <META CHARSET="UTF-8">
  <BASE HREF="https://www.example.com/">
  <TITLE>An application with a long head</TITLE>
  <LINK REL="STYLESHEET" HREF="default.css">
  <LINK REL="STYLESHEET ALTERNATE" HREF="big.css" TITLE="Big Text">
  <SCRIPT SRC="support.js"></SCRIPT>
  <META NAME="APPLICATION-NAME" CONTENT="Long headed application">
 </HEAD>
 <BODY>
 ...)";

	char32_t const* s = UR"(<!DOCTYPE html><html lang="EN"><head>
  <meta charset="UTF-8">
  <base href="https://www.example.com/">
  <title>An application with a long head</title>
  <link rel="STYLESHEET" href="default.css">
  <link rel="STYLESHEET ALTERNATE" href="big.css" title="Big Text">
  <script src="support.js"></script>
  <meta name="APPLICATION-NAME" content="Long headed application">
 </head>
 <body>
 ...</body></html>)";

	BOOST_CHECK(parse(in) == s);
}

/*
4.2.2 The title element

https://html.spec.whatwg.org/multipage/semantics.html#the-title-element
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_document_4)
{
	char32_t const* in = UR"(<title>Introduction to The Mating Rituals of Bees</title>
    ...
  <h1>Introduction</h1>
  <p>This companion guide to the highly successful
  <cite>Introduction to Medieval Bee-Keeping</cite> book is...)";

	char32_t const* s = UR"(<html><head><title>Introduction to The Mating Rituals of Bees</title>
    </head><body>...
  <h1>Introduction</h1>
  <p>This companion guide to the highly successful
  <cite>Introduction to Medieval Bee-Keeping</cite> book is...</p></body></html>)";

	BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_document_5)
{
	char32_t const* in = UR"(  <title>Dances used during bee mating rituals</title>
    ...
  <h1>The Dances</h1>)";

	char32_t const* s = UR"(<html><head><title>Dances used during bee mating rituals</title>
    </head><body>...
  <h1>The Dances</h1></body></html>)";

	BOOST_CHECK(parse(in) == s);
}

/*
4.2.3 The base element

https://html.spec.whatwg.org/multipage/semantics.html#the-base-element
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_document_6)
{
	char32_t const* in = UR"(<!DOCTYPE html>
<html lang="en">
    <head>
        <title>This is an example for the &lt;base&gt; element</title>
        <base href="https://www.example.com/news/index.html">
    </head>
    <body>
        <p>Visit the <a href="archives.html">archives</a>.</p>
    </body>
</html>)";

	char32_t const* s = UR"(<!DOCTYPE html><html lang="en"><head>
        <title>This is an example for the &lt;base&gt; element</title>
        <base href="https://www.example.com/news/index.html">
    </head>
    <body>
        <p>Visit the <a href="archives.html">archives</a>.</p>
    
</body></html>)";

	BOOST_CHECK(parse(in) == s);
}

/*
4.2.4 The link element

https://html.spec.whatwg.org/multipage/semantics.html#the-link-element
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_document_7)
{
	char32_t const* in = UR"(<link rel="author license" href="/about">)";

	char32_t const* s = UR"(<html><head><link rel="author license" href="/about"></head><body></body></html>)";

	BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_document_8)
{
	char32_t const* in = UR"(<link disabled rel="alternate stylesheet" href="css/pooh">)";

	char32_t const* s = UR"(<html><head><link disabled="" rel="alternate stylesheet" href="css/pooh"></head><body></body></html>)";

	BOOST_CHECK(parse(in) == s);
}

/*
4.2.4.2 Processing the type attribute

https://html.spec.whatwg.org/multipage/semantics.html#processing-the-type-attribute
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_document_9)
{
    char32_t const* in = UR"(<link rel="stylesheet" href="A" type="text/plain">
<link rel="stylesheet" href="B" type="text/css">
<link rel="stylesheet" href="C">)";

    char32_t const* s = UR"(<html><head><link rel="stylesheet" href="A" type="text/plain">
<link rel="stylesheet" href="B" type="text/css">
<link rel="stylesheet" href="C"></head><body></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

/*
4.2.5.1 Standard metadata names

https://html.spec.whatwg.org/multipage/semantics.html#standard-metadata-names
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_document_10)
{
    char32_t const* in = UR"(<meta name=generator content="Frontweaver 8.2">)";

    char32_t const* s = UR"(<html><head><meta name="generator" content="Frontweaver 8.2"></head><body></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_document_11)
{
    char32_t const* in = UR"(<!DOCTYPE HTML>
<html lang="en-GB">
 <head>
  <title>Typefaces on UK motorways</title>
  <meta name="keywords" content="british,type face,font,fonts,highway,highways">
 </head>
 <body>
  ...)";

    char32_t const* s = UR"(<!DOCTYPE html><html lang="en-GB"><head>
  <title>Typefaces on UK motorways</title>
  <meta name="keywords" content="british,type face,font,fonts,highway,highways">
 </head>
 <body>
  ...</body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_document_12)
{
    char32_t const* in = UR"(<!DOCTYPE HTML>
<title>HTML Standard</title>
<meta name="theme-color" content="#3c790a">
...)";

    char32_t const* s = UR"(<!DOCTYPE html><html><head><title>HTML Standard</title>
<meta name="theme-color" content="#3c790a">
</head><body>...</body></html>)";

    BOOST_CHECK(parse(in) == s);
}

/*
4.2.5.3 Pragma directives

https://html.spec.whatwg.org/multipage/semantics.html#pragma-directives
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_document_13)
{
    char32_t const* in = UR"(<meta http-equiv="Refresh" content="300">)";

    char32_t const* s = UR"(<html><head><meta http-equiv="Refresh" content="300"></head><body></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_document_14)
{
    char32_t const* in = UR"(<meta http-equiv="Refresh" content="20; URL=page4.html">)";

    char32_t const* s = UR"(<html><head><meta http-equiv="Refresh" content="20; URL=page4.html"></head><body></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_document_15)
{
    char32_t const* in = UR"(<meta http-equiv="Content-Security-Policy" content="script-src 'self'; object-src 'none'">)";

    char32_t const* s = UR"(<html><head><meta http-equiv="Content-Security-Policy" content="script-src 'self'; object-src 'none'"></head><body></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

/*
4.2.5.4 Specifying the document's character encoding

https://html.spec.whatwg.org/multipage/semantics.html#charset
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_document_16)
{
    char32_t const* in = UR"(<meta charset="utf-8">)";

    char32_t const* s = UR"(<html><head><meta charset="utf-8"></head><body></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_CASE(simple_html_sample_document_17)
{
    char32_t const* in = UR"(<?xml version="1.0" encoding="utf-8"?>)";

    char32_t const* s = UR"(<!--?xml version="1.0" encoding="utf-8"?--><html><head></head><body></body></html>)";

    BOOST_CHECK(parse(in) == s);
}

/*
4.2.6 The style element

https://html.spec.whatwg.org/multipage/semantics.html#the-style-element
*/
BOOST_AUTO_TEST_CASE(simple_html_sample_document_18)
{
    char32_t const* in = UR"(<!DOCTYPE html>
<html lang="en-US">
 <head>
  <title>My favorite book</title>
  <style>
   body { color: black; background: white; }
   em { font-style: normal; color: red; }
  </style>
 </head>
 <body>
  <p>My <em>favorite</em> book of all time has <em>got</em> to be
  <cite>A Cat's Life</cite>. It is a book by P. Rahmel that talks
  about the <i lang="la">Felis Catus</i> in modern human society.</p>
 </body>
</html>)";

    char32_t const* s = UR"(<!DOCTYPE html><html lang="en-US"><head>
  <title>My favorite book</title>
  <style>
   body { color: black; background: white; }
   em { font-style: normal; color: red; }
  </style>
 </head>
 <body>
  <p>My <em>favorite</em> book of all time has <em>got</em> to be
  <cite>A Cat's Life</cite>. It is a book by P. Rahmel that talks
  about the <i lang="la">Felis Catus</i> in modern human society.</p>
 
</body></html>)";

    BOOST_CHECK(parse(in) == s);
}

BOOST_AUTO_TEST_SUITE_END()
