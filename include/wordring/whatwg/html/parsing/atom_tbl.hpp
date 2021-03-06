#pragma once
// generated by wordring_cpp/generator/whatwg/html/atom_tbl.cpp

#include <wordring/whatwg/html/parsing/atom_defs.hpp>

#include <wordring/string/atom.hpp>
#include <wordring/trie/trie.hpp>

#include <array>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace wordring::whatwg::html::parsing
{
	extern std::unordered_map<std::u32string, tag_name> const tag_atom_tbl;

	extern std::unordered_map<std::u32string, attribute_name> const attribute_atom_tbl;

	extern std::unordered_map<std::u32string, ns_name> const ns_uri_atom_tbl;

	extern std::array<std::u32string, 427> const tag_name_tbl;

	extern std::array<std::u32string, 634> const attribute_name_tbl;

	extern std::array<std::u32string, 7> const ns_uri_tbl;

	std::uint32_t constexpr named_character_reference_max_length = 32;
	extern wordring::trie<char32_t> const named_character_reference_idx_tbl;
	extern std::array<std::array<char32_t, 2>, 2231> const named_character_reference_map_tbl;

	extern std::unordered_map<char32_t, char32_t> const character_reference_code_tbl;

	extern std::unordered_map<std::u32string, std::u32string> const svg_attributes_conversion_tbl;

	struct foreign_attributes_conversion_entry
	{
		std::u32string m_prefix;
		std::u32string m_local_name;
		ns_name m_namespace;
	}; 
	extern std::unordered_map<std::u32string, foreign_attributes_conversion_entry> const foreign_attributes_conversion_tbl;

	extern wordring::trie<char32_t> const quirks_mode_tbl;

	extern std::unordered_map<std::u32string, std::u32string> const svg_elements_conversion_tbl;
}
