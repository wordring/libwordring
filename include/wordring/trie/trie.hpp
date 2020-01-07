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

		using label_vector = static_vector<std::uint16_t, 257>;

	public:
		trie_base()
			: m_c({ { 0, 0 }, { 0, 0 } })
		{
			reserve(1022);
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
		*/
		void allocate(index_type index)
		{
			assert(0 < index);
			assert(m_c.data()->m_check < 0);
			assert((m_c.data() + index)->m_check <= 0);

			auto data = m_c.data();

			index_type i = -data->m_check;
			while ((data + i)->m_check != -index) i = -(data + i)->m_check;

			(data + i)->m_check = (data + index)->m_check;
			if((data + index)->m_check == 0) data->m_base = -i;

			(data + index)->m_check = 0; // 使用するノードを初期化
		}

		/*! 引数labelsで指定されるすべてのラベル位置を使用出来るbase位置を検索し返す。

		- 空きがない場合、新たに空きを確保する。
		- ラベルに対応するすべての使用位置はフリー・リストから削除され、使用状態となる。
		*/
		index_type allocate(label_vector const& labels)
		{
			assert(std::distance(labels.begin(), labels.end()) != 0);

			for (index_type i = -m_c.data()->m_check; i != 0; i = -(m_c.data() + i)->m_check)
			{
				index_type base = i - *labels.begin();
				if (is_free(base, labels))
				{
					for (std::uint16_t label : labels) allocate(base + label);
					return base;
				}
			}

			reserve();

			return allocate(labels);
		}

		void free(index_type index)
		{
			assert(0 < index);

			auto data = m_c.data();

			(data + index)->m_check = 0;
			(data + index)->m_base = 0;

			(data - data->m_base)->m_check = -index;
			data->m_base = -index;
		}

		void free(index_type base, label_vector const& labels)
		{
		}

		/*! base1からbase2へノードを移動する

		- 衝突時に使用する。
		- 
		*/
		void move(index_type src, index_type dest, label_vector const& labels)
		{
			auto data = m_c.data();

			index_type parent = (data + src)->m_check;
			(data + parent)->m_base = dest;

			for (auto label : labels) if ((data + src + label)->m_check == parent) free(src + label);
			for (auto label : labels) (data + dest + label)->m_check = parent;
		}

		/*! 状態stateからラベルlabelの遷移において、遷移先が空きか調べる
		*/
		bool is_free(index_type state, std::uint16_t label)
		{
			index_type base = (m_c.data() + state)->m_base;

			if (m_c.size() <= base + label) return false;
			if (0 < (m_c.data() + base + label)->m_check) return false;

			return true;
		}

		bool is_free(index_type base, label_vector const& labels) const
		{
			if (base <= 0) return false; // マイナスの位置は使用できないため、ここで弾く。

			auto const* d = m_c.data();
			for (std::uint16_t label : labels)
			{
				index_type i = base + label;
				if ((m_c.size() <= i) || (0 <= (d + i)->m_check && -d->m_base != i)) return false;
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
			//index_type base_new{};

			label_vector labels{}; // 既存のラベル
			label_vector labels_new{ range.begin(), range.end() }; // 引数で指定される遷移ラベルを追加

			if (base != 0)
			{
				for (index_type label = 0; label < 257 && base + label < m_c.size(); ++label)
					if ((m_c.data() + base + label)->m_check == parent) labels.push_back(label);
			}

			if (is_free(base, labels_new)) for (auto label : labels_new) allocate(base + label);
			else
			{
				if (base == 0);
				//index_type base_new = allocate(labels_new);
			}

			// 追加できるか確認
			if (is_free(base, labels_new) == false)
			{
				index_type dest = allocate(labels_new);
				if (base != 0) move(base, dest, labels_new);
				base = dest;
				(m_c.data() + parent)->m_base = base;
			}

			for (auto label : labels_new) (m_c.data() + base + label)->m_check = parent;

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
