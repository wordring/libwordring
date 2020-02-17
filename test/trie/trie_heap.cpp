// test/trie/trie_heap.cpp

#include <boost/test/unit_test.hpp>

#include <wordring/trie/trie_heap.hpp>

#include <sstream>

namespace
{
	using base_node = wordring::trie_node;
	using base_iterator = wordring::trie_heap_serialize_iterator<std::vector<base_node, std::allocator<base_node>> const>;
	using base_heap = wordring::trie_heap< std::allocator<base_node>>;

	class test_iterator : public base_iterator
	{
	public:
		using base_type = base_iterator;

		using base_type::container;

		using base_type::trie_heap_serialize_iterator;

		test_iterator(container const& c, std::uint32_t n)
			: base_type(c, n)
		{
		}
	};

	class test_heap : public base_heap
	{
	public:
		using base_type = base_heap;

		using base_type::assign;
		using base_type::trie_heap;
		using base_type::reserve;
		using base_type::allocate;
		using base_type::free;
		using base_type::locate;
		using base_type::is_free;

		using base_type::m_c;

	public:
		test_heap() = default;

		test_heap(std::initializer_list<base_node> il)
			: base_type(il)
		{
		}
	};
}

BOOST_AUTO_TEST_SUITE(trie_heap__test)

// ----------------------------------------------------------------------------
// trie_node
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(trie_node__construct__1)
{
	using namespace wordring;

	static_assert(std::is_pod_v<trie_node>);
}

// ----------------------------------------------------------------------------
// trie_heap_serialize_iterator
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(trie_heap_serialize_iterator__construct__1)
{
	test_heap heap{};
	auto it1 = test_iterator(heap.m_c, 0);
	auto it2 = test_iterator(heap.m_c, 1);

	BOOST_CHECK(it1 != it2);
}

// ----------------------------------------------------------------------------
// trie_heap
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(trie_heap__construct__1)
{
	test_heap heap{};
}

BOOST_AUTO_TEST_CASE(trie_heap__construct__2)
{
	test_heap heap({ { 0, 1 }, { 2, 3 } });
}

// void assign(InputIterator first, InputIterator last)
BOOST_AUTO_TEST_CASE(trie_heap__assign__1)
{
	using namespace wordring;

	test_heap heap{};
	std::vector<std::uint32_t> v1{ 0x01020304, 0x05060708 };

	heap.assign(v1.begin(), v1.end());

	std::vector<trie_node> v2{ { 0x01020304, 0x05060708 } };
	BOOST_CHECK(heap.m_c == v2);
}

BOOST_AUTO_TEST_CASE(trie_heap__assign__2)
{
	using namespace wordring;

	test_heap heap{};
	std::vector<std::uint32_t> v1{};

	heap.assign(v1.begin(), v1.end());

	std::vector<trie_node> v2{};
	BOOST_CHECK(heap.m_c == v2);
}

BOOST_AUTO_TEST_CASE(trie_heap__assign__3)
{
	using namespace wordring;

	test_heap heap{};
	std::vector<std::uint8_t> v1{ 1, 2, 3, 4, 5, 6, 7, 8 };

	heap.assign(v1.begin(), v1.end());

	std::vector<trie_node> v2{ { 0x01020304, 0x05060708 } };
	BOOST_CHECK(heap.m_c == v2);
}

BOOST_AUTO_TEST_CASE(trie_heap__assign__4)
{
	using namespace wordring;

	test_heap heap{};
	std::vector<std::uint8_t> v1{};

	heap.assign(v1.begin(), v1.end());

	std::vector<trie_node> v2{};
	BOOST_CHECK(heap.m_c == v2);
}

// 内部 -----------------------------------------------------------------------

// void reserve(std::size_t)
BOOST_AUTO_TEST_CASE(trie_heap__reserve__1)
{
	std::vector<int> v(1024, 0);
	std::size_t i1 = v.size();
	std::size_t i2 = v.capacity();

	v.reserve(1024);

	std::size_t i3 = v.size();
	std::size_t i4 = v.capacity();

	BOOST_CHECK(i1 <= i2);
	BOOST_CHECK(i3 <= i4);
}

BOOST_AUTO_TEST_CASE(trie_heap__reserve__2)
{
	using namespace wordring;

	test_heap heap{};
	heap.reserve(0);

	std::vector<trie_node> v{ { 0, 0 }, { 0, 0 } };
	BOOST_CHECK(heap.m_c == v);
}

BOOST_AUTO_TEST_CASE(trie_heap__reserve__3)
{
	using namespace wordring;

	test_heap heap{};
	heap.reserve(1);

	std::vector<trie_node> v{ { 0, -2 }, { 0, 0 }, { 0, 0 } };
	BOOST_CHECK(heap.m_c == v);
}

