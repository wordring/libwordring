#pragma once

#include <list>

namespace wordring
{
	template <typename T>
	class tree_node
	{
	public:
		using container = std::list<tree_node<T>>;

		class iterator {};

	public:
		iterator begin() { return iterator(); }

		iterator end() { return iterator(); }

	private:
		T m_value;
		container m_children;
	};
}
