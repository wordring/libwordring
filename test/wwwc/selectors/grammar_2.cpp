// test/wwwc/selectors/grammar_2.cpp

#include <boost/test/unit_test.hpp>

#include <wordring/wwwc/selectors/grammar.hpp>

#include <wordring/wwwc/css_syntax/parsing.hpp>
#include <wordring/wwwc/css_syntax/token.hpp>

#include <wordring/html/simple_html.hpp>
#include <wordring/html/simple_traits.hpp>
#include <wordring/tree/tree_iterator.hpp>

#include <iterator>
#include <memory>
#include <string>

namespace
{
	namespace css = wordring::wwwc::css;
	namespace html = wordring::html;

	using document_type_name = wordring::html::document_type_name;
	using document_mode_name = wordring::html::document_mode_name;

	using const_iterator = html::u8simple_tree::const_iterator;
	using tree_iterator = wordring::tree_iterator<html::u8simple_tree::const_iterator>;

	inline html::u8simple_tree parse(std::u8string html)
	{
		return html::make_document<html::u8simple_tree>(html.begin(), html.end());
	}
}

BOOST_AUTO_TEST_SUITE(css_selectors_grammar_1_test)

// ------------------------------------------------------------------------------------------------
// <id-selector> = <hash-token>
// ------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(grammar_id_selector_match_1)
{
	using namespace wordring::wwwc::css;

	auto tree = parse(u8"<p id='test'>paragraph</p>");
	tree_iterator it(tree.begin());

	parse_context pc;
	auto m = css::parse_grammar<id_selector>(U"#test", pc);
	while (it != tree_iterator())
	{
		match_context<const_iterator> ctx{ document_type_name::Html, document_mode_name::Quirks };
		if (m.match(it.base(), ctx)) break;
		++it;
	}

	auto tag = html::get_qualified_name(*it);
	auto text = html::data(*it.base().begin());
	BOOST_CHECK(tag == u8"p");
	BOOST_CHECK(text == u8"paragraph");

	std::u16string s;
	html::to_string(it.base(), std::back_inserter(s)); // to_string() は子を文字列化するので、<p>タグ自身が現れない。。
	BOOST_CHECK(s == u"paragraph");
}

BOOST_AUTO_TEST_CASE(grammar_id_selector_match_2)
{
	using namespace wordring::wwwc::css;

	auto tree = parse(u8"<p ID='TEST'>paragraph</p>");
	tree_iterator it(tree.begin());

	parse_context pc;
	auto m = css::parse_grammar<id_selector>(U"#test", pc);
	while (it != tree_iterator())
	{
		match_context<const_iterator> ctx{ document_type_name::Html, document_mode_name::Quirks };
		if (m.match(it.base(), ctx)) break;
		++it;
	}

	BOOST_CHECK(it != tree_iterator());
}

BOOST_AUTO_TEST_CASE(grammar_id_selector_match_3)
{
	using namespace wordring::wwwc::css;

	auto tree = parse(u8"<p ID='test'>paragraph</p>");
	tree_iterator it(tree.begin());

	parse_context pc;
	auto m = css::parse_grammar<id_selector>(U"#test", pc);
	while (it != tree_iterator())
	{
		match_context<const_iterator> ctx{ document_type_name::Html, document_mode_name::Quirks };
		if (m.match(it.base(), ctx)) break;
		++it;
	}

	BOOST_CHECK(it != tree_iterator());
}

// limited_quirks は属性値の大小を区別するのでマッチしない。
BOOST_AUTO_TEST_CASE(grammar_id_selector_match_4)
{
	using namespace wordring::wwwc::css;

	auto tree = parse(u8"<p ID='TEST'>paragraph</p>");
	tree_iterator it(tree.begin());

	parse_context pc;
	auto m = css::parse_grammar<id_selector>(U"#test", pc);
	while (it != tree_iterator())
	{
		match_context<const_iterator> ctx{ document_type_name::Html, document_mode_name::LimitedQuirks };
		if (m.match(it.base(), ctx)) break;
		++it;
	}

	BOOST_CHECK(it == tree_iterator());
}

// 属性値が無いのでマッチしない。
BOOST_AUTO_TEST_CASE(grammar_id_selector_match_5)
{
	using namespace wordring::wwwc::css;

	auto tree = parse(u8"<p ID>paragraph</p>");
	tree_iterator it(tree.begin());

	parse_context pc;
	auto m = css::parse_grammar<id_selector>(U"#test", pc);
	while (it != tree_iterator())
	{
		match_context<const_iterator> ctx{ document_type_name::Html, document_mode_name::Quirks };
		if (m.match(it.base(), ctx)) break;
		++it;
	}

	BOOST_CHECK(it == tree_iterator());
}

// 属性値が無いのでマッチしない。
BOOST_AUTO_TEST_CASE(grammar_id_selector_match_6)
{
	using namespace wordring::wwwc::css;

	auto tree = parse(u8"<p ID=''>paragraph</p>");
	tree_iterator it(tree.begin());

	parse_context pc;
	auto m = css::parse_grammar<id_selector>(U"#test", pc);
	while (it != tree_iterator())
	{
		match_context<const_iterator> ctx{ document_type_name::Html, document_mode_name::Quirks };
		if (m.match(it.base(), ctx)) break;
		++it;
	}

	BOOST_CHECK(it == tree_iterator());
}

BOOST_AUTO_TEST_CASE(grammar_id_selector_match_7)
{
	using namespace wordring::wwwc::css;

	auto tree = parse(u8"<p id='test'>paragraph</p>");
	tree_iterator it(tree.begin());

	parse_context pc;
	auto m = css::parse_grammar<id_selector>(U"#test", pc);
	while (it != tree_iterator())
	{
		match_context<const_iterator> ctx{ document_type_name::Xml, document_mode_name::NoQuirks };
		if (m.match(it.base(), ctx)) break;
		++it;
	}

	BOOST_CHECK(it != tree_iterator());
}

