#pragma once

#include <wordring/trie/trie_heap.hpp>

#include <optional>

namespace wordring
{
	template <typename Container>
	class const_trie_heap_iterator
	{
	public:
		using difference_type   = std::ptrdiff_t;
		using value_type        = std::uint8_t;
		using pointer           = value_type*;
		using reference         = value_type&;
		using iterator_category = std::input_iterator_tag;

	protected:
		using index_type = typename trie_node::index_type;
		using node_type  = trie_node;
		using container  = Container const;

		static constexpr std::uint16_t null_value = 256;

	protected:
		const_trie_heap_iterator()
			: m_c(nullptr)
			, m_index(0)
		{
		}

		const_trie_heap_iterator(container& c, index_type index)
			: m_c(std::addressof(c))
			, m_index(index)
		{
		}

		/*! イテレータが指す値を返す
		*/
		value_type value() const
		{
			assert(base() <= m_index && m_index < base() + null_value);

			return m_index - base();
		}

		index_type at_index(value_type label) const
		{
			node_type const* d = m_c->data();

			index_type idx = (d + m_index)->m_base + label;
			return (idx < limit() && (d + idx)->m_check == m_index)
				? idx
				: 0;
		}

		/*! イテレータを前進させる
		*/
		void advance() { m_index = find(m_index + 1, base() + null_value, mother()); }


		template <typename String>
		String string() const
		{
			std::basic_string<value_type> tmp;
			for (auto p = *this; 1 < p.m_index; p = *p.parent()) tmp.push_back(*p);
			return String(tmp.rbegin(), tmp.rend());
		}

		/*! 親のINDEXを返す
		- 根で呼び出した場合、0を返す。
		*/
		index_type parent_index() const
		{
			return (1 < m_index)
				? (m_c->data() + m_index)->m_check
				: 0;
		}

		/*! 0-255に相当する文字で遷移できる最初の子を指すINDEXを返す
		- 空遷移は含めない。
		- 遷移先（子）が無い場合、0を返す。
		*/
		index_type begin_index() const
		{
			assert(1 <= m_index);

			index_type idx = (m_c->data() + m_index)->m_base;
			return (1 <= idx)
				? find(idx, idx + null_value, m_index)
				: 0;
		}

		index_type end_index() const { return 0; }

		/*! 引数firstからlastの範囲で、m_checkが引数checkと一致する状態番号を返す

		- 見つからない場合、0を返す。
		- 状態番号は1から始まるので0と衝突しない。
		- 引数lastがコンテナの大きさを超える場合、そこで検索を終了する。
		*/
		index_type find(index_type first, index_type last, index_type check) const
		{
			index_type tail = std::min(last, limit());
			for (; first < tail; ++first) if ((m_c->data() + first)->m_check == check) break;

			return (first != tail) ? first : 0;
		}

		/*! 親の状態番号を返す
		*/
		index_type mother() const
		{
			assert(1 <= m_index && m_index < limit());

			node_type const* d = m_c->data();
			index_type parent = (d + m_index)->m_check;

			return (1 <= parent)
				? parent
				: 0;
		}

		/*! 親のBASEを返す
		- つまりコレはm_indexからラベルを差し引いた値にに相当する。
		*/
		index_type base() const
		{
			node_type const* d = m_c->data();

			index_type parent = mother();
			assert(parent != 0);

			index_type idx = (parent != 0) ? (d + parent)->m_base : 0;
			assert(1 <= idx); // thisが子である以上、親のBASEが存在する。

			return idx;
		}

		/*! コンテナの最終状態番号の次を返す
		- キャストで行が増えるのを抑制するために用意した。
		*/
		index_type limit() const { return m_c->size(); }

		/*! 空遷移がある場合trueを返す
		*/
		bool has_null() const
		{
			if (m_index == 1) return false;

			node_type const* d = m_c->data();
			index_type idx = (d + m_index)->m_base + null_value;

			return (idx < limit() && (d + idx)->m_check == m_index);
		}

		/*! 兄弟にあたる状態がある場合、trueを返す
		*/
		bool has_sibling() const
		{
			if (m_index == 1) return false;

			index_type check = (m_c->data() + m_index)->m_check;

			index_type first = base();
			index_type last = first + null_value;

			while (true)
			{
				first = find(first, last, check);
				if (first == 0) return false;
				if (first != m_index) return true;
				++first;
			}

			return false;
		}

	protected:
		container* m_c;
		index_type m_index;
	};
}
