// test/serialize/serialize.cpp

#include <boost/test/unit_test.hpp>

#include <array>

#include <wordring/serialize/serialize.hpp>

BOOST_AUTO_TEST_SUITE(serialize_test)

BOOST_AUTO_TEST_CASE(serialize_1)
{
	using namespace wordring;

	BOOST_CHECK(((0xAABBCCDDu >> 24) & 0xFF) == 0xAA);
	BOOST_CHECK(((0xAABBCCDDu >> 16) & 0xFF) == 0xBB);
	BOOST_CHECK(((0xAABBCCDDu >> 8) & 0xFF) == 0xCC);
	BOOST_CHECK(((0xAABBCCDDu >> 0) & 0xFF) == 0xDD);

	BOOST_CHECK((serialize(static_cast<std::uint8_t>(0xAA)) == std::array<std::uint8_t, 1>{ 0xAA }));
	BOOST_CHECK((serialize(static_cast<std::uint16_t>(0xAABB)) == std::array<std::uint8_t, 2>{ 0xAA, 0xBB }));
	BOOST_CHECK((serialize(static_cast<std::uint32_t>(0xAABBCCDD)) == std::array<std::uint8_t, 4>{ 0xAA, 0xBB, 0xCC, 0xDD }));
	BOOST_CHECK((serialize(static_cast<std::int8_t>(0xA)) == std::array<std::uint8_t, 1>{ 0xA }));
	BOOST_CHECK((serialize(static_cast<std::int16_t>(0xABB)) == std::array<std::uint8_t, 2>{ 0xA, 0xBB }));
	BOOST_CHECK((serialize(static_cast<std::int32_t>(0xABBCCDD)) == std::array<std::uint8_t, 4>{ 0xA, 0xBB, 0xCC, 0xDD }));
}

BOOST_AUTO_TEST_CASE(serialize_2)
{
	using namespace wordring;

	BOOST_CHECK(serialize(static_cast<std::uint8_t>(0xAA), 0) == 0xAA);

	BOOST_CHECK(serialize(static_cast<std::uint16_t>(0xAABB), 0) == 0xAA);
	BOOST_CHECK(serialize(static_cast<std::uint16_t>(0xAABB), 1) == 0xBB);

	BOOST_CHECK(serialize(static_cast<std::uint32_t>(0xAABBCCDD), 0) == 0xAA);
	BOOST_CHECK(serialize(static_cast<std::uint32_t>(0xAABBCCDD), 1) == 0xBB);
	BOOST_CHECK(serialize(static_cast<std::uint32_t>(0xAABBCCDD), 2) == 0xCC);
	BOOST_CHECK(serialize(static_cast<std::uint32_t>(0xAABBCCDD), 3) == 0xDD);

	BOOST_CHECK(serialize(static_cast<std::int8_t>(0xA), 0) == 0xA);

	BOOST_CHECK(serialize(static_cast<std::int16_t>(0xABB), 0) == 0xA);
	BOOST_CHECK(serialize(static_cast<std::int16_t>(0xABB), 1) == 0xBB);

	BOOST_CHECK(serialize(static_cast<std::int32_t>(0xABBCCDD), 0) == 0xA);
	BOOST_CHECK(serialize(static_cast<std::int32_t>(0xABBCCDD), 1) == 0xBB);
	BOOST_CHECK(serialize(static_cast<std::int32_t>(0xABBCCDD), 2) == 0xCC);
	BOOST_CHECK(serialize(static_cast<std::int32_t>(0xABBCCDD), 3) == 0xDD);
}

BOOST_AUTO_TEST_SUITE_END()
