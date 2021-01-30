// test/tag_tree/tag_tree.cpp

#include <boost/test/unit_test.hpp>

#include <wordring/tag_tree/tag_tree.hpp>

#include <wordring/css/selector.hpp>
#include <wordring/html/simple_html.hpp>
#include <wordring/html/simple_traits.hpp>

#include <iterator>
#include <string>

namespace wordring::html
{
}

namespace
{
	class test_tree : public wordring::tag_tree<wordring::html::simple_node<std::u8string>>
	{
	public:
		using wordring::tag_tree<wordring::html::simple_node<std::u8string>>::allocate;
		using wordring::tag_tree<wordring::html::simple_node<std::u8string>>::free;
		using wordring::tag_tree<wordring::html::simple_node<std::u8string>>::link;
		using wordring::tag_tree<wordring::html::simple_node<std::u8string>>::unlink;
		using wordring::tag_tree<wordring::html::simple_node<std::u8string>>::m_c;
	};

	std::u8string print(test_tree::const_iterator it)
	{
		using namespace wordring::html;

		using traits = node_traits<test_tree::const_iterator>;
		if (traits::is_text(it)) return traits::data(it);

		return u8"";
	}
}

BOOST_AUTO_TEST_SUITE(tag_tree_test)

// ------------------------------------------------------------------------------------------------
// struct tag_node
// ------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------
// class tag_tree
// ------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(tag_tree_construct_1)
{
	using namespace wordring::html;
	test_tree tree;
	BOOST_CHECK(tree.begin() == tree.end());
}

BOOST_AUTO_TEST_CASE(tag_tree_copy_construct_1)
{
	using namespace wordring::html;
	test_tree t1;
	t1.insert(t1.end(), simple_text<std::u8string>{ u8"1" });

	test_tree t2(t1);
	BOOST_CHECK(u8"1" == print(t2.begin()));
}

BOOST_AUTO_TEST_CASE(tag_tree_move_construct_1)
{
	using namespace wordring::html;
	test_tree t1;
	t1.insert(t1.end(), simple_text<std::u8string>{ u8"1" });

	test_tree t2(std::move(t1));
	BOOST_CHECK(u8"1" == print(t2.begin()));
}

BOOST_AUTO_TEST_CASE(tag_tree_copy_assign_1)
{
	using namespace wordring::html;
	test_tree t1;
	t1.insert(t1.end(), simple_text<std::u8string>{ u8"1" });

	test_tree t2;
	t2 = t1;
	BOOST_CHECK(u8"1" == print(t2.begin()));
}

BOOST_AUTO_TEST_CASE(tag_tree_move_assign_1)
{
	using namespace wordring::html;
	test_tree t1;
	t1.insert(t1.end(), simple_text<std::u8string>{ u8"1" });

	test_tree t2;
	t2 = std::move(t1);
	BOOST_CHECK(u8"1" == print(t2.begin()));
}

BOOST_AUTO_TEST_CASE(tag_tree_begin_1)
{
	using namespace wordring::html;
	test_tree t1;
	t1.insert(t1.end(), simple_text<std::u8string>{ u8"1" });

	BOOST_CHECK(u8"1" == print(t1.begin()));
}

BOOST_AUTO_TEST_CASE(tag_tree_begin_2)
{
	using namespace wordring::html;
	test_tree t1;
	t1.insert(t1.end(), simple_text<std::u8string>{ u8"1" });

	test_tree const& t2 = t1;
	BOOST_CHECK(u8"1" == print(t2.begin()));
}

BOOST_AUTO_TEST_CASE(tag_tree_cbegin_1)
{
	using namespace wordring::html;
	test_tree t1;
	t1.insert(t1.cend(), simple_text<std::u8string>{ u8"1" });

	BOOST_CHECK(u8"1" == print(t1.cbegin()));
}

