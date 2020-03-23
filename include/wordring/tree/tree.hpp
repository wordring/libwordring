#pragma once

#include <algorithm>
#include <array>
#include <iterator>
#include <limits>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

namespace wordring
{
	template <typename T, typename Allocator>
	class tree;
}

namespace wordring::detail
{
	// ------------------------------------------------------------------------
	// tree_node
	// ------------------------------------------------------------------------

	template <typename T>
	struct tree_node
	{
		using index_type = std::uint32_t;
		using value_type = T;

		static constexpr index_type null_value = 0;

		index_type m_parent;
		index_type m_prev;
		index_type m_next;
		index_type m_child;

		value_type m_value;
	};

	// ------------------------------------------------------------------------
	// tree_node_iterator
	// ------------------------------------------------------------------------

	template <typename Container>
	class tree_node_iterator
	{
		template <typename T1, typename Allocator1>
		friend class wordring::tree;

		template <typename Container1>
		friend class tree_node_iterator;

		template <typename Container1, typename Container2>
		friend bool operator==(tree_node_iterator<Container1> const&, tree_node_iterator<Container2> const&);

		template <typename Container1, typename Container2>
		friend bool operator!=(tree_node_iterator<Container1> const&, tree_node_iterator<Container2> const&);

	protected:
		using container  = Container;
		using node_type  = typename container::value_type;
		using index_type = typename node_type::index_type;

		static constexpr index_type null_value = node_type::null_value;

	public:
		using value_type        = typename node_type::value_type;
		using difference_type   = std::ptrdiff_t;
		using reference         = value_type&;
		using const_reference   = value_type const&;
		using pointer           = value_type*;
		using const_pointer     = value_type const*;
		using iterator_category = std::bidirectional_iterator_tag;

		using reverse_iterator = std::reverse_iterator<tree_node_iterator<container>>;

	public:
		/*! @brief 空のイテレータを作成する
		*/
		tree_node_iterator() noexcept
			: m_c(nullptr)
			, m_parent(0)
			, m_index(0)
		{
		}

	protected:
		/*! @brief イテレータを作成する
		
		wordring::tree から使用される。
		*/
		tree_node_iterator(container& c, index_type parent, index_type idx) noexcept
			: m_c(std::addressof(c))
			, m_parent(parent)
			, m_index(idx)
		{
		}

	public:
		/*! @brief const_iterator を取得する
		*/
		operator tree_node_iterator<container const>() const
		{
			tree_node_iterator<container const> result;
			result.m_c = m_c;
			result.m_parent = m_parent;
			result.m_index = m_index;
			return result;
		}

		/*! @brief イテレータが指す要素の参照を取得する
		*/
		auto& operator*() const
		{
			assert(m_index != null_value);

			return (m_c->data() + m_index)->m_value;
		}

		/*! @brief イテレータが指す要素のポインタを取得する
		*/
		auto operator->() const
		{
			assert(m_index != null_value);

			return std::addressof((m_c->data() + m_index)->m_value);
		}

		/*! @brief インクリメントする
		*/
		tree_node_iterator& operator++()
		{
			assert(m_index != null_value);

			m_index = (m_c->data() + m_index)->m_next;

			return *this;
		}

		/*! @brief インクリメントする
		*/
		tree_node_iterator operator++(int)
		{
			assert(m_index != null_value);

			auto result = *this;
			operator++();

			return result;
		}
		
		/* @brief デクリメントする
		*/
		tree_node_iterator& operator--()
		{
			auto const* d = m_c->data();

			if (m_index != 0) // prevがある場合(end以外はある)
			{
				assert(m_parent != 0 && (d + m_parent)->m_child != m_index); // 先頭をデクリメントしようとしている。
				m_index = (d + m_index)->m_prev;
			}
			else if (m_parent != 0) // prevが無い場合(endは無い)
			{
				index_type child = (d + m_parent)->m_child; // begin
				m_index = (d + child)->m_prev; // endの前はtail
			}
			else // 根の終端
			{
				m_index = d->m_child;
				assert(m_index != 0); // 空のツリーで根をデクリメントしようとしている。
			}

			return *this;
		}

		/* @brief デクリメントする
		*/
		tree_node_iterator operator--(int)
		{
			auto result = *this;
			operator--();
			return result;
		}

		/*! @brief 子の先頭を指すイテレータを取得する
		*/
		tree_node_iterator begin() const
		{
			assert(m_index != null_value);
			return tree_node_iterator(*m_c, m_index, (m_c->data() + m_index)->m_child);
		}

