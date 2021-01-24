// test/wwwc/selectors/api.cpp

#include <boost/test/unit_test.hpp>

#include <wordring/wwwc/css_defs.hpp>

#include <wordring/wwwc/selectors/api.hpp>
#include <wordring/wwwc/selectors/grammar.hpp>

#include <wordring/html/simple_html.hpp>

#include <iterator>
#include <string>
#include <vector>

BOOST_AUTO_TEST_SUITE(css_selectors_api_test)

BOOST_AUTO_TEST_CASE(api_parse_selector_1)
{
	using namespace wordring::wwwc::css;
	using namespace wordring::html;

	parse_context pctx;
	auto cs = parse_grammar<complex_selector>(U"p", pctx);

	std::u8string src = u8"<p>text</p>";
	u8simple_tree tree = make_document<u8simple_tree>(src.begin(), src.end());

	std::vector<u8simple_tree::const_iterator> v;
	match_selector(cs, pctx, tree.begin(), tree.end(), std::back_inserter(v));

	BOOST_CHECK(v.size() == 1);
	std::u32string s;
	wordring::html::to_string(v.front(), std::back_inserter(s));
	BOOST_CHECK(s == U"text");
}

BOOST_AUTO_TEST_SUITE_END()