// XML は属性名を大小区別するのでマッチしない。
// ・・・が、HTMLパーサーが属性名を小文字化していたため、HTML文書でテストすることは出来なかった。
BOOST_AUTO_TEST_CASE(grammar_id_selector_match_8)
{
	using namespace wordring::wwwc::css;

	auto tree = parse(u8"<p ID='test'>paragraph</p>");
	tree_iterator it(tree.begin());

	parse_context pc;
	auto m = css::parse_grammar<id_selector>(U"#test", pc);
	while (it != tree_iterator())
	{
		match_context<const_iterator> ctx{ document_type_name::Xml, document_mode_name::NoQuirks };
		if (m.match(it.base(), ctx)) break;
		++it;
	}

	// BOOST_CHECK(it == tree_iterator());
	BOOST_WARN(true);
	BOOST_TEST_MESSAGE("TODO: XML Dcument was never implemented.");
}

// XML は属性値を大小区別するのでマッチしない。
BOOST_AUTO_TEST_CASE(grammar_id_selector_match_9)
{
	using namespace wordring::wwwc::css;

	auto tree = parse(u8"<p id='TEST'>paragraph</p>");
	tree_iterator it(tree.begin());

	parse_context pc;
	auto m = css::parse_grammar<id_selector>(U"#test", pc);
	while (it != tree_iterator())
	{
		match_context<const_iterator> ctx{ document_type_name::Xml, document_mode_name::NoQuirks };
		if (m.match(it.base(), ctx)) break;
		++it;
	}

	BOOST_CHECK(it == tree_iterator());
}

// ------------------------------------------------------------------------------------------------
//	<class-selector> = '.' <ident-token>
// ------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(grammar_class_selector_match_1)
{
	using namespace wordring::wwwc::css;

	auto tree = parse(u8"<p class='test1'>paragraph</p>");
	tree_iterator it(tree.begin());

	parse_context pc;
	auto m = css::parse_grammar<class_selector>(U".test1", pc);
	while (it != tree_iterator())
	{
		match_context<const_iterator> ctx{ document_type_name::Html, document_mode_name::NoQuirks };
		if (m.match(it.base(), ctx)) break;
		++it;
	}

	auto tag = html::get_qualified_name(*it);
	auto text = html::data(*it.base().begin());
	BOOST_CHECK(tag == u8"p");
	BOOST_CHECK(text == u8"paragraph");
}

BOOST_AUTO_TEST_CASE(grammar_class_selector_match_2)
{
	using namespace wordring::wwwc::css;

	auto tree = parse(u8"<p CLASS='test1 test2 TEST3'>paragraph</p>");
	tree_iterator it(tree.begin());

	parse_context pc;
	auto m = css::parse_grammar<class_selector>(U".test3", pc);
	while (it != tree_iterator())
	{
		match_context<const_iterator> ctx{ document_type_name::Html, document_mode_name::Quirks };
		if (m.match(it.base(), ctx)) break;
		++it;
	}

	BOOST_CHECK(it != tree_iterator());
}

// limited_quirks は属性値の大小を区別するのでマッチしない。
BOOST_AUTO_TEST_CASE(grammar_class_selector_match_3)
{
	using namespace wordring::wwwc::css;

	auto tree = parse(u8"<p CLASS='test1 test2 TEST3'>paragraph</p>");
	tree_iterator it(tree.begin());

	parse_context pc;
	auto m = css::parse_grammar<class_selector>(U".test3", pc);
	while (it != tree_iterator())
	{
		match_context<const_iterator> ctx{ document_type_name::Html, document_mode_name::LimitedQuirks };
		if (m.match(it.base(), ctx)) break;
		++it;
	}

	BOOST_CHECK(it == tree_iterator());
}

// 属性値が無いのでマッチしない。
BOOST_AUTO_TEST_CASE(grammar_class_selector_match_4)
{
	using namespace wordring::wwwc::css;

	auto tree = parse(u8"<p CLASS>paragraph</p>");
	tree_iterator it(tree.begin());

	parse_context pc;
	auto m = css::parse_grammar<class_selector>(U".test", pc);
	while (it != tree_iterator())
	{
		match_context<const_iterator> ctx{ document_type_name::Html, document_mode_name::Quirks };
		if (m.match(it.base(), ctx)) break;
		++it;
	}

	BOOST_CHECK(it == tree_iterator());
}

// 属性値が無いのでマッチしない。
BOOST_AUTO_TEST_CASE(grammar_class_selector_match_5)
{
	using namespace wordring::wwwc::css;

	auto tree = parse(u8"<p CLASS=''>paragraph</p>");
	tree_iterator it(tree.begin());

	parse_context pc;
	auto m = css::parse_grammar<class_selector>(U".test", pc);
	while (it != tree_iterator())
	{
		match_context<const_iterator> ctx{ document_type_name::Html, document_mode_name::Quirks };
		if (m.match(it.base(), ctx)) break;
		++it;
	}

	BOOST_CHECK(it == tree_iterator());
}

// ------------------------------------------------------------------------------------------------
// <pseudo-class-selector>	= ':' <ident-token> | ':' <function-token> <any-value> ')'
// ------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(grammar_pseudo_class_selector_match_1)
{
	using namespace wordring::wwwc::css;

	auto tree = parse(u8"<p>paragraph</p>");
	tree_iterator it(tree.begin());

	parse_context pc;
	auto m = css::parse_grammar<pseudo_class_selector>(U":root", pc);
	while (it != tree_iterator())
	{
		match_context<const_iterator> ctx{ document_type_name::Html, document_mode_name::Quirks };
		if (m.match(it.base(), ctx)) break;
		++it;
	}

	BOOST_CHECK(it != tree_iterator());

	auto tag = html::get_qualified_name(*it);
	BOOST_CHECK(tag == u8"html");
}

