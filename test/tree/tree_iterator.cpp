// test/tree/tree_iterator.cpp

#include <boost/test/unit_test.hpp>

#include <wordring/tree/tree.hpp>
#include <wordring/tree/tree_iterator.hpp>

#include <algorithm>
#include <iterator>
#include <string>

BOOST_AUTO_TEST_SUITE(tree_iterator__test)

// ----------------------------------------------------------------------------
// tree_iterator_stack
// ----------------------------------------------------------------------------

// explicit tree_iterator_stack(Allocator const& alloc = Allocator())
BOOST_AUTO_TEST_CASE(tree_iterator_stack__construct__1)
{
	using namespace wordring;
	using stack_type = detail::tree_iterator_stack<tree<char>::iterator, std::allocator<tree<char>::iterator>>;

	stack_type stack;

	BOOST_CHECK(stack.empty());
}

// explicit tree_iterator_stack(Iterator const& base, Allocator const& alloc = Allocator())
BOOST_AUTO_TEST_CASE(tree_iterator_stack__construct__2)
{
	using namespace wordring;
	using stack_type = detail::tree_iterator_stack<tree<char>::iterator, std::allocator<tree<char>::iterator>>;

	tree<char> t{ { '0', { { '1', { '4', '5' } }, { '2', { '6' } }, { '3' } } } };
	stack_type stack{ t.begin() };

	BOOST_CHECK(stack.empty() == false);
}

// tree_iterator_stack(Iterator first, Iterator last, Allocator const& alloc = Allocator())
BOOST_AUTO_TEST_CASE(tree_iterator_stack__construct__3)
{
	using namespace wordring;
	using stack_type = detail::tree_iterator_stack<tree<char>::iterator, std::allocator<tree<char>::iterator>>;

	tree<char> t{ { '0', { { '1', { '4', '5' } }, { '2', { '6' } }, { '3' } } } };
	stack_type stack{ t.begin().begin(), t.begin().end() };

	BOOST_CHECK(!stack.empty());
}

// Iterator& top() const
BOOST_AUTO_TEST_CASE(tree_iterator_stack__top__1)
{
	using namespace wordring;
	using stack_type = detail::tree_iterator_stack<tree<char>::iterator, std::allocator<tree<char>::iterator>>;

	tree<char> t{ { '0', { { '1', { '4', '5' } }, { '2', { '6' } }, { '3' } } } };
	stack_type stack{ t.begin().begin(), t.begin().end() };

	BOOST_CHECK(*stack.top() == '1');
}

// void pop()
BOOST_AUTO_TEST_CASE(tree_iterator_stack__pop__1)
{
	using namespace wordring;
	using stack_type = detail::tree_iterator_stack<tree<char>::iterator, std::allocator<tree<char>::iterator>>;

	tree<char> t{ { '0', { { '1', { '4', '5' } }, { '2', { '6' } }, { '3' } } } };
	stack_type stack{ t.begin().begin(), t.begin().end() };

	std::string s;
	while (!stack.empty())
	{
		s.push_back(*stack.top());
		stack.pop();
	}
	BOOST_CHECK(s == "123");
}

// void push(Iterator const& it)
BOOST_AUTO_TEST_CASE(tree_iterator_stack__push__1)
{
	using namespace wordring;
	using stack_type = detail::tree_iterator_stack<tree<char>::iterator, std::allocator<tree<char>::iterator>>;

	tree<char> t{ { '0', { { '1', { '4', '5' } }, { '2', { '6' } }, { '3' } } } };
	stack_type stack{ t.begin().begin(), t.begin().end() };

	stack.push(t.begin());

	std::string s;
	while (!stack.empty())
	{
		s.push_back(*stack.top());
		stack.pop();
	}
	BOOST_CHECK(s == "0123");
}

// void push(Iterator first, Iterator last)
BOOST_AUTO_TEST_CASE(tree_iterator_stack__push__2)
{
	using namespace wordring;
	using stack_type = detail::tree_iterator_stack<tree<char>::iterator, std::allocator<tree<char>::iterator>>;

	tree<char> t{ { '0', { { '1', { '4', '5' } }, { '2', { '6' } }, { '3' } } } };
	stack_type stack{ t.begin() };

	stack.push(t.begin().begin(), t.begin().end());

	std::string s;
	while (!stack.empty())
	{
		s.push_back(*stack.top());
		stack.pop();
	}
	BOOST_CHECK(s == "1230");
}

// bool empty() const
BOOST_AUTO_TEST_CASE(tree_iterator_stack__empty__1)
{
	using namespace wordring;
	using stack_type = detail::tree_iterator_stack<tree<char>::iterator, std::allocator<tree<char>::iterator>>;

	tree<char> t{ { '0', { { '1', { '4', '5' } }, { '2', { '6' } }, { '3' } } } };

	stack_type stack;
	BOOST_CHECK(stack.empty());
	stack.push(t.begin());
	BOOST_CHECK(stack.empty() == false);
	stack.pop();
	BOOST_CHECK(stack.empty());
}

// ----------------------------------------------------------------------------
// tree_iterator_queue
// ----------------------------------------------------------------------------

