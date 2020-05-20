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

		test_iterator(typename test_tree::container& c, index_type parent, index_type idx)
			: base_type(c, parent, idx)
		{
		}
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

BOOST_AUTO_TEST_SUITE(tree_test)

// ----------------------------------------------------------------------------
// tree_node
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(tree_node_construct_1)
{
	using namespace wordring::detail;

	static_assert(std::is_pod_v<tree_node<int>>);

	tree_node<int> n{};

	BOOST_CHECK(n.m_value == 0);
}

// ----------------------------------------------------------------------------
// tree_node_iterator
// ----------------------------------------------------------------------------

/*
空のイテレータを作成する

tree_node_iterator() noexcept
*/
BOOST_AUTO_TEST_CASE(tree_node_iterator_construct_1)
{
	using namespace wordring::detail;

	tree_node_iterator<test_tree::container> it;
}

BOOST_AUTO_TEST_CASE(tree_node_iterator_construct_2)
{
	using namespace wordring::detail;

	test_tree::container v;
	auto it = test_iterator(v, 0, 0);
}

/*
const_iterator を取得する

operator tree_node_iterator<container const>() const
*/
BOOST_AUTO_TEST_CASE(tree_node_iterator_cast_1)
{
	using namespace wordring;
	using namespace wordring::detail;

	test_tree::container v;
	auto it = test_iterator(v, 0, 0);
	tree_node_iterator<test_tree::container> it1 = it;
	tree_node_iterator<test_tree::container const> it2 = it;

	BOOST_CHECK(it1 == it2);
}

/*
イテレータが指す要素の参照を取得する

auto& operator*() const
*/
BOOST_AUTO_TEST_CASE(tree_node_iterator_reference_1)
{
	test_tree t;
	t.insert(t.end(), 100);

	auto it1 = t.begin();
	BOOST_CHECK(*it1 == 100);
	*it1 = 90;
	BOOST_CHECK(*it1 == 90);

	// const_iterator の返す参照の値が const 修飾されているか確認
	static_assert(std::is_const_v<std::remove_reference_t<decltype(*t.cbegin())>>);
}

/*
イテレータが指す要素のポインタを取得する

auto operator->() const
*/
BOOST_AUTO_TEST_CASE(tree_node_iterator_reference_2)
{
	using namespace wordring;

	tree<std::string> t;
	t.insert(t.end(), "100");

	auto it1 = t.begin();
	BOOST_CHECK(it1->at(0) == '1');
	it1->at(0) = '2';
	BOOST_CHECK(it1->at(0) == '2');

	// const_iterator の返すポインタの値が const 修飾されているか確認
	static_assert(std::is_const_v<std::remove_reference_t<decltype(t.cbegin()->at(0))>>);
}

/*
インクリメントする

tree_node_iterator& operator++()
tree_node_iterator operator++(int)
*/
BOOST_AUTO_TEST_CASE(tree_node_iterator_increment_1)
{
	test_tree t;
	auto it1 = t.insert(t.begin(), 1);
	auto it2 = t.insert(it1.end(), 2);
	auto it3 = t.insert(it1.end(), 3);
	auto it4 = t.insert(it1.end(), 4);

	BOOST_CHECK(*it1 == 1);
	BOOST_CHECK(++it1 == t.end());

	BOOST_CHECK(*it2++ == 2);
	BOOST_CHECK(it2++ == it3);
	BOOST_CHECK(it2++ == it4);
	BOOST_CHECK(it2 == t.begin().end());
}

/*
デクリメントする

tree_node_iterator& operator--()
tree_node_iterator operator--(int)
*/
BOOST_AUTO_TEST_CASE(tree_node_iterator_decrement_1)
{
	test_tree t;
	auto it1 = t.insert(t.begin(), 1);
	t.insert(it1.end(), 2);
	t.insert(it1.end(), 3);
	t.insert(it1.end(), 4);

	BOOST_CHECK(*it1 == 1);
	BOOST_CHECK(--t.end() == t.begin());

	std::vector<int> v;
	auto it5 = std::make_reverse_iterator(it1.end());
	auto it6 = std::make_reverse_iterator(it1.begin());
	std::copy(it5, it6, std::back_inserter(v));

	BOOST_CHECK(v == std::vector<int>({ 4, 3, 2 }));
}

