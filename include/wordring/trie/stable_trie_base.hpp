#pragma once

#include <wordring/trie/stable_trie_base_iterator.hpp>
#include <wordring/trie/trie_heap.hpp>

//#include <algorithm>
//#include <cstdint>
#include <utility>

namespace wordring
{
	// ------------------------------------------------------------------------
	// stable_trie_base_value_proxy
	// ------------------------------------------------------------------------

	struct stable_trie_base_value_proxy
	{
		using index_type = typename trie_node::index_type;

		index_type* m_base;

		stable_trie_base_value_proxy(index_type* base)
			: m_base(base)
		{
		}

		void operator=(index_type val)
		{
			if (val < 0) throw std::invalid_argument("");
			*m_base = -val;
		}

		operator index_type() const
		{
			assert(*m_base <= 0);
			return -*m_base;
		}
	};

	// ------------------------------------------------------------------------
	// stable_trie_base
	// ------------------------------------------------------------------------

	/*!
	- 挿入や削除によって終端のイテレータが無効とならない安定なTrie木。
	*/
	template <typename Key, typename Allocator = std::allocator<trie_node>>
	class stable_trie_base : public trie_heap<Allocator>
	{
		template <typename Key1, typename Allocator1>
		friend std::ostream& operator<<(std::ostream&, stable_trie_base<Key1, Allocator1> const&);

		template <typename Key1, typename Allocator1>
		friend std::istream& operator>>(std::istream&, stable_trie_base<Key1, Allocator1>&);

	protected:
		using base_type = trie_heap<Allocator>;

		using typename base_type::container;
		using typename base_type::label_vector;
		using typename base_type::index_type;
		using typename base_type::node_type;

		using base_type::null_value;

	public:
		using key_type        = Key;
		using symbol_type     = std::make_unsigned_t<typename std::iterator_traits<typename key_type::const_iterator>::value_type>;
		using value_type      = std::pair<key_type const, uint32_t>;
		using size_type       = typename container::size_type;
		using allocator_type  = Allocator;
		using reference       = stable_trie_base_value_proxy;
		using const_reference = stable_trie_base_value_proxy const;
		using const_iterator  = const_stable_trie_base_iterator<key_type, container const>;

	public:
		using typename base_type::serialize_iterator;

		using base_type::get_allocator;
		using base_type::ibegin;
		using base_type::iend;
		using base_type::clear;

	protected:
		using base_type::assign;
		using base_type::limit;
		//using base_type::reserve;
		//using base_type::allocate;
		//using base_type::relocate;
		using base_type::free;
		//using base_type::locate;
		//using base_type::is_free;
		//using base_type::is_tail;
		using base_type::has_child;
		using base_type::has_null;
		//using base_type::has_sibling;
		using base_type::at;
		using base_type::add;

		using base_type::m_c;

		static_assert(sizeof(symbol_type) == 1);

	public:
		stable_trie_base()
			: base_type()
		{
		}

		explicit stable_trie_base(allocator_type const& alloc)
			: base_type(alloc)
		{
		}

		/*! 直列化データからの復元
		*/
		template <typename InputIterator, typename std::enable_if_t<std::is_integral_v<typename std::iterator_traits<InputIterator>::value_type>, std::nullptr_t> = nullptr>
		stable_trie_base(InputIterator first, InputIterator last, allocator_type const& alloc = allocator_type())
			: base_type(alloc)
		{
			assign(first, last);
		}

		/*! 文字列リストからの構築
		*/
		template <typename ForwardIterator, typename std::enable_if_t<std::negation_v<std::is_integral<typename std::iterator_traits<ForwardIterator>::value_type>>, std::nullptr_t> = nullptr>
		stable_trie_base(ForwardIterator first, ForwardIterator last, allocator_type const& alloc = allocator_type())
			: base_type(alloc)
		{
			assign(first, last);
		}

		/*! 初期化子リストからの構築
		*/
		stable_trie_base(std::initializer_list<trie_node> il, allocator_type const& alloc = allocator_type())
			: base_type(il, alloc)
		{
		}

		/*! 直列化データからの復元
		*/
		template <typename InputIterator, typename std::enable_if_t<std::is_integral_v<typename std::iterator_traits<InputIterator>::value_type>, std::nullptr_t> = nullptr>
		void assign(InputIterator first, InputIterator last)
		{
			base_type::assign(first, last);
		}

		/*! 文字列リストからの構築
		*/
		template <typename ForwardIterator, typename std::enable_if_t<std::negation_v<std::is_integral<typename std::iterator_traits<ForwardIterator>::value_type>>, std::nullptr_t> = nullptr>
		void assign(ForwardIterator first, ForwardIterator last)
		{
			clear();
			insert(first, last);
		}

		// 要素アクセス --------------------------------------------------------

		reference at(key_type const& key)
		{
			auto it = find(key);
			if (it == cend()) throw std::out_of_range("");

			index_type idx = (m_c.data() + it.m_index)->m_base + null_value;
			assert(1 < idx && idx < limit());
			
			return reference(std::addressof((m_c.data() + idx)->m_base));
		}

		const_reference const at(key_type const& key) const
		{
			return const_cast<stable_trie_base*>(this)->at(key);
		}

