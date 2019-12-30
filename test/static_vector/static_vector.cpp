// test/static_vector/static_vector.cpp

#include <boost/test/unit_test.hpp>

#include <wordring/static_vector/static_vector.hpp>

#include <array>
#include <string>
#include <vector>

BOOST_AUTO_TEST_SUITE(static_vector__test)

// ----------------------------------------------------------------------------
// static_vector_iterator
// ----------------------------------------------------------------------------

// static_vector_iterator()
BOOST_AUTO_TEST_CASE(static_vector_iterator__construct__1)
{
	using namespace wordring;
	auto it1 = static_vector_iterator<std::array<char, 16>>();
	auto it2 = static_vector_iterator<std::array<char, 16> const>();

	BOOST_CHECK(it1 == it2);
}

// static_vector_iterator(container& c, difference_type pos)
BOOST_AUTO_TEST_CASE(static_vector_iterator__construct__2)
{
	using namespace wordring;

	std::array<char, 16> sv;

	auto it1 = static_vector_iterator<std::array<char, 16>>(sv, 5);
	auto it2 = static_vector_iterator<std::array<char, 16> const>(sv, 5);

	BOOST_CHECK(it1 == it2);
}

// static_vector_iterator(static_vector_iterator<Container1> const& rhs)
BOOST_AUTO_TEST_CASE(static_vector_iterator__construct__3)
{
	using namespace wordring;

	std::array<char, 16> sv;

	auto it1 = static_vector_iterator<std::array<char, 16>>(sv, 0);
	auto it2 = static_vector_iterator<std::array<char, 16> const>(it1);

	BOOST_CHECK(it1 == it2);
}

// static_vector_iterator(static_vector_iterator const&)
BOOST_AUTO_TEST_CASE(static_vector_iterator__construct__4)
{
	using namespace wordring;

	std::array<char, 16> sv;

	auto it1 = static_vector_iterator<std::array<char, 16>>(sv, 5);
	auto it2 = static_vector_iterator<std::array<char, 16>>(it1);

	BOOST_CHECK(it1 == it2);

	auto it3 = static_vector_iterator<std::array<char, 16> const>(sv, 5);
	auto it4 = static_vector_iterator<std::array<char, 16> const>(it3);

	BOOST_CHECK(it3 == it4);
}

// static_vector_iterator(static_vector_iterator&&)
BOOST_AUTO_TEST_CASE(static_vector_iterator__construct__5)
{
	using namespace wordring;

	std::array<char, 16> sv;

	auto it1 = static_vector_iterator<std::array<char, 16>>(sv, 5);
	auto it2 = static_vector_iterator<std::array<char, 16>>(std::move(it1));
	auto it3 = static_vector_iterator<std::array<char, 16>>(sv, 5);

	BOOST_CHECK(it2 == it3);
}

// static_vector_iterator& operator=(static_vector_iterator<Container1> const& rhs)
BOOST_AUTO_TEST_CASE(static_vector_iterator__assign__1)
{
	using namespace wordring;

	std::array<char, 16> sv;

	auto it1 = static_vector_iterator<std::array<char, 16> const>();
	auto it2 = static_vector_iterator<std::array<char, 16>>(sv, 5);
	it1 = it2;

	BOOST_CHECK(it1 == it2);
}

// static_vector_iterator& operator=(static_vector_iterator const&)
BOOST_AUTO_TEST_CASE(static_vector_iterator__assign__2)
{
	using namespace wordring;

	std::array<char, 16> sv;

	auto it1 = static_vector_iterator<std::array<char, 16>>(sv, 5);
	auto it2 = static_vector_iterator<std::array<char, 16>>();
	it2 = it1;
	auto it3 = static_vector_iterator<std::array<char, 16>>(sv, 5);

	BOOST_CHECK(it2 == it3);

	auto it4 = static_vector_iterator<std::array<char, 16> const>(sv, 5);
	auto it5 = static_vector_iterator<std::array<char, 16> const>();
	it5 = it4;
	auto it6 = static_vector_iterator<std::array<char, 16> const>(sv, 5);

	BOOST_CHECK(it5 == it6);
}

// static_vector_iterator& operator=(static_vector_iterator&&)
BOOST_AUTO_TEST_CASE(static_vector_iterator__assign__3)
{
	using namespace wordring;

	std::array<char, 16> sv;

	auto it1 = static_vector_iterator<std::array<char, 16>>(sv, 5);
	auto it2 = static_vector_iterator<std::array<char, 16>>();
	it2 = std::move(it1);
	auto it3 = static_vector_iterator<std::array<char, 16>>(sv, 5);

	BOOST_CHECK(it2 == it3);

	auto it4 = static_vector_iterator<std::array<char, 16> const>(sv, 5);
	auto it5 = static_vector_iterator<std::array<char, 16> const>();
	it5 = std::move(it4);
	auto it6 = static_vector_iterator<std::array<char, 16> const>(sv, 5);

	BOOST_CHECK(it5 == it6);
}

// reference operator*()
BOOST_AUTO_TEST_CASE(static_vector_iterator__reference__1)
{
	using namespace wordring;

	std::array<char, 16> sv{ 1, 2, 3 };

	auto it = static_vector_iterator<std::array<char, 16>>(sv, 1);
	*it = 5;
	BOOST_CHECK(*it == 5);
}

// const_reference operator*() const
BOOST_AUTO_TEST_CASE(static_vector_iterator__reference__2)
{
	using namespace wordring;

	std::array<char, 16> const sv{ 1, 2, 3 };

	auto const it = static_vector_iterator<std::array<char, 16> const>(sv, 1);
	BOOST_CHECK(*it == 2);
}

// pointer operator->()
BOOST_AUTO_TEST_CASE(static_vector_iterator__reference__3)
{
	using namespace wordring;

	std::array<std::string, 16> sv{ "1", "2", "3" };

	auto it = static_vector_iterator<std::array<std::string, 16>>(sv, 1);
	it->assign("5");
	BOOST_CHECK(it->at(0) == '5');
}

// const_pointer operator->() const
BOOST_AUTO_TEST_CASE(static_vector_iterator__reference__4)
{
	using namespace wordring;

	std::array<std::string, 16> const sv{ "1", "2", "3" };

	auto const it = static_vector_iterator<std::array<std::string, 16> const>(sv, 1);
	BOOST_CHECK(it->at(0) == '2');
}

// reference operator[](int n)
BOOST_AUTO_TEST_CASE(static_vector_iterator__reference__5)
{
	using namespace wordring;

	std::array<std::string, 16> sv{ "1", "2", "3" };

	auto it = static_vector_iterator<std::array<std::string, 16>>(sv, 1);
	it[1] = "5";
	BOOST_CHECK(it[1] == "5");
}

// const_reference operator[](int n) const
BOOST_AUTO_TEST_CASE(static_vector_iterator__reference__6)
{
	using namespace wordring;

	std::array<std::string, 16> const sv{ "1", "2", "3" };

	auto const it = static_vector_iterator<std::array<std::string, 16> const>(sv, 1);
	BOOST_CHECK(it[1] == "3");
}

// static_vector_iterator& operator++()
BOOST_AUTO_TEST_CASE(static_vector_iterator__increment__1)
{
	using namespace wordring;

	std::array<std::string, 16> sv{ "1", "2", "3" };

	auto it1 = static_vector_iterator<std::array<std::string, 16>>(sv, 2);
	auto it2 = static_vector_iterator<std::array<std::string, 16>>(sv, 3);
	BOOST_CHECK(++it1 == it2);
}

BOOST_AUTO_TEST_CASE(static_vector_iterator__increment__2)
{
	using namespace wordring;

	std::array<std::string, 16> const sv{ "1", "2", "3" };

	auto it1 = static_vector_iterator<std::array<std::string, 16> const>(sv, 2);
	auto it2 = static_vector_iterator<std::array<std::string, 16> const>(sv, 3);
	BOOST_CHECK(++it1 == it2);
}

// static_vector_iterator operator++(int)
BOOST_AUTO_TEST_CASE(static_vector_iterator__increment__3)
{
	using namespace wordring;

	std::array<std::string, 16> sv{ "1", "2", "3" };

	auto it = static_vector_iterator<std::array<std::string, 16>>(sv, 1);
	BOOST_CHECK(*it++ == "2");
	BOOST_CHECK(*it++ == "3");
}

BOOST_AUTO_TEST_CASE(static_vector_iterator__increment__4)
{
	using namespace wordring;

	std::array<std::string, 16> const sv{ "1", "2", "3" };

	auto it = static_vector_iterator<std::array<std::string, 16> const>(sv, 1);
	BOOST_CHECK(*it++ == "2");
	BOOST_CHECK(*it++ == "3");
}