BOOST_AUTO_TEST_CASE(tag_tree_end_1)
{
	using namespace wordring::html;
	test_tree t1;
	t1.insert(t1.end(), simple_text<std::u8string>{ u8"1" });
	t1.insert(t1.end(), simple_text<std::u8string>{ u8"2" });
	t1.insert(t1.end(), simple_text<std::u8string>{ u8"3" });

	BOOST_CHECK(u8"3" == print(--t1.end()));
	BOOST_CHECK(u8"2" == print(----t1.end()));
	BOOST_CHECK(u8"1" == print(------t1.end()));
}

BOOST_AUTO_TEST_CASE(tag_tree_end_2)
{
	using namespace wordring::html;
	test_tree t1;
	t1.insert(t1.end(), simple_text<std::u8string>{ u8"1" });
	t1.insert(t1.end(), simple_text<std::u8string>{ u8"2" });
	t1.insert(t1.end(), simple_text<std::u8string>{ u8"3" });

	test_tree const& t2 = t1;
	BOOST_CHECK(u8"3" == print(--t2.end()));
	BOOST_CHECK(u8"2" == print(----t2.end()));
	BOOST_CHECK(u8"1" == print(------t2.end()));
}

BOOST_AUTO_TEST_CASE(tag_tree_cend_1)
{
	using namespace wordring::html;
	test_tree t1;
	t1.insert(t1.cend(), simple_text<std::u8string>{ u8"1" });
	t1.insert(t1.cend(), simple_text<std::u8string>{ u8"2" });
	t1.insert(t1.cend(), simple_text<std::u8string>{ u8"3" });

	BOOST_CHECK(u8"3" == print(--t1.cend()));
	BOOST_CHECK(u8"2" == print(----t1.cend()));
	BOOST_CHECK(u8"1" == print(------t1.cend()));
}

BOOST_AUTO_TEST_CASE(tag_tree_empty_1)
{
	using namespace wordring::html;
	test_tree t1;
	t1.insert(t1.cend(), simple_text<std::u8string>{ u8"1" });

	BOOST_CHECK(!t1.empty());
}

BOOST_AUTO_TEST_CASE(tag_tree_empty_2)
{
	using namespace wordring::html;
	test_tree t1;
	t1.insert(t1.cend(), simple_text<std::u8string>{ u8"1" });
	t1.insert(t1.cend(), simple_text<std::u8string>{ u8"2" });

	BOOST_CHECK(!t1.empty());
}

BOOST_AUTO_TEST_CASE(tag_tree_empty_3)
{
	using namespace wordring::html;
	test_tree t1;
	BOOST_CHECK(t1.empty());
}

BOOST_AUTO_TEST_CASE(tag_tree_size_1)
{
	using namespace wordring::html;
	test_tree t1;
	t1.insert(t1.cend(), simple_text<std::u8string>{ u8"1" });

	BOOST_CHECK(t1.size() == 1);
}

BOOST_AUTO_TEST_CASE(tag_tree_size_2)
{
	using namespace wordring::html;
	test_tree t1;
	t1.insert(t1.cend(), simple_text<std::u8string>{ u8"1" });
	t1.insert(t1.cend(), simple_text<std::u8string>{ u8"2" });

	BOOST_CHECK(t1.size() == 2);
}

BOOST_AUTO_TEST_CASE(tag_tree_size_3)
{
	using namespace wordring::html;

	test_tree t1;
	auto it = t1.insert(t1.end(), simple_element<std::u8string>{});
	t1.insert(it.end(), simple_text<std::u8string>{ u8"1" });
	t1.insert(it.end(), simple_text<std::u8string>{ u8"2" });
	t1.insert(it.end(), simple_text<std::u8string>{ u8"3" });

	BOOST_CHECK(t1.size() == 4);
}

BOOST_AUTO_TEST_CASE(tag_tree_size_4)
{
	using namespace wordring::html;

	test_tree t1;
	BOOST_CHECK(t1.size() == 0);
}