// explicit tree_iterator_queue(Allocator const& alloc = Allocator())
BOOST_AUTO_TEST_CASE(tree_iterator_queue__construct__1)
{
	using namespace wordring;
	using queue_type = detail::tree_iterator_queue<tree<char>::iterator, std::allocator<tree<char>::iterator>>;

	queue_type queue;

	BOOST_CHECK(queue.empty());
}

// explicit tree_iterator_queue(Iterator const& base, Allocator const& alloc = Allocator())
BOOST_AUTO_TEST_CASE(tree_iterator_queue__construct__2)
{
	using namespace wordring;
	using queue_type = detail::tree_iterator_queue<tree<char>::iterator, std::allocator<tree<char>::iterator>>;

	tree<char> t{ { '0', { { '1', { '4', '5' } }, { '2', { '6' } }, { '3' } } } };
	queue_type queue{ t.begin() };

	BOOST_CHECK(queue.empty() == false);
}

// tree_iterator_queue(Iterator first, Iterator last, Allocator const& alloc = Allocator())
BOOST_AUTO_TEST_CASE(tree_iterator_queue__construct__3)
{
	using namespace wordring;
	using queue_type = detail::tree_iterator_queue<tree<char>::iterator, std::allocator<tree<char>::iterator>>;

	tree<char> t{ { '0', { { '1', { '4', '5' } }, { '2', { '6' } }, { '3' } } } };
	queue_type queue{ t.begin().begin(), t.begin().end() };

	BOOST_CHECK(!queue.empty());
}

// Iterator& top() const
BOOST_AUTO_TEST_CASE(tree_iterator_queue__top__1)
{
	using namespace wordring;
	using queue_type = detail::tree_iterator_queue<tree<char>::iterator, std::allocator<tree<char>::iterator>>;

	tree<char> t{ { '0', { { '1', { '4', '5' } }, { '2', { '6' } }, { '3' } } } };
	queue_type queue{ t.begin().begin(), t.begin().end() };

	BOOST_CHECK(*queue.top() == '1');
}

// void pop()
BOOST_AUTO_TEST_CASE(tree_iterator_queue__pop__1)
{
	using namespace wordring;
	using queue_type = detail::tree_iterator_queue<tree<char>::iterator, std::allocator<tree<char>::iterator>>;

	tree<char> t{ { '0', { { '1', { '4', '5' } }, { '2', { '6' } }, { '3' } } } };
	queue_type queue{ t.begin().begin(), t.begin().end() };

	std::string s;
	while (!queue.empty())
	{
		s.push_back(*queue.top());
		queue.pop();
	}
	BOOST_CHECK(s == "123");
}

// void push(Iterator const& it)
BOOST_AUTO_TEST_CASE(tree_iterator_queue__push__1)
{
	using namespace wordring;
	using queue_type = detail::tree_iterator_queue<tree<char>::iterator, std::allocator<tree<char>::iterator>>;

	tree<char> t{ { '0', { { '1', { '4', '5' } }, { '2', { '6' } }, { '3' } } } };
	queue_type queue{ t.begin().begin(), t.begin().end() };

	queue.push(t.begin());

	std::string s;
	while (!queue.empty())
	{
		s.push_back(*queue.top());
		queue.pop();
	}
	BOOST_CHECK(s == "1230");
}

// void push(Iterator first, Iterator last)
BOOST_AUTO_TEST_CASE(tree_iterator_queue__push__2)
{
	using namespace wordring;
	using queue_type = detail::tree_iterator_queue<tree<char>::iterator, std::allocator<tree<char>::iterator>>;

	tree<char> t{ { '0', { { '1', { '4', '5' } }, { '2', { '6' } }, { '3' } } } };
	queue_type queue{ t.begin() };

	queue.push(t.begin().begin(), t.begin().end());

	std::string s;
	while (!queue.empty())
	{
		s.push_back(*queue.top());
		queue.pop();
	}
	BOOST_CHECK(s == "0123");
}

// bool empty() const
BOOST_AUTO_TEST_CASE(tree_iterator_queue__empty__1)
{
	using namespace wordring;
	using queue_type = detail::tree_iterator_queue<tree<char>::iterator, std::allocator<tree<char>::iterator>>;

	tree<char> t{ { '0', { { '1', { '4', '5' } }, { '2', { '6' } }, { '3' } } } };

	queue_type queue;
	BOOST_CHECK(queue.empty());
	queue.push(t.begin());
	BOOST_CHECK(queue.empty() == false);
	queue.pop();
	BOOST_CHECK(queue.empty());
}

// ----------------------------------------------------------------------------
// tree_iterator
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(tree_iterator__construct__1)
{
	using namespace wordring;

	tree<char> t{ { '0', {{ '1', { '4', '5' } }, { '2', { '6' } }, { '3' } } } };

	auto it1 = tree_iterator<decltype(t.begin())>();
	auto it2 = tree_iterator<decltype(t.begin())>();

	BOOST_CHECK(it1 == it2);
}