// ------------------------------------------------------------------------------------------------
// <pseudo-element-selector> = ':' <pseudo-class-selector>
// ------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------
// <attribute-selector> =
//		'[' <wq-name> ']' |
//		'[' <wq-name> <attr-matcher> [ <string-token> | <ident-token> ] <attr-modifier>? ']'
// ------------------------------------------------------------------------------------------------

// [attr] HTML
BOOST_AUTO_TEST_CASE(grammar_attribute_selector_match_1)
{
	using namespace wordring::wwwc::css;

	auto tree = parse(u8"<p attr>paragraph</p>");
	tree_iterator it(tree.begin());

	parse_context pc;
	auto m = css::parse_grammar<attribute_selector>(U"[ attr ]", pc);
	while (it != tree_iterator())
	{
		match_context<const_iterator> ctx{ document_type_name::Html, document_mode_name::Quirks };
		if (m.match(it.base(), ctx)) break;
		++it;
	}

	BOOST_CHECK(it != tree_iterator());

	auto tag = html::get_qualified_name(*it);
	BOOST_CHECK(tag == u8"p");
}

BOOST_AUTO_TEST_CASE(grammar_attribute_selector_match_2)
{
	using namespace wordring::wwwc::css;

	auto tree = parse(u8"<p ATTR>paragraph</p>");
	tree_iterator it(tree.begin());

	parse_context pc;
	auto m = css::parse_grammar<attribute_selector>(U"[attr]", pc);
	while (it != tree_iterator())
	{
		match_context<const_iterator> ctx{ document_type_name::Html, document_mode_name::Quirks };
		if (m.match(it.base(), ctx)) break;
		++it;
	}
	BOOST_CHECK(it != tree_iterator());
}

BOOST_AUTO_TEST_CASE(grammar_attribute_selector_match_3)
{
	using namespace wordring::wwwc::css;

	auto tree = parse(u8"<p attr>paragraph</p>");
	tree_iterator it(tree.begin());

	parse_context pc;
	auto m = css::parse_grammar<attribute_selector>(U"[ATTR]", pc);
	while (it != tree_iterator())
	{
		match_context<const_iterator> ctx{ document_type_name::Html, document_mode_name::Quirks };
		if (m.match(it.base(), ctx)) break;
		++it;
	}
	BOOST_CHECK(it != tree_iterator());
}

// [attr] XML
BOOST_AUTO_TEST_CASE(grammar_attribute_selector_match_4)
{
	using namespace wordring::wwwc::css;

	auto tree = parse(u8"<p attr>paragraph</p>");
	tree_iterator it(tree.begin());

	parse_context pc;
	auto m = css::parse_grammar<attribute_selector>(U"[ATTR]", pc);
	while (it != tree_iterator())
	{
		match_context<const_iterator> ctx{ document_type_name::Xml, document_mode_name::NoQuirks };
		if (m.match(it.base(), ctx)) break;
		++it;
	}
	BOOST_CHECK(it == tree_iterator());
}

// [attr=val] HTML
BOOST_AUTO_TEST_CASE(grammar_attribute_selector_match_5)
{
	using namespace wordring::wwwc::css;

	auto tree = parse(u8"<p attr='val'>paragraph</p>");
	tree_iterator it(tree.begin());

	parse_context pc;
	auto m = css::parse_grammar<attribute_selector>(U"[attr='val']", pc);
	while (it != tree_iterator())
	{
		match_context<const_iterator> ctx{ document_type_name::Html, document_mode_name::Quirks };
		if (m.match(it.base(), ctx)) break;
		++it;
	}
	BOOST_CHECK(it != tree_iterator());
}

BOOST_AUTO_TEST_CASE(grammar_attribute_selector_match_6)
{
	using namespace wordring::wwwc::css;

	auto tree = parse(u8"<p attr='  val  '>paragraph</p>");
	tree_iterator it(tree.begin());

	parse_context pc;
	auto m = css::parse_grammar<attribute_selector>(U"[attr='  val  ']", pc);
	while (it != tree_iterator())
	{
		match_context<const_iterator> ctx{ document_type_name::Html, document_mode_name::Quirks };
		if (m.match(it.base(), ctx)) break;
		++it;
	}
	BOOST_CHECK(it != tree_iterator());
}

BOOST_AUTO_TEST_CASE(grammar_attribute_selector_match_7)
{
	using namespace wordring::wwwc::css;

	auto tree = parse(u8"<p attr='VAL'>paragraph</p>");
	tree_iterator it(tree.begin());

	parse_context pc;
	auto m = css::parse_grammar<attribute_selector>(U"[attr='val'i]", pc);
	while (it != tree_iterator())
	{
		match_context<const_iterator> ctx{ document_type_name::Html, document_mode_name::Quirks };
		if (m.match(it.base(), ctx)) break;
		++it;
	}
	BOOST_CHECK(it != tree_iterator());
}

BOOST_AUTO_TEST_CASE(grammar_attribute_selector_match_8)
{
	using namespace wordring::wwwc::css;

	auto tree = parse(u8"<p attr='VAL'>paragraph</p>");
	tree_iterator it(tree.begin());

	parse_context pc;
	auto m = css::parse_grammar<attribute_selector>(U"[attr='val's]", pc);
	while (it != tree_iterator())
	{
		match_context<const_iterator> ctx{ document_type_name::Html, document_mode_name::Quirks };
		if (m.match(it.base(), ctx)) break;
		++it;
	}
	BOOST_CHECK(it == tree_iterator());
}

// [attr~=val]
BOOST_AUTO_TEST_CASE(grammar_attribute_selector_match_9)
{
	using namespace wordring::wwwc::css;

	auto tree = parse(u8"<p attr='val'>paragraph</p>");
	tree_iterator it(tree.begin());

	parse_context pc;
	auto m = css::parse_grammar<attribute_selector>(U"[attr~='val']", pc);
	while (it != tree_iterator())
	{
		match_context<const_iterator> ctx{ document_type_name::Html, document_mode_name::Quirks };
		if (m.match(it.base(), ctx)) break;
		++it;
	}
	BOOST_CHECK(it != tree_iterator());
}