BOOST_AUTO_TEST_CASE(tag_tree_clear_1)
{
	using namespace wordring::html;

	test_tree t1;
	auto it = t1.insert(t1.end(), simple_element<std::u8string>{});
	t1.insert(it.end(), simple_text<std::u8string>{ u8"1" });
	t1.insert(it.end(), simple_text<std::u8string>{ u8"2" });
	t1.insert(it.end(), simple_text<std::u8string>{ u8"3" });

	BOOST_CHECK(t1.size() == 4);

	t1.clear();
	BOOST_CHECK(t1.size() == 0);
}

BOOST_AUTO_TEST_CASE(tag_tree_swap_1)
{
	using namespace wordring::html;

	test_tree t1;
	auto it1 = t1.insert(t1.end(), simple_element<std::u8string>{});
	t1.insert(it1.end(), simple_text<std::u8string>{ u8"1" });
	t1.insert(it1.end(), simple_text<std::u8string>{ u8"2" });
	t1.insert(it1.end(), simple_text<std::u8string>{ u8"3" });
	BOOST_CHECK(t1.size() == 4);

	test_tree t2;
	t2.insert(t2.end(), simple_element<std::u8string>{});
	BOOST_CHECK(t2.size() == 1);

	t1.swap(t2);

	BOOST_CHECK(t1.size() == 1);
	BOOST_CHECK(t2.size() == 4);
}

BOOST_AUTO_TEST_CASE(tag_tree_insert_1)
{
	using namespace wordring::html;

	test_tree tree;
	auto it = tree.insert(tree.end(), simple_element<std::u8string>{});
	tree.insert(it.end(), simple_text<std::u8string>{ u8"1" });
	tree.insert(it.end(), simple_text<std::u8string>{ u8"2" });
	tree.insert(it.end(), simple_text<std::u8string>{ u8"3" });

	std::u8string s;
	auto it1 = it.begin();
	auto it2 = it.end();
	while (it1 != it2)
	{
		s += print(it1);
		++it1;
	}

	BOOST_CHECK(s == u8"123");
}

BOOST_AUTO_TEST_CASE(tag_tree_insert_2)
{
	using namespace wordring::html;

	test_tree tree;
	auto it1 = tree.insert(tree.end(), simple_element<std::u8string>{});
	auto it2 = tree.insert(it1.end(), simple_text<std::u8string>{ u8"1" });
	auto it3 = tree.insert(it2, simple_text<std::u8string>{ u8"2" });
	tree.insert(it3, simple_text<std::u8string>{ u8"3" });

	std::u8string s;
	auto it4 = it1.begin();
	auto it5 = it1.end();
	while (it4 != it5)
	{
		s += print(it4);
		++it4;
	}

	BOOST_CHECK(s == u8"321");
}

BOOST_AUTO_TEST_CASE(tag_tree_move_1)
{
	using namespace wordring::html;

	test_tree t1;
	auto it1 = t1.insert(t1.end(), simple_element<std::u8string>{});
	t1.insert(t1.end(), simple_text<std::u8string>{ u8"1" });

	t1.move(it1.end(), ++t1.begin());

	BOOST_CHECK((*t1.m_c)[0].m_next == 1);
	BOOST_CHECK((*t1.m_c)[1].m_next == 3);
	BOOST_CHECK((*t1.m_c)[3].m_next == 2);
	BOOST_CHECK((*t1.m_c)[2].m_next == 0);
	BOOST_CHECK(print(it1.begin()) == u8"1");
}

