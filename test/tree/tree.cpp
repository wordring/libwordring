// test/tree/tree.cpp

#include <boost/test/unit_test.hpp>

#include <iterator>
#include <list>
#include <type_traits>

#include <wordring/tree/tree.hpp>

BOOST_AUTO_TEST_SUITE(tree__test)

// explicit basic_tree()
BOOST_AUTO_TEST_CASE(tree__construct__1)
{
	wordring::tree<int> t{};
	BOOST_CHECK(*t == 0);
	BOOST_CHECK(t.begin() == t.end());
}

// basic_tree(element_type const&, allocator_type const&)
BOOST_AUTO_TEST_CASE(tree__construct__2)
{
	int i = 1;
	wordring::tree<int> t{ i };
	BOOST_CHECK(*t == 1);
	BOOST_CHECK(t.begin() == t.end());
}

// basic_tree(element_type&&, allocator_type const&)
BOOST_AUTO_TEST_CASE(tree__construct__3)
{
	wordring::tree<int> t{ 1 };
	BOOST_CHECK(*t == 1);
	BOOST_CHECK(t.begin() == t.end());
}

// basic_tree(element_type const&, InputIterator, InputIterator, allocator_type const&)
BOOST_AUTO_TEST_CASE(tree__construct__4)
{
	int const i{ 1 };
	std::list<int> l{ 1, 2 };
	wordring::tree<int> t(i, l.begin(), l.end());
	BOOST_CHECK(*t == 1);
	BOOST_CHECK(std::distance(t.begin(), t.end()) == 2);
}

// basic_tree(basic_tree const&)
BOOST_AUTO_TEST_CASE(tree__construct__5)
{
	wordring::tree<int> const t1{ 1 };
	wordring::tree<int> t2{ t1 };
	BOOST_CHECK(*t2 == 1);
	BOOST_CHECK(t2.begin() == t2.end());
}

// basic_tree(basic_tree&&)
BOOST_AUTO_TEST_CASE(tree__construct__6)
{
	wordring::tree<int> t2{ wordring::tree<int>{ 1 } };
	BOOST_CHECK(*t2 == 1);
	BOOST_CHECK(t2.begin() == t2.end());
}

// basic_tree(element_type const&, std::initializer_list<element_type>, allocator_type const&)
BOOST_AUTO_TEST_CASE(tree__construct__7)
{
	int const i{ 1 };
	wordring::tree<int> t{ i, { 1, 2, 3, 4 } };
	BOOST_CHECK(*t == 1);
	BOOST_CHECK(t.begin() != t.end());
}

// basic_tree(element_type const&, std::initializer_list<this_type>, allocator_type const&)
BOOST_AUTO_TEST_CASE(tree__construct__8)
{
	int const i{ 1 };
	wordring::tree<int> t { i, { { 2, { 1, 2 }}, { 3, { 3, 4 }} } };

	BOOST_CHECK(*t == 1);
	BOOST_CHECK(t.begin() != t.end());
}

// basic_tree& operator=(basic_tree const&)
BOOST_AUTO_TEST_CASE(tree__assign__1)
{
	wordring::tree<int> const t1{ 1, { 1, 2, 3, 4 } };
	wordring::tree<int> t2{};

	t2 = t1;

	BOOST_CHECK(*t2 == 1);
	BOOST_CHECK(t2.begin() != t2.end());
}

// basic_tree& operator=(basic_tree&&)
BOOST_AUTO_TEST_CASE(tree__assign__2)
{
	wordring::tree<int> t1{ 1, { 1, 2, 3, 4 } };
	wordring::tree<int> t2{};

	t2 = std::move(t1);

	BOOST_CHECK(*t2 == 1);
	BOOST_CHECK(t2.begin() != t2.end());
}

// void assign(InputIterator first, InputIterator last)
BOOST_AUTO_TEST_CASE(tree__assign__3)
{
	wordring::tree<int> const t1{ 1, { { 2, { 1, 2 }}, { 3, { 3, 4 }} } };
	wordring::tree<int> t2{};
	t2.assign(t1.begin(), t1.end());
	BOOST_CHECK(**t2.begin() == 2);
	BOOST_CHECK(**std::next(t2.begin()) == 3);
	BOOST_CHECK(std::distance(t1.begin(), t1.end()) == std::distance(t2.begin(), t2.end()));

	std::list l{ 1, 2 };
	t2.assign(l.begin(), l.end());
	BOOST_CHECK(**t2.begin() == 1);
	BOOST_CHECK(**std::next(t2.begin()) == 2);
	BOOST_CHECK(std::distance(t2.begin(), t2.end()) == 2);
}

