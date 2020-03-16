// test/tree/tree.cpp

#include <boost/test/unit_test.hpp>

#include <wordring/tree/tree.hpp>

#include <algorithm>
#include <iterator>
#include <list>
#include <string>
#include <type_traits>
#include <vector>

namespace
{
	struct test_tree : public wordring::tree<int>
	{
		using base_type = wordring::tree<int>;
		using typename base_type::container;

		using base_type::allocate;
		using base_type::free;

		using base_type::m_c;
	};

	struct test_iterator : public test_tree::iterator
	{
		using base_type = test_tree::iterator;

		using base_type::m_index;
		using base_type::m_parent;

		test_iterator(base_type const& it) : base_type(it) {}
	};

	struct const_test_iterator : public test_tree::const_iterator
	{
		using base_type = test_tree::const_iterator;
		using typename base_type::index_type;

		using base_type::m_index;
		using base_type::m_parent;

		const_test_iterator(base_type const& it) : base_type(it) {}

		const_test_iterator(typename test_tree::container const& c, index_type parent, index_type idx)
			: base_type(c, parent, idx)
		{
		}
	};
}

BOOST_AUTO_TEST_SUITE(tree__test)

// ----------------------------------------------------------------------------
// tree_node
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(tree_node__construct__1)
{
	using namespace wordring::detail;

	static_assert(std::is_pod_v<tree_node<int>>);

	tree_node<int> n{};

	BOOST_CHECK(n.m_value == 0);
}

// ------------------------------------------------------------------------
// tree_node_proxy
// ------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(tree_node_proxy__construct__1)
{
	wordring::detail::tree_node_proxy<char> p{ '0' };

	BOOST_CHECK(p.m_value == '0');
	BOOST_CHECK(p.m_children.empty());
}

BOOST_AUTO_TEST_CASE(tree_node_proxy__construct__2)
{
	wordring::detail::tree_node_proxy<char> p{ '0', { '1', '2', '3' } };

	BOOST_CHECK(p.m_value == '0');
	BOOST_CHECK(p.m_children.size() == 3);
}

BOOST_AUTO_TEST_CASE(tree_node_proxy__construct__3)
{
	wordring::detail::tree_node_proxy<char> p
	{
		'0',
		{
			{ '1', { '4', '5' } },
			{ '2', { '6' } },
			{ '3' }
		}
	};

	BOOST_CHECK(p.m_children.size() == 3);
	BOOST_CHECK(p.m_children[0].m_children.size() == 2);
	BOOST_CHECK(p.m_children[1].m_children.size() == 1);
	BOOST_CHECK(p.m_children[2].m_children.empty());

	BOOST_CHECK(p.m_value == '0');
}

// ----------------------------------------------------------------------------
// tree_node_iterator
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(tree_node_iterator__construct__1)
{
	using namespace wordring;

	tree<int> t;
}


// ----------------------------------------------------------------------------
// tree
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(tree__construct__1)
{
	using namespace wordring;

	tree<int> t;
}

// 変更 -----------------------------------------------------------------------


BOOST_AUTO_TEST_CASE(tree__clear__1)
{
	using namespace wordring;

	test_tree t;
}

/*
根を挿入する

iterator insert(const_iterator pos, value_type&& value)
*/
BOOST_AUTO_TEST_CASE(tree__insert__1)
{
	using namespace wordring;

	test_tree t;
	test_iterator it = t.insert(t.begin(), 100);

	BOOST_CHECK(it.m_parent == 0);
	BOOST_CHECK(it.m_index == 1);

	BOOST_CHECK(t.size() == 1);

	BOOST_CHECK(t.m_c[0].m_child == 1);

	BOOST_CHECK(t.m_c[1].m_prev == 1);
	BOOST_CHECK(t.m_c[1].m_next == 0);
}

/*
ノードの間に挿入する
*/
BOOST_AUTO_TEST_CASE(tree__insert__2)
{
	using namespace wordring;

	test_tree t;
	t.m_c.assign(4, { 0, 0, 0, 0 });
	t.m_c[0] = { 3, 0, 0, 1 };
	t.m_c[1] = { 0, 0, 0, 2 };
	t.m_c[2] = { 1, 3, 3, 0 };
	t.m_c[3] = { 1, 2, 0, 0 };

	test_iterator it = t.insert(const_test_iterator(t.m_c, 1, 3), 100);

	BOOST_CHECK(it.m_parent == 1);
	BOOST_CHECK(it.m_index == 4);

	BOOST_CHECK(t.size() == 4);

	BOOST_CHECK(t.m_c[0].m_child == 1);

	BOOST_CHECK(t.m_c[1].m_child == 2);

	BOOST_CHECK(t.m_c[2].m_prev == 3);
	BOOST_CHECK(t.m_c[2].m_next == 4);

	BOOST_CHECK(t.m_c[4].m_prev == 2);
	BOOST_CHECK(t.m_c[4].m_next == 3);
	
	BOOST_CHECK(t.m_c[3].m_prev == 4);
	BOOST_CHECK(t.m_c[3].m_next == 0);
}

