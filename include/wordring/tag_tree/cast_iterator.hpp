#pragma once

#include <wordring/tag_tree/serial_iterator.hpp>
#include <wordring/tag_tree/tag_tree_iterator.hpp>

#include <cstdint>
#include <iterator>
#include <type_traits>

namespace wordring::detail
{
	template <typename Char, typename NodePointer> class tag_tree_cast_iterator;

	template <typename Char, typename NodePointer>
	class const_tag_tree_cast_iterator
	{
		friend class tag_tree_cast_iterator<Char, NodePointer>;

	public:
		using node_pointer = NodePointer;

		using value_type        = Char;
		using difference_type   = typename std::iterator_traits<node_pointer>::difference_type;
		using reference         = value_type const&;
		using pointer           = value_type const*;
		using iterator_category = typename std::iterator_traits<node_pointer>::iterator_category;

		using base_type = tag_tree_character_iterator<typename node_pointer::value_type>;

	public:
		const_tag_tree_cast_iterator()
			: m_it()
		{}

		const_tag_tree_cast_iterator(node_pointer it)
			: m_it(it)
		{}

		operator node_pointer() const { return m_it; }

		reference operator*() const
		{
			return reinterpret_cast<reference>(*m_it);
		}

		pointer operator->() const
		{
			return reinterpret_cast<pointer>(m_it.operator->());
		}

		const_tag_tree_cast_iterator& operator++()
		{
			++m_it;
			return *this;
		}

		const_tag_tree_cast_iterator operator++(int)
		{
			auto it = *this;
			++m_it;
			return it;
		}

		const_tag_tree_cast_iterator& operator--()
		{
			--m_it;
			return *this;
		}

		const_tag_tree_cast_iterator operator--(int)
		{
			auto it = *this;
			--m_it;
			return it;
		}

		bool operator==(const_tag_tree_cast_iterator const& x) const
		{
			return m_it == x.m_it;
		}

		bool operator!=(const_tag_tree_cast_iterator const& x) const
		{
			return !(operator==(x));
		}

	protected:
		base_type m_it;
	};

	template <typename Char, typename NodePointer>
	class tag_tree_cast_iterator : public const_tag_tree_cast_iterator<Char, NodePointer>
	{
	public:
		using node_pointer = NodePointer;

		using value_type        = Char;
		using difference_type   = typename std::iterator_traits<node_pointer>::difference_type;
		using reference         = value_type&;
		using pointer           = value_type*;
		using iterator_category = typename std::iterator_traits<node_pointer>::iterator_category;

		using base_type = const_tag_tree_cast_iterator<Char, NodePointer>;

	private:
		using base_type::m_it;

	public:
		tag_tree_cast_iterator()
			: base_type()
		{}

		tag_tree_cast_iterator(node_pointer it)
			: base_type(it)
		{}

		operator node_pointer() const { return m_it; }

		reference operator*() const
		{
			return reinterpret_cast<reference>(*m_it);
		}

		pointer operator->() const
		{
			return reinterpret_cast<pointer>(m_it.operator->());
		}

		tag_tree_cast_iterator& operator++()
		{
			base_type::operator++();
			return *this;
		}

		tag_tree_cast_iterator operator++(int)
		{
			auto it = *this;
			base_type::operator++();
			return it;
		}

		tag_tree_cast_iterator& operator--()
		{
			base_type::operator--();
			return *this;
		}

		tag_tree_cast_iterator operator--(int)
		{
			auto it = *this;
			base_type::operator--();
			return it;
		}
	};
}