// static_vector_iterator& operator--()
BOOST_AUTO_TEST_CASE(static_vector_iterator__decrement__1)
{
	using namespace wordring;

	std::array<std::string, 16> sv{ "1", "2", "3" };

	auto it1 = static_vector_iterator<std::array<std::string, 16>>(sv, 2);
	auto it2 = static_vector_iterator<std::array<std::string, 16>>(sv, 3);
	BOOST_CHECK(it1 == --it2);
}

BOOST_AUTO_TEST_CASE(static_vector_iterator__decrement__2)
{
	using namespace wordring;

	std::array<std::string, 16> const sv{ "1", "2", "3" };

	auto it1 = static_vector_iterator<std::array<std::string, 16> const>(sv, 2);
	auto it2 = static_vector_iterator<std::array<std::string, 16> const>(sv, 3);
	BOOST_CHECK(it1 == --it2);
}

// static_vector_iterator operator--(int)
BOOST_AUTO_TEST_CASE(static_vector_iterator__decrement__3)
{
	using namespace wordring;

	std::array<std::string, 16> sv{ "1", "2", "3" };

	auto it = static_vector_iterator<std::array<std::string, 16>>(sv, 2);
	BOOST_CHECK(*it-- == "3");
	BOOST_CHECK(*it-- == "2");
	BOOST_CHECK(*it == "1");
}

BOOST_AUTO_TEST_CASE(static_vector_iterator__decrement__4)
{
	using namespace wordring;

	std::array<std::string, 16> const sv{ "1", "2", "3" };

	auto it = static_vector_iterator<std::array<std::string, 16> const>(sv, 2);
	BOOST_CHECK(*it-- == "3");
	BOOST_CHECK(*it-- == "2");
	BOOST_CHECK(*it == "1");
}

// static_vector_iterator& operator+=(size_t n)
BOOST_AUTO_TEST_CASE(static_vector_iterator__add_assign__1)
{
	using namespace wordring;

	std::array<std::string, 16> sv{ "1", "2", "3" };

	auto it = static_vector_iterator<std::array<std::string, 16>>(sv, 0);
	it += 2;
	BOOST_CHECK(*it == "3");
}

BOOST_AUTO_TEST_CASE(static_vector_iterator__add_assign__2)
{
	using namespace wordring;

	std::array<std::string, 16> const sv{ "1", "2", "3" };

	auto it = static_vector_iterator<std::array<std::string, 16> const>(sv, 0);
	it += 2;
	BOOST_CHECK(*it == "3");
}

// static_vector_iterator& operator-=(size_t n)
BOOST_AUTO_TEST_CASE(static_vector_iterator__sub_assign__1)
{
	using namespace wordring;

	std::array<std::string, 16> sv{ "1", "2", "3" };

	auto it = static_vector_iterator<std::array<std::string, 16>>(sv, 3);
	it -= 2;
	BOOST_CHECK(*it == "2");
}

BOOST_AUTO_TEST_CASE(static_vector_iterator__sub_assign__2)
{
	using namespace wordring;

	std::array<std::string, 16> const sv{ "1", "2", "3" };

	auto it = static_vector_iterator<std::array<std::string, 16> const>(sv, 3);
	it -= 2;
	BOOST_CHECK(*it == "2");
}

// inline auto operator+(static_vector_iterator<Container1> const& lhs, static_vector_iterator<Container2> const& rhs)
BOOST_AUTO_TEST_CASE(static_vector_iterator__add__1)
{
	using namespace wordring;

	std::array<std::string, 16> sv{ "1", "2", "3" };

	auto it1 = static_vector_iterator<std::array<std::string, 16>>(sv, 1);
	auto it2 = static_vector_iterator<std::array<std::string, 16> const>(sv, 2);

	BOOST_CHECK(it1 + it2 == 3);
}

// inline static_vector_iterator<Container1> operator+(static_vector_iterator<Container1> const& it, std::size_t n)
BOOST_AUTO_TEST_CASE(static_vector_iterator__add__2)
{
	using namespace wordring;

	std::array<std::string, 16> sv{ "1", "2", "3" };

	auto it = static_vector_iterator<std::array<std::string, 16>>(sv, 0);

	BOOST_CHECK(*(it + 1) == "2");
}

BOOST_AUTO_TEST_CASE(static_vector_iterator__add__3)
{
	using namespace wordring;

	std::array<std::string, 16> const sv{ "1", "2", "3" };

	auto it = static_vector_iterator<std::array<std::string, 16> const>(sv, 0);

	BOOST_CHECK(*(it + 1) == "2");
}

// inline static_vector_iterator<Container1> operator+(std::size_t n, static_vector_iterator<Container1> const& it)
BOOST_AUTO_TEST_CASE(static_vector_iterator__add__4)
{
	using namespace wordring;

	std::array<std::string, 16> sv{ "1", "2", "3" };

	auto it = static_vector_iterator<std::array<std::string, 16>>(sv, 0);

	BOOST_CHECK(*(1 + it) == "2");
}

BOOST_AUTO_TEST_CASE(static_vector_iterator__add__5)
{
	using namespace wordring;

	std::array<std::string, 16> const sv{ "1", "2", "3" };

	auto it = static_vector_iterator<std::array<std::string, 16> const>(sv, 0);

	BOOST_CHECK(*(1 + it) == "2");
}

// inline auto operator-(static_vector_iterator<Container1> const& lhs, static_vector_iterator<Container2> const& rhs)
BOOST_AUTO_TEST_CASE(static_vector_iterator__sub__1)
{
	using namespace wordring;

	std::array<std::string, 16> sv{ "1", "2", "3" };

	auto it1 = static_vector_iterator<std::array<std::string, 16>>(sv, 1);
	auto it2 = static_vector_iterator<std::array<std::string, 16> const>(sv, 2);

	BOOST_CHECK(it1 - it2 == -1);
}

// inline static_vector_iterator<Container1> operator-(static_vector_iterator<Container1> const& it, std::size_t n)
BOOST_AUTO_TEST_CASE(static_vector_iterator__sub__2)
{
	using namespace wordring;

	std::array<std::string, 16> sv{ "1", "2", "3" };

	auto it = static_vector_iterator<std::array<std::string, 16>>(sv, 3);

	BOOST_CHECK(*(it - 2) == "2");
}

BOOST_AUTO_TEST_CASE(static_vector_iterator__sub__3)
{
	using namespace wordring;

	std::array<std::string, 16> const sv{ "1", "2", "3" };

	auto it = static_vector_iterator<std::array<std::string, 16> const>(sv, 3);

	BOOST_CHECK(*(it - 2) == "2");
}

// inline bool operator==(static_vector_iterator<Container1> const& lhs, static_vector_iterator<Container2> const& rhs)
BOOST_AUTO_TEST_CASE(static_vector_iterator__equal__1)
{
	using namespace wordring;

	std::array<std::string, 16> sv{ "1", "2", "3" };

	auto it1 = static_vector_iterator<std::array<std::string, 16>>(sv, 0);
	auto it2 = static_vector_iterator<std::array<std::string, 16>>(sv, 0);

	BOOST_CHECK(it1 == it2);
}

BOOST_AUTO_TEST_CASE(static_vector_iterator__equal__2)
{
	using namespace wordring;

	std::array<std::string, 16> sv{ "1", "2", "3" };

	auto it1 = static_vector_iterator<std::array<std::string, 16>>(sv, 0);
	auto it2 = static_vector_iterator<std::array<std::string, 16>>(sv, 3);

	BOOST_CHECK((it1 == it2) == false);
}

BOOST_AUTO_TEST_CASE(static_vector_iterator__equal__3)
{
	using namespace wordring;

	std::array<std::string, 16> const sv{ "1", "2", "3" };

	auto it1 = static_vector_iterator<std::array<std::string, 16> const>(sv, 0);
	auto it2 = static_vector_iterator<std::array<std::string, 16> const>(sv, 0);

	BOOST_CHECK(it1 == it2);
}

BOOST_AUTO_TEST_CASE(static_vector_iterator__equal__4)
{
	using namespace wordring;

	std::array<std::string, 16> const sv{ "1", "2", "3" };

	auto it1 = static_vector_iterator<std::array<std::string, 16> const>(sv, 0);
	auto it2 = static_vector_iterator<std::array<std::string, 16> const>(sv, 3);

	BOOST_CHECK((it1 == it2) == false);
}


BOOST_AUTO_TEST_CASE(static_vector_iterator__equal__5)
{
	using namespace wordring;

	std::array<std::string, 16> sv{ "1", "2", "3" };

	auto it1 = static_vector_iterator<std::array<std::string, 16>>(sv, 0);
	auto it2 = static_vector_iterator<std::array<std::string, 16> const>(sv, 0);

	BOOST_CHECK(it1 == it2);
}

