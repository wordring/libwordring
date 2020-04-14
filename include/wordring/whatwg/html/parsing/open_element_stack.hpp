#pragma once


// https://html.spec.whatwg.org/multipage/parsing.html
// https://triple-underscore.github.io/HTML-parsing-ja.html

#include <wordring/whatwg/html/parsing/token.hpp>

#include <cstdint>
#include <string>
#include <deque>

namespace wordring::whatwg::html::parsing
{

	template <typename Policy>
	struct open_element_stack_entry
	{
		using policy = Policy;
		using node_pointer = typename policy::node_pointer;

		start_tag_token m_token;
		node_pointer    m_it;
	};

	template <typename Policy>
	class open_element_stack
	{
		using policy       = Policy;
		using node_pointer = typename policy::node_pointer;

		using container = std::deque<open_element_stack_entry<policy>>;

		using value_type = typename container::value_type;

	public:
		using iterator = typename container::iterator;
		using const_iterator = typename container::const_iterator;
		using reverse_iterator = typename container::reverse_iterator;
		using const_reverse_iterator = typename container::const_reverse_iterator;

	public:
		//value_type& front() { return m_c.front(); }

		value_type const& front() const { return m_c.front(); }

		value_type& back() { return m_c.back(); }

		value_type const& back() const { return m_c.back(); }

		//iterator begin() { return m_c.begin(); }

		//iterator end() { return m_c.end(); }

		const_iterator begin() const { return m_c.begin(); }

		const_iterator end() const { return m_c.end(); }

		//reverse_iterator rbegin() { return m_c.rbegin(); }

		const_reverse_iterator rbegin() const { return m_c.rbegin(); }

		//reverse_iterator rend() { return m_c.rend(); }

		const_reverse_iterator rend() const { return m_c.rend(); }

		bool empty() const { return m_c.empty(); }

		void push_back(value_type entry) { m_c.push_back(entry); }

		void pop_back() { m_c.pop_back(); }

	protected:

		container m_c;
	};
}
