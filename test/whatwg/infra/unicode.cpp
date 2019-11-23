
#include <boost/test/unit_test.hpp>

#include <string>

#include <wordring/whatwg/infra/unicode.hpp>

BOOST_AUTO_TEST_SUITE(whatwg_infra__unicode_test)

#pragma warning(push)
#pragma warning(disable:4566)
std::u8string const utf_8{ u8"ABCDEあいうえお𠀋𡈽𡌛𡑮𡢽AÀⱥ𐊠" };
std::u16string const utf_16{ u"ABCDEあいうえお𠀋𡈽𡌛𡑮𡢽AÀⱥ𐊠" };
std::u32string const utf_32{ U"ABCDEあいうえお𠀋𡈽𡌛𡑮𡢽AÀⱥ𐊠" };
#pragma warning(pop)

// utf-8
BOOST_AUTO_TEST_CASE(to_code_point__1)
{
	using namespace wordring::whatwg;

	std::u32string out{};
	uint32_t cp{};

	auto it = utf_8.begin();
	while (it != utf_8.end())
	{
		it = to_code_point(it, utf_8.end(), cp);
		out.push_back(cp);
	}
	BOOST_CHECK(out == utf_32);
}

// UTF-16
BOOST_AUTO_TEST_CASE(to_code_point__2)
{
	using namespace wordring::whatwg;

	std::u32string out{};
	uint32_t cp{};

	auto it = utf_16.begin();
	while (it != utf_16.end())
	{
		it = to_code_point(it, utf_16.end(), cp);
		out.push_back(cp);
	}
	BOOST_CHECK(out == utf_32);
}

// UTF-32
BOOST_AUTO_TEST_CASE(to_code_point__3)
{
	using namespace wordring::whatwg;

	std::u32string out{};
	uint32_t cp{};

	auto it = utf_32.begin();
	while (it != utf_32.end())
	{
		it = to_code_point(it, utf_32.end(), cp);
		out.push_back(cp);
	}
	BOOST_CHECK(out == utf_32);
}

// ----------------------------------------------------------------------------

// UTF-8
BOOST_AUTO_TEST_CASE(to_string__1)
{
	using namespace wordring::whatwg;

	std::u8string out{};
	for (auto cp : utf_32) to_string(cp, std::back_inserter(out));
	BOOST_CHECK(out == utf_8);
}

// UTF-16
BOOST_AUTO_TEST_CASE(to_string__2)
{
	using namespace wordring::whatwg;

	std::u16string out{};
	for (auto cp : utf_32) to_string(cp, std::back_inserter(out));
	BOOST_CHECK(out == utf_16);
}

// UTF-32
BOOST_AUTO_TEST_CASE(to_string__3)
{
	using namespace wordring::whatwg;

	std::u32string out{};
	for (auto cp : utf_32) to_string(cp, std::back_inserter(out));
	BOOST_CHECK(out == utf_32);
}

// ----------------------------------------------------------------------------

// UTF-8 -> UTF-8
BOOST_AUTO_TEST_CASE(encoding_cast__1)
{
	using namespace wordring::whatwg;
	BOOST_CHECK(encoding_cast<std::u8string>(utf_8) == utf_8);
}

// UTF-8 -> UTF-16
BOOST_AUTO_TEST_CASE(encoding_cast__2)
{
	using namespace wordring::whatwg;
	BOOST_CHECK(encoding_cast<std::u16string>(utf_8) == utf_16);
}

// UTF-8 -> UTF-32
BOOST_AUTO_TEST_CASE(encoding_cast__3)
{
	using namespace wordring::whatwg;
	BOOST_CHECK(encoding_cast<std::u32string>(utf_8) == utf_32);
}

// UTF-16 -> UTF-8
BOOST_AUTO_TEST_CASE(encoding_cast__4)
{
	using namespace wordring::whatwg;
	BOOST_CHECK(encoding_cast<std::u8string>(utf_16) == utf_8);
}

// UTF-16 -> UTF-16
BOOST_AUTO_TEST_CASE(encoding_cast__5)
{
	using namespace wordring::whatwg;
	BOOST_CHECK(encoding_cast<std::u16string>(utf_16) == utf_16);
}

// UTF-16-> UTF-32
BOOST_AUTO_TEST_CASE(encoding_cast__6)
{
	using namespace wordring::whatwg;
	BOOST_CHECK(encoding_cast<std::u32string>(utf_16) == utf_32);
}

// UTF-32 -> UTF-8
BOOST_AUTO_TEST_CASE(encoding_cast__7)
{
	using namespace wordring::whatwg;
	BOOST_CHECK(encoding_cast<std::u8string>(utf_32) == utf_8);
}

// UTF-32 -> UTF-16
BOOST_AUTO_TEST_CASE(encoding_cast__8)
{
	using namespace wordring::whatwg;
	BOOST_CHECK(encoding_cast<std::u16string>(utf_32) == utf_16);
}

// UTF-32 -> UTF-32
BOOST_AUTO_TEST_CASE(encoding_cast__9)
{
	using namespace wordring::whatwg;
	BOOST_CHECK(encoding_cast<std::u32string>(utf_32) == utf_32);
}

BOOST_AUTO_TEST_SUITE_END()