BOOST_AUTO_TEST_CASE(static_vector_iterator__equal__6)
{
	using namespace wordring;

	std::array<std::string, 16> sv{ "1", "2", "3" };

	auto it1 = static_vector_iterator<std::array<std::string, 16> const>(sv, 0);
	auto it2 = static_vector_iterator<std::array<std::string, 16>>(sv, 3);

	BOOST_CHECK((it1 == it2) == false);
}

// inline bool operator!=(static_vector_iterator<Container1> const& lhs, static_vector_iterator<Container2> const& rhs)
BOOST_AUTO_TEST_CASE(static_vector_iterator__not_equal__1)
{
	using namespace wordring;

	std::array<std::string, 16> sv{ "1", "2", "3" };

	auto it1 = static_vector_iterator<std::array<std::string, 16>>(sv, 0);
	auto it2 = static_vector_iterator<std::array<std::string, 16>>(sv, 3);

	BOOST_CHECK(it1 != it2);
}

BOOST_AUTO_TEST_CASE(static_vector_iterator__not_equal__2)
{
	using namespace wordring;

	std::array<std::string, 16> sv{ "1", "2", "3" };

	auto it1 = static_vector_iterator<std::array<std::string, 16>>(sv, 0);
	auto it2 = static_vector_iterator<std::array<std::string, 16>>(sv, 0);

	BOOST_CHECK((it1 != it2) == false);
}

BOOST_AUTO_TEST_CASE(static_vector_iterator__not_equal__3)
{
	using namespace wordring;

	std::array<std::string, 16> const sv{ "1", "2", "3" };

	auto it1 = static_vector_iterator<std::array<std::string, 16> const>(sv, 0);
	auto it2 = static_vector_iterator<std::array<std::string, 16> const>(sv, 3);

	BOOST_CHECK(it1 != it2);
}

BOOST_AUTO_TEST_CASE(static_vector_iterator__not_equal__4)
{
	using namespace wordring;

	std::array<std::string, 16> const sv{ "1", "2", "3" };

	auto it1 = static_vector_iterator<std::array<std::string, 16> const>(sv, 0);
	auto it2 = static_vector_iterator<std::array<std::string, 16> const>(sv, 0);

	BOOST_CHECK((it1 != it2) == false);
}


BOOST_AUTO_TEST_CASE(static_vector_iterator__not_equal__5)
{
	using namespace wordring;

	std::array<std::string, 16> sv{ "1", "2", "3" };

	auto it1 = static_vector_iterator<std::array<std::string, 16>>(sv, 0);
	auto it2 = static_vector_iterator<std::array<std::string, 16> const>(sv, 3);

	BOOST_CHECK(it1 != it2);
}

BOOST_AUTO_TEST_CASE(static_vector_iterator__not_equal__6)
{
	using namespace wordring;

	std::array<std::string, 16> sv{ "1", "2", "3" };

	auto it1 = static_vector_iterator<std::array<std::string, 16> const>(sv, 3);
	auto it2 = static_vector_iterator<std::array<std::string, 16>>(sv, 3);

	BOOST_CHECK((it1 != it2) == false);
}

// inline bool operator>(static_vector_iterator<Container1> const& lhs, static_vector_iterator<Container2> const& rhs)
BOOST_AUTO_TEST_CASE(static_vector_iterator__more_than__1)
{
	using namespace wordring;

	std::array<std::string, 16> sv{ "1", "2", "3" };

	auto it1 = static_vector_iterator<std::array<std::string, 16>>(sv, 3);
	auto it2 = static_vector_iterator<std::array<std::string, 16>>(sv, 0);

	BOOST_CHECK(it1 > it2);
}

BOOST_AUTO_TEST_CASE(static_vector_iterator__more_than__2)
{
	using namespace wordring;

	std::array<std::string, 16> sv{ "1", "2", "3" };

	auto it1 = static_vector_iterator<std::array<std::string, 16> const>(sv, 3);
	auto it2 = static_vector_iterator<std::array<std::string, 16> const>(sv, 0);

	BOOST_CHECK(it1 > it2);
}

BOOST_AUTO_TEST_CASE(static_vector_iterator__more_than__3)
{
	using namespace wordring;

	std::array<std::string, 16> sv{ "1", "2", "3" };

	auto it1 = static_vector_iterator<std::array<std::string, 16> const>(sv, 0);
	auto it2 = static_vector_iterator<std::array<std::string, 16>>(sv, 0);

	BOOST_CHECK((it1 > it2) == false);
}

BOOST_AUTO_TEST_CASE(static_vector_iterator__more_than__4)
{
	using namespace wordring;

	std::array<std::string, 16> sv{ "1", "2", "3" };

	auto it1 = static_vector_iterator<std::array<std::string, 16>>(sv, 0);
	auto it2 = static_vector_iterator<std::array<std::string, 16> const>(sv, 3);

	BOOST_CHECK((it1 > it2) == false);
}

// inline bool operator<(static_vector_iterator<Container1> const& lhs, static_vector_iterator<Container2> const& rhs)
BOOST_AUTO_TEST_CASE(static_vector_iterator__less_than__1)
{
	using namespace wordring;

	std::array<std::string, 16> sv{ "1", "2", "3" };

	auto it1 = static_vector_iterator<std::array<std::string, 16>>(sv, 0);
	auto it2 = static_vector_iterator<std::array<std::string, 16>>(sv, 3);

	BOOST_CHECK(it1 < it2);
}

BOOST_AUTO_TEST_CASE(static_vector_iterator__less_than__2)
{
	using namespace wordring;

	std::array<std::string, 16> sv{ "1", "2", "3" };

	auto it1 = static_vector_iterator<std::array<std::string, 16> const>(sv, 0);
	auto it2 = static_vector_iterator<std::array<std::string, 16> const>(sv, 3);

	BOOST_CHECK(it1 < it2);
}

BOOST_AUTO_TEST_CASE(static_vector_iterator__less_than__3)
{
	using namespace wordring;

	std::array<std::string, 16> sv{ "1", "2", "3" };

	auto it1 = static_vector_iterator<std::array<std::string, 16> const>(sv, 0);
	auto it2 = static_vector_iterator<std::array<std::string, 16>>(sv, 0);

	BOOST_CHECK((it1 < it2) == false);
}

BOOST_AUTO_TEST_CASE(static_vector_iterator__less_than__4)
{
	using namespace wordring;

	std::array<std::string, 16> sv{ "1", "2", "3" };

	auto it1 = static_vector_iterator<std::array<std::string, 16>>(sv, 3);
	auto it2 = static_vector_iterator<std::array<std::string, 16> const>(sv, 0);

	BOOST_CHECK((it1 < it2) == false);
}

// inline bool operator>=(static_vector_iterator<Container1> const& lhs, static_vector_iterator<Container2> const& rhs)
BOOST_AUTO_TEST_CASE(static_vector_iterator__more_than_or_equal__1)
{
	using namespace wordring;

	std::array<std::string, 16> sv{ "1", "2", "3" };

	auto it1 = static_vector_iterator<std::array<std::string, 16>>(sv, 3);
	auto it2 = static_vector_iterator<std::array<std::string, 16>>(sv, 0);

	BOOST_CHECK(it1 >= it2);
}

BOOST_AUTO_TEST_CASE(static_vector_iterator__more_than_or_equal__2)
{
	using namespace wordring;

	std::array<std::string, 16> sv{ "1", "2", "3" };

	auto it1 = static_vector_iterator<std::array<std::string, 16> const>(sv, 3);
	auto it2 = static_vector_iterator<std::array<std::string, 16> const>(sv, 0);

	BOOST_CHECK(it1 >= it2);
}

BOOST_AUTO_TEST_CASE(static_vector_iterator__more_than_or_equal__3)
{
	using namespace wordring;

	std::array<std::string, 16> sv{ "1", "2", "3" };

	auto it1 = static_vector_iterator<std::array<std::string, 16> const>(sv, 0);
	auto it2 = static_vector_iterator<std::array<std::string, 16>>(sv, 0);

	BOOST_CHECK(it1 >= it2);
}

BOOST_AUTO_TEST_CASE(static_vector_iterator__more_than_or_equal__4)
{
	using namespace wordring;

	std::array<std::string, 16> sv{ "1", "2", "3" };

	auto it1 = static_vector_iterator<std::array<std::string, 16>>(sv, 0);
	auto it2 = static_vector_iterator<std::array<std::string, 16> const>(sv, 3);

	BOOST_CHECK((it1 >= it2) == false);
}

