#pragma once

#include <deque>
#include <iterator>
#include <utility>

#include <wordring/trie/triplet.hpp>

namespace wordring
{
	template <typename T>
	class serialize_iterator
	{
	public:
/*		using difference_type = typename Iterator::difference_type;
		using value_type = uint8_t;
		using pointer = typename Iterator::pointer;
		using reference = typename Iterator::reference;
*/		using iterator_category = std::forward_iterator_tag;

	private:
	};

	struct trie_node
	{
		int32_t base;
		uint32_t check;
	};

	template <typename Container = std::deque<trie_node>>
	class trie_base
	{
	public:
		using container = Container;

	public:
		void erase(uint32_t index);
		void find();
		void insert(uint32_t index, uint8_t label);

		template <typename Iterator>
		void insert()
		{


		}
		uint32_t next(uint32_t index, uint8_t label);

	private:
		container m_array;
	};

	template <typename Key, typename Value, typename Container = std::deque<trie_node>>
	class basic_trie : public trie_base<Container>
	{
	public:
		using key_type = Key;
		using value_type = Value;
		using character = typename key_type::value_type;
	};

	template <typename Key, typename Value>
	using trie = basic_trie<Key, Value>;
}
