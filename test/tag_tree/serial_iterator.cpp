// test/tag_tree/serial_iterator.cpp

#include <boost/test/unit_test.hpp>

#include <wordring/tag_tree/serial_iterator.hpp>
#include <wordring/tag_tree/tag_tree.hpp>

#include <wordring/css/selector.hpp>
#include <wordring/html/simple_html.hpp>
#include <wordring/html/simple_traits.hpp>

#include <iterator>
#include <string>
#include <type_traits>

static_assert(std::is_copy_constructible_v<wordring::detail::tag_tree_serial_iterator<wordring::html::u8simple_node>>);
static_assert(std::is_copy_assignable_v<wordring::detail::tag_tree_serial_iterator<wordring::html::u8simple_node>>);

namespace
{
	std::u8string print(wordring::html::u8simple_tree::const_serial_iterator it)
	{
		using namespace wordring::html;

		using traits = node_traits<u8simple_tree::const_iterator>;
		if (traits::is_text(it)) return traits::data(it);

		std::u8string s;

		auto it1 = it.start_tag();
		if (it1 != u8simple_tree::const_serial_iterator())
		{
			s += u8"<";
			if (it.is_end_tag()) s += u8"/";
			s += traits::get_local_name(it1);
			s += u8">";
		}

		return s;
	}
}

BOOST_AUTO_TEST_SUITE(tag_tree_serial_iterator_test)

// ------------------------------------------------------------------------------------------------
// class tag_tree_serial_iterator
// ------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(serial_iterator_construct_1)
{
	using namespace wordring::detail;
	using namespace wordring::html;

	u8simple_tree t1;
	t1.insert(t1.end(), simple_element<std::u8string>(u8"p"));

	tag_tree_serial_iterator<u8simple_node> it1(t1.begin());

	BOOST_CHECK(it1->local_name() == u8"p");
}

BOOST_AUTO_TEST_CASE(serial_iterator_convert_1)
{
	using namespace wordring::detail;
	using namespace wordring::html;

	u8simple_tree t1;
	auto it1 = t1.insert(t1.end(), simple_element<std::u8string>(u8"p"));

	tag_tree_iterator<u8simple_node> it = it1;

	BOOST_CHECK(it->local_name() == u8"p");
}

BOOST_AUTO_TEST_CASE(serial_iterator_convert_2)
{
	using namespace wordring::detail;
	using namespace wordring::html;

	u8simple_tree t1;
	auto it1 = t1.insert(t1.end(), simple_element<std::u8string>(u8"p"));

	tag_tree_iterator<u8simple_node const> it = it1;

	BOOST_CHECK(it->local_name() == u8"p");
}

// 実際は変換関数が呼ばれてるだけ。
BOOST_AUTO_TEST_CASE(serial_iterator_assign_1)
{
	using namespace wordring::detail;
	using namespace wordring::html;

	u8simple_tree t1;
	t1.insert(t1.end(), simple_element<std::u8string>(u8"p"));

	tag_tree_serial_iterator<u8simple_node> it1 = t1.begin();

	BOOST_CHECK(it1->local_name() == u8"p");
}

BOOST_AUTO_TEST_CASE(serial_iterator_reference_1)
{
	using namespace wordring::detail;
	using namespace wordring::html;

	u8simple_tree t1;
	t1.insert(t1.end(), simple_element<std::u8string>(u8"p"));

	u8simple_node const& n1 = *t1.sbegin();

	BOOST_CHECK(n1.local_name() == u8"p");
}

BOOST_AUTO_TEST_CASE(serial_iterator_pointer_1)
{
	using namespace wordring::detail;
	using namespace wordring::html;

	u8simple_tree t1;
	t1.insert(t1.end(), simple_element<std::u8string>(u8"p"));

	BOOST_CHECK(t1.sbegin()->local_name() == u8"p");
}

BOOST_AUTO_TEST_CASE(serial_iterator_increment_1)
{
	using namespace wordring::html;
	u8simple_tree t1;
	auto it1 = t1.insert(t1.end(), simple_element<std::u8string>(u8"p"));
	auto it2 = t1.insert(t1.end(), simple_element<std::u8string>(u8"p"));
	auto it3 = t1.insert(t1.end(), simple_element<std::u8string>(u8"p"));

	t1.insert(it1.end(), simple_text<std::u8string>{ u8"TEXT1" });
	t1.insert(it2.end(), simple_text<std::u8string>{ u8"TEXT2" });
	t1.insert(it3.end(), simple_text<std::u8string>{ u8"TEXT3" });

	std::u8string s;
	auto it4 = t1.sbegin();
	auto it5 = t1.send();
	while (it4 != it5)
	{
		s += print(it4);
		++it4;
	}

	BOOST_CHECK(s == u8"<p>TEXT1</p><p>TEXT2</p><p>TEXT3</p>");
}

BOOST_AUTO_TEST_CASE(serial_iterator_decrement_1)
{
	using namespace wordring::html;
	u8simple_tree t1;
	auto it1 = t1.insert(t1.end(), simple_element<std::u8string>(u8"p"));
	auto it2 = t1.insert(t1.end(), simple_element<std::u8string>(u8"p"));
	auto it3 = t1.insert(t1.end(), simple_element<std::u8string>(u8"p"));

	t1.insert(it1.end(), simple_text<std::u8string>{ u8"TEXT1" });
	t1.insert(it2.end(), simple_text<std::u8string>{ u8"TEXT2" });
	t1.insert(it3.end(), simple_text<std::u8string>{ u8"TEXT3" });

	std::u8string s;
	auto it4 = t1.sbegin();
	auto it5 = t1.send();
	while (true)
	{
		s += print(it5);
		if (it4 == it5) break;
		--it5;
	}

	BOOST_CHECK(s == u8"</p>TEXT3<p></p>TEXT2<p></p>TEXT1<p>");
}

BOOST_AUTO_TEST_CASE(serial_iterator_is_start_tag_1)
{
	using namespace wordring::detail;
	using namespace wordring::html;

	u8simple_tree t1;
	t1.insert(t1.end(), simple_element<std::u8string>(u8"p"));

	BOOST_CHECK(t1.sbegin().is_start_tag());
}

BOOST_AUTO_TEST_CASE(serial_iterator_is_end_tag_1)
{
	using namespace wordring::detail;
	using namespace wordring::html;

	u8simple_tree t1;
	t1.insert(t1.end(), simple_element<std::u8string>(u8"p"));

	BOOST_CHECK((--t1.send()).is_end_tag());
}

BOOST_AUTO_TEST_CASE(serial_iterator_start_tag_1)
{
	using namespace wordring::detail;
	using namespace wordring::html;

	u8simple_tree t1;
	t1.insert(t1.end(), simple_element<std::u8string>(u8"p"));

	BOOST_CHECK((--t1.send()).start_tag()->local_name() == u8"p");
}

BOOST_AUTO_TEST_CASE(serial_iterator_end_tag_1)
{
	using namespace wordring::detail;
	using namespace wordring::html;

	u8simple_tree t1;
	t1.insert(t1.end(), simple_element<std::u8string>(u8"p"));

	BOOST_CHECK(t1.sbegin().end_tag().is_end_tag());
}

BOOST_AUTO_TEST_SUITE_END()