// void assign(std::initializer_list<element_type>)
BOOST_AUTO_TEST_CASE(tree__assign__4)
{
	wordring::tree<int> t{ 1, { 1, 2, 3, 4, 5 } };
	t.assign({ 1, 2 });
	BOOST_CHECK(**t.begin() == 1);
	BOOST_CHECK(**std::next(t.begin()) == 2);
	BOOST_CHECK(std::distance(t.begin(), t.end()) == 2);
}

// void assign(std::initializer_list<this_type>)
BOOST_AUTO_TEST_CASE(tree__assign__5)
{
	wordring::tree<int> t{ 1, { 1, 2, 3, 4, 5 } };
	t.assign({ { 1, { 1, 2 } }, { 2, { 1, 2 } } });
	BOOST_CHECK(**t.begin() == 1);
	BOOST_CHECK(**std::next(t.begin()) == 2);
	BOOST_CHECK(std::distance(t.begin(), t.end()) == 2);
}

// allocator_type get_allocator() const

// 要素アクセス --------------------------------------------------------

// reference front()
BOOST_AUTO_TEST_CASE(tree__front__1)
{
	wordring::tree<int> t{ 2, { 1, 2, 3, 4, 5 } };

	t.front() = 10;
	BOOST_CHECK(**t.begin() == 10);
}

// const_reference front() const
BOOST_AUTO_TEST_CASE(tree__front__2)
{
	wordring::tree<int> const t{ 2, { 1, 2, 3, 4, 5 } };

	BOOST_CHECK(*t.front() == 1);
}

// reference back()
BOOST_AUTO_TEST_CASE(tree__back__1)
{
	wordring::tree<int> t{ 2, { 1, 2, 3, 4, 5 } };

	t.back() = 10;
	BOOST_CHECK(**std::prev(t.end()) == 10);
}

// const_reference back() const
BOOST_AUTO_TEST_CASE(tree__back__2)
{
	wordring::tree<int> const t{ 2, { 1, 2, 3, 4, 5 } };

	BOOST_CHECK(*t.back() == 5);
}

// イテレータ ----------------------------------------------------------

// iterator begin() noexcept
BOOST_AUTO_TEST_CASE(tree__begin__1)
{
	wordring::tree<int> t{ 1, { 2, 3, 4, 5, 6 } };

	*t.begin() = 10;
	BOOST_CHECK(**t.begin() == 10);
}

// const_iterator begin() const noexcept
BOOST_AUTO_TEST_CASE(tree__begin__2)
{
	wordring::tree<int> const t{ 1, { 2, 3, 4, 5, 6 } };
	BOOST_CHECK((std::is_same_v<decltype(t.begin()), typename wordring::tree<int>::const_iterator>));
	BOOST_CHECK(**t.begin() == 2);
}

// const_iterator cbegin() const noexcept
BOOST_AUTO_TEST_CASE(tree__cbegin__1)
{
	wordring::tree<int> t{ 1, { 2, 3, 4, 5, 6 } };
	BOOST_CHECK((std::is_same_v<decltype(t.cbegin()), typename wordring::tree<int>::const_iterator>));
	BOOST_CHECK(**t.cbegin() == 2);
}

// iterator end() noexcept
BOOST_AUTO_TEST_CASE(tree__end__1)
{
	wordring::tree<int> t{ 1, { 2, 3, 4, 5, 6 } };

	*std::prev(t.end()) = 10;
	BOOST_CHECK(**std::prev(t.end()) == 10);
}

// const_iterator end() const noexcept
BOOST_AUTO_TEST_CASE(tree__end__2)
{
	wordring::tree<int> const t{ 1, { 2, 3, 4, 5, 6 } };
	BOOST_CHECK((std::is_same_v<decltype(t.end()), typename wordring::tree<int>::const_iterator>));
	BOOST_CHECK(**std::prev(t.end()) == 6);
}

// const_iterator cend() const noexcept
BOOST_AUTO_TEST_CASE(tree__cend__1)
{
	wordring::tree<int> t{ 1, { 2, 3, 4, 5, 6 } };
	BOOST_CHECK((std::is_same_v<decltype(t.cend()), typename wordring::tree<int>::const_iterator>));
	BOOST_CHECK(**std::prev(t.cend()) == 6);
}

