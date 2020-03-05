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

	/*! @brief 任意の整数型をラベルとして用いることが出来る汎用Trie

	@tparam Label ラベルとして使用する任意の整数型
	@tparam Base 基本クラスとして使用するTrie実装クラス

	メモリー使用量削減を目標とする <b>trie</b> と葉のINDEXが衝突によって変更されない
	<b>stable_trie</b> が事前に定義されている。

	@code
		template <typename Label, typename Allocator = std::allocator<detail::trie_node>>
		using trie = basic_trie<Label, detail::trie_base<Allocator>>;

		template <typename Label, typename Allocator = std::allocator<detail::trie_node>>
		using stable_trie = basic_trie<Label, detail::stable_trie_base<Allocator>>;
	@endcode

	- @ref detail::trie_base
	- @ref detail::stable_trie_base

	@par trieのコンセプト

	trie木の一般的な説明は、以下のページが詳しい。\n
	https://ja.wikipedia.org/wiki/%E3%83%88%E3%83%A9%E3%82%A4%E6%9C%A8

	このクラスは、木に一般化するため、遷移ラベルをノードの値と見做せるよう設計した。
	そのため、木に対するアルゴリズムを適用できる。

	このクラスは任意の整数列を格納できるが、文書内では、キーとして使う整数列のことを文字列と表記する。

	利用者からは、以下の図のように見える。

	@image html trie_concept.svg

	- @ref detail::const_trie_iterator
	- @ref wordring::tree

	@par ラベル

	一般的にダブル・アレイはラベルとして8ビット整数を使うが、このクラスは直列化によって
	任意の整数型をラベルとして利用することが出来る。
	根はラベルを持たない。

	@par 値の格納

	葉に2,147,483,647までの正の整数値を格納できる。
	mapのように任意の型の値を格納できるよう拡張する場合、値の配列に対する添字を葉に格納することを想定している。
	葉に格納される値の取得・設定にはメンバ関数 at() を使う。
	イテレータの逆参照によって得られる値は遷移ラベルであり、葉に格納される値と明確に区別される。

	- @ref detail::trie_heap
	- @ref at
	
	@par イテレータ

	ノード間の移動には、イテレータを用いる。
	イテレータは木に対するイテレータと互換性を保つよう設計した。
	イテレータを逆参照すると親から当該ノードへの遷移に使われたラベルを取り出せる。

	イテレータは兄弟ノード、親、子、子への添字アクセスを提供する。

	- @ref detail::const_trie_iterator

	木と同様の走査に以下のイテレータ・アダプタを使用できる。

	- @ref wordring::basic_tree_iterator

	@par 直列化

	trieは辞書や文字列アトムに使われるため、直列化が必要な場合がある。
	直列化には ibegin(), iend() を使う。

	- @ref detail::trie_heap::ibegin() const
	- @ref detail::trie_heap::iend() const

	ibegin() の逆参照は32ビット整数値を返す。
	ファイルへ保存するためにバイト列を必要とする場合、直列化イテレータを使う。

	- @ref wordring::serialize_iterator
	*/
	template <typename Label, typename Base>
	class basic_trie : public Base
	{
		template <typename Label1, typename Base1>
		friend std::ostream& operator<<(std::ostream&, basic_trie<Label1, Base1> const&);

		template <typename Label1, typename Base1>
		friend std::istream& operator>>(std::istream&, basic_trie<Label1, Base1>&);

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
		using reference       = detail::trie_value_proxy;
		using const_reference = detail::trie_value_proxy const;
		using const_iterator  = detail::const_trie_iterator<label_type, typename base_type::const_iterator>;

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
		/*! @brief 空のコンテナを構築する

		@par 例
		@code
			auto t = trie<char32_t>();
		@endcode
		*/
		basic_trie()
			: base_type()
		{
		}

		/*! @brief アロケータを指定して空のコンテナを構築する

		@param [in] alloc アロケータ

		@par 例
		@code
			auto t = trie<char32_t>(std::allocator<detail::trie_node>());
		@endcode
		*/
		explicit basic_trie(allocator_type const& alloc)
			: base_type(alloc)
		{
		}

		/*! @brief 直列化データから構築する

		@param [in]
			first 直列化データの先頭を指すイテレータ
		@param [in]
			last 直列化データの終端を指すイテレータ
		@param [in]
			alloc アロケータ

		@sa assign(InputIterator first, InputIterator last)\n
		@sa detail::trie_heap::ibegin() const\n
		@sa detail::trie_heap::iend() const

		@par 例
		@code
			// 直列化データを作成
			std::vector<std::u32string> v1{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
			auto t1 = trie<char32_t>(v1.begin(), v1.end());
			std::vector<std::int32_t> v2{ t1.ibegin(), t1.iend() };

			// 直列化データから構築
			auto t2 = trie<char32_t>(v2.begin(), v2.end());
		@endcode
		*/
		template <typename InputIterator, typename std::enable_if_t<std::is_integral_v<typename std::iterator_traits<InputIterator>::value_type>, std::nullptr_t> = nullptr>
		basic_trie(InputIterator first, InputIterator last, allocator_type const& alloc = allocator_type())
			: base_type(alloc)
		{
			assign(first, last);
		}

		/*! @brief 文字列のリストから構築する

		@param [in]
			first 文字列リストの先頭を指すイテレータ
		@param [in]
			last 文字列リストの終端を指すイテレータ
		@param [in]
			alloc アロケータ

		@sa assign(ForwardIterator first, ForwardIterator last)

		@par 例
		@code
			// 文字列のリスト
			std::vector<std::u32string> v1{ U"あ", U"あう", U"い", U"うあい", U"うえ" };

			// 文字列のリストから構築
			auto t1 = trie<char32_t>(v1.begin(), v1.end());
		@endcode
		*/
		template <typename ForwardIterator, typename std::enable_if_t<std::negation_v<std::is_integral<typename std::iterator_traits<ForwardIterator>::value_type>>, std::nullptr_t> = nullptr>
		basic_trie(ForwardIterator first, ForwardIterator last, allocator_type const& alloc = allocator_type())
			: base_type(alloc)
		{
			assign(first, last);
		}

		/*! @brief 初期化子リストから構築する

		@param [in]
			il ノード・データの初期化子リスト
		@param [in]
			alloc アロケータ

		@sa detail::trie_heap::trie_heap(std::initializer_list<trie_node>, allocator_type const&)
		*/
		basic_trie(std::initializer_list<detail::trie_node> il, allocator_type const& alloc = allocator_type())
			: base_type(il, alloc)
		{
		}

		/*! @brief 直列化データから割り当てる
		*/
		template <typename InputIterator, typename std::enable_if_t<std::is_integral_v<typename std::iterator_traits<InputIterator>::value_type>, std::nullptr_t> = nullptr>
		void assign(InputIterator first, InputIterator last)
		{
			base_type::assign(first, last);
		}

		/*! @brief 文字列リストから割り当てる
		*/
		template <typename ForwardIterator, typename std::enable_if_t<std::negation_v<std::is_integral<typename std::iterator_traits<ForwardIterator>::value_type>>, std::nullptr_t> = nullptr>
		void assign(ForwardIterator first, ForwardIterator last)
		{
			using string_type    = typename std::iterator_traits<ForwardIterator>::value_type;
			using character_type = typename string_type::value_type;

			static_assert(coefficient == sizeof(character_type));

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

		/*! @brief 葉の値への参照を返す

		葉に2,147,483,647までの正の整数値を格納できる。
		*/
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
			static_assert(coefficient == sizeof(decltype(*first)));

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

	template <typename Label1, typename Base1>
	inline std::ostream& operator<<(std::ostream& os, basic_trie<Label1, Base1> const& trie)
	{
		typename basic_trie<Label1, Base1>::base_type const& base = trie;
		return os << base;
	}

	template <typename Label1, typename Base1>
	inline std::istream& operator>>(std::istream& is, basic_trie<Label1, Base1>& trie)
	{
		typename basic_trie<Label1, Base1>::base_type& base = trie;
		return is >> base;
	}

	/* @define trie

	@brief Trie

	- 葉のINDEXは、挿入時の衝突によって変更される場合がある。
	*/
	template <typename Label, typename Allocator = std::allocator<detail::trie_node>>
	using trie = basic_trie<Label, detail::trie_base<Allocator>>;

	template <typename Label, typename Allocator = std::allocator<detail::trie_node>>
	using stable_trie = basic_trie<Label, detail::stable_trie_base<Allocator>>;
}
