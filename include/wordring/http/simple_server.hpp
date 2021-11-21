#pragma once

#include <wordring/http/http_defs.hpp>

#include <regex>
#include <vector>

namespace wordring::http
{
	struct request_matcher_item
	{
		std::vector<method_name> m_methods;
		std::regex m_regex;
	};

	class request_matcher
	{
	public:
		struct item
		{
			std::vector<method_name> m_methods;
			std::regex m_regex;
		};

	protected:

	};

	class http_responder
	{
		void on_request() {}
		void write() {}
		void read() {}
	};

	class websocket_responder
	{

	};

	class simple_server
	{
	protected:

	};
}