// reverse_iterator rbegin() noexcept
BOOST_AUTO_TEST_CASE(tree__rbegin__1)
{
	wordring::tree<int> t{ 1, { 2, 3, 4, 5, 6 } };

	*t.rbegin() = 10;
	BOOST_CHECK(**t.rbegin() == 10);
}

// const_reverse_iterator rbegin() const noexcept
BOOST_AUTO_TEST_CASE(tree__rbegin__2)
{
	wordring::tree<int> const t{ 1, { 2, 3, 4, 5, 6 } };
	BOOST_CHECK((std::is_same_v<decltype(t.rbegin()), typename wordring::tree<int>::const_reverse_iterator>));
	BOOST_CHECK(**t.rbegin() == 6);
}

// const_reverse_iterator crbegin() const noexcept
BOOST_AUTO_TEST_CASE(tree__crbegin__1)
{
	wordring::tree<int> t{ 1, { 2, 3, 4, 5, 6 } };
	BOOST_CHECK((std::is_same_v<decltype(t.crbegin()), typename wordring::tree<int>::const_reverse_iterator>));
	BOOST_CHECK(**t.crbegin() == 6);
}

// reverse_iterator rend() noexcept
BOOST_AUTO_TEST_CASE(tree__rend__1)
{
	wordring::tree<int> t{ 1, { 2, 3, 4, 5, 6 } };

	*std::prev(t.rend()) = 10;
	BOOST_CHECK(**std::prev(t.rend()) == 10);
}

// const_reverse_iterator rend()
BOOST_AUTO_TEST_CASE(tree__rend__2)
{
	wordring::tree<int> const t{ 1, { 2, 3, 4, 5, 6 } };
	BOOST_CHECK((std::is_same_v<decltype(t.rend()), typename wordring::tree<int>::const_reverse_iterator>));
	BOOST_CHECK(**std::prev(t.rend()) == 2);
}

// const_iterator crend() const noexcept
BOOST_AUTO_TEST_CASE(tree__crend__1)
{
	wordring::tree<int> t{ 1, { 2, 3, 4, 5, 6 } };
	BOOST_CHECK((std::is_same_v<decltype(t.crend()), typename wordring::tree<int>::const_reverse_iterator>));
	BOOST_CHECK(**std::prev(t.crend()) == 2);
}

// 容量 ---------------------------------------------------------------

// bool empty() const noexcept
BOOST_AUTO_TEST_CASE(tree__empty__1)
{
	BOOST_CHECK(wordring::tree<int>().empty() == true);
	BOOST_CHECK(wordring::tree<int>( 0, { 1 } ).empty() == false);
}

// size_type size() const noexcept
BOOST_AUTO_TEST_CASE(tree__size__1)
{
	BOOST_CHECK(wordring::tree<int>().size() == 0);
	BOOST_CHECK(wordring::tree<int>(0, { 1 }).size() == 1);
}

// size_type max_size() const noexcept
BOOST_AUTO_TEST_CASE(tree__max_size__1)
{
	BOOST_CHECK(0 < wordring::tree<int>().max_size());
}

// 変更 ---------------------------------------------------------------

// void clear() noexcept
BOOST_AUTO_TEST_CASE(tree__clear__1)
{
	wordring::tree<int> t{ 1, { 2, 3, 4, 5, 6 } };
	t.clear();
	BOOST_CHECK(t.begin() == t.end());
}

// iterator insert(const_iterator, this_type const&)
BOOST_AUTO_TEST_CASE(tree__insert__1)
{
	wordring::tree<int> const t1{ 1 };
	wordring::tree<int> t2{}, t3{};

	t2.insert(t2.begin(), t1);
	BOOST_CHECK(**t2.begin() == 1);

	int const i = 2;
	t3.insert(t3.begin(), i);
	BOOST_CHECK(**t3.begin() == 2);
}

// iterator insert(const_iterator, this_type&&)
BOOST_AUTO_TEST_CASE(tree__insert__2)
{
	wordring::tree<int> t1{ 1 }, t2{};

	t2.insert(t2.begin(), std::move(t1));
	BOOST_CHECK(**t2.begin() == 1);
}

// iterator insert(const_iterator, size_type, this_type const&)
BOOST_AUTO_TEST_CASE(tree__insert__3)
{
	wordring::tree<int> t1{ 1 }, t2{};

	t2.insert(t2.begin(), 5, t1);
	BOOST_CHECK(std::distance(t2.begin(), t2.end()) == 5);
	BOOST_CHECK(**t2.begin() == 1);
	BOOST_CHECK(**std::prev(t2.end()) == 1);
}