BOOST_AUTO_TEST_CASE(tag_tree_move_2)
{
	using namespace wordring::html;

	test_tree t1;
	auto it1 = t1.insert(t1.end(), simple_element<std::u8string>{});
	auto it2 = t1.insert(t1.end(), simple_element<std::u8string>{});
	t1.insert(it2.end(), simple_text<std::u8string>{ u8"1" });

	t1.move(it1, it2);

	BOOST_CHECK((*t1.m_c)[0].m_next == 3);
	BOOST_CHECK((*t1.m_c)[3].m_next == 5);
	BOOST_CHECK((*t1.m_c)[5].m_next == 4);
	BOOST_CHECK((*t1.m_c)[4].m_next == 1);
	BOOST_CHECK((*t1.m_c)[1].m_next == 2);
	BOOST_CHECK((*t1.m_c)[2].m_next == 0);
	BOOST_CHECK(print(t1.begin().begin()) == u8"1");
}

BOOST_AUTO_TEST_CASE(tag_tree_move_3)
{
	using namespace wordring::html;

	test_tree t1;
	auto it1 = t1.insert(t1.end(), simple_element<std::u8string>{});
	auto it2 = t1.insert(t1.end(), simple_element<std::u8string>{});
	t1.insert(it2.end(), simple_element<std::u8string>{});

	t1.move(it1, it2);

	BOOST_CHECK((*t1.m_c)[0].m_next == 3);
	BOOST_CHECK((*t1.m_c)[3].m_next == 5);
	BOOST_CHECK((*t1.m_c)[5].m_next == 6);
	BOOST_CHECK((*t1.m_c)[6].m_next == 4);
	BOOST_CHECK((*t1.m_c)[4].m_next == 1);
	BOOST_CHECK((*t1.m_c)[1].m_next == 2);
	BOOST_CHECK((*t1.m_c)[2].m_next == 0);
}

BOOST_AUTO_TEST_CASE(tag_tree_move_4)
{
	using namespace wordring::html;

	test_tree t1;
	auto it1 = t1.insert(t1.end(), simple_element<std::u8string>{});
	auto it2 = t1.insert(t1.end(), simple_element<std::u8string>{});
	auto it3 = t1.insert(it2.end(), simple_element<std::u8string>{});

	t1.move(it1.end(), it3);

	BOOST_CHECK((*t1.m_c)[0].m_next == 1);
	BOOST_CHECK((*t1.m_c)[1].m_next == 5);
	BOOST_CHECK((*t1.m_c)[5].m_next == 6);
	BOOST_CHECK((*t1.m_c)[6].m_next == 2);
	BOOST_CHECK((*t1.m_c)[2].m_next == 3);
	BOOST_CHECK((*t1.m_c)[3].m_next == 4);
	BOOST_CHECK((*t1.m_c)[4].m_next == 0);
}

BOOST_AUTO_TEST_CASE(tag_tree_erase_1)
{
	using namespace wordring::html;

	test_tree t1;
	t1.insert(t1.end(), simple_text<std::u8string>{ u8"1" });
	t1.insert(t1.end(), simple_text<std::u8string>{ u8"2" });
	t1.insert(t1.end(), simple_text<std::u8string>{ u8"3" });

	t1.erase(t1.begin());

	std::u8string s;
	auto it1 = t1.begin();
	auto it2 = t1.end();
	while (it1 != it2)
	{
		s += print(it1);
		++it1;
	}

	BOOST_CHECK(s == u8"23");
}

BOOST_AUTO_TEST_CASE(tag_tree_erase_2)
{
	using namespace wordring::html;

	test_tree t1;
	t1.insert(t1.end(), simple_text<std::u8string>{ u8"1" });
	t1.insert(t1.end(), simple_text<std::u8string>{ u8"2" });
	t1.insert(t1.end(), simple_text<std::u8string>{ u8"3" });

	t1.erase(++t1.begin());

	std::u8string s;
	auto it1 = t1.begin();
	auto it2 = t1.end();
	while (it1 != it2)
	{
		s += print(it1);
		++it1;
	}

	BOOST_CHECK(s == u8"13");
}

