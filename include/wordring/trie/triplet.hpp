#pragma once

#include <bitset>
#include <cassert>
#include <deque>
#include <iterator>
#include <string_view>
#include <tuple>
#include <utility>

#include <wordring/serialize/serialize_iterator.hpp>

namespace wordring
{
	template <typename Node>
	class trie_iterator_base
	{
	public:
		using difference_type   = std::ptrdiff_t;
		using value_type        = Node;
		using pointer           = value_type*;
		using reference         = value_type&;
		using iterator_category = typename std::input_iterator_tag;

	public:
	};

	template <typename Node>
	class triplet_trie_node_iterator
	{
	public:
		using node_type = Node;

		using difference_type = std::ptrdiff_t;
		using value_type = uint8_t;
		//using pointer = value_type*;
		//using reference = value_type&;
		using iterator_category = typename std::input_iterator_tag;

	};

	class triplet_trie
	{
	public:
		class node_base
		{
		public:
			node_base() noexcept
				: m_first(0)
				, m_last(0)
				, m_position(-1) {}

			node_base(uint32_t first_, uint32_t last_, int32_t position_) noexcept
				: m_first(first_)
				, m_last(last_)
				, m_position(position_) {}

			node_base(node_base const& rhs) = default;

			/*! ノードが含む文字列の数を返す
			このノードで終わる文字列も含むので、子の数と一致しない場合がある。
			*/
			uint32_t count() const noexcept { return m_last - m_first; }

			uint32_t first() const noexcept { return m_first; }

			void first(uint32_t first_) noexcept { m_first = first_; }

			uint32_t last() const noexcept { return m_last; }

			void last(uint32_t last_) noexcept { m_last = last_; }

			int32_t position() const noexcept { return m_position; }

			void position(int32_t position_) noexcept { m_position = position_; }

		private:
			uint32_t m_first;    // m_listにおけるbegin()相当の位置。
			uint32_t m_last;     // m_listにおけるend()相当の位置。
			int32_t  m_position; // 0から始まる文字列中の位置。（根は-1）。
		};

		using node_list = std::deque<node_base>;
		using bitset = std::bitset<256>;

		class node : public node_base
		{
		public:
		public:
			node() noexcept
				: m_triplet(nullptr)
			{
			}

			node(triplet_trie const& triplet, node_base const& base) noexcept
				: m_triplet(&triplet)
				, node_base(base) {}

			node(triplet_trie const& triplet, uint32_t first, uint32_t last, int32_t position) noexcept
				: m_triplet(&triplet)
				, node_base(first, last, position) {}

			/*! ノードが含む子の数を返す
			*/
			uint32_t size() const noexcept { return last() - first() - (is_terminal() ? 1 : 0); }


			/*! 子のノード集合を返す
			*/
			node_list children()
			{
				node_list result{};

				auto it1 = std::next(m_triplet->begin(), first());
				auto it2 = std::next(m_triplet->begin(), last());
				int32_t pos = position() + 1;
				assert(it1 != it2);

				uint16_t ch1{}, ch2{};

				uint32_t state{ 0 };
				for (uint32_t i = first(); it1 != it2; ++it1, ++i)
				{
					if (it1->size() <= static_cast<std::size_t>(pos)) continue;
					switch (state)
					{
					case 0: // 開始状態
					Start:
						ch1 = it1->at(pos);
						result.push_back(node_base(i, i + 1, pos));
						state = 1;
						break;
					case 1: // 継続状態
						ch2 = it1->at(pos);
						if (ch1 != ch2) goto Start;
						result.back().last(i + 1);
					}
				}

				return result;
			}

			/*! ノードが文字列の終端に到達しているときtrueを返す

			- 文字列のリストがソート済みであることに依存している。
			- ノードが文字列の終端に到達していたとしても、共通接頭辞を持つ他のノードを包含している可能性がある。
			- ノードのsize()が1を返し、この関数がtrueを返す場合は、他のノードを包含していない。
			*/
			bool is_terminal() const
			{
				assert(position() != -1);
				assert(count() != 0);
				auto const& string = *(std::next(m_triplet->begin(), first()));
				return (string.size() == position() + 1) ? true : false;
			}

			/*! このノードから移動できる記号の集合をビットセットで返す。
			*/
			bitset indexes() const
			{
				bitset result{};

				auto it1 = std::next(m_triplet->begin(), first());
				auto it2 = std::next(m_triplet->begin(), last());
				int32_t pos = position() + 1;
				assert(it1 != it2);

				for (; it1 != it2; ++it1)
				{
					if (it1->size() <= static_cast<std::size_t>(pos)) continue;
					result.set(it1->at(pos));
				}

				return result;
			}

			/*! このノードへ移動するための記号を返す。
			*/
			uint32_t symbol() const
			{
				assert(position() != -1);
				assert(count() != 0);
				return std::next(m_triplet->begin(), first())->at(position());
			}

		private:
			triplet_trie const* m_triplet;
		};

	public:
		using base_type = std::deque<std::deque<std::uint8_t>>;

		using reference       = typename base_type::reference;
		using const_reference = typename base_type::const_reference;
		using iterator        = typename base_type::iterator;
		using const_iterator  = typename base_type::const_iterator;
		using size_type       = typename base_type::size_type;
		using difference_type = typename base_type::difference_type;
		using value_type      = typename base_type::value_type;
		using allocator_type  = typename base_type::allocator_type;
		using pointer         = typename base_type::pointer;
		using const_pointer   = typename base_type::const_pointer;

	public:
		triplet_trie() = default;

		triplet_trie(triplet_trie const& rhs)
			: m_list(rhs.m_list)
		{
		}

		template <typename Iterator>
		triplet_trie(Iterator first,  Iterator last)
		{
			insert(end(), first, last);
		}

		base_type const& base() const noexcept { return m_list; }

		iterator begin() noexcept { return m_list.begin(); }

		const_iterator begin() const noexcept { return m_list.begin(); }

		iterator end() noexcept { return m_list.end(); }

		const_iterator end() const noexcept { return m_list.end(); }

		template <typename String, typename std::enable_if_t<std::is_class_v<String>, std::nullptr_t> = nullptr>
		iterator insert(const_iterator position, String const& string)
		{
			return m_list.emplace(position, serialize_iterator(string.begin()), serialize_iterator(string.end()));
		}

		template <typename String, typename std::enable_if_t<std::is_pointer_v<String>, std::nullptr_t> = nullptr>
		iterator insert(const_iterator position, String string)
		{
			std::basic_string_view<std::remove_pointer_t<String>> s{ string };
			return m_list.emplace(position, serialize_iterator(s.begin()), serialize_iterator(s.end()));
		}

		template <typename Iterator>
		iterator insert(const_iterator position, Iterator first, Iterator last)
		{
			auto i = std::distance(m_list.cbegin(), position);
			auto j = i;
			while(first != last) insert(std::next(m_list.cbegin(), i++), *first++);
			return std::next(m_list.begin(), i);
		}

		template <typename String>
		void push_back(String string) { insert(end(), string); }

		node root() const { return node{ *this, 0, m_list.size(), -1 }; }

	private:
		base_type m_list;
	};
}
