#pragma once

#include <algorithm>
#include <deque>
#include <iterator>
#include <utility>

#include <wordring/trie/triplet.hpp>

namespace wordring
{
	struct trie_node { int32_t base, check; };

	class trie_base
	{
	public:
		using container = std::deque<trie_node>;

	public:
		void reserve(std::size_t n)
		{
			int j = m_array.size();

		}

		void free_node(std::uint32_t i)
		{

		}
		void erase(uint32_t index);
		void find();
		void insert(uint32_t index, uint8_t label);

		template <typename Iterator>
		void insert(Iterator first, Iterator last)
		{
			triplet_trie t(first, last);
			std::sort(t.begin(), t.end());
			//auto c = t.children(t.root());
		}

		uint32_t next(uint32_t index, uint8_t label);

	private:
		container m_array;
	};

	template <typename Key, typename Value>
	class basic_trie : public trie_base
	{
	public:
		using key_type = Key;
		using value_type = Value;
		using character = typename key_type::value_type;
	};

	template <typename Key, typename Value>
	using trie = basic_trie<Key, Value>;
}
