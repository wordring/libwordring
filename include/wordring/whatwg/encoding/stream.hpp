#pragma once

#include <deque>
#include <iterator>

namespace wordring
{
namespace whatwg
{

	template <typename Iterator>
	class stream
	{
	public:
		using iterator_type = Iterator;
		using value_type = typename std::iterator<iterator_type>::value_type;

	private:
		std::deque<value_type> m_front;
		std::deque<value_type> m_back;
		iterator_type m_iterator;
	};

}
}