// iterator insert(const_iterator, InputIterator, InputIterator)
BOOST_AUTO_TEST_CASE(tree__insert__4)
{
	wordring::tree<int> t{};
	std::list l{ 1, 2, 3, 4, 5 };

	t.insert(t.begin(), l.begin(), l.end());
	BOOST_CHECK(std::distance(t.begin(), t.end()) == 5);
	BOOST_CHECK(**t.begin() == 1);
	BOOST_CHECK(**std::prev(t.end()) == 5);
}

// iterator insert(const_iterator, std::initializer_list<this_type>)
BOOST_AUTO_TEST_CASE(tree__insert__5)
{
	wordring::tree<int> t{};

	t.insert(t.begin(), { { 0, { 1, 2, 3, 4, 5 } }, { 1, { 1, 2, 3, 4, 5 } } });
	BOOST_CHECK(std::distance(t.begin(), t.end()) == 2);
	BOOST_CHECK(**t.begin() == 0);
	BOOST_CHECK(**std::prev(t.end()) == 1);
}

// iterator insert(const_iterator, std::initializer_list<element_type>)
BOOST_AUTO_TEST_CASE(tree__insert__6)
{
	wordring::tree<int> t{};

	t.insert(t.begin(), { 1, 2, 3, 4, 5 });
	BOOST_CHECK(std::distance(t.begin(), t.end()) == 5);
	BOOST_CHECK(**t.begin() == 1);
	BOOST_CHECK(**std::prev(t.end()) == 5);
}

// iterator emplace(const_iterator, Args&&...)
BOOST_AUTO_TEST_CASE(tree__emplace__1)
{
	wordring::tree<int> t{};

	t.emplace(t.begin(), 1);
	BOOST_CHECK(**t.begin() == 1);
}

// iterator erase(const_iterator)
BOOST_AUTO_TEST_CASE(tree__erase__1)
{
	wordring::tree<int> t{ 0, { 1, 2, 3, 4, 5 } };

	t.erase(t.cbegin());
	BOOST_CHECK(std::distance(t.begin(), t.end()) == 4);
	BOOST_CHECK(**t.begin() == 2);
}

// iterator erase(const_iterator, const_iterator)
BOOST_AUTO_TEST_CASE(tree__erase__2)
{
	wordring::tree<int> t{ 0, { 1, 2, 3, 4, 5 } };

	t.erase(t.cbegin(), std::next(t.cbegin(), 2));
	BOOST_CHECK(std::distance(t.begin(), t.end()) == 3);
	BOOST_CHECK(**t.begin() == 3);
}

// void push_back(this_type const&)
BOOST_AUTO_TEST_CASE(tree__push_back__1)
{
	wordring::tree<int> const t1{ 0, { 1, 2, 3, 4, 5 } };
	wordring::tree<int> t2{}, t3{};

	t2.push_back(t1);
	BOOST_CHECK(std::distance(t2.begin(), t2.end()) == 1);
	BOOST_CHECK(**t2.begin() == 0);

	int const i{ 1 };
	t3.push_back(i);
	BOOST_CHECK(std::distance(t3.begin(), t3.end()) == 1);
	BOOST_CHECK(**t3.begin() == 1);
}

// void push_back(this_type&&)
BOOST_AUTO_TEST_CASE(tree__push_back__2)
{
	wordring::tree<int> const t1{ 0, { 1, 2, 3, 4, 5 } };
	wordring::tree<int> t2{}, t3{};

	t2.push_back(std::move(t1));
	BOOST_CHECK(std::distance(t2.begin(), t2.end()) == 1);
	BOOST_CHECK(**t2.begin() == 0);

	int const i{ 1 };
	t3.push_back(std::move(i));
	BOOST_CHECK(std::distance(t3.begin(), t3.end()) == 1);
	BOOST_CHECK(**t3.begin() == 1);
}

// reference emplace_back(Args&&...)
BOOST_AUTO_TEST_CASE(tree__emplace_back__1)
{
	wordring::tree<int> t{};

	t.emplace_back(1);
	BOOST_CHECK(std::distance(t.begin(), t.end()) == 1);
	BOOST_CHECK(**t.begin() == 1);
}

// void pop_back()
BOOST_AUTO_TEST_CASE(tree__pop_back__1)
{
	wordring::tree<int> t{ 0, { 1, 2, 3, 4, 5 } };

	t.pop_back();
	BOOST_CHECK(std::distance(t.begin(), t.end()) == 4);
	BOOST_CHECK(**std::prev(t.end()) == 4);
}




BOOST_AUTO_TEST_SUITE_END()
