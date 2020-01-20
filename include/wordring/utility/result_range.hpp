#pragma once

namespace wordring
{
	template <typename Iterator>
	class result_range
	{
	public:
		using iterator = Iterator;

	public:
		result_range(iterator first, iterator last)
			: m_begin(first)
			, m_end(last)
		{
		}

		iterator begin() const { return m_begin; }

		iterator end() const { return m_end; }

	protected:
		iterator m_begin;
		iterator m_end;
	};

}
