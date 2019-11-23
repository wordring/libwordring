#include <wordring/whatwg/url/host.hpp>

using namespace wordring::whatwg;

std::optional<host> host::get_public_suffix() const
{
	if (type() != host_type::domain) return std::optional<host>{};

	return std::optional<host>();
}

host_type host::type() const noexcept
{
	return static_cast<host_type>(m_value.index());
}

std::optional<host> get_public_suffix()
{
	return std::optional<host>();
}
