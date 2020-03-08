#pragma once

#include <algorithm>
#include <iterator>
#include <type_traits>

namespace wordring::detail
{
	/*! @brief 文字列の集合をTrie構築に適した形で走査するイテレータ

	書籍「形態素解析の理論と実装」で紹介されている三つ組みによる走査の実装。

	@tparam Iterator
		ベースとなる std::vector<std::string>::const_iterator のような文字列の集合に対するイテレータ\n
		文字列を値とするSTLコンテナを指すイテレータを想定している。

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
		using iterator_type   = Iterator;
		using string_type     = typename std::iterator_traits<iterator_type>::value_type;
		using char_type       = typename string_type::value_type;
		using string_iterator = typename string_type::const_iterator;

		using difference_type   = typename std::iterator_traits<iterator_type>::difference_type;
		using value_type        = std::uint16_t;
		using pointer           = value_type*;
		using reference         = value_type&;
		using iterator_category = std::input_iterator_tag;

		static constexpr std::uint16_t null_value = 256u;

	public:
		/*! @brief ベースとなるイテレータから構築する

		@param first 
			文字列集合の先頭を指すイテレータ

		@param last
			文字列集合の終端を指すイテレータ

		@par 例
		@code
			std::vector<std::string> list{ "a", "ac", "b", "cab", "cd" };
			auto it = const_list_trie_iterator(list.begin(), list.end());
		@endcode
		*/
		const_list_trie_iterator(iterator_type first, iterator_type last)
			: m_first(first)
			, m_last(last)
			, m_next(first)
			, m_level(-1)
		{
		}

		/*! @brief コピー構築する
		*/
		const_list_trie_iterator(const_list_trie_iterator const& other)
			: m_first(other.m_first)
			, m_last(other.m_last)
			, m_next(other.m_first)
			, m_level(other.m_level)
		{
		}

		/*! @brief イテレータが指す値を返す
		*/
		value_type operator*() const
		{
			std::uint32_t lv = m_level;

			assert(m_first != m_last);
			assert(0 <= m_level && lv <= m_first->size());

			auto it = m_first;
			if (it->size() <= lv) ++it;

			return static_cast<std::uint8_t>(*std::next(it->begin(), lv));
		}

		/*! @brief イテレータを進める

			@return
				*this
		*/
		const_list_trie_iterator& operator++()
		{
			m_first = next();

			return *this;
		}

		/*! @brief イテレータを進める

			@return
				変更前の位置を保持するconst_list_trie_iterator
		*/
		const_list_trie_iterator operator++(int)
		{
			auto tmp = *this;
			operator++();
			return tmp;
		}

		/*! @brief 根からイテレータ迄の文字列を返す

			@return
				文字列の先頭を指すイテレータと終端を指すイテレータの組

		@par 例
		@code
			auto it1 = it.begin().begin(); // "ac"

			auto pair = it1.string();
			
			auto s = std::string(pair.first, pair.second);
			assert(s == "ac");
		@endcode
		*/
		std::pair<string_iterator, string_iterator> string() const
		{
			std::uint32_t lv = (m_level == -1) ? 0 : m_level + 1;

			return std::make_pair(m_first->begin(), std::next(m_first->begin(), lv));
		}

		/*! @brief 子の先頭を指すイテレータを返す
		*/
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

		/*! @brief 子の終端を指すイテレータを返す
		*/
		const_list_trie_iterator end() const
		{
			auto it = begin();

			return const_list_trie_iterator(it.m_last, it.m_last, it.m_level);
		}

		/*! @brief 子を持つ場合、trueを返す
		*/
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

			std::uint8_t ch = static_cast<std::uint8_t>(operator*());
			std::uint8_t c = *std::next(it->begin(), lv);
			if (ch == c) return true;

			return false;
		}

		/*! @brief 末尾に達した文字列が有る場合、trueを返す
		*/
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

		/*! @brief レベルに応じて次の文字位置を探し、その文字列を指すイテレータを返す

		この関数は、高速化目的でキャッシュを行うため、スレッド安全性を持たない。。
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