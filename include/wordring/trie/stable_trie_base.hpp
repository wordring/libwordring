#pragma once

#include <wordring/trie/stable_trie_base_iterator.hpp>
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

namespace wordring::detail
{
	// ------------------------------------------------------------------------
	// stable_trie_base
	// ------------------------------------------------------------------------

	/*! @brief 挿入や削除によって葉を指すイテレータが無効とならない安定なTrie木の実装

	@tparam Allocator
		アロケータ

	@details
		ダブルアレイの制約により、labelの型は8ビット固定。
		葉を指すイテレータ以外は、衝突した場合、無効となる。

		このクラスは、文字列末尾の次に葉を表現するnull値を挿入することで葉の安定性を保つ。
		null値に子は無いため、挿入や削除によって衝突が起きることが無い。
		末尾にnull値が挿入される分、わずかながらメモリー使用量が増える。

		このクラスは、葉のINDEXが変わらない必要のある文字列アトム等の用途を想定している。

	@image html stable_trie_base_state.svg

	@sa wordring::basic_trie
	*/
	template <typename Allocator = std::allocator<trie_node>>
	class stable_trie_base : public trie_heap<Allocator>
	{
		template <typename Allocator1>
		friend std::ostream& operator<<(std::ostream&, stable_trie_base<Allocator1> const&);

		template <typename Allocator1>
		friend std::istream& operator>>(std::istream&, stable_trie_base<Allocator1>&);

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
		using const_iterator  = const_stable_trie_base_iterator<container const>;

	public:
		using typename base_type::serialize_iterator;

		using base_type::get_allocator;
		using base_type::ibegin;
		using base_type::iend;
		using base_type::clear;

	protected:
		//using base_type::assign;
		using base_type::limit;
		using base_type::free;
		using base_type::has_child;
		using base_type::has_null;
		using base_type::at;
		using base_type::add;

		using base_type::m_c;

	public:
		/*! @brief 空のコンテナを構築する

		@par 例
		@code
			auto trie = stable_trie_base();
		@endcode
		*/
		stable_trie_base()
			: base_type()
		{
		}

		/*! @brief アロケータを指定して空のコンテナを構築する

		@param [in] alloc アロケータ

		@par 例
		@code
			auto trie = stable_trie_base(std::allocator<detail::trie_node>());
		@endcode
		*/
		explicit stable_trie_base(allocator_type const& alloc)
			: base_type(alloc)
		{
		}

		/*! @brief 直列化データからの構築
		
		@param [in]
			first 直列化データの先頭を指すイテレータ
		@param [in]
			last 直列化データの終端を指すイテレータ
		@param [in]
			alloc アロケータ

		@sa assign(InputIterator first, InputIterator last)
		
		@par 例
		@code
			std::vector<std::string> v1{ "a", "ac", "b", "cab", "cd" };
			auto t1 = stable_trie_base(v1.begin(), v1.end());

			std::vector<std::int32_t> v2{ t1.ibegin(), t1.iend() };
			
			auto t2 = stable_trie_base(v2.begin(), v2.end());
		@endcode
		*/
		template <typename InputIterator, typename std::enable_if_t<std::is_integral_v<typename std::iterator_traits<InputIterator>::value_type>, std::nullptr_t> = nullptr>
		stable_trie_base(InputIterator first, InputIterator last, allocator_type const& alloc = allocator_type())
			: base_type(alloc)
		{
			assign(first, last);
		}

		/*! @brief 文字列リストからの構築

		@param [in]
			first 文字列リストの先頭を指すイテレータ
		@param [in]
			last 文字列リストの終端を指すイテレータ
		@param [in]
			alloc アロケータ
			
		@par 例
		@code
			std::vector<std::string> v{ "a", "ac", "b", "cab", "cd" };
	
			auto trie = stable_trie_base(v.begin(), v.end());
		@endcode
		*/
		template <typename ForwardIterator, typename std::enable_if_t<std::negation_v<std::is_integral<typename std::iterator_traits<ForwardIterator>::value_type>>, std::nullptr_t> = nullptr>
		stable_trie_base(ForwardIterator first, ForwardIterator last, allocator_type const& alloc = allocator_type())
			: base_type(alloc)
		{
			assign(first, last);
		}