// inline bool operator<=(static_vector_iterator<Container1> const& lhs, static_vector_iterator<Container2> const& rhs)
BOOST_AUTO_TEST_CASE(static_vector_iterator__less_than_or_equal__1)
{
	using namespace wordring;

	std::array<std::string, 16> sv{ "1", "2", "3" };

	auto it1 = static_vector_iterator<std::array<std::string, 16>>(sv, 0);
	auto it2 = static_vector_iterator<std::array<std::string, 16>>(sv, 3);

	BOOST_CHECK(it1 <= it2);
}

BOOST_AUTO_TEST_CASE(static_vector_iterator__less_than_or_equal__2)
{
	using namespace wordring;

	std::array<std::string, 16> sv{ "1", "2", "3" };

	auto it1 = static_vector_iterator<std::array<std::string, 16> const>(sv, 0);
	auto it2 = static_vector_iterator<std::array<std::string, 16> const>(sv, 3);

	BOOST_CHECK(it1 <= it2);
}

BOOST_AUTO_TEST_CASE(static_vector_iterator__less_than_or_equal__3)
{
	using namespace wordring;

	std::array<std::string, 16> sv{ "1", "2", "3" };

	auto it1 = static_vector_iterator<std::array<std::string, 16> const>(sv, 0);
	auto it2 = static_vector_iterator<std::array<std::string, 16>>(sv, 0);

	BOOST_CHECK(it1 <= it2);
}

BOOST_AUTO_TEST_CASE(static_vector_iterator__less_than_or_equal__4)
{
	using namespace wordring;

	std::array<std::string, 16> sv{ "1", "2", "3" };

	auto it1 = static_vector_iterator<std::array<std::string, 16>>(sv, 3);
	auto it2 = static_vector_iterator<std::array<std::string, 16> const>(sv, 0);

	BOOST_CHECK((it1 <= it2) == false);
}

// ----------------------------------------------------------------------------
// static_vector
// ----------------------------------------------------------------------------

// static_vector()
BOOST_AUTO_TEST_CASE(static_vector__construct__1)
{
	using namespace wordring;

	static_vector<char, 16> sv1;
	static_vector<char, 16> sv2{};
	auto sv3 = static_vector<char, 16>();
}

// static_vector(size_type count, value_type const& value)
BOOST_AUTO_TEST_CASE(static_vector__construct__2)
{
	using namespace wordring;

	static_vector<char, 16> sv1(2, '\1');
	std::string s1(sv1.begin(), sv1.end());
	BOOST_CHECK(s1 == "\1\1");
}

// static_vector(size_type count)
BOOST_AUTO_TEST_CASE(static_vector__construct__3)
{
	using namespace wordring;

	std::string s1{ '\0', '\0' };
	static_vector<char, 16> sv1(2);
	std::string s2(sv1.begin(), sv1.end());
	BOOST_CHECK(s1 == s2);
}

// static_vector(InputIterator first, InputIterator last)
BOOST_AUTO_TEST_CASE(static_vector__construct__4)
{
	using namespace wordring;

	std::string s1("123");
	static_vector<char, 16> sv1(s1.begin(), s1.end());
	std::string s2(sv1.begin(), sv1.end());
	BOOST_CHECK(s1 == s2);
}

// static_vector(static_vector const&)
BOOST_AUTO_TEST_CASE(static_vector__construct__5)
{
	using namespace wordring;

	static_vector<char, 16> sv1(2, '\1');
	static_vector<char, 16> sv2(sv1);
	std::string s2(sv2.begin(), sv2.end());
	BOOST_CHECK(s2 == "\1\1");
}

// static_vector(static_vector&&)
BOOST_AUTO_TEST_CASE(static_vector__construct__6)
{
	using namespace wordring;

	static_vector<char, 16> sv1(2, '\1');
	static_vector<char, 16> sv2(std::move(sv1));

	std::string s2(sv2.begin(), sv2.end());
	BOOST_CHECK(s2 == "\1\1");
}

// static_vector(std::initializer_list<value_type> ilist)
BOOST_AUTO_TEST_CASE(static_vector__construct__7)
{
	using namespace wordring;

	static_vector<char, 16> sv1{ '1', '2', '3' };

	std::string s1(sv1.begin(), sv1.end());
	BOOST_CHECK(s1 == "123");
}

// static_vector& operator=(static_vector const&)
BOOST_AUTO_TEST_CASE(static_vector__assign__1)
{
	using namespace wordring;

	static_vector<char, 16> sv1{ '1', '2', '3' };
	static_vector<char, 16> sv2;

	sv2 = sv1;

	std::string s1(sv2.begin(), sv2.end());
	BOOST_CHECK(s1 == "123");
}

// static_vector& operator=(static_vector&&)
BOOST_AUTO_TEST_CASE(static_vector__assign__2)
{
	using namespace wordring;

	static_vector<char, 16> sv1{ '1', '2', '3' };
	static_vector<char, 16> sv2;

	sv2 = std::move(sv1);

	std::string s1(sv2.begin(), sv2.end());
	BOOST_CHECK(s1 == "123");
}

// static_vector& operator=(std::initializer_list<value_type> ilist)
BOOST_AUTO_TEST_CASE(static_vector__assign__3)
{
	using namespace wordring;

	static_vector<char, 16> sv;

	sv = { '1', '2', '3' };

	std::string s(sv.begin(), sv.end());
	BOOST_CHECK(s == "123");
}

// void assign(size_type count, value_type const& value)
BOOST_AUTO_TEST_CASE(static_vector__assign__4)
{
	using namespace wordring;

	static_vector<char, 16> sv;

	sv.assign(2, '\1');

	std::string s(sv.begin(), sv.end());
	BOOST_CHECK(s == "\1\1");
}

// void assign(InputIterator first, InputIterator last)
BOOST_AUTO_TEST_CASE(static_vector__assign__5)
{
	using namespace wordring;

	static_vector<char, 16> sv;
	std::string s1("123");

	sv.assign(s1.begin(), s1.end());

	std::string s2(sv.begin(), sv.end());
	BOOST_CHECK(s1 == s2);
}

// void assign(std::initializer_list<value_type> ilist)
BOOST_AUTO_TEST_CASE(static_vector__assign__6)
{
	using namespace wordring;

	static_vector<char, 16> sv;

	sv.assign({ '1', '2', '3' });

	std::string s(sv.begin(), sv.end());
	BOOST_CHECK(s == "123");
}

// 要素アクセス ----------------------------------------------------------------

// reference at(size_type pos)
BOOST_AUTO_TEST_CASE(static_vector__at__1)
{
	using namespace wordring;

	static_vector<char, 16> sv{ '1', '2', '3' };

	sv.at(1) = '5';

	BOOST_CHECK(sv.at(1) == '5');
}

// const_reference at(size_type pos) const
BOOST_AUTO_TEST_CASE(static_vector__at__2)
{
	using namespace wordring;

	static_vector<char, 16> const sv{ '1', '2', '3' };

	BOOST_CHECK(sv.at(1) == '2');
}

// reference operator[](size_type pos)
BOOST_AUTO_TEST_CASE(static_vector__at__3)
{
	using namespace wordring;

	static_vector<char, 16> sv{ '1', '2', '3' };

	sv[1] = '5';

	BOOST_CHECK(sv[1] == '5');
}

// const_reference operator[](size_type pos) const
BOOST_AUTO_TEST_CASE(static_vector__at__4)
{
	using namespace wordring;

	static_vector<char, 16> const sv{ '1', '2', '3' };

	BOOST_CHECK(sv[1] == '2');
}

// reference front()
BOOST_AUTO_TEST_CASE(static_vector__front__1)
{
	using namespace wordring;

	static_vector<char, 16> sv{ '1', '2', '3' };

	sv.front() = '5';

	BOOST_CHECK(sv.front() == '5');
}

// const_reference front() const
BOOST_AUTO_TEST_CASE(static_vector__front__2)
{
	using namespace wordring;

	static_vector<char, 16> const sv{ '1', '2', '3' };

	BOOST_CHECK(sv.front() == '1');
}

// reference back()
BOOST_AUTO_TEST_CASE(static_vector__back__1)
{
	using namespace wordring;

	static_vector<char, 16> sv{ '1', '2', '3' };

	BOOST_CHECK(sv.back() == '3');

	sv.back() = '5';

	BOOST_CHECK(sv.back() == '5');
}

// const_reference back() const
BOOST_AUTO_TEST_CASE(static_vector__back__2)
{
	using namespace wordring;

	static_vector<char, 16> const sv{ '1', '2', '3' };

	BOOST_CHECK(sv.back() == '3');
}

// pointer data()
BOOST_AUTO_TEST_CASE(static_vector__data__1)
{
	using namespace wordring;

	static_vector<char, 16> sv{ '1', '2', '3' };

	BOOST_CHECK(*(sv.data()) == '1');

	*sv.data() = '5';

	BOOST_CHECK(*sv.data() == '5');
}

