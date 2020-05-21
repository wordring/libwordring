#pragma once

// https://html.spec.whatwg.org/multipage/parsing.html
// https://triple-underscore.github.io/HTML-parsing-ja.html

#include <wordring/whatwg/encoding/encoding.hpp>

#include <wordring/whatwg/html/parsing/atom_tbl.hpp>
#include <wordring/whatwg/html/parsing/input_stream.hpp>
#include <wordring/whatwg/html/parsing/parser_defs.hpp>
#include <wordring/whatwg/html/parsing/tokenization.hpp>
#include <wordring/whatwg/html/parsing/tree_construction_dispatcher.hpp>

#include <iostream>

namespace wordring::whatwg::html::parsing
{
	template <typename T, typename Policy>
	class parser : public tree_construction_dispatcher<T, Policy>
	{
	public:
		using base_type = tree_construction_dispatcher<T, Policy>;
		using this_type = T;

		using policy = Policy;

		using string_type    = typename policy::string_type;
		using container_type = typename policy::container_type;
		using node_type      = typename policy::node_type;
		using node_pointer   = typename container_type::iterator;


		using base_type::report_error;
		using base_type::eof;

	public:
		parser()
			: m_script_nesting_level(0)
			, m_parser_pause_flag(false)
		{
		}



		void change_encoding(wordring::whatwg::encoding::name name) {}

		void on_report_error(error e) {}

	protected:

		encoding_confidence m_confidence;

		std::uint32_t m_script_nesting_level;
		bool m_parser_pause_flag;

		container_type m_container_type;
	};
}