BOOST_AUTO_TEST_CASE(tree_iterator__construct__2)
{
	using namespace wordring;

	tree<char> const t{ { '0', {{ '1', { '4', '5' } }, { '2', { '6' } }, { '3' } } } };

	auto it1 = tree_iterator<decltype(t.begin())>();
	auto it2 = tree_iterator<decltype(t.begin())>();

	BOOST_CHECK(it1 == it2);
}

BOOST_AUTO_TEST_CASE(tree_iterator__construct__3)
{
	using namespace wordring;

	tree<std::string> t{ { "0", {{ "1", { "4", "5" } }, { "2", { "6" } }, { "3" } } } };

	auto it1 = tree_iterator<decltype(t.begin())>(t.begin());
	auto it2 = tree_iterator<decltype(t.begin())>(t.begin());

	BOOST_CHECK(it1 == it2);
}

BOOST_AUTO_TEST_CASE(tree_iterator__construct__4)
{
	using namespace wordring;

	tree<std::string> const t{ { "0", {{ "1", { "4", "5" } }, { "2", { "6" } }, { "3" } } } };

	auto it1 = tree_iterator<decltype(t.begin())>(t.begin());
	auto it2 = tree_iterator<decltype(t.begin())>(t.begin());

	BOOST_CHECK(it1 == it2);
}

BOOST_AUTO_TEST_CASE(tree_iterator__construct__5)
{
	using namespace wordring;

	tree<char> t{ { '0', {{ '1', { '4', '5' } }, { '2', { '6' } }, { '3' } } } };

	auto it1 = tree_iterator<decltype(t.begin())>(t.begin());
	auto it2 = tree_iterator<decltype(t.begin())>();

	BOOST_CHECK((it1 == it2) == false);
}

BOOST_AUTO_TEST_CASE(tree_iterator__construct__6)
{
	using namespace wordring;

	tree<char> const t{ { '0', {{ '1', { '4', '5' } }, { '2', { '6' } }, { '3' } } } };

	auto it1 = tree_iterator<decltype(t.begin())>(t.begin());
	auto it2 = tree_iterator<decltype(t.begin())>();

	BOOST_CHECK((it1 == it2) == false);
}

BOOST_AUTO_TEST_CASE(tree_iterator__construct__7)
{
	using namespace wordring;

	tree<char> t{ { '0', {{ '1', { '4', '5' } }, { '2', { '6' } }, { '3' } } } };

	auto it1 = tree_iterator<decltype(t.begin())>();
	auto it2 = tree_iterator<decltype(t.begin())>(t.begin());

	BOOST_CHECK((it1 == it2) == false);
}

BOOST_AUTO_TEST_CASE(tree_iterator__construct__8)
{
	using namespace wordring;

	tree<char> const t{ { '0', {{ '1', { '4', '5' } }, { '2', { '6' } }, { '3' } } } };

	auto it1 = tree_iterator<decltype(t.begin())>();
	auto it2 = tree_iterator<decltype(t.begin())>(t.begin());

	BOOST_CHECK((it1 == it2) == false);
}

BOOST_AUTO_TEST_CASE(tree_iterator__construct__9)
{
	using namespace wordring;

	tree<char> t{ { '0', {{ '1', { '4', '5' } }, { '2', { '6' } }, { '3' } } } };

	auto it1 = tree_iterator<decltype(t.begin())>(t.begin().begin(), t.begin().end());
	auto it2 = tree_iterator<decltype(t.begin())>();

	BOOST_CHECK((it1 == it2) == false);
}

BOOST_AUTO_TEST_CASE(tree_iterator__construct__10)
{
	using namespace wordring;

	tree<char> const t{ { '0', {{ '1', { '4', '5' } }, { '2', { '6' } }, { '3' } } } };

	auto it1 = tree_iterator<decltype(t.begin())>(t.begin().begin(), t.begin().end());
	auto it2 = tree_iterator<decltype(t.begin())>();

	BOOST_CHECK((it1 == it2) == false);
}

// basic_tree_iterator& operator=(basic_tree_iterator&& rhs)
BOOST_AUTO_TEST_CASE(tree_iterator__assign__1)
{
	using namespace wordring;

	tree<char> const t{ { '0', {{ '1', { '4', '5' } }, { '2', { '6' } }, { '3' } } } };

	auto it1 = tree_iterator<decltype(t.begin())>(t.begin().begin(), t.begin().end());
	auto it2 = tree_iterator<decltype(t.begin())>(t.begin().begin(), t.begin().end());
	auto it3 = tree_iterator<decltype(t.begin())>();

	BOOST_CHECK((it1 == it3) == false);

	it3 = std::move(it2);

	BOOST_CHECK(it1 == it3);
}

// base_type base() const
BOOST_AUTO_TEST_CASE(tree_iterator__base__1)
{
	using namespace wordring;

	tree<char> const t{ { '0', {{ '1', { '4', '5' } }, { '2', { '6' } }, { '3' } } } };

	auto it1 = tree_iterator<decltype(t.begin())>(t.begin().begin(), t.begin().end());
	auto it2 = tree_iterator<decltype(t.begin())>(t.begin().begin(), t.begin().end());

	BOOST_CHECK(it1.base() == it2.base());
}