		/*! @brief 子の終端を指すイテレータを取得する
		*/
		tree_node_iterator end() const
		{
			assert(m_index != null_value);
			return tree_node_iterator(*m_c, m_index, null_value);
		}

		/*! @brief 子の末尾を指す逆走イテレータを取得する
		*/
		reverse_iterator rbegin() const
		{
			return std::make_reverse_iterator(end());
		}

		/*! @brief 子の先頭の前を指す逆走イテレータを取得する
		*/
		reverse_iterator rend() const
		{
			return std::make_reverse_iterator(begin());
		}

		tree_node_iterator parent() const
		{
			assert(m_index != null_value);

			auto* h = m_c->data();

			index_type p = (h + m_index)->m_parent;                             // 親
			index_type pp = (p == null_value) ? null_value : (h + p)->m_parent; // 親の親

			return tree_node_iterator(*m_c, pp, p);
		}

	protected:
		container* m_c;
		index_type m_parent;
		index_type m_index;
	};

	template <typename Container1, typename Container2>
	inline bool operator==(tree_node_iterator<Container1> const& lhs, tree_node_iterator<Container2> const& rhs)
	{
		assert(const_cast<Container1 const*>(lhs.m_c) == const_cast<Container2 const*>(rhs.m_c));
		return !(lhs != rhs);
	}

	template <typename Container1, typename Container2>
	inline bool operator!=(tree_node_iterator<Container1> const& lhs, tree_node_iterator<Container2> const& rhs)
	{
		assert(const_cast<Container1 const*>(lhs.m_c) == const_cast<Container2 const*>(rhs.m_c));
		return lhs.m_index != rhs.m_index || lhs.m_parent != rhs.m_parent;
	}
}

namespace wordring
{
	// ------------------------------------------------------------------------
	// tree
	// ------------------------------------------------------------------------

	/*! @class tree tree.hpp wordring/tree/tree.hpp

	@brief 木コンテナ

	@par 木

	根以外のノードは一つの親を持つ。
	ノードは0あるいはそれ以上の子を持つ。
	子を持たないノードを葉と呼ぶ。
	ノードは0以上の兄弟を持つ。

	@image html tree_concept.svg

	@par イテレータ

	<b>tree</b> において要素間の移動はイテレータを使う。
	<b>tree</b> のメンバ関数 <b>begin()</b> で根を指すイテレータを取得する。

	イテレータのメンバ関数 <b>%parent()</b> で親を取得する。
	イテレータのメンバ関数 <b>%begin()</b> で先頭の子を取得する。
	イテレータのメンバ関数 <b>%end()</b> で子の終端を取得する。
	イテレータのメンバ関数 <b>%operator++()</b> で次の兄弟を取得する。

	@image html tree_node_iterator_concept.svg

	@internal
	<hr>
	@par 木ノード

	<b>%tree</b> の内部は、一つの配列にすべてのノードが収まっている。
	各ノードは、親、子の先頭、前の兄弟、次の兄弟の各インデックスを持つ。
	配列のインデックスは <b>1</b> から始まる。
	（もしあるなら）根のインデックスは <b>1</b> 。

	子の先頭の <b>m_prev</b> には最後の兄弟のインデックスを格納する。
	先頭の子であることを確認するには、親の <b>m_child</b> と自身が一致するか調べる。

	イテレータは、指すノードのインデックスと親のインデックスを保持する。
	終端を指すイテレータは、インデックスを <b>0</b> として示す。
	終端を指すイテレータも（デクリメントで必要となるため）親のインデックスを保持する。
	根の親は <b>0</b> である。
	終端を指すイテレータをデクリメントする場合、親の <b>m_child</b> の <b>m_prev</b> へ
	移動する。

	@image html tree_node_concept.svg

	- @ref detail::tree_node
	- @ref detail::tree_node_iterator

	@par インデックス <b>0</b>

	配列のインデックス <b>0</b> は、ノードとして使用されない。
	そのため、未使用ノードの先頭、格納するノード数を保持するのに使う。

	@par 未使用ノード

	未使用ノードは、 <b>m_prev</b> に前の未使用ノード、 <b>m_next</b> に次の未使用ノードの
	各インデックスを格納して双方向リンクリストとする。

	未使用ノード末尾は、 <b>m_next</b> に <b>0</b> を格納して示す。
	未使用ノードが一つも無い場合、インデックス <b>0</b> の <b>m_next</b> に <b>0</b> を格納して示す。

	現在の実装は、インデックス順に未使用ノードのリンクが並ぶよう、解放時に整列させる。
	この動作によって検索性能が向上するが、解放時間が増える。

	@image html tree_node_free.svg

	*/
	template <typename T, typename Allocator = std::allocator<detail::tree_node<T>>>
	class tree
	{
		template <typename Container1>
		friend class detail::tree_node_iterator;