/*
子の先頭を指すイテレータを取得する
子の終端を指すイテレータを取得する

tree_node_iterator begin() const
tree_node_iterator end() const
*/
BOOST_AUTO_TEST_CASE(tree_node_iterator_begin_1)
{
	test_tree t;
	auto it1 = t.insert(t.begin(), 1);
	t.insert(it1.end(), 2);
	t.insert(it1.end(), 3);
	t.insert(it1.end(), 4);

	BOOST_CHECK(*it1 == 1);
	BOOST_CHECK(--t.end() == t.begin());

	std::vector<int> v;
	auto it5 = std::make_reverse_iterator(it1.end());
	auto it6 = std::make_reverse_iterator(it1.begin());
	std::copy(it5, it6, std::back_inserter(v));

	BOOST_CHECK(v == std::vector<int>({ 4, 3, 2 }));
}

/*
子の末尾を指す逆走イテレータを取得する
子の先頭の前を指す逆走イテレータを取得する

reverse_iterator rbegin() const
reverse_iterator rend() const
*/
BOOST_AUTO_TEST_CASE(tree_node_iterator_rbegin_1)
{
	test_tree t;
	auto it1 = t.insert(t.begin(), 1);
	t.insert(it1.end(), 2);
	t.insert(it1.end(), 3);
	t.insert(it1.end(), 4);

	BOOST_CHECK(*it1 == 1);
	BOOST_CHECK(--t.end() == t.begin());

	std::vector<int> v;
	auto it5 = it1.rbegin();
	auto it6 = it1.rend();
	std::copy(it5, it6, std::back_inserter(v));

	BOOST_CHECK(v == std::vector<int>({ 4, 3, 2 }));
}

/*
親を指すイテレータを取得する

tree_node_iterator parent() const
*/
BOOST_AUTO_TEST_CASE(tree_node_iterator_parent_1)
{
	test_tree t;
	auto it1 = t.insert(t.begin(), 1);
	auto it2 = t.insert(it1.end(), 2);

	BOOST_CHECK(it2.parent() == it1);
}

BOOST_AUTO_TEST_CASE(tree_node_iterator_parent_2)
{
	// 根の親を取得する。
	test_tree t;
	auto it1 = t.insert(t.begin(), 1);

	BOOST_CHECK(it1.parent() == decltype(it1)());
}

// ----------------------------------------------------------------------------
// tree
// ----------------------------------------------------------------------------

/*
空のコンテナを構築する

tree()
*/
BOOST_AUTO_TEST_CASE(tree_construct_1)
{
	using namespace wordring;

	tree<int> t;
	BOOST_CHECK(t.size() == 0);
}

/*
アロケータを指定して空のコンテナを構築する

explicit tree(allocator_type const& alloc)
*/
BOOST_AUTO_TEST_CASE(tree_construct_2)
{
	using namespace wordring;

	auto t = tree<int>(std::allocator<int>());

	BOOST_CHECK(t.size() == 0);
}

/*
要素からコンテナを構築する

explicit tree(value_type const& value, allocator_type const& alloc = allocator_type())
*/
BOOST_AUTO_TEST_CASE(tree_construct_3)
{
	using namespace wordring;

	int i = 100;
	auto t = tree<int>(i);

	BOOST_CHECK(t.size() == 1);
}

/*
要素からコンテナを構築する

explicit tree(value_type&& value, allocator_type const& alloc = allocator_type())
*/
BOOST_AUTO_TEST_CASE(tree_construct_4)
{
	using namespace wordring;

	tree<int> t(1);

	BOOST_CHECK(t.size() == 1);
}

// 変更 -----------------------------------------------------------------------


BOOST_AUTO_TEST_CASE(tree_clear_1)
{
	using namespace wordring;

	test_tree t;
}