BOOST_AUTO_TEST_CASE(tree_iterator__base__2)
{
	using namespace wordring;

	tree<char> const t{ { '0', {{ '1', { '4', '5' } }, { '2', { '6' } }, { '3' } } } };

	auto it1 = tree_iterator<decltype(t.begin())>(t.begin().begin(), t.begin().end());
	auto it2 = tree_iterator<decltype(t.begin())>(t.begin().begin());

	BOOST_CHECK(it1.base() == it2.base());
}

BOOST_AUTO_TEST_CASE(tree_iterator__base__3)
{
	using namespace wordring;

	tree<char> const t{ { '0', {{ '1', { '4', '5' } }, { '2', { '6' } }, { '3' } } } };

	auto it1 = tree_iterator<decltype(t.begin())>(t.begin().end());
	auto it2 = tree_iterator<decltype(t.begin())>(t.begin().begin());

	BOOST_CHECK(it1.base() != it2.base());
}

// reference operator*() const
BOOST_AUTO_TEST_CASE(tree_iterator__reference__1)
{
	using namespace wordring;

	tree<char> const t{ { '0', {{ '1', { '4', '5' } }, { '2', { '6' } }, { '3' } } } };

	auto it = tree_iterator<decltype(t.begin())>(t.begin());

	BOOST_CHECK(*it == '0');
}

BOOST_AUTO_TEST_CASE(tree_iterator__reference__2)
{
	using namespace wordring;

	tree<char> t{ { '0', {{ '1', { '4', '5' } }, { '2', { '6' } }, { '3' } } } };

	auto it = tree_iterator<decltype(t.begin())>(t.begin());
	*it = '9';

	BOOST_CHECK(*it == '9');
}

// pointer operator->() const
BOOST_AUTO_TEST_CASE(tree_iterator__reference__3)
{
	using namespace wordring;

	tree<std::string> const t{ { "0", {{ "1", { "4", "5" } }, { "2", { "6" } }, { "3" } } } };

	auto it = tree_iterator<decltype(t.begin())>(t.begin());

	BOOST_CHECK(it->front() == '0');
}

BOOST_AUTO_TEST_CASE(tree_iterator__reference__4)
{
	using namespace wordring;

	tree<std::string> t{ { "0", {{ "1", { "4", "5" } }, { "2", { "6" } }, { "3" } } } };

	auto it = tree_iterator<decltype(t.begin())>(t.begin());
	it->front() = '9';

	BOOST_CHECK(*it == "9");
}

// basic_tree_iterator& operator++()
BOOST_AUTO_TEST_CASE(tree_iterator__increment__1)
{
	using namespace wordring;

	tree<char> t{ { '0', {{ '1', { '4', '5' } }, { '2', { '6' } }, { '3' } } } };

	auto it1 = tree_iterator<decltype(t.begin())>(t.begin());
	auto it2 = tree_iterator<decltype(t.begin())>();

	std::string s;
	std::copy(it1, it2, std::back_inserter(s));

	BOOST_CHECK(s == "0145263");
}

BOOST_AUTO_TEST_CASE(tree_iterator__increment__2)
{
	using namespace wordring;

	tree<char> const t{ { '0', {{ '1', { '4', '5' } }, { '2', { '6' } }, { '3' } } } };

	auto it1 = tree_iterator<decltype(t.begin())>(t.begin());
	auto it2 = tree_iterator<decltype(t.begin())>();

	std::string s;
	std::copy(it1, it2, std::back_inserter(s));

	BOOST_CHECK(s == "0145263");
}

BOOST_AUTO_TEST_CASE(tree_iterator__increment__3)
{
	using namespace wordring;

	tree<char> const t{ { '0', {{ '1', { '4', '5' } }, { '2', { '6' } }, { '3' } } } };

	auto it1 = tree_iterator<decltype(t.begin())>(t.begin().begin(), t.begin().end());
	auto it2 = tree_iterator<decltype(t.begin())>();

	std::string s;
	std::copy(it1, it2, std::back_inserter(s));

	BOOST_CHECK(s == "145263");
}

// base_type operator++(int)
BOOST_AUTO_TEST_CASE(tree_iterator__increment__4)
{
	using namespace wordring;

	tree<char> t{ { '0', {{ '1', { '4', '5' } }, { '2', { '6' } }, { '3' } } } };

	auto it1 = tree_iterator<decltype(t.begin())>(t.begin());
	auto it2 = tree_iterator<decltype(t.begin())>();

	std::string s;
	while(it1 != it2) s.push_back(*it1++);

	BOOST_CHECK(s == "0145263");
}

BOOST_AUTO_TEST_CASE(tree_iterator__increment__5)
{
	using namespace wordring;

	tree<char> const t{ { '0', {{ '1', { '4', '5' } }, { '2', { '6' } }, { '3' } } } };

	auto it1 = tree_iterator<decltype(t.begin())>(t.begin());
	auto it2 = tree_iterator<decltype(t.begin())>();

	std::string s;
	while(it1 != it2) s.push_back(*it1++);

	BOOST_CHECK(s == "0145263");
}

