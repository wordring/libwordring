// test/serialize/serialize_iterator.cpp

#include <boost/test/unit_test.hpp>

#include <wordring/serialize/serialize_iterator.hpp>

#include <algorithm>
#include <deque>
#include <iterator>
#include <sstream>
#include <string>
#include <string_view>

BOOST_AUTO_TEST_SUITE(serialize_iterator__test)

// ----------------------------------------------------------------------------
// serialize_iterator
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(serialize_iterator__constrcut__1)
{
	using namespace wordring;

	std::istringstream stream{};
	auto it1 = std::istream_iterator<char>(stream);
	auto it2 = std::istream_iterator<char>();

	auto it3 = serialize_iterator(it1);
	auto it4 = serialize_iterator(it2);
	
	BOOST_CHECK(it1 == it2);
	BOOST_CHECK(it3 == it4);
}

BOOST_AUTO_TEST_CASE(serialize_iterator__constrcut__2)
{
	using namespace wordring;

	std::deque<char16_t> c{};

	auto it1 = serialize_iterator(c.begin());
	auto it2 = serialize_iterator(c.end());
	
	BOOST_CHECK(it1 == it2);
}

BOOST_AUTO_TEST_CASE(serialize_iterator__constrcut__3)
{
	using namespace wordring;

	auto s = std::u16string(u"");

	auto it1 = serialize_iterator(s.begin());
	auto it2 = serialize_iterator(s.end());
	BOOST_CHECK(it1 == it2);
}

BOOST_AUTO_TEST_CASE(serialize_iterator__constrcut__4)
{
	using namespace wordring;

	auto s = std::u16string(u"あ");

	auto it1 = serialize_iterator(s.begin());
	auto it2 = serialize_iterator(s.end());

	BOOST_CHECK(it1 != it2);
}

BOOST_AUTO_TEST_CASE(serialize_iterator__constrcut__5)
{
	using namespace wordring;

	auto s = std::u16string(u"あ");

	auto it1 = serialize_iterator(s.begin());
	auto it2 = serialize_iterator(s.end());

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
	using namespace wordring;

	auto s = u"あ";
	auto sv = std::u16string_view(s);
	
	auto it1 = serialize_iterator(s);
	auto it2 = serialize_iterator(s + sv.size());

	auto it3(it1);
	auto it4(it2);
	
	BOOST_CHECK(it1 != it2);
	BOOST_CHECK(it3 != it4);
	BOOST_CHECK(it1 == it3);
	BOOST_CHECK(it2 == it4);
}

BOOST_AUTO_TEST_CASE(serialize_iterator__reference__1)
{
	using namespace wordring;

	auto s1 = std::u32string(U"あaい");

	auto it1 = serialize_iterator(s1.begin());
	auto it2 = serialize_iterator(s1.end());

	auto s2 = std::string(it1, it2);

	std::string s3{ 0, 0, 0x30u, 0x42u, 0, 0, 0, 0x61u, 0, 0, 0x30u, 0x44u };

	BOOST_CHECK(s2 == s3);
}

BOOST_AUTO_TEST_CASE(serialize_iterator__reference__2)
{
	using namespace wordring;

	auto s1 = std::u16string(u"あaい");

	auto it1 = serialize_iterator(s1.begin());
	auto it2 = serialize_iterator(s1.end());

	auto s2 = std::string(it1, it2);

	std::string s3{ 0x30u, 0x42u, 0x0u, 0x61u, 0x30u, 0x44u };

	BOOST_CHECK(s2 == s3);
}

BOOST_AUTO_TEST_CASE(serialize_iterator__reference__3)
{
	using namespace wordring;

	auto s1 = std::string("abc");

	auto it1 = serialize_iterator(s1.begin());
	auto it2 = serialize_iterator(s1.end());

	auto s2 = std::string(it1, it2);

	BOOST_CHECK(s1 == s2);
}

