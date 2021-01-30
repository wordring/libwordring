#pragma once

#include <wordring/tag_tree/serial_iterator.hpp>
#include <wordring/tag_tree/tag_node.hpp>
#include <wordring/tag_tree/tag_tree_iterator.hpp>

#include <wordring/html/html_defs.hpp>

#include <cstdint>
#include <iterator>
#include <type_traits>

namespace wordring
{
	template <typename Value>
	class tag_tree;
}

namespace wordring::detail
{
	template <typename Value>
	class tag_tree_character_iterator
	{
		template <typename Value1>
		friend bool operator==(tag_tree_character_iterator<Value1> const&, tag_tree_character_iterator<Value1> const&);

		template <typename Value1>
		friend bool operator!=(tag_tree_character_iterator<Value1> const&, tag_tree_character_iterator<Value1> const&);

	public:
		using node_traits       = html::node_traits<tag_tree_iterator<std::remove_cv_t<Value>>>;
		using const_node_traits = html::node_traits<tag_tree_iterator<std::remove_cv_t<Value> const>>;
		using node_type         = typename node_traits::node_type;
		using node_pointer      = typename node_traits::node_pointer;

		using string_type    = typename node_traits::string_type;
		using character_type = typename string_type::value_type;

		using base_type = tag_tree_serial_iterator<Value>;
		using container = typename base_type::container;

		using value_type        = std::remove_cv_t<character_type>;
		using difference_type   = std::ptrdiff_t;
		using reference         = std::conditional_t<std::is_const_v<Value>, value_type const, value_type>&;
		using pointer           = std::conditional_t<std::is_const_v<Value>, value_type const, value_type>*;
		using iterator_category = std::bidirectional_iterator_tag;

		//static_assert(sizeof(character_type) == sizeof(value_type));

	public:
		tag_tree_character_iterator() = default;
		tag_tree_character_iterator(node_pointer np)
			: m_first(np)
			, m_last(np.m_c, 0)
			, m_i(0)
		{
			while (m_first != m_last && !node_traits::is_text(m_first)) ++m_first;
		}

		tag_tree_character_iterator end() const { return tag_tree_character_iterator(); }

		/*! @brief 文字を返す
		*/
		reference operator*() const
		{
			return const_cast<reference>(node_traits::data(m_first)[m_i]);
		}

		/*! @brief イテレータを進める
		*
		* @return *this
		*/
		tag_tree_character_iterator& operator++()
		{
			if (node_traits::is_text(m_first))
			{
				++m_i;
				if (m_i < node_traits::data(m_first).size()) return *this;

				++m_first;
				m_i = 0;
			}
			while (m_first != m_last && !node_traits::is_text(m_first)) ++m_first;

			return *this;
		}

		/*! @brief イテレータを進める
		*
		* @return 進める前のイテレータ
		*/
		tag_tree_character_iterator operator++(int)
		{
			tag_tree_character_iterator it(*this);
			operator++();
			return it;
		}

		tag_tree_character_iterator& operator--()
		{
			if (node_traits::is_text(m_first))
			{
				if (m_i != 0)
				{
					--m_i;
					return *this;
				}
				--m_first;
			}

			while (m_first != m_last && !node_traits::is_text(m_first)) --m_first;
			
			if (m_first != m_last)
			{
				m_i = node_traits::data(m_first).size() - 1;
			}


			return *this;
		}
	protected:
		base_type     m_first;
		base_type     m_last;
		std::uint32_t m_i;
	};

	template <typename Value1>
	inline bool operator==(tag_tree_character_iterator<Value1> const& lhs, tag_tree_character_iterator<Value1> const& rhs)
	{
		return lhs.m_first == rhs.m_first && lhs.m_i == rhs.m_i;
	}

	template <typename Value1>
	inline bool operator!=(tag_tree_character_iterator<Value1> const& lhs, tag_tree_character_iterator<Value1> const& rhs)
	{
		return !(lhs == rhs);
	}
}