BOOST_AUTO_TEST_CASE(tree_iterator__increment__6)
{
	using namespace wordring;

	tree<char> const t{ { '0', {{ '1', { '4', '5' } }, { '2', { '6' } }, { '3' } } } };

	auto it1 = tree_iterator<decltype(t.begin())>(t.begin().begin(), t.begin().end());
	auto it2 = tree_iterator<decltype(t.begin())>();

	std::string s;
	while(it1 != it2) s.push_back(*it1++);

	BOOST_CHECK(s == "145263");
}

// inline bool operator==(basic_tree_iterator const& lhs, basic_tree_iterator const& rhs)

BOOST_AUTO_TEST_CASE(tree_iterator__equal__1)
{
	using namespace wordring;

	tree<char> t{ { '0', {{ '1', { '4', '5' } }, { '2', { '6' } }, { '3' } } } };

	auto it1 = tree_iterator<decltype(t.begin())>(t.begin());
	auto it2 = tree_iterator<decltype(t.begin())>(t.begin());

	BOOST_CHECK(it1 == it2);
}

BOOST_AUTO_TEST_CASE(tree_iterator__equal__2)
{
	using namespace wordring;

	tree<char> const t{ { '0', {{ '1', { '4', '5' } }, { '2', { '6' } }, { '3' } } } };

	auto it1 = tree_iterator<decltype(t.begin())>();
	auto it2 = tree_iterator<decltype(t.begin())>();

	BOOST_CHECK(it1 == it2);
}

BOOST_AUTO_TEST_CASE(tree_iterator__equal__3)
{
	using namespace wordring;

	tree<char> const t{ { '0', {{ '1', { '4', '5' } }, { '2', { '6' } }, { '3' } } } };

	auto it1 = tree_iterator<decltype(t.begin())>(t.begin());
	auto it2 = tree_iterator<decltype(t.begin())>();

	BOOST_CHECK((it1 == it2) == false);
}

BOOST_AUTO_TEST_CASE(tree_iterator__equal__4)
{
	using namespace wordring;

	tree<char> const t{ { '0', {{ '1', { '4', '5' } }, { '2', { '6' } }, { '3' } } } };

	auto it1 = tree_iterator<decltype(t.begin())>();
	auto it2 = tree_iterator<decltype(t.begin())>(t.begin());

	BOOST_CHECK((it1 == it2) == false);
}

// inline bool operator!=(basic_tree_iterator const& lhs, basic_tree_iterator const& rhs)

BOOST_AUTO_TEST_CASE(tree_iterator__not_equal__1)
{
	using namespace wordring;

	tree<char> const t{ { '0', {{ '1', { '4', '5' } }, { '2', { '6' } }, { '3' } } } };

	auto it1 = tree_iterator<decltype(t.begin())>(t.begin());
	auto it2 = tree_iterator<decltype(t.begin())>(t.begin());

	BOOST_CHECK((it1 != it2) == false);
}

BOOST_AUTO_TEST_CASE(tree_iterator__not_equal__2)
{
	using namespace wordring;

	tree<char> const t{ { '0', {{ '1', { '4', '5' } }, { '2', { '6' } }, { '3' } } } };

	auto it1 = tree_iterator<decltype(t.begin())>();
	auto it2 = tree_iterator<decltype(t.begin())>();

	BOOST_CHECK((it1 != it2) == false);
}

BOOST_AUTO_TEST_CASE(tree_iterator__not_equal__3)
{
	using namespace wordring;

	tree<char> const t{ { '0', {{ '1', { '4', '5' } }, { '2', { '6' } }, { '3' } } } };

	auto it1 = tree_iterator<decltype(t.begin())>(t.begin());
	auto it2 = tree_iterator<decltype(t.begin())>();

	BOOST_CHECK(it1 != it2);
}

BOOST_AUTO_TEST_CASE(tree_iterator__not_equal__4)
{
	using namespace wordring;

	tree<char> const t{ { '0', {{ '1', { '4', '5' } }, { '2', { '6' } }, { '3' } } } };

	auto it1 = tree_iterator<decltype(t.begin())>();
	auto it2 = tree_iterator<decltype(t.begin())>(t.begin());

	BOOST_CHECK(it1 != it2);
}

// ----------------------------------------------------------------------------
// level_order_tree_iterator
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(level_order_tree_iterator__construct__1)
{
	using namespace wordring;

	tree<char> t{ { '0', {{ '1', { '4', '5' } }, { '2', { '6' } }, { '3' } } } };

	auto it1 = level_order_tree_iterator<decltype(t.begin())>();
	auto it2 = level_order_tree_iterator<decltype(t.begin())>();

	BOOST_CHECK(it1 == it2);
}

BOOST_AUTO_TEST_CASE(level_order_tree_iterator__construct__2)
{
	using namespace wordring;

	tree<char> const t{ { '0', {{ '1', { '4', '5' } }, { '2', { '6' } }, { '3' } } } };

	auto it1 = level_order_tree_iterator<decltype(t.begin())>();
	auto it2 = level_order_tree_iterator<decltype(t.begin())>();

	BOOST_CHECK(it1 == it2);
}

