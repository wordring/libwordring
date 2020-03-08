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

	メモリー使用量削減を目標とする <b>trie</b> と葉からの空遷移先INDEXが衝突によって変更されない
	<b>stable_trie</b> が事前に定義されている。

	@code
		template <typename Label, typename Allocator = std::allocator<detail::trie_node>>
		using trie = basic_trie<Label, detail::trie_base<Allocator>>;

		template <typename Label, typename Allocator = std::allocator<detail::trie_node>>
		using stable_trie = basic_trie<Label, detail::stable_trie_base<Allocator>>;
	@endcode

	- @ref wordring::trie
	- @ref wordring::stable_trie
	- @ref detail::trie_base
	- @ref detail::stable_trie_base

	@par Trie木のコンセプト

	Trie木の一般的な説明は、以下のページが詳しい。\n
	https://ja.wikipedia.org/wiki/%E3%83%88%E3%83%A9%E3%82%A4%E6%9C%A8

	このクラスは、木に一般化するため、遷移ラベルをノードの値と見做せるよう設計した。
	そのため、木に対するアルゴリズムを適用できる。

	このクラスは任意の整数列を格納できるが、文書内では、キーとして使う整数列のことを文字列と表記する。

	利用者からは、以下の図のように見える。

	@image html trie_concept.svg

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
	- @ref at()
	
	@par イテレータ

	ノード間の移動には、イテレータを用いる。
	イテレータは木に対するイテレータと互換性を保つよう設計した。
	イテレータを逆参照すると親から当該ノードへの遷移に使われたラベルを取り出せる。

	親を得るには <b>parent()</b> を使う。
	子を得るには、<b>%begin()</b> 、 <b>%end()</b> 、 <b>%operator[]()</b> を使う。
	次の兄弟を得るには、 <b>%operator++()</b> を使う。
	葉を指しているか調べるには、<b>%operator bool()</b> あるいは <b>%operator!()</b> を使う。

	@image html tree_node_iterator_concept.svg

	- @ref detail::const_trie_iterator

	@par 予測入力への適用

	部分木全体の走査にイテレータ・アダプタを使用できる。
	search() と wordring::basic_tree_iterator を組み合わせることで、予測入力などに使える。

	search() で前方一致検索を行い、戻り値のイテレータを親として wordring::basic_tree_iterator によって全走査を行う。
	<b>%operator bool()</b> で葉を確認し、葉を発見するたびに <b>parent()</b> で親をたどると、後続の文字列を容易に列挙できる。
	候補数を制限したい場合、数に達した時点で走査を止めると良い。

	- @ref wordring::basic_tree_iterator

	@par 直列化

	Trie木は辞書や文字列アトムに使われるため、直列化が必要な場合がある。
	直列化には <b>ibegin()</b>, <b>iend()</b> を使う。

	- @ref detail::trie_heap::ibegin() const
	- @ref detail::trie_heap::iend() const

	<b>ibegin()</b> の逆参照は32ビット整数値を返す。
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

		@par 例
		@code
			// 初期化子リストから構築
			auto t1 = trie<char32_t>({ { 1, 2 }, { 3, 4 }, { 5, 6 } });
		@endcode
		*/
		basic_trie(std::initializer_list<detail::trie_node> il, allocator_type const& alloc = allocator_type())
			: base_type(il, alloc)
		{
		}

		/*! @brief 直列化データから割り当てる

		@param [in]
			first 直列化データの先頭を指すイテレータ
		@param [in]
			last 直列化データの終端を指すイテレータ

		バイト直列化の例は以下を参照。

		@sa detail::trie_heap::ibegin() const

		@par 例
		@code
			// 元となるtrieを作成
			std::vector<std::u32string> v1{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
			auto t1 = trie<char32_t>(v1.begin(), v1.end());

			// 直列化データを作成
			auto v2 = std::vector<std::int32_t>(t1.ibegin(), t1.iend());

			// 直列化データから割り当てる
			trie<char32_t> t2;
			t2.assign(v2.begin(), v2.end());
		@endcode
		*/
		template <typename InputIterator, typename std::enable_if_t<std::is_integral_v<typename std::iterator_traits<InputIterator>::value_type>, std::nullptr_t> = nullptr>
		void assign(InputIterator first, InputIterator last)
		{
			base_type::assign(first, last);
		}

		/*! @brief 文字列リストから割り当てる


		@param [in]
			first 文字列リストの先頭を指すイテレータ
		@param [in]
			last 文字列リストの終端を指すイテレータ

		@sa trie_heap::assign(InputIterator first, InputIterator last)

		@par 例
		@code
			// 文字列リスト
			std::vector<std::u32string> v{ U"あ", U"あう", U"い", U"うあい", U"うえ" };

			// 文字列リストから割り当てる
			trie<char32_t> t;
			t.assign(v.begin(), v.end());
		@endcode
		*/
		template <typename ForwardIterator, typename std::enable_if_t<std::negation_v<std::is_integral<typename std::iterator_traits<ForwardIterator>::value_type>>, std::nullptr_t> = nullptr>
		void assign(ForwardIterator first, ForwardIterator last)
		{
			using string_type    = typename std::iterator_traits<ForwardIterator>::value_type;
			using character_type = typename string_type::value_type;

			assert(coefficient == sizeof(character_type));

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

		@param [in] pos 葉を指すイテレータ

		@return 葉の値に対するプロキシ

		葉に2,147,483,647までの正の整数値を格納できる。
		実際に返される値は軽量プロキシである。

		入力の正当性はチェックされない。

		@par 例
		@code
			// Trie木を作成
			std::vector<std::u32string> v{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
			auto t = trie<char32_t>(v.begin(), v.end());

			// 検索
			auto pos = t.find(std::u32string(U"あう"));

			// 値への参照を取得
			auto val = t.at(pos);
			// 値を設定
			val = 100;

			// 検証
			assert(t.at(std::u32string(U"あう")) == 100);
		@endcode
		*/
		reference at(const_iterator pos)
		{
			return base_type::at(static_cast<typename base_type::const_iterator>(pos));
		}

		/*! @brief 葉の値への参照を返す

		@param [in] pos 葉を指すイテレータ

		@return 葉の値に対するプロキシ

		@sa at(const_iterator pos)
		*/
		const_reference at(const_iterator pos) const
		{
			return const_cast<basic_trie<Label, Base>*>(this)->at(pos);
		}

		/*! @brief 葉の値への参照を返す

		@param [in] first キー文字列の先頭を指すイテレータ
		@param [in] last キー文字列の終端を指すイテレータ

		@return 葉の値に対するプロキシ

		@throw std::out_of_range キー文字列が格納されていない場合

		@sa at(const_iterator pos)

		入力の正当性はチェックされる。

		@par 例
		@code
			// Trie木を作成
			std::vector<std::u32string> v{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
			auto t = trie<char32_t>(v.begin(), v.end());

			// 値への参照を取得
			std::u32string key{ U"あう" };
			auto val = t.at(key.begin(), key.end());
			// 値を設定
			val = 100;

			// 検証
			assert(t.at(std::u32string(U"あう")) == 100);
		@endcode
		*/
		template <typename InputIterator>
		reference at(InputIterator first, InputIterator last)
		{
			assert(coefficient == sizeof(typename std::iterator_traits<InputIterator>::value_type));

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

		/*! @brief 葉の値への参照を返す

		@param [in] first キー文字列の先頭を指すイテレータ
		@param [in] last キー文字列の終端を指すイテレータ

		@return 葉の値に対するプロキシ

		@sa at(InputIterator first, InputIterator last)
		*/
		template <typename InputIterator>
		const_reference at(InputIterator first, InputIterator last) const
		{
			return const_cast<basic_trie<Label, Base>*>(this)->at(first, last);
		}

		/*! @brief 葉の値への参照を返す

		@param [in] key キー文字列（ラベル列）

		@return 葉の値に対するプロキシ

		@throw std::out_of_range キーが格納されていない場合

		@sa at(const_iterator pos)

		入力の正当性はチェックされる。

		@par 例
		@code
			// Trie木を作成
			std::vector<std::u32string> v{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
			auto t = trie<char32_t>(v.begin(), v.end());

			// 値への参照を取得
			auto val = t.at(std::u32string(U"あう"));
			// 値を設定
			val = 100;

			// 検証
			assert(t.at(std::u32string(U"あう")) == 100);
		@endcode
		*/
		template <typename Key>
		reference at(Key const& key)
		{
			return at(std::begin(key), std::end(key));
		}

		/*! @brief 葉の値への参照を返す

		@param [in] key キー文字列（ラベル列）

		@return 葉の値に対するプロキシ

		@throw std::out_of_range キーが格納されていない場合

		@sa at(Key const& key)
		*/
		template <typename Key>
		const_reference const at(Key const& key) const
		{
			return at(std::begin(key), std::end(key));
		}

		/*! @brief 葉の値への参照を返す

		@param [in] key キー文字列（ラベル列）

		@return 葉の値に対するプロキシ

		@sa at(const_iterator pos)

		キー文字列が格納されていない場合、新たに挿入し、その葉の値への参照を返す。

		@par 例
		@code
			// Trie木を作成
			std::vector<std::u32string> v{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
			auto t = trie<char32_t>(v.begin(), v.end());

			// 値への参照を取得して、値を設定
			t[std::u32string(U"あう")] = 100;

			// 検証
			assert(t.at(std::u32string(U"あう")) == 100);
		@endcode
		*/
		template <typename Key>
		reference operator[](Key const& key)
		{
			const_iterator it = find(key);
			if (it == cend()) it = insert(key);

			return at(it);
		}

		// イテレータ ----------------------------------------------------------

		/*! @brief 根を指すイテレータを返す

		@return 根を指すイテレータ

		空のTrieも根を持つ。
		*/
		const_iterator begin() const noexcept { return const_iterator(m_c, 1); }

		/*! @brief 根を指すイテレータを返す

		@return 根を指すイテレータ

		空のTrieも根を持つ。
		*/
		const_iterator cbegin() const noexcept { return const_iterator(m_c, 1); }

		/*! @brief 根の終端を指すイテレータを返す

		@return 根の終端を指すイテレータ
		*/
		const_iterator end() const noexcept { return const_iterator(m_c, 0); }

		/*! @brief 根の終端を指すイテレータを返す

		@return 根の終端を指すイテレータ
		*/
		const_iterator cend() const noexcept { return const_iterator(m_c, 0); }

		// 容量 ---------------------------------------------------------------

		/*! @brief キー文字列を格納していないことを調べる

		@return 格納している文字列が0の場合 true 、それ以外は false を返す。
		*/
		bool empty() const noexcept { return size() == 0; }

		/*! @brief 格納しているキー文字列数を調べる

		@return コンテナに格納されているキー文字列の数
		*/
		size_type size() const noexcept
		{
			return base_type::size();
		}

		/*! @brief キー文字列の格納可能な最大数を調べる

		@return キー文字列の格納可能な最大数
		*/
		static constexpr size_type max_size() noexcept
		{
			return base_type::max_size() / coefficient;
		}

		// 変更 ---------------------------------------------------------------

		/*! @brief キー文字列を挿入する

		@param [in] first キー文字列の先頭を指すイテレータ
		@param [in] last  キー文字列の終端を指すイテレータ
		@param [in] value 葉へ格納する値（省略時は0）

		@return 挿入された最後の文字に対応するノードを指すイテレータ

		空遷移で終わるとしても、空遷移ではなく最後の文字に対応するノードを指すイテレータを返す。

		@par 例
		@code
			trie<char32_t> t;

			// キー文字列と値100を挿入する
			auto key = std::u32string(U"あう");
			auto it = t.insert(key.begin(), key.end(), 100);

			// 挿入された最後の文字に対応するイテレータが返されている
			assert(*it == U'う');
			// 値100が設定されている
			assert(t.at(it) == 100);
			// 挿入したキー文字列が格納されている
			assert(t.contains(std::u32string(U"あう")));
		@endcode
		*/
		template <typename InputIterator>
		const_iterator insert(InputIterator first, InputIterator last, value_type value = 0)
		{
			assert(coefficient == sizeof(typename std::iterator_traits<InputIterator>::value_type));

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

		/*! @brief キー文字列を挿入する

		@param [in] key キー文字列
		@param [in] value 葉へ格納する値（省略時は0）

		@return 挿入された最後の文字に対応するノードを指すイテレータ

		空遷移で終わるとしても、空遷移ではなく最後の文字に対応するノードを指すイテレータを返す。

		@par 例
		@code
			trie<char32_t> t;

			// キー文字列と値100を挿入する
			auto it = t.insert(std::u32string(U"あう"), 100);

			// 挿入された最後の文字に対応するイテレータが返されている
			assert(*it == U'う');
			// 値100が設定されている
			assert(t.at(it) == 100);
			// 挿入したキー文字列が格納されている
			assert(t.contains(std::u32string(U"あう")));
		@endcode
		*/
		template <typename Key>
		const_iterator insert(Key const& key, value_type value = 0)
		{
			return insert(std::begin(key), std::end(key), value);
		}

		/*! @brief キー文字列を削除する

		@param [in] pos 削除するキー文字列の末尾に対応するノードへのイテレータ

		@par 例
		@code
			// Trie木を作成
			std::vector<std::u32string> v{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
			auto t = trie<char32_t>(v.begin(), v.end());

			// キー文字列を検索する
			auto it = t.find(std::u32string(U"あう"));

			// キーを削除する
			t.erase(it);

			// 検証
			assert(! t.contains(std::u32string(U"あう")));
		@endcode
		*/
		void erase(const_iterator pos)
		{
			base_type::erase(static_cast<typename base_type::const_iterator>(pos));
		}

		/*! @brief キー文字列を削除する

		@param [in] first 削除するキー文字列の先頭を指すイテレータ
		@param [in] last  削除するキー文字列の終端を指すイテレータ

		@sa erase(const_iterator pos)

		@par 例
		@code
			// Trie木を作成
			std::vector<std::u32string> v{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
			auto t = trie<char32_t>(v.begin(), v.end());

			// キー文字列を削除する
			auto key = std::u32string(U"あう");
			t.erase(key.begin(), key.end());

			// 検証
			assert(! t.contains(std::u32string(U"あう")));
		@endcode
		*/
		template <typename InputIterator>
		void erase(InputIterator first, InputIterator last)
		{
			assert(coefficient == sizeof(typename std::iterator_traits<InputIterator>::value_type));

			erase(find(first, last));
		}

		/*! @brief キー文字列を削除する

		@param [in] key 削除するキー文字列

		@sa erase(const_iterator pos)

		@par 例
		@code
			// Trie木を作成
			std::vector<std::u32string> v{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
			auto t = trie<char32_t>(v.begin(), v.end());

			// キー文字列を削除する
			t.erase(std::u32string(U"あう"));

			// 検証
			assert(! t.contains(std::u32string(U"あう")));
		@endcode
		*/
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

		/*! @brief 部分一致検索

		@param [in] first 検索するキー文字列の先頭を指すイテレータ
		@param [in] last  検索するキー文字列の終端を指すイテレータ

		@return 一致した最後のノードと次の文字を指すイテレータのペア

		一文字も一致しない場合、cbegin()を返す。

		@par 例
		@code
			// Trie木を作成
			std::vector<std::u32string> v{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
			auto t = trie<char32_t>(v.begin(), v.end());

			// キー文字列を部分一致検索する
			std::u32string s{ U"うい" };
			auto pair = t.lookup(s.begin(), s.end());

			// 一致した最後のノードと
			assert(*pair.first == U'う');
			// 一致した最後のキー文字の次を返す
			assert(*pair.second == U'い');
		@endcode
		*/
		template <typename InputIterator>
		auto lookup(InputIterator first, InputIterator last) const
		{
			assert(coefficient == sizeof(typename std::iterator_traits<InputIterator>::value_type));

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

				std::uint32_t i = 0;
				auto ret = base_type::lookup(it1, it2, i);
				
				i = i % coefficient;
				if (i != 0) for (; i != 0; --i) ret.first = ret.first.parent();

				result.first = ret.first;
				result.second = ret.second.base();
			}

			return result;
		}

		/*! @brief 前方一致検索

		@param [in] key  検索するキー文字列

		@return 一致した最後のノード

		一文字も一致しない場合、cbegin()を返す。
		*/
		template <typename InputIterator>
		const_iterator search(InputIterator first, InputIterator last) const
		{
			assert(coefficient == sizeof(typename std::iterator_traits<InputIterator>::value_type));

			auto pair = lookup(first, last);

			return (pair.second == last)
				? pair.first
				: cend();
		}

		/*! @brief 前方一致検索

		@param [in] key  検索するキー文字列

		@return 一致した最後のノード

		一文字も一致しない場合、cbegin()を返す。

		@par 例
		@code
			// Trie木を作成
			std::vector<std::u32string> v{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
			auto t = trie<char32_t>(v.begin(), v.end());

			// キー文字列を前方一致検索する
			auto it = t.search(std::u32string(U"うあ"));

			// 葉以外のノードにも一致する
			assert(!it);
			// 検索文字列全体のみに一致する
			assert(*it == U'あ');
		@endcode
		*/
		template <typename Key>
		const_iterator search(Key const& key) const
		{
			return search(std::begin(key), std::end(key));
		}

		/*! @brief 完全一致検索

		@param [in] first 検索するキー文字列の先頭を指すイテレータ
		@param [in] last  検索するキー文字列の終端を指すイテレータ

		@return
			入力されたキー文字列と完全に一致する葉がある場合、そのノードを指すイテレータ。
			それ以外の場合、 cend() 。
		*/
		template <typename InputIterator>
		const_iterator find(InputIterator first, InputIterator last) const
		{
			assert(coefficient == sizeof(typename std::iterator_traits<InputIterator>::value_type));

			auto pair = lookup(first, last);

			auto it = pair.second;
			index_type idx = pair.first.m_index;

			return (it == last && is_tail(idx))
				? pair.first
				: cend();
		}

		/*! @brief 完全一致検索

		@param [in] key 検索するキー文字列

		@return
			入力されたキー文字列と完全に一致する葉がある場合、そのノードを指すイテレータ。
			それ以外の場合、 cend() 。

		@par 例
		@code
			// Trie木を作成
			std::vector<std::u32string> v{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
			auto t = trie<char32_t>(v.begin(), v.end());

			// キー文字列を完全一致検索する
			auto it = t.find(std::u32string(U"あ"));

			// 葉のみに一致する
			BOOST_CHECK(it);
			// 検索文字列全体のみに一致する
			BOOST_CHECK(*it == U'あ');
		@endcode
		*/
		template <typename Key>
		const_iterator find(Key const& key) const
		{
			return find(std::begin(key), std::end(key));
		}

		/*! @brief キー文字列が格納されているか調べる

		@param [in] first キー文字列の先頭を指すイテレータ
		@param [in] last  キー文字列の終端を指すイテレータ

		@return 格納されている場合 true 、それ以外の場合 false
		*/
		template <typename InputIterator>
		bool contains(InputIterator first, InputIterator last) const
		{
			assert(coefficient == sizeof(typename std::iterator_traits<InputIterator>::value_type));

			auto pair = lookup(first, last);

			auto it = pair.second;
			index_type idx = pair.first.m_index;

			return !(it != last || !is_tail(idx));
		}

		/*! @brief キー文字列が格納されているか調べる

		@param [in] key  キー文字列

		@return 格納されている場合 true 、それ以外の場合 false

		@par 例
		@code
			// Trie木を作成
			std::vector<std::u32string> v{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
			auto t = trie<char32_t>(v.begin(), v.end());

			// キー文字列「うあい」は格納されている
			assert(t.contains(std::u32string(U"うあい")));
			// キー文字列「え」は格納されていない
			assert(t.contains(std::u32string(U"え")) == false);
		@endcode
		*/
		template <typename Key>
		bool contains(Key const& key) const
		{
			return contains(std::begin(key), std::end(key));
		}
	};

	/*! @brief ストリームへ出力する

	速度を必要とする場合、使用を推奨しない。

	@par 例
	@code
		// Trie木を作成
		std::vector<std::u32string> v{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
		auto t1 = trie<char32_t>(v.begin(), v.end());

		// ストリームへ出力
		std::stringstream ss;
		ss << t1;

		// ストリームから入力
		trie<char32_t> t2;
		ss >> t2;

		// 検証
		assert(t1.size() == t2.size());
	@endcode
	*/
	template <typename Label1, typename Base1>
	inline std::ostream& operator<<(std::ostream& os, basic_trie<Label1, Base1> const& trie)
	{
		typename basic_trie<Label1, Base1>::base_type const& base = trie;
		return os << base;
	}

	/*! @brief ストリームから入力する

	速度を必要とする場合、使用を推奨しない。
	*/
	template <typename Label1, typename Base1>
	inline std::istream& operator>>(std::istream& is, basic_trie<Label1, Base1>& trie)
	{
		typename basic_trie<Label1, Base1>::base_type& base = trie;
		return is >> base;
	}

	/*! @brief メモリー使用量削減を目標とする汎用Trie

	辞書を用途として想定する。
	*/
	template <typename Label, typename Allocator = std::allocator<detail::trie_node>>
	using trie = basic_trie<Label, detail::trie_base<Allocator>>;

	/*! @brief 葉からの空遷移先INDEXが衝突によって変更されない汎用Trie

	文字列ATOMを用途として想定する。
	葉から空遷移した先のINDEXが挿入によって変更されないため、そのINDEXから文字列を復元できる。
	*/
	template <typename Label, typename Allocator = std::allocator<detail::trie_node>>
	using stable_trie = basic_trie<Label, detail::stable_trie_base<Allocator>>;
}