BOOST_AUTO_TEST_CASE(tag_tree_erase_3)
{
	using namespace wordring::html;

	test_tree t1;
	t1.insert(t1.end(), simple_text<std::u8string>{ u8"1" });
	t1.insert(t1.end(), simple_text<std::u8string>{ u8"2" });
	t1.insert(t1.end(), simple_text<std::u8string>{ u8"3" });

	t1.erase(++++t1.begin());

	std::u8string s;
	auto it1 = t1.begin();
	auto it2 = t1.end();
	while (it1 != it2)
	{
		s += print(it1);
		++it1;
	}

	BOOST_CHECK(s == u8"12");
}

BOOST_AUTO_TEST_CASE(tag_tree_allocate_1)
{
	using namespace wordring::html;

	test_tree tree;
	std::uint32_t idx = tree.allocate(simple_text<std::u8string>{});
	BOOST_CHECK(idx == 1);
}

BOOST_AUTO_TEST_CASE(tag_tree_allocate_2)
{
	using namespace wordring::html;

	test_tree tree;
	std::uint32_t i1 = tree.allocate(simple_text<std::u8string>{});
	std::uint32_t i2 = tree.allocate(simple_text<std::u8string>{});
	std::uint32_t i3 = tree.allocate(simple_text<std::u8string>{});
	tree.free(i1);
	tree.free(i2);
	tree.free(i3);

	std::uint32_t i4 = tree.allocate(simple_text<std::u8string>{});
	BOOST_CHECK(i4 == 1);

	BOOST_CHECK((*tree.m_c)[0].m_head == 3);
	BOOST_CHECK((*tree.m_c)[0].m_tail == 2);

	BOOST_CHECK((*tree.m_c)[2].m_head == 0);
	BOOST_CHECK((*tree.m_c)[2].m_tail == 3);

	BOOST_CHECK((*tree.m_c)[3].m_head == 2);
	BOOST_CHECK((*tree.m_c)[3].m_tail == 0);
}

BOOST_AUTO_TEST_CASE(tag_tree_free_1)
{
	using namespace wordring::html;

	test_tree tree;
	std::uint32_t i1 = tree.allocate(simple_text<std::u8string>{});
	tree.free(i1);

	BOOST_CHECK((*tree.m_c)[0].m_head == 1);
	BOOST_CHECK((*tree.m_c)[0].m_tail == 1);

	BOOST_CHECK((*tree.m_c)[1].m_head == 0);
	BOOST_CHECK((*tree.m_c)[1].m_tail == 0);
}

BOOST_AUTO_TEST_CASE(tag_tree_free_2)
{
	using namespace wordring::html;

	test_tree tree;
	std::uint32_t i1 = tree.allocate(simple_text<std::u8string>{});
	std::uint32_t i2 = tree.allocate(simple_text<std::u8string>{});
	std::uint32_t i3 = tree.allocate(simple_text<std::u8string>{});
	tree.free(i1);
	tree.free(i2);
	tree.free(i3);

	BOOST_CHECK((*tree.m_c)[0].m_head == 3);
	BOOST_CHECK((*tree.m_c)[0].m_tail == 1);

	BOOST_CHECK((*tree.m_c)[1].m_head == 0);
	BOOST_CHECK((*tree.m_c)[1].m_tail == 2);

	BOOST_CHECK((*tree.m_c)[2].m_head == 1);
	BOOST_CHECK((*tree.m_c)[2].m_tail == 3);

	BOOST_CHECK((*tree.m_c)[3].m_head == 2);
	BOOST_CHECK((*tree.m_c)[3].m_tail == 0);
}

