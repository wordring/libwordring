#pragma once

#include <bitset>
#include <cassert>
#include <deque>
#include <iterator>
#include <optional>
#include <tuple>
#include <utility>

#include <wordring/serialize/serialize_iterator.hpp>

namespace wordring
{
	class triplet_trie
	{
	public:
		class node;

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
		/*! 指定された深さの記号を返すイテレータ
		*/
		class symbol_iterator
		{
			friend bool operator==(symbol_iterator const&, symbol_iterator const&);
			friend bool operator!=(symbol_iterator const&, symbol_iterator const&);
			friend std::ptrdiff_t operator-(symbol_iterator const&, symbol_iterator const&);

		public:
			using difference_type   = std::ptrdiff_t;
			using value_type        = std::optional<uint32_t>;
			using pointer           = void;
			using reference         = void;
			using iterator_category = std::input_iterator_tag;

		public:
			symbol_iterator() : m_depth(-1) {}

			symbol_iterator(base_type::const_iterator const & current, int32_t depth)
				: m_current(current)
				, m_depth(depth) {}

			symbol_iterator(symbol_iterator const& rhs) = default;

			value_type operator*() const
			{
				assert(m_depth != -1);
				return (m_depth < static_cast<int32_t>(m_current->size())) ? value_type(m_current->at(m_depth)) : value_type();
			}

			symbol_iterator& operator++() noexcept
			{
				++m_current;
				return *this;
			}

			symbol_iterator operator++(int) noexcept
			{
				symbol_iterator result(*this);
				operator++();
				return result;
			}

		private:
			typename base_type::const_iterator m_current;
			int32_t m_depth;
		};

		/*! 子ノードを巡回するイテレータ
		*/
		class node_iterator
		{
			friend bool operator==(node_iterator const&, node_iterator const&);
			friend bool operator!=(node_iterator const&, node_iterator const&);
			friend std::ptrdiff_t operator-(node_iterator const&, node_iterator const&);

		public:
			using difference_type   = std::ptrdiff_t;
			using value_type        = node;
			using pointer           = void;
			using reference         = void;
			using iterator_category = std::input_iterator_tag;

		public:
			node_iterator() : m_depth(-1) {}

			/*! 有効な値を指すイテレータを作る
			*/
			node_iterator(base_type::const_iterator basis, uint32_t first, uint32_t last, int32_t depth)
				: m_basis(basis)
				, m_it1(std::next(basis, first))
				, m_it2(std::next(basis, first))
				, m_last(std::next(basis, last))
				, m_depth(depth)
			{
				assert(first != last); // 終端を指すイテレータはこのコンストラクタで作れない。
				if (!(*symbol_iterator(m_it1, m_depth)).has_value()) m_it2 = ++m_it1;
				operator++();
			}

			/*! 終端を指すイテレータを作る
			*/
			node_iterator(base_type::const_iterator basis, uint32_t last, int32_t depth)
				: m_basis(basis)
				, m_it1(std::next(basis, last))
				, m_it2(std::next(basis, last))
				, m_last(std::next(basis, last))
				, m_depth(depth) {}

			node_iterator(node_iterator const&) = default;

			value_type operator*() const
			{
				return value_type(m_basis, std::distance(m_basis, m_it1), std::distance(m_basis, m_it2), m_depth);
			}

			node_iterator& operator++()
			{
				m_it1 = m_it2;
				if (m_it1 != m_last) // 末尾ノードの次は、終端。
				{
					symbol_iterator s1(m_it1, m_depth), s2, s3(m_last, m_depth);
					auto ch1 = *s1;
					s2 = std::find_if_not(s1, s3, [=](auto ch2) { return ch1 == ch2; });
					m_it2 = std::next(m_it1, s2 - s1);
				}
				return *this;
			}

			node_iterator operator++(int)
			{
				node_iterator result(*this);
				operator++();
				return result;
			}

		private:
			base_type::const_iterator m_basis; // m_list.begin()
			base_type::const_iterator m_it1;   // 現在の開始位置。
			base_type::const_iterator m_it2;   // 現在の終端位置。
			base_type::const_iterator m_last;  // 終端。

			int32_t  m_depth;                  // 0から始まる文字列中の位置。（根は-1）。
		};

		class node_base
		{
		public:
			node_base() noexcept
				: m_first(0)
				, m_last(0)
				, m_depth(-1) {}

			node_base(uint32_t begin, uint32_t end, int32_t i) noexcept
				: m_first(begin)
				, m_last(end)
				, m_depth(i) {}

			node_base(node_base const&) = default;

			/*! ノードが含む文字列の数を返す
			このノードで終わる文字列も含むので、子の数と一致しない場合がある。
			*/
			uint32_t count() const noexcept { return m_last - m_first; }

			uint32_t first() const noexcept { return m_first; }

			void first(uint32_t i) noexcept { m_first = i; }