BOOST_AUTO_TEST_CASE(trie_heap__reserve__4)
{
	using namespace wordring;

	test_heap heap{};
	heap.reserve(2);

	std::vector<trie_node> v{ { 0, -2 }, { 0, 0 }, { 0, -3 }, { 0, 0 } };
	BOOST_CHECK(heap.m_c == v);
}

BOOST_AUTO_TEST_CASE(trie_heap__reserve__5)
{
	using namespace wordring;

	test_heap heap{};
	heap.m_c = { { 0, -2 }, { 0, 0 }, { 0, 0 }, { 0, 0 } };
	heap.reserve(1);

	std::vector<trie_node> v{ { 0, -2 }, { 0, 0 }, { 0, -4 }, { 0, 0 }, { 0, 0 } };
	BOOST_CHECK(heap.m_c.size() == 5);
	BOOST_CHECK(heap.m_c == v);
}

BOOST_AUTO_TEST_CASE(trie_heap__reserve__6)
{
	using namespace wordring;

	test_heap heap{};
	heap.m_c = { { 0, -2 }, { 0, 0 }, { 0, 0 }, { 0, 0 } };
	heap.reserve(2);

	std::vector<trie_node> v{ { 0, -2 }, { 0, 0 }, { 0, -4 }, { 0, 0 }, { 0, -5 }, { 0, 0 } };
	BOOST_CHECK(heap.m_c.size() == 6);
	BOOST_CHECK(heap.m_c == v);
}

BOOST_AUTO_TEST_CASE(trie_heap__reserve__7)
{
	using namespace wordring;

	test_heap heap{};
	heap.m_c = { { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } };
	heap.reserve(1);

	std::vector<trie_node> v{ { 0, -4 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } };
	BOOST_CHECK(heap.m_c.size() == 5);
	BOOST_CHECK(heap.m_c == v);
}

BOOST_AUTO_TEST_CASE(trie_heap__reserve__8)
{
	using namespace wordring;

	test_heap heap{};
	heap.m_c = { { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } };
	heap.reserve(2);

	std::vector<trie_node> v{ { 0, -4 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, -5 }, { 0, 0 } };
	BOOST_CHECK(heap.m_c.size() == 6);
	BOOST_CHECK(heap.m_c == v);
}

// void allocate(index_type id)
BOOST_AUTO_TEST_CASE(trie_heap__allocate__1)
{
	using namespace wordring;

	test_heap heap{};
	heap.m_c = { { 0, -2 }, { 0, 0 }, { 0, 0 } };

	heap.allocate(2);

	std::vector<trie_node> v{ { 0, 0 }, { 0, 0 }, { 0, 0 } };
	BOOST_CHECK(heap.m_c == v);
}

BOOST_AUTO_TEST_CASE(trie_heap__allocate__2)
{
	using namespace wordring;

	test_heap heap{};
	heap.m_c = { { 0, -3 }, { 0, 0 }, { 0, 0 }, { 0, -4 }, { 0, 0 } };

	heap.allocate(3);

	std::vector<trie_node> v{ { 0, -4 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } };
	BOOST_CHECK(heap.m_c == v);
}

BOOST_AUTO_TEST_CASE(trie_heap__allocate__3)
{
	using namespace wordring;

	test_heap heap{};
	heap.m_c = { { 0, -1 }, { 0, -2 }, { 0, -3 }, { 0, 0 } };

	heap.allocate(2);

	std::vector<trie_node> v{ { 0, -1 }, { 0, -3 }, { 0, 0 }, { 0, 0 } };
	BOOST_CHECK(heap.m_c == v);
}

// void allocate(index_type base, label_vector const& labels)
BOOST_AUTO_TEST_CASE(trie_heap__allocate__4)
{
	using namespace wordring;

	test_heap heap{};
	heap.m_c = { { 0, -1 }, { 0, 0 } };

	static_vector<std::uint16_t, 257> sv = { 0 };
	heap.allocate(1, sv);

	std::vector<trie_node> v{ { 0, 0 }, { 0, 0 } };
	BOOST_CHECK(heap.m_c == v);
}

BOOST_AUTO_TEST_CASE(trie_heap__allocate__5)
{
	using namespace wordring;

	test_heap heap{};
	heap.m_c = { { 0, -1 }, { 0, -2 }, { 0, -3 }, { 0, -4 }, { 0, 0 } };

	heap.allocate(1, { 0, 2 });

	std::vector<trie_node> v{ { 0, -2 }, { 0, 0 }, { 0, -4 }, { 0, 0 }, { 0, 0 } };
	BOOST_CHECK(heap.m_c == v);
}

