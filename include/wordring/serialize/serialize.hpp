#pragma once

#include <array>
#include <cstddef>
#include <type_traits>

namespace wordring
{
	template <typename T, typename std::enable_if_t<std::is_integral_v<T>, std::nullptr_t> = nullptr>
	inline auto serialize(T number)
	{
		std::size_t constexpr length = sizeof(T);
		std::make_unsigned_t<T> n = number;

		std::array<std::uint8_t, length> result{};
		for (int i = 0; i < length; i++) result[i] = (n >> ((length - i - 1) * 8)) & 0xFFu;
		return result;
	}

	template <typename T, typename std::enable_if_t<std::is_integral_v<T>, std::nullptr_t> = nullptr>
	inline uint8_t serialize(T number, uint32_t position)
	{
		return (std::make_unsigned_t<T>(number) >> ((sizeof(T) - position - 1) * 8)) & 0xFFu;
	}

	template <typename InputIterator, typename T>
	inline auto deserialize(InputIterator first, InputIterator last, T& result)
	{
		using unsigned_type = std::make_unsigned_t<typename std::iterator_traits<InputIterator>::value_type>;

		std::make_unsigned_t<T> buf = 0;
		for (std::size_t i = 0; i < sizeof(T) && first != last; ++i)
			buf = (buf << 8) + static_cast<unsigned_type>(*first++);
		result = buf;

		return first;
	}
}