BOOST_AUTO_TEST_CASE(level_order_tree_iterator__construct__3)
{
	using namespace wordring;

	tree<std::string> t{ { "0", {{ "1", { "4", "5" } }, { "2", { "6" } }, { "3" } } } };

	auto it1 = level_order_tree_iterator<decltype(t.begin())>(t.begin());
	auto it2 = level_order_tree_iterator<decltype(t.begin())>(t.begin());

	BOOST_CHECK(it1 == it2);
}

BOOST_AUTO_TEST_CASE(level_order_tree_iterator__construct__4)
{
	using namespace wordring;

	tree<std::string> const t{ { "0", {{ "1", { "4", "5" } }, { "2", { "6" } }, { "3" } } } };

	auto it1 = level_order_tree_iterator<decltype(t.begin())>(t.begin());
	auto it2 = level_order_tree_iterator<decltype(t.begin())>(t.begin());

	BOOST_CHECK(it1 == it2);
}

BOOST_AUTO_TEST_CASE(level_order_tree_iterator__construct__5)
{
	using namespace wordring;

	tree<char> t{ { '0', {{ '1', { '4', '5' } }, { '2', { '6' } }, { '3' } } } };

	auto it1 = level_order_tree_iterator<decltype(t.begin())>(t.begin());
	auto it2 = level_order_tree_iterator<decltype(t.begin())>();

	BOOST_CHECK((it1 == it2) == false);
}

BOOST_AUTO_TEST_CASE(level_order_tree_iterator__construct__6)
{
	using namespace wordring;

	tree<char> const t{ { '0', {{ '1', { '4', '5' } }, { '2', { '6' } }, { '3' } } } };

	auto it1 = level_order_tree_iterator<decltype(t.begin())>(t.begin());
	auto it2 = level_order_tree_iterator<decltype(t.begin())>();

	BOOST_CHECK((it1 == it2) == false);
}

BOOST_AUTO_TEST_CASE(level_order_tree_iterator__construct__7)
{
	using namespace wordring;

	tree<char> t{ { '0', {{ '1', { '4', '5' } }, { '2', { '6' } }, { '3' } } } };

	auto it1 = level_order_tree_iterator<decltype(t.begin())>();
	auto it2 = level_order_tree_iterator<decltype(t.begin())>(t.begin());

	BOOST_CHECK((it1 == it2) == false);
}

BOOST_AUTO_TEST_CASE(level_order_tree_iterator__construct__8)
{
	using namespace wordring;

	tree<char> const t{ { '0', {{ '1', { '4', '5' } }, { '2', { '6' } }, { '3' } } } };

	auto it1 = level_order_tree_iterator<decltype(t.begin())>();
	auto it2 = level_order_tree_iterator<decltype(t.begin())>(t.begin());

	BOOST_CHECK((it1 == it2) == false);
}

BOOST_AUTO_TEST_CASE(level_order_tree_iterator__construct__9)
{
	using namespace wordring;

	tree<char> t{ { '0', {{ '1', { '4', '5' } }, { '2', { '6' } }, { '3' } } } };

	auto it1 = level_order_tree_iterator<decltype(t.begin())>(t.begin().begin(), t.begin().end());
	auto it2 = level_order_tree_iterator<decltype(t.begin())>();

	BOOST_CHECK((it1 == it2) == false);
}

BOOST_AUTO_TEST_CASE(level_order_tree_iterator__construct__10)
{
	using namespace wordring;

	tree<char> const t{ { '0', {{ '1', { '4', '5' } }, { '2', { '6' } }, { '3' } } } };

	auto it1 = level_order_tree_iterator<decltype(t.begin())>(t.begin().begin(), t.begin().end());
	auto it2 = level_order_tree_iterator<decltype(t.begin())>();

	BOOST_CHECK((it1 == it2) == false);
}

// basic_level_order_tree_iterator& operator=(basic_level_order_tree_iterator&& rhs)
BOOST_AUTO_TEST_CASE(level_order_tree_iterator__assign__1)
{
	using namespace wordring;

	tree<char> const t{ { '0', {{ '1', { '4', '5' } }, { '2', { '6' } }, { '3' } } } };

	auto it1 = level_order_tree_iterator<decltype(t.begin())>(t.begin().begin(), t.begin().end());
	auto it2 = level_order_tree_iterator<decltype(t.begin())>(t.begin().begin(), t.begin().end());
	auto it3 = level_order_tree_iterator<decltype(t.begin())>();

	BOOST_CHECK((it1 == it3) == false);

	it3 = std::move(it2);

	BOOST_CHECK(it1 == it3);
}

// base_type base() const
BOOST_AUTO_TEST_CASE(level_order_tree_iterator__base__1)
{
	using namespace wordring;

	tree<char> const t{ { '0', {{ '1', { '4', '5' } }, { '2', { '6' } }, { '3' } } } };

	auto it1 = level_order_tree_iterator<decltype(t.begin())>(t.begin().begin(), t.begin().end());
	auto it2 = level_order_tree_iterator<decltype(t.begin())>(t.begin().begin(), t.begin().end());

	BOOST_CHECK(it1.base() == it2.base());
}