	protected:
		using node_type  = detail::tree_node<T>;
		using index_type = typename node_type::index_type;
		using container  = std::vector<node_type, Allocator>;

		static constexpr index_type null_value = node_type::null_value;

	public:
		// メンバ型
		using value_type      = T;
		using allocator_type  = Allocator;
		using size_type       = std::size_t;
		using difference_type = std::ptrdiff_t;
		using reference       = value_type&;
		using const_reference = value_type const&;
		using pointer         = value_type*;
		using const_pointer   = value_type const*;
		using iterator        = detail::tree_node_iterator<container>;
		using const_iterator  = detail::tree_node_iterator<container const>;

	public:
		/*! @brief 空のコンテナを構築する
		*/
		tree()
			: m_c(1, node_type{})
		{
		}

		/*! @brief アロケータを指定して空のコンテナを構築する

		@param [in]	alloc アロケータ
		*/
		explicit tree(allocator_type const& alloc)
			: m_c(1, node_type{}, alloc)
		{
		}

		/*! @brief 要素からコンテナを構築する

		@param [in] value 根となる要素
		@param [in]	alloc アロケータ
		*/
		explicit tree(value_type const& value, allocator_type const& alloc = allocator_type())
			: m_c(1, node_type{}, alloc)
		{
			insert(begin(), value);
		}

		/*! @brief 要素からコンテナを構築する

		@param [in] value 根となる要素
		@param [in]	alloc アロケータ
		*/
		explicit tree(value_type&& value, allocator_type const& alloc = allocator_type())
			: m_c(1, node_type{}, alloc)
		{
			insert(begin(), std::move(value));
		}

		void assign(value_type const& value)
		{
			clear();
			insert(begin(), value);
		}

		void assign(value_type&& value)
		{
			clear();
			insert(begin(), std::move(value));
		}

		allocator_type get_allocator() const { return m_c.get_allocator(); }

		// 要素アクセス --------------------------------------------------------

		reference front()
		{
			assert(!empty());
			return *begin();
		}

		const_reference front() const
		{
			assert(!empty());
			return *cbegin();
		}

		reference back() { return front(); }

		const_reference back() const { return front(); }

		// イテレータ ----------------------------------------------------------

		/*! @brief 根を指すイテレータを返す

		@return 根を指すイテレータ
		*/
		iterator begin() noexcept { return iterator(m_c, 0, empty() ? 0 : 1); }

		/*! @brief 根を指すイテレータを返す

		@return 根を指すイテレータ
		*/
		const_iterator begin() const noexcept { return const_iterator(m_c, 0, empty() ? 0 : 1); }

		/*! @brief 根を指すイテレータを返す

		@return 根を指すイテレータ
		*/
		const_iterator cbegin() const noexcept { return begin(); }

		/*! @brief 根の終端を指すイテレータを返す

		@return 根の終端を指すイテレータ
		*/
		iterator end() noexcept { return iterator(m_c, 0, 0); }

		/*! @brief 根の終端を指すイテレータを返す

		@return 根の終端を指すイテレータ
		*/
		const_iterator end() const noexcept { return const_iterator(m_c, 0, 0); }

		/*! @brief 根の終端を指すイテレータを返す

		@return 根の終端を指すイテレータ
		*/
		const_iterator cend() const noexcept { return end(); }

		// 容量 ---------------------------------------------------------------

		/*! @brief 要素を格納していないことを調べる
		
		@return 要素を格納していない場合 true 、それ以外の場合 false
		*/
		bool empty() const noexcept { return size() == 0; }

		/*! @brief 格納している要素数を調べる

		@return コンテナに格納されている要素の数
		*/
		size_type size() const noexcept { return m_c.front().m_parent; }

		/*! @brief 格納可能な要素の最大数を調べる

		@return 格納可能な要素の最大数
		*/
		size_type max_size() const noexcept { return m_c.max_size(); }

		// 変更 ---------------------------------------------------------------

		/*! @brief すべての要素を削除する
		*/
		void clear() noexcept
		{
			m_c.clear();
			m_c.emplace_back(node_type{});
		}

		void swap(tree& other) { m_c.swap(other.m_c); }

