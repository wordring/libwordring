#pragma once

#include <array>
#include <iterator>
#include <type_traits>

namespace wordring
{
	template <typename Label, typename Base>
	class const_trie_iterator : public Base
	{
		template <typename Label1, typename Base1>
		friend class basic_trie;

		template <typename Label1, typename Base1>
		friend bool operator==(const_trie_iterator<Label1, Base1> const&, const_trie_iterator<Label1, Base1> const&);

		template <typename Label1, typename Base1>
		friend bool operator!=(const_trie_iterator<Label1, Base1> const&, const_trie_iterator<Label1, Base1> const&);

	protected:
		using base_type = Base;

		using typename base_type::index_type;
		using typename base_type::node_type;
		using typename base_type::container;

		using unsigned_type = std::make_unsigned_t<Label>;

	public:
		using difference_type   = std::ptrdiff_t;
		using value_type        = Label;
		using pointer           = value_type*;
		using reference         = value_type&;
		using iterator_category = std::input_iterator_tag;

		static constexpr std::uint16_t null_value = 256u;
		static std::uint32_t constexpr coefficient = sizeof(value_type) / sizeof(typename base_type::value_type);

		static_assert(1 <= coefficient);

	public:
		using base_type::operator bool;
		using base_type::operator!;

	protected:
		using base_type::value;
		using base_type::limit;
		using base_type::find;

		using base_type::m_c;
		using base_type::m_index;

	public:
		const_trie_iterator()
			: base_type()
		{
		}

	protected:
		const_trie_iterator(container& c, index_type index)
			: base_type(c, index)
		{
		}

		const_trie_iterator(base_type const& it)
			: base_type(it)
		{
		}

	public:
		value_type operator*() const
		{
			assert(1 < m_index && m_index < limit());

			value_type result = 0;

			if constexpr (coefficient == 1) return base_type::value();
			else
			{
				node_type const* d = m_c->data();
				index_type idx = m_index;

				for (std::uint32_t i = 0; i < coefficient; ++i)
				{
					index_type parent = (d + idx)->m_check;
					assert(1 <= parent && parent < limit());

					index_type base = (d + parent)->m_base;
					assert(1 <= base && base < limit());

					result += idx - base << i * 8;

					idx = parent;
				}
			}

			return result;
		}

		const_trie_iterator operator[](value_type label) const
		{
			index_type idx = 0;

			if constexpr (coefficient == 1) idx = base_type::at_index(label);
			else
			{
				index_type parent = m_index;
				assert(1 <= parent && parent < base_type::limit());

				node_type const* d = m_c->data();
				for (std::uint32_t i = 0; i < coefficient; ++i)
				{
					std::uint8_t ch = static_cast<unsigned_type>(label) >> (coefficient - i - 1) * 8 & 0xFFu;
					index_type base = (d + parent)->m_base;
					idx = base + ch;

					if ((d + idx)->m_check != parent)
					{
						idx = 0;
						break;
					}

					parent = idx;
				}
			}

			return const_trie_iterator(*m_c, idx);
		}

		const_trie_iterator& operator++()
		{
			if constexpr (coefficient == 1) base_type::advance();
			else
			{
				index_type    idx = 0;
				std::uint32_t lv = 0;

				node_type const* d = m_c->data();

				// 右、あるいは右上を探す
				for (index_type i = m_index; lv < coefficient; ++lv)
				{
					index_type parent = (d + i)->m_check;
					index_type base = (d + parent)->m_base;
					// 右兄弟を探す
					i = find(i + 1, base + null_value, parent);
					if (i != 0)
					{
						idx = i;
						break;
					}
					i = parent;
				}

				// 足の長さをそろえる
				if (idx != 0)
				{
					for (; 0 < lv; --lv)
					{
						index_type parent = idx;
						index_type base = (d + idx)->m_base;
						idx = find(base, base + null_value, parent);
						assert(idx != 0);
					}
				}

				m_index = idx;
			}

			return *this;
		}

		const_trie_iterator operator++(int)
		{
			auto result = *this;
			operator++();
			return result;
		}

		template <typename String>
		void string(String& result) const
		{
			result.clear();
			for (auto p = *this; 1 < p.m_index; p = p.parent()) result.push_back(*p);
			std::reverse(std::begin(result), std::end(result));
		}

		const_trie_iterator parent() const
		{
			index_type idx = m_index;

			if constexpr (coefficient == 1) idx = base_type::parent_index();
			else
			{
				for (std::uint32_t lv = 0; lv < coefficient; ++lv)
				{
					idx = (m_c->data() + idx)->m_check;
					assert(1 <= idx && idx < limit());
				}
			}

			return const_trie_iterator(*m_c, idx);
		}

		const_trie_iterator begin() const
		{
			index_type idx = m_index;

			if constexpr (coefficient == 1) idx = base_type::begin_index();
			else
			{
				std::uint32_t lv = 0;

				trie_node const* d = m_c->data();
				while (lv < coefficient && idx != 0)
				{
					index_type base = (d + idx)->m_base;
					idx = (1 <= base)
						? find(base, base + null_value, idx)
						: 0;
					++lv;
				}

				idx = (lv == coefficient)
					? idx
					: 0;
			}

			return const_trie_iterator(*m_c, idx);
		}

		const_trie_iterator end() const
		{
			return const_trie_iterator(*m_c, 0);
		}
	};

	template <typename Label1, typename Base1>
	inline bool operator==(const_trie_iterator<Label1, Base1> const& lhs, const_trie_iterator<Label1, Base1> const& rhs)
	{
		assert(lhs.m_c == rhs.m_c);
		return lhs.m_index == rhs.m_index;
	}

	template <typename Label1, typename Base1>
	inline bool operator!=(const_trie_iterator<Label1, Base1> const& lhs, const_trie_iterator<Label1, Base1> const& rhs)
	{
		return !(lhs == rhs);
	}
}