BOOST_AUTO_TEST_CASE(tag_tree_link_1)
{
	using namespace wordring::html;

	test_tree tree;
	BOOST_CHECK((*tree.m_c)[0].m_prev == 0);
	BOOST_CHECK((*tree.m_c)[0].m_next == 0);

	std::uint32_t i = tree.allocate(simple_text<std::u8string>{});
	BOOST_CHECK(i == 1);
	tree.link(0, i);
	BOOST_CHECK((*tree.m_c)[0].m_prev == 1);
	BOOST_CHECK((*tree.m_c)[0].m_next == 1);
	BOOST_CHECK((*tree.m_c)[1].m_prev == 0);
	BOOST_CHECK((*tree.m_c)[1].m_next == 0);

	i = tree.allocate(simple_text<std::u8string>{});
	BOOST_CHECK(i == 2);
	tree.link(0, i);
	BOOST_CHECK((*tree.m_c)[0].m_prev == 2);
	BOOST_CHECK((*tree.m_c)[0].m_next == 1);
	BOOST_CHECK((*tree.m_c)[1].m_prev == 0);
	BOOST_CHECK((*tree.m_c)[1].m_next == 2);
	BOOST_CHECK((*tree.m_c)[2].m_prev == 1);
	BOOST_CHECK((*tree.m_c)[2].m_next == 0);

	i = tree.allocate(simple_text<std::u8string>{});
	BOOST_CHECK(i == 3);
	tree.link(0, i);
	BOOST_CHECK((*tree.m_c)[0].m_prev == 3);
	BOOST_CHECK((*tree.m_c)[0].m_next == 1);
	BOOST_CHECK((*tree.m_c)[1].m_prev == 0);
	BOOST_CHECK((*tree.m_c)[1].m_next == 2);
	BOOST_CHECK((*tree.m_c)[2].m_prev == 1);
	BOOST_CHECK((*tree.m_c)[2].m_next == 3);
	BOOST_CHECK((*tree.m_c)[3].m_prev == 2);
	BOOST_CHECK((*tree.m_c)[3].m_next == 0);
}

BOOST_AUTO_TEST_CASE(tag_tree_link_2)
{
	using namespace wordring::html;

	test_tree tree;
	BOOST_CHECK((*tree.m_c)[0].m_prev == 0);
	BOOST_CHECK((*tree.m_c)[0].m_next == 0);

	std::uint32_t i = tree.allocate(simple_text<std::u8string>{ u8"1" });
	tree.link(0, i);
	i = tree.allocate(simple_text<std::u8string>{ u8"2" });
	tree.link(0, i);
	i = tree.allocate(simple_text<std::u8string>{ u8"3" });
	tree.link(0, i);
	BOOST_CHECK((*tree.m_c)[0].m_prev == 3);
	BOOST_CHECK((*tree.m_c)[0].m_next == 1);
	BOOST_CHECK((*tree.m_c)[1].m_prev == 0);
	BOOST_CHECK((*tree.m_c)[1].m_next == 2);
	BOOST_CHECK((*tree.m_c)[2].m_prev == 1);
	BOOST_CHECK((*tree.m_c)[2].m_next == 3);
	BOOST_CHECK((*tree.m_c)[3].m_prev == 2);
	BOOST_CHECK((*tree.m_c)[3].m_next == 0);

	tree.unlink(2);
	BOOST_CHECK((*tree.m_c)[0].m_prev == 3);
	BOOST_CHECK((*tree.m_c)[0].m_next == 1);
	BOOST_CHECK((*tree.m_c)[1].m_prev == 0);
	BOOST_CHECK((*tree.m_c)[1].m_next == 3);
	BOOST_CHECK((*tree.m_c)[3].m_prev == 1);
	BOOST_CHECK((*tree.m_c)[3].m_next == 0);

	tree.free(2);
	i = tree.allocate(simple_text<std::u8string>{ u8"4" });
	BOOST_CHECK(i == 2);
	tree.link(0, i);
	BOOST_CHECK((*tree.m_c)[0].m_prev == 2);
	BOOST_CHECK((*tree.m_c)[0].m_next == 1);
	BOOST_CHECK((*tree.m_c)[1].m_prev == 0);
	BOOST_CHECK((*tree.m_c)[1].m_next == 3);
	BOOST_CHECK((*tree.m_c)[3].m_prev == 1);
	BOOST_CHECK((*tree.m_c)[3].m_next == 2);
	BOOST_CHECK((*tree.m_c)[2].m_prev == 3);
	BOOST_CHECK((*tree.m_c)[2].m_next == 0);
}

