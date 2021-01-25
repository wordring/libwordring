
#include <boost/test/unit_test.hpp>

#include <wordring/whatwg/dom/infrastructure.hpp>

#include <wordring/html/simple_html.hpp>

#include <iterator>
#include <string>
#include <vector>

BOOST_AUTO_TEST_SUITE(whatwg_dom_test)

// ------------------------------------------------------------------------------------------------
// inline void scope_match(NodePointer np, std::u32string selector, OutputIterator out)
// ------------------------------------------------------------------------------------------------

// 単体でマッチ
BOOST_AUTO_TEST_CASE(infrastructure_scope_match_1)
{
	using namespace wordring::html;
	using namespace wordring::whatwg::dom;

	std::u8string html = u8"<p>text</p>";
	u8simple_tree tree = make_document<u8simple_tree>(html.begin(), html.end());

	std::vector<u8simple_tree::const_iterator> v;

	// 文書ノードから<p>タグを検索
	scope_match(tree.begin(), U"p", std::back_inserter(v));
	BOOST_CHECK(v.size() == 1);
	std::u32string s;
	to_string(v.front(), std::back_inserter(s));
	BOOST_CHECK(s == U"text"); // to_string() はノードの子を文字列化するため、<p> 自身は含まれない。
}

// 複数のノードでマッチ
BOOST_AUTO_TEST_CASE(infrastructure_scope_match_2)
{
	using namespace wordring::html;
	using namespace wordring::whatwg::dom;

	std::u8string html = u8"<p>1</p><p>2</p>";
	u8simple_tree tree = make_document<u8simple_tree>(html.begin(), html.end());

	std::vector<u8simple_tree::const_iterator> v;

	// 文書ノードから<p>タグを検索
	scope_match(tree.begin(), U"p", std::back_inserter(v));
	BOOST_CHECK(v.size() == 2);
	std::u32string s;
	to_string(v[0], std::back_inserter(s));
	BOOST_CHECK(s == U"1"); // to_string() はノードの子を文字列化するため、<p> 自身は含まれない。
	s.clear();
	to_string(v[1], std::back_inserter(s));
	BOOST_CHECK(s == U"2"); // to_string() はノードの子を文字列化するため、<p> 自身は含まれない。
}

// ノード自身にはマッチしない
BOOST_AUTO_TEST_CASE(infrastructure_scope_match_3)
{
	using namespace wordring::html;
	using namespace wordring::whatwg::dom;

	std::u8string html = u8"<p>text</p>";
	u8simple_tree tree = make_document<u8simple_tree>(html.begin(), html.end());

	std::vector<u8simple_tree::const_iterator> v1, v2;

	// 文書ノードから<p>タグを検索
	scope_match(tree.begin(), U"p", std::back_inserter(v1));
	BOOST_CHECK(v1.size() == 1);

	// <p> タグから <p> タグ自身を検索
	scope_match(v1[0], U"p", std::back_inserter(v2));
	BOOST_CHECK(v2.size() == 0);
}

BOOST_AUTO_TEST_SUITE_END()
