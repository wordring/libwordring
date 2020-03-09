#pragma once

#include <wordring/trie/trie.hpp>

#include <type_traits>

namespace wordring
{
	template <typename String>
	class basic_atom : public stable_trie<typename String::value_type>::const_iterator
	{
		template <typename String1>
		friend class basic_atom_set;

	protected:
		using base_type = typename stable_trie<typename String::value_type>::const_iterator;

	public:
		using base_type::string;

	protected:
		using base_type::m_c;
		using base_type::m_index;

	protected:
		basic_atom(base_type it)
			: base_type(it)
		{
		}

	public:
		basic_atom()
			: base_type()
		{
		}

		std::uint32_t index() const { return static_cast<std::uint32_t>(base_type::m_index); }
	};

	template <typename String>
	class basic_atom_set : public stable_trie<typename String::value_type>
	{
	protected:
		using base_type = stable_trie<typename String::value_type>;

	public:
		using atom_type = basic_atom<String>;

	protected:
		using base_type::null_value;

	public:
		template <typename InputIterator, typename std::enable_if_t<std::negation_v<std::is_integral<typename std::iterator_traits<InputIterator>::value_type>>, std::nullptr_t> = nullptr>
		void assign(InputIterator first, InputIterator last)
		{
			while (first != last)
			{
				atom_type it = base_type::insert(*first++);
				base_type::at(static_cast<typename atom_type::base_type>(it)) = it.index();
			}
		}

		template <typename InputIterator>
		atom_type insert(InputIterator first, InputIterator last)
		{
			auto it = base_type::insert(first, last);

			//base_type::at(it) = idx;

			return it;
		}
	};

	
}