/*
子が無い場合の挿入
*/
BOOST_AUTO_TEST_CASE(tree__insert__3)
{
	using namespace wordring;

	test_tree t;
	t.m_c.assign(2, { 0, 0, 0, 0 });
	t.m_c[0] = { 1, 0, 0, 1 };
	t.m_c[1] = { 0, 0, 0, 0 };

	test_iterator it = t.insert(const_test_iterator(t.m_c, 1, 0), 100);

	BOOST_CHECK(it.m_parent == 1);
	BOOST_CHECK(it.m_index == 2);

	BOOST_CHECK(t.size() == 2);

	BOOST_CHECK(t.m_c[0].m_child == 1);

	BOOST_CHECK(t.m_c[1].m_child == 2);

	BOOST_CHECK(t.m_c[2].m_prev == 2);
	BOOST_CHECK(t.m_c[2].m_next == 0);
}

/*
先頭の前に挿入
*/
BOOST_AUTO_TEST_CASE(tree__insert__4)
{
	using namespace wordring;

	test_tree t;
	t.m_c.assign(4, { 0, 0, 0, 0 });
	t.m_c[0] = { 3, 0, 0, 1 };
	t.m_c[1] = { 0, 0, 0, 2 };
	t.m_c[2] = { 1, 3, 3, 0 };
	t.m_c[3] = { 1, 2, 0, 0 };

	test_iterator it = t.insert(const_test_iterator(t.m_c, 1, 2), 100);

	BOOST_CHECK(it.m_parent == 1);
	BOOST_CHECK(it.m_index == 4);

	BOOST_CHECK(t.size() == 4);

	BOOST_CHECK(t.m_c[0].m_child == 1);

	BOOST_CHECK(t.m_c[1].m_child == 4);

	BOOST_CHECK(t.m_c[4].m_prev == 3);
	BOOST_CHECK(t.m_c[4].m_next == 2);

	BOOST_CHECK(t.m_c[2].m_prev == 4);
	BOOST_CHECK(t.m_c[2].m_next == 3);

	BOOST_CHECK(t.m_c[3].m_prev == 2);
	BOOST_CHECK(t.m_c[3].m_next == 0);
}

/*
単独の子の前に挿入
*/
BOOST_AUTO_TEST_CASE(tree__insert__5)
{
	using namespace wordring;

	test_tree t;
	t.m_c.assign(3, { 0, 0, 0, 0 });
	t.m_c[0] = { 2, 0, 0, 1 };
	t.m_c[1] = { 0, 0, 0, 2 };
	t.m_c[2] = { 1, 2, 0, 0 };

	test_iterator it = t.insert(const_test_iterator(t.m_c, 1, 2), 100);

	BOOST_CHECK(it.m_parent == 1);
	BOOST_CHECK(it.m_index == 3);

	BOOST_CHECK(t.size() == 3);

	BOOST_CHECK(t.m_c[0].m_child == 1);

	BOOST_CHECK(t.m_c[1].m_child == 3);

	BOOST_CHECK(t.m_c[3].m_prev == 2);
	BOOST_CHECK(t.m_c[3].m_next == 2);

	BOOST_CHECK(t.m_c[2].m_prev == 3);
	BOOST_CHECK(t.m_c[2].m_next == 0);
}

/*
終端の前に挿入
*/
BOOST_AUTO_TEST_CASE(tree__insert__6)
{
	using namespace wordring;

	test_tree t;
	t.m_c.assign(4, { 0, 0, 0, 0 });
	t.m_c[0] = { 3, 0, 0, 1 };
	t.m_c[1] = { 0, 0, 0, 2 };
	t.m_c[2] = { 1, 3, 3, 0 };
	t.m_c[3] = { 1, 2, 0, 0 };

	test_iterator it = t.insert(const_test_iterator(t.m_c, 1, 0), 100);

	BOOST_CHECK(it.m_parent == 1);
	BOOST_CHECK(it.m_index == 4);

	BOOST_CHECK(t.size() == 4);

	BOOST_CHECK(t.m_c[0].m_child == 1);

	BOOST_CHECK(t.m_c[1].m_child == 2);

	BOOST_CHECK(t.m_c[2].m_prev == 4);
	BOOST_CHECK(t.m_c[2].m_next == 3);

	BOOST_CHECK(t.m_c[3].m_prev == 2);
	BOOST_CHECK(t.m_c[3].m_next == 4);

	BOOST_CHECK(t.m_c[4].m_prev == 3);
	BOOST_CHECK(t.m_c[4].m_next == 0);
}

