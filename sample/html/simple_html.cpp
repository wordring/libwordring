// sample/html/simple_html.cpp

#include <boost/test/unit_test.hpp>

#include <wordring/css/selector.hpp>
#include <wordring/html/simple_html.hpp>

#include <cassert>
#include <iterator>
#include <string>
#include <string_view>

BOOST_AUTO_TEST_SUITE(html_simple_html_sample)

/*
* HTML ドキュメントと文字列の相互変換
*/
BOOST_AUTO_TEST_CASE(simple_html_make_document_1)
{
	namespace html = wordring::html;

	// 文字列から HTML ドキュメントを作成する。
	std::u16string_view sv = u"<p>Hello HTML!</p>";
	auto doc = html::make_document<html::u8simple_tree>(sv.begin(), sv.end());

	// HTML を文字列化する。
	std::u16string s;
	html::to_string(doc.begin(), std::back_inserter(s));

	assert(s == u"<html><head></head><body><p>Hello HTML!</p></body></html>");
}

/*
* 文字コードはユニコードであれば十分
*/
BOOST_AUTO_TEST_CASE(simple_html_encoding_1)
{
	namespace html = wordring::html;

	// 文字列から HTML ドキュメントを作成する。
	std::u32string_view sv = U"<p>Hello HTML!</p>";
	auto doc = html::make_document<html::u16simple_tree>(sv.begin(), sv.end());

	// HTML を文字列化する。
	std::u8string s;
	html::to_string(doc.begin(), std::back_inserter(s));

	assert(s == u8"<html><head></head><body><p>Hello HTML!</p></body></html>");
}

/*
* CSS セレクタ
*/
BOOST_AUTO_TEST_CASE(simple_html_query_selector_1)
{
	namespace html = wordring::html;
	namespace css  = wordring::css;

	// 文字列から HTML ドキュメントを作成する。
	std::u32string_view sv = U"<p>Hello HTML!</p>";
	auto doc = html::make_document<html::u16simple_tree>(sv.begin(), sv.end());

	// CSS セレクタで<p>タグを検索する。
	auto it = css::query_selector(doc.begin(), u8"p");

	// <p> タグ以下を文字列化する。
	std::u8string s;
	html::to_string(it, std::back_inserter(s));

	// to_string() は、ノードの子孫を文字列化し、ノード自身を含まない。
	assert(s == u8"Hello HTML!");
}

/*
* 要素を挿入する
*/
BOOST_AUTO_TEST_CASE(simple_html_insert_1)
{
	namespace html = wordring::html;
	namespace css = wordring::css;

	// 文字列から HTML ドキュメントを作成する。
	std::u32string_view sv = U"<p>Hello HTML!</p>";
	auto doc = html::make_document<html::u16simple_tree>(sv.begin(), sv.end());

	// CSS セレクタで<body>タグを検索する。※パーサによって<body>タグが挿入されています。
	auto body = css::query_selector(doc.begin(), u8"BODY");

	// <body> タグの末尾に <div> タグを挿入する。
	auto div = html::simple_element<std::u16string>(u"div");
	doc.insert(body.end(), div);

	// <body> タグ以下を文字列化する。
	std::u16string s;
	html::to_string(body, std::back_inserter(s));

	assert(s == u"<p>Hello HTML!</p><div></div>");
}

BOOST_AUTO_TEST_CASE(simple_html_insert_2)
{
	namespace html = wordring::html;
	namespace css = wordring::css;

	// 文字列から HTML ドキュメントを作成する。
	std::u32string_view sv = U"<p>Hello HTML!</p>";
	auto doc = html::make_document<html::u16simple_tree>(sv.begin(), sv.end());

	// CSS セレクタで<body>タグを検索する。※パーサによって<body>タグが挿入されています。
	auto body = css::query_selector(doc.begin(), u8"BODY");

	// <body> タグの末尾に <div> タグを挿入する。※列挙値でタグ名を指定して要素を作成。
	auto div = html::simple_element<std::u16string>(html::tag_name::Div);
	doc.insert(body.end(), div);

	// <body> タグ以下を文字列化する。
	std::u8string s;
	html::to_string(body, std::back_inserter(s));

	assert(s == u8"<p>Hello HTML!</p><div></div>");
}

/*
* 要素を削除する
*/
BOOST_AUTO_TEST_CASE(simple_html_erase_1)
{
	namespace html = wordring::html;
	namespace css = wordring::css;

	// 文字列から HTML ドキュメントを作成する。
	std::u32string_view sv = U"<p>Hello HTML!</p>";
	auto doc = html::make_document<html::u16simple_tree>(sv.begin(), sv.end());

	// CSS セレクタで<p>タグを検索する。
	auto p = css::query_selector(doc.begin(), u8"P");

	// <p> タグを削除する。
	doc.erase(p);

	// 文書を文字列化する。
	std::u16string s;
	html::to_string(doc.begin(), std::back_inserter(s));

	// <p> タグ以下が消えている。
	assert(s == u"<html><head></head><body></body></html>");
}



BOOST_AUTO_TEST_SUITE_END()