BOOST_AUTO_TEST_CASE(grammar_attribute_selector_match_10)
{
	using namespace wordring::wwwc::css;

	auto tree = parse(u8"<p attr='val1 val2 val3'>paragraph</p>");
	tree_iterator it(tree.begin());

	parse_context pc;
	auto m = css::parse_grammar<attribute_selector>(U"[attr~='val3']", pc);
	while (it != tree_iterator())
	{
		match_context<const_iterator> ctx{ document_type_name::Html, document_mode_name::Quirks };
		if (m.match(it.base(), ctx)) break;
		++it;
	}
	BOOST_CHECK(it != tree_iterator());
}

BOOST_AUTO_TEST_CASE(grammar_attribute_selector_match_11)
{
	using namespace wordring::wwwc::css;

	auto tree = parse(u8"<p attr='val1 val2 val3'>paragraph</p>");
	tree_iterator it(tree.begin());

	parse_context pc;
	auto m = css::parse_grammar<attribute_selector>(U"[attr~='VAL3' i]", pc);
	while (it != tree_iterator())
	{
		match_context<const_iterator> ctx{ document_type_name::Html, document_mode_name::Quirks };
		if (m.match(it.base(), ctx)) break;
		++it;
	}
	BOOST_CHECK(it != tree_iterator());
}

BOOST_AUTO_TEST_CASE(grammar_attribute_selector_match_12)
{
	using namespace wordring::wwwc::css;

	auto tree = parse(u8"<p attr='val1 val2 val3'>paragraph</p>");
	tree_iterator it(tree.begin());

	parse_context pc;
	auto m = css::parse_grammar<attribute_selector>(U"[attr~='VAL3']", pc);
	while (it != tree_iterator())
	{
		match_context<const_iterator> ctx{ document_type_name::Html, document_mode_name::Quirks };
		if (m.match(it.base(), ctx)) break;
		++it;
	}
	BOOST_CHECK(it == tree_iterator());
}

// [attr|=val]
BOOST_AUTO_TEST_CASE(grammar_attribute_selector_match_13)
{
	using namespace wordring::wwwc::css;

	auto tree = parse(u8"<p attr='val-'>paragraph</p>");
	tree_iterator it(tree.begin());

	parse_context pc;
	auto m = css::parse_grammar<attribute_selector>(U"[attr|='val']", pc);
	while (it != tree_iterator())
	{
		match_context<const_iterator> ctx{ document_type_name::Html, document_mode_name::Quirks };
		if (m.match(it.base(), ctx)) break;
		++it;
	}
	BOOST_CHECK(it != tree_iterator());
}

BOOST_AUTO_TEST_CASE(grammar_attribute_selector_match_14)
{
	using namespace wordring::wwwc::css;

	auto tree = parse(u8"<p attr='va-'>paragraph</p>");
	tree_iterator it(tree.begin());

	parse_context pc;
	auto m = css::parse_grammar<attribute_selector>(U"[attr|='val']", pc);
	while (it != tree_iterator())
	{
		match_context<const_iterator> ctx{ document_type_name::Html, document_mode_name::Quirks };
		if (m.match(it.base(), ctx)) break;
		++it;
	}
	BOOST_CHECK(it == tree_iterator());
}

BOOST_AUTO_TEST_CASE(grammar_attribute_selector_match_15)
{
	using namespace wordring::wwwc::css;

	auto tree = parse(u8"<p attr='val'>paragraph</p>");
	tree_iterator it(tree.begin());

	parse_context pc;
	auto m = css::parse_grammar<attribute_selector>(U"[attr|='val']", pc);
	while (it != tree_iterator())
	{
		match_context<const_iterator> ctx{ document_type_name::Html, document_mode_name::Quirks };
		if (m.match(it.base(), ctx)) break;
		++it;
	}
	BOOST_CHECK(it == tree_iterator());
}

BOOST_AUTO_TEST_CASE(grammar_attribute_selector_match_16)
{
	using namespace wordring::wwwc::css;

	auto tree = parse(u8"<p attr='val'>paragraph</p>");
	tree_iterator it(tree.begin());

	parse_context pc;
	auto m = css::parse_grammar<attribute_selector>(U"[attr|='VAL'i]", pc);
	while (it != tree_iterator())
	{
		match_context<const_iterator> ctx{ document_type_name::Html, document_mode_name::Quirks };
		if (m.match(it.base(), ctx)) break;
		++it;
	}
	BOOST_CHECK(it == tree_iterator());
}

BOOST_AUTO_TEST_CASE(grammar_attribute_selector_match_17)
{
	using namespace wordring::wwwc::css;

	auto tree = parse(u8"<p attr='va-'>paragraph</p>");
	tree_iterator it(tree.begin());

	parse_context pc;
	auto m = css::parse_grammar<attribute_selector>(U"[attr|='VAL'i]", pc);
	while (it != tree_iterator())
	{
		match_context<const_iterator> ctx{ document_type_name::Html, document_mode_name::Quirks };
		if (m.match(it.base(), ctx)) break;
		++it;
	}
	BOOST_CHECK(it == tree_iterator());
}

// [attr^=val]
BOOST_AUTO_TEST_CASE(grammar_attribute_selector_match_18)
{
	using namespace wordring::wwwc::css;

	auto tree = parse(u8"<p attr='valval'>paragraph</p>");
	tree_iterator it(tree.begin());

	parse_context pc;
	auto m = css::parse_grammar<attribute_selector>(U"[attr^='val']", pc);
	while (it != tree_iterator())
	{
		match_context<const_iterator> ctx{ document_type_name::Html, document_mode_name::Quirks };
		if (m.match(it.base(), ctx)) break;
		++it;
	}
	BOOST_CHECK(it != tree_iterator());
}

