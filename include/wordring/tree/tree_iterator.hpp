#pragma once

#include <iterator>

namespace wordring
{
	template <typename Node, typename Container>
	class tree_iterator_base
	{
	public:
		using node_type = Node;
		using container = Container;


	private:
		container m_container;
	};
}
