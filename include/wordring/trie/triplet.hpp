#pragma once


#include <cassert>
#include <deque>
#include <iterator>
#include <tuple>
#include <utility>

#include <iostream>

namespace wordring
{
	template <typename List>
	class triplet_trie
	{
	public:
		using list_type = List;
		using list_iterator = typename list_type::const_iterator;
		using string_type = typename list_type::value_type;

		struct node
		{
			list_iterator first;
			list_iterator last;
			int16_t position;
		};

	public:
		triplet_trie(list_type const& list) : m_list{ list } {}

		node root() const { return node{ m_list.begin(), m_list.end(), -1 }; }

		std::deque<node> children(node const & parent) const
		{
			std::deque<node> result{};

			auto first = parent.first;
			auto last = parent.last;
			int16_t position = parent.position + 1;
			assert(first != last);

			uint16_t ch1{}, ch2{};

			uint32_t state{ 0 };
			for (; first != last; ++first)
			{
				if (first->size() <= static_cast<std::size_t>(position)) continue;
				switch (state)
				{
				case 0: // 開始状態
				Start:
					ch1 = first->at(position);
					result.push_back(node{ first, std::next(first, 1), position });
					state = 1;
					break;
				case 1: // 継続状態
					ch2 = first->at(position);
					if (ch1 != ch2) goto Start;
					result.back().last = std::next(first, 1);
				}
			}
			return result;
		}

	private:
		list_type const & m_list;
	};
}
