#pragma once

#include <wordring/utility/result_range.hpp>

#include <algorithm>
#include <atomic>
#include <iterator>
#include <type_traits>

namespace wordring
{
	/*!
	- #による空遷移も状態番号を持つ事に注意。この動作は、ダブルアレイ構築に必要なため設定した。
	*/
	template <typename Iterator>
	class const_list_trie_iterator
	{
		template <typename Iterator1>
		friend bool operator==(const_list_trie_iterator<Iterator1> const&, const_list_trie_iterator<Iterator1> const&);

		template <typename Iterator1>
		friend bool operator!=(const_list_trie_iterator<Iterator1> const&, const_list_trie_iterator<Iterator1> const&);

	public:
		using iterator_type = Iterator;
		using string_type   = typename std::iterator_traits<iterator_type>::value_type;
		using char_type     = typename string_type::value_type;
		using string_iterator = typename string_type::const_iterator;

		using difference_type   = typename std::iterator_traits<iterator_type>::difference_type;
		using value_type        = std::uint16_t;
		using pointer           = value_type*;
		using reference         = value_type&;
		using iterator_category = std::input_iterator_tag;

		using parent_view = result_range<string_iterator>;
		using string_view = result_range<string_iterator>;

		static constexpr std::uint16_t null_value = 256u;

	public:
		const_list_trie_iterator(iterator_type first, iterator_type last)
			: m_first(first)
			, m_last(last)
			, m_next(first)
			, m_level(-1)
		{
		}

		const_list_trie_iterator(const_list_trie_iterator const& other)
			: m_first(other.m_first)
			, m_last(other.m_last)
			, m_next(other.m_first)
			, m_level(other.m_level)
		{
		}

		/*! 親に相当する文字列を返す
		*/
		parent_view parent() const
		{
			std::uint32_t lv = (m_level == -1) ? 0 : m_level;

			return parent_view(m_first->cbegin(), std::next(m_first->cbegin(), lv));
		}

		string_view string() const
		{
			std::uint32_t lv = (m_level == -1) ? 0 : m_level + 1;

			return parent_view(m_first->cbegin(), std::next(m_first->cbegin(), lv));
		}

		value_type operator*() const
		{
			std::uint32_t lv = m_level;

			assert(m_first != m_last);
			assert(0 <= m_level && lv <= m_first->size());

			auto it = m_first;
			if (it->size() <= lv) ++it;

			return *std::next(it->begin(), lv);
		}

		const_list_trie_iterator& operator++()
		{
			m_first = next();

			return *this;
		}

		const_list_trie_iterator operator++(int)
		{
			auto tmp = *this;
			operator++();
			return tmp;
		}

		const_list_trie_iterator begin() const
		{
			std::uint32_t lv = m_level;
			auto it1 = m_first;
			auto it2 = m_last;

			if (it1 != it2 && lv + 1 == it1->size()) ++it1; // 空白読み飛ばし

			if (m_level != -1)
			{
				it2 = next();

				if (it1 == it2)
				{
					it1 = m_first;
					it2 = m_first;
				}
			}

			return const_list_trie_iterator(it1, it2, m_level + 1);
		}

		const_list_trie_iterator end() const
		{
			auto it = begin();

			return const_list_trie_iterator(it.m_last, it.m_last, it.m_level);
		}

		bool has_child() const
		{
			// rootの場合
			if (m_level == -1)
			{
				if (m_first != m_last) return true;
				return false;
			}

			// root 以外の場合
			std::uint32_t lv = m_level;
			auto it = m_first;

			if (lv + 1 == it->size()) ++it;
			if (it == m_last) return false;

			auto ch = operator*();
			auto c = *std::next(it->begin(), lv);
			if (ch == c) return true;

			return false;
		}

		bool has_null() const
		{
			// rootの場合
			if (m_level == -1) return false;

			// root 以外の場合
			std::uint32_t lv = m_level;
			if (lv + 1 == m_first->size()) return true;

			return false;
		}

	protected:
		const_list_trie_iterator(iterator_type first, iterator_type last, int32_t level)
			: m_first(first)
			, m_last(last)
			, m_next(first)
			, m_level(level)
		{
		}

		public:
		/*!
		- スレッド安全性を持たない。
		*/
		iterator_type next() const
		{
			if (m_next != m_first) return m_next;

			std::uint32_t lv = m_level;

			auto it1 = m_first;
			auto it2 = m_last;
			if (it1->size() <= lv) ++it1; // 途中終端を飛ばす

			std::uint8_t ch = *std::next(it1->begin(), lv);
			while (it1 != it2)
			{
				std::uint8_t c = *std::next(it1->begin(), lv);
				if (ch != c) break;
				++it1;
			}
			return m_next = it1;
		}

	private:
		iterator_type m_first;
		iterator_type m_last;
		iterator_type mutable m_next; // キャッシュ
		int32_t       m_level;
	};

	template <typename Iterator1>
	inline bool operator==(const_list_trie_iterator<Iterator1> const& lhs, const_list_trie_iterator<Iterator1> const& rhs)
	{
		return !(lhs != rhs);
	}

	template <typename Iterator1>
	inline bool operator!=(const_list_trie_iterator<Iterator1> const& lhs, const_list_trie_iterator<Iterator1> const& rhs)
	{
		return lhs.m_first != rhs.m_first || lhs.m_level != rhs.m_level;
	}

}