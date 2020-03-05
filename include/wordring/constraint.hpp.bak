#pragma once

#include <iterator>
#include <type_traits>

namespace wordring
{
	/*!
	*/
	template <typename, typename = void>
	struct is_iterator : std::false_type {};

	template <typename T>
	struct is_iterator<T, std::void_t<typename std::iterator_traits<T>::iterator_category>> : std::true_type {};

	template <typename T>
	inline constexpr bool is_iterator_v = is_iterator<T>::value;

	/*!
	*/
	template <typename, typename = void>
	struct has_iterator : std::false_type {};

	template <typename T>
	struct has_iterator<T, std::void_t<typename T::iterator>> : std::true_type {};

	template <typename T>
	inline constexpr bool has_iterator_v = has_iterator<T>::value;

	/*!
	*/
	template <typename, typename = void>
	struct has_value_type : std::false_type {};

	template <typename T>
	struct has_value_type<T, std::void_t<typename T::value_type>> : std::true_type {};

	template <typename T>
	inline constexpr bool has_value_type_v = has_value_type<T>::value;

	/*!
	*/
	template <typename, typename = void>
	struct is_container : std::false_type {};

	template <typename T>
	struct is_container<T,
		std::void_t<std::enable_if_t<std::negation_v<is_iterator<T>>>>> : std::true_type {};

	template <typename T>
	inline constexpr bool is_container_v = is_container<T>::value;

	/*!
	*/
	//template <typename T>
	//using is_output_iterator = std::is_same<typename std::iterator_traits<T>::iterator_category, std::output_iterator_tag>;

	template <typename, typename = void>
	struct is_output_iterator : std::false_type {};

	template <typename T>
	struct is_output_iterator<T, std::void_t<std::is_same<typename std::iterator_traits<T>::iterator_category, std::output_iterator_tag>>> : std::true_type {};

	template <typename T>
	inline constexpr bool is_output_iterator_v = is_output_iterator<T>::value;
}