		/*! @brief 初期化子リストからの構築

		@param [in]
			il ノード・データの初期化子リスト
		@param [in]
			alloc アロケータ

		@par 例
		@code
			auto trie = stable_trie_base({ { 0, 1 }, { 2, 3 } });
		@endcode
		*/
		stable_trie_base(std::initializer_list<trie_node> il, allocator_type const& alloc = allocator_type())
			: base_type(il, alloc)
		{
		}

		/*! @brief 直列化データからの割り当て

		@param [in]
			first 直列化データの先頭を指すイテレータ
		@param [in]
			last 直列化データの終端を指すイテレータ

		@sa
			trie_heap::assign(InputIterator first, InputIterator last)\n
			trie_heap::ibegin() const\n
			trie_heap::iend() const

		@par 例
		@code
			std::vector<std::string> v1{ "a", "ac", "b", "cab", "cd" };
			auto t1 = stable_trie_base(v1.begin(), v1.end());

			auto v2 = std::vector<std::int32_t>(t1.ibegin(), t1.iend());

			stable_trie_base<> t2;
			t2.assign(v2.begin(), v2.end());
		@endcode
		*/
		template <typename InputIterator, typename std::enable_if_t<std::is_integral_v<typename std::iterator_traits<InputIterator>::value_type>, std::nullptr_t> = nullptr>
		void assign(InputIterator first, InputIterator last)
		{
			base_type::assign(first, last);
		}

		/*! @brief 文字列リストからの割り当て

		@param [in]
			first 文字列集合の先頭を指すイテレータ
		@param [in]
			last 文字列集合の終端を指すイテレータ

		@par 例
		@code
			std::vector<std::string> v{ "a", "ac", "b", "cab", "cd" };
			stable_trie_base<> trie;
			trie.assign(v.begin(), v.end());
		@endcode

		葉の値は全て0に初期化される。
		*/
		template <typename ForwardIterator, typename std::enable_if_t<std::negation_v<std::is_integral<typename std::iterator_traits<ForwardIterator>::value_type>>, std::nullptr_t> = nullptr>
		void assign(ForwardIterator first, ForwardIterator last)
		{
			clear();
			while (first != last)
			{
				insert(*first);
				++first;
			}
		}

		// 要素アクセス --------------------------------------------------------

		/*! @brief 葉の値への参照を返す

		@param [in] pos 葉を指すイテレータ

		@return 葉の値に対するプロキシ

		葉に2,147,483,647までの正の整数値を格納できる。
		実際に返される値は軽量プロキシである。

		入力の正当性はチェックされない。

		このメンバは、継承するクラスから呼び出される目的で用意した。
		*/
		reference at(const_iterator pos)
		{
			node_type* d = m_c.data();

			index_type idx = (d + pos.m_index)->m_base + null_value;
			assert(1 < idx && idx < limit());

			return reference(std::addressof((d + idx)->m_base));
		}

		/*! @brief 葉の値への参照を返す

		@param [in] pos 葉を指すイテレータ

		@return 葉の値に対するプロキシ

		@sa at(const_iterator pos)
		*/
		const_reference at(const_iterator pos) const
		{
			return const_cast<stable_trie_base*>(this)->at(pos);
		}

		/*! @brief 葉の値への参照を返す

		@param [in] first キー文字列の先頭を指すイテレータ
		@param [in] last キー文字列の終端を指すイテレータ

		@return 葉の値に対するプロキシ

		@throw std::out_of_range キーが格納されていない場合

		@sa at(const_iterator pos)

		入力の正当性はチェックされる。
		*/
		template <typename InputIterator>
		reference at(InputIterator first, InputIterator last)
		{
			auto it = find(first, last);
			if (it == cend()) throw std::out_of_range("");

			return at(it);
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
			return const_cast<stable_trie_base*>(this)->at(first, last);
		}