BOOST_AUTO_TEST_CASE(trie_heap__allocate__6)
{
	using namespace wordring;

	test_heap heap{};
	heap.m_c = { { 0, 0 } };

	static_vector<std::uint16_t, 257> sv = { 0 };
	heap.allocate(1, sv);

	std::vector<trie_node> v{ { 0, 0 }, { 0, 0 } };
	BOOST_CHECK(heap.m_c == v);
}

BOOST_AUTO_TEST_CASE(trie_heap__allocate__7)
{
	using namespace wordring;

	test_heap heap{};
	heap.m_c = { { 0, 0 } };

	heap.allocate(1, { 0, 2 });

	std::vector<trie_node> v{ { 0, -2 }, { 0, 0 }, { 0, 0 }, { 0, 0 } };
	BOOST_CHECK(heap.m_c == v);
}

BOOST_AUTO_TEST_CASE(trie_heap__free__0)
{
	using namespace wordring;
	std::vector<std::string> v{ "1" };

	BOOST_CHECK(std::is_sorted(v.begin(), v.end()));
}

// index_type free(index_type id, index_type before = 0)
BOOST_AUTO_TEST_CASE(trie_heap__free__1)
{
	using namespace wordring;

	test_heap heap{};
	heap.m_c = { { 0, 0 }, { 0, 0 }, { 0, 9 } };

	heap.free(2);

	std::vector<trie_node> v{ { 0, -2 }, { 0, 0 }, { 0, 0 } };
	BOOST_CHECK(heap.m_c == v);
}

BOOST_AUTO_TEST_CASE(trie_heap__free__2)
{
	using namespace wordring;

	test_heap heap{};
	heap.m_c = { { 0, -1 }, { 0, -3 }, { 0, 9 }, { 0, 0 } };

	heap.free(2);

	std::vector<trie_node> v{ { 0, -1 }, { 0, -2 }, { 0, -3 }, { 0, 0 } };
	BOOST_CHECK(heap.m_c == v);
}

BOOST_AUTO_TEST_CASE(trie_heap__free__3)
{
	using namespace wordring;

	test_heap heap{};
	heap.m_c = { { 0, -2 }, { 0, 9 }, { 0, 0 }, { 0, 9 } };

	heap.free(3);

	std::vector<trie_node> v{ { 0, -2 }, { 0, 9 }, { 0, -3 }, { 0, 0 } };
	BOOST_CHECK(heap.m_c == v);
}

// void free(index_type base, label_vector const& labels)
BOOST_AUTO_TEST_CASE(trie_heap__free__4)
{
	using namespace wordring;

	test_heap heap{};
	heap.m_c = { { 0, 0 }, { 0, 0 }, { 0, 9 } };

	static_vector<std::uint16_t, 257> labels{ 0 };
	heap.free(2, labels);

	std::vector<trie_node> v{ { 0, -2 }, { 0, 0 }, { 0, 0 } };
	BOOST_CHECK(heap.m_c == v);
}

BOOST_AUTO_TEST_CASE(trie_heap__free__5)
{
	using namespace wordring;

	test_heap heap{};
	heap.m_c = { { 0, -4 }, { 0, 0 }, { 0, 9 }, { 0, 9 }, { 0, 0 } };

	heap.free(2, { 0, 1 });

	std::vector<trie_node> v{ { 0, -2 }, { 0, 0 }, { 0, -3 }, { 0, -4 }, { 0, 0 } };
	BOOST_CHECK(heap.m_c == v);
}

BOOST_AUTO_TEST_CASE(trie_heap__free__6)
{
	using namespace wordring;

	test_heap heap{};
	heap.m_c = { { 0, -3 }, { 0, 0 }, { 0, 9 }, { 0, -5 }, { 0, 9 }, { 0, 0 } };

	heap.free(2, { 0, 2 });

	std::vector<trie_node> v{ { 0, -2 }, { 0, 0 }, { 0, -3 }, { 0, -4 }, { 0, -5 }, { 0, 0 } };
	BOOST_CHECK(heap.m_c == v);
}

BOOST_AUTO_TEST_CASE(trie_heap__free__7)
{
	using namespace wordring;

	test_heap heap{};
	heap.m_c = { { 0, -3 }, { 0, 0 }, { 0, 9 }, { 0, 0 }, { 0, 9 } };

	heap.free(2, { 0, 2 });

	std::vector<trie_node> v{ { 0, -2 }, { 0, 0 }, { 0, -3 }, { 0, -4 }, { 0, 0 } };
	BOOST_CHECK(heap.m_c == v);
}

