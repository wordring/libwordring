// test/whatwg/html/parsing/atom_tbl.cpp

#include <boost/test/unit_test.hpp>

#include <wordring/whatwg/html/parsing/atom_tbl.hpp>

#include <wordring/whatwg/html/parsing/atom_defs.hpp>

#include <algorithm>
#include <iterator>

BOOST_AUTO_TEST_SUITE(whatwg_html_parsing_atom_tbl_test)

/*
extern wordring::trie<char32_t> const tag_atom_tbl;
*/
BOOST_AUTO_TEST_CASE(atom_tbl_tag_atom_tbl_1)
{
	using namespace wordring::whatwg::html::parsing;

	BOOST_CHECK(tag_atom_tbl.find(std::u32string(U"a")) != tag_atom_tbl.end());
	BOOST_CHECK(tag_atom_tbl.at(std::u32string(U"a")) == tag_name::A);
}

BOOST_AUTO_TEST_CASE(atom_tbl_tag_atom_tbl_2)
{
	using namespace wordring::whatwg::html::parsing;

	BOOST_CHECK(tag_atom_tbl.find(std::u32string(U"xor")) != tag_atom_tbl.end());
	BOOST_CHECK(tag_atom_tbl.at(std::u32string(U"xor")) == tag_name::Xor);
}

BOOST_AUTO_TEST_CASE(atom_tbl_tag_atom_tbl_3)
{
	using namespace wordring::whatwg::html::parsing;

	BOOST_CHECK(tag_atom_tbl.find(std::u32string(U"altGlyph")) != tag_atom_tbl.end());
	BOOST_CHECK(tag_atom_tbl.at(std::u32string(U"altGlyph")) == tag_name::AltGlyph);
}

BOOST_AUTO_TEST_CASE(atom_tbl_tag_atom_tbl_4)
{
	using namespace wordring::whatwg::html::parsing;

	BOOST_CHECK(tag_atom_tbl.find(std::u32string(U"altglyph")) != tag_atom_tbl.end());
	BOOST_CHECK(tag_atom_tbl.at(std::u32string(U"altglyph")) == tag_name::Altglyph);
}

/*
extern wordring::trie<char32_t> const attribute_atom_tbl;
*/
BOOST_AUTO_TEST_CASE(atom_tbl_attribute_atom_tbl_1)
{
	using namespace wordring::whatwg::html::parsing;

	BOOST_CHECK(attribute_atom_tbl.find(std::u32string(U"abbr")) != attribute_atom_tbl.end());
	BOOST_CHECK(attribute_atom_tbl.at(std::u32string(U"abbr")) == attribute_name::Abbr);
}

BOOST_AUTO_TEST_CASE(atom_tbl_attribute_atom_tbl_2)
{
	using namespace wordring::whatwg::html::parsing;

	BOOST_CHECK(attribute_atom_tbl.find(std::u32string(U"zoomAndPan")) != attribute_atom_tbl.end());
	BOOST_CHECK(attribute_atom_tbl.at(std::u32string(U"zoomAndPan")) == attribute_name::ZoomAndPan);
}

BOOST_AUTO_TEST_CASE(atom_tbl_attribute_atom_tbl_3)
{
	using namespace wordring::whatwg::html::parsing;

	BOOST_CHECK(attribute_atom_tbl.find(std::u32string(U"zoomandpan")) != attribute_atom_tbl.end());
	BOOST_CHECK(attribute_atom_tbl.at(std::u32string(U"zoomandpan")) == attribute_name::Zoomandpan);
}

/*
extern std::array<std::u32string, 406> const tag_name_tbl;
*/
BOOST_AUTO_TEST_CASE(atom_tbl_tag_name_tbl_1)
{
	using namespace wordring::whatwg::html::parsing;

	BOOST_CHECK(tag_name_tbl[1] == U"a");
}

/*
extern std::array<std::u32string, 634> const attribute_name_tbl;
*/
BOOST_AUTO_TEST_CASE(atom_tbl_attribute_name_tbl_1)
{
	using namespace wordring::whatwg::html::parsing;

	BOOST_CHECK(attribute_name_tbl[1] == U"abbr");
	BOOST_CHECK(attribute_name_tbl[633] == U"zoomandpan");
}

/*
extern std::array<std::u32string, 7> const ns_uri_tbl;
*/
BOOST_AUTO_TEST_CASE(atom_tbl_ns_uri_tbl_1)
{
	using namespace wordring::whatwg::html::parsing;

	BOOST_CHECK(ns_uri_tbl[1] == U"http://www.w3.org/1999/xhtml");
	BOOST_CHECK(ns_uri_tbl[6] == U"http://www.w3.org/2000/xmlns/");
}

/*
extern wordring::trie<char32_t> const named_character_reference_idx_tbl;
*/
BOOST_AUTO_TEST_CASE(atom_tbl_named_character_reference_idx_tbl_1)
{
	using namespace wordring::whatwg::html::parsing;

	auto key = std::u32string_view(U"xodot;");
	auto pair = named_character_reference_idx_tbl.lookup(key.begin(), key.end());

	BOOST_CHECK(pair.first != named_character_reference_idx_tbl.begin());
}

