#pragma once

#include <wordring/tag_tree/tag_node.hpp>
#include <wordring/tag_tree/tag_tree_iterator.hpp>

namespace wordring
{
	template <typename Value>
	class tag_tree;
}

namespace wordring::detail
{
	template <typename Value>
	class tag_tree_serial_iterator
	{
		friend class wordring::tag_tree<std::remove_cv_t<Value>>;
		friend class wordring::tag_tree<std::remove_cv_t<Value> const>;

		friend class tag_tree_character_iterator<std::remove_cv_t<Value>>;
		friend class tag_tree_character_iterator<std::remove_cv_t<Value const>>;

		template <typename Value1, typename Value2>
		friend bool operator==(tag_tree_serial_iterator<Value1> const&, tag_tree_serial_iterator<Value2> const&);

		template <typename Value1, typename Value2>
		friend bool operator!=(tag_tree_serial_iterator<Value1> const&, tag_tree_serial_iterator<Value2> const&);

	public:
		using value_type = std::remove_cv_t<Value>;
		using wrapper    = tag_node<value_type>;
		
		using difference_type   = std::ptrdiff_t;
		using reference         = std::conditional_t<std::is_const_v<Value>, value_type const&, value_type&>;
		using pointer           = std::conditional_t<std::is_const_v<Value>, value_type const*, value_type*>;
		using iterator_category = std::bidirectional_iterator_tag;

		using container = std::vector<detail::tag_node<value_type>>;

	public:
		tag_tree_serial_iterator()
			: m_c(nullptr)
			, m_i(0) {}

		tag_tree_serial_iterator(tag_tree_iterator<value_type> const& x)
			: m_c(x.m_c)
			, m_i(x.m_i) {}

		tag_tree_serial_iterator(container* c, std::uint32_t i)
			: m_c(c)
			, m_i(i) {}

		/*! @brief const_serial_iterator へ変換する
		*/
		operator tag_tree_serial_iterator<value_type const>() const
		{
			return tag_tree_serial_iterator<value_type const>(m_c, m_i);
		}

		/*! @brief tag_tree_iterator へ変換する
		*/
		operator tag_tree_iterator<Value>() const
		{
			return tag_tree_iterator<Value>(m_c, m_i);
		}

		reference operator*() const
		{
			return const_cast<std::remove_cv_t<reference>>((m_c->data() + m_i)->m_value);
		}

		pointer operator->() const
		{
			return const_cast<std::remove_cv_t<pointer>>(&(m_c->data() + m_i)->m_value);
		}

		tag_tree_serial_iterator& operator++()
		{
			assert(m_i != 0);
			m_i = (m_c->data() + m_i)->m_next;

			return *this;
		}

		tag_tree_serial_iterator operator++(int)
		{
			tag_tree_iterator it = *this;
			operator++();
			return it;
		}

		tag_tree_serial_iterator& operator--()
		{
			m_i = (m_c->data() + m_i)->m_prev;

			return *this;
		}

		tag_tree_serial_iterator operator--(int)
		{
			tag_tree_iterator it = *this;
			operator--();
			return it;
		}

		/*! @brief 開始タグか検査する
		* 
		* @return 開始タグを指す場合、 true を返す
		*/
		bool is_start_tag() const { return (m_c->data() + m_i)->m_tail != 0; }

		/*! @brief 終了タグか検査する
		*
		* @return 終了タグを指す場合、 true を返す
		*/
		bool is_end_tag() const { return (m_c->data() + m_i)->m_head != 0; };

		/*! @brief 開始タグを返す
		*
		* @return 開始タグを指すイテレータを返す
		* 
		* 要素以外を指すイテレータ上で呼び出した場合、 send() を返す。
		*/
		tag_tree_serial_iterator start_tag() const
		{
			wrapper* d = m_c->data();
			return tag_tree_serial_iterator(m_c, is_start_tag() ? m_i : (d + m_i)->m_head);
		}

		/*! @brief 終了タグを返す
		*
		* @return 終了タグを指すイテレータを返す
		* 
		* 要素以外を指すイテレータ上で呼び出した場合、 send() を返す。
		*/
		tag_tree_serial_iterator end_tag() const
		{
			wrapper* d = m_c->data();
			return tag_tree_serial_iterator(m_c, is_end_tag() ? m_i : (d + m_i)->m_tail);
		}

	protected:
		container*    m_c;
		std::uint32_t m_i;
	};

	template <typename Value1, typename Value2>
	inline bool operator==(tag_tree_serial_iterator<Value1> const& lhs, tag_tree_serial_iterator<Value2> const& rhs)
	{
		assert(lhs.m_c == nullptr || rhs.m_c == nullptr || lhs.m_c->data() == rhs.m_c->data());
		return lhs.m_i == rhs.m_i;
	}

	template <typename Value1, typename Value2>
	inline bool operator!=(tag_tree_serial_iterator<Value1> const& lhs, tag_tree_serial_iterator<Value2> const& rhs)
	{
		return !(lhs == rhs);
	}
}
