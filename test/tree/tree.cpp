// test/tree/tree.cpp

#include <boost/test/unit_test.hpp>

#include <wordring/tree/tree.hpp>

#include <algorithm>
#include <iterator>
#include <list>
#include <string>
#include <type_traits>
#include <vector>

BOOST_AUTO_TEST_SUITE(tree__test)

using tree_node = wordring::tree_node<char>;

class test_iterator : public wordring::tree<char>::iterator
{
public:
	using wordring::tree<char>::iterator::index_type;
	using wordring::tree<char>::iterator::tree_type;
	using wordring::tree<char>::iterator::iterator;
	using wordring::tree<char>::iterator::m_tree;
	using wordring::tree<char>::iterator::m_parent;
	using wordring::tree<char>::iterator::m_index;

	test_iterator(wordring::tree<char>::iterator&& it) : wordring::tree<char>::iterator(it) {}

	test_iterator(tree_type& tree, index_type parent, index_type index)
		: wordring::tree<char>::iterator(tree, parent, index)
	{
	}

	index_type i_parent() const { return (heap().data() + m_index)->m_parent; }

	index_type i_child() const { return (heap().data() + m_index)->m_child; }

	index_type i_prev() const { return (heap().data() + m_index)->m_prev; }

	index_type i_next() const { return (heap().data() + m_index)->m_next; }
};

using const_test_iterator = wordring::tree<char>::const_iterator;

class test_tree : public wordring::tree<char>
{
public:
	using wordring::tree<char>::null_value;

	using wordring::tree<char>::index_type;
	using wordring::tree<char>::container;

	using wordring::tree<char>::allocate;
	using wordring::tree<char>::free;
	using wordring::tree<char>::recursive_not_equal;

	using wordring::tree<char>::m_heap;
	using wordring::tree<char>::m_root;
	using wordring::tree<char>::m_free;

	test_iterator at(index_type i) { return test_iterator(*this, (m_heap.data() + i)->m_parent, i); }

	auto& heap() { return m_heap; }
};

static constexpr test_tree::index_type null_value = test_tree::null_value;

using container = test_tree::container;

/*
container make_test_container()
{
	container h{};
	//             parent      child       prev        next        value
	h.emplace_back(null_value, 1,          null_value, null_value, '0'); // 0
	h.emplace_back(0,          4,          3,          2,          '1'); // 1
	h.emplace_back(0,          6,          1,          3,          '2'); // 2
	h.emplace_back(0,          null_value, 2,          null_value, '3'); // 3
	h.emplace_back(1,          null_value, 5,          5,          '4'); // 4
	h.emplace_back(1,          null_value, 4,          null_value, '5'); // 5
	h.emplace_back(2,          null_value, 6,          null_value, '6'); // 6

	return h;
}
*/

test_tree make_test_tree()
{
	test_tree t{};

	auto it0 = t.insert(t.cbegin(), '0');
	auto it1 = t.insert(it0.end(), '1');
	auto it2 = t.insert(it0.end(), '2');
	t.insert(it0.end(), '3');
	t.insert(it1.end(), '4');
	t.insert(it1.end(), '5');
	t.insert(it2.end(), '6');

	return t;
}

// ----------------------------------------------------------------------------
// tree_node
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(tree_node__construct__1)
{
	tree_node n{};
	BOOST_CHECK(n.m_value == 0);
}

BOOST_AUTO_TEST_CASE(tree_node__construct__2)
{
	tree_node n(1, 2, 3, 4, 5);
	BOOST_CHECK(n.m_value == 5);
}

// ------------------------------------------------------------------------
// tree_node_proxy
// ------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(tree_node_proxy__construct__1)
{
	wordring::tree_node_proxy<char> p{ '0' };

	BOOST_CHECK(p.m_value == '0');
	BOOST_CHECK(p.m_children.empty());
}

BOOST_AUTO_TEST_CASE(tree_node_proxy__construct__2)
{
	wordring::tree_node_proxy<char> p{ '0', { '1', '2', '3' } };

	BOOST_CHECK(p.m_value == '0');
	BOOST_CHECK(p.m_children.size() == 3);
}

