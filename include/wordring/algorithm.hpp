#pragma once

#include <iterator>
#include <type_traits>

#include <wordring/constraint.hpp>

// ------------------------------------------------------------------------

namespace wordring
{
	template <typename Value, typename OutputIterator, typename std::enable_if_t<is_output_iterator_v<OutputIterator>, nullptr_t> = nullptr>
	void push_back(Value value, OutputIterator & output)
	{
		*output++ = value;
	}

	template <typename Value, typename Container, typename std::enable_if_t<std::is_object_v<typename Container::value_type>, nullptr_t> = nullptr>
	void push_back(Value value, Container & output)
	{
		output.push_back(value);
	}
}