/*
単独の子の後に挿入
*/
BOOST_AUTO_TEST_CASE(tree__insert__7)
{
	using namespace wordring;

	test_tree t;
	t.m_c.assign(3, { 0, 0, 0, 0 });
	t.m_c[0] = { 2, 0, 0, 1 };
	t.m_c[1] = { 0, 0, 0, 2 };
	t.m_c[2] = { 1, 2, 0, 0 };

	test_iterator it = t.insert(const_test_iterator(t.m_c, 1, 0), 100);

	BOOST_CHECK(it.m_parent == 1);
	BOOST_CHECK(it.m_index == 3);

	BOOST_CHECK(t.size() == 3);

	BOOST_CHECK(t.m_c[0].m_child == 1);

	BOOST_CHECK(t.m_c[1].m_child == 2);

	BOOST_CHECK(t.m_c[2].m_prev == 3);
	BOOST_CHECK(t.m_c[2].m_next == 3);

	BOOST_CHECK(t.m_c[3].m_prev == 2);
	BOOST_CHECK(t.m_c[3].m_next == 0);
}

/*
根を削除

iterator erase(const_iterator pos)
*/
BOOST_AUTO_TEST_CASE(tree__erase__1)
{
	using namespace wordring;

	test_tree t;
	t.m_c.assign(2, { 0, 0, 0, 0 });
	t.m_c[0] = { 1, 0, 0, 1 };
	t.m_c[1] = { 0, 1, 0, 0 };

	test_iterator it = t.erase(const_test_iterator(t.m_c, 0, 1));

	BOOST_CHECK(it.m_parent == 0);
	BOOST_CHECK(it.m_index == 0);

	BOOST_CHECK(t.size() == 0);

	BOOST_CHECK(t.m_c[0].m_child == 0);
}

/*
ノードの間を削除
*/
BOOST_AUTO_TEST_CASE(tree__erase__2)
{
	using namespace wordring;

	test_tree t;
	t.m_c.assign(5, { 0, 0, 0, 0 });
	t.m_c[0] = { 4, 0, 0, 1 };
	t.m_c[1] = { 0, 0, 0, 2 };
	t.m_c[2] = { 1, 3, 4, 0 };
	t.m_c[4] = { 1, 2, 3, 0 };
	t.m_c[3] = { 1, 4, 0, 0 };

	test_iterator it = t.erase(const_test_iterator(t.m_c, 1, 4));

	BOOST_CHECK(it.m_parent == 1);
	BOOST_CHECK(it.m_index == 3);

	BOOST_CHECK(t.size() == 3);

	BOOST_CHECK(t.m_c[0].m_child == 1);

	BOOST_CHECK(t.m_c[1].m_child == 2);

	BOOST_CHECK(t.m_c[2].m_prev == 3);
	BOOST_CHECK(t.m_c[2].m_next == 3);

	BOOST_CHECK(t.m_c[3].m_prev == 2);
	BOOST_CHECK(t.m_c[3].m_next == 0);
}

/*
先頭の前を削除
*/
BOOST_AUTO_TEST_CASE(tree__erase__3)
{
	using namespace wordring;

	test_tree t;
	t.m_c.assign(5, { 0, 0, 0, 0 });
	t.m_c[0] = { 4, 0, 0, 1 };
	t.m_c[1] = { 0, 0, 0, 4 };
	t.m_c[4] = { 1, 3, 2, 0 };
	t.m_c[2] = { 1, 4, 3, 0 };
	t.m_c[3] = { 1, 2, 0, 0 };

	test_iterator it = t.erase(const_test_iterator(t.m_c, 1, 4));

	BOOST_CHECK(it.m_parent == 1);
	BOOST_CHECK(it.m_index == 2);

	BOOST_CHECK(t.size() == 3);

	BOOST_CHECK(t.m_c[0].m_child == 1);

	BOOST_CHECK(t.m_c[1].m_child == 2);

	BOOST_CHECK(t.m_c[2].m_prev == 3);
	BOOST_CHECK(t.m_c[2].m_next == 3);

	BOOST_CHECK(t.m_c[3].m_prev == 2);
	BOOST_CHECK(t.m_c[3].m_next == 0);
}

