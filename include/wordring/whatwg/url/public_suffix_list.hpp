#pragma once

// https://url.spec.whatwg.org/
// https://triple-underscore.github.io/URL-ja.html

#include <wordring/string/atom.hpp>
#include <wordring/trie/trie.hpp>

namespace wordring::whatwg
{
	class public_suffix_list
	{
	public:
	protected:
		u8atom_set         m_entries;
		trie<std::int32_t> m_trie;
	};
}
