#pragma once

#include <wordring/trie/trie_base.hpp>


namespace wordring
{

	template <typename Key, typename Value>
	class basic_trie : public trie_base<>
	{
	public:
		using key_type = Key;
		using value_type = Value;
		using character = typename key_type::value_type;
	};

	template <typename Key, typename Value>
	using trie = basic_trie<Key, Value>;
}
