// test/tag_tree/tag_tree_iterator.cpp

#include <boost/test/unit_test.hpp>

#include <wordring/tag_tree/tag_tree.hpp>
#include <wordring/tag_tree/tag_tree_iterator.hpp>

#include <wordring/css/selector.hpp>
#include <wordring/html/simple_html.hpp>
#include <wordring/html/simple_traits.hpp>

#include <iterator>
#include <string>

namespace
{
	std::u8string print(wordring::html::u8simple_tree::const_iterator it)
	{
		using namespace wordring::html;

		using traits = node_traits<u8simple_tree::const_iterator>;
		if (traits::is_text(it)) return traits::data(it);

		return u8"";
	}
}

BOOST_AUTO_TEST_SUITE(tag_tree_iterator_test)

// ------------------------------------------------------------------------------------------------
// class tag_tree_iterator
// ------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(tag_tree_iterator_increment_1)
{
	using namespace wordring::html;
	u8simple_tree t1;
	t1.insert(t1.end(), simple_text<std::u8string>{ u8"1" });
	t1.insert(t1.end(), simple_text<std::u8string>{ u8"2" });
	t1.insert(t1.end(), simple_text<std::u8string>{ u8"3" });

	BOOST_CHECK(u8"1" == print(t1.begin()));
	BOOST_CHECK(u8"2" == print(++t1.begin()));
	BOOST_CHECK(u8"3" == print(++++t1.begin()));
}

BOOST_AUTO_TEST_CASE(tag_tree_iterator_increment_2)
{
	using namespace wordring::html;
	u8simple_tree t1;
	auto it1 = t1.insert(t1.end(), simple_element<std::u8string>{});
	t1.insert(it1.end(), simple_text<std::u8string>{ u8"1" });
	auto it3 = t1.insert(t1.end(), simple_element<std::u8string>{});
	t1.insert(it3.end(), simple_text<std::u8string>{ u8"1" });
	auto it5 = t1.insert(t1.end(), simple_element<std::u8string>{});
	t1.insert(it5.end(), simple_text<std::u8string>{ u8"1" });

	BOOST_CHECK(std::next(it1) == it3);
	BOOST_CHECK(std::next(it3) == it5);
	BOOST_CHECK(std::next(it5) == t1.end());
}

BOOST_AUTO_TEST_CASE(tag_tree_iterator_decrement_1)
{
	using namespace wordring::html;
	u8simple_tree t1;
	t1.insert(t1.end(), simple_text<std::u8string>{ u8"1" });
	t1.insert(t1.end(), simple_text<std::u8string>{ u8"2" });
	t1.insert(t1.end(), simple_text<std::u8string>{ u8"3" });

	BOOST_CHECK(u8"3" == print(--t1.end()));
	BOOST_CHECK(u8"2" == print(----t1.end()));
	BOOST_CHECK(u8"1" == print(------t1.end()));
}

BOOST_AUTO_TEST_CASE(tag_tree_iterator_decrement_2)
{
	using namespace wordring::html;
	u8simple_tree t1;
	auto it1 = t1.insert(t1.end(), simple_element<std::u8string>{});
	t1.insert(it1.end(), simple_text<std::u8string>{ u8"1" });
	auto it3 = t1.insert(t1.end(), simple_element<std::u8string>{});
	t1.insert(it3.end(), simple_text<std::u8string>{ u8"1" });
	auto it5 = t1.insert(t1.end(), simple_element<std::u8string>{});
	t1.insert(it5.end(), simple_text<std::u8string>{ u8"1" });

	BOOST_CHECK(std::prev(t1.end()) == it5);
	BOOST_CHECK(std::prev(it5) == it3);
	BOOST_CHECK(std::prev(it3) == it1);
}

BOOST_AUTO_TEST_CASE(tag_tree_iterator_parent_1)
{
	using namespace wordring::html;
	u8simple_tree t1;
	auto it1 = t1.insert(t1.end(), simple_element<std::u8string>{});

	BOOST_CHECK(it1.parent() == decltype(it1)());
}

BOOST_AUTO_TEST_CASE(tag_tree_iterator_parent_2)
{
	using namespace wordring::html;
	u8simple_tree t1;
	auto it1 = t1.insert(t1.end(), simple_element<std::u8string>{});
	auto it2 = t1.insert(it1.end(), simple_text<std::u8string>{ u8"1" });

	BOOST_CHECK(it2.parent() == it1);
}

BOOST_AUTO_TEST_CASE(tag_tree_iterator_parent_3)
{
	using namespace wordring::html;
	u8simple_tree t1;
	auto it1 = t1.insert(t1.end(), simple_element<std::u8string>{});
	t1.insert(it1.end(), simple_text<std::u8string>{ u8"1" });
	t1.insert(it1.end(), simple_text<std::u8string>{ u8"1" });
	auto it4 = t1.insert(it1.end(), simple_text<std::u8string>{ u8"1" });

	BOOST_CHECK(it4.parent() == it1);
}

BOOST_AUTO_TEST_CASE(tag_tree_iterator_begin_1)
{
	using namespace wordring::html;
	u8simple_tree t1;
	auto it1 = t1.insert(t1.end(), simple_element<std::u8string>{});
	auto it2 = t1.insert(it1.end(), simple_text<std::u8string>{ u8"1" });
	auto it3 = t1.insert(it1.end(), simple_text<std::u8string>{ u8"2" });

	BOOST_CHECK(it1.begin() == it2);
	BOOST_CHECK(--it1.end() == it3);
}

BOOST_AUTO_TEST_CASE(tag_tree_iterator_begin_2)
{
	using namespace wordring::html;
	u8simple_tree t1;
	auto it1 = t1.insert(t1.end(), simple_text<std::u8string>{ u8"1" });

	BOOST_CHECK(t1.begin() == it1);
	BOOST_CHECK(--t1.end() == it1);

	BOOST_CHECK(it1.begin() == it1.end());
}

BOOST_AUTO_TEST_CASE(tag_tree_iterator_begin_3)
{
	using namespace wordring::html;
	u8simple_tree t1;
	auto it1 = t1.insert(t1.end(), simple_text<std::u8string>{ u8"1" });
	t1.insert(t1.end(), simple_text<std::u8string>{ u8"1" });

	BOOST_CHECK(it1.begin() == it1.end());
}

BOOST_AUTO_TEST_SUITE_END()
