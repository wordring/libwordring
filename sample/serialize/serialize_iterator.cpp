// sample/serialize/serialize_iterator.cpp

#include <boost/test/unit_test.hpp>

#include <wordring/serialize/serialize_iterator.hpp>

#include <cassert>
#include <iostream>
#include <string>
#include <vector>

BOOST_AUTO_TEST_SUITE(serialize_iterator__sample)

/*
説明用サンプル
*/
BOOST_AUTO_TEST_CASE(deserialize_iterator__concept__1)
{
	using namespace wordring;

	std::string s{ '\x1', '\x23', '\x45', '\x67', '\x89', '\xAB','\xCD', '\xEF' };

	auto it1 = deserialize_iterator<std::uint32_t, decltype(s.begin())>(s.begin());
	auto it2 = deserialize_iterator<std::uint32_t, decltype(s.begin())>(s.end());

	std::cout << std::hex;
	while (it1 != it2) std::cout << *it1++ << std::endl;
}

/*
説明用サンプル
*/
BOOST_AUTO_TEST_CASE(serialize_iterator__concept__1)
{
	using namespace wordring;

	std::vector<std::uint32_t> v{ 0x1234567u, 0x89ABCDEFu };

	auto it1 = serialize_iterator(v.begin());
	auto it2 = serialize_iterator(v.end());

	std::cout << std::hex;
	while (it1 != it2) std::cout << static_cast<int>(*it1++) << std::endl;
}

BOOST_AUTO_TEST_SUITE_END()
