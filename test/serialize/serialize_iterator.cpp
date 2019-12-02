// test/serialize/serialize_iterator.cpp

#include <boost/test/unit_test.hpp>

#include <deque>
#include <iterator>
#include <sstream>
#include <string>
#include <string_view>

#include <wordring/serialize/serialize_iterator.hpp>

BOOST_AUTO_TEST_SUITE(serialize_iterator__test)

BOOST_AUTO_TEST_CASE(serialize_iterator__constrcut__1)
{
	using namespace wordring;

	std::istringstream stream{};
	auto it1 = std::istream_iterator<char>(stream);
	auto it2 = std::istream_iterator<char>();
	serialize_iterator<std::istream_iterator<char>> first(it1);
	serialize_iterator<std::istream_iterator<char>> last(it2);
	BOOST_CHECK(it1 == it2);
	BOOST_CHECK(first == last);
}

BOOST_AUTO_TEST_CASE(serialize_iterator__constrcut__2)
{
	using namespace wordring;

	std::deque<char16_t> c{};
	serialize_iterator<decltype(c.begin())> first(c.begin());
	serialize_iterator<decltype(c.end())> last(c.end());
	BOOST_CHECK(first == last);
}

BOOST_AUTO_TEST_CASE(serialize_iterator__constrcut__3)
{
	using namespace wordring;

	std::u16string c{ u"" };
	serialize_iterator<decltype(c.begin())> first(c.begin());
	serialize_iterator<decltype(c.end())> last(c.end());
	BOOST_CHECK(first == last);
}

BOOST_AUTO_TEST_CASE(serialize_iterator__constrcut__4)
{
	using namespace wordring;

	std::u16string c{ u"あ" };
	serialize_iterator<decltype(c.begin())> first(c.begin());
	serialize_iterator<decltype(c.end())> last(c.end());
	BOOST_CHECK(first != last);
}

BOOST_AUTO_TEST_CASE(serialize_iterator__constrcut__5)
{
	std::u16string c{ u"あ" };
	wordring::serialize_iterator<decltype(c.begin())> it1(c.begin());
	wordring::serialize_iterator<decltype(c.end())> it2(c.end());
	auto it3(it1);
	auto it4(it2);
	BOOST_CHECK(it1 != it2);
	BOOST_CHECK(it3 != it4);
	BOOST_CHECK(it1 == it3);
	BOOST_CHECK(it2 == it4);
}

// テンプレート引数にポインタを使う。
BOOST_AUTO_TEST_CASE(serialize_iterator__constrcut__6)
{
	auto s = u"あ";
	std::u16string_view sv{ s };
	wordring::serialize_iterator<decltype(s)> it1(s);
	wordring::serialize_iterator<decltype(s)> it2(s + sv.size());
	auto it3(it1);
	auto it4(it2);
	BOOST_CHECK(it1 != it2);
	BOOST_CHECK(it3 != it4);
	BOOST_CHECK(it1 == it3);
	BOOST_CHECK(it2 == it4);
}

BOOST_AUTO_TEST_CASE(serialize_iterator__dereference__1)
{
	std::u16string c{ u"あ" };
	wordring::serialize_iterator<decltype(c.begin())> it1(c.begin());
	wordring::serialize_iterator<decltype(c.end())> it2(c.end());
	BOOST_CHECK(*it1 == 0x30u);
}

BOOST_AUTO_TEST_CASE(serialize_iterator__increment__1)
{
	std::u16string c{ u"あaい" };
	wordring::serialize_iterator<decltype(c.begin())> it1(c.begin());
	wordring::serialize_iterator<decltype(c.end())> it2(c.end());
	BOOST_CHECK(*it1 == 0x30u);
	++it1;
	BOOST_CHECK(*it1 == 0x42u);
	++it1;
	BOOST_CHECK(*it1 == 0x0u);
	++it1;
	BOOST_CHECK(*it1 == 0x61u);
	++it1;
	BOOST_CHECK(*it1 == 0x30u);
	++it1;
	BOOST_CHECK(*it1 == 0x44u);
	++it1;

	BOOST_CHECK(it1 == it2);
}

BOOST_AUTO_TEST_CASE(serialize_iterator__increment__2)
{
	std::u16string c{ u"あaい" };
	wordring::serialize_iterator<decltype(c.begin())> it1(c.begin());
	wordring::serialize_iterator<decltype(c.end())> it2(c.end());
	BOOST_CHECK(*it1++ == 0x30u);
	BOOST_CHECK(*it1++ == 0x42u);
	BOOST_CHECK(*it1++ == 0x0u);
	BOOST_CHECK(*it1++ == 0x61u);
	BOOST_CHECK(*it1++ == 0x30u);
	BOOST_CHECK(*it1++ == 0x44u);

	BOOST_CHECK(it1 == it2);
}

BOOST_AUTO_TEST_SUITE_END()
