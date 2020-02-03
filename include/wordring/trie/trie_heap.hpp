#pragma once

#include <wordring/static_vector/static_vector.hpp>

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

	template <typename Allocator>
	class trie_heap
	{
	protected:
		using index_type = typename trie_node::index_type;
		using container = std::vector<trie_node, Allocator>;

		using label_vector = static_vector<std::uint16_t, 257>;

	protected:
		static constexpr std::uint16_t null_value = 256u;

	protected:
		trie_heap()
			: m_c({ { 0, 0 }, { 0, 0 } })
		{
		}

	public:
		void reserve(std::size_t n = 1)
		{
			index_type const i = m_c.size(); // reserveする先頭の番号

			for (index_type j = i; j < i + n; ++j) m_c.emplace_back(0, -(j + 1));

			auto data = m_c.data();
			if ((data + -(data->m_base))->m_check == 0) (data + -(data->m_base))->m_check = -i; // 
			data->m_base = -(m_c.size() - 1); // 最後の空きノード番号を設定
			m_c.back().m_check = 0;
		}

	protected:
		/*! indexで指定されるフリー・ノードを使用可能状態にする
		*/
		void allocate(index_type index)
		{
			assert(0 < index);
			assert(m_c.data()->m_check < 0);
			assert((m_c.data() + index)->m_check <= 0);

			assert(index < m_c.size());

			auto data = m_c.data();

			index_type i = -data->m_check;
			while ((data + i)->m_check != -index) i = -(data + i)->m_check;

			(data + i)->m_check = (data + index)->m_check;
			if ((data + index)->m_check == 0) data->m_base = -i;

			(data + index)->m_check = 0; // 使用するノードを初期化
		}

		/*! 引数labelsで指定されるすべてのラベル位置を使用出来るbase位置を検索し返す。

		- 空きがない場合、新たに空きを確保する。
		- ラベルに対応するすべての使用位置はフリー・リストから削除され、使用状態となる。
		- 親のBASEに入る値が返る。
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

		bool is_free(index_type base, label_vector const& labels)
		{
			if (base <= 0) return false; // マイナスの位置は使用できないため、ここで弾く。

			if (m_c.size() <= static_cast<std::uint32_t>(base + null_value)) reserve(null_value);

			auto const* data = m_c.data();
			for (std::uint16_t label : labels)
			{
				index_type check = (data + base + label)->m_check;
				if (0 < check || (0 == check && -data->m_base != base + label)) return false;
			}

			return true;
		}

	protected:
		container m_c;
	};
}
