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
	template <typename Value> class tag_tree_serial_iterator;

	template <typename Value>
	class const_tag_tree_serial_iterator
	{
		friend class wordring::tag_tree<Value>;

		friend class const_tag_tree_character_iterator<Value>;
		friend class tag_tree_character_iterator<Value>;

		friend class const_tag_tree_iterator<Value>;
		friend class tag_tree_iterator<Value>;

		friend class tag_tree_serial_iterator<Value>;

	public:
		using value_type = std::remove_cv_t<Value>;
		using wrapper    = tag_node<value_type>;
		
		using difference_type   = std::ptrdiff_t;
		using reference         = value_type const&;
		using pointer           = value_type const*;
		using iterator_category = std::bidirectional_iterator_tag;

		using container = std::vector<detail::tag_node<value_type>>;

	public:
		const_tag_tree_serial_iterator()
			: m_c(nullptr)
			, m_i(0) {}

		const_tag_tree_serial_iterator(const_tag_tree_iterator<Value> const& x)
			: m_c(x.m_c)
			, m_i(x.m_i) {}

		const_tag_tree_serial_iterator(tag_tree_iterator<Value> const& x)
			: m_c(x.m_c)
			, m_i(x.m_i) {}

		const_tag_tree_serial_iterator(container* c, std::uint32_t i)
			: m_c(c)
			, m_i(i) {}

		/*! @brief const_tag_tree_iterator へ変換する
		*/
		operator const_tag_tree_iterator<Value>() const
		{
			return const_tag_tree_iterator<Value>(m_c, m_i);
		}

		reference operator*() const
		{
			return (m_c->data() + m_i)->m_value;
		}

		pointer operator->() const
		{
			return &(m_c->data() + m_i)->m_value;
		}

		const_tag_tree_serial_iterator& operator++()
		{
			assert(m_i != 0);
			m_i = (m_c->data() + m_i)->m_next;

			return *this;
		}

		const_tag_tree_serial_iterator operator++(int)
		{
			tag_tree_iterator it = *this;
			operator++();
			return it;
		}

		const_tag_tree_serial_iterator& operator--()
		{
			m_i = (m_c->data() + m_i)->m_prev;

			return *this;
		}

		const_tag_tree_serial_iterator operator--(int)
		{
			const_tag_tree_serial_iterator it = *this;
			operator--();
			return it;
		}

		bool operator==(const_tag_tree_serial_iterator const& x) const
		{
			assert(m_c == nullptr || x.m_c == nullptr || m_c->data() == x.m_c->data());
			return m_i == x.m_i;
		}

		bool operator!=(const_tag_tree_serial_iterator const& x) const
		{
			return !operator==(x);
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
		const_tag_tree_serial_iterator start_tag() const
		{
			wrapper* d = m_c->data();
			return const_tag_tree_serial_iterator(m_c, is_start_tag() ? m_i : (d + m_i)->m_head);
		}

		/*! @brief 終了タグを返す
		*
		* @return 終了タグを指すイテレータを返す
		* 
		* 要素以外を指すイテレータ上で呼び出した場合、 send() を返す。
		*/
		const_tag_tree_serial_iterator end_tag() const
		{
			wrapper* d = m_c->data();
			return const_tag_tree_serial_iterator(m_c, is_end_tag() ? m_i : (d + m_i)->m_tail);
		}

	protected:
		container*    m_c;
		std::uint32_t m_i;
	};

	template <typename Value>
	class tag_tree_serial_iterator : public const_tag_tree_serial_iterator<Value>
	{
		friend class wordring::tag_tree<Value>;

		friend class const_tag_tree_character_iterator<Value>;
		friend class tag_tree_character_iterator<Value>;

	public:
		using value_type = std::remove_cv_t<Value>;
		using wrapper    = tag_node<value_type>;

		using difference_type   = std::ptrdiff_t;
		using reference         = value_type&;
		using pointer           = value_type*;
		using iterator_category = std::bidirectional_iterator_tag;

		using container = std::vector<detail::tag_node<value_type>>;

		using base_type = const_tag_tree_serial_iterator<Value>;

	private:
		using base_type::m_c;
		using base_type::m_i;

	public:
		tag_tree_serial_iterator()
			: base_type()
		{}

		tag_tree_serial_iterator(tag_tree_iterator<Value> const& x)
			: base_type(x)
		{}

		tag_tree_serial_iterator(container* c, std::uint32_t i)
			: base_type(c, i)
		{}

		/*! @brief tag_tree_iterator へ変換する
		*/
		operator tag_tree_iterator<Value>() const
		{
			return tag_tree_iterator<Value>(m_c, m_i);
		}

		reference operator*() const
		{
			return const_cast<reference>(base_type::operator*());
		}

		pointer operator->() const
		{
			return const_cast<pointer>(base_type::operator->());
		}

		tag_tree_serial_iterator& operator++()
		{
			base_type::operator++();
			return *this;
		}

		tag_tree_serial_iterator operator++(int)
		{
			tag_tree_iterator it = *this;
			base_type::operator++();
			return it;
		}

		tag_tree_serial_iterator& operator--()
		{
			base_type::operator--();
			return *this;
		}

		tag_tree_serial_iterator operator--(int)
		{
			tag_tree_iterator it = *this;
			base_type::operator--();
			return it;
		}

		/*! @brief 開始タグか検査する
		*
		* @return 開始タグを指す場合、 true を返す
		*/
		using base_type::is_start_tag;

		/*! @brief 終了タグか検査する
		*
		* @return 終了タグを指す場合、 true を返す
		*/
		using base_type::is_end_tag;

		/*! @brief 開始タグを返す
		*
		* @return 開始タグを指すイテレータを返す
		*
		* 要素以外を指すイテレータ上で呼び出した場合、 send() を返す。
		*/
		tag_tree_serial_iterator start_tag() const
		{
			auto it = base_type::start_tag();
			return tag_tree_serial_iterator(it.m_c, it.m_i);
		}

		/*! @brief 終了タグを返す
		*
		* @return 終了タグを指すイテレータを返す
		*
		* 要素以外を指すイテレータ上で呼び出した場合、 send() を返す。
		*/
		tag_tree_serial_iterator end_tag() const
		{
			auto it = base_type::end_tag();
			return tag_tree_serial_iterator(it.m_c, it.m_i);
		}
	};
}