BOOST_AUTO_TEST_CASE(tag_tree_link_3)
{
	using namespace wordring::html;

	test_tree tree;
	std::uint32_t i = tree.allocate(simple_text<std::u8string>{ u8"1" });
	BOOST_CHECK(i == 1);
	tree.link(0, i);
	i = tree.allocate(simple_text<std::u8string>{ u8"2" });
	BOOST_CHECK(i == 2);
	tree.link(1, i);

	BOOST_CHECK((*tree.m_c)[0].m_prev == 1);
	BOOST_CHECK((*tree.m_c)[0].m_next == 2);
	BOOST_CHECK((*tree.m_c)[2].m_prev == 0);
	BOOST_CHECK((*tree.m_c)[2].m_next == 1);
	BOOST_CHECK((*tree.m_c)[1].m_prev == 2);
	BOOST_CHECK((*tree.m_c)[1].m_next == 0);
}

BOOST_AUTO_TEST_CASE(tag_tree_link_4)
{
	using namespace wordring::html;

	test_tree tree;
	std::uint32_t i = tree.allocate(simple_text<std::u8string>{ u8"1" });
	BOOST_CHECK(i == 1);
	tree.link(0, i);
	i = tree.allocate(simple_text<std::u8string>{ u8"2" });
	BOOST_CHECK(i == 2);
	tree.link(1, i);
	i = tree.allocate(simple_text<std::u8string>{ u8"2" });
	BOOST_CHECK(i == 3);
	tree.link(0, i);

	BOOST_CHECK((*tree.m_c)[0].m_prev == 3);
	BOOST_CHECK((*tree.m_c)[0].m_next == 2);
	BOOST_CHECK((*tree.m_c)[2].m_prev == 0);
	BOOST_CHECK((*tree.m_c)[2].m_next == 1);
	BOOST_CHECK((*tree.m_c)[1].m_prev == 2);
	BOOST_CHECK((*tree.m_c)[1].m_next == 3);
	BOOST_CHECK((*tree.m_c)[3].m_prev == 1);
	BOOST_CHECK((*tree.m_c)[3].m_next == 0);
}

BOOST_AUTO_TEST_CASE(tag_tree_unlink_1)
{
	using namespace wordring::html;

	test_tree tree;
	std::uint32_t i = tree.allocate(simple_text<std::u8string>{});
	tree.link(0, i);
	i = tree.allocate(simple_text<std::u8string>{});
	tree.link(0, i);
	i = tree.allocate(simple_text<std::u8string>{});
	tree.link(0, i);
	BOOST_CHECK((*tree.m_c)[0].m_prev == 3);
	BOOST_CHECK((*tree.m_c)[0].m_next == 1);
	BOOST_CHECK((*tree.m_c)[1].m_prev == 0);
	BOOST_CHECK((*tree.m_c)[1].m_next == 2);
	BOOST_CHECK((*tree.m_c)[2].m_prev == 1);
	BOOST_CHECK((*tree.m_c)[2].m_next == 3);
	BOOST_CHECK((*tree.m_c)[3].m_prev == 2);
	BOOST_CHECK((*tree.m_c)[3].m_next == 0);

	tree.unlink(1);
	BOOST_CHECK((*tree.m_c)[0].m_prev == 3);
	BOOST_CHECK((*tree.m_c)[0].m_next == 2);
	BOOST_CHECK((*tree.m_c)[2].m_prev == 0);
	BOOST_CHECK((*tree.m_c)[2].m_next == 3);
	BOOST_CHECK((*tree.m_c)[3].m_prev == 2);
	BOOST_CHECK((*tree.m_c)[3].m_next == 0);
}

