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

	template <typename Trie>
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

	// ------------------------------------------------------------------------
	// trie_base
	// ------------------------------------------------------------------------

	template <typename Allocator = std::allocator<trie_node>>
	class trie_base
	{
	public:
		using index_type = typename trie_node::index_type;
		using container = std::vector<trie_node>;

		using label_vector = static_vector<std::uint8_t, 257>;

	public:
		trie_base()
			: m_c({ { 0, 0 }, { 0, 0 } })
		{
		}

	protected:
		void reserve(std::size_t n = 1024)
		{
			index_type const i = m_c.size(); // reserveする先頭の番号

			for (std::size_t j = i; j < i + n; ++j) m_c.emplace_back(0, -(j + 1)); 

			auto data = m_c.data();
			if((data + -(data->m_base))->m_check == 0) (data + -(data->m_base))->m_check = -i; // 
			data->m_base = -(m_c.size() - 1); // 最後の空きノード番号を設定
			m_c.back().m_check = 0;
		}

		/*! indexで指定されるフリー・ノードを使用可能状態にする

		- 引数beforeは検索開始位置のヒント。
		- 戻り値は、連続検索で検索開始位置として使用できる、ひとつ前の空きノード位置。
		*/
		index_type allocate(index_type index, index_type before = 0)
		{
			assert(0 < index);
			assert(0 <= before);
			assert((m_c.data() + index)->m_check <= 0);

			auto data = m_c.data();

			for (index_type j = before; j < index; j = -(data + before)->m_check) before = j;

			(data + before)->m_check = (data + index)->m_check;
			if((data + index)->m_check == 0) data->m_base = -before;

			(data + index)->m_check = 0; // 使用するノードを初期化

			return before; // ひとつ前の空きノードを返す
		}

		/*! 引数destsで指定されるすべてのラベル位置を使用出来るbase位置を検索し返す。

		- 空きがない場合、新たに空きを確保する。
		- ラベルに対応するすべての使用位置はフリー・リストから削除され、使用状態となる。
		*/
		template <typename Range1>
		index_type allocate(Range1 dests)
		{
			assert(std::distance(dests.begin(), dests.end()) != 0);

			for (index_type i = -m_c.data()->m_check; i != 0; i = -(m_c.data() + i)->m_check)
			{
				index_type base = i - *dests.begin();
				if (is_free(base, dests))
				{
					index_type before = 0;
					for (std::uint8_t label : dests) before = allocate(base + label, before);
					return base;
				}
			}

			reserve();

			return allocate(dests);
		}

		void free(index_type index, index_type before = 0)
		{
			assert(0 < index);

			auto data = m_c.data();

			if(data->m_check != 0) for (index_type j = before; j < index; j = -(data + before)->m_check) before = j;

			(data + index)->m_check = (data + before)->m_check;
			(data + before)->m_check = -index;

			if ((data + index)->m_check == 0) data->m_base = -index;
		}

		index_type modify(index_type base1, index_type base2, label_vector const& labels)
		{
			return 0;
		}

		template <typename Range1>
		bool is_free(index_type base, Range1 dests) const
		{
			if (base <= 0) return false; // マイナスの位置は使用できないため、ここで弾く。

			auto const* d = m_c.data();
			for (uint8_t label : dests)
			{
				index_type i = base + label;
				if ((0 <= (d + i)->m_check && -d->m_base != i) || (m_c.size() <= i)) return false;
			}
			
			return true;
		}

		/*!
		- Range1::const_iteratorの逆参照は遷移labelを返す。
		*/
		template <typename Range1>
		index_type insert_child(index_type parent, Range1 range)
		{
			index_type base = (m_c.data() + parent)->m_base; // 遷移先配置の起点（遷移先が定義されていない場合0）

			static_vector<std::uint8_t, 257> dests{ range.begin(), range.end() }; // 引数で指定される遷移ラベルを追加

			if (base != 0) // 既存の遷移ラベルを追加
			{
				for (index_type label = 0; label < 257 && base + label < m_c.size(); ++label)
					if ((m_c.data() + base + label)->m_check == parent) dests.push_back(label);
			}

			// 追加できるか確認
			if (is_free(base, dests) == false)
			{
				index_type base2 = allocate(dests);
				if (base != 0) modify(base, base2, dests);
				base = base2;
				(m_c.data() + parent)->m_base = base;
			}

			for (auto label : dests) (m_c.data() + base + label)->m_check = parent;

			return base;
		}

		index_type next(index_type index, uint8_t label)
		{

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


	protected:
		container m_c;
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
