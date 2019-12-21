#pragma once

#include <wordring/tree/basic_tree.hpp>

#include <list>

namespace wordring
{

	template <typename T, typename Allocator>
	class list_tree_base
	{
	public:
		using value_type = basic_tree<T, Allocator, list_tree_base<T, Allocator>>;

		using container = std::list<value_type>;
		using reference = typename container::reference;

	protected:
		list_tree_base() = default;

		explicit list_tree_base(Allocator const& alloc) : m_children(alloc) {}

		list_tree_base(std::initializer_list<value_type> ilist, Allocator const& alloc)
			: m_children(ilist, alloc) {}

		~list_tree_base() = default;

	public:
		// 変更 ---------------------------------------------------------------

		void push_front(value_type const& value) { m_children.push_front(value); }

		void push_front(value_type&& value) { m_children.push_front(std::move(value)); }

		template <typename... Args>
		reference emplace_front(Args&&... args) { return m_children.emplace_front(std::forward<Args>(args)...); }

		void pop_front() { m_children.pop_front(); }

		// 操作 ---------------------------------------------------------------


	protected:
		container m_children;
	};

	template <typename T, typename Allocator = std::allocator<T>>
	using tree = basic_tree<T, Allocator, list_tree_base<T, Allocator>>;
}
