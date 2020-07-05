#pragma once

#include <deque>
#include <initializer_list>
#include <iterator>
#include <optional>

namespace wordring::whatwg::encoding
{
	// ----------------------------------------------------------------------------------------
	// 3. Terminology
	//
	// https://encoding.spec.whatwg.org/#terminology
	// https://triple-underscore.github.io/Encoding-ja.html#terminology
	// ----------------------------------------------------------------------------------------

	template <typename InputIteratorT>
	class stream
	{
	public:
		using iterator_type = InputIteratorT;
		using value_type = typename std::iterator_traits<iterator_type>::value_type;
		using result_type = std::optional<value_type>;

	public:
		stream(iterator_type first, iterator_type last) : m_first(first), m_last(last) {}

		bool operator !() const { return m_front.empty() && m_first == m_last; }

		operator bool() const { return !(operator!()); }

		result_type read()
		{
			if (!m_front.empty())
			{
				value_type result{ m_front.front() };
				m_front.pop_front();
				return result;
			}

			if (m_first != m_last) return *m_first++;
			return result_type{};
		}

		void prepend(value_type token) { m_front.push_front(token); }

		template <typename InputIterator>
		void prepend(InputIterator first, InputIterator last) { m_front.insert(m_front.begin(), first, last); }

		template <typename Value>
		void prepend(std::initializer_list<Value> tokens) { prepend(tokens.begin(), tokens.end()); }

	private:
		std::deque<value_type> m_front;
		iterator_type m_first;
		iterator_type m_last;
	};

}