/*
削除で単独になる子の前を削除
*/
BOOST_AUTO_TEST_CASE(tree__erase__4)
{
	using namespace wordring;

	test_tree t;
	t.m_c.assign(4, { 0, 0, 0, 0 });
	t.m_c[0] = { 3, 0, 0, 1 };
	t.m_c[1] = { 0, 0, 0, 3 };
	t.m_c[3] = { 1, 2, 2, 0 };
	t.m_c[2] = { 1, 3, 0, 0 };

	test_iterator it = t.erase(const_test_iterator(t.m_c, 1, 3));

	BOOST_CHECK(it.m_parent == 1);
	BOOST_CHECK(it.m_index == 2);

	BOOST_CHECK(t.size() == 2);

	BOOST_CHECK(t.m_c[0].m_child == 1);

	BOOST_CHECK(t.m_c[1].m_child == 2);

	BOOST_CHECK(t.m_c[2].m_prev == 2);
	BOOST_CHECK(t.m_c[2].m_next == 0);
}


/*
終端の前を削除
*/
BOOST_AUTO_TEST_CASE(tree__erase__5)
{
	using namespace wordring;

	test_tree t;
	t.m_c.assign(5, { 0, 0, 0, 0 });
	t.m_c[0] = { 4, 0, 0, 1 };
	t.m_c[1] = { 0, 0, 0, 2 };
	t.m_c[2] = { 1, 4, 3, 0 };
	t.m_c[3] = { 1, 2, 4, 0 };
	t.m_c[4] = { 1, 3, 0, 0 };

	test_iterator it = t.erase(const_test_iterator(t.m_c, 1, 4));

	BOOST_CHECK(it.m_parent == 1);
	BOOST_CHECK(it.m_index == 0);

	BOOST_CHECK(t.size() == 3);

	BOOST_CHECK(t.m_c[0].m_child == 1);

	BOOST_CHECK(t.m_c[1].m_child == 2);

	BOOST_CHECK(t.m_c[2].m_prev == 3);
	BOOST_CHECK(t.m_c[2].m_next == 3);

	BOOST_CHECK(t.m_c[3].m_prev == 2);
	BOOST_CHECK(t.m_c[3].m_next == 0);
}

/*
削除で単独になる子の終端の前を削除
*/
BOOST_AUTO_TEST_CASE(tree__erase__6)
{
	using namespace wordring;

	test_tree t;
	t.m_c.assign(4, { 0, 0, 0, 0 });
	t.m_c[0] = { 3, 0, 0, 1 };
	t.m_c[1] = { 0, 0, 0, 2 };
	t.m_c[2] = { 1, 3, 3, 0 };
	t.m_c[3] = { 1, 2, 0, 0 };

	test_iterator it = t.erase(const_test_iterator(t.m_c, 1, 3));

	BOOST_CHECK(it.m_parent == 1);
	BOOST_CHECK(it.m_index == 0);

	BOOST_CHECK(t.size() == 2);

	BOOST_CHECK(t.m_c[0].m_child == 1);

	BOOST_CHECK(t.m_c[1].m_child == 2);

	BOOST_CHECK(t.m_c[2].m_prev == 2);
	BOOST_CHECK(t.m_c[2].m_next == 0);
}

/*
子孫削除
*/
BOOST_AUTO_TEST_CASE(tree__erase__7)
{
	using namespace wordring;

	test_tree t;
	t.m_c.assign(6, { 0, 0, 0, 0 });
	t.m_c[0] = { 5, 0, 0, 1 };
	t.m_c[1] = { 0, 0, 0, 2 };
	t.m_c[2] = { 1, 3, 4, 0 };
	t.m_c[4] = { 1, 2, 3, 5 };
	t.m_c[3] = { 1, 4, 0, 0 };
	t.m_c[5] = { 4, 0, 0, 0 };

	test_iterator it = t.erase(const_test_iterator(t.m_c, 0, 1));

	BOOST_CHECK(it.m_parent == 0);
	BOOST_CHECK(it.m_index == 0);

	BOOST_CHECK(t.size() == 0);
}

// 内部 -----------------------------------------------------------------------

/*
index_type allocate()

未使用ノードが無い状態から、ノードを確保する
*/
BOOST_AUTO_TEST_CASE(tree__allocate__1)
{
	using namespace wordring;

	test_tree t;
	t.m_c.assign({ { 0, 0, 0, 0 } });

	auto idx = t.allocate();

	BOOST_CHECK(idx == 1);

	BOOST_CHECK(t.m_c[0].m_prev == 0);
	BOOST_CHECK(t.m_c[0].m_next == 0);

	BOOST_CHECK(t.m_c.size() == 2);
}