BOOST_AUTO_TEST_CASE(grammar_attribute_selector_match_19)
{
	using namespace wordring::wwwc::css;

	auto tree = parse(u8"<p attr='valval'>paragraph</p>");
	tree_iterator it(tree.begin());

	parse_context pc;
	auto m = css::parse_grammar<attribute_selector>(U"[attr^='VAL'i]", pc);
	while (it != tree_iterator())
	{
		match_context<const_iterator> ctx{ document_type_name::Html, document_mode_name::Quirks };
		if (m.match(it.base(), ctx)) break;
		++it;
	}
	BOOST_CHECK(it != tree_iterator());
}

BOOST_AUTO_TEST_CASE(grammar_attribute_selector_match_20)
{
	using namespace wordring::wwwc::css;

	auto tree = parse(u8"<p attr='valval'>paragraph</p>");
	tree_iterator it(tree.begin());

	parse_context pc;
	auto m = css::parse_grammar<attribute_selector>(U"[attr^='VAL']", pc);
	while (it != tree_iterator())
	{
		match_context<const_iterator> ctx{ document_type_name::Html, document_mode_name::Quirks };
		if (m.match(it.base(), ctx)) break;
		++it;
	}
	BOOST_CHECK(it == tree_iterator());
}

// [attr$=val]
BOOST_AUTO_TEST_CASE(grammar_attribute_selector_match_21)
{
	using namespace wordring::wwwc::css;

	auto tree = parse(u8"<p attr='foobar'>paragraph</p>");
	tree_iterator it(tree.begin());

	parse_context pc;
	auto m = css::parse_grammar<attribute_selector>(U"[attr$='bar']", pc);
	while (it != tree_iterator())
	{
		match_context<const_iterator> ctx{ document_type_name::Html, document_mode_name::Quirks };
		if (m.match(it.base(), ctx)) break;
		++it;
	}
	BOOST_CHECK(it != tree_iterator());
}

BOOST_AUTO_TEST_CASE(grammar_attribute_selector_match_22)
{
	using namespace wordring::wwwc::css;

	auto tree = parse(u8"<p attr='foobar'>paragraph</p>");
	tree_iterator it(tree.begin());

	parse_context pc;
	auto m = css::parse_grammar<attribute_selector>(U"[attr$='BAR'i]", pc);
	while (it != tree_iterator())
	{
		match_context<const_iterator> ctx{ document_type_name::Html, document_mode_name::Quirks };
		if (m.match(it.base(), ctx)) break;
		++it;
	}
	BOOST_CHECK(it != tree_iterator());
}

BOOST_AUTO_TEST_CASE(grammar_attribute_selector_match_23)
{
	using namespace wordring::wwwc::css;

	auto tree = parse(u8"<p attr='foobar'>paragraph</p>");
	tree_iterator it(tree.begin());

	parse_context pc;
	auto m = css::parse_grammar<attribute_selector>(U"[attr$='BAR']", pc);
	while (it != tree_iterator())
	{
		match_context<const_iterator> ctx{ document_type_name::Html, document_mode_name::Quirks };
		if (m.match(it.base(), ctx)) break;
		++it;
	}
	BOOST_CHECK(it == tree_iterator());
}

// [attr*=val]
BOOST_AUTO_TEST_CASE(grammar_attribute_selector_match_24)
{
	using namespace wordring::wwwc::css;

	auto tree = parse(u8"<p attr='foobar'>paragraph</p>");
	tree_iterator it(tree.begin());

	parse_context pc;
	auto m = css::parse_grammar<attribute_selector>(U"[attr*='foo']", pc);
	while (it != tree_iterator())
	{
		match_context<const_iterator> ctx{ document_type_name::Html, document_mode_name::Quirks };
		if (m.match(it.base(), ctx)) break;
		++it;
	}
	BOOST_CHECK(it != tree_iterator());
}

BOOST_AUTO_TEST_CASE(grammar_attribute_selector_match_25)
{
	using namespace wordring::wwwc::css;

	auto tree = parse(u8"<p attr='foobar'>paragraph</p>");
	tree_iterator it(tree.begin());

	parse_context pc;
	auto m = css::parse_grammar<attribute_selector>(U"[attr*='oob']", pc);
	while (it != tree_iterator())
	{
		match_context<const_iterator> ctx{ document_type_name::Html, document_mode_name::Quirks };
		if (m.match(it.base(), ctx)) break;
		++it;
	}
	BOOST_CHECK(it != tree_iterator());
}

BOOST_AUTO_TEST_CASE(grammar_attribute_selector_match_26)
{
	using namespace wordring::wwwc::css;

	auto tree = parse(u8"<p attr='foobar'>paragraph</p>");
	tree_iterator it(tree.begin());

	parse_context pc;
	auto m = css::parse_grammar<attribute_selector>(U"[attr*='bar']", pc);
	while (it != tree_iterator())
	{
		match_context<const_iterator> ctx{ document_type_name::Html, document_mode_name::Quirks };
		if (m.match(it.base(), ctx)) break;
		++it;
	}
	BOOST_CHECK(it != tree_iterator());
}

BOOST_AUTO_TEST_CASE(grammar_attribute_selector_match_27)
{
	using namespace wordring::wwwc::css;

	auto tree = parse(u8"<p attr='foobar'>paragraph</p>");
	tree_iterator it(tree.begin());

	parse_context pc;
	auto m = css::parse_grammar<attribute_selector>(U"[attr*='OOB'i]", pc);
	while (it != tree_iterator())
	{
		match_context<const_iterator> ctx{ document_type_name::Html, document_mode_name::Quirks };
		if (m.match(it.base(), ctx)) break;
		++it;
	}
	BOOST_CHECK(it != tree_iterator());
}