BOOST_AUTO_TEST_CASE(tree_node_proxy__construct__3)
{
	wordring::tree_node_proxy<char> p
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
// basic_tree_node_iterator
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(basic_tree_node_iterator__construct__1)
{
	test_tree t{};
	BOOST_CHECK(test_iterator(t, null_value, null_value) == test_iterator(t, null_value, null_value));
	BOOST_CHECK(const_test_iterator(t, null_value, null_value) == const_test_iterator(t, null_value, null_value));
}

BOOST_AUTO_TEST_CASE(basic_tree_node_iterator__assign__1)
{
	test_tree t{};
	test_iterator it1(t, null_value, null_value);
	test_iterator it2(t, null_value, null_value);
	const_test_iterator it3(t, null_value, null_value);
	const_test_iterator it4(t, null_value, null_value);

	it1 = it2;
	BOOST_CHECK(it1 == it2);
	it3 = it4;
	BOOST_CHECK(it3 == it4);
	// const_iteratorへiteratorを代入する。
	it3 = it1;
	BOOST_CHECK(it3 == it1);
}

BOOST_AUTO_TEST_CASE(basic_tree_node_iterator__reference__1)
{
	test_tree t{};
	auto& h = t.m_heap;
	//             parent      child       prev        next        value
	h.emplace_back(null_value, null_value, null_value, null_value, '0'); // 0
	test_iterator it1(t, null_value, 0);
	const_test_iterator it2(t, null_value, 0);

	BOOST_CHECK(*it1 == '0');
	*it1 = '1';
	BOOST_CHECK(*it2 == '1');
}

BOOST_AUTO_TEST_CASE(basic_tree_node_iterator__reference__2)
{
	wordring::tree<std::string> t{};
	t.insert(t.begin(), "ABCDE");

	BOOST_CHECK(t.begin()->back() == 'E');
	t.begin()->back() = '1';
	BOOST_CHECK(t.begin()->back() == '1');
}

BOOST_AUTO_TEST_CASE(basic_tree_node_iterator__increment__1)
{
	test_tree t = make_test_tree();
	std::string result;

	auto it1 = const_test_iterator(t, null_value, null_value);
	auto it2 = const_test_iterator(t, null_value, null_value);

	std::copy(it1, it2, std::back_inserter(result));
	BOOST_CHECK(result == "");
}

BOOST_AUTO_TEST_CASE(basic_tree_node_iterator__increment__2)
{
	test_tree t = make_test_tree();
	std::string result;

	const_test_iterator it1(t, null_value, 0);
	const_test_iterator it2(t, null_value, null_value);
	while (it1 != it2) result.push_back(*it1++);
	BOOST_CHECK(result == "0");
}

BOOST_AUTO_TEST_CASE(basic_tree_node_iterator__increment__3)
{
	test_tree t = make_test_tree();
	std::string result;

	const_test_iterator it1(t, 0, 1);
	const_test_iterator it2(t, 0, null_value);
	while (it1 != it2) result.push_back(*it1++);
	BOOST_CHECK(result == "123");
}

BOOST_AUTO_TEST_CASE(basic_tree_node_iterator__increment__4)
{
	test_tree t = make_test_tree();
	std::string result;

	const_test_iterator it1(t, 1, 4);
	const_test_iterator it2(t, 1, null_value);
	while (it1 != it2) result.push_back(*it1++);
	BOOST_CHECK(result == "45");
}

BOOST_AUTO_TEST_CASE(basic_tree_node_iterator__increment__5)
{
	test_tree t = make_test_tree();
	std::string result;

	const_test_iterator it1(t, 2, 6);
	const_test_iterator it2(t, 2, null_value);
	while (it1 != it2) result.push_back(*it1++);
	BOOST_CHECK(result == "6");
}

BOOST_AUTO_TEST_CASE(basic_tree_node_iterator__decrement__1)
{
	test_tree t = make_test_tree();
	std::string result;

	auto it1 = std::make_reverse_iterator(const_test_iterator(t, null_value, null_value));
	auto it2 = std::make_reverse_iterator(const_test_iterator(t, null_value, null_value));

	std::copy(it1, it2, std::back_inserter(result));
	BOOST_CHECK(result == "");
}

BOOST_AUTO_TEST_CASE(basic_tree_node_iterator__decrement__2)
{
	test_tree t = make_test_tree();
	std::string result;

	auto it1 = std::make_reverse_iterator(const_test_iterator(t, null_value, null_value));
	auto it2 = std::make_reverse_iterator(const_test_iterator(t, null_value, 0));

	std::copy(it1, it2, std::back_inserter(result));
	BOOST_CHECK(result == "0");
}

BOOST_AUTO_TEST_CASE(basic_tree_node_iterator__decrement__3)
{
	test_tree t = make_test_tree();
	std::string result;

	auto it1 = std::make_reverse_iterator(const_test_iterator(t, 0, null_value));
	auto it2 = std::make_reverse_iterator(const_test_iterator(t, 0, 1));

	std::copy(it1, it2, std::back_inserter(result));
	BOOST_CHECK(result == "321");
}

BOOST_AUTO_TEST_CASE(basic_tree_node_iterator__decrement__4)
{
	test_tree t = make_test_tree();
	std::string result;

	auto it1 = std::make_reverse_iterator(const_test_iterator(t, 1, null_value));
	auto it2 = std::make_reverse_iterator(const_test_iterator(t, 1, 4));

	std::copy(it1, it2, std::back_inserter(result));
	BOOST_CHECK(result == "54");
}

BOOST_AUTO_TEST_CASE(basic_tree_node_iterator__decrement__5)
{
	test_tree t = make_test_tree();
	std::string result;

	auto it1 = std::make_reverse_iterator(const_test_iterator(t, 2, null_value));
	auto it2 = std::make_reverse_iterator(const_test_iterator(t, 2, 6));

	std::copy(it1, it2, std::back_inserter(result));
	BOOST_CHECK(result == "6");
}

BOOST_AUTO_TEST_CASE(basic_tree_node_iterator__decrement__6)
{
	test_tree t = make_test_tree();

	auto it1 = std::make_reverse_iterator(const_test_iterator(t, null_value, null_value));
	auto it2 = std::make_reverse_iterator(const_test_iterator(t, null_value, 0));

	BOOST_CHECK(it1 == --it2);
}

// root
BOOST_AUTO_TEST_CASE(basic_tree_node_iterator__parent__1)
{
	test_tree t = make_test_tree();
	BOOST_CHECK(const_test_iterator(t, null_value, 0).parent().begin() == const_test_iterator(t, null_value, null_value));
}

BOOST_AUTO_TEST_CASE(basic_tree_node_iterator__parent__2)
{
	test_tree t = make_test_tree();
	BOOST_CHECK(*const_test_iterator(t, 0, 1).parent().begin() == '0');
}

BOOST_AUTO_TEST_CASE(basic_tree_node_iterator__parent__3)
{
	test_tree t = make_test_tree();
	BOOST_CHECK(*const_test_iterator(t, 0, 2).parent().begin() == '0');
}

BOOST_AUTO_TEST_CASE(basic_tree_node_iterator__parent__4)
{
	test_tree t = make_test_tree();
	BOOST_CHECK(*const_test_iterator(t, 0, 2).parent().begin() == '0');
}

BOOST_AUTO_TEST_CASE(basic_tree_node_iterator__parent__5)
{
	test_tree t = make_test_tree();
	BOOST_CHECK(*const_test_iterator(t, 1, 4).parent().begin() == '1');
}

BOOST_AUTO_TEST_CASE(basic_tree_node_iterator__parent__6)
{
	test_tree t = make_test_tree();
	BOOST_CHECK(*const_test_iterator(t, 1, 5).parent().begin() == '1');
}

BOOST_AUTO_TEST_CASE(basic_tree_node_iterator__parent__7)
{
	test_tree t = make_test_tree();
	BOOST_CHECK(*const_test_iterator(t, 2, 6).parent().begin() == '2');
}

// root
BOOST_AUTO_TEST_CASE(basic_tree_node_iterator__children__1)
{
	test_tree t = make_test_tree();
	std::string result;
	for(auto const c : const_test_iterator(t, null_value, 0)) result.push_back(c);
	BOOST_CHECK(result == "123");
}

BOOST_AUTO_TEST_CASE(basic_tree_node_iterator__children__2)
{
	test_tree t = make_test_tree();
	std::string result;
	for(auto const c : const_test_iterator(t, 0, 1)) result.push_back(c);
	BOOST_CHECK(result == "45");
}

BOOST_AUTO_TEST_CASE(basic_tree_node_iterator__children__3)
{
	test_tree t = make_test_tree();
	std::string result;
	for(auto const c : const_test_iterator(t, 0, 2)) result.push_back(c);
	BOOST_CHECK(result == "6");
}

BOOST_AUTO_TEST_CASE(basic_tree_node_iterator__children__4)
{
	test_tree t = make_test_tree();
	std::string result;
	for(auto const c : const_test_iterator(t, 0, 3)) result.push_back(c);
	BOOST_CHECK(result == "");
}

BOOST_AUTO_TEST_CASE(basic_tree_node_iterator__children__5)
{
	test_tree t = make_test_tree();
	std::string result;
	for(auto const c : const_test_iterator(t, 1, 4)) result.push_back(c);
	BOOST_CHECK(result == "");
}

BOOST_AUTO_TEST_CASE(basic_tree_node_iterator__children__6)
{
	test_tree t = make_test_tree();
	std::string result;
	for(auto const c : const_test_iterator(t, 1, 5)) result.push_back(c);
	BOOST_CHECK(result == "");
}

BOOST_AUTO_TEST_CASE(basic_tree_node_iterator__children__7)
{
	test_tree t = make_test_tree();
	std::string result;
	for(auto const c : const_test_iterator(t, 2, 6)) result.push_back(c);
	BOOST_CHECK(result == "");
}

BOOST_AUTO_TEST_CASE(basic_tree_node_iterator__equal__1)
{
	test_tree t = make_test_tree();
	BOOST_CHECK(test_iterator(t, 2, 6) == test_iterator(t, 2, 6));
	BOOST_CHECK(const_test_iterator(t, 2, 6) == const_test_iterator(t, 2, 6));
}

BOOST_AUTO_TEST_CASE(basic_tree_node_iterator__equal__2)
{
	test_tree t = make_test_tree();
	BOOST_CHECK(const_test_iterator(t, 2, 6) == test_iterator(t, 2, 6));
	BOOST_CHECK(test_iterator(t, 2, 6) == const_test_iterator(t, 2, 6));
}

BOOST_AUTO_TEST_CASE(basic_tree_node_iterator__equal__3)
{
	test_tree t = make_test_tree();
	BOOST_CHECK(test_iterator(t, null_value, null_value) == test_iterator(t, null_value, null_value));
	BOOST_CHECK(const_test_iterator(t, null_value, null_value) == const_test_iterator(t, null_value, null_value));
}

BOOST_AUTO_TEST_CASE(basic_tree_node_iterator__equal__4)
{
	test_tree t = make_test_tree();
	BOOST_CHECK((test_iterator(t, 0, 6) == test_iterator(t, 2, 6)) == false);
	BOOST_CHECK((test_iterator(t, 0, 6) == test_iterator(t, 0, 5)) == false);
}

BOOST_AUTO_TEST_CASE(basic_tree_node_iterator__not_equal__1)
{
	test_tree t = make_test_tree();
	BOOST_CHECK(test_iterator(t, 0, 6) != test_iterator(t, 2, 6));
	BOOST_CHECK(const_test_iterator(t, 0, 0) != const_test_iterator(t, 0, 1));
}

BOOST_AUTO_TEST_CASE(basic_tree_node_iterator__not_equal__2)
{
	test_tree t = make_test_tree();
	BOOST_CHECK(const_test_iterator(t, 0, 6) != test_iterator(t, 2, 6));
	BOOST_CHECK(test_iterator(t, 0, 6) != const_test_iterator(t, 2, 6));
}

BOOST_AUTO_TEST_CASE(basic_tree_node_iterator__not_equal__3)
{
	test_tree t = make_test_tree();
	BOOST_CHECK(test_iterator(t, 0, null_value) != test_iterator(t, null_value, null_value));
	BOOST_CHECK(const_test_iterator(t, 0, null_value) != const_test_iterator(t, null_value, null_value));
}

BOOST_AUTO_TEST_CASE(basic_tree_node_iterator__not_equal__4)
{
	test_tree t = make_test_tree();
	BOOST_CHECK((test_iterator(t, 0, 6) != test_iterator(t, 0, 6)) == false);
	BOOST_CHECK((test_iterator(t, 0, 6) != test_iterator(t, 0, 6)) == false);
}

// ----------------------------------------------------------------------------
// tree
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(tree__allocate__1)
{
	test_tree t{};
	BOOST_CHECK(t.allocate() == 0);
	BOOST_CHECK(t.allocate() == 1);
	BOOST_CHECK(t.allocate() == 2);
	BOOST_CHECK(t.m_heap.size() == 3);
	BOOST_CHECK(t.m_free == null_value);
}

BOOST_AUTO_TEST_CASE(tree__allocate__2)
{
	test_tree t{};
	BOOST_CHECK(t.allocate() == 0);
	BOOST_CHECK(t.allocate() == 1);
	BOOST_CHECK(t.allocate() == 2);
	BOOST_CHECK(t.m_heap.size() == 3);
	BOOST_CHECK(t.m_free == null_value);
}

BOOST_AUTO_TEST_CASE(tree__free__1)
{
	test_tree t{};
	BOOST_CHECK(t.allocate() == 0);
	t.free(0);
	BOOST_CHECK(t.allocate() == 0);
	t.free(0);
	BOOST_CHECK(t.allocate() == 0);
	BOOST_CHECK(t.allocate() == 1);
	t.free(1);
	BOOST_CHECK(t.allocate() == 1);
	BOOST_CHECK(t.allocate() == 2);
	BOOST_CHECK(t.allocate() == 3);
}

BOOST_AUTO_TEST_CASE(tree__free__2)
{
	test_tree t{};
	auto& h = t.m_heap;

	//             parent      child       prev        next
	h.emplace_back(null_value, 1,          null_value, null_value); // 0
	h.emplace_back(0,          null_value, null_value, 2         ); // 1
	h.emplace_back(0,          null_value, 1,          3         ); // 2
	h.emplace_back(0,          null_value, 2,          null_value); // 3

	t.free(0);

	BOOST_CHECK(t.allocate() == 0);
	BOOST_CHECK(t.allocate() == 1);
	BOOST_CHECK(t.allocate() == 2);
	BOOST_CHECK(t.allocate() == 3);
	BOOST_CHECK(t.m_free == null_value);
	BOOST_CHECK(t.allocate() == 4);
}

BOOST_AUTO_TEST_CASE(tree__free__3)
{
	test_tree t{};
	auto& h = t.m_heap;

	t.m_free = 4;
	//             parent      child       prev        next
	h.emplace_back(null_value, 1,          null_value, null_value); // 0
	h.emplace_back(0,          null_value, null_value, 2         ); // 1
	h.emplace_back(0,          null_value, 1,          3         ); // 2
	h.emplace_back(0,          null_value, 2,          null_value); // 3
	h.emplace_back(null_value, null_value, null_value, 5         ); // 4
	h.emplace_back(null_value, null_value, null_value, 6         ); // 5
	h.emplace_back(null_value, null_value, null_value, 7         ); // 6
	h.emplace_back(null_value, null_value, null_value, null_value); // 7

	t.free(0);

	BOOST_CHECK(t.allocate() == 0);
	BOOST_CHECK(t.allocate() == 1);
	BOOST_CHECK(t.allocate() == 2);
	BOOST_CHECK(t.allocate() == 3);
	BOOST_CHECK(t.allocate() == 4);
	BOOST_CHECK(t.allocate() == 5);
	BOOST_CHECK(t.allocate() == 6);
	BOOST_CHECK(t.allocate() == 7);
	BOOST_CHECK(t.m_free == null_value);
	BOOST_CHECK(t.allocate() == 8);
}

BOOST_AUTO_TEST_CASE(tree__free__4)
{
	test_tree t{};
	auto& h = t.m_heap;

	//             parent      child       prev        next
	h.emplace_back(null_value, 1,          null_value, null_value); // 0
	h.emplace_back(0,          4,          null_value, 2         ); // 1
	h.emplace_back(0,          6,          1,          3         ); // 2
	h.emplace_back(0,          null_value, 2,          null_value); // 3
	h.emplace_back(1,          null_value, null_value, 5         ); // 4
	h.emplace_back(1,          null_value, 4,          null_value); // 5
	h.emplace_back(2,          null_value, null_value, null_value); // 6

	t.free(0);

	BOOST_CHECK(t.allocate() == 0);
	BOOST_CHECK(t.allocate() == 1);
	BOOST_CHECK(t.allocate() == 2);
	BOOST_CHECK(t.allocate() == 3);
	BOOST_CHECK(t.allocate() == 6);
	BOOST_CHECK(t.allocate() == 4);
	BOOST_CHECK(t.allocate() == 5);
	BOOST_CHECK(t.m_free == null_value);
	BOOST_CHECK(t.allocate() == 7);
}

BOOST_AUTO_TEST_CASE(tree__free__5)
{
	test_tree t{};
	auto& h = t.m_heap;

	//             parent      child       prev        next
	h.emplace_back(null_value, 1,          null_value, null_value); // 0
	h.emplace_back(0,          4,          null_value, 2         ); // 1
	h.emplace_back(0,          6,          1,          3         ); // 2
	h.emplace_back(0,          null_value, 2,          null_value); // 3
	h.emplace_back(1,          null_value, null_value, 5         ); // 4
	h.emplace_back(1,          null_value, 4,          null_value); // 5
	h.emplace_back(2,          null_value, null_value, null_value); // 6

	t.free(1);

	BOOST_CHECK(t.allocate() == 1);
	BOOST_CHECK(t.allocate() == 4);
	BOOST_CHECK(t.allocate() == 5);
	BOOST_CHECK(t.m_free == null_value);
	BOOST_CHECK(t.allocate() == 7);
}

// 根のallocate
BOOST_AUTO_TEST_CASE(tree__allocate__3)
{
	test_tree t{};
	test_iterator it0(t.allocate(t.begin(), '0'));
	BOOST_CHECK(it0.i_parent() == null_value);
	BOOST_CHECK(it0.i_prev() == 0);
	BOOST_CHECK(*it0 == '0');
	BOOST_CHECK(t.size() == 1);
}

// 単独の子をallocate
BOOST_AUTO_TEST_CASE(tree__allocate__4)
{
	test_tree t{};
	// 根
	test_iterator it0 = t.allocate(t.begin(), '0');
	BOOST_CHECK(it0.i_child() == null_value);
	// 単独の子
	test_iterator it1 = t.allocate(it0.begin(), '1');
	BOOST_CHECK(it0.i_child() == 1);
	BOOST_CHECK(it1.i_parent() == 0);
	BOOST_CHECK(it1.i_prev() == 1);
	BOOST_CHECK(it1.i_next() == null_value);
	BOOST_CHECK(*it1 == '1');
	BOOST_CHECK(t.size() == 2);
}

// 最後の子をallocate
BOOST_AUTO_TEST_CASE(tree__allocate__5)
{
	test_tree t{};
	// 根
	test_iterator it0 = t.allocate(t.begin(), '0');
	// 単独の子
	test_iterator it2 = t.allocate(it0.begin(), '2');
	BOOST_CHECK(it0.i_child() == it2.m_index);
	BOOST_CHECK(it2.i_parent() == 0);
	BOOST_CHECK(it2.i_prev() == it2.m_index);
	BOOST_CHECK(it2.i_next() == null_value);
	// 最後の子
	test_iterator it3 = t.allocate(it0.end(), '3');
	BOOST_CHECK(it0.i_child() == it2.m_index);
	BOOST_CHECK(it2.i_prev() == it3.m_index);
	BOOST_CHECK(it2.i_next() == it3.m_index);
	BOOST_CHECK(it3.i_parent() == 0);
	BOOST_CHECK(it3.i_prev() == it2.m_index);
	BOOST_CHECK(it3.i_next() == null_value);

	std::string s("23");
	BOOST_CHECK(std::equal(s.begin(), s.end(), it0.begin(), it0.end()));
	BOOST_CHECK(t.size() == 3);
}

// 先頭の子をallocate
BOOST_AUTO_TEST_CASE(tree__allocate__6)
{
	test_tree t{};
	// 根
	test_iterator it0 = t.allocate(t.begin(), '0');
	// 単独の子
	test_iterator it2 = t.allocate(it0.begin(), '2');
	BOOST_CHECK(it0.i_child() == it2.m_index);
	// 最後の子
	test_iterator it3 = t.allocate(it0.end(), '3');
	BOOST_CHECK(it0.i_child() == it2.m_index);
	BOOST_CHECK(it3.i_parent() == 0);
	BOOST_CHECK(it3.i_prev() == it2.m_index);
	BOOST_CHECK(it3.i_next() == null_value);
	// 先頭の子
	test_iterator it1 = t.allocate(it2, '1');
	BOOST_CHECK(it0.i_child() == it1.m_index);
	BOOST_CHECK(it3.i_prev() == it2.m_index);
	BOOST_CHECK(it3.i_next() == null_value);
	BOOST_CHECK(it1.i_parent() == it0.m_index);
	BOOST_CHECK(it1.i_prev() == it3.m_index);
	BOOST_CHECK(it1.i_next() == it2.m_index);

	std::string s("123");
	BOOST_CHECK(std::equal(s.begin(), s.end(), it0.begin(), it0.end()));
	BOOST_CHECK(t.size() == 4);
}

// 中間の子をallocate
BOOST_AUTO_TEST_CASE(tree__allocate__7)
{
	test_tree t{};
	// 根
	test_iterator it0 = t.allocate(t.begin(), '0');
	// 単独の子
	test_iterator it2 = t.allocate(it0.begin(), '2');
	// 最後の子
	test_iterator it4 = t.allocate(it0.end(), '4');
	// 先頭の子
	test_iterator it1 = t.allocate(it2, '1');
	// 中間の子
	test_iterator it3 = t.allocate(it4, '3');

	BOOST_CHECK(it0.i_child() == it1.m_index);
	
	BOOST_CHECK(it1.i_next() == it2.m_index);
	BOOST_CHECK(it2.i_next() == it3.m_index);
	BOOST_CHECK(it3.i_next() == it4.m_index);
	BOOST_CHECK(it4.i_next() == null_value);

	BOOST_CHECK(it4.i_prev() == it3.m_index);
	BOOST_CHECK(it3.i_prev() == it2.m_index);
	BOOST_CHECK(it2.i_prev() == it1.m_index);
	BOOST_CHECK(it1.i_prev() == it4.m_index);

	BOOST_CHECK(it1.i_parent() == it0.m_index);
	BOOST_CHECK(it2.i_parent() == it0.m_index);
	BOOST_CHECK(it3.i_parent() == it0.m_index);
	BOOST_CHECK(it4.i_parent() == it0.m_index);

	std::string s("1234");
	BOOST_CHECK(std::equal(s.begin(), s.end(), it0.begin(), it0.end()));
	BOOST_CHECK(t.size() == 5);
}

// 最後の子をallocate -> 最後の子をallocate
BOOST_AUTO_TEST_CASE(tree__allocate__8)
{
	test_tree t{};
	// 根
	test_iterator it0 = t.allocate(t.end(), '0');
	// 単独の子
	test_iterator it1 = t.allocate(it0.end(), '1');
	// 最後の子
	test_iterator it2 = t.allocate(it0.end(), '2');
	// 最後の子
	test_iterator it3 = t.allocate(it0.end(), '3');

	BOOST_CHECK(it0.i_child() == it1.m_index);

	BOOST_CHECK(it1.i_next() == it2.m_index);
	BOOST_CHECK(it2.i_next() == it3.m_index);
	BOOST_CHECK(it3.i_next() == null_value);

	BOOST_CHECK(it3.i_prev() == it2.m_index);
	BOOST_CHECK(it2.i_prev() == it1.m_index);
	BOOST_CHECK(it1.i_prev() == it3.m_index);

	BOOST_CHECK(it1.i_parent() == it0.m_index);
	BOOST_CHECK(it2.i_parent() == it0.m_index);
	BOOST_CHECK(it3.i_parent() == it0.m_index);

	std::string s("123");
	BOOST_CHECK(std::equal(s.begin(), s.end(), it0.begin(), it0.end()));
	BOOST_CHECK(t.size() == 4);
}

// 最初の子をallocate -> 最初の子をallocate
BOOST_AUTO_TEST_CASE(tree__allocate__9)
{
	test_tree t{};
	// 根
	test_iterator it0 = t.allocate(t.end(), '0');
	// 単独の子
	test_iterator it3 = t.allocate(it0.begin(), '3');
	// 最初の子
	test_iterator it2 = t.allocate(it0.begin(), '2');
	// 最初の子
	test_iterator it1 = t.allocate(it0.begin(), '1');

	BOOST_CHECK(it0.i_child() == it1.m_index);

	BOOST_CHECK(it1.i_next() == it2.m_index);
	BOOST_CHECK(it2.i_next() == it3.m_index);
	BOOST_CHECK(it3.i_next() == null_value);

	BOOST_CHECK(it3.i_prev() == it2.m_index);
	BOOST_CHECK(it2.i_prev() == it1.m_index);
	BOOST_CHECK(it1.i_prev() == it3.m_index);

	BOOST_CHECK(it1.i_parent() == it0.m_index);
	BOOST_CHECK(it2.i_parent() == it0.m_index);
	BOOST_CHECK(it3.i_parent() == it0.m_index);

	std::string s("123");
	BOOST_CHECK(std::equal(s.begin(), s.end(), it0.begin(), it0.end()));
	BOOST_CHECK(t.size() == 4);
}

// 中間の子をallocate -> 中間の子をallocate
BOOST_AUTO_TEST_CASE(tree__allocate__10)
{
	test_tree t{};
	// 根
	test_iterator it0 = t.allocate(t.begin(), '0');
	// 単独の子
	test_iterator it1 = t.allocate(it0.begin(), '1');
	// 最後の子
	test_iterator it4 = t.allocate(it0.end(), '4');
	// 中間の子
	test_iterator it2 = t.allocate(it4, '2');
	// 中間の子
	test_iterator it3 = t.allocate(it4, '3');

	BOOST_CHECK(it0.i_child() == it1.m_index);

	BOOST_CHECK(it1.i_next() == it2.m_index);
	BOOST_CHECK(it2.i_next() == it3.m_index);
	BOOST_CHECK(it3.i_next() == it4.m_index);
	BOOST_CHECK(it4.i_next() == null_value);

	BOOST_CHECK(it4.i_prev() == it3.m_index);
	BOOST_CHECK(it3.i_prev() == it2.m_index);
	BOOST_CHECK(it2.i_prev() == it1.m_index);
	BOOST_CHECK(it1.i_prev() == it4.m_index);

	BOOST_CHECK(it1.i_parent() == it0.m_index);
	BOOST_CHECK(it2.i_parent() == it0.m_index);
	BOOST_CHECK(it3.i_parent() == it0.m_index);
	BOOST_CHECK(it4.i_parent() == it0.m_index);

	std::string s("1234");
	BOOST_CHECK(std::equal(s.begin(), s.end(), it0.begin(), it0.end()));
	BOOST_CHECK(t.size() == 5);
}

// フルチェック
BOOST_AUTO_TEST_CASE(tree__allocate__11)
{
	test_tree t = make_test_tree();

	BOOST_CHECK(t.at(0).i_parent() == null_value);
	BOOST_CHECK(t.at(0).i_child() == 1);
	BOOST_CHECK(t.at(0).i_prev() == 0);
	BOOST_CHECK(t.at(0).i_next() == null_value);

	BOOST_CHECK(t.at(1).i_parent() == 0);
	BOOST_CHECK(t.at(1).i_child() == 4);
	BOOST_CHECK(t.at(1).i_prev() == 3);
	BOOST_CHECK(t.at(1).i_next() == 2);

	BOOST_CHECK(t.at(2).i_parent() == 0);
	BOOST_CHECK(t.at(2).i_child() == 6);
	BOOST_CHECK(t.at(2).i_prev() == 1);
	BOOST_CHECK(t.at(2).i_next() == 3);

	BOOST_CHECK(t.at(3).i_parent() == 0);
	BOOST_CHECK(t.at(3).i_child() == null_value);
	BOOST_CHECK(t.at(3).i_prev() == 2);
	BOOST_CHECK(t.at(3).i_next() == null_value);

	BOOST_CHECK(t.at(4).i_parent() == 1);
	BOOST_CHECK(t.at(4).i_child() == null_value);
	BOOST_CHECK(t.at(4).i_prev() == 5);
	BOOST_CHECK(t.at(4).i_next() == 5);

	BOOST_CHECK(t.at(5).i_parent() == 1);
	BOOST_CHECK(t.at(5).i_child() == null_value);
	BOOST_CHECK(t.at(5).i_prev() == 4);
	BOOST_CHECK(t.at(5).i_next() == null_value);

	BOOST_CHECK(t.at(6).i_parent() == 2);
	BOOST_CHECK(t.at(6).i_child() == null_value);
	BOOST_CHECK(t.at(6).i_prev() == 6);
	BOOST_CHECK(t.at(6).i_next() == null_value);
}

// 根の削除
BOOST_AUTO_TEST_CASE(tree__free__6)
{
	test_tree t = make_test_tree();

	test_iterator it = t.free(t.begin());

	BOOST_CHECK(it == t.end());
	BOOST_CHECK(t.size() == 0);
	BOOST_CHECK(t.m_root == null_value);
}

// 単独要素の削除
BOOST_AUTO_TEST_CASE(tree__free__7)
{
	test_tree t = make_test_tree();

	test_iterator it = t.free(t.at(6));

	BOOST_CHECK(it == t.at(2).end());
	BOOST_CHECK(t.size() == 6);

	BOOST_CHECK(t.at(2).i_child() == null_value);
}

// 最終要素の削除
BOOST_AUTO_TEST_CASE(tree__free__8)
{
	test_tree t = make_test_tree();

	test_iterator it = t.free(t.at(5));
	BOOST_CHECK(it == t.at(1).end());
	BOOST_CHECK(t.size() == 6);
	BOOST_CHECK(t.at(1).i_child() == 4);
	BOOST_CHECK(t.at(4).i_prev() == 4);
	BOOST_CHECK(t.at(4).i_next() == null_value);
	
	it = t.free(t.at(3));
	BOOST_CHECK(it == t.at(0).end());
	BOOST_CHECK(t.size() == 5);
	BOOST_CHECK(t.at(0).i_child() == 1);
	BOOST_CHECK(t.at(2).i_prev() == 1);
	BOOST_CHECK(t.at(2).i_next() == null_value);
}

// 先頭要素の削除
BOOST_AUTO_TEST_CASE(tree__free__9)
{
	test_tree t = make_test_tree();

	test_iterator it = t.free(t.at(4));
	BOOST_CHECK(it == t.at(5));
	BOOST_CHECK(t.size() == 6);
	BOOST_CHECK(t.at(1).i_child() == 5);
	BOOST_CHECK(t.at(5).i_prev() == 5);
	BOOST_CHECK(t.at(5).i_next() == null_value);

	it = t.free(t.at(1));
	BOOST_CHECK(it == t.at(2));
	BOOST_CHECK(t.size() == 4);
	BOOST_CHECK(t.at(0).i_child() == 2);
	BOOST_CHECK(t.at(2).i_prev() == 3);
	BOOST_CHECK(t.at(2).i_next() == 3);
	BOOST_CHECK(t.at(3).i_prev() == 2);
	BOOST_CHECK(t.at(3).i_next() == null_value);
}

// 中間要素の削除
BOOST_AUTO_TEST_CASE(tree__free__10)
{
	test_tree t = make_test_tree();

	test_iterator it = t.free(t.at(2));
	BOOST_CHECK(it == t.at(3));
	BOOST_CHECK(t.size() == 5);
	BOOST_CHECK(t.at(0).i_child() == 1);
	BOOST_CHECK(t.at(1).i_prev() == 3);
	BOOST_CHECK(t.at(1).i_next() == 3);
	BOOST_CHECK(t.at(3).i_prev() == 1);
	BOOST_CHECK(t.at(3).i_next() == null_value);
}

// 最終要素の削除 -> 最終要素の削除
BOOST_AUTO_TEST_CASE(tree__free__11)
{
	test_tree t = make_test_tree();

	test_iterator it = t.free(t.at(3));
	BOOST_CHECK(it == t.at(0).end());
	BOOST_CHECK(t.size() == 6);
	BOOST_CHECK(t.at(0).i_child() == 1);
	BOOST_CHECK(t.at(1).i_prev() == 2);
	BOOST_CHECK(t.at(1).i_next() == 2);
	BOOST_CHECK(t.at(2).i_prev() == 1);
	BOOST_CHECK(t.at(2).i_next() == null_value);

	it = t.free(t.at(2));
	BOOST_CHECK(it == t.at(0).end());
	BOOST_CHECK(t.size() == 4);
	BOOST_CHECK(t.at(0).i_child() == 1);
	BOOST_CHECK(t.at(1).i_prev() == 1);
	BOOST_CHECK(t.at(1).i_next() == null_value);
}

// 先頭要素の削除 -> 先頭要素の削除
BOOST_AUTO_TEST_CASE(tree__free__12)
{
	test_tree t = make_test_tree();

	test_iterator it = t.free(t.at(1));
	BOOST_CHECK(it == t.at(2));
	BOOST_CHECK(t.size() == 4);
	BOOST_CHECK(t.at(0).i_child() == 2);
	BOOST_CHECK(t.at(2).i_prev() == 3);
	BOOST_CHECK(t.at(2).i_next() == 3);
	BOOST_CHECK(t.at(3).i_prev() == 2);
	BOOST_CHECK(t.at(3).i_next() == null_value);

	it = t.free(t.at(2));
	BOOST_CHECK(it == t.at(3));
	BOOST_CHECK(t.size() == 2);
	BOOST_CHECK(t.at(0).i_child() == 3);
	BOOST_CHECK(t.at(3).i_prev() == 3);
	BOOST_CHECK(t.at(3).i_next() == null_value);
}

// 中間要素の削除 -> 中間要素の削除
BOOST_AUTO_TEST_CASE(tree__free__13)
{
	test_tree t{};

	test_iterator it = t.allocate(t.end(), '0');
	t.allocate(it.end(), '1');
	t.allocate(it.end(), '2');
	t.allocate(it.end(), '3');
	t.allocate(it.end(), '4');

	it = t.free(t.at(2));
	BOOST_CHECK(it == t.at(3));
	BOOST_CHECK(t.size() == 4);
	BOOST_CHECK(t.at(0).i_child() == 1);
	BOOST_CHECK(t.at(1).i_prev() == 4);
	BOOST_CHECK(t.at(1).i_next() == 3);
	BOOST_CHECK(t.at(3).i_prev() == 1);
	BOOST_CHECK(t.at(3).i_next() == 4);
	BOOST_CHECK(t.at(4).i_prev() == 3);
	BOOST_CHECK(t.at(4).i_next() == null_value);

	it = t.free(t.at(3));
	BOOST_CHECK(it == t.at(4));
	BOOST_CHECK(t.size() == 3);
	BOOST_CHECK(t.at(0).i_child() == 1);
	BOOST_CHECK(t.at(1).i_prev() == 4);
	BOOST_CHECK(t.at(1).i_next() == 4);
	BOOST_CHECK(t.at(4).i_prev() == 1);
	BOOST_CHECK(t.at(4).i_next() == null_value);

	std::string s("14");
	BOOST_CHECK(std::equal(s.begin(), s.end(), t.at(0).begin(), t.at(0).end()));
	BOOST_CHECK(t.size() == 3);
}

BOOST_AUTO_TEST_CASE(tree__recursive_not_equal__1)
{
	test_tree t1 = make_test_tree();
	test_tree t2 = make_test_tree();
	BOOST_CHECK(test_tree::recursive_not_equal(t1.begin(), t1.end(), t2.begin(), t2.end()) == false);
}

BOOST_AUTO_TEST_CASE(tree__recursive_not_equal__2)
{
	test_tree t1 = make_test_tree();
	test_tree t2;
	BOOST_CHECK(test_tree::recursive_not_equal(t1.begin(), t1.end(), t2.begin(), t2.end()));
}

BOOST_AUTO_TEST_CASE(tree__recursive_not_equal__3)
{
	test_tree t1 = make_test_tree();
	test_tree t2;
	t2.insert(t2.end(), { '0', { 1, 2 } });
	BOOST_CHECK(test_tree::recursive_not_equal(t1.begin(), t1.end(), t2.begin(), t2.end()));
}

BOOST_AUTO_TEST_CASE(tree__recursive_not_equal__4)
{
	test_tree t1;
	test_tree t2 = make_test_tree();
	BOOST_CHECK(test_tree::recursive_not_equal(t1.begin(), t1.end(), t2.begin(), t2.end()));
}

BOOST_AUTO_TEST_CASE(tree__recursive_not_equal__5)
{
	test_tree t1;
	t1.insert(t1.end(), { '0', { 1, 2 } });
	test_tree t2 = make_test_tree();
	BOOST_CHECK(test_tree::recursive_not_equal(t1.begin(), t1.end(), t2.begin(), t2.end()));
}

BOOST_AUTO_TEST_CASE(tree__recursive_not_equal__6)
{
	test_tree t1;
	t1.insert(t1.end(), { '0', { 1, 2 } });
	test_tree t2;
	t2.insert(t2.end(), { '0', { 1, 2 } });
	BOOST_CHECK(test_tree::recursive_not_equal(t1.begin(), t1.end(), t2.begin(), t2.end()) == false);
}

BOOST_AUTO_TEST_CASE(tree__recursive_not_equal__7)
{
	test_tree t1 = make_test_tree();
	test_tree t2 = make_test_tree();
	BOOST_CHECK(test_tree::recursive_not_equal(t1.begin().begin(), t1.begin().end(), t2.begin().begin(), t2.begin().end()) == false);
}

BOOST_AUTO_TEST_CASE(tree__recursive_not_equal__8)
{
	test_tree t1 = make_test_tree();
	test_tree t2 = make_test_tree();
	BOOST_CHECK(test_tree::recursive_not_equal(++t1.begin().begin(), t1.begin().end(), ++t2.begin().begin(), t2.begin().end()) == false);
}

BOOST_AUTO_TEST_CASE(tree__recursive_not_equal__9)
{
	test_tree t1 = make_test_tree();
	test_tree t2 = make_test_tree();
	BOOST_CHECK(test_tree::recursive_not_equal(++t1.begin().begin(), t1.begin().end(), t2.begin().begin(), t2.begin().end()));
}

// tree()
BOOST_AUTO_TEST_CASE(tree__construct__1)
{
	wordring::tree<char> t;
	BOOST_CHECK(t.begin() == t.end());
}

// explicit tree(allocator_type const& alloc)
BOOST_AUTO_TEST_CASE(tree__construct__2)
{
	wordring::tree<char> t{ wordring::tree<char>::allocator_type() };
	BOOST_CHECK(t.begin() == t.end());
}

// explicit tree(value_type const& value, allocator_type const& alloc = allocator_type())
BOOST_AUTO_TEST_CASE(tree__construct__3)
{
	char const c = '0';
	wordring::tree<char> t{ c };
	BOOST_CHECK(*t.begin() == '0');
}

// explicit tree(value_type&& value, allocator_type const& alloc = allocator_type())
BOOST_AUTO_TEST_CASE(tree__construct__4)
{
	std::string s{ "0" };
	wordring::tree<std::string> t{ std::move(s) };
	BOOST_CHECK(*t.begin() == "0");
}

// tree(tree const&)
BOOST_AUTO_TEST_CASE(tree__construct__5)
{
	wordring::tree<char> t1{ '0' };
	wordring::tree<char> t2{ t1 };
	BOOST_CHECK(*t2.begin() == '0');
}

// tree(tree&&)
BOOST_AUTO_TEST_CASE(tree__construct__6)
{
	wordring::tree<char> t1{ '0' };
	wordring::tree<char> t2{ std::move(t1) };
	BOOST_CHECK(*t2.begin() == '0');
}

// explicit tree(node_proxy proxy, allocator_type const& alloc = allocator_type())
BOOST_AUTO_TEST_CASE(tree__construct__7)
{
	wordring::tree<char> t{ { '0', { '1', '2' } } };

	BOOST_CHECK(*t.begin() == '0');
	BOOST_CHECK(*t.begin().begin() == '1');
	BOOST_CHECK(*(++t.begin().begin()) == '2');
	BOOST_CHECK(++t.begin().begin() == --t.begin().end());
}

// tree& operator=(tree const&)
BOOST_AUTO_TEST_CASE(tree__assign__1)
{
	wordring::tree<char> t1{ '0' };
	wordring::tree<char> t2;
	t2 = t1;
	BOOST_CHECK(*t2.begin() == '0');
}

// tree& operator=(tree&&)
BOOST_AUTO_TEST_CASE(tree__assign__2)
{
	wordring::tree<char> t1{ '0' };
	wordring::tree<char> t2;
	t2 = std::move(t1);
	BOOST_CHECK(*t2.begin() == '0');
}

// tree& operator=(node_proxy proxy)
BOOST_AUTO_TEST_CASE(tree__assign__3)
{
	wordring::tree<char> t{ { '9', { '8', '7' } } };
	t = { '0', { '1', '2' } };
	BOOST_CHECK(*t.begin() == '0');
	BOOST_CHECK(*t.begin().begin() == '1');
}

// void assign(value_type const& value)
BOOST_AUTO_TEST_CASE(tree__assign__4)
{
	wordring::tree<char> t{ '0' };
	char const c = '1';
	t.assign(c);
	BOOST_CHECK(*t.begin() == '1');
}

// void assign(value_type&& value)
BOOST_AUTO_TEST_CASE(tree__assign__5)
{
	wordring::tree<char> t{ '0' };
	char const c = '1';
	t.assign(std::move(c));
	BOOST_CHECK(*t.begin() == '1');
}

// void assign(node_proxy proxy)
BOOST_AUTO_TEST_CASE(tree__assign__6)
{
	wordring::tree<char> t{ { '9', { '8', '7' } } };
	t.assign({ '0', { '1', '2' } });
	BOOST_CHECK(*t.begin() == '0');
	BOOST_CHECK(*t.begin().begin() == '1');
}

// reference front()
BOOST_AUTO_TEST_CASE(tree__front__1)
{
	wordring::tree<char> t{ { '0', { '1', '2' } } };
	t.front() = '9';
	BOOST_CHECK(t.front() == '9');
}

// const_reference front() const
BOOST_AUTO_TEST_CASE(tree__front__2)
{
	wordring::tree<char> const t{ { '0', { '1', '2' } } };
	BOOST_CHECK(t.front() == '0');
}

// reference back()
BOOST_AUTO_TEST_CASE(tree__back__1)
{
	wordring::tree<char> t{ { '0', { '1', '2' } } };
	t.back() = '9';
	BOOST_CHECK(t.back() == '9');
}

// const_reference back() const
BOOST_AUTO_TEST_CASE(tree__back__2)
{
	wordring::tree<char> const t{ { '0', { '1', '2' } } };
	BOOST_CHECK(t.back() == '0');
}

// iterator begin()
BOOST_AUTO_TEST_CASE(tree__begin__1)
{
	wordring::tree<char> t{ { '0', { '1', '2' } } };
	*t.begin() = '9';
	BOOST_CHECK(*t.begin() == '9');
}

// const_iterator begin() const
BOOST_AUTO_TEST_CASE(tree__begin__2)
{
	wordring::tree<char> const t{ { '0', { '1', '2' } } };
	BOOST_CHECK(*t.begin() == '0');
}

// const_iterator cbegin() const
BOOST_AUTO_TEST_CASE(tree__cbegin__1)
{
	wordring::tree<char> const t{ { '0', { '1', '2' } } };
	BOOST_CHECK(*t.cbegin() == '0');
}

// 
BOOST_AUTO_TEST_CASE(tree__cbegin__2)
{
	wordring::tree<char> t{ { '0', { '1', '2' } } };
	BOOST_CHECK(*t.cbegin() == '0');
}

// iterator end()
BOOST_AUTO_TEST_CASE(tree__end__1)
{
	wordring::tree<char> t{ { '0', { '1', '2' } } };
	*--t.end() = '9';
	BOOST_CHECK(*--t.end() == '9');
}

// const_iterator end() const
BOOST_AUTO_TEST_CASE(tree__end__2)
{
	wordring::tree<char> const t{ { '0', { '1', '2' } } };
	BOOST_CHECK(*--t.end() == '0');
}

// const_iterator cend() const
BOOST_AUTO_TEST_CASE(tree__cend__1)
{
	wordring::tree<char> const t{ { '0', { '1', '2' } } };
	BOOST_CHECK(*--t.cend() == '0');
}

// 
BOOST_AUTO_TEST_CASE(tree__cend__2)
{
	wordring::tree<char> t{ { '0', { '1', '2' } } };
	BOOST_CHECK(*--t.cend() == '0');
}

// bool empty() const
BOOST_AUTO_TEST_CASE(tree__empty__1)
{
	wordring::tree<char> const t;
	BOOST_CHECK(t.empty());
}

// 
BOOST_AUTO_TEST_CASE(tree__empty__2)
{
	wordring::tree<char> const t{ { '0', { '1', '2' } } };
	BOOST_CHECK(t.empty() == false);
}

// size_type size() const
BOOST_AUTO_TEST_CASE(tree__size__1)
{
	wordring::tree<char> const t{ { '0', { '1', '2' } } };
	BOOST_CHECK(t.size() == 3);
}

BOOST_AUTO_TEST_CASE(tree__size__2)
{
	wordring::tree<char> t{ { '0', { '1', '2' } } };
	t.erase(t.begin());
	BOOST_CHECK(t.size() == 0);
}

BOOST_AUTO_TEST_CASE(tree__size__3)
{
	wordring::tree<char> t{ { '0', { '1', '2' } } };
	t.erase(t.begin().begin());
	t.insert(t.begin().end(), '3');
	BOOST_CHECK(t.size() == 3);
}

// size_type max_size() const
BOOST_AUTO_TEST_CASE(tree__max_size__1)
{
	wordring::tree<char> const t;
	BOOST_CHECK(t.max_size() != 0);
}

// void clear()
BOOST_AUTO_TEST_CASE(tree__clear__1)
{
	wordring::tree<char> t{ { '0', { '1', '2' } } };
	t.clear();
	BOOST_CHECK(t.size() == 0);
	BOOST_CHECK(t.begin() == t.end());
}

// iterator insert(const_iterator pos, value_type const& value)
BOOST_AUTO_TEST_CASE(tree__insert__1)
{
	test_tree t{};

	char const s = '0';
	auto it = t.insert(t.begin(), s);

	BOOST_CHECK(*it == '0');
	BOOST_CHECK(t.size() == 1);
}

// iterator insert(const_iterator pos, value_type&& value)
BOOST_AUTO_TEST_CASE(tree__insert__2)
{
	test_tree t{};

	char const s = '0';
	auto it = t.insert(t.begin(), std::move(s));

	BOOST_CHECK(*it == '0');
	BOOST_CHECK(t.size() == 1);
}

// iterator insert(const_iterator pos, size_type count, value_type const& value)
BOOST_AUTO_TEST_CASE(tree__insert__3)
{
	test_tree t{};
	t.insert(t.end(), '0');

	auto it = t.insert(t.begin().end(), 5, '1');
	BOOST_CHECK(it == t.begin().begin());
	std::string s("11111");
	BOOST_CHECK(std::equal(s.begin(), s.end(), t.begin().begin(), t.begin().end()));
}

// iterator insert(const_iterator pos, InputIterator first, InputIterator last)
BOOST_AUTO_TEST_CASE(tree__insert__4)
{
	test_tree t{};
	t.insert(t.end(), '0');

	std::string s{ "12345" };
	auto it = t.insert(t.begin().end(), s.begin(), s.end());
	BOOST_CHECK(it == t.begin().begin());
	BOOST_CHECK(std::equal(s.begin(), s.end(), t.begin().begin(), t.begin().end()));
}

// iterator insert(const_iterator pos, tree_node_proxy<T> proxy)
BOOST_AUTO_TEST_CASE(tree__insert__5)
{
	test_tree t{};
	auto it = t.insert(t.end(), { '0', { '1','2' } });
	BOOST_CHECK(it == t.begin());
	BOOST_CHECK(*t.begin().begin() == '1');
}

// iterator insert(const_iterator pos, std::initializer_list<value_type> ilist)
BOOST_AUTO_TEST_CASE(tree__insert__6)
{
	test_tree t{};
	t.insert(t.end(), '0');

	auto it = t.insert(t.begin().end(), { '1', '2', '3', '4', '5' });
	BOOST_CHECK(it == t.begin().begin());
	std::string s{ "12345" };
	BOOST_CHECK(std::equal(s.begin(), s.end(), t.begin().begin(), t.begin().end()));
}

// iterator emplace(const_iterator pos, Args&&... args)
BOOST_AUTO_TEST_CASE(tree__emplace__1)
{
	test_tree t{};
	auto it = t.emplace(t.end(), '0');
	BOOST_CHECK(it == t.begin());
	BOOST_CHECK(*t.begin() == '0');
}

// iterator erase(const_iterator pos)
BOOST_AUTO_TEST_CASE(tree__erase__1)
{
	test_tree t = make_test_tree();
	auto it = t.erase(t.at(1));
	BOOST_CHECK(it == t.at(2));
	BOOST_CHECK(t.size() == 4);
}

BOOST_AUTO_TEST_CASE(tree__erase__2)
{
	test_tree t = make_test_tree();
	auto it = t.erase(t.at(0));
	BOOST_CHECK(it == t.end());
	BOOST_CHECK(t.empty());
}

// iterator erase(const_iterator first, const_iterator last)
BOOST_AUTO_TEST_CASE(tree__erase__3)
{
	test_tree t = make_test_tree();
	auto it = t.erase(t.at(1), t.at(3));
	BOOST_CHECK(it == t.at(3));
	BOOST_CHECK(t.size() == 2);
}

BOOST_AUTO_TEST_CASE(tree__erase__4)
{
	test_tree t = make_test_tree();
	auto it = t.erase(t.begin(), t.end());
	BOOST_CHECK(it == t.end());
	BOOST_CHECK(t.empty());
}

BOOST_AUTO_TEST_CASE(tree__equal__1)
{
	wordring::tree<char> t1{{ '0', { '1', '2' } }};
	wordring::tree<char> t2{{ '0', { '1', '2' } }};

	BOOST_CHECK(t1 == t2);
}

BOOST_AUTO_TEST_CASE(tree__equal__2)
{
	wordring::tree<char> t1{{ '0', { '1', '2' } }};
	wordring::tree<char> t2{{ '0', { '1', '3' } }};

	BOOST_CHECK((t1 == t2) == false);
}

BOOST_AUTO_TEST_CASE(tree__equal__3)
{
	wordring::tree<char> t1;
	wordring::tree<char> t2;

	BOOST_CHECK(t1 == t2);
}

BOOST_AUTO_TEST_CASE(tree__equal__4)
{
	wordring::tree<char> t1;
	wordring::tree<char> t2{{ '0', { '1', '3' } }};

	BOOST_CHECK((t1 == t2) == false);
}

BOOST_AUTO_TEST_CASE(tree__equal__5)
{
	wordring::tree<char> t1{{ '0', { '1', '2' } }};
	wordring::tree<char> t2;

	BOOST_CHECK((t1 == t2) == false);
}

BOOST_AUTO_TEST_CASE(tree__not_equal__1)
{
	wordring::tree<char> t1{{ '0', { '1', '2' } }};
	wordring::tree<char> t2{{ '0', { '1', '2' } }};

	BOOST_CHECK((t1 != t2) == false);
}

BOOST_AUTO_TEST_CASE(tree__not_equal__2)
{
	wordring::tree<char> t1{{ '0', { '1', '2' } }};
	wordring::tree<char> t2{{ '0', { '1', '3' } }};

	BOOST_CHECK(t1 != t2);
}

BOOST_AUTO_TEST_CASE(tree__not_equal__3)
{
		wordring::tree<char> t1;
		wordring::tree<char> t2;

		BOOST_CHECK((t1 != t2) == false);
}

BOOST_AUTO_TEST_CASE(tree__not_equal__4)
{
	wordring::tree<char> t1;
	wordring::tree<char> t2{{ '0', { '1', '3' } }};

	BOOST_CHECK(t1 != t2);
}

BOOST_AUTO_TEST_CASE(tree__not_equal__5)
{
	wordring::tree<char> t1{{ '0', { '1', '2' } }};
	wordring::tree<char> t2;

	BOOST_CHECK(t1 != t2);
}

BOOST_AUTO_TEST_SUITE_END()