BOOST_AUTO_TEST_CASE(tag_tree_unlink_2)
{
	using namespace wordring::html;

	test_tree tree;
	std::uint32_t i = tree.allocate(simple_text<std::u8string>{});
	tree.link(0, i);
	i = tree.allocate(simple_text<std::u8string>{});
	tree.link(0, i);
	i = tree.allocate(simple_text<std::u8string>{});
	tree.link(0, i);
	BOOST_CHECK((*tree.m_c)[0].m_prev == 3);
	BOOST_CHECK((*tree.m_c)[0].m_next == 1);
	BOOST_CHECK((*tree.m_c)[1].m_prev == 0);
	BOOST_CHECK((*tree.m_c)[1].m_next == 2);
	BOOST_CHECK((*tree.m_c)[2].m_prev == 1);
	BOOST_CHECK((*tree.m_c)[2].m_next == 3);
	BOOST_CHECK((*tree.m_c)[3].m_prev == 2);
	BOOST_CHECK((*tree.m_c)[3].m_next == 0);

	tree.unlink(2);
	BOOST_CHECK((*tree.m_c)[0].m_prev == 3);
	BOOST_CHECK((*tree.m_c)[0].m_next == 1);
	BOOST_CHECK((*tree.m_c)[1].m_prev == 0);
	BOOST_CHECK((*tree.m_c)[1].m_next == 3);
	BOOST_CHECK((*tree.m_c)[3].m_prev == 1);
	BOOST_CHECK((*tree.m_c)[3].m_next == 0);
}

BOOST_AUTO_TEST_CASE(tag_tree_unlink_3)
{
	using namespace wordring::html;

	test_tree tree;
	std::uint32_t i = tree.allocate(simple_text<std::u8string>{});
	tree.link(0, i);
	i = tree.allocate(simple_text<std::u8string>{});
	tree.link(0, i);
	i = tree.allocate(simple_text<std::u8string>{});
	tree.link(0, i);
	BOOST_CHECK((*tree.m_c)[0].m_prev == 3);
	BOOST_CHECK((*tree.m_c)[0].m_next == 1);
	BOOST_CHECK((*tree.m_c)[1].m_prev == 0);
	BOOST_CHECK((*tree.m_c)[1].m_next == 2);
	BOOST_CHECK((*tree.m_c)[2].m_prev == 1);
	BOOST_CHECK((*tree.m_c)[2].m_next == 3);
	BOOST_CHECK((*tree.m_c)[3].m_prev == 2);
	BOOST_CHECK((*tree.m_c)[3].m_next == 0);

	tree.unlink(3);
	BOOST_CHECK((*tree.m_c)[0].m_prev == 2);
	BOOST_CHECK((*tree.m_c)[0].m_next == 1);
	BOOST_CHECK((*tree.m_c)[1].m_prev == 0);
	BOOST_CHECK((*tree.m_c)[1].m_next == 2);
	BOOST_CHECK((*tree.m_c)[2].m_prev == 1);
	BOOST_CHECK((*tree.m_c)[2].m_next == 0);
}

BOOST_AUTO_TEST_CASE(tag_tree_unlink_4)
{
	using namespace wordring::html;

	test_tree tree;
	std::uint32_t i = tree.allocate(simple_text<std::u8string>{});
	tree.link(0, i);
	BOOST_CHECK((*tree.m_c)[0].m_prev == 1);
	BOOST_CHECK((*tree.m_c)[0].m_next == 1);
	BOOST_CHECK((*tree.m_c)[1].m_prev == 0);
	BOOST_CHECK((*tree.m_c)[1].m_next == 0);
	tree.unlink(1);
	BOOST_CHECK((*tree.m_c)[0].m_prev == 0);
	BOOST_CHECK((*tree.m_c)[0].m_next == 0);
}

BOOST_AUTO_TEST_SUITE_END()