BOOST_AUTO_TEST_CASE(grammar_attribute_selector_match_28)
{
	using namespace wordring::wwwc::css;

	auto tree = parse(u8"<p attr='foobar'>paragraph</p>");
	tree_iterator it(tree.begin());

	parse_context pc;
	auto m = css::parse_grammar<attribute_selector>(U"[attr*='FOO']", pc);
	while (it != tree_iterator())
	{
		match_context<const_iterator> ctx{ document_type_name::Html, document_mode_name::Quirks };
		if (m.match(it.base(), ctx)) break;
		++it;
	}
	BOOST_CHECK(it == tree_iterator());
}

// ------------------------------------------------------------------------------------------------
// <subclass-selector> = <id-selector> | <class-selector> | <attribute-selector> | <pseudo-class-selector>
// ------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(grammar_subclass_selector_match_1)
{
	using namespace wordring::wwwc::css;

	auto tree = parse(u8"<p attr='foo'>paragraph</p>");
	tree_iterator it(tree.begin());

	parse_context pc;
	auto m = css::parse_grammar<subclass_selector>(U"[attr='foo']", pc);
	while (it != tree_iterator())
	{
		match_context<const_iterator> ctx{ document_type_name::Html, document_mode_name::Quirks };
		if (m.match(it.base(), ctx)) break;
		++it;
	}
	BOOST_CHECK(it != tree_iterator());
}

BOOST_AUTO_TEST_CASE(grammar_subclass_selector_match_2)
{
	using namespace wordring::wwwc::css;

	auto tree = parse(u8"<p attr='foo'>paragraph</p>");
	tree_iterator it(tree.begin());

	parse_context pc;
	auto m = css::parse_grammar<subclass_selector>(U"[attr='FOO']", pc);
	while (it != tree_iterator())
	{
		match_context<const_iterator> ctx{ document_type_name::Html, document_mode_name::Quirks };
		if (m.match(it.base(), ctx)) break;
		++it;
	}
	BOOST_CHECK(it == tree_iterator());
}

// ------------------------------------------------------------------------------------------------
// <type-selector> = <wq-name> | <ns-prefix>? '*'
// ------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(grammar_type_selector_match_1)
{
	using namespace wordring::wwwc::css;

	auto tree = parse(u8"<p>paragraph</p>");
	tree_iterator it(tree.begin());

	parse_context pc;
	auto m = css::parse_grammar<type_selector>(U"p", pc);
	while (it != tree_iterator())
	{
		match_context<const_iterator> ctx{ document_type_name::Html, document_mode_name::Quirks };
		if (m.match(it.base(), ctx)) break;
		++it;
	}
	BOOST_CHECK(it != tree_iterator());
	std::u16string s;
	html::to_string(it.base(), std::back_inserter(s)); // to_string() は子を文字列化するので、<p>タグ自身が現れない。。
	BOOST_CHECK(s == u"paragraph");
}

BOOST_AUTO_TEST_CASE(grammar_type_selector_match_2)
{
	using namespace wordring::wwwc::css;

	auto tree = parse(u8"<p>paragraph</p>");
	tree_iterator it(tree.begin());

	parse_context pc;
	auto m = css::parse_grammar<type_selector>(U"p", pc);
	while (it != tree_iterator())
	{
		match_context<const_iterator> ctx{ document_type_name::Html, document_mode_name::Quirks };
		if (m.match(it.base(), ctx)) break;
		++it;
	}
	BOOST_CHECK(it != tree_iterator());
	std::u16string s;
	html::to_string(it.base(), std::back_inserter(s)); // to_string() は子を文字列化するので、<p>タグ自身が現れない。。
	BOOST_CHECK(s == u"paragraph");
}

// ------------------------------------------------------------------------------------------------
// <simple-selector> = <type-selector> | <subclass-selector>
// ------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(grammar_simple_selector_match_1)
{
	using namespace wordring::wwwc::css;

	auto tree = parse(u8"<p>paragraph</p>");
	tree_iterator it(tree.begin());

	parse_context pc;
	auto m = css::parse_grammar<simple_selector>(U"p", pc);
	while (it != tree_iterator())
	{
		match_context<const_iterator> ctx{ document_type_name::Html, document_mode_name::Quirks };
		if (m.match(it.base(), ctx)) break;
		++it;
	}
	BOOST_CHECK(it != tree_iterator());
	std::u16string s;
	html::to_string(it.base(), std::back_inserter(s)); // to_string() は子を文字列化するので、<p>タグ自身が現れない。。
	BOOST_CHECK(s == u"paragraph");
}

BOOST_AUTO_TEST_CASE(grammar_simple_selector_match_2)
{
	using namespace wordring::wwwc::css;

	auto tree = parse(u8"<p attr='foo'>paragraph</p>");
	tree_iterator it(tree.begin());

	parse_context pc;
	auto m = css::parse_grammar<simple_selector>(U"[attr='foo']", pc);
	while (it != tree_iterator())
	{
		match_context<const_iterator> ctx{ document_type_name::Html, document_mode_name::Quirks };
		if (m.match(it.base(), ctx)) break;
		++it;
	}
	BOOST_CHECK(it != tree_iterator());
}

BOOST_AUTO_TEST_CASE(grammar_simple_selector_match_3)
{
	using namespace wordring::wwwc::css;

	auto tree = parse(u8"<p attr='foo'>paragraph</p>");
	tree_iterator it(tree.begin());

	parse_context pc;
	auto m = css::parse_grammar<simple_selector>(U"h1", pc);
	while (it != tree_iterator())
	{
		match_context<const_iterator> ctx{ document_type_name::Html, document_mode_name::Quirks };
		if (m.match(it.base(), ctx)) break;
		++it;
	}
	BOOST_CHECK(it == tree_iterator());
}