BOOST_AUTO_TEST_CASE(atom_tbl_named_character_reference_idx_tbl_2)
{
	using namespace wordring::whatwg::html::parsing;

	auto key = std::u32string_view(U"xod");
	auto pair = named_character_reference_idx_tbl.lookup(key.begin(), key.end());
	bool partial_match = pair.second == key.end() && !pair.first;

	BOOST_CHECK(partial_match);
}

BOOST_AUTO_TEST_CASE(atom_tbl_named_character_reference_idx_tbl_3)
{
	using namespace wordring::whatwg::html::parsing;

	auto key = std::u32string_view(U"xodot;ABCDEFG");
	auto pair = named_character_reference_idx_tbl.lookup(key.begin(), key.end());
	bool match = static_cast<bool>(pair.first);

	BOOST_CHECK(match);
}

BOOST_AUTO_TEST_CASE(atom_tbl_named_character_reference_idx_tbl_4)
{
	using namespace wordring::whatwg::html::parsing;

	auto key = std::u32string_view(U"xodot;ABCDEFG");
	auto pair = named_character_reference_idx_tbl.lookup(key.begin(), key.end());
	bool full_match = pair.second == key.end() && static_cast<bool>(pair.first);

	BOOST_CHECK(full_match == false);
}

BOOST_AUTO_TEST_CASE(atom_tbl_named_character_reference_idx_tbl_5)
{
	using namespace wordring::whatwg::html::parsing;

	auto key = std::u32string_view(U"xodot;");
	auto pair = named_character_reference_idx_tbl.lookup(key.begin(), key.end());
	bool full_match = pair.second == key.end() && static_cast<bool>(pair.first);

	BOOST_CHECK(full_match);
}

BOOST_AUTO_TEST_CASE(atom_tbl_named_character_reference_idx_tbl_6)
{
	using namespace wordring::whatwg::html::parsing;

	auto key = std::u32string_view(U"あ");
	auto it = named_character_reference_idx_tbl.search(key.begin(), key.end());

	bool match_failed = it == named_character_reference_idx_tbl.begin()
		|| it == named_character_reference_idx_tbl.end();

	BOOST_CHECK(match_failed);
}

BOOST_AUTO_TEST_CASE(atom_tbl_named_character_reference_idx_tbl_7)
{
	using namespace wordring::whatwg::html::parsing;

	auto key = std::u32string_view(U"xodあ");
	auto it = named_character_reference_idx_tbl.search(key.begin(), key.end());

	bool match_failed = it == named_character_reference_idx_tbl.begin()
		|| it == named_character_reference_idx_tbl.end();

	BOOST_CHECK(match_failed);
}

BOOST_AUTO_TEST_CASE(atom_tbl_named_character_reference_idx_tbl_8)
{
	using namespace wordring::whatwg::html::parsing;

	auto key = std::u32string_view(U"xod");
	auto it = named_character_reference_idx_tbl.search(key.begin(), key.end());

	bool match_failed = it == named_character_reference_idx_tbl.begin()
		|| it == named_character_reference_idx_tbl.end();

	BOOST_CHECK(!match_failed);
}

BOOST_AUTO_TEST_CASE(atom_tbl_named_character_reference_idx_tbl_9)
{
	using namespace wordring::whatwg::html::parsing;

	auto key = std::u32string_view(U"xodot;");
	auto it = named_character_reference_idx_tbl.search(key.begin(), key.end());

	bool match_failed = it == named_character_reference_idx_tbl.begin()
		|| it == named_character_reference_idx_tbl.end();

	BOOST_CHECK(!match_failed);
}

/*
extern std::array<std::array<char32_t, 2>, 2231> const named_character_reference_map_tbl;
*/
BOOST_AUTO_TEST_CASE(atom_tbl_named_character_reference_map_tbl_1)
{
	using namespace wordring::whatwg::html::parsing;

	auto key = std::u32string_view(U"Aacute;");
	auto pair = named_character_reference_idx_tbl.lookup(key.begin(), key.end());
	auto idx = named_character_reference_idx_tbl.at(pair.first);
	auto a = named_character_reference_map_tbl.at(idx);

	BOOST_CHECK(a[0] == 0x000C1);
	BOOST_CHECK(a[1] == 0);
}

/*
extern std::unordered_map<char32_t, char32_t> const character_reference_code_tbl;
*/
BOOST_AUTO_TEST_CASE(atom_tbl_character_reference_code_tbl_1)
{
	using namespace wordring::whatwg::html::parsing;

	BOOST_CHECK(character_reference_code_tbl.at(U'\x80') == U'\x20AC');
	BOOST_CHECK(character_reference_code_tbl.at(U'\x9F') == U'\x178');
}

/*
extern wordring::trie<char32_t> const quirks_mode_tbl;
*/
BOOST_AUTO_TEST_CASE(atom_tbl_quirks_mode_tbl_1)
{
	using namespace wordring::whatwg::html::parsing;

	BOOST_CHECK(quirks_mode_tbl.find(std::u32string_view(U"+//silmaril//dtd html pro v0r11 19970101//")) != quirks_mode_tbl.end());
	BOOST_CHECK(quirks_mode_tbl.find(std::u32string_view(U"-//webtechs//dtd mozilla html//")) != quirks_mode_tbl.end());
}

BOOST_AUTO_TEST_SUITE_END()