// const_pointer data() const
BOOST_AUTO_TEST_CASE(static_vector__data__2)
{
	using namespace wordring;

	static_vector<char, 16> sv{ '1', '2', '3' };

	BOOST_CHECK(*sv.data() == '1');
}

// イテレータ ------------------------------------------------------------------

// iterator begin()
BOOST_AUTO_TEST_CASE(static_vector__begin__1)
{
	using namespace wordring;

	static_vector<char, 16> sv{ '1', '2', '3' };

	BOOST_CHECK(*sv.begin() == '1');

	*sv.begin() = '5';

	BOOST_CHECK(*sv.begin() == '5');
}

// const_iterator begin() const
BOOST_AUTO_TEST_CASE(static_vector__begin__2)
{
	using namespace wordring;

	static_vector<char, 16> const sv{ '1', '2', '3' };

	BOOST_CHECK(*sv.begin() == '1');
}

// const_iterator cbegin() const
BOOST_AUTO_TEST_CASE(static_vector__cbegin__1)
{
	using namespace wordring;

	static_vector<char, 16> const sv{ '1', '2', '3' };

	BOOST_CHECK(*sv.cbegin() == '1');
}

// iterator end()
BOOST_AUTO_TEST_CASE(static_vector__end__1)
{
	using namespace wordring;

	static_vector<char, 16> sv{ '1', '2', '3' };

	*--sv.end() = '5';

	BOOST_CHECK(*--sv.end() == '5');
}

// const_iterator end() const
BOOST_AUTO_TEST_CASE(static_vector__end__2)
{
	using namespace wordring;

	static_vector<char, 16> const sv{ '1', '2', '3' };

	BOOST_CHECK(*--sv.end() == '3');
}

// const_iterator cend() const
BOOST_AUTO_TEST_CASE(static_vector__cend__1)
{
	using namespace wordring;

	static_vector<char, 16> const sv{ '1', '2', '3' };

	BOOST_CHECK(*--sv.end() == '3');
}

// reverse_iterator rbegin()
BOOST_AUTO_TEST_CASE(static_vector__rbegin__1)
{
	using namespace wordring;

	static_vector<char, 16> sv{ '1', '2', '3' };

	BOOST_CHECK(*sv.rbegin() == '3');

	*sv.rbegin() = '5';

	BOOST_CHECK(*sv.rbegin() == '5');
}

// const_reverse_iterator rbegin() const
BOOST_AUTO_TEST_CASE(static_vector__rbegin__2)
{
	using namespace wordring;

	static_vector<char, 16> const sv{ '1', '2', '3' };

	BOOST_CHECK(*sv.rbegin() == '3');
}

// const_reverse_iterator crbegin() const
BOOST_AUTO_TEST_CASE(static_vector__crbegin__1)
{
	using namespace wordring;

	static_vector<char, 16> const sv{ '1', '2', '3' };

	BOOST_CHECK(*sv.crbegin() == '3');
}

// reverse_iterator rend()
BOOST_AUTO_TEST_CASE(static_vector__rend__1)
{
	using namespace wordring;

	static_vector<char, 16> sv{ '1', '2', '3' };

	BOOST_CHECK(*--sv.rend() == '1');

	*--sv.rend() = '5';

	BOOST_CHECK(*--sv.rend() == '5');
}

// const_reverse_iterator rend() const
BOOST_AUTO_TEST_CASE(static_vector__rend__2)
{
	using namespace wordring;

	static_vector<char, 16> const sv{ '1', '2', '3' };

	BOOST_CHECK(*--sv.rend() == '1');
}

// const_reverse_iterator crend() const
BOOST_AUTO_TEST_CASE(static_vector__crend__1)
{
	using namespace wordring;

	static_vector<char, 16> const sv{ '1', '2', '3' };

	BOOST_CHECK(*--sv.crend() == '1');
}

// 容量 -----------------------------------------------------------------------

// bool empty() const
BOOST_AUTO_TEST_CASE(static_vector__empty__1)
{
	using namespace wordring;

	static_vector<char, 16> const sv1{};
	BOOST_CHECK(sv1.empty());

	static_vector<char, 16> const sv2{ '1', '2', '3' };
	BOOST_CHECK(sv2.empty() == false);
}

// size_type size() const
BOOST_AUTO_TEST_CASE(static_vector__size__1)
{
	using namespace wordring;

	static_vector<char, 16> const sv1{};
	BOOST_CHECK(sv1.size() == 0);

	static_vector<char, 16> const sv2{ '1', '2', '3' };
	BOOST_CHECK(sv2.size() == 3);
}

// size_type max_size() const
BOOST_AUTO_TEST_CASE(static_vector__max_size__1)
{
	using namespace wordring;

	static_vector<char, 16> const sv1{};
	BOOST_CHECK(sv1.max_size() == 16);
}

// size_type capacity() const
BOOST_AUTO_TEST_CASE(static_vector__capacity__1)
{
	using namespace wordring;

	static_vector<char, 16> const sv1{};
	BOOST_CHECK(sv1.capacity() == 16);
}

// 変更 -----------------------------------------------------------------------

// void clear()
BOOST_AUTO_TEST_CASE(static_vector__clear__1)
{
	using namespace wordring;

	static_vector<char, 3> sv{ '1', '2', '3' };

	sv.clear();

	BOOST_CHECK(sv.empty());
}

// iterator insert(const_iterator pos, value_type const& value)
BOOST_AUTO_TEST_CASE(static_vector__insert__1)
{
	using namespace wordring;

	std::vector<char> v{ '1', '2', '3' };
	static_vector<char, 4> sv{ '1', '2', '3' };
	char ch = '5';

	auto it_v = v.insert(++v.begin(), ch);
	auto it_sv = sv.insert(++sv.begin(), ch);

	BOOST_CHECK(v.size() == sv.size());
	BOOST_CHECK(*it_v == *it_sv);
	BOOST_CHECK(std::equal(v.begin(), v.end(), sv.begin(), sv.end()));
}

BOOST_AUTO_TEST_CASE(static_vector__insert__2)
{
	using namespace wordring;

	std::vector<char> v{ '1', '2', '3' };
	static_vector<char, 4> sv{ '1', '2', '3' };
	char ch = '5';

	auto it_v = v.insert(v.begin(), ch);
	auto it_sv = sv.insert(sv.begin(), ch);

	BOOST_CHECK(v.size() == sv.size());
	BOOST_CHECK(*it_v == *it_sv);
	BOOST_CHECK(std::equal(v.begin(), v.end(), sv.begin(), sv.end()));
}

BOOST_AUTO_TEST_CASE(static_vector__insert__3)
{
	using namespace wordring;

	std::vector<char> v{ '1', '2', '3' };
	static_vector<char, 4> sv{ '1', '2', '3' };
	char ch = '5';

	auto it_v = v.insert(v.end(), ch);
	auto it_sv = sv.insert(sv.end(), ch);

	BOOST_CHECK(v.size() == sv.size());
	BOOST_CHECK(*it_v == *it_sv);
	BOOST_CHECK(std::equal(v.begin(), v.end(), sv.begin(), sv.end()));
}

// iterator insert(const_iterator pos, value_type&& value)
BOOST_AUTO_TEST_CASE(static_vector__insert__4)
{
	using namespace wordring;

	std::vector<char> v{ '1', '2', '3' };
	static_vector<char, 4> sv{ '1', '2', '3' };
	char ch = '5';

	auto it_v = v.insert(++v.begin(), std::move(ch));
	auto it_sv = sv.insert(++sv.begin(), std::move(ch));

	BOOST_CHECK(v.size() == sv.size());
	BOOST_CHECK(*it_v == *it_sv);
	BOOST_CHECK(std::equal(v.begin(), v.end(), sv.begin(), sv.end()));
}

BOOST_AUTO_TEST_CASE(static_vector__insert__5)
{
	using namespace wordring;

	std::vector<char> v{ '1', '2', '3' };
	static_vector<char, 4> sv{ '1', '2', '3' };
	char ch = '5';

	auto it_v = v.insert(v.begin(), std::move(ch));
	auto it_sv = sv.insert(sv.begin(), std::move(ch));

	BOOST_CHECK(v.size() == sv.size());
	BOOST_CHECK(*it_v == *it_sv);
	BOOST_CHECK(std::equal(v.begin(), v.end(), sv.begin(), sv.end()));
}

BOOST_AUTO_TEST_CASE(static_vector__insert__6)
{
	using namespace wordring;

	std::vector<char> v{ '1', '2', '3' };
	static_vector<char, 4> sv{ '1', '2', '3' };
	char ch = '5';

	auto it_v = v.insert(v.end(), std::move(ch));
	auto it_sv = sv.insert(sv.end(), std::move(ch));

	BOOST_CHECK(v.size() == sv.size());
	BOOST_CHECK(*it_v == *it_sv);
	BOOST_CHECK(std::equal(v.begin(), v.end(), sv.begin(), sv.end()));
}