// ------------------------------------------------------------------------------------------------
// <compound-selector> = [ <type-selector>? <subclass-selector>*
//							[ <pseudo-element-selector> <pseudo-class-selector>* ]* ]!
// ------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(grammar_compound_selector_match_1)
{
	using namespace wordring::wwwc::css;

	auto tree = parse(u8"<p id='id1' class='cls1 cls2'>paragraph</p>");
	tree_iterator it(tree.begin());

	parse_context pc;
	auto m = css::parse_grammar<compound_selector>(U"p#id1.cls2", pc);
	while (it != tree_iterator())
	{
		match_context<const_iterator> ctx{ document_type_name::Html, document_mode_name::Quirks };
		if (m.match(it.base(), ctx)) break;
		++it;
	}
	BOOST_CHECK(it != tree_iterator());
	std::u16string s;
	html::to_string(it.base(), std::back_inserter(s)); // to_string() は子を文字列化するので、<p>タグ自身が現れない。。
	BOOST_CHECK(s == u"paragraph");
}

BOOST_AUTO_TEST_CASE(grammar_compound_selector_match_2)
{
	using namespace wordring::wwwc::css;

	auto tree = parse(u8"<p id='id1' class='cls1 cls2'>paragraph</p>");
	tree_iterator it(tree.begin());

	parse_context pc;
	auto m = css::parse_grammar<compound_selector>(U"p#id1.cls3", pc);
	while (it != tree_iterator())
	{
		match_context<const_iterator> ctx{ document_type_name::Html, document_mode_name::Quirks };
		if (m.match(it.base(), ctx)) break;
		++it;
	}
	BOOST_CHECK(it == tree_iterator());
}

// ------------------------------------------------------------------------------------------------
// <complex-selector> = <compound-selector> [ <combinator>? <compound-selector> ]*
// ------------------------------------------------------------------------------------------------

// 子孫「 」
BOOST_AUTO_TEST_CASE(grammar_complex_selector_match_1)
{
	using namespace wordring::wwwc::css;

	auto tree = parse(u8"<div><p><span>span!</span></p></div>");
	tree_iterator it(tree.begin());

	parse_context pc;
	auto m = css::parse_grammar<complex_selector>(U"div span", pc);
	while (it != tree_iterator())
	{
		match_context<const_iterator> ctx{ document_type_name::Html, document_mode_name::Quirks };
		if (m.match(it.base(), ctx)) break;
		++it;
	}
	BOOST_CHECK(it != tree_iterator());
	std::u16string s;
	html::to_string(it.base(), std::back_inserter(s)); // to_string() は子を文字列化するので、<span>タグ自身が現れない。。
	BOOST_CHECK(s == u"span!");
}

BOOST_AUTO_TEST_CASE(grammar_complex_selector_match_2)
{
	using namespace wordring::wwwc::css;

	auto tree = parse(u8"<div><p><span>span!</span></p></div>");
	tree_iterator it(tree.begin());

	parse_context pc;
	auto m = css::parse_grammar<complex_selector>(U"p span", pc);
	while (it != tree_iterator())
	{
		match_context<const_iterator> ctx{ document_type_name::Html, document_mode_name::Quirks };
		if (m.match(it.base(), ctx)) break;
		++it;
	}
	BOOST_CHECK(it != tree_iterator());
}

BOOST_AUTO_TEST_CASE(grammar_complex_selector_match_3)
{
	using namespace wordring::wwwc::css;

	auto tree = parse(u8"<div><p><span>span!</span></p></div>");
	tree_iterator it(tree.begin());

	parse_context pc;
	auto m = css::parse_grammar<complex_selector>(U"div p", pc);
	while (it != tree_iterator())
	{
		match_context<const_iterator> ctx{ document_type_name::Html, document_mode_name::Quirks };
		if (m.match(it.base(), ctx)) break;
		++it;
	}
	BOOST_CHECK(it != tree_iterator());
}

BOOST_AUTO_TEST_CASE(grammar_complex_selector_match_4)
{
	using namespace wordring::wwwc::css;

	auto tree = parse(u8"<div><p><span>span!</span></p></div>");
	tree_iterator it(tree.begin());

	parse_context pc;
	auto m = css::parse_grammar<complex_selector>(U"span p", pc);
	while (it != tree_iterator())
	{
		match_context<const_iterator> ctx{ document_type_name::Html, document_mode_name::Quirks };
		if (m.match(it.base(), ctx)) break;
		++it;
	}
	BOOST_CHECK(it == tree_iterator());
}

// 子「>」
BOOST_AUTO_TEST_CASE(grammar_complex_selector_match_6)
{
	using namespace wordring::wwwc::css;

	auto tree = parse(u8"<div><p><span>span!</span></p></div>");
	tree_iterator it(tree.begin());

	parse_context pc;
	auto m = css::parse_grammar<complex_selector>(U"div>p", pc);
	while (it != tree_iterator())
	{
		match_context<const_iterator> ctx{ document_type_name::Html, document_mode_name::Quirks };
		if (m.match(it.base(), ctx)) break;
		++it;
	}
	BOOST_CHECK(it != tree_iterator());
}

BOOST_AUTO_TEST_CASE(grammar_complex_selector_match_7)
{
	using namespace wordring::wwwc::css;

	auto tree = parse(u8"<div><p><span>span!</span></p></div>");
	tree_iterator it(tree.begin());

	parse_context pc;
	auto m = css::parse_grammar<complex_selector>(U"p>div", pc);
	while (it != tree_iterator())
	{
		match_context<const_iterator> ctx{ document_type_name::Html, document_mode_name::Quirks };
		if (m.match(it.base(), ctx)) break;
		++it;
	}
	BOOST_CHECK(it == tree_iterator());
}