		/*! @brief 要素を挿入する

		@param [in] pos   挿入位置を指すイテレータ
		@param [in] value 挿入する値

		@return 挿入された要素を指すイテレータ

		@sa iterator insert(const_iterator pos, value_type&& value)
		*/
		iterator insert(const_iterator pos, value_type const& value)
		{
			return insert(pos, value_type(value));
		}

		/*! @brief ノードを挿入する

		@param [in] pos   挿入位置を指すイテレータ
		@param [in] value 挿入する値

		@return 挿入されたノードを指すイテレータ

		挿入位置は、引数 <b>pos</b> の前。
		子として追加されるのではないことに注意。

		既存のノードが有る状態で根を挿入しようとした場合、何もせず end() を返す。
		この場合、debug実行ではアサーションで失敗する。

		@internal
		<hr>

		<b>idx</b> は、allocate() で取得した新規ノードのインデックス。
		
		<b>parent</b> は、引数 <b>pos</b> から取得する。

		<b>child</b> は、 <b>parent</b> の <b>m_child</b> 。

		<b>after</b> は、（挿入後にAfterとなるので）挿入位置のインデックス。
		end() に挿入する場合、 <b>0</b> 。

		<b>before</b> は
		- <b>after</b> が有り
			- 且つ <b>parent</b> の <b>m_child</b> が <b>after</b> でない場合、<b>after</b> の <b>m_prev</b> 
			- それ以外の場合、 <b>0</b>
		- <b>after</b> が無く
			- <b>child</b> が有る場合、 <b>child</b> の <b>m_prev</b>
			- それ以外の場合、 <b>0</b>
		.

		@par 根を挿入する場合

		インデックス <b>0</b> の前に挿入。

		- idx == 1
		- before == 無し
		- after == 無し

		根は一つしか無い。
		挿入前に格納数は <b>0</b> でなければならない。

		@image html tree_node_insert_1.svg

		@par ノードの間に挿入する場合

		インデックス <b>3</b> の前に挿入。
		- idx == 4
		- before == 2
		- after == 3

		@image html tree_node_insert_2.svg

		@par 子が無い場合の挿入

		インデックス <b>0</b> の前に挿入。
		- idx == 2
		- before == 無し
		- after == 無し

		@image html tree_node_insert_3.svg

		@par 先頭の前に挿入

		インデックス <b>2</b> の前に挿入。
		- idx == 4
		- before == 無し
		- after == 2

		@image html tree_node_insert_4.svg
		
		@par 単独の子の先頭の前に挿入

		インデックス <b>2</b> の前に挿入。
		- idx == 3
		- before == 無し
		- after == 2

		@image html tree_node_insert_5.svg

		@par 終端の前に挿入

		インデックス <b>0</b> の前に挿入。
		- idx == 4
		- before == 3
		- after == 無し

		@image html tree_node_insert_6.svg

		@par 単独の子の終端の前に挿入

		インデックス <b>0</b> の前に挿入。
		- idx == 3
		- before == 2
		- after == 無し

		@image html tree_node_insert_7.svg

		<hr>

		@par before が無い場合

		- 親の <b>m_child</b> を新規挿入されるノードとする。
		- <b>after</b> が有る場合
			- <b>after</b> の <b>m_prev</b> には <b>tail</b> のインデックスが入っている。
		- <b>after</b> が無い場合
			-  <b>tail</b> は新規挿入されるノード自身である。
		- 新規挿入されるノードの <b>m_prev</b> を <b>tail</b> とする。

		@par before が有る場合

		- <b>before</b> の <b>m_next</b> を新規挿入されるノードとする。

		@par after が無い場合

		- 新規挿入されるノードの <b>m_next</b> を <b>0</b> とする。
		- <b>before</b> が有る場合
			- 親の <b>m_child</b> には先頭のインデックスが入っている。
			- 先頭の <b>m_prev</b> を新規挿入されるノードとする。
		.

		@par after が有る場合

		-  <b>after</b> の <b>m_prev</b> を新規挿入されるノードとする。

		*/
		iterator insert(const_iterator pos, value_type&& value)
		{
			index_type parent = pos.m_parent;

			assert(!(parent == 0 && m_c.size() != 1)); // 根に複数の要素を追加しようとしている。
			if (parent == 0 && m_c.size() != 1) return end();

			index_type idx = allocate(std::move(value));

			node_type* d = m_c.data();

			index_type child  = (d + parent)->m_child;
			index_type after  = pos.m_index;
			index_type before = 0;
			if (after != 0) before = (child != after) ? (d + after)->m_prev : 0;
			else before = (child != 0) ? (d + child)->m_prev : 0;

			if (before == 0)
			{
				(d + parent)->m_child = idx;
				index_type tail = (after != 0) ? (d + after)->m_prev : idx;
				(d + idx)->m_prev = tail;
			}
			else
			{
				(d + before)->m_next = idx;
				(d + idx)->m_prev = before;
			}

			if (after == 0)
			{
				(d + idx)->m_next = 0;
				if (before != 0) (d + child)->m_prev = idx;
			}
			else
			{
				(d + after)->m_prev = idx;
				(d + idx)->m_next = after;
			}
			(d + idx)->m_parent = parent;

			++d->m_parent;

			return iterator(m_c, parent, idx);
		}