		reference operator[](key_type const& key)
		{
			auto it = find(key);
			if (it == cend()) it = insert(key);

			index_type idx = (m_c.data() + it.m_index)->m_base + null_value;
			assert(1 < idx && idx < limit());

			return reference(std::addressof((m_c.data() + idx)->m_base));
		}

		// イテレータ ----------------------------------------------------------

		const_iterator begin() const noexcept { return const_iterator(m_c, 1); }

		const_iterator cbegin() const noexcept { return const_iterator(m_c, 1); }

		const_iterator end() const noexcept { return const_iterator(m_c, 0); }

		const_iterator cend() const noexcept { return const_iterator(m_c, 0); }

		// 容量 ---------------------------------------------------------------

		bool empty() const noexcept { return size() == 0; }

		size_type size() const noexcept { return m_c.front().m_base; }

		static constexpr size_type max_size() noexcept
		{
			return std::numeric_limits<std::int32_t>::max() / sizeof(node_type);
		}

		// 変更 ---------------------------------------------------------------

		const_iterator insert(value_type const& value)
		{
			auto it1 = std::begin(value.first);
			auto it2 = std::end(value.first);
			if (it1 == it2) return cend();

			index_type parent = 1;

			// 登録済み遷移をスキップする
			for (index_type idx = at(parent, *it1); idx != 0; idx = at(parent, *it1))
			{
				parent = idx;
				++it1;
				if (it1 == it2) break;
			}

			if (!has_null(parent) || it1 != it2)
			{
				// 新規文字列による遷移を追加
				while (it1 != it2)
				{
					std::uint16_t label = static_cast<std::uint8_t>(*it1++);
					index_type base = add(parent, label);
					parent = base + label;
				}
				assert(parent != 1);

				// 終端の空遷移を追加
				index_type idx = add(parent, null_value) + null_value;
				(m_c.data() + idx)->m_base = -value.second;

				++m_c.front().m_base;
			}

			return const_iterator(m_c, parent);
		}

		const_iterator insert(key_type const& key)
		{
			return insert(value_type(key, 0));
		}

		template <typename InputIterator>
		void insert(InputIterator first, InputIterator last)
		{
			while (first != last)
			{
				insert(*first);
				++first;
			}
		}

		void erase(const_iterator pos)
		{
			index_type idx = pos.m_index;
			assert(0 <= idx && idx < limit());

			if (idx <= 1 || !has_null(idx)) return;
			assert(has_null(idx));

			index_type term = (m_c.data() + idx)->m_base + null_value;
			assert((m_c.data() + term)->m_check == idx);
			free(term);

			while (idx != 1 && !has_null(idx) && !has_child(idx))
			{
				index_type parent = (m_c.data() + idx)->m_check;
				free(idx);
				idx = parent;
			}

			--m_c.front().m_base;
			if (empty()) clear();
		}

		void erase(key_type const& key) { erase(find(key)); }

		void swap(stable_trie_base& other)
		{
			base_type::swap(other);
		}

		// 検索 ---------------------------------------------------------------

		/*! 部分一致検索

		- 引数first, lastは検索文字列のbegin(), end()。
		- 戻り値は一致した最後の状態と次の文字を指すイテレータのペア。

		- 一文字も一致しない場合、cbegin()を返す。
		*/
		template <typename InputIterator>
		auto search(InputIterator first, InputIterator last) const ->std::pair<const_iterator, InputIterator>
		{
			index_type parent = 1;

			while (first != last)
			{
				std::uint16_t ch = static_cast<std::uint8_t>(*first);
				index_type idx = at(parent, ch);
				if (idx == 0) break;
				++first;
				parent = idx;
				assert(1 <= parent && parent < limit());
			}

			return std::make_pair(const_iterator(m_c, parent), first);
		}

		/*! 前方一致検索

		- 空文字列に対してはcbegin()を返す。
		- この動作は再検討が必要かもしれない。
		*/
		const_iterator search(key_type const& key) const
		{
			auto it1 = std::begin(key);
			auto it2 = std::end(key);

			auto pair = search(it1, it2);

			return (pair.second == it2)
				? pair.first
				: cend();
		}

		/*! 完全一致検索
		*/
		const_iterator find(key_type const& key) const
		{
			auto it1 = std::begin(key);
			auto it2 = std::end(key);

			auto pair = search(it1, it2);

			auto it = pair.second;
			index_type idx = pair.first.m_index;

			return (it == it2 && has_null(idx))
				? pair.first
				: cend();
		}

		bool contains(key_type const& key) const
		{
			auto it1 = std::begin(key);
			auto it2 = std::end(key);

			auto pair = search(it1, it2);

			auto it = pair.second;
			index_type idx = pair.first.m_index;

			return !(it != it2 || !has_null(idx));
		}
	};

	template <typename Key1, typename Allocator1>
	inline std::ostream& operator<<(std::ostream& os, stable_trie_base<Key1, Allocator1> const& trie)
	{
		typename stable_trie_base<Key1, Allocator1>::base_type const& heap = trie;
		return os << heap;
	}

	template <typename Key1, typename Allocator1>
	inline std::istream& operator>>(std::istream& is, stable_trie_base<Key1, Allocator1>& trie)
	{
		typename stable_trie_base<Key1, Allocator1>::base_type& heap = trie;
		return is >> heap;
	}
}
