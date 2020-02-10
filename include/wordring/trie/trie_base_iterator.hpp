#pragma once

#include <wordring/trie/trie_heap.hpp>

#include <optional>

namespace wordring
{
	template <typename Container>
	class const_trie_base_iterator
	{
		template <typename Allocator>
		friend class trie_base;

		template <typename Container1>
		friend bool operator==(const_trie_base_iterator<Container1> const&, const_trie_base_iterator<Container1> const&);

		template <typename Container1>
		friend bool operator!=(const_trie_base_iterator<Container1> const&, const_trie_base_iterator<Container1> const&);

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

	public:
		const_trie_base_iterator()
			: m_c(nullptr)
			, m_index(0)
		{
		}

	protected:
		const_trie_base_iterator(container& c, index_type index)
			: m_c(std::addressof(c))
			, m_index(index)
		{
		}

	public:
		/*! 文字列終端の場合trueを返す*/
		operator bool() const
		{
			if (m_index <= 1) return false;

			node_type const* d = m_c->data();
			index_type idx = (d + m_index)->m_base; // 子のBASEインデックス。

			// 子が無い（BASEが1未満）場合。
			if (idx <= 0) return true;
			// 子はあるが、途中で終端もある（文字null_valueによる遷移）場合。
			if ((idx + null_value) < tail() && (d + idx + null_value)->m_check == m_index) return true;

			return false;
		}

		bool operator!() const { return operator bool() == false; }

		value_type operator*() const
		{
			assert(base() <= m_index && m_index < base() + null_value);

			return m_index - base();
		}

		const_trie_base_iterator operator[](value_type label) const
		{
			node_type const* d = m_c->data();

			index_type idx = (d + m_index)->m_base + label;
			idx = (idx < tail() && (d + idx)->m_check == m_index) ? idx
				: 0;
			return const_trie_base_iterator(*m_c, idx);
		}

		const_trie_base_iterator& operator++()
		{
			m_index = find(m_index + 1, base() + null_value, mother());

			return *this;
		}

		const_trie_base_iterator operator++(int)
		{
			auto result = *this;

			operator++();

			return result;
		}

		template <typename String>
		String string() const
		{
			std::basic_string<value_type> tmp;
			for (auto p = *this; 1 < p.m_index; p = *p.parent()) tmp.push_back(*p);
			return String(tmp.rbegin(), tmp.rend());
		}

		std::optional<const_trie_base_iterator> parent() const
		{
			return (1 < m_index)
				? std::optional<const_trie_base_iterator>(const_trie_base_iterator(*m_c, (m_c->data() + m_index)->m_check))
				: std::nullopt;
		}

		/*! 0-255に相当する文字で遷移できる最初の子を指すイテレータを返す
		- 256による空遷移は含めない。
		- 遷移先（子）が無い場合、end()を返す。
		*/
		const_trie_base_iterator begin() const
		{
			assert(1 <= m_index);

			index_type i = (m_c->data() + m_index)->m_base;
			i = (0 < i) ? find(i, i + null_value, m_index) : 0;
			return const_trie_base_iterator(*m_c, i);
		}

		const_trie_base_iterator end() const
		{
			return const_trie_base_iterator(*m_c, 0);
		}

	protected:
		/*! 引数firstからlastの範囲で、m_checkが引数checkと一致する状態番号を返す

		- 見つからない場合、0を返す。
		- 状態番号は1から始まるので0と衝突しない。
		- 引数lastがコンテナの大きさを超える場合、そこで検索を終了する。
		*/
		index_type find(index_type first, index_type last, index_type check) const
		{
			index_type limit = std::min(last, static_cast<index_type>(m_c->size()));
			for (; first < limit; ++first) if ((m_c->data() + first)->m_check == check) break;

			return (first != limit) ? first : 0;
		}

		/*! 親の状態番号を返す
		*/
		index_type mother() const
		{
			assert(1 <= m_index && m_index < tail());

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
		index_type tail() const { return m_c->size(); }

		/*! 空遷移がある場合trueを返す
		*/
		bool has_null() const
		{
			if (m_index == 1) return false;

			node_type const* d = m_c->data();
			index_type idx = (d + m_index)->m_base + null_value;

			return (idx < tail() && (d + idx)->m_check == m_index);
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

	template <typename Container1>
	inline bool operator==(const_trie_base_iterator<Container1> const& lhs, const_trie_base_iterator<Container1> const& rhs)
	{
		assert(lhs.m_c == rhs.m_c);
		return lhs.m_index == rhs.m_index;
	}

	template <typename Container1>
	inline bool operator!=(const_trie_base_iterator<Container1> const& lhs, const_trie_base_iterator<Container1> const& rhs)
	{
		return !(lhs == rhs);
	}
}