		/*! @brief 部分木を挿入する
		
		@param [in] pos 挿入位置を指すイテレータ
		@param [in] sub 部分木の根を指すイテレータ

		@return posを返す

		@internal
		<hr>

		@par 二つのスタックを使って部分木をコピーするアルゴリズム

		スタックを使った一般的な木探索アルゴリズムは、木の構造を保持しないため、
		POPしたノードを「どこに挿入するか？」という情報を取り出せない。
		そこで、挿入地点を記録する「<b>親スタック</b>」を使う。

		<b>探索スタック</b>から値をPOPするとき、子が有ればすべて<b>探索スタック</b>にPUSHする。
		同時に、親を<b>親スタック</b>にPUSHする。
		<b>親スタック</b>にPUSHするとき、スタックTOPが挿入しようとするノードの親であるか検査する。
		親でない場合、POPしてからノードをPUSHする。

		以上の操作で、<b>親スタック</b>のTOPは常に挿入地点となる。

		ところで、以上のアルゴリズムは、引数の部分木について親を記録する。
		実際に必要なのは、挿入先の親である。
		そこで、<b>親スタック</b>には、挿入先の親も一緒に記録する。

		@image html tree_node_insert_8.svg

		@par 実装

		- 根の挿入位置は <b>pos</b> だが、それ以外は親スタックTOPの <b>%end()</b> 。
		  従って、根だけ特別扱いが必要。
		.

		@todo スタックはインデックスが有れば十分で、イテレータを入れておくのは冗長。
			  今後、性能を改良するときに考慮する必要がある。
		*/
		iterator insert(const_iterator pos, const_iterator sub)
		{
			std::vector<std::array<const_iterator, 2>> x; // 親スタック [pos, sub]
			std::vector<const_iterator> v;                // 探索スタック

			// 根を特別扱い
			{
				iterator it = insert(pos, *sub);
				x.push_back({ it, sub });
				auto it1 = sub.rbegin();
				auto it2 = sub.rend();
				while (it1 != it2) v.push_back((++it1).base());
			}

			// 根以外
			while (!v.empty())
			{
				const_iterator it = v.back();
				v.pop_back();

				if (it.parent() != x.back()[1]) x.pop_back();
				const_iterator p = insert(x.back()[0].end(), *it);

				if (it.begin() != it.end()) // 子がある
				{
					auto it1 = it.rbegin();
					auto it2 = it.rend();
					while(it1 != it2)  v.push_back((++it1).base());

					x.push_back({ p, it });
				}
			}

			return iterator(m_c, pos.m_parent, pos.m_index);
		}

		template <typename... Args>
		iterator emplace(const_iterator pos, Args&&... args)
		{
			return insert(pos, value_type(std::forward<Args>(args)...));
		}

