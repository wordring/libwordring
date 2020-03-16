#pragma once

#include <algorithm>
#include <deque>
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
	// tree_node_proxy
	// ------------------------------------------------------------------------

	/*! 初期化子リスト挿入時、ツリー階層を解決するために、コンパイラから使用される
	*/
	template <typename T>
	struct tree_node_proxy
	{
		T m_value;
		std::vector<tree_node_proxy<T>> m_children;

		tree_node_proxy(T const& value)
			: m_value(value)
			, m_children()
		{
		}

		tree_node_proxy(T const& value, std::initializer_list<T> children)
			: m_value(value)
			, m_children(children.begin(), children.end())
		{
		}

		tree_node_proxy(T const& value, std::initializer_list<tree_node_proxy<T>> children)
			: m_value(value)
			, m_children(children)
		{
		}
	};

	// ------------------------------------------------------------------------
	// tree_node_iterator
	// ------------------------------------------------------------------------

	template <typename T, typename Container>
	class tree_node_iterator
	{
		template <typename T1, typename Allocator1>
		friend class wordring::tree;

		friend class tree_node_iterator<std::remove_const_t<T>, std::remove_const_t<Container>>;
		friend class tree_node_iterator<std::add_const_t<T>, std::add_const_t<Container>>;

		template <typename T1, typename Container1, typename T2, typename Container2>
		friend bool operator==(tree_node_iterator<T1, Container1> const&, tree_node_iterator<T2, Container2> const&);

		template <typename T1, typename Container1, typename T2, typename Container2>
		friend bool operator!=(tree_node_iterator<T1, Container1> const&, tree_node_iterator<T2, Container2> const&);

	public:
		using value_type = T;
		using difference_type = std::ptrdiff_t;
		using reference = value_type&;
		using pointer = value_type*;
		using iterator_category = std::bidirectional_iterator_tag;

	protected:
		using node_type  = tree_node<T>;
		using index_type = typename node_type::index_type;
		using container  = Container;

		static constexpr index_type null_value = node_type::null_value;

	public:
		tree_node_iterator(container& c, index_type parent, index_type idx) noexcept
			: m_c(std::addressof(c))
			, m_parent(parent)
			, m_index(idx)
		{
		}

		/* iteratorからconst_iteratorを作成する
		*/
		template <typename Iterator>
		tree_node_iterator(Iterator const& other) noexcept
			: m_c(other.m_c)
			, m_parent(other.m_parent)
			, m_index(other.m_index)
		{
		}

		template <typename Iterator>
		tree_node_iterator& operator=(Iterator const& other) noexcept
		{
			m_c = other.m_c;
			m_parent = other.m_parent;
			m_index = other.m_index;

			return *this;
		}

		reference operator*() const
		{
			assert(m_index != null_value);

			return (m_c->m_c.data() + m_index)->m_value;
		}

		pointer operator->() const { return std::addressof(operator*()); }

		tree_node_iterator& operator++()
		{
			assert(m_index != null_value);

			m_index = (m_c->m_c.data() + m_index)->m_next;

			return *this;
		}

		tree_node_iterator operator++(int)
		{
			assert(m_index != null_value);

			auto result = *this;
			operator++();

			return result;
		}

		tree_node_iterator& operator--()
		{
			auto* h = m_c->m_c.data();

			if (m_index != null_value) // prevがある場合(end以外はある)
			{
				assert(m_parent != null_value && (h + m_parent)->m_child != m_index); // 先頭をデクリメントしようとしている。
				m_index = (h + m_index)->m_prev;
			}
			else if (m_parent != null_value) // prevが無い場合(endは無い)
			{
				index_type child = (h + m_parent)->m_child; // begin
				m_index = (h + child)->m_prev; // endの前はtail
			}
			else
			{
				assert(!m_c->empty()); // 空のツリーで根をデクリメントしようとしている。
				m_index = m_c->m_root; // 親が無い場合(rootは親が無い)
			}

			return *this;
		}

		tree_node_iterator operator--(int)
		{
			auto result = *this;

			operator--();

			return result;
		}

		tree_node_iterator begin() const
		{
			assert(m_index != null_value);
			return tree_node_iterator(*m_c, m_index, (m_c->m_c.data() + m_index)->m_child);
		}

		tree_node_iterator end() const
		{
			assert(m_index != null_value);
			return tree_node_iterator(*m_c, m_index, null_value);
		}

		tree_node_iterator parent() const
		{
			assert(m_index != null_value);

			auto* h = m_c->m_c.data();

			index_type p = (h + m_index)->m_parent;                             // 親
			index_type pp = (p == null_value) ? null_value : (h + p)->m_parent; // 親の親

			return tree_node_iterator(*m_c, pp, p);
		}

	protected:
		container* m_c;
		index_type m_parent;
		index_type m_index;
	};

	template <typename T1, typename Container1, typename T2, typename Container2>
	inline bool operator==(tree_node_iterator<T1, Container1> const& lhs, tree_node_iterator<T2, Container2> const& rhs)
	{
		assert(const_cast<Container1 const*>(lhs.m_c) == const_cast<Container2 const*>(rhs.m_c));
		return !(lhs != rhs);
	}

	template <typename T1, typename Container1, typename T2, typename Container2>
	inline bool operator!=(tree_node_iterator<T1, Container1> const& lhs, tree_node_iterator<T2, Container2> const& rhs)
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

	イテレータのメンバ関数 <b>parent()</b> で親を取得する。
	イテレータのメンバ関数 <b>begin()</b> で先頭の子を取得する。
	イテレータのメンバ関数 <b>end()</b> で子の終端を取得する。
	イテレータのメンバ関数 <b>operator++()</b> で次の兄弟を取得する。

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
		template <typename T1, typename Container1>
		friend class detail::tree_node_iterator;

	protected:
		using tree_type = tree<T, Allocator>;
		using node_type = detail::tree_node<T>;
		using index_type = typename node_type::index_type;
		using node_proxy = detail::tree_node_proxy<T>;
		using container = std::vector<node_type, Allocator>;

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
		using iterator        = detail::tree_node_iterator<value_type, container>;
		using const_iterator  = detail::tree_node_iterator<value_type const, container const>;

	public:
		tree()
			: m_c(1, node_type{})
		{
		}

		explicit tree(allocator_type const& alloc)
			: m_c(1, node_type{}, alloc)
		{
		}

		/*! 要素から構築する
		*/
		explicit tree(value_type const& value, allocator_type const& alloc = allocator_type())
			: m_c(1, node_type{}, alloc)
		{
			insert(begin(), value);
		}

		/*! 要素から構築する
		*/
		explicit tree(value_type&& value, allocator_type const& alloc = allocator_type())
			: m_c(1, node_type{}, alloc)
		{
			insert(begin(), std::move(value));
		}

		explicit tree(node_proxy proxy, allocator_type const& alloc = allocator_type())
			: m_c(1, node_type{}, alloc)
		{
			insert(begin(), proxy);
		}

		//tree& operator=(tree const&) = default;

		//tree& operator=(tree&&) noexcept = default;

		tree& operator=(node_proxy proxy)
		{
			clear();
			insert(begin(), proxy);
			return *this;
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

		void assign(node_proxy proxy)
		{
			clear();
			insert(begin(), proxy);
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

		iterator begin() noexcept { return iterator(m_c, 0, empty() ? 0 : 1); }

		const_iterator begin() const noexcept { return iterator(m_c, 0, empty() ? 0 : 1); }

		const_iterator cbegin() const noexcept { return begin(); }

		iterator end() noexcept { return iterator(m_c, 0, 0); }

		const_iterator end() const noexcept { return const_iterator(m_c, 0, 0); }

		const_iterator cend() const noexcept { return end(); }

		// 容量 ---------------------------------------------------------------

		bool empty() const noexcept { return size() == 0; }

		size_type size() const noexcept { return m_c.front().m_parent; }

		size_type max_size() const noexcept { return m_c.max_size(); }

		// 変更 ---------------------------------------------------------------

		void clear() noexcept
		{
			m_c.clear();
			m_c.emplace_back(node_type{});
		}

		/*! @brief ノードを挿入する

		@param [in] pos   挿入位置を指すイテレータ
		@param [in] value 挿入する値

		@return 挿入されたノードを指すイテレータ

		@sa iterator insert(const_iterator pos, value_type&& value)
		*/
		iterator insert(const_iterator pos, value_type const& value)
		{
			return allocate(pos, value_type(value));
		}

		/*! @brief ノードを挿入する

		@param [in] pos   挿入位置を指すイテレータ
		@param [in] value 挿入する値

		@return 挿入されたノードを指すイテレータ

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

			assert(!(parent == 0 && m_c.size() != 1));
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

			++d->m_parent;

			return iterator(m_c, parent, idx);
		}

		iterator insert(const_iterator pos, size_type count, value_type const& value)
		{
			iterator result(*this, pos.m_parent, pos.m_index);

			if (count != 0) result = allocate(pos, value);
			for (size_t i = 1; i < count; i++) allocate(pos, value);

			return result;
		}

		template <typename InputIterator>
		iterator insert(const_iterator pos, InputIterator first, InputIterator last)
		{
			iterator result(*this, pos.m_parent, pos.m_index);

			if (first != last) pos = result = allocate(pos, *first++);
			while (first != last) pos = insert(++pos, *first++);

			return result;
		}

		iterator insert(const_iterator pos, detail::tree_node_proxy<T> proxy)
		{
			iterator result = insert(pos, proxy.m_value);

			auto it1 = proxy.m_children.cbegin();
			auto it2 = proxy.m_children.cend();
			auto it3 = result.end();
			while (it1 != it2)
			{
				it3 = insert(it3, *it1++);
				++it3;
			}

			return result;
		}

		iterator insert(const_iterator pos, std::initializer_list<value_type> ilist)
		{
			return insert(pos, ilist.begin(), ilist.end());
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

		- <b>lead</b> は、<b>idx</b> の <b>m_child</b> とする。
		  これは、子孫を解放するために使われる。
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

			iterator result(m_c, parent, after);

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

			return result;
		}

		/*! @brief 要素を削除する

		@param [in] first 削除する要素範囲の先頭を指すイテレータ
		@param [in] last  削除する要素範囲の終端を指すイテレータ

		@return 削除された要素の次を指すイテレータ

		@sa iterator erase(const_iterator pos)
		*/
		iterator erase(const_iterator first, const_iterator last)
		{
			assert(first.m_parent == last.m_parent);

			while (first != last) erase(first++);

			return iterator(*this, first.m_parent, first.m_index);
		}

	protected:
		// 内部 ---------------------------------------------------------------

		/*! @brief ノードを確保する

		@param [in] val 格納する値

		@return 確保されたノードのインデックス値

		@sa index_type allocate(value_type&& val)
		*/
		index_type allocate(value_type const& val = value_type())
		{
			return allocate(value_type(val));
		}

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

		/*! @brief ノードを開放する

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