// iterator insert(const_iterator pos, size_type count, value_type const& value)
BOOST_AUTO_TEST_CASE(static_vector__insert__7)
{
	using namespace wordring;

	std::vector<char> v{ '1', '2', '3' };
	static_vector<char, 5> sv{ '1', '2', '3' };
	char ch = '5';

	auto it_v = v.insert(++v.begin(), 2, ch);
	auto it_sv = sv.insert(++sv.begin(), 2, ch);

	BOOST_CHECK(*it_v == *it_sv);
	std::string s_v(v.begin(), v.end());
	std::string s_sv(sv.begin(), sv.end());
	BOOST_CHECK(s_v == s_sv);
}

BOOST_AUTO_TEST_CASE(static_vector__insert__8)
{
	using namespace wordring;

	std::vector<char> v{ '1', '2', '3' };
	static_vector<char, 5> sv{ '1', '2', '3' };
	char ch = '5';

	auto it_v = v.insert(v.begin(), 2, ch);
	auto it_sv = sv.insert(sv.begin(), 2, ch);

	BOOST_CHECK(*it_v == *it_sv);
	std::string s_v(v.begin(), v.end());
	std::string s_sv(sv.begin(), sv.end());
	BOOST_CHECK(s_v == s_sv);
}

BOOST_AUTO_TEST_CASE(static_vector__insert__9)
{
	using namespace wordring;

	std::vector<char> v{ '1', '2', '3' };
	static_vector<char, 5> sv{ '1', '2', '3' };
	char ch = '5';

	auto it_v = v.insert(v.end(), 2, ch);
	auto it_sv = sv.insert(sv.end(), 2, ch);

	BOOST_CHECK(*it_v == *it_sv);
	std::string s_v(v.begin(), v.end());
	std::string s_sv(sv.begin(), sv.end());
	BOOST_CHECK(s_v == s_sv);
}

// iterator insert(const_iterator pos, InputIterator first, InputIterator last)
BOOST_AUTO_TEST_CASE(static_vector__insert__10)
{
	using namespace wordring;

	std::vector<char> v{ '1', '2', '3' };
	static_vector<char, 16> sv{ '1', '2', '3' };
	std::string s1{ "456" };

	auto it_v = v.insert(++v.begin(), s1.begin(), s1.end());
	auto it_sv = sv.insert(++sv.begin(), s1.begin(), s1.end());

	BOOST_CHECK(*it_v == *it_sv);
	BOOST_CHECK(v.size() == sv.size());
	BOOST_CHECK(std::equal(v.begin(), v.end(), sv.begin(), sv.end()));
}

BOOST_AUTO_TEST_CASE(static_vector__insert__11)
{
	using namespace wordring;

	std::vector<char> v{ '1', '2', '3' };
	static_vector<char, 16> sv{ '1', '2', '3' };
	std::string s1{ "4567" };

	auto it_v = v.insert(v.begin(), s1.begin(), s1.end());
	auto it_sv = sv.insert(sv.begin(), s1.begin(), s1.end());

	BOOST_CHECK(*it_v == *it_sv);
	BOOST_CHECK(v.size() == sv.size());
	BOOST_CHECK(std::equal(v.begin(), v.end(), sv.begin(), sv.end()));
}

BOOST_AUTO_TEST_CASE(static_vector__insert__12)
{
	using namespace wordring;

	std::vector<char> v{ '1', '2', '3' };
	static_vector<char, 16> sv{ '1', '2', '3' };
	std::string s1{ "456" };

	auto it_v = v.insert(v.end(), s1.begin(), s1.end());
	auto it_sv = sv.insert(sv.end(), s1.begin(), s1.end());

	BOOST_CHECK(*it_v == *it_sv);
	BOOST_CHECK(v.size() == sv.size());
	BOOST_CHECK(std::equal(v.begin(), v.end(), sv.begin(), sv.end()));
}

// iterator insert(const_iterator pos, std::initializer_list<value_type> ilist)
BOOST_AUTO_TEST_CASE(static_vector__insert__13)
{
	using namespace wordring;

	std::vector<char> v{ '1', '2', '3' };
	static_vector<char, 16> sv{ '1', '2', '3' };

	auto it_v = v.insert(++v.begin(), { '4', '5', '6' });
	auto it_sv = sv.insert(++sv.begin(), { '4', '5', '6' });

	BOOST_CHECK(*it_v == *it_sv);
	BOOST_CHECK(v.size() == sv.size());
	BOOST_CHECK(std::equal(v.begin(), v.end(), sv.begin(), sv.end()));
}

BOOST_AUTO_TEST_CASE(static_vector__insert__14)
{
	using namespace wordring;

	std::vector<char> v{ '1', '2', '3' };
	static_vector<char, 16> sv{ '1', '2', '3' };

	auto it_v = v.insert(v.begin(), { '4', '5', '6' });
	auto it_sv = sv.insert(sv.begin(), { '4', '5', '6' });

	BOOST_CHECK(*it_v == *it_sv);
	BOOST_CHECK(v.size() == sv.size());
	BOOST_CHECK(std::equal(v.begin(), v.end(), sv.begin(), sv.end()));
}

BOOST_AUTO_TEST_CASE(static_vector__insert__15)
{
	using namespace wordring;

	std::vector<char> v{ '1', '2', '3' };
	static_vector<char, 16> sv{ '1', '2', '3' };

	auto it_v = v.insert(v.end(), { '4', '5', '6' });
	auto it_sv = sv.insert(sv.end(), { '4', '5', '6' });

	BOOST_CHECK(*it_v == *it_sv);
	BOOST_CHECK(v.size() == sv.size());
	BOOST_CHECK(std::equal(v.begin(), v.end(), sv.begin(), sv.end()));
}

// iterator erase(const_iterator pos)
BOOST_AUTO_TEST_CASE(static_vector__erase__1)
{
	using namespace wordring;

	std::vector<char> v{ '1', '2', '3' };
	static_vector<char, 16> sv{ '1', '2', '3' };

	auto it_v = v.erase(v.begin());
	auto it_sv = sv.erase(sv.begin());

	BOOST_CHECK(v.size() == sv.size());
	BOOST_CHECK(*it_v == *it_sv);
	BOOST_CHECK(*it_v == '2');

	std::string s_v(v.begin(), v.end());
	std::string s_sv(sv.begin(), sv.end());
	BOOST_CHECK(s_v == s_sv);
	BOOST_CHECK(s_v == "23");
}

BOOST_AUTO_TEST_CASE(static_vector__erase__2)
{
	using namespace wordring;

	std::vector<char> v{ '1', '2', '3' };
	static_vector<char, 16> sv{ '1', '2', '3' };

	auto it_v = v.erase(--v.end());
	auto it_sv = sv.erase(--sv.end());

	BOOST_CHECK(v.size() == sv.size());
	BOOST_CHECK(it_v == v.end());
	BOOST_CHECK(it_sv == sv.end());

	std::string s_v(v.begin(), v.end());
	std::string s_sv(sv.begin(), sv.end());
	BOOST_CHECK(s_v == s_sv);
}

BOOST_AUTO_TEST_CASE(static_vector__erase__3)
{
	using namespace wordring;

	std::vector<char> v{ '1', '2', '3' };
	static_vector<char, 16> sv{ '1', '2', '3' };

	auto it_v = v.erase(v.begin() + 1);
	auto it_sv = sv.erase(sv.begin() + 1);

	BOOST_CHECK(v.size() == sv.size());
	BOOST_CHECK(*it_v == '3');
	BOOST_CHECK(*it_sv == '3');

	std::string s_v(v.begin(), v.end());
	std::string s_sv(sv.begin(), sv.end());
	BOOST_CHECK(s_v == s_sv);
}

// iterator erase(const_iterator first, const_iterator last)
BOOST_AUTO_TEST_CASE(static_vector__erase__4)
{
	using namespace wordring;

	std::vector<char> v{ '1', '2', '3', '4' };
	static_vector<char, 16> sv{ '1', '2', '3', '4' };

	auto it_v = v.erase(v.begin() + 1, v.begin() + 3);
	auto it_sv = sv.erase(sv.begin() + 1, sv.begin() + 3);

	BOOST_CHECK(v.size() == sv.size());
	BOOST_CHECK(*it_v == *it_sv);
	BOOST_CHECK(*it_v == '4');

	std::string s_v(v.begin(), v.end());
	std::string s_sv(sv.begin(), sv.end());
	BOOST_CHECK(s_v == s_sv);
	BOOST_CHECK(s_v == "14");
}

