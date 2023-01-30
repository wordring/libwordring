// test/html/simple_html.cpp

#include <boost/test/unit_test.hpp>

#include <wordring/html/simple_html.hpp>

#include <wordring/compatibility.hpp>
#include <wordring/tree/tree.hpp>
#include <wordring/tree/tree_iterator.hpp>

#include <iterator>
#include <string>

namespace
{
	auto get_document(wordring::html::u8simple_tree& t)
	{
		auto it1 = wordring::tree_iterator<decltype(t.begin())>(t.begin());
		auto it2 = wordring::tree_iterator<decltype(t.begin())>();

		while (it1 != it2)
		{
			if (it1->is_document()) return it1.base();
			++it1;
		}

		return t.end();
	}
}

BOOST_AUTO_TEST_SUITE(simple_html_test)

BOOST_AUTO_TEST_CASE(simple_html_make_document_1)
{
	using namespace wordring::html;

	std::u8string const in = u8"あいうえお";

	std::u8string const s = u8R"*(<html><head></head><body>あいうえお</body></html>)*";

	auto tree = make_document<u8simple_tree>(in.begin(), in.end());
	auto doc = get_document(tree);

	std::u8string out;
	to_string(doc, std::back_inserter(out));

	BOOST_CHECK(s == out);
}

BOOST_AUTO_TEST_CASE(simple_html_make_document_2)
{
	using namespace wordring::html;

	std::string const in = "<meta charset=\"shift-jis\">\x82\xA0\x82\xA2\x82\xA4\x82\xA6\x82\xA8";

	std::u8string const s = u8R"*(<html><head><meta charset="shift-jis"></head><body>あいうえお</body></html>)*";

	auto tree = make_document<u8simple_tree>(in.begin(), in.end());
	auto doc = get_document(tree);

	std::u8string out;
	to_string(doc, std::back_inserter(out));

	std::string out2;
	for (std::size_t i = 0; i < out.size(); i++) out2 += static_cast<char>(out[i]);
	std::cout << out2;

	BOOST_CHECK(s == out);
}

BOOST_AUTO_TEST_SUITE_END()
