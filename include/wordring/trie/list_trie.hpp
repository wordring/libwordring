#pragma once

#include <algorithm>
#include <iterator>
#include <type_traits>

namespace wordring
{
	template <typename Iterator>
	class list_trie_iterator
	{
		template <typename Iterator1>
		friend bool operator==(list_trie_iterator<Iterator1> const&, list_trie_iterator<Iterator1> const&);

		template <typename Iterator1>
		friend bool operator!=(list_trie_iterator<Iterator1> const&, list_trie_iterator<Iterator1> const&);

	public:
		using iterator_type = Iterator;
		using string_type   = typename std::iterator_traits<iterator_type>::value_type;

		using difference_type   = typename std::iterator_traits<iterator_type>::difference_type;
		using value_type        = typename string_type::value_type;
		using pointer           = value_type*;
		using reference         = value_type&;
		using iterator_category = std::input_iterator_tag;

		class result_pair
		{
			using iterator_type = typename string_type::const_iterator;

		public:
			result_pair(iterator_type first, iterator_type last)
				: m_first(first)
				, m_last(last)
			{
			}

			iterator_type begin() const noexcept { return m_first; }

			iterator_type end() const noexcept { return m_last; }

		private:
			iterator_type m_first;
			iterator_type m_last;
		};

	public:
		list_trie_iterator(iterator_type first, iterator_type last)
			: m_first(first)
			, m_next(first)
			, m_last(last)
			, m_level(-1)
		{
		}

		list_trie_iterator(list_trie_iterator const&) = default;

		list_trie_iterator(list_trie_iterator&&) = default;

		bool leaf() const { return m_first.size() == m_level; }

		result_pair string() const
		{
			return result_pair(m_first->cbegin(), std::next(m_first->cbegin(), m_level + 1));
		}

		value_type operator*() const
		{
			assert(m_first != m_last);
			assert(0 <= m_level);

			return *std::next(m_first->begin(), m_level);
		}

		list_trie_iterator& operator++()
		{
			m_first = next();
			return *this;
		}

		list_trie_iterator operator++(int)
		{
			auto result = *this;

			operator++();

			return result;
		}

		list_trie_iterator begin()
		{
			if (m_level < 0) return list_trie_iterator(m_first, m_last, 0);
			return list_trie_iterator(m_first, next(), m_level + 1);
		}

		list_trie_iterator end()
		{
			return list_trie_iterator(next(), next(), m_level + 1);
		}

	protected:
		list_trie_iterator(iterator_type first, iterator_type last, int32_t level)
			: m_first(first)
			, m_next(first)
			, m_last(last)
			, m_level(level)
		{
		}

		iterator_type next()
		{
			if(m_level < 0) return m_last;
			if (m_first != m_next) return m_next;

			auto it = m_first;
			auto ch = operator*();
			while (it != m_last && *std::next(it->begin(), m_level) == ch) ++it;
			
			const_cast<iterator_type&>(m_next) = it;
			
			return it;
		}

	private:
		iterator_type m_first;
		iterator_type m_next;
		iterator_type m_last;
		int32_t       m_level;
	};

	template <typename Iterator1>
	inline bool operator==(list_trie_iterator<Iterator1> const& lhs, list_trie_iterator<Iterator1> const& rhs)
	{
		return !(lhs != rhs);
	}

	template <typename Iterator1>
	inline bool operator!=(list_trie_iterator<Iterator1> const& lhs, list_trie_iterator<Iterator1> const& rhs)
	{
		return lhs.m_first != rhs.m_first || lhs.m_level != rhs.m_level;
	}

}