BOOST_AUTO_TEST_CASE(static_vector__erase__5)
{
	using namespace wordring;

	std::vector<char> v{ '1', '2', '3', '4' };
	static_vector<char, 16> sv{ '1', '2', '3', '4' };

	auto it_v = v.erase(v.begin(), ++v.begin());
	auto it_sv = sv.erase(sv.begin(), ++sv.begin());

	BOOST_CHECK(v.size() == sv.size());
	BOOST_CHECK(*it_v == *it_sv);
	BOOST_CHECK(*it_sv == '2');

	std::string s_v(v.begin(), v.end());
	std::string s_sv(sv.begin(), sv.end());
	BOOST_CHECK(s_v == s_sv);
	BOOST_CHECK(s_v == "234");
}

BOOST_AUTO_TEST_CASE(static_vector__erase__6)
{
	using namespace wordring;

	std::vector<char> v{ '1', '2', '3', '4' };
	static_vector<char, 16> sv{ '1', '2', '3', '4' };

	auto it_v = v.erase(--v.end(), v.end());
	auto it_sv = sv.erase(--sv.end(), sv.end());

	BOOST_CHECK(v.size() == sv.size());
	BOOST_CHECK(it_v == v.end());
	BOOST_CHECK(it_sv == sv.end());

	std::string s_v(v.begin(), v.end());
	std::string s_sv(sv.begin(), sv.end());
	BOOST_CHECK(s_v == s_sv);
	BOOST_CHECK(s_v == "123");
}

BOOST_AUTO_TEST_CASE(static_vector__erase__7)
{
	using namespace wordring;

	std::vector<char> v{ '1', '2', '3', '4' };
	static_vector<char, 16> sv{ '1', '2', '3', '4' };

	auto it_v = v.erase(v.begin(), v.begin());
	auto it_sv = sv.erase(sv.begin(), sv.begin());

	BOOST_CHECK(sv.size() == 4);
	BOOST_CHECK(v.size() == sv.size());
	BOOST_CHECK(*it_v == *it_sv);
	BOOST_CHECK(*it_v == '1');

	std::string s_v(v.begin(), v.end());
	std::string s_sv(sv.begin(), sv.end());
	BOOST_CHECK(s_v == s_sv);
	BOOST_CHECK(s_v == "1234");
}

BOOST_AUTO_TEST_CASE(static_vector__erase__8)
{
	using namespace wordring;

	std::vector<char> v{ '1', '2', '3', '4' };
	static_vector<char, 16> sv{ '1', '2', '3', '4' };

	auto it_v = v.erase(--v.end(), --v.end());
	auto it_sv = sv.erase(--sv.end(), --sv.end());

	BOOST_CHECK(sv.size() == 4);
	BOOST_CHECK(v.size() == sv.size());
	BOOST_CHECK(it_v == --v.end());
	BOOST_CHECK(it_sv == --sv.end());

	std::string s_v(v.begin(), v.end());
	std::string s_sv(sv.begin(), sv.end());
	BOOST_CHECK(s_v == s_sv);
	BOOST_CHECK(s_v == "1234");
}

// void push_back(value_type const& value)
BOOST_AUTO_TEST_CASE(static_vector__push_back__1)
{
	using namespace wordring;

	static_vector<char, 16> sv{ '1', '2' };

	char const ch = '3';
	sv.push_back(ch);

	BOOST_CHECK(sv.size() == 3);

	std::string s(sv.begin(), sv.end());
	BOOST_CHECK(s == "123");
}

// void push_back(value_type&& value)
BOOST_AUTO_TEST_CASE(static_vector__push_back__2)
{
	using namespace wordring;

	static_vector<char, 16> sv{ '1', '2' };

	char const ch = '3';
	sv.push_back(std::move(ch));

	BOOST_CHECK(sv.size() == 3);

	std::string s(sv.begin(), sv.end());
	BOOST_CHECK(s == "123");
}

// void pop_back()
BOOST_AUTO_TEST_CASE(static_vector__pop_back__1)
{
	using namespace wordring;

	static_vector<char, 16> sv{ '1', '2', '3' };

	sv.pop_back();

	BOOST_CHECK(sv.size() == 2);

	std::string s(sv.begin(), sv.end());
	BOOST_CHECK(s == "12");
}

BOOST_AUTO_TEST_CASE(static_vector__pop_back__2)
{
	using namespace wordring;

	static_vector<char, 16> sv{ '1' };

	sv.pop_back();

	BOOST_CHECK(sv.size() == 0);

	std::string s(sv.begin(), sv.end());
	BOOST_CHECK(s == "");
}

// void resize(size_type count)
BOOST_AUTO_TEST_CASE(static_vector__resize__1)
{
	using namespace wordring;

	static_vector<char, 16> sv{ '1', '2', '3' };

	sv.resize(2);

	BOOST_CHECK(sv.size() == 2);

	std::string s(sv.begin(), sv.end());
	BOOST_CHECK(s == "12");
}

BOOST_AUTO_TEST_CASE(static_vector__resize__2)
{
	using namespace wordring;

	static_vector<char, 16> sv{ '1', '2', '3' };

	sv.resize(5);

	BOOST_CHECK(sv.size() == 5);

	std::string s1(sv.begin(), sv.end());
	std::string s2{ '1', '2', '3', '\0', '\0' };
	BOOST_CHECK(s1 == s2);
}

BOOST_AUTO_TEST_CASE(static_vector__resize__3)
{
	using namespace wordring;

	static_vector<char, 16> sv{ '1', '2', '3' };

	sv.resize(3);

	BOOST_CHECK(sv.size() == 3);

	std::string s(sv.begin(), sv.end());
	BOOST_CHECK(s == "123");
}

BOOST_AUTO_TEST_CASE(static_vector__resize__4)
{
	using namespace wordring;

	static_vector<char, 16> sv{ };

	sv.resize(0);

	BOOST_CHECK(sv.size() == 0);

	std::string s(sv.begin(), sv.end());
	BOOST_CHECK(s == "");
}

BOOST_AUTO_TEST_CASE(static_vector__resize__5)
{
	using namespace wordring;

	static_vector<char, 16> sv{ };

	sv.resize(2);

	BOOST_CHECK(sv.size() == 2);

	std::string s1(sv.begin(), sv.end());
	std::string s2{ '\0','\0' };
	BOOST_CHECK(s1 == s2);
}

// void resize(size_type count, value_type const& value)
BOOST_AUTO_TEST_CASE(static_vector__resize__6)
{
	using namespace wordring;

	static_vector<char, 16> sv{ '1', '2', '3' };
	char const ch = '5';

	sv.resize(2, ch);

	BOOST_CHECK(sv.size() == 2);

	std::string s(sv.begin(), sv.end());
	BOOST_CHECK(s == "12");
}

BOOST_AUTO_TEST_CASE(static_vector__resize__7)
{
	using namespace wordring;

	static_vector<char, 16> sv{ '1', '2', '3' };
	char const ch = '5';

	sv.resize(5, ch);

	BOOST_CHECK(sv.size() == 5);

	std::string s(sv.begin(), sv.end());
	BOOST_CHECK(s == "12355");
}

BOOST_AUTO_TEST_CASE(static_vector__resize__8)
{
	using namespace wordring;

	static_vector<char, 16> sv{ '1', '2', '3' };
	char const ch = '5';

	sv.resize(3, ch);

	BOOST_CHECK(sv.size() == 3);

	std::string s(sv.begin(), sv.end());
	BOOST_CHECK(s == "123");
}

BOOST_AUTO_TEST_CASE(static_vector__resize__9)
{
	using namespace wordring;

	static_vector<char, 16> sv{ };
	char const ch = '5';

	sv.resize(0, ch);

	BOOST_CHECK(sv.size() == 0);

	std::string s(sv.begin(), sv.end());
	BOOST_CHECK(s == "");
}

BOOST_AUTO_TEST_CASE(static_vector__resize__10)
{
	using namespace wordring;

	static_vector<char, 16> sv{ };
	char const ch = '5';

	sv.resize(2, ch);

	BOOST_CHECK(sv.size() == 2);

	std::string s(sv.begin(), sv.end());
	BOOST_CHECK(s == "55");
}

// void swap(static_vector& other)
BOOST_AUTO_TEST_CASE(static_vector__swap__1)
{
	using namespace wordring;

	static_vector<char, 16> sv1{ '1', '2', '3' };
	static_vector<char, 16> sv2{ '4', '5' };

	sv1.swap(sv2);

	BOOST_CHECK(sv1.size() == 2);
	BOOST_CHECK(sv2.size() == 3);

	std::string s1(sv1.begin(), sv1.end());
	std::string s2(sv2.begin(), sv2.end());
	BOOST_CHECK(s1 == "45");
	BOOST_CHECK(s2 == "123");
}

