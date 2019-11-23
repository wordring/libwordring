#pragma once

// https://url.spec.whatwg.org/
// https://triple-underscore.github.io/URL-ja.html

#include <array>
#include <cassert>
#include <cstdint>
#include <optional>
#include <string>
#include <variant>

namespace wordring::whatwg
{
	enum class host_type : uint32_t
	{
		domain       = 0,
		ipv4_address = 1,
		ipv6_address = 2,
		opaque_host  = 3,
		empty_host   = 4,
	};

	class domain
	{
	private:
		std::u32string m_value;
	};

	class ipv4_address
	{
	private:
		uint32_t m_value;
	};

	class ipv6_address
	{
	private:
		std::array<uint16_t, 8> m_value;
	};

	class opaque_host
	{
	private:
		std::u32string m_value;
	};

	class empty_host
	{
	private:
		std::u32string m_value;
	};

	class host
	{
	public:
		std::optional<host> get_public_suffix() const;

		host_type type() const noexcept;

	private:
		std::variant<domain, ipv4_address, ipv6_address, opaque_host, empty_host> m_value;
	};

	inline bool is_forbidden_host_code_point(uint32_t cp)
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

	std::optional<host> get_public_suffix();
}