BOOST_AUTO_TEST_CASE(level_order_tree_iterator__base__2)
{
	using namespace wordring;

	tree<char> const t{ { '0', {{ '1', { '4', '5' } }, { '2', { '6' } }, { '3' } } } };

	auto it1 = level_order_tree_iterator<decltype(t.begin())>(t.begin().begin(), t.begin().end());
	auto it2 = level_order_tree_iterator<decltype(t.begin())>(t.begin().begin());

	BOOST_CHECK(it1.base() == it2.base());
}

BOOST_AUTO_TEST_CASE(level_order_tree_iterator__base__3)
{
	using namespace wordring;

	tree<char> const t{ { '0', {{ '1', { '4', '5' } }, { '2', { '6' } }, { '3' } } } };

	auto it1 = level_order_tree_iterator<decltype(t.begin())>(t.begin().end());
	auto it2 = level_order_tree_iterator<decltype(t.begin())>(t.begin().begin());

	BOOST_CHECK(it1.base() != it2.base());
}

// reference operator*() const
BOOST_AUTO_TEST_CASE(level_order_tree_iterator__reference__1)
{
	using namespace wordring;

	tree<char> const t{ { '0', {{ '1', { '4', '5' } }, { '2', { '6' } }, { '3' } } } };

	auto it = level_order_tree_iterator<decltype(t.begin())>(t.begin());

	BOOST_CHECK(*it == '0');
}

BOOST_AUTO_TEST_CASE(level_order_tree_iterator__reference__2)
{
	using namespace wordring;

	tree<char> t{ { '0', {{ '1', { '4', '5' } }, { '2', { '6' } }, { '3' } } } };

	auto it = level_order_tree_iterator<decltype(t.begin())>(t.begin());
	*it = '9';

	BOOST_CHECK(*it == '9');
}

// pointer operator->() const
BOOST_AUTO_TEST_CASE(level_order_tree_iterator__reference__3)
{
	using namespace wordring;

	tree<std::string> const t{ { "0", {{ "1", { "4", "5" } }, { "2", { "6" } }, { "3" } } } };

	auto it = level_order_tree_iterator<decltype(t.begin())>(t.begin());

	BOOST_CHECK(it->front() == '0');
}

BOOST_AUTO_TEST_CASE(level_order_tree_iterator__reference__4)
{
	using namespace wordring;

	tree<std::string> t{ { "0", {{ "1", { "4", "5" } }, { "2", { "6" } }, { "3" } } } };

	auto it = level_order_tree_iterator<decltype(t.begin())>(t.begin());
	it->front() = '9';

	BOOST_CHECK(*it == "9");
}

// basic_level_order_tree_iterator& operator++()
BOOST_AUTO_TEST_CASE(level_order_tree_iterator__increment__1)
{
	using namespace wordring;

	tree<char> t{ { '0', {{ '1', { '4', '5' } }, { '2', { '6' } }, { '3' } } } };

	auto it1 = level_order_tree_iterator<decltype(t.begin())>(t.begin());
	auto it2 = level_order_tree_iterator<decltype(t.begin())>();

	std::string s;
	std::copy(it1, it2, std::back_inserter(s));

	BOOST_CHECK(s == "0123456");
}

BOOST_AUTO_TEST_CASE(level_order_tree_iterator__increment__2)
{
	using namespace wordring;

	tree<char> const t{ { '0', {{ '1', { '4', '5' } }, { '2', { '6' } }, { '3' } } } };

	auto it1 = level_order_tree_iterator<decltype(t.begin())>(t.begin());
	auto it2 = level_order_tree_iterator<decltype(t.begin())>();

	std::string s;
	std::copy(it1, it2, std::back_inserter(s));

	BOOST_CHECK(s == "0123456");
}

BOOST_AUTO_TEST_CASE(level_order_tree_iterator__increment__3)
{
	using namespace wordring;

	tree<char> const t{ { '0', {{ '1', { '4', '5' } }, { '2', { '6' } }, { '3' } } } };

	auto it1 = level_order_tree_iterator<decltype(t.begin())>(t.begin().begin(), t.begin().end());
	auto it2 = level_order_tree_iterator<decltype(t.begin())>();

	std::string s;
	std::copy(it1, it2, std::back_inserter(s));

	BOOST_CHECK(s == "123456");
}

// base_type operator++(int)
BOOST_AUTO_TEST_CASE(level_order_tree_iterator__increment__4)
{
	using namespace wordring;

	tree<char> t{ { '0', {{ '1', { '4', '5' } }, { '2', { '6' } }, { '3' } } } };

	auto it1 = level_order_tree_iterator<decltype(t.begin())>(t.begin());
	auto it2 = level_order_tree_iterator<decltype(t.begin())>();

	std::string s;
	while(it1 != it2) s.push_back(*it1++);

	BOOST_CHECK(s == "0123456");
}