// 非メンバ関数 ----------------------------------------------------------------

// inline bool operator==(static_vector<T1, N1> const& lhs, static_vector<T1, N1> const& rhs)
BOOST_AUTO_TEST_CASE(static_vector__equal__1)
{
	using namespace wordring;

	static_vector<char, 16> sv1{ '1', '2', '3' };
	static_vector<char, 16> sv2{ '1', '2', '3' };

	BOOST_CHECK(sv1 == sv2);
}

BOOST_AUTO_TEST_CASE(static_vector__equal__2)
{
	using namespace wordring;

	static_vector<char, 16> sv1{};
	static_vector<char, 16> sv2{};

	BOOST_CHECK(sv1 == sv2);
}

BOOST_AUTO_TEST_CASE(static_vector__equal__3)
{
	using namespace wordring;

	static_vector<char, 16> sv1{ '1', '2', '3' };
	static_vector<char, 16> sv2{ '1', '2' };

	BOOST_CHECK((sv1 == sv2) == false);
}

BOOST_AUTO_TEST_CASE(static_vector__equal__4)
{
	using namespace wordring;

	static_vector<char, 16> sv1{};
	static_vector<char, 16> sv2{ '1', '2' };

	BOOST_CHECK((sv1 == sv2) == false);
}

// inline bool operator!=(static_vector<T1, N1> const& lhs, static_vector<T1, N1> const& rhs)
BOOST_AUTO_TEST_CASE(static_vector__not_equal__1)
{
	using namespace wordring;

	static_vector<char, 16> sv1{ '1', '2', '3' };
	static_vector<char, 16> sv2{ '1', '2', '3' };

	BOOST_CHECK((sv1 != sv2) == false);
}

BOOST_AUTO_TEST_CASE(static_vector__not_equal__2)
{
	using namespace wordring;

	static_vector<char, 16> sv1{};
	static_vector<char, 16> sv2{};

	BOOST_CHECK((sv1 != sv2) == false);
}

BOOST_AUTO_TEST_CASE(static_vector__not_equal__3)
{
	using namespace wordring;

	static_vector<char, 16> sv1{ '1', '2', '3' };
	static_vector<char, 16> sv2{ '1', '2' };

	BOOST_CHECK(sv1 != sv2);
}

BOOST_AUTO_TEST_CASE(static_vector__not_equal__4)
{
	using namespace wordring;

	static_vector<char, 16> sv1{};
	static_vector<char, 16> sv2{ '1', '2' };

	BOOST_CHECK(sv1 != sv2);
}

// inline bool operator<(static_vector<T1, N1> const& lhs, static_vector<T1, N1> const& rhs)
BOOST_AUTO_TEST_CASE(static_vector__less_than__1)
{
	using namespace wordring;

	static_vector<char, 16> sv1{ '1', '2', '3' };
	static_vector<char, 16> sv2{ '2' };

	BOOST_CHECK(sv1 < sv2);
}

BOOST_AUTO_TEST_CASE(static_vector__less_than__2)
{
	using namespace wordring;

	static_vector<char, 16> sv1{ '1', '2', '3' };
	static_vector<char, 16> sv2{ '1', '2', '3', '4' };

	BOOST_CHECK(sv1 < sv2);
}

BOOST_AUTO_TEST_CASE(static_vector__less_than__3)
{
	using namespace wordring;

	static_vector<char, 16> sv1{};
	static_vector<char, 16> sv2{ '1', '2', '3' };

	BOOST_CHECK(sv1 < sv2);
}

BOOST_AUTO_TEST_CASE(static_vector__less_than__4)
{
	using namespace wordring;

	static_vector<char, 16> sv1{};
	static_vector<char, 16> sv2{};

	BOOST_CHECK((sv1 < sv2) == false);
}

BOOST_AUTO_TEST_CASE(static_vector__less_than__5)
{
	using namespace wordring;

	static_vector<char, 16> sv1{ '1', '2', '3' };
	static_vector<char, 16> sv2{ '1', '2', '3' };

	BOOST_CHECK((sv1 < sv2) == false);
}

BOOST_AUTO_TEST_CASE(static_vector__less_than__6)
{
	using namespace wordring;

	static_vector<char, 16> sv1{ '1', '2', '3' };
	static_vector<char, 16> sv2{ '1', '2' };

	BOOST_CHECK((sv1 < sv2) == false);
}

BOOST_AUTO_TEST_CASE(static_vector__less_than__7)
{
	using namespace wordring;

	static_vector<char, 16> sv1{ '2' };
	static_vector<char, 16> sv2{ '1', '2', '3' };

	BOOST_CHECK((sv1 < sv2) == false);
}

// inline bool operator<=(static_vector<T1, N1> const& lhs, static_vector<T1, N1> const& rhs)
BOOST_AUTO_TEST_CASE(static_vector__less_than_or_equal__1)
{
	using namespace wordring;

	static_vector<char, 16> sv1{ '1', '2', '3' };
	static_vector<char, 16> sv2{ '2' };

	BOOST_CHECK(sv1 <= sv2);
}

BOOST_AUTO_TEST_CASE(static_vector__less_than_or_equal__2)
{
	using namespace wordring;

	static_vector<char, 16> sv1{ '1', '2', '3' };
	static_vector<char, 16> sv2{ '1', '2', '3' };

	BOOST_CHECK(sv1 <= sv2);
}

BOOST_AUTO_TEST_CASE(static_vector__less_than_or_equal__3)
{
	using namespace wordring;

	static_vector<char, 16> sv1{};
	static_vector<char, 16> sv2{};

	BOOST_CHECK(sv1 <= sv2);
}

// inline bool operator>(static_vector<T1, N1> const& lhs, static_vector<T1, N1> const& rhs)
BOOST_AUTO_TEST_CASE(static_vector__more_than__1)
{
	using namespace wordring;

	static_vector<char, 16> sv1{ '2' };
	static_vector<char, 16> sv2{ '1', '2', '3' };

	BOOST_CHECK(sv1 > sv2);
}

BOOST_AUTO_TEST_CASE(static_vector__more_than__2)
{
	using namespace wordring;

	static_vector<char, 16> sv1{ '1', '2', '3', '4' };
	static_vector<char, 16> sv2{ '1', '2', '3' };

	BOOST_CHECK(sv1 > sv2);
}

BOOST_AUTO_TEST_CASE(static_vector__more_than__3)
{
	using namespace wordring;

	static_vector<char, 16> sv1{ '1', '2', '3' };
	static_vector<char, 16> sv2{};

	BOOST_CHECK(sv1 > sv2);
}

BOOST_AUTO_TEST_CASE(static_vector__more_than__4)
{
	using namespace wordring;

	static_vector<char, 16> sv1{};
	static_vector<char, 16> sv2{};

	BOOST_CHECK((sv1 > sv2) == false);
}

BOOST_AUTO_TEST_CASE(static_vector__more_than__5)
{
	using namespace wordring;

	static_vector<char, 16> sv1{ '1', '2', '3' };
	static_vector<char, 16> sv2{ '1', '2', '3' };

	BOOST_CHECK((sv1 > sv2) == false);
}

BOOST_AUTO_TEST_CASE(static_vector__more_than__6)
{
	using namespace wordring;

	static_vector<char, 16> sv1{ '1', '2' };
	static_vector<char, 16> sv2{ '1', '2', '3' };

	BOOST_CHECK((sv1 > sv2) == false);
}

BOOST_AUTO_TEST_CASE(static_vector__more_than__7)
{
	using namespace wordring;

	static_vector<char, 16> sv1{ '1', '2', '3' };
	static_vector<char, 16> sv2{ '2' };

	BOOST_CHECK((sv1 > sv2) == false);
}

// inline bool operator>=(static_vector<T1, N1> const& lhs, static_vector<T1, N1> const& rhs)
BOOST_AUTO_TEST_CASE(static_vector__more_than_or_equal__1)
{
	using namespace wordring;

	static_vector<char, 16> sv1{ '2' };
	static_vector<char, 16> sv2{ '1', '2', '3' };

	BOOST_CHECK(sv1 >= sv2);
}

BOOST_AUTO_TEST_CASE(static_vector__more_than_or_equal__2)
{
	using namespace wordring;

	static_vector<char, 16> sv1{ '1', '2', '3' };
	static_vector<char, 16> sv2{ '1', '2', '3' };

	BOOST_CHECK(sv1 >= sv2);
}

BOOST_AUTO_TEST_CASE(static_vector__more_than_or_equal__3)
{
	using namespace wordring;

	static_vector<char, 16> sv1{};
	static_vector<char, 16> sv2{};

	BOOST_CHECK(sv1 >= sv2);
}

BOOST_AUTO_TEST_SUITE_END()
