#pragma once

// https://url.spec.whatwg.org/
// https://triple-underscore.github.io/URL-ja.html

#include <wordring/whatwg/html/parser_defs.hpp>

#include <array>
#include <cassert>
#include <cstdint>
#include <optional>
#include <string>
#include <variant>

namespace wordring::whatwg
{
	struct domain { std::string v; };

	struct ipv4_address { std::uint32_t v; };

	struct ipv6_address { std::array<std::uint16_t, 8> v; };

	struct opaque_host { std::string v; };

	struct empty_host { std::nullptr_t v; };

	using host = std::variant<
		domain,
		ipv4_address,
		ipv6_address,
		opaque_host,
		empty_host>;

	inline bool is_forbidden_host_code_point(std::uint32_t cp)
	{
		switch (cp)
		{
		case 0:
		case 0x9u:
		case 0xAu:
		case 0xDu:
		case 0x20u:
		case 0x23u:
		case 0x25u:
		case 0x2Fu:
		case 0x3Au:
		case 0x3Fu:
		case 0x40u:
		case 0x5Bu:
		case 0x5Cu:
		case 0x5Du:
			return true;
		}
		return false;
	}
}
