#pragma once

#include <wordring/tree/basic_tree.hpp>

#include <vector>

namespace wordring
{
	template <typename T, typename Allocator>
	class vector_tree_base
	{
	public:
		using value_type = basic_tree<T, Allocator, vector_tree_base<T, Allocator>>;
		using container = std::vector<basic_tree<T, Allocator, vector_tree_base<T, Allocator>>>;

		using reference = typename container::reference;
		using const_reference = typename container::const_reference;
		using size_type = typename container::size_type;

	protected:
		vector_tree_base() = default;

		explicit vector_tree_base(Allocator const& alloc) : m_children(alloc) {}

		vector_tree_base(std::initializer_list<typename container::value_type> ilist, Allocator const& alloc)
			: m_children(ilist, alloc) {}

		~vector_tree_base() = default;

	public:
		// 要素アクセス --------------------------------------------------------

		reference at(size_type pos) { return m_children.at(pos); }

		const_reference at(size_type pos) const { return m_children.at(pos); }

	protected:
		container m_children;
	};

	template <typename T, typename Allocator = std::allocator<T>>
	using vector_tree = basic_tree<T, Allocator, vector_tree_base<T, Allocator>>;
}