/*
[0]に関連するよう、ノードを確保する
*/
BOOST_AUTO_TEST_CASE(tree__allocate__2)
{
	using namespace wordring;

	test_tree t;
	t.m_c.assign({ { 0, 1, 1, 0 }, { 0, 0, 0, 0 } });

	auto idx = t.allocate();

	BOOST_CHECK(idx == 1);

	BOOST_CHECK(t.m_c[0].m_prev == 0);
	BOOST_CHECK(t.m_c[0].m_next == 0);

	BOOST_CHECK(t.m_c.size() == 2);
}

/*
後ろに未使用ノードが有る状態から、ノードを確保する
*/
BOOST_AUTO_TEST_CASE(tree__allocate__3)
{
	using namespace wordring;

	test_tree t;
	t.m_c.assign({ { 0, 3, 2, 0 }, { 0, 0, 0, 0 }, { 0, 0, 3, 0 }, { 0, 2, 0, 0 } });

	auto idx = t.allocate();

	BOOST_CHECK(idx == 2);

	BOOST_CHECK(t.m_c[0].m_prev == 3);
	BOOST_CHECK(t.m_c[0].m_next == 3);

	BOOST_CHECK(t.m_c[1].m_prev == 0);
	BOOST_CHECK(t.m_c[1].m_next == 0);

	BOOST_CHECK(t.m_c.size() == 4);
}

/*
void free(index_type idx)

未使用ノードが無い状態から、使用済みノードを開放する
*/
BOOST_AUTO_TEST_CASE(tree__free__1)
{
	using namespace wordring;

	test_tree t;
	t.m_c.assign({ { 0, 0, 0, 0 }, { 0, 0, 0, 0 } });

	t.free(1);

	BOOST_CHECK(t.m_c[0].m_prev == 1);
	BOOST_CHECK(t.m_c[0].m_next == 1);
}

/*
前に未使用ノードが有り、後ろに未使用ノードが無い状態から、使用済みノードを開放する
*/
BOOST_AUTO_TEST_CASE(tree__free__2)
{
	using namespace wordring;

	test_tree t;
	t.m_c.assign({ { 0, 1, 1, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } });

	t.free(2);

	BOOST_CHECK(t.m_c[0].m_prev == 2);
	BOOST_CHECK(t.m_c[0].m_next == 1);

	BOOST_CHECK(t.m_c[1].m_prev == 0);
	BOOST_CHECK(t.m_c[1].m_next == 2);

	BOOST_CHECK(t.m_c[2].m_prev == 1);
	BOOST_CHECK(t.m_c[2].m_next == 0);
}

/*
前に未使用ノードが無く、後ろに未使用ノードが有る状態から、使用済みノードを開放する
*/
BOOST_AUTO_TEST_CASE(tree__free__3)
{
	using namespace wordring;

	test_tree t;
	t.m_c.assign({ { 0, 2, 2, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } });

	t.free(1);

	BOOST_CHECK(t.m_c[0].m_prev == 2);
	BOOST_CHECK(t.m_c[0].m_next == 1);

	BOOST_CHECK(t.m_c[1].m_prev == 0);
	BOOST_CHECK(t.m_c[1].m_next == 2);

	BOOST_CHECK(t.m_c[2].m_prev == 1);
	BOOST_CHECK(t.m_c[2].m_next == 0);
}

/*
前後に未使用ノードが有る状態から、使用済みノードを開放する
*/
BOOST_AUTO_TEST_CASE(tree__free__4)
{
	using namespace wordring;

	test_tree t;
	t.m_c.assign({ { 0, 3, 1, 0 }, { 0, 0, 3, 0 }, { 0, 0, 0, 0 }, { 0, 1, 0, 0 } });

	t.free(2);

	BOOST_CHECK(t.m_c[0].m_prev == 3);
	BOOST_CHECK(t.m_c[0].m_next == 1);

	BOOST_CHECK(t.m_c[1].m_prev == 0);
	BOOST_CHECK(t.m_c[1].m_next == 2);

	BOOST_CHECK(t.m_c[2].m_prev == 1);
	BOOST_CHECK(t.m_c[2].m_next == 3);

	BOOST_CHECK(t.m_c[3].m_prev == 2);
	BOOST_CHECK(t.m_c[3].m_next == 0);
}


BOOST_AUTO_TEST_SUITE_END()