/*
要素を挿入する

iterator insert(const_iterator pos, value_type const& value)
*/
BOOST_AUTO_TEST_CASE(tree_insert_1)
{
	using namespace wordring;

	test_tree t;
	int i = 100;
	test_iterator it = t.insert(t.begin(), i);

	BOOST_CHECK(it.m_parent == 0);
	BOOST_CHECK(it.m_index == 1);

	BOOST_CHECK(t.size() == 1);

	BOOST_CHECK(t.m_c[0].m_child == 1);

	BOOST_CHECK(t.m_c[1].m_prev == 1);
	BOOST_CHECK(t.m_c[1].m_next == 0);
}

/*
根を挿入する

iterator insert(const_iterator pos, value_type&& value)
*/
BOOST_AUTO_TEST_CASE(tree_insert_2)
{
	using namespace wordring;

	test_tree t;
	test_iterator it = t.insert(t.begin(), 100);

	BOOST_CHECK(it.m_parent == 0);
	BOOST_CHECK(it.m_index == 1);

	BOOST_CHECK(t.size() == 1);

	BOOST_CHECK(t.m_c[0].m_child == 1);

	BOOST_CHECK(t.m_c[1].m_parent == 0);
	BOOST_CHECK(t.m_c[1].m_prev == 1);
	BOOST_CHECK(t.m_c[1].m_next == 0);
	BOOST_CHECK(t.m_c[1].m_child == 0);
}

/*
ノードの間に挿入する
*/
BOOST_AUTO_TEST_CASE(tree_insert_3)
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

	BOOST_CHECK(t.m_c[1].m_parent == 0);
	BOOST_CHECK(t.m_c[1].m_child == 2);

	BOOST_CHECK(t.m_c[2].m_parent == 1);
	BOOST_CHECK(t.m_c[2].m_prev == 3);
	BOOST_CHECK(t.m_c[2].m_next == 4);
	BOOST_CHECK(t.m_c[2].m_child == 0);

	BOOST_CHECK(t.m_c[4].m_parent == 1);
	BOOST_CHECK(t.m_c[4].m_prev == 2);
	BOOST_CHECK(t.m_c[4].m_next == 3);
	BOOST_CHECK(t.m_c[4].m_child == 0);

	BOOST_CHECK(t.m_c[3].m_parent == 1);
	BOOST_CHECK(t.m_c[3].m_prev == 4);
	BOOST_CHECK(t.m_c[3].m_next == 0);
	BOOST_CHECK(t.m_c[3].m_child == 0);
}

/*
子が無い場合の挿入
*/
BOOST_AUTO_TEST_CASE(tree_insert_4)
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

	BOOST_CHECK(t.m_c[1].m_parent == 0);
	BOOST_CHECK(t.m_c[1].m_child == 2);

	BOOST_CHECK(t.m_c[2].m_parent == 1);
	BOOST_CHECK(t.m_c[2].m_prev == 2);
	BOOST_CHECK(t.m_c[2].m_next == 0);
	BOOST_CHECK(t.m_c[2].m_child == 0);
}

/*
先頭の前に挿入
*/
BOOST_AUTO_TEST_CASE(tree_insert_5)
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

	BOOST_CHECK(t.m_c[1].m_parent == 0);
	BOOST_CHECK(t.m_c[1].m_child == 4);

	BOOST_CHECK(t.m_c[4].m_parent == 1);
	BOOST_CHECK(t.m_c[4].m_prev == 3);
	BOOST_CHECK(t.m_c[4].m_next == 2);
	BOOST_CHECK(t.m_c[4].m_child == 0);

	BOOST_CHECK(t.m_c[2].m_parent == 1);
	BOOST_CHECK(t.m_c[2].m_prev == 4);
	BOOST_CHECK(t.m_c[2].m_next == 3);
	BOOST_CHECK(t.m_c[2].m_child == 0);

	BOOST_CHECK(t.m_c[3].m_parent == 1);
	BOOST_CHECK(t.m_c[3].m_prev == 2);
	BOOST_CHECK(t.m_c[3].m_next == 0);
	BOOST_CHECK(t.m_c[3].m_child == 0);
}

