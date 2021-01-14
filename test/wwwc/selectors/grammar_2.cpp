// test/wwwc/selectors/grammar_2.cpp

#include <boost/test/unit_test.hpp>

#include <wordring/wwwc/selectors/grammar.hpp>

#include <wordring/wwwc/css_syntax/parsing.hpp>
#include <wordring/wwwc/css_syntax/token.hpp>

#include <wordring/html/simple_html.hpp>
#include <wordring/tree/tree_iterator.hpp>

#include <iterator>
#include <memory>
#include <string>

namespace
{
	namespace css = wordring::wwwc::css;
	namespace html = wordring::html;

	using tree_iterator = wordring::tree_iterator<html::u8simple_tree::const_iterator>;

	std::vector<css::syntax_primitive> s_primitives;

	inline css::syntax_primitive_stream stream(std::u32string in)
	{
		std::optional<std::vector<css::syntax_primitive>> v = css::parse_grammar(std::move(in)
			, [](std::vector<css::syntax_primitive> const&)->bool { return true; });

		s_primitives = std::move(*v);
		return css::syntax_primitive_stream(s_primitives);
	}

	inline html::u8simple_tree parse(std::u8string html)
	{
		return html::make_document<html::u8simple_tree>(html.begin(), html.end());
	}
}

BOOST_AUTO_TEST_SUITE(css_selectors_grammar_1_test)

// ------------------------------------------------------------------------------------------------
//	<id-selector> = <hash-token>
// ------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(grammar_id_selector_match_1)
{
	using namespace wordring::wwwc::css;

	auto tree = parse(u8"<p id='test'>paragraph</p>");

	auto m = id_selector::consume(stream(U"#test"));

	std::u32string html;
	wordring::html::to_string(tree.begin(), std::back_inserter(html));

	tree_iterator it(tree.begin());

	while (it != tree_iterator())
	{
		if (m.match(*it)) break;
		++it;
	}

	BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END()
