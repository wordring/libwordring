#pragma once

#include <wordring/static_vector/static_vector.hpp>
#include <wordring/utility/result_range.hpp>

#include <algorithm>
#include <iterator>
#include <deque>
#include <type_traits>
#include <utility>

namespace wordring
{
	/*!
	*/
	template <typename Iterator>
	class trie_construct_iterator
	{
		template <typename Iterator1>
		friend bool operator==(trie_construct_iterator<Iterator1> const&, trie_construct_iterator<Iterator1> const&);

		template <typename Iterator1>
		friend bool operator!=(trie_construct_iterator<Iterator1> const&, trie_construct_iterator<Iterator1> const&);

	public:
		using iterator_type = Iterator;

		using difference_type   = typename std::iterator_traits<iterator_type>::difference_type;
		using value_type        = std::uint16_t;
		using pointer           = value_type*;
		using reference         = value_type&;
		using iterator_category = std::input_iterator_tag;

		using string_iterator = typename iterator_type::string_iterator;

		using label_vector = static_vector<std::uint16_t, 257>;
		using parent_view  = result_range<string_iterator>;

		static constexpr std::uint16_t null_value = 256u;

	public:
		trie_construct_iterator(iterator_type root)
			: m_queue(1, root)
		{
			assign_children();
		}

		iterator_type base() const
		{
			return m_queue.front();
		}

		parent_view parent() const
		{
			return m_queue.front().string();
		}

		label_vector const& children() const
		{
			return m_labels;
		}

		bool empty() const { return m_queue.empty(); }

		trie_construct_iterator& operator++()
		{
			m_queue.pop_front();
			assign_children();
			return *this;
		}

		trie_construct_iterator operator++(int)
		{
			auto tmp = *this;
			operator++();
			return tmp;
		}

	protected:
		void assign_children()
		{
			m_labels.clear();
			if (empty()) return;

			auto it = m_queue.front();
			auto it1 = it.begin();
			auto it2 = it.end();

			while (it1 != it2)
			{
				m_labels.push_back(*it1);
				if (it1.has_child()) m_queue.push_back(it1);
				++it1;
			}
			if (it.has_null()) m_labels.push_back(null_value);
		}

	protected:
		std::deque<iterator_type> m_queue;
		label_vector              m_labels;
	};

	template <typename Iterator1>
	inline bool operator==(trie_construct_iterator<Iterator1> const& lhs, trie_construct_iterator<Iterator1> const& rhs)
	{
		return !(lhs != rhs);
	}

	template <typename Iterator1>
	inline bool operator!=(trie_construct_iterator<Iterator1> const& lhs, trie_construct_iterator<Iterator1> const& rhs)
	{
		return lhs.m_queue.size() != rhs.m_queue.size() || lhs.m_queue != rhs.m_queue;
	}

}