/*
単独の子の前に挿入
*/
BOOST_AUTO_TEST_CASE(tree_insert_6)
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

	BOOST_CHECK(t.m_c[1].m_parent == 0);
	BOOST_CHECK(t.m_c[1].m_child == 3);

	BOOST_CHECK(t.m_c[3].m_parent == 1);
	BOOST_CHECK(t.m_c[3].m_prev == 2);
	BOOST_CHECK(t.m_c[3].m_next == 2);
	BOOST_CHECK(t.m_c[3].m_child == 0);

	BOOST_CHECK(t.m_c[2].m_parent == 1);
	BOOST_CHECK(t.m_c[2].m_prev == 3);
	BOOST_CHECK(t.m_c[2].m_next == 0);
	BOOST_CHECK(t.m_c[2].m_child == 0);
}

/*
終端の前に挿入
*/
BOOST_AUTO_TEST_CASE(tree_insert_7)
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

	BOOST_CHECK(t.m_c[1].m_parent == 0);
	BOOST_CHECK(t.m_c[1].m_child == 2);

	BOOST_CHECK(t.m_c[2].m_parent == 1);
	BOOST_CHECK(t.m_c[2].m_prev == 4);
	BOOST_CHECK(t.m_c[2].m_next == 3);
	BOOST_CHECK(t.m_c[2].m_child == 0);

	BOOST_CHECK(t.m_c[3].m_parent == 1);
	BOOST_CHECK(t.m_c[3].m_prev == 2);
	BOOST_CHECK(t.m_c[3].m_next == 4);
	BOOST_CHECK(t.m_c[3].m_child == 0);

	BOOST_CHECK(t.m_c[4].m_parent == 1);
	BOOST_CHECK(t.m_c[4].m_prev == 3);
	BOOST_CHECK(t.m_c[4].m_next == 0);
	BOOST_CHECK(t.m_c[4].m_child == 0);
}

/*
単独の子の後に挿入
*/
BOOST_AUTO_TEST_CASE(tree_insert_8)
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

	BOOST_CHECK(t.m_c[1].m_parent == 0);
	BOOST_CHECK(t.m_c[1].m_child == 2);

	BOOST_CHECK(t.m_c[2].m_parent == 1);
	BOOST_CHECK(t.m_c[2].m_prev == 3);
	BOOST_CHECK(t.m_c[2].m_next == 3);
	BOOST_CHECK(t.m_c[2].m_child == 0);

	BOOST_CHECK(t.m_c[3].m_parent == 1);
	BOOST_CHECK(t.m_c[3].m_prev == 2);
	BOOST_CHECK(t.m_c[3].m_next == 0);
	BOOST_CHECK(t.m_c[3].m_child == 0);
}

/*
部分木を挿入する

iterator insert(const_iterator pos, const_iterator sub)
*/
BOOST_AUTO_TEST_CASE(tree_insert_9)
{
	using namespace wordring;

	test_tree t1;
	auto it1 = t1.insert(t1.begin(), 1);
	auto it2 = t1.insert(it1.end(), 2);
	t1.insert(it2.end(), 4);
	t1.insert(it2.end(), 5);
	auto it3 = t1.insert(it1.end(), 3);
	t1.insert(it3.end(), 6);
	t1.insert(it3.end(), 7);

	test_tree t2;
	t2.insert(t2.cend(), t1.cbegin());

	BOOST_CHECK(t2.size() == t1.size());
	
	auto p1 = t2.begin();
	BOOST_CHECK(*p1 == 1);

	auto p2 = p1.begin();
	BOOST_CHECK(*p2 == 2);
	auto p3 = ++p1.begin();
	BOOST_CHECK(*p3 == 3);

	auto p4 = p2.begin();
	BOOST_CHECK(*p4 == 4);
	auto p5 = ++p2.begin();
	BOOST_CHECK(*p5 == 5);

	auto p6 = p3.begin();
	BOOST_CHECK(*p6 == 6);
	auto p7 = ++p3.begin();
	BOOST_CHECK(*p7 == 7);
}

