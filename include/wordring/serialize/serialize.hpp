#pragma once

#include <array>
#include <cstddef>
#include <type_traits>

namespace wordring
{
	template <typename T, typename std::enable_if_t<std::is_integral_v<T>, std::nullptr_t> = nullptr>
	inline auto serialize(T number)
	{
		std::size_t constexpr l = sizeof(T);
		std::make_unsigned_t<T> n = number;

		std::array<std::uint8_t, l> result{};
		for (int i = 0; i < l; i++) result[i] = (n >> ((l - i - 1) * 8)) & 0xFFu;
		return result;
	}

	template <typename T, typename std::enable_if_t<std::is_integral_v<T>, std::nullptr_t> = nullptr>
	inline uint8_t serialize(T number, uint32_t position)
	{
		return (std::make_unsigned_t<T>(number) >> ((sizeof(T) - position - 1) * 8)) & 0xFFu;
	}
}