		/*! @brief 要素を削除する
		
		@param [in] pos 削除する要素を指すイテレータ
		
		@return 削除された要素の次を指すイテレータ

		@internal
		<hr>

		@par 根を削除

		- idx == 1
		- before == 0
		- after == 0

		@image html tree_node_erase_1.svg

		@par ノードの間を削除

		- idx == 4
		- before == 2
		- after == 3

		@image html tree_node_erase_2.svg

		@par 先頭の前を削除

		- idx == 4
		- before == 0
		- after == 2

		@image html tree_node_erase_4.svg

		@par 削除で単独になる子の前を削除

		- idx == 4
		- before == 0
		- after == 2

		@image html tree_node_erase_5.svg

		@par 終端の前を削除

		- idx == 4
		- before == 3
		- after == 0

		@image html tree_node_erase_6.svg

		@par 削除で単独になる子の終端の前を削除

		- idx == 3
		- before == 2
		- after == 0

		@image html tree_node_erase_7.svg

		<hr>

		- <b>parent</b> 、 <b>idx</b> は、 <b>pos</b> から取得する。
		- <b>child</b> は、 <b>parent</b> の <b>m_child</b> 。
		- <b>beore</b> は、 <b>idx</b> の <b>m_prev</b> 。
		- <b>child</b> が <b>idx</b> と一致する場合、<b>before</b> は無い。
		- <b>after</b> は、 <b>idx</b> の <b>m_next</b> 。

		子孫の解放は、リンクの付け替えが発生しないので、単に開放するのみである。

		<hr>

		- <b>before</b> が無い場合
			- 削除後、 <b>after</b> が <b>先頭</b> になる
				-  <b>after</b> が有る場合
					- <b>parent</b> の <b>m_child</b> に <b>after</b> を設定する。
					- <b>after</b> の <b>m_prev</b> に <b>idx</b> の <b>m_prev</b> 
					 （ココには今、末尾のインデックスが入っている）を設定する。
				-  <b>after</b> が無い場合、 <b>parent</b> の <b>m_child</b> に <b>0</b> を設定する。
		- <b>before</b> が有る場合
			- <b>before</b> の <b>m_next</b> に <b>after</b> を設定する。
			  <b>after</b> が無い場合、末尾を表す <b>0</b> なので、そのまま設定できる。
		- <b>after</b> が無い場合
			- 削除後、 <b>before</b> が <b>末尾</b> になる
				- <b>before</b> が有る（つまり兄弟がある）場合、 <b>parent</b> の <b>m_child</b>
				  （つまり <b>child</b> ）に <b>before</b> を設定する。
		- <b>after</b> が有る場合
			- <b>after</b> の <b>m_prev</b> に <b>idx</b> の <b>m_prev</b> を設定する。
		.
		*/
		iterator erase(const_iterator pos)
		{
			index_type idx = pos.m_index;
			assert(idx != 0);
			index_type parent = pos.m_parent;

			node_type* d = m_c.data();
			index_type child = (d + parent)->m_child;
			index_type before = (child != idx) ? (d + idx)->m_prev : 0;
			index_type after = (d + idx)->m_next;

			if (before == 0)
			{
				if (after == 0) (d + parent)->m_child = 0;
				else
				{
					(d + parent)->m_child = after;
					(d + after)->m_prev = (d + idx)->m_prev;
				}
			}
			else (d + before)->m_next = after;

			if (after == 0)
			{
				if (before != 0) (d + child)->m_prev = before;
			}
			else (d + after)->m_prev = (d + idx)->m_prev;

			// 解放
			std::vector<index_type> v(1, idx);
			while (!v.empty())
			{
				index_type i = v.back();
				v.pop_back();
				for (index_type j = (d + i)->m_child; j != 0; j = (d + j)->m_next) v.push_back(j);

				--d->m_parent;
				free(i);
			}

			return iterator(m_c, parent, after);
		}

	protected:
		// 内部 ---------------------------------------------------------------

		/*! @brief ノードを確保する

		@param [in] val 格納する値

		@return 確保されたノードのインデックス値
		*/
		index_type allocate(value_type&& val)
		{
			node_type* d = m_c.data();
			index_type idx = d->m_next;

			if (idx == 0)
			{
				idx = m_c.size();
				m_c.emplace_back(node_type{ 0, 0, 0, 0, std::move(val) });
			}
			else
			{
				index_type before = (d + idx)->m_prev;
				index_type after = (d + idx)->m_next;
				(d + idx)->m_value = std::move(val);

				(d + before)->m_next = after;
				(d + after)->m_prev = before;
			}

			return idx;
		}

		/*! @brief ノードを解放する

		@param [in] idx 解放するノードのインデックス
		*/
		void free(index_type idx)
		{
			assert(idx < m_c.size());

			node_type* d = m_c.data();

			// idx を超えない最後の未使用ノードを探す
			index_type before = 0;
			for (index_type i = d->m_next; i != 0 && i < idx; i = (d + before)->m_next) before = i;

			// idx を超える最初の未使用ノードを探す
			index_type after = (d + before)->m_next;

			// リンクを張り替える
			(d + before)->m_next = idx;

			(d + idx)->m_prev = before;
			(d + idx)->m_next = after;

			(d + after)->m_prev = idx;

			// 使わない項目を0に初期化
			(d + idx)->m_parent = 0;
			(d + idx)->m_child = 0;
		}

	protected:
		container  m_c;
	};
}
