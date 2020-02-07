#pragma once

#include <wordring/static_vector/static_vector.hpp>

#include <algorithm>
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
		using node_type = trie_node;
		using container = std::vector<trie_node, Allocator>;

		using label_vector = static_vector<std::uint16_t, 257>;

	protected:
		static constexpr std::uint16_t null_value = 256u;

	protected:
		trie_heap()
			: m_c(2, { 0, 0 })
		{
		}

		void reserve(std::size_t n, index_type before = 0)
		{
			index_type id = m_c.size(); // reserveする先頭の番号
			m_c.insert(m_c.end(), n, { 0, 0 });

			node_type* d = m_c.data();
			// 値が0のINDEXを探す
			for (index_type i = -(d + before)->m_check; i != 0; i = -(d + before)->m_check) before = i;
			// CHECKを更新する
			for (index_type limit = m_c.size(); id != limit; before = id++) (d + before)->m_check = -id;
		}

		/*! idで指定される空きノードを使用可能にする
		*/
		void allocate(index_type id, index_type before = 0)
		{
			assert(m_c[id].m_check <= 0);

			node_type* d = m_c.data();
			// CHECKがidと一致するINDEXを探す
			for (index_type i = -(d + before)->m_check; i != id; i = -(d + before)->m_check) before = i;
			// CHECKを更新する
			(d + before)->m_check = (d + id)->m_check;
			(d + id)->m_check = 0;
		}

		/*! base + labelsを使用可能にする
		*/
		void allocate(index_type base, label_vector const& labels, index_type before = 0)
		{
			assert(1 <= base);
			assert(!labels.empty());
			assert(std::is_sorted(labels.begin(), labels.end()));
			assert(before < m_c.size());

			if (m_c.size() <= base + labels.back()) reserve(base + labels.back() - m_c.size() + 1);

			trie_node* d = m_c.data();
			for (std::uint16_t label : labels)
			{
				index_type id = base + label;
				if (1 <= (d + id)->m_check) continue; // 登録済み
				for (index_type i = before; i != id; i = -(d + before)->m_check) before = i;

				(d + before)->m_check = (d + id)->m_check;
				(d + id)->m_check = 0;
			}
		}

		index_type relocate(index_type parent, index_type from, label_vector const& labels)
		{
			label_vector children;

			index_type limit = m_c.size() - 1;
			limit = std::min(from + null_value, limit);
			for (index_type i = from; i <= limit; ++i)
			{
				assert(i < m_c.size());
				assert(0 <= i - from);
				if ((m_c.data() + i)->m_check == parent) children.push_back(i - from);
			}

			label_vector all;
			std::set_union(labels.begin(), labels.end(), children.begin(), children.end(), std::back_inserter(all));

			index_type before = 0;
			index_type to = locate(all, before);
			allocate(to, all, before);

			node_type* d = m_c.data();
			for (std::uint16_t label : children)
			{
				index_type idx = from + label; // 子の旧INDEX

				(d + to + label)->m_base = (d + idx)->m_base; // 子のBASEを置換
				(d + to + label)->m_check = (d + idx)->m_check; // 子のCHECKを置換

				// 孫のCHECKを置き換え
				index_type base = (d + idx)->m_base;
				index_type limit = m_c.size() - 1;
				limit = std::min(base + null_value, limit);
				for (index_type i = base; i <= limit; ++i)
				{
					assert(i < m_c.size());
					if ((d + i)->m_check == idx) (d + i)->m_check = to + label;
				}
			}

			assert(parent < m_c.size());
			(d + parent)->m_base = to;
			free(from, children);

			return to;
		}

		/*! idを解放して未使用リンクにつなげる
		- before を返す。
		- これが次のfree呼び出しのヒントとして使える。
		*/
		index_type free(index_type id, index_type before = 0)
		{
			assert(1 <= id);

			node_type* d = m_c.data();

			for (index_type i = -(d + before)->m_check; i != 0 && i < id; i = -(d + before)->m_check) before = i;

			(d + id)->m_base = 0;
			(d + id)->m_check = (d + before)->m_check;
			(d + before)->m_check = -id;

			return before;
		}

		/*! base + labelsを解放して未使用リンクにつなげる
		*/
		void free(index_type base, label_vector const& labels, index_type before = 0)
		{
			assert(!labels.empty());
			assert(std::is_sorted(labels.begin(), labels.end()));
			assert(1 <= base && base + labels.back() < m_c.size());

			for (auto label : labels) before = free(base + label, before);
		}

		/*! 未使用ノードを検索する

		- labelsが配置される最初のノードの直前の空きノードをbeforeで返す。
		- これがallocate呼び出しのヒントとして使える。

		- INDEX1に子は配置されない。
		*/
		index_type locate(label_vector const& labels, index_type& before) const
		{
			assert(!labels.empty());
			assert(std::is_sorted(labels.begin(), labels.end()));

			index_type base = 0;
			before = 0;

			std::uint16_t offset = labels.front();

			trie_node const* d = m_c.data();
			index_type id = -d->m_check;

			if (id != 0)
			{
				// BASEが負にならないよう、検索開始位置を設定する。
				for (; 0 != id && id <= offset; id = -(d + id)->m_check) before = id;
				for(; id != 0 && !is_free(id - offset, labels); id = -(d + id)->m_check) before = id;
				if (id != 0) base = id - offset;
			}

			if (base == 0)
			{
				base = std::max(static_cast<int>(m_c.size()) - offset, 1);
				//before = 0;
			}


			return base;
		}

		/*! base + labelsがすべて未使用である場合、trueを返す
		*/
		bool is_free(index_type base, label_vector const& labels) const
		{
			assert(1 <= base);
			assert(!labels.empty());

			node_type const* d = m_c.data();
			for (std::uint16_t label : labels)
			{
				index_type id = base + label;
				if (m_c.size() < id) break;
				if (1 <= (d + id)->m_check) return false;
			}

			return true;
		}

		/*! base + labelsすべてについて、未使用あるいはparentからの遷移である場合、trueを返す
		*/
		bool is_free(index_type parent, index_type base, label_vector const& labels) const
		{
			assert(1 <= parent);
			assert(1 <= base);
			assert(!labels.empty());

			node_type const* d = m_c.data();
			for (std::uint16_t label : labels)
			{
				index_type id = base + label;
				if (m_c.size() < id) break;
				index_type check = (d + id)->m_check;
				if (1 <= check && parent != check) return false;
			}

			return true;
		}

	protected:
		container m_c;
	};
}