// index_type locate(label_vector const& labels, index_type& before) const
BOOST_AUTO_TEST_CASE(trie_heap__locate__1)
{
	test_heap heap{};

	std::int32_t before;
	BOOST_CHECK(heap.locate({ 0 }, before) == 2);
	BOOST_CHECK(before == 0);
}

BOOST_AUTO_TEST_CASE(trie_heap__locate__2)
{
	test_heap heap{};

	std::int32_t before;
	BOOST_CHECK(heap.locate({ 1 }, before) == 1);
	BOOST_CHECK(before == 0);
}

BOOST_AUTO_TEST_CASE(trie_heap__locate__3)
{
	using namespace wordring;

	test_heap heap{};
	heap.reserve(1);

	std::vector<trie_node> v{ { 0, -2 }, { 0, 0 }, { 0, 0 } };
	BOOST_CHECK(heap.m_c == v);

	std::int32_t before;
	BOOST_CHECK(heap.locate({ 1 }, before) == 1);
	BOOST_CHECK(before == 0);
}

BOOST_AUTO_TEST_CASE(trie_heap__locate__4)
{
	using namespace wordring;

	test_heap heap{};
	heap.reserve(2);

	std::vector<trie_node> v{ { 0, -2 }, { 0, 0 }, { 0, -3 }, { 0, 0 } };
	BOOST_CHECK(heap.m_c == v);

	std::int32_t before;
	BOOST_CHECK(heap.locate({ 1, 2 }, before) == 1);
	BOOST_CHECK(before == 0);
}

BOOST_AUTO_TEST_CASE(trie_heap__locate__5)
{
	test_heap heap{};
	heap.m_c = { { 0, -3 }, { 0, 0 }, { 0, 0 }, { 0, 0 } };

	std::int32_t before;
	BOOST_CHECK(heap.locate({ 1 }, before) == 2);
	BOOST_CHECK(before == 0);
}

BOOST_AUTO_TEST_CASE(trie_heap__locate__6)
{
	test_heap heap{};
	heap.m_c = { { 0, -3 }, { 0, 0 }, { 0, 0 }, { 0, 0 } };

	std::int32_t before;
	BOOST_CHECK(heap.locate({ 2 }, before) == 1);
	BOOST_CHECK(before == 0);
}

BOOST_AUTO_TEST_CASE(trie_heap__locate__7)
{
	test_heap heap{};
	heap.m_c = { { 0, -2 }, { 0, 0 }, { 0, -4 }, { 0, 9 }, { 0, 0 } };

	std::int32_t before;
	BOOST_CHECK(heap.locate({ 2 }, before) == 2);
	BOOST_CHECK(before == 2);
}

// bool is_free(index_type base, label_vector const& labels) const
BOOST_AUTO_TEST_CASE(trie_heap__is_free__1)
{
	test_heap heap{};
	heap.m_c = { { 0, -1 }, { 0, 0 } };

	BOOST_CHECK(heap.is_free(1, { 0 }));
}

BOOST_AUTO_TEST_CASE(trie_heap__is_free__2)
{
	test_heap heap{};
	heap.m_c = { { 0, -1 }, { 0, 1 } };

	BOOST_CHECK(heap.is_free(1, { 1 }));
}

BOOST_AUTO_TEST_CASE(trie_heap__is_free__3)
{
	test_heap heap{};
	heap.m_c = { { 0, -1 }, { 0, 1 } };

	BOOST_CHECK(heap.is_free(1, { 0 }) == false);
}

// bool is_free(index_type parent, index_type base, label_vector const& labels) const
BOOST_AUTO_TEST_CASE(trie_heap__is_free__4)
{
	test_heap heap{};
	heap.m_c = { { 0, -3 }, { 0, 0 }, { 0, 1 }, { 0, 0 } };

	BOOST_CHECK(heap.is_free(1, 1, { 1, 2 }));
}

// 関数 -----------------------------------------------------------------------

// inline std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, trie_heap<Allocator1> const& heap)
// inline std::basic_istream<char>& operator>>(std::basic_istream<char>& is, trie_heap<Allocator1>& heap)
BOOST_AUTO_TEST_CASE(trie_heap__stream__1)
{
	test_heap h1{}, h2{};
	h1.m_c = { { 1, 2 }, { 3, 4 }, { 5, 6 }, { 7, 8 } };

	std::stringstream ss;
	ss << h1;
	ss >> h2;

	BOOST_CHECK(h1.m_c == h2.m_c);
}

BOOST_AUTO_TEST_CASE(trie_heap__stream__2)
{
	test_heap h1{}, h2{};
	h1.m_c = {};

	std::stringstream ss;
	ss << h1;
	ss >> h2;

	BOOST_CHECK(h1.m_c == h2.m_c);
}

BOOST_AUTO_TEST_SUITE_END()
