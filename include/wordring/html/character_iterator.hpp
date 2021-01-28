#pragma once

#include <wordring/html/html_defs.hpp>

#include <cstdint>
#include <iterator>
#include <type_traits>

namespace wordring::html
{

	template <typename Iterator, typename Character = std::nullptr_t>
	class character_iterator
	{
		template <typename Iterator1>
		friend bool operator==(character_iterator<Iterator1> const&, character_iterator<Iterator1> const&);

		template <typename Iterator1>
		friend bool operator!=(character_iterator<Iterator1> const&, character_iterator<Iterator1> const&);

		template <typename T, bool>
		struct node_pointer_{ using type = int; };

		template <typename T>
		struct node_pointer_<T, false> { using type = T; };

		template <typename T>
		struct node_pointer_<T, true> { using type = typename T::base_type; };

		template <typename T>
		class has_base
		{
		private:
			template <typename U>
			static auto test(U u)->decltype(u.base(), std::true_type());
			static auto test(...)->decltype(std::false_type());

		public:
			static bool constexpr value = decltype(test(std::declval<T>()))::value;

			using type = typename node_pointer_<T, value>::type;
		};

	public:
		using base_type    = Iterator;
		using node_pointer = typename has_base<base_type>::type;
		using node_traits  = wordring::html::node_traits<node_pointer>;
		using string_type = typename node_traits::string_type;// std::conditional_t<std::is_const_v<typename node_traits::node_type>, typename node_traits::string_type const, typename node_traits::string_type>;

		using value_type        = typename string_type::value_type;
		using difference_type   = typename std::iterator_traits<node_pointer>::difference_type;
		using reference         = value_type&;
		using pointer           = value_type*;
		using iterator_category = std::conditional_t<
			std::is_base_of_v<std::bidirectional_iterator_tag, typename base_type::iterator_category>,
				std::bidirectional_iterator_tag, std::forward_iterator_tag>;

	public:
		character_iterator() = default;
		character_iterator(base_type first, base_type last)
			: m_first(first)
			, m_last(last)
			, m_i(0)
		{
			while (m_first != m_last && !node_traits::is_text(current())) ++m_first;
		}

		character_iterator end() const { return { m_last, m_last }; }

		/*! @brief 文字を返す
		*/
		reference operator*() const
		{
			return const_cast<reference>(node_traits::data(current())[m_i]);
		}

		/*! @brief イテレータを進める
		*
		* @return *this
		*/
		character_iterator& operator++()
		{
			if (node_traits::is_text(current()))
			{
				++m_i;
				if (m_i < node_traits::data(current()).size()) return *this;
				else
				{
					++m_first;
					m_i = 0;
				}
			}
			while (m_first != m_last && !node_traits::is_text(current())) ++m_first;

			return *this;
		}

		/*! @brief イテレータを進める
		*
		* @return 進める前のイテレータ
		*/
		character_iterator operator++(int)
		{
			character_iterator it(*this);
			operator++();
			return it;
		}

	private:
		node_pointer current() const
		{
			if constexpr (std::is_same_v<base_type, node_pointer>)
			{
				return m_first;
			}
			return m_first.base();
		}

	private:
		base_type m_first;
		base_type m_last;
		std::uint32_t m_i;
	};


	template <typename Iterator>
	inline bool operator==(character_iterator<Iterator> const& lhs, character_iterator<Iterator> const& rhs)
	{
		return lhs.m_first == rhs.m_first && lhs.m_i == rhs.m_i;
	}

	template <typename Iterator>
	inline bool operator!=(character_iterator<Iterator> const& lhs, character_iterator<Iterator> const& rhs)
	{
		return !(lhs == rhs);
	}
}
