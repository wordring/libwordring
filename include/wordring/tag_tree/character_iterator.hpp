#pragma once

#include <wordring/tag_tree/serial_iterator.hpp>
#include <wordring/tag_tree/tag_node.hpp>
#include <wordring/tag_tree/tag_tree_iterator.hpp>

#include <wordring/html/html_defs.hpp>

#include <cstdint>
#include <iterator>
#include <type_traits>
#include <vector>
namespace wordring
{
	template <typename Value>
	class tag_tree;
}

namespace wordring::detail
{
	template <typename Value> class tag_tree_character_iterator;

	/*! @brief HTML/XML の文字を巡回するイテレータ
	* 
	* @internal
	* <hr>
	* 
	* 
	*/
	template <typename Value>
	class const_tag_tree_character_iterator
	{
		friend class tag_tree_character_iterator<Value>;

	public:
		using node_traits       = html::node_traits<tag_tree_iterator<Value>>;
		using node_type         = typename node_traits::node_type;
		using node_pointer      = typename node_traits::node_pointer;

		using string_type    = typename node_traits::string_type;
		using character_type = typename string_type::value_type;

		using base_type = tag_tree_serial_iterator<Value>;
		using container = typename base_type::container;

		using value_type        = std::remove_cv_t<character_type>;
		using difference_type   = std::ptrdiff_t;
		using reference         = value_type const&;
		using pointer           = value_type const*;
		using iterator_category = std::bidirectional_iterator_tag;

	public:
		const_tag_tree_character_iterator()
			: m_it()
			, m_i(0)
		{}

		const_tag_tree_character_iterator(node_pointer np)
			: m_it(np)
			, m_i(0)
		{
			m_it = next();
		}

		const_tag_tree_character_iterator end() const
		{
			return { node_pointer(m_it.m_c, 0) };
		}

		operator base_type() const { return m_it; }

		/*! @brief tag_tree_iterator へ変換する
		*/
		operator tag_tree_iterator<Value>() const
		{
			return static_cast<tag_tree_iterator<Value>>(m_it);
		}

		/*! @brief 文字を返す
		*/
		reference operator*() const
		{
			assert(m_it.m_i != 0);
			assert(node_traits::is_text(m_it));

			return node_traits::data(m_it)[m_i];
		}

		pointer operator->() const
		{
			assert(m_it.m_i != 0);
			assert(node_traits::is_text(m_it));
			
			return &node_traits::data(m_it)[m_i];
		}

		/*! @brief イテレータを進める
		*
		* @return *this
		*/
		const_tag_tree_character_iterator& operator++()
		{
			assert(m_it.m_i != 0);

			if (node_traits::is_text(m_it))
			{
				++m_i;
				if (m_i < node_traits::data(m_it).size()) return *this;

				++m_it;
				m_i = 0;
			}

			m_it = next();

			return *this;
		}

		/*! @brief イテレータを進める
		*
		* @return 進める前のイテレータ
		*/
		const_tag_tree_character_iterator operator++(int)
		{
			const_tag_tree_character_iterator it(*this);
			operator++();
			return it;
		}

		const_tag_tree_character_iterator& operator--()
		{
			if (m_it != base_type())
			{
				if (node_traits::is_text(m_it))
				{
					if (m_i != 0)
					{
						--m_i;
						return *this;
					}
					--m_it;
				}
			}
			else --m_it;

			m_it = prev();

			if (m_it != base_type()) m_i = node_traits::data(m_it).size() - 1;

			return *this;
		}

		const_tag_tree_character_iterator operator--(int)
		{
			const_tag_tree_character_iterator it(*this);
			operator--();
			return it;
		}

		bool operator==(const_tag_tree_character_iterator const& x) const
		{
			return m_it == x.m_it && m_i == x.m_i;
		}

		bool operator!=(const_tag_tree_character_iterator const& x) const
		{
			return !(operator==(x));
		}

	protected:
		/*! @brief 前のテキストノードを指すイテレータを返す
		* 
		* 既にテキストノードを指している場合、現在のイテレータを返す。
		* 
		* @internal
		* <hr>
		* 
		* tag_tree の直列イテレータは、 begin() の前も end() と一致するので、特別な配慮無く
		* デクリメントできる。
		*/
		base_type prev() const
		{
			auto it1 = m_it;
			auto it2 = base_type();
			while (it1 != it2 && !node_traits::is_text(it1)) --it1;

			return it1;
		}

		/*! @brief 次のテキストノードを指すイテレータを返す
		* 
		* 既にテキストノードを指している場合、現在のイテレータを返す。
		*/
		base_type next() const
		{
			auto it1 = m_it;
			auto it2 = base_type();
			while (it1 != it2 && !node_traits::is_text(it1)) ++it1;

			return it1;
		}

	protected:
		base_type     m_it;
		std::uint32_t m_i;
	};



	template <typename Value>
	class tag_tree_character_iterator : public const_tag_tree_character_iterator<Value>
	{
	public:
		using node_traits  = html::node_traits<tag_tree_iterator<Value>>;
		using node_type    = typename node_traits::node_type;
		using node_pointer = typename node_traits::node_pointer;

		using string_type    = typename node_traits::string_type;
		using character_type = typename string_type::value_type;

		using value_type        = std::remove_cv_t<character_type>;
		using difference_type   = std::ptrdiff_t;
		using reference         = value_type&;
		using pointer           = value_type*;
		using iterator_category = std::bidirectional_iterator_tag;

		using base_type = const_tag_tree_character_iterator<Value>;

	private:
		using base_type::m_it;
		using base_type::m_i;

	public:
		tag_tree_character_iterator()
			: base_type()
		{}

		tag_tree_character_iterator(node_pointer np)
			: base_type(np)
		{}

		tag_tree_character_iterator end() const
		{
			return { node_pointer(m_it.m_c, 0) };
		}

		operator base_type() const { return m_it; }

		/*! @brief tag_tree_iterator へ変換する
		*/
		operator tag_tree_iterator<Value>() const
		{
			return static_cast<tag_tree_iterator<Value>>(m_it);
		}

		/*! @brief 文字を返す
		*/
		reference operator*() const
		{
			return const_cast<reference>(base_type::operator*());
		}

		pointer operator->() const
		{
			return const_cast<pointer>(base_type::operator->());
		}

		/*! @brief イテレータを進める
		*
		* @return *this
		*/
		tag_tree_character_iterator& operator++()
		{
			base_type::operator++();
			return *this;
		}

		/*! @brief イテレータを進める
		*
		* @return 進める前のイテレータ
		*/
		tag_tree_character_iterator operator++(int)
		{
			tag_tree_character_iterator it(*this);
			base_type::operator++();
			return it;
		}

		tag_tree_character_iterator& operator--()
		{
			base_type::operator--();
			return *this;
		}

		tag_tree_character_iterator operator--(int)
		{
			tag_tree_character_iterator it(*this);
			base_type::operator--();
			return it;
		}
	};

}