BOOST_AUTO_TEST_CASE(level_order_tree_iterator__increment__5)
{
	using namespace wordring;

	tree<char> const t{ { '0', {{ '1', { '4', '5' } }, { '2', { '6' } }, { '3' } } } };

	auto it1 = level_order_tree_iterator<decltype(t.begin())>(t.begin());
	auto it2 = level_order_tree_iterator<decltype(t.begin())>();

	std::string s;
	while(it1 != it2) s.push_back(*it1++);

	BOOST_CHECK(s == "0123456");
}

BOOST_AUTO_TEST_CASE(level_order_tree_iterator__increment__6)
{
	using namespace wordring;

	tree<char> const t{ { '0', {{ '1', { '4', '5' } }, { '2', { '6' } }, { '3' } } } };

	auto it1 = level_order_tree_iterator<decltype(t.begin())>(t.begin().begin(), t.begin().end());
	auto it2 = level_order_tree_iterator<decltype(t.begin())>();

	std::string s;
	while(it1 != it2) s.push_back(*it1++);

	BOOST_CHECK(s == "123456");
}

// inline bool operator==(basic_level_order_tree_iterator const& lhs, basic_level_order_tree_iterator const& rhs)

BOOST_AUTO_TEST_CASE(level_order_tree_iterator__equal__1)
{
	using namespace wordring;

	tree<char> t{ { '0', {{ '1', { '4', '5' } }, { '2', { '6' } }, { '3' } } } };

	auto it1 = level_order_tree_iterator<decltype(t.begin())>(t.begin());
	auto it2 = level_order_tree_iterator<decltype(t.begin())>(t.begin());

	BOOST_CHECK(it1 == it2);
}

BOOST_AUTO_TEST_CASE(level_order_tree_iterator__equal__2)
{
	using namespace wordring;

	tree<char> const t{ { '0', {{ '1', { '4', '5' } }, { '2', { '6' } }, { '3' } } } };

	auto it1 = level_order_tree_iterator<decltype(t.begin())>();
	auto it2 = level_order_tree_iterator<decltype(t.begin())>();

	BOOST_CHECK(it1 == it2);
}

BOOST_AUTO_TEST_CASE(level_order_tree_iterator__equal__3)
{
	using namespace wordring;

	tree<char> const t{ { '0', {{ '1', { '4', '5' } }, { '2', { '6' } }, { '3' } } } };

	auto it1 = level_order_tree_iterator<decltype(t.begin())>(t.begin());
	auto it2 = level_order_tree_iterator<decltype(t.begin())>();

	BOOST_CHECK((it1 == it2) == false);
}

BOOST_AUTO_TEST_CASE(level_order_tree_iterator__equal__4)
{
	using namespace wordring;

	tree<char> const t{ { '0', {{ '1', { '4', '5' } }, { '2', { '6' } }, { '3' } } } };

	auto it1 = level_order_tree_iterator<decltype(t.begin())>();
	auto it2 = level_order_tree_iterator<decltype(t.begin())>(t.begin());

	BOOST_CHECK((it1 == it2) == false);
}

// inline bool operator!=(basic_level_order_tree_iterator const& lhs, basic_level_order_tree_iterator const& rhs)

BOOST_AUTO_TEST_CASE(level_order_tree_iterator__not_equal__1)
{
	using namespace wordring;

	tree<char> const t{ { '0', {{ '1', { '4', '5' } }, { '2', { '6' } }, { '3' } } } };

	auto it1 = level_order_tree_iterator<decltype(t.begin())>(t.begin());
	auto it2 = level_order_tree_iterator<decltype(t.begin())>(t.begin());

	BOOST_CHECK((it1 != it2) == false);
}

BOOST_AUTO_TEST_CASE(level_order_tree_iterator__not_equal__2)
{
	using namespace wordring;

	tree<char> const t{ { '0', {{ '1', { '4', '5' } }, { '2', { '6' } }, { '3' } } } };

	auto it1 = level_order_tree_iterator<decltype(t.begin())>();
	auto it2 = level_order_tree_iterator<decltype(t.begin())>();

	BOOST_CHECK((it1 != it2) == false);
}

BOOST_AUTO_TEST_CASE(level_order_tree_iterator__not_equal__3)
{
	using namespace wordring;

	tree<char> const t{ { '0', {{ '1', { '4', '5' } }, { '2', { '6' } }, { '3' } } } };

	auto it1 = level_order_tree_iterator<decltype(t.begin())>(t.begin());
	auto it2 = level_order_tree_iterator<decltype(t.begin())>();

	BOOST_CHECK(it1 != it2);
}

BOOST_AUTO_TEST_CASE(level_order_tree_iterator__not_equal__4)
{
	using namespace wordring;

	tree<char> const t{ { '0', {{ '1', { '4', '5' } }, { '2', { '6' } }, { '3' } } } };

	auto it1 = level_order_tree_iterator<decltype(t.begin())>();
	auto it2 = level_order_tree_iterator<decltype(t.begin())>(t.begin());

	BOOST_CHECK(it1 != it2);
}

BOOST_AUTO_TEST_SUITE_END()
