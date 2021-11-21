#pragma once

namespace wordring::http
{
	template <typename String>
	class simple_client
	{
	public:
		using string_type = String;

	public:
		template <typename>
		void get(string_type const& url);
	};
}