// 次「+」
BOOST_AUTO_TEST_CASE(grammar_complex_selector_match_8)
{
	using namespace wordring::wwwc::css;

	auto tree = parse(u8"<div><p><h1></h1><h2>h2!</h2><h3></h3></p></div>");
	tree_iterator it(tree.begin());

	parse_context pc;
	auto m = css::parse_grammar<complex_selector>(U"h1+h2", pc);
	while (it != tree_iterator())
	{
		match_context<const_iterator> ctx{ document_type_name::Html, document_mode_name::Quirks };
		if (m.match(it.base(), ctx)) break;
		++it;
	}
	BOOST_CHECK(it != tree_iterator());
	std::u16string s;
	html::to_string(it.base(), std::back_inserter(s)); // to_string() は子を文字列化するので、<span>タグ自身が現れない。。
	BOOST_CHECK(s == u"h2!");
}

BOOST_AUTO_TEST_CASE(grammar_complex_selector_match_9)
{
	using namespace wordring::wwwc::css;

	auto tree = parse(u8"<div><p><h1></h1>text1<!-- Comment --><h2></h2><h3></h3></p></div>");
	tree_iterator it(tree.begin());

	parse_context pc;
	auto m = css::parse_grammar<complex_selector>(U"h1+h2", pc);
	while (it != tree_iterator())
	{
		match_context<const_iterator> ctx{ document_type_name::Html, document_mode_name::Quirks };
		if (m.match(it.base(), ctx)) break;
		++it;
	}
	BOOST_CHECK(it != tree_iterator());
}

BOOST_AUTO_TEST_CASE(grammar_complex_selector_match_10)
{
	using namespace wordring::wwwc::css;

	auto tree = parse(u8"<div><p><h1></h1><h2></h2><h3></h3></p></div>");
	tree_iterator it(tree.begin());

	parse_context pc;
	auto m = css::parse_grammar<complex_selector>(U"h1+h3", pc);
	while (it != tree_iterator())
	{
		match_context<const_iterator> ctx{ document_type_name::Html, document_mode_name::Quirks };
		if (m.match(it.base(), ctx)) break;
		++it;
	}
	BOOST_CHECK(it == tree_iterator());
}

// 後続「~」
BOOST_AUTO_TEST_CASE(grammar_complex_selector_match_11)
{
	using namespace wordring::wwwc::css;

	auto tree = parse(u8"<div><p><h1></h1><h2></h2><h3>h3!</h3></p></div>");
	tree_iterator it(tree.begin());

	parse_context pc;
	auto m = css::parse_grammar<complex_selector>(U"h1~h3", pc);
	while (it != tree_iterator())
	{
		match_context<const_iterator> ctx{ document_type_name::Html, document_mode_name::Quirks };
		if (m.match(it.base(), ctx)) break;
		++it;
	}
	BOOST_CHECK(it != tree_iterator());
	std::u16string s;
	html::to_string(it.base(), std::back_inserter(s)); // to_string() は子を文字列化するので、<span>タグ自身が現れない。。
	BOOST_CHECK(s == u"h3!");
}

BOOST_AUTO_TEST_CASE(grammar_complex_selector_match_12)
{
	using namespace wordring::wwwc::css;

	auto tree = parse(u8"<div><p><h1></h1>text1<!-- Comment --><h2></h2>text1<!-- Comment --><h3></h3></p></div>");
	tree_iterator it(tree.begin());

	parse_context pc;
	auto m = css::parse_grammar<complex_selector>(U"h1~h3", pc);
	while (it != tree_iterator())
	{
		match_context<const_iterator> ctx{ document_type_name::Html, document_mode_name::Quirks };
		if (m.match(it.base(), ctx)) break;
		++it;
	}
	BOOST_CHECK(it != tree_iterator());
}

BOOST_AUTO_TEST_CASE(grammar_complex_selector_match_13)
{
	using namespace wordring::wwwc::css;

	auto tree = parse(u8"<div><p><h1></h1><h2></h2><h3></h3></p></div>");
	tree_iterator it(tree.begin());

	parse_context pc;
	auto m = css::parse_grammar<complex_selector>(U"h2~h3", pc);
	while (it != tree_iterator())
	{
		match_context<const_iterator> ctx{ document_type_name::Html, document_mode_name::Quirks };
		if (m.match(it.base(), ctx)) break;
		++it;
	}
	BOOST_CHECK(it != tree_iterator());
}

BOOST_AUTO_TEST_CASE(grammar_complex_selector_match_14)
{
	using namespace wordring::wwwc::css;

	auto tree = parse(u8"<div><p><h1></h1><h2></h2><h3></h3></p></div>");
	tree_iterator it(tree.begin());

	parse_context pc;
	auto m = css::parse_grammar<complex_selector>(U"h3~h3", pc);
	while (it != tree_iterator())
	{
		match_context<const_iterator> ctx{ document_type_name::Html, document_mode_name::Quirks };
		if (m.match(it.base(), ctx)) break;
		++it;
	}
	BOOST_CHECK(it == tree_iterator());
}

// ------------------------------------------------------------------------------------------------
// <relative-selector> = <combinator>? <complex-selector>
// ------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(grammar_relative_selector_match_1)
{
	using namespace wordring::wwwc::css;

	auto tree = parse(u8"<div><p><h1></h1><h2></h2><h3>h3!</h3></p></div>");
	tree_iterator it1(tree.begin());

	parse_context pc;
	auto m1 = css::parse_grammar<type_selector>(U"div", pc);

	match_context<const_iterator> ctx1{ document_type_name::Html, document_mode_name::Quirks };
	while (it1 != tree_iterator())
	{
		if (m1.match(it1.base(), ctx1)) break;
		++it1;
	}
	auto div = it1.base();

	auto m2 = css::parse_grammar<relative_selector>(U"p > h1", pc);
	match_context<const_iterator> ctx2{ document_type_name::Html, document_mode_name::Quirks };
	m2.absolutize(ctx2);
}

BOOST_AUTO_TEST_SUITE_END()
