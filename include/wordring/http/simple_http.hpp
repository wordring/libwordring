#pragma once

namespace wordring::http
{
	template <typename string>
	class simple_client
	{
	public:
		void get();
	};

	template <typename Container, typename String>
	Container get_resource(String url)
	{
		return Container{};
	}
}
