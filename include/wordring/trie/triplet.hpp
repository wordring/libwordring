#pragma once

#include <bitset>
#include <cassert>
#include <deque>
#include <iterator>
#include <tuple>
#include <utility>

#include <iostream>

namespace wordring
{
	struct triplet_trie_node
	{
		uint32_t first;   // m_listにおけるbegin()相当の位置。
		uint32_t last;    // m_listにおけるend()相当の位置。
		int16_t position; // 0から始まる文字列中の位置。（根は-1）。

		uint32_t size() const { return last - first; }
	};

	template <typename List>
	class triplet_trie
	{
	public:
		using list_type = List;
		using list_iterator = typename list_type::const_iterator;
		using string_type = typename list_type::value_type;

		struct node
		{
			uint32_t first : 24; // m_listにおけるbegin()相当の位置。
			uint32_t last : 24; // m_listにおけるend()相当の位置。
			int16_t position; // 0から始まる文字列中の位置。（根は-1）。

			uint32_t size() const { return last - first; }
		};

		using node_list = std::deque<node>;
		using bitset = std::bitset<256>;

	public:
		triplet_trie(list_type const& list) : m_list{ list } {}

		node_list children(node const & parent) const
		{
			node_list result{};

			auto first = std::next(m_list.begin(), parent.first);
			auto last = std::next(m_list.begin(), parent.last);
			int16_t position = parent.position + 1;
			assert(first != last);

			uint16_t ch1{}, ch2{};

			uint32_t state{ 0 };
			for (uint32_t i = parent.first; first != last; ++first, ++i)
			{
				if (first->size() <= static_cast<std::size_t>(position)) continue;
				switch (state)
				{
				case 0: // 開始状態
				Start:
					ch1 = first->at(position);
					result.push_back(node{ i, i + 1, position });
					state = 1;
					break;
				case 1: // 継続状態
					ch2 = first->at(position);
					if (ch1 != ch2) goto Start;
					result.back().last = i + 1;
				}
			}
			return result;
		}

		bool is_accepted_state(node const& entry) const
		{
			assert(entry.position != -1);
			assert(entry.size() != 0);
			auto const& string = m_list.at(entry.first);
			return (string.size() == entry.position + 1) ? true : false;
		}

		bitset indexes(node_list const& entries) const
		{
			bitset result{};
			for (auto const& entry : entries) result.set(index(entry));
			return result;
		}

		node root() const { return node{ 0, m_list.size(), -1 }; }

		uint16_t to_symbol(node const& entry) const
		{
			assert(entry.position != -1);
			assert(entry.size() != 0);
			auto const& string = m_list.at(entry.first);
			return string.at(entry.position);
		}

	private:
		list_type const & m_list;
	};
}