/*
部分木を移動する

iterator move(const_iterator pos, const_iterator sub)
*/
BOOST_AUTO_TEST_CASE(tree_move_1)
{
	// 祖先を子孫へ移動。
	using namespace wordring;

	test_tree t;
	auto it1 = t.insert(t.end(), 1);
	auto it2 = t.insert(it1.end(), 2);
	auto it3 = t.insert(it2.end(), 3);

	BOOST_CHECK_THROW(t.move(it3, it1), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(tree_move_2)
{
	// 移動元と移動先が同じ
	using namespace wordring;

	test_tree t;
	auto it1 = t.insert(t.end(), 1);
	auto it2 = t.insert(it1.end(), 2);
	t.insert(it2.end(), 3);

	BOOST_CHECK_THROW(t.move(it1, it1), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(tree_move_3)
{
	using namespace wordring;

	test_tree t;
	auto it1 = t.insert(t.end(), 1);
	auto it2 = t.insert(it1.end(), 2);
	auto it3 = t.insert(it2.end(), 3);

	t.move(it1.begin(), it3);

	BOOST_CHECK(*it1 == 1);
	BOOST_CHECK(*it1.begin() == 3);
	BOOST_CHECK(*++(it1.begin()) == 2);
}

BOOST_AUTO_TEST_CASE(tree_move_4)
{
	// 根に複数のノードを入れる
	using namespace wordring;

	test_tree t;
	auto it1 = t.insert(t.end(), 1);
	t.insert(it1.end(), 2);
	auto it3 = t.insert(it1.end(), 3);

	t.move(t.end(), it3);

	BOOST_CHECK(*it1 == 1);
	BOOST_CHECK(*std::next(it1) == 3);
	BOOST_CHECK(*it1.begin() == 2);
}

BOOST_AUTO_TEST_CASE(tree_move_5)
{
}

/*
根を削除

iterator erase(const_iterator pos)
*/
BOOST_AUTO_TEST_CASE(tree_erase_1)
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
BOOST_AUTO_TEST_CASE(tree_erase_2)
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
BOOST_AUTO_TEST_CASE(tree_erase_3)
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
BOOST_AUTO_TEST_CASE(tree_erase_4)
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
BOOST_AUTO_TEST_CASE(tree_erase_5)
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
BOOST_AUTO_TEST_CASE(tree_erase_6)
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
BOOST_AUTO_TEST_CASE(tree_erase_7)
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
BOOST_AUTO_TEST_CASE(tree_allocate_1)
{
	using namespace wordring;

	test_tree t;
	t.m_c.assign({ { 0, 0, 0, 0 } });

	auto idx = t.allocate(100);

	BOOST_CHECK(idx == 1);

	BOOST_CHECK(t.m_c[0].m_prev == 0);
	BOOST_CHECK(t.m_c[0].m_next == 0);

	BOOST_CHECK(t.m_c.size() == 2);
}

/*
[0]に関連するよう、ノードを確保する
*/
BOOST_AUTO_TEST_CASE(tree_allocate_2)
{
	using namespace wordring;

	test_tree t;
	t.m_c.assign({ { 0, 1, 1, 0 }, { 0, 0, 0, 0 } });

	auto idx = t.allocate(100);

	BOOST_CHECK(idx == 1);

	BOOST_CHECK(t.m_c[0].m_prev == 0);
	BOOST_CHECK(t.m_c[0].m_next == 0);

	BOOST_CHECK(t.m_c.size() == 2);
}

/*
後ろに未使用ノードが有る状態から、ノードを確保する
*/
BOOST_AUTO_TEST_CASE(tree_allocate_3)
{
	using namespace wordring;

	test_tree t;
	t.m_c.assign({ { 0, 3, 2, 0 }, { 0, 0, 0, 0 }, { 0, 0, 3, 0 }, { 0, 2, 0, 0 } });

	auto idx = t.allocate(100);

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
BOOST_AUTO_TEST_CASE(tree_free_1)
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
BOOST_AUTO_TEST_CASE(tree_free_2)
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
BOOST_AUTO_TEST_CASE(tree_free_3)
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
BOOST_AUTO_TEST_CASE(tree_free_4)
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
