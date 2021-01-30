#pragma once

#include <wordring/html/simple_traits.hpp>

namespace wordring::detail
{
	template <typename Value>
	struct tag_node
	{
		using value_type = Value;

		std::uint32_t m_prev = 0;
		std::uint32_t m_next = 0;

		std::uint32_t m_head = 0;
		std::uint32_t m_tail = 0;

		value_type m_value = value_type();
	};
}
