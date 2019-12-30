#pragma once

#include <wordring/static_vector/static_vector.hpp>
#include <wordring/trie/list_trie.hpp>
#include <wordring/trie/trie_defs.hpp>

#include <algorithm>
#include <bitset>
#include <cassert>
#include <cstdint>
#include <iterator>
#include <utility>
#include <vector>

namespace wordring
{
	struct trie_node
	{
		using index_type = std::int32_t;

		index_type m_base;
		index_type m_check;

		trie_node()
			: m_base(0)
			, m_check(0)
		{
		}

		trie_node(index_type base, index_type check)
			: m_base(base)
			, m_check(check)
		{
		}
	};

	class trie_base_iterator
	{
	public:
		using value_type        = std::uint8_t;
		using difference_type   = std::ptrdiff_t;
		using reference         = value_type&;
		using pointer           = value_type*;
		using iterator_category = std::input_iterator_tag;

	public:
	};

	template <typename Allocator = std::allocator<trie_node>>
	class trie_base
	{
	public:
		using index_type = typename trie_node::index_type;
		using destination_set = std::array<uint8_t, 257>;
		using container = std::vector<trie_node>;

	public:
		trie_base()
			: m_container(1)
		{
		}

	protected:
		void reserve(std::size_t n)
		{
			index_type const i = m_container.size(); // reserveする先頭の番号

			for (std::size_t j = i; j < i + n; ++j) m_container.emplace_back(0, -(j + 1)); 

			auto data = m_container.data();
			if((data + -(data->m_base))->m_check == 0) (data + -(data->m_base))->m_check = -i; // 
			data->m_base = -(m_container.size() - 1); // 最後の空きノード番号を設定
			m_container.back().m_check = 0;
		}

		index_type allocate(index_type index, index_type before = 0)
		{
			auto data = m_container.data();

			assert((data + index)->m_check <= 0);

			for (index_type j = before; j < index; j = -(data + before)->m_check) before = j;

			(data + before)->m_check = (data + index)->m_check;
			if((data + index)->m_check == 0) data->m_base = -before;

			(data + index)->m_check = 0; // 使用するノードを初期化

			return before; // ひとつ前の空きノードを返す
		}

		index_type allocate(std::bitset<257> bits)
		{
			
			return 0;
		}

		void free(index_type index, index_type before = 0)
		{
			auto data = m_container.data();

			for (index_type j = before; j < index; j = -(data + before)->m_check) before = j;

			(data + index)->m_check = (data + before)->m_check;
			(data + before)->m_check = -index;

			if ((data + index)->m_check == 0) data->m_base = -index;
		}

		index_type modify(index_type pos)
		{

		}

		index_type insert(index_type pos, uint8_t val)
		{
			index_type i = (m_container.data() + pos)->m_base; // 基準

			if (m_container.size() < i + val) reserve(257);

			auto data = m_container.data();

			if ((m_container.data() + i + val)->m_base != 0)
			{


			}



		}

		void erase(uint32_t index);
		void find();
		void insert(uint32_t index, uint8_t label);

		template <typename Iterator>
		void insert(Iterator first, Iterator last)
		{
			//triplet_trie t(first, last);
			//std::sort(t.begin(), t.end());
			//auto c = t.children(t.root());
		}

		uint32_t next(uint32_t index, uint8_t label);

	protected:
		container m_container;
	};

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
