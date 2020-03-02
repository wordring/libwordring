#pragma once

#include <wordring/trie/list_trie_iterator.hpp>
#include <wordring/trie/trie_base_iterator.hpp>
#include <wordring/trie/trie_construct_iterator.hpp>
#include <wordring/trie/trie_heap.hpp>

#include <algorithm>
#include <cassert>
#include <climits>
#include <cstdint>
#include <istream>
#include <iterator>
#include <memory>
#include <ostream>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace wordring
{
	// ------------------------------------------------------------------------
	// trie_base
	// ------------------------------------------------------------------------

	/*!
	- 挿入や削除によってすべてのイテレータが無効となる。
	*/
	template <typename Allocator = std::allocator<trie_node>>
	class trie_base : public trie_heap<Allocator>
	{
		template <typename Allocator1>
		friend std::ostream& operator<<(std::ostream&, trie_base<Allocator1> const&);

		template <typename Allocator1>
		friend std::istream& operator>>(std::istream&, trie_base<Allocator1>&);

	protected:
		using base_type = trie_heap<Allocator>;

		using typename base_type::container;
		using typename base_type::label_vector;
		using typename base_type::index_type;
		using typename base_type::node_type;

		using base_type::null_value;

	public:
		using label_type      = typename base_type::label_type;
		using value_type      = std::uint32_t;
		using size_type       = typename container::size_type;
		using allocator_type  = Allocator;
		using reference       = trie_value_proxy;
		using const_reference = trie_value_proxy const;
		using const_iterator  = const_trie_base_iterator<container const>;

	public:
		using typename base_type::serialize_iterator;

		using base_type::get_allocator;
		using base_type::ibegin;
		using base_type::iend;
		using base_type::clear;

	protected:
		using base_type::assign;
		using base_type::limit;
		using base_type::free;
		using base_type::is_tail;
		using base_type::has_child;
		using base_type::has_null;
		using base_type::has_sibling;
		using base_type::at;
		using base_type::add;

		using base_type::m_c;

	public:
		trie_base()
			: base_type()
		{
		}

		explicit trie_base(allocator_type const& alloc)
			: base_type(alloc)
		{
		}

		/*! 直列化データからの構築
		*/
		template <typename InputIterator, typename std::enable_if_t<std::is_integral_v<typename std::iterator_traits<InputIterator>::value_type>, std::nullptr_t> = nullptr>
		trie_base(InputIterator first, InputIterator last, allocator_type const& alloc = allocator_type())
			: base_type(alloc)
		{
			assign(first, last);
		}

		/*! 文字列リストからの構築
		*/
		template <typename ForwardIterator, typename std::enable_if_t<std::negation_v<std::is_integral<typename std::iterator_traits<ForwardIterator>::value_type>>, std::nullptr_t> = nullptr>
		trie_base(ForwardIterator first, ForwardIterator last, allocator_type const& alloc = allocator_type())
			: base_type(alloc)
		{
			assign(first, last);
		}

		/*! 初期化子リストからの構築
		*/
		trie_base(std::initializer_list<trie_node> il, allocator_type const& alloc = allocator_type())
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

		/*! 文字列リストからの割り当て

		- 葉の値は全て0に初期化される。
		*/
		template <typename ForwardIterator, typename std::enable_if_t<std::negation_v<std::is_integral<typename std::iterator_traits<ForwardIterator>::value_type>>, std::nullptr_t> = nullptr>
		void assign(ForwardIterator first, ForwardIterator last)
		{
			using iterator_category = typename std::iterator_traits<ForwardIterator>::iterator_category;

			static_assert(
				   std::is_same_v<iterator_category, std::forward_iterator_tag>
				|| std::is_same_v<iterator_category, std::bidirectional_iterator_tag>
				|| std::is_same_v<iterator_category, std::random_access_iterator_tag>);

			clear();

			if (!std::is_sorted(first, last) || std::adjacent_find(first, last) != last)
			{
				while (first != last)
				{
					insert(*first);
					++first;
				}
				return;
			}

			using list_iterator = const_list_trie_iterator<ForwardIterator>;
			using construct_iterator = trie_construct_iterator<list_iterator>;

			auto li = list_iterator(first, last);
			auto it = construct_iterator(li);

			while (!it.empty() && !it.children().empty())
			{
				auto view = it.parent();
				auto it1 = lookup(view.begin(), view.end());
				if (it1.first.m_index == 0) it1.first.m_index = 1; // rootの場合。
				add(it1.first.m_index, it.children());
				++it;
			}

			m_c.front().m_base = std::distance(first, last);
		}

		// 要素アクセス --------------------------------------------------------

		reference at(const_iterator pos)
		{
			node_type* d = m_c.data();
			// 子遷移が有り、なおかつ文字列終端の場合に対応する。
			index_type base = (d + pos.m_index)->m_base;
			index_type idx = (base <= 0)
				? pos.m_index
				: base + null_value;

			return reference(std::addressof((d + idx)->m_base));
		}

		const_reference at(const_iterator pos) const
		{
			return const_cast<trie_base*>(this)->at(pos);
		}

		/*! 文字列に対応する値を指す逆参照プロキシを返す

		- [first, last)は、文字を参照するイテレータ。
		*/
		template <typename InputIterator>
		reference at(InputIterator first, InputIterator last)
		{
			auto it = find(first, last);
			if (it == cend()) throw std::out_of_range("");

			node_type* d = m_c.data();
			// 子遷移が有り、なおかつ文字列終端の場合に対応する。
			index_type base = (d + it.m_index)->m_base;
			index_type idx = (base <= 0)
				? it.m_index
				: base + null_value;

			return reference(std::addressof((d + idx)->m_base));
		}

		template <typename InputIterator>
		const_reference at(InputIterator first, InputIterator last) const
		{
			return const_cast<trie_base*>(this)->at(first, last);
		}

		/*!
		 key は、文字列を想定する。
		*/
		template <typename Key>
		reference at(Key const& key)
		{
			return at(std::begin(key), std::end(key));
		}

		template <typename Key>
		const_reference const at(Key const& key) const
		{
			return at(std::begin(key), std::end(key));
		}

		template <typename Key>
		reference operator[](Key const& key)
		{
			auto it = find(key);
			if (it == cend()) it = insert(key);

			node_type* d = m_c.data();
			// 子遷移が有り、なおかつ文字列終端の場合に対応する。
			index_type base = (d + it.m_index)->m_base;
			index_type idx = (base <= 0)
				? it.m_index
				: base + null_value;

			return reference(std::addressof((d + idx)->m_base));
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

		/*! 文字列を挿入する

		- first、lastは文字列を示すイテレータ。
		- 戻り値は、挿入された最後の文字を指すイテレータ。
		- 空遷移で終わるとしても、空遷移先ではなく最後の文字を指すイテレータを返す。
		*/
		template <typename InputIterator>
		const_iterator insert(InputIterator first, InputIterator last, value_type value = 0)
		{
			assert(value <= std::numeric_limits<int32_t>::max());

			if (first == last) return cend();

			index_type parent = 1;

			// 登録済み遷移をスキップする
			for (index_type idx = at(parent, static_cast<std::uint8_t>(*first)); idx != 0; idx = at(parent, static_cast<std::uint8_t>(*first)))
			{
				parent = idx;
				++first;
				if (first == last) break;
			}
			if (first != last) ++m_c.front().m_base;

			// 空遷移の解決
			if (has_child(parent)) // 既存遷移有り、新規文字列無し
			{
				if (first == last && !has_null(parent))
				{
					index_type base = add(parent, null_value);
					assert((m_c.data() + base + null_value)->m_base == 0);
					(m_c.data() + base + null_value)->m_base = -value;
					++m_c.front().m_base;
				}
			}
			else if (parent != 1 && !has_null(parent) && first != last) // 後続遷移無し、新規文字列有り
			{
				index_type val = (m_c.data() + parent)->m_base;
				assert(val <= 0);
				index_type base = add(parent, null_value);
				assert((m_c.data() + base + null_value)->m_base == 0);
				(m_c.data() + base + null_value)->m_base = val;
			}

			// 新規文字列による遷移を追加
			index_type base = 0; 
			while (first != last)
			{
				std::uint16_t label = static_cast<std::uint8_t>(*first);
				++first;
				base = add(parent, label);
				parent = base + label;
			}
			if (base != 0)
			{
				assert((m_c.data() + parent)->m_base == 0);
				(m_c.data() + parent)->m_base = -value;
			}

			assert(parent != 1);

			return const_iterator(m_c, parent);
		}

		/*! 文字列を挿入する
		*/
		template <typename Key>
		const_iterator insert(Key const& key, value_type value = 0)
		{
			return insert(std::begin(key), std::end(key), value);
		}

		/*! 文字列を削除する
		*/
		void erase(const_iterator pos)
		{
			index_type idx = pos.m_index;
			assert(0 <= idx && idx < limit());

			if (idx <= 1 || !is_tail(idx)) return;

			while (true)
			{
				if (has_null(idx))
				{
					index_type i = (m_c.data() + idx)->m_base + null_value;
					assert((m_c.data() + i)->m_check == idx);

					free(i);
					if (!has_child(idx)) (m_c.data() + idx)->m_base = 0;

					break;
				}

				index_type parent = 0;
				if (!has_sibling(idx)) parent = (m_c.data() + idx)->m_check; // 兄弟が無い場合
				assert(0 <= parent && parent < limit());

				if (idx != 1) free(idx);
				if (parent == 0) break;

				idx = parent;
			}

			--m_c.front().m_base;
			if (empty()) clear();
		}

		template <typename InputIterator>
		void erase(InputIterator first, InputIterator last)
		{
			erase(find(first, last));
		}

		template <typename Key>
		void erase(Key const& key)
		{
			erase(std::begin(key), std::end(key));
		}

		void swap(trie_base& other)
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
		auto lookup(InputIterator first, InputIterator last) const
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

		template <typename InputIterator>
		const_iterator search(InputIterator first, InputIterator last) const
		{
			auto pair = lookup(first, last);

			return (pair.second == last)
				? pair.first
				: cend();
		}

		template <typename Key>
		const_iterator search(Key const& key) const
		{
			return search(std::begin(key), std::end(key));
		}

		/*! 完全一致検索
		*/
		template <typename InputIterator>
		const_iterator find(InputIterator first, InputIterator last) const
		{
			auto pair = lookup(first, last);

			auto it = pair.second;
			index_type idx = pair.first.m_index;

			return (it == last && is_tail(idx))
				? pair.first
				: cend();
		}

		template <typename Key>
		const_iterator find(Key const& key) const
		{
			return find(std::begin(key), std::end(key));
		}

		template <typename InputIterator>
		bool contains(InputIterator first, InputIterator last) const
		{
			auto pair = lookup(first, last);

			auto it = pair.second;
			index_type idx = pair.first.m_index;

			return !(it != last || !is_tail(idx));
		}

		template <typename Key>
		bool contains(Key const& key) const
		{
			return contains(std::begin(key), std::end(key));
		}
	};

	template <typename Allocator1>
	inline std::ostream& operator<<(std::ostream& os, trie_base<Allocator1> const& trie)
	{
		typename trie_base<Allocator1>::base_type const& heap = trie;
		return os << heap;
	}

	template <typename Allocator1>
	inline std::istream& operator>>(std::istream& is, trie_base<Allocator1>& trie)
	{
		typename trie_base<Allocator1>::base_type& heap = trie;
		return is >> heap;
	}
}
