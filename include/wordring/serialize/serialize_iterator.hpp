#pragma once

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <type_traits>

#include <wordring/serialize/serialize.hpp>

namespace wordring
{
	// ------------------------------------------------------------------------
	// serialize_iterator
	// ------------------------------------------------------------------------

	/*! @class serialize_iterator serialize_iterator.hpp wordring/serialize_iterator.hpp
	
	@brief 任意型の整数列に対するイテレータをバイトを返すイテレータへ変換する

	@tparam InputIterator
		整数列に対する入力イテレータ

	@image html serialize_iterator_concept.svg

	@par 例
	@code
		std::vector<std::uint32_t> v{ 0x1234567u, 0x89ABCDEFu };

		auto it1 = serialize_iterator(v.begin());
		auto it2 = serialize_iterator(v.end());

		std::cout << std::hex;
		while (it1 != it2) std::cout << static_cast<int>(*it1++) << std::endl;
	@endcode
		出力
	@code
		1
		23
		45
		67
		89
		ab
		cd
		ef
	@endcode

	@sa wordring::deserialize_iterator
	*/
	template <typename InputIterator>
	class serialize_iterator
	{
		template <typename InputIterator1>
		friend bool operator==(serialize_iterator<InputIterator1> const&, serialize_iterator<InputIterator1> const&);

		template <typename InputIterator1>
		friend bool operator!=(serialize_iterator<InputIterator1> const&, serialize_iterator<InputIterator1> const&);

	public:
		using iterator_type = InputIterator;
		using unsigned_type = std::make_unsigned_t<typename std::iterator_traits<iterator_type>::value_type>;

		using difference_type   = std::ptrdiff_t;
		using value_type        = std::uint8_t;
		using pointer           = value_type*;
		using reference         = value_type&;
		using iterator_category = std::input_iterator_tag;

		static std::uint32_t constexpr coefficient = sizeof(unsigned_type);

		static_assert(std::is_integral_v<typename std::iterator_traits<iterator_type>::value_type>);

	public:
		/*! @brief 空のイテレータを構築する
		*/
		serialize_iterator()
			: m_index(0)
		{
		}

		/*! @brief 元となるイテレータから直列化イテレータを構築する
		
		@param base 元となるイテレータ
		*/
		serialize_iterator(iterator_type base)
			: m_it(base)
			, m_index(0)
		{
		}

		/*! @brief 元となるイテレータを返す
		*/
		iterator_type base() const
		{
			return m_it;
		}

		value_type operator*() const
		{
			std::uint32_t n = (coefficient - 1) - (m_index % coefficient);

			return (static_cast<unsigned_type>(*m_it) >> n * 8) & 0xFFu;
		}

		serialize_iterator& operator++()
		{
			++m_index;
			if (m_index % coefficient == 0) ++m_it;
			return *this;
		}

		serialize_iterator operator++(int)
		{
			auto result = *this;
			operator++();
			return result;
		}

	private:
		iterator_type m_it;
		std::uint32_t m_index;
	};

	template <typename InputIterator1>
	inline bool operator==(serialize_iterator<InputIterator1> const& lhs, serialize_iterator<InputIterator1> const& rhs)
	{
		return !(lhs != rhs);
	}

	template <typename InputIterator1>
	inline bool operator!=(serialize_iterator<InputIterator1> const& lhs, serialize_iterator<InputIterator1> const& rhs)
	{
		std::uint32_t constexpr coefficient = serialize_iterator<InputIterator1>::coefficient;
		return lhs.m_it != rhs.m_it || lhs.m_index % coefficient != rhs.m_index % coefficient;
	}

	// ------------------------------------------------------------------------
	// deserialize_iterator
	// ------------------------------------------------------------------------

	/*! @class deserialize_iterator serialize_iterator.hpp wordring/serialize_iterator.hpp
	
	@brief バイト列に対するイテレータを任意型の整数を返すイテレータへ変換する

	@tparam Value         返す値の型
	@tparam InputIterator 整数列に対する入力イテレータ

	@image html deserialize_iterator_concept.svg

	@par 例
	@code
		std::string s{ '\x1', '\x23', '\x45', '\x67', '\x89', '\xAB','\xCD', '\xEF' };

		auto it1 = deserialize_iterator<std::uint32_t, decltype(s.begin())>(s.begin());
		auto it2 = deserialize_iterator<std::uint32_t, decltype(s.begin())>(s.end());

		std::cout << std::hex;
		while (it1 != it2) std::cout << *it1++ << std::endl;
	@endcode
		出力
	@code
		1234567
		89abcdef
	@endcode

	@sa wordring::serialize_iterator
	*/
	template <typename Value, typename ForwardIterator>
	class deserialize_iterator
	{
		template <typename Value1, typename ForwardIterator1>
		friend bool operator==(deserialize_iterator<Value1, ForwardIterator1> const&, deserialize_iterator<Value1, ForwardIterator1> const&);

		template <typename Value1, typename ForwardIterator1>
		friend bool operator!=(deserialize_iterator<Value1, ForwardIterator1> const&, deserialize_iterator<Value1, ForwardIterator1> const&);

	public:
		using iterator_type     = ForwardIterator;
		using unsigned_type     = std::make_unsigned_t<Value>;

		using difference_type   = std::ptrdiff_t;
		using value_type        = Value;
		using pointer           = value_type*;
		using reference         = value_type&;
		using iterator_category = std::input_iterator_tag;

		static std::uint32_t constexpr coefficient = sizeof(unsigned_type);

	public:
		deserialize_iterator()
			: m_it()
		{
		}

		deserialize_iterator(iterator_type it)
			: m_it(it)
		{
			using category = typename std::iterator_traits<iterator_type>::iterator_category;

			static_assert(
				   std::is_same_v<category, std::forward_iterator_tag>
				|| std::is_same_v<category, std::bidirectional_iterator_tag>
				|| std::is_same_v<category, std::random_access_iterator_tag>);
		}

		value_type operator*() const
		{
			unsigned_type v = 0;

			iterator_type it = m_it;
			for (std::uint32_t i = 0; i < coefficient; ++i) v = (v << 8) + static_cast<std::uint8_t>(*it++);

			return v;
		}

		deserialize_iterator& operator++()
		{
			std::advance(m_it, coefficient);
			return *this;
		}

		deserialize_iterator operator++(int)
		{
			auto result = *this;
			operator++();
			return result;
		}

	protected:
		iterator_type m_it;
	};

	template <typename Value1, typename ForwardIterator1>
	inline bool operator==(deserialize_iterator<Value1, ForwardIterator1> const& lhs, deserialize_iterator<Value1, ForwardIterator1> const& rhs)
	{
		return lhs.m_it == rhs.m_it;
	}

	template <typename Value1, typename ForwardIterator1>
	inline bool operator!=(deserialize_iterator<Value1, ForwardIterator1> const& lhs, deserialize_iterator<Value1, ForwardIterator1> const& rhs)
	{
		return lhs.m_it != rhs.m_it;
	}
}