BOOST_AUTO_TEST_CASE(serialize_iterator__increment__1)
{
	using namespace wordring;

	auto c = std::u16string(u"あaい");

	auto it1 = serialize_iterator(c.begin());
	auto it2 = serialize_iterator(c.end());

	auto v1 = std::vector<char>(it1, it2);

	std::vector<char> v2{ 0x30u, 0x42u, 0x0u, 0x61u, 0x30u, 0x44u };

	BOOST_CHECK(v1 == v2);
}

BOOST_AUTO_TEST_CASE(serialize_iterator__increment__2)
{
	using namespace wordring;

	auto c = std::u16string(u"あaい");

	auto it1 = serialize_iterator(c.begin());
	auto it2 = serialize_iterator(c.end());

	std::vector<char> v1;
	while (it1 != it2) v1.push_back(*it1++);

	std::vector<char> v2{ 0x30u, 0x42u, 0x0u, 0x61u, 0x30u, 0x44u };

	BOOST_CHECK(v1 == v2);
}

// ----------------------------------------------------------------------------
// deserialize_iterator
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(deserialize_iterator__constrcut__1)
{
	using namespace wordring;
	using test_iterator = deserialize_iterator<uint16_t, std::vector<std::uint8_t>::iterator>;

	std::vector<std::uint8_t> v1{ 0x30u, 0x42u, 0x0u, 0x61u, 0x30u, 0x44u };

	auto it1 = test_iterator(v1.begin());
	auto it2 = test_iterator(v1.end());

	BOOST_CHECK(it1 != it2);
}

BOOST_AUTO_TEST_CASE(deserialize_iterator__reference__1)
{
	using namespace wordring;
	using test_iterator = deserialize_iterator<char32_t, std::vector<std::uint8_t>::iterator>;

	std::vector<std::uint8_t> v1{ 0, 0, 0x30u, 0x42u, 0, 0, 0, 0x61u, 0, 0, 0x30u, 0x44u };

	auto it1 = test_iterator(v1.begin());
	auto it2 = test_iterator(v1.end());

	std::u32string s1;
	std::copy(it1, it2, std::back_inserter(s1));

	auto s2 = std::u32string(U"あaい");

	BOOST_CHECK(s1 == s2);
}

BOOST_AUTO_TEST_CASE(deserialize_iterator__reference__2)
{
	using namespace wordring;
	using test_iterator = deserialize_iterator<char16_t, std::vector<std::uint8_t>::iterator>;

	std::vector<std::uint8_t> v1{ 0x30u, 0x42u, 0x0u, 0x61u, 0x30u, 0x44u };

	auto it1 = test_iterator(v1.begin());
	auto it2 = test_iterator(v1.end());

	std::u16string s1;
	std::copy(it1, it2, std::back_inserter(s1));

	auto s2 = std::u16string(u"あaい");

	BOOST_CHECK(s1 == s2);
}

BOOST_AUTO_TEST_CASE(deserialize_iterator__reference__3)
{
	using namespace wordring;
	using test_iterator = deserialize_iterator<std::uint8_t, std::vector<std::uint8_t>::iterator>;

	std::vector<std::uint8_t> v1{ 1, 2, 3, 4 };

	auto it1 = test_iterator(v1.begin());
	auto it2 = test_iterator(v1.end());

	std::vector<std::uint8_t> v2;
	std::copy(it1, it2, std::back_inserter(v2));

	BOOST_CHECK(v1 == v2);
}

// ----------------------------------------------------------------------------
// mix test
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(mix__1)
{
	using namespace wordring;
	using test_iterator = deserialize_iterator<std::uint32_t, std::string::iterator>;

	auto s1 = std::u32string(U"あaい");
	
	auto it1 = serialize_iterator(s1.begin());
	auto it2 = serialize_iterator(s1.end());

	auto s2 = std::string(it1, it2);

	auto it3 = test_iterator(s2.begin());
	auto it4 = test_iterator(s2.end());

	auto s3 = std::u32string(it3, it4);

	BOOST_CHECK(s1 == s3);
}


BOOST_AUTO_TEST_SUITE_END()
