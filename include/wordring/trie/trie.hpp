#pragma once

#include <wordring/serialize/serialize_iterator.hpp>
#include <wordring/trie/stable_trie_base.hpp>
#include <wordring/trie/trie_base.hpp>
#include <wordring/trie/trie_iterator.hpp>

#include <memory>
#include <string>
#include <type_traits>

namespace wordring
{
	// ------------------------------------------------------------------------
	// basic_trie
	// ------------------------------------------------------------------------

	template <typename Label, typename Base>
	class basic_trie : public Base
	{
	protected:
		using base_type = Base;

		using typename base_type::container;
		using typename base_type::label_vector;
		using typename base_type::index_type;
		using typename base_type::node_type;

		using base_type::null_value;

	public:
		using label_type      = Label;
		using value_type      = std::uint32_t;
		using size_type       = typename container::size_type;
		using allocator_type  = typename base_type::allocator_type;
		using reference       = trie_value_proxy;
		using const_reference = trie_value_proxy const;
		using const_iterator  = const_trie_iterator<label_type, typename base_type::const_iterator>;

	public:
		using typename base_type::serialize_iterator;

		using base_type::get_allocator;
		using base_type::ibegin;
		using base_type::iend;
		using base_type::clear;

	protected:
		using base_type::is_tail;

		using base_type::m_c;

		static std::uint32_t constexpr coefficient = sizeof(label_type) / sizeof(typename base_type::label_type);

		static_assert(std::is_integral_v<label_type>);
		static_assert(1 <= coefficient);

	public:
		basic_trie()
			: base_type()
		{
		}

		explicit basic_trie(allocator_type const& alloc)
			: base_type(alloc)
		{
		}

		/*! 直列化データからの復元
		*/
		template <typename InputIterator, typename std::enable_if_t<std::is_integral_v<typename std::iterator_traits<InputIterator>::value_type>, std::nullptr_t> = nullptr>
		basic_trie(InputIterator first, InputIterator last, allocator_type const& alloc = allocator_type())
			: base_type(alloc)
		{
			assign(first, last);
		}

		/*! 文字列リストからの構築
		*/
		template <typename ForwardIterator, typename std::enable_if_t<std::negation_v<std::is_integral<typename std::iterator_traits<ForwardIterator>::value_type>>, std::nullptr_t> = nullptr>
		basic_trie(ForwardIterator first, ForwardIterator last, allocator_type const& alloc = allocator_type())
			: base_type(alloc)
		{
			assign(first, last);
		}

		/*! 初期化子リストからの構築
		*/
		basic_trie(std::initializer_list<trie_node> il, allocator_type const& alloc = allocator_type())
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
			if constexpr (coefficient == 1) base_type::assign(first, last);
			else
			{
				while (first != last)
				{
					insert(*first);
					++first;
				}
			}
		}

		// 要素アクセス --------------------------------------------------------

		reference at(const_iterator pos)
		{
			return base_type::at(static_cast<typename base_type::const_iterator>(pos));
		}

		const_reference at(const_iterator pos) const
		{
			return const_cast<basic_trie<Label, Base>*>(this)->at(pos);
		}

		template <typename InputIterator>
		reference at(InputIterator first, InputIterator last)
		{
			reference result;

			if constexpr (coefficient == 1) result = base_type::at(first, last);
			else
			{
				auto it1 = wordring::serialize_iterator(first);
				auto it2 = wordring::serialize_iterator(last);

				result = base_type::at(it1, it2);
			}

			return result;
		}

		template <typename InputIterator>
		const_reference at(InputIterator first, InputIterator last) const
		{
			return const_cast<basic_trie<Label, Base>*>(this)->at(first, last);
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
			const_iterator it = find(key);
			if (it == cend()) it = insert(key);

			return at(it);
		}

		// イテレータ ----------------------------------------------------------

		const_iterator begin() const noexcept { return const_iterator(m_c, 1); }

		const_iterator cbegin() const noexcept { return const_iterator(m_c, 1); }

		const_iterator end() const noexcept { return const_iterator(m_c, 0); }

		const_iterator cend() const noexcept { return const_iterator(m_c, 0); }

		// 容量 ---------------------------------------------------------------

		bool empty() const noexcept { return size() == 0; }

		size_type size() const noexcept
		{
			return base_type::size();
		}

		static constexpr size_type max_size() noexcept
		{
			return base_type::max_size() / coefficient;
		}

		// 変更 ---------------------------------------------------------------

		template <typename InputIterator>
		const_iterator insert(InputIterator first, InputIterator last, value_type value = 0)
		{
			const_iterator result;

			if constexpr (coefficient == 1) result = base_type::insert(first, last, value);
			else
			{
				auto it1 = wordring::serialize_iterator(first);
				auto it2 = wordring::serialize_iterator(last);

				result = base_type::insert(it1, it2, value);
			}

			return result;
		}

		template <typename Key>
		const_iterator insert(Key const& key, value_type value = 0)
		{
			return insert(std::begin(key), std::end(key), value);
		}

		/*! 文字列を削除する
		*/
		void erase(const_iterator pos)
		{
			base_type::erase(static_cast<typename base_type::const_iterator>(pos));
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

		void swap(basic_trie<Label, Base>& other)
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
			std::pair<const_iterator, InputIterator> result;

			if constexpr (coefficient == 1)
			{
				auto ret = base_type::lookup(first, last);
				result.first = ret.first;
				result.second = ret.second;
			}
			else
			{
				auto it1 = wordring::serialize_iterator(first);
				auto it2 = wordring::serialize_iterator(last);

				auto ret = base_type::lookup(it1, it2);

				result.first = ret.first;
				result.second = ret.second.base();
			}

			return result;
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

	template <typename Label, typename Allocator = std::allocator<trie_node>>
	using trie = basic_trie<Label, trie_base<Allocator>>;

	template <typename Label, typename Allocator = std::allocator<trie_node>>
	using stable_trie = basic_trie<Label, stable_trie_base<Allocator>>;
}
