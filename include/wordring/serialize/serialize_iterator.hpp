#pragma once

#include <cstddef>
#include <iterator>

#include <wordring/serialize/serialize.hpp>

namespace wordring
{
	template <typename InputIterator>
	class serialize_iterator
	{
	public:
		using base_iterator   = InputIterator;
		using base_value_type = typename std::iterator_traits<base_iterator>::value_type;

		using difference_type   = std::ptrdiff_t;
		using value_type        = uint8_t;
		using pointer           = value_type*;
		using reference         = value_type&;
		using iterator_category = typename std::input_iterator_tag;

	public:
		serialize_iterator()
			: m_position{ 0 }
		{
		}

		serialize_iterator(base_iterator base)
			: m_base{ base }
			, m_position{ 0 }
		{
		}

		serialize_iterator(serialize_iterator const& rhs)
			: m_base{rhs.m_base }
			, m_position{ rhs.m_position }
		{
		}

		~serialize_iterator() noexcept
		{
		}

		serialize_iterator& operator=(serialize_iterator const& rhs)
		{
			m_base = rhs.m_base;
			m_position = rhs.m_position;
			return *this
		}

		value_type operator*() const
		{
			return serialize(*m_base, m_position);
		}

		serialize_iterator<base_iterator>& operator++()
		{
			uint32_t constexpr l = sizeof(base_value_type);
			if (m_position < l - 1) ++m_position;
			else
			{
				m_position = 0;
				++m_base;
			}
			return *this;
		}

		serialize_iterator<base_iterator> operator++(int)
		{
			serialize_iterator<base_iterator> result{ *this };
			++(*this);
			return result;
		}

		bool equal(serialize_iterator<base_iterator> const& rhs) const
		{
			return m_base == rhs.m_base && m_position == rhs.m_position;
		}

	private:
		base_iterator m_base;
		uint32_t m_position;
	};

	template <typename InputIterator>
	inline bool operator==(serialize_iterator<InputIterator> const& lhs, serialize_iterator<InputIterator> const& rhs)
	{
		return lhs.equal(rhs);
	}

	template <typename InputIterator>
	inline bool operator!=(serialize_iterator<InputIterator> const& lhs, serialize_iterator<InputIterator> const& rhs)
	{
		return !lhs.equal(rhs);
	}
}
