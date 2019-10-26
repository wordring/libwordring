#pragma once

#include <iterator>
#include <type_traits>

#include <wordring/constraint.hpp>

// ------------------------------------------------------------------------

namespace wordring
{
	template <typename Value, typename OutputIterator,
		typename std::enable_if_t<is_output_iterator_v<OutputIterator>, nullptr_t> = nullptr>
	inline void push_back_(Value value, OutputIterator & output)
	{
		*output++ = value;
	}

	template <typename Value, typename Container,
		typename std::enable_if_t<std::is_object_v<typename Container::value_type>, nullptr_t> = nullptr>
	inline void push_back_(Value value, Container & output)
	{
		output.push_back(value);
	}

	template <typename Container1, typename Container2,
		typename std::enable_if_t<std::conjunction_v<is_container<Container1>, is_container<Container2>>, nullptr_t> = nullptr>
	inline bool equal(Container1 const & container_1, Container2 const& container_2)
	{
		return std::equal(container_1.begin(), container_1.end(), container_2.begin(), container_2.end());
	}
}