		/*! @brief 葉の値への参照を返す

		@param [in] key キー文字列（ラベル列）

		@return 葉の値に対するプロキシ

		@throw std::out_of_range キーが格納されていない場合

		@sa at(const_iterator pos)

		入力の正当性はチェックされる。
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
		const_reference at(Key const& key) const
		{
			return at(std::begin(key), std::end(key));
		}

		/*! @brief 葉の値への参照を返す

		@param [in] key キー文字列（ラベル列）

		@return 葉の値に対するプロキシ

		@sa at(const_iterator pos)

		キー文字列が格納されていない場合、新たに挿入し、その葉の値への参照を返す。
		*/
		template <typename Key>
		reference operator[](Key const& key)
		{
			auto it = find(key);
			if (it == cend()) it = insert(key);

			index_type idx = (m_c.data() + it.m_index)->m_base + null_value;
			assert(1 < idx && idx < limit());

			return reference(std::addressof((m_c.data() + idx)->m_base));
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

		end() と cend() はシグネチャ以外同一である。
		stable_trie_base::end() と <b>const_iterator::end()</b> は同一である。
		*/
		const_iterator end() const noexcept { return const_iterator(m_c, 0); }

		/*! @brief 根の終端を指すイテレータを返す

		@return 根の終端を指すイテレータ

		end() と cend() はシグネチャ以外同一である。
		stable_trie_base::end() と <b>const_iterator::end()</b> は同一である。
		*/
		const_iterator cend() const noexcept { return const_iterator(m_c, 0); }

		// 容量 ---------------------------------------------------------------

		/*! @brief キー文字列を格納していないことを調べる
		
		@return 格納している文字列が無い場合 true 、それ以外は false を返す。
		*/
		bool empty() const noexcept { return size() == 0; }

		/*! @brief 格納しているキー文字列数を調べる
		
		@return コンテナに格納されているキー文字列の数
		*/
		size_type size() const noexcept { return m_c.front().m_base; }

		/*! @brief キー文字列の格納可能な最大数を調べる
		
		@return キー文字列の格納可能な最大数
		*/
		static constexpr size_type max_size() noexcept
		{
			return std::numeric_limits<std::int32_t>::max() / sizeof(node_type);
		}

		// 変更 ---------------------------------------------------------------

		/*! @brief キー文字列を挿入する

		@param [in] first キー文字列の先頭を指すイテレータ
		@param [in] last  キー文字列の終端を指すイテレータ
		@param [in] value 葉へ格納する値（省略時は0）

		@return 挿入された最後の文字に対応するノードを指すイテレータ

		キー文字列「ab」を挿入した場合の戻り値は以下の図の通り。

		<b>%stable_trie_base</b> の葉は必ず空遷移で終わるが、空遷移ではなく最後の文字に対応するノードを指すイテレータを返す。

		@image html stable_trie_base_insert.svg
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

			if (!has_null(parent) || first != last)
			{
				// 新規文字列による遷移を追加
				while (first != last)
				{
					std::uint16_t label = static_cast<std::uint8_t>(*first++);
					index_type base = add(parent, label);
					parent = base + label;
				}
				assert(parent != 1);

				// 終端の空遷移を追加
				index_type idx = add(parent, null_value) + null_value;
				(m_c.data() + idx)->m_base = -value;

				++m_c.front().m_base;
			}

			return const_iterator(m_c, parent);
		}

		/*! @brief キー文字列を挿入する

		@param [in] key キー文字列
		@param [in] value 葉へ格納する値（省略時は0）

		@return 挿入された最後の文字に対応するノードを指すイテレータ

		@sa insert(InputIterator, InputIterator, value_type)

		<b>%stable_trie_base</b> の葉は必ず空遷移で終わるが、空遷移ではなく最後の文字に対応するノードを指すイテレータを返す。
		*/
		template <typename Key>
		const_iterator insert(Key const& key, value_type value = 0)
		{
			return insert(std::begin(key), std::end(key), value);
		}

		/*! @brief キー文字列を削除する
		
		@param [in] pos 削除するキー文字列の末尾に対応するノードへのイテレータ

		このメンバは、継承するクラスから呼び出される目的で用意した。
		*/
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

		/*! @brief キー文字列を削除する

		@param [in] first 削除するキー文字列の先頭を指すイテレータ
		@param [in] last  削除するキー文字列の終端を指すイテレータ

		@sa erase(const_iterator pos)
		*/
		template <typename InputIterator>
		void erase(InputIterator first, InputIterator last)
		{
			erase(find(first, last));
		}

		/*! @brief キー文字列を削除する

		@param [in] key 削除するキー文字列

		@sa erase(const_iterator pos)
		*/
		template <typename Key>
		void erase(Key const& key)
		{
			erase(std::begin(key), std::end(key));
		}

		void swap(stable_trie_base& other)
		{
			base_type::swap(other);
		}

		// 検索 ---------------------------------------------------------------

	protected:
		/*! @brief 部分一致検索

		@param [in]  first 検索するキー文字列の先頭を指すイテレータ
		@param [in]  last  検索するキー文字列の終端を指すイテレータ
		@param [out] i     遷移した数（0で初期化されている必要がある）

		@return 一致した最後のノードと次の文字を指すイテレータのペア

		一文字も一致しない場合、cbegin()を返す。

		このメンバは、 wordring::basic_trie::lookup() から使用される意図で用意された。
		仮にラベルが4バイトの場合、3バイト目でマッチしなくなった時、3バイト戻る要求があるため、
		引数 <b>i<b/> を通して遷移した数を返すようにした。
		*/
		template <typename InputIterator>
		auto lookup(InputIterator first, InputIterator last, std::uint32_t& i) const
		{
			assert(i == 0);

			index_type parent = 1;

			while (first != last)
			{
				std::uint16_t ch = static_cast<std::uint8_t>(*first);
				index_type idx = at(parent, ch);
				if (idx == 0) break;
				++first;
				++i;
				parent = idx;
				assert(1 <= parent && parent < limit());
			}

			return std::make_pair(const_iterator(m_c, parent), first);
		}

	public:
		/*! @brief 部分一致検索

		@param [in] first 検索するキー文字列の先頭を指すイテレータ
		@param [in] last  検索するキー文字列の終端を指すイテレータ

		@return 一致した最後のノードと次の文字を指すイテレータのペア

		一文字も一致しない場合、cbegin()を返す。
		*/
		template <typename InputIterator>
		auto lookup(InputIterator first, InputIterator last) const
		{
			std::uint32_t i = 0;
			return lookup(first, last, i);

			/*
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
			*/
		}

		/*! @brief 前方一致検索

		@param [in] key  検索するキー文字列

		@return 一致した最後のノード

		一文字も一致しない場合、cbegin()を返す。
		*/
		template <typename Key>
		const_iterator search(Key const& key) const
		{
			auto it1 = std::begin(key);
			auto it2 = std::end(key);

			auto pair = lookup(it1, it2);

			return (pair.second == it2)
				? pair.first
				: cend();
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
			auto pair = lookup(first, last);

			auto it = pair.second;
			index_type idx = pair.first.m_index;

			return (it == last && has_null(idx))
				? pair.first
				: cend();
		}

		/*! @brief 完全一致検索

		@param [in] key 検索するキー文字列

		@return
			入力されたキー文字列と完全に一致する葉がある場合、そのノードを指すイテレータ。
			それ以外の場合、 cend() 。
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
			auto pair = lookup(first, last);

			auto it = pair.second;
			index_type idx = pair.first.m_index;

			return !(it != last || !has_null(idx));
		}

		/*! @brief キー文字列が格納されているか調べる

		@param [in] key  キー文字列

		@return 格納されている場合 true 、それ以外の場合 false
		*/
		template <typename Key>
		bool contains(Key const& key) const
		{
			return contains(std::begin(key), std::end(key));
		}
	};

	/*! @brief ストリームへ出力する
	
	速度を必要とする場合、使用を推奨しない。
	*/
	template <typename Allocator1>
	inline std::ostream& operator<<(std::ostream& os, stable_trie_base<Allocator1> const& trie)
	{
		typename stable_trie_base<Allocator1>::base_type const& heap = trie;
		return os << heap;
	}

	/*! @brief ストリームから入力する

	速度を必要とする場合、使用を推奨しない。
	*/
	template <typename Allocator1>
	inline std::istream& operator>>(std::istream& is, stable_trie_base<Allocator1>& trie)
	{
		typename stable_trie_base<Allocator1>::base_type& heap = trie;
		return is >> heap;
	}
}
