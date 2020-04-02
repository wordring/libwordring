// test/whatwg/html/parsing/atom_tbl.cpp

#include <boost/test/unit_test.hpp>

#include <wordring/whatwg/html/parsing/atom_tbl.hpp>

#include <algorithm>
#include <iterator>

BOOST_AUTO_TEST_SUITE(whatwg__html__parsing__atom_tbl__test)

BOOST_AUTO_TEST_CASE(atom_tbl__named_character_reference_idx_tbl__1)
{
	using namespace wordring::whatwg::html::parsing;

	auto key = std::u32string_view(U"xodot;");
	auto pair = named_character_reference_idx_tbl.lookup(key.begin(), key.end());

	BOOST_CHECK(pair.first != named_character_reference_idx_tbl.begin());
}

BOOST_AUTO_TEST_CASE(atom_tbl__named_character_reference_idx_tbl__2)
{
	using namespace wordring::whatwg::html::parsing;

	auto key = std::u32string_view(U"xod");
	auto pair = named_character_reference_idx_tbl.lookup(key.begin(), key.end());
	bool partial_match = pair.second == key.end() && !pair.first;

	BOOST_CHECK(partial_match);
}

BOOST_AUTO_TEST_CASE(atom_tbl__named_character_reference_idx_tbl__3)
{
	using namespace wordring::whatwg::html::parsing;

	auto key = std::u32string_view(U"xodot;ABCDEFG");
	auto pair = named_character_reference_idx_tbl.lookup(key.begin(), key.end());
	bool match = static_cast<bool>(pair.first);

	BOOST_CHECK(match);
}

BOOST_AUTO_TEST_CASE(atom_tbl__named_character_reference_idx_tbl__4)
{
	using namespace wordring::whatwg::html::parsing;

	auto key = std::u32string_view(U"xodot;ABCDEFG");
	auto pair = named_character_reference_idx_tbl.lookup(key.begin(), key.end());
	bool full_match = pair.second == key.end() && static_cast<bool>(pair.first);

	BOOST_CHECK(full_match == false);
}

BOOST_AUTO_TEST_CASE(atom_tbl__named_character_reference_idx_tbl__5)
{
	using namespace wordring::whatwg::html::parsing;

	auto key = std::u32string_view(U"xodot;");
	auto pair = named_character_reference_idx_tbl.lookup(key.begin(), key.end());
	bool full_match = pair.second == key.end() && static_cast<bool>(pair.first);

	BOOST_CHECK(full_match);
}

BOOST_AUTO_TEST_CASE(atom_tbl__named_character_reference_idx_tbl__6)
{
	using namespace wordring::whatwg::html::parsing;

	auto key = std::u32string_view(U"あ");
	auto it = named_character_reference_idx_tbl.search(key.begin(), key.end());

	bool match_failed = it == named_character_reference_idx_tbl.begin()
		|| it == named_character_reference_idx_tbl.end();

	BOOST_CHECK(match_failed);
}

BOOST_AUTO_TEST_CASE(atom_tbl__named_character_reference_idx_tbl__7)
{
	using namespace wordring::whatwg::html::parsing;

	auto key = std::u32string_view(U"xodあ");
	auto it = named_character_reference_idx_tbl.search(key.begin(), key.end());

	bool match_failed = it == named_character_reference_idx_tbl.begin()
		|| it == named_character_reference_idx_tbl.end();

	BOOST_CHECK(match_failed);
}

BOOST_AUTO_TEST_CASE(atom_tbl__named_character_reference_idx_tbl__8)
{
	using namespace wordring::whatwg::html::parsing;

	auto key = std::u32string_view(U"xod");
	auto it = named_character_reference_idx_tbl.search(key.begin(), key.end());

	bool match_failed = it == named_character_reference_idx_tbl.begin()
		|| it == named_character_reference_idx_tbl.end();

	BOOST_CHECK(!match_failed);
}

BOOST_AUTO_TEST_CASE(atom_tbl__named_character_reference_idx_tbl__9)
{
	using namespace wordring::whatwg::html::parsing;

	auto key = std::u32string_view(U"xodot;");
	auto it = named_character_reference_idx_tbl.search(key.begin(), key.end());

	bool match_failed = it == named_character_reference_idx_tbl.begin()
		|| it == named_character_reference_idx_tbl.end();

	BOOST_CHECK(!match_failed);
}

BOOST_AUTO_TEST_CASE(atom_tbl__named_character_reference_map_tbl__1)
{
	using namespace wordring::whatwg::html::parsing;

	auto key = std::u32string_view(U"Aacute;");
	auto pair = named_character_reference_idx_tbl.lookup(key.begin(), key.end());
	auto idx = named_character_reference_idx_tbl.at(pair.first);
	auto a = named_character_reference_map_tbl.at(idx);

	BOOST_CHECK(a[0] == 0x000C1);
	BOOST_CHECK(a[1] == 0);
}

BOOST_AUTO_TEST_SUITE_END()