			uint32_t last() const noexcept { return m_last; }

			void last(uint32_t i) noexcept { m_last = i; }

			int32_t depth() const noexcept { return m_depth; }

			void depth(int32_t i) noexcept { m_depth = i; }

		private:
			uint32_t m_first; // m_list.begin()からノード開始位置までの距離。
			uint32_t m_last;  // m_list.begin()からノード終端位置までの距離。
			int32_t  m_depth; // 0から始まる文字列中の位置。（根は-1）。
		};

		using node_list = std::deque<node_base>;
		using bitset = std::bitset<256>;

		class node : public node_base
		{
		public:
			using const_iterator = node_iterator;

		public:
			node() = default;

			node(base_type::const_iterator const & basis, node_base const& base) noexcept
				: m_basis(basis)
				, node_base(base) {}

			node(base_type::const_iterator const& basis, uint32_t first, uint32_t last, int32_t position) noexcept
				: m_basis(basis)
				, node_base(first, last, position) {}

			node(node const&) = default;
			
			const_iterator begin() const { return const_iterator(m_basis, first(), last(), depth() + 1); }

			const_iterator end() const { return const_iterator(m_basis, last(), depth() + 1); }
			
			node at(uint32_t n) const { return *std::next(begin(), n); }

			/*! ノードが含む子の数を返す
			*/
			uint32_t size() const
			{
				uint32_t result = 0;
				auto it1 = begin(), it2 = end();
				for (; it1 != it2; ++result) ++it1;
				return result;
			/*
				uint32_t result = 0;

				auto it1 = std::next(m_basis, first());
				auto it2 = std::next(m_basis, last());
				int32_t pos = depth() + 1;
				assert(it1 != it2);

				uint16_t ch1{}, ch2{};
				uint32_t state{ 0 };
				for (; it1 != it2; ++it1)
				{
					if (it1->size() <= static_cast<std::size_t>(pos)) continue;
					switch (state)
					{
					case 0: // 開始状態
					Start:
						ch1 = it1->at(pos);
						++result;
						state = 1;
						break;
					case 1: // 継続状態
						ch2 = it1->at(pos);
						if (ch1 != ch2) goto Start;
					}
				}

				return result;
				*/
			}


			/*! 子のノード集合を返す
			*/
			node_list children() const
			{
				node_list result{};

				auto it1 = std::next(m_basis, first());
				auto it2 = std::next(m_basis, last());
				int32_t pos = depth() + 1;
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
				//assert(position() != -1);
				assert(count() != 0);
				auto const& string = *(std::next(m_basis, first()));
				return (string.size() == depth() + 1) ? true : false;
			}

			/*! このノードから移動できる記号の集合をビットセットで返す。
			*/
			bitset indexes() const
			{
				bitset result{};

				auto it1 = std::next(m_basis, first());
				auto it2 = std::next(m_basis, last());
				int32_t pos = depth() + 1;
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
				assert(depth() != -1); // 根は記号を持たない。
				assert(count() != 0);
				return std::next(m_basis, first())->at(depth());
			}

		private:
			typename base_type::const_iterator m_basis;
		};

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

		node root() const { return node(m_list.cbegin(), 0, static_cast<uint32_t>(m_list.size()), -1 ); }

	private:
		base_type m_list;
	};

	inline bool operator==(triplet_trie::symbol_iterator const& lhs, triplet_trie::symbol_iterator const& rhs)
	{
		return lhs.m_current == rhs.m_current && lhs.m_depth == rhs.m_depth;
	}

	inline bool operator!=(triplet_trie::symbol_iterator const& lhs, triplet_trie::symbol_iterator const& rhs)
	{
		return lhs.m_current != rhs.m_current || lhs.m_depth != rhs.m_depth;
	}

	inline typename triplet_trie::symbol_iterator::difference_type
		operator-(triplet_trie::symbol_iterator const& lhs, triplet_trie::symbol_iterator const& rhs)
	{
		assert(lhs.m_depth == rhs.m_depth);
		return lhs.m_current - rhs.m_current;
	}

	// triplet_trie::node_iterator ------------------------------------------- 

	inline bool operator==(triplet_trie::node_iterator const& lhs, triplet_trie::node_iterator const& rhs)
	{
		return lhs.m_it1 == rhs.m_it2 && lhs.m_depth == rhs.m_depth;
	}

	inline bool operator!=(triplet_trie::node_iterator const& lhs, triplet_trie::node_iterator const& rhs)
	{
		return lhs.m_it1 != rhs.m_it2 || lhs.m_depth != rhs.m_depth;
	}

	inline std::ptrdiff_t operator-(triplet_trie::node_iterator const& lhs, triplet_trie::node_iterator const& rhs)
	{
		assert(lhs.m_depth == rhs.m_depth);
		return lhs.m_it1 - rhs.m_it2;
	}
}
