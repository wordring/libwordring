#pragma once

#include <deque>

#include <wordring/whatwg/html/parser_defs.hpp>

namespace wordring::whatwg::html::detail
{
	template <
		typename ErrorHandler,
		typename InputStream,
		typename Tokenizer,
		typename Dispatcher,
		typename NodeType
	>
	struct parser_context
	{
		using error_handler_type = ErrorHandler;
		using input_stream_type  = InputStream;
		using tokenizer_type     = Tokenizer;
		using dispatcher_type    = Dispatcher;
		using node_type          = NodeType;

		error_handler_type m_error_handler;
		input_stream_type  m_input_stream;
		tokenizer_type     m_tokenizer;
		dispatcher_type    m_dispatcher;

		// トークン化 ----------------------------------------------------------

		tokenizer_state m_tokenizer_state;
		tokenizer_state m_return_state;

		// 木構築 -------------------------------------------------------------

		insertion_mode        m_insertion_mode;
		std::deque<node_type> m_open_elements;
		int m_active_formatting_elements;

		bool scripting_flag;
		bool frameset_ok_flag;
	};
}
