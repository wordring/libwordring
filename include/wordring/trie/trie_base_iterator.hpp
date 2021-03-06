﻿#pragma once

#include <wordring/trie/trie_heap_iterator.hpp>

namespace wordring::detail
{
	template <typename Container>
	class const_trie_base_iterator : public const_trie_heap_iterator<Container>
	{
		template <typename Allocator1>
		friend class trie_base;

		template <typename Container1>
		friend bool operator==(const_trie_base_iterator<Container1> const&, const_trie_base_iterator<Container1> const&);

		template <typename Container1>
		friend bool operator!=(const_trie_base_iterator<Container1> const&, const_trie_base_iterator<Container1> const&);

	protected:
		using base_type = const_trie_heap_iterator<Container>;

		using typename base_type::index_type;
		using typename base_type::node_type;
		using typename base_type::container;

	public:
		using difference_type   = std::ptrdiff_t;
		using value_type        = std::uint8_t;
		using pointer           = value_type*;
		using reference         = value_type&;
		using iterator_category = std::input_iterator_tag;

		static constexpr std::uint16_t null_value = 256u;

	protected:
		using base_type::value;
		using base_type::at_index;
		using base_type::advance;
		using base_type::parent_index;
		using base_type::begin_index;
		using base_type::end_index;

		using base_type::find;
		using base_type::mother;
		using base_type::base;
		using base_type::limit;
		using base_type::has_null;
		using base_type::has_sibling;

		using base_type::m_c;
		using base_type::m_index;

	public:
		const_trie_base_iterator()
			: base_type()
		{
		}

	protected:
		const_trie_base_iterator(container& c, index_type index)
			: base_type(c, index)
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
			if ((idx + null_value) < limit() && (d + idx + null_value)->m_check == m_index) return true;

			return false;
		}

		bool operator!() const { return operator bool() == false; }

		value_type operator*() const { return value(); }

		const_trie_base_iterator operator[](value_type label) const
		{
			return const_trie_base_iterator(*m_c, at_index(label));
		}

		const_trie_base_iterator& operator++()
		{
			advance();
			return *this;
		}

		const_trie_base_iterator operator++(int)
		{
			auto result = *this;
			advance();
			return result;
		}

		template <typename String>
		void string(String& result) const
		{
			result.clear();
			for (auto p = *this; 1 < p.m_index; p = p.parent()) result.push_back(*p);
			std::reverse(std::begin(result), std::end(result));
		}

		const_trie_base_iterator parent() const
		{
			return const_trie_base_iterator(*m_c, parent_index());
		}

		/*! 0-255に相当する文字で遷移できる最初の子を指すイテレータを返す
		- 256による空遷移は含めない。
		- 遷移先（子）が無い場合、end()を返す。
		*/
		const_trie_base_iterator begin() const
		{
			return const_trie_base_iterator(*m_c, begin_index());
		}

		const_trie_base_iterator end() const
		{
			return const_trie_base_iterator(*m_c, end_index());
		}
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
