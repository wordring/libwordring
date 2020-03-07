#pragma once

#include <algorithm>
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
		using index_type = uint32_t;
		using value_type = T;

		static constexpr index_type null_value = std::numeric_limits<index_type>::max();

		index_type m_parent;
		index_type m_child;
		index_type m_prev;
		index_type m_next;
		value_type m_value;

		tree_node()
			: m_parent(null_value)
			, m_child(null_value)
			, m_prev(null_value)
			, m_next(null_value)
			, m_value()
		{
		}

		tree_node(value_type&& value)
			: m_parent(null_value)
			, m_child(null_value)
			, m_prev(null_value)
			, m_next(null_value)
			, m_value(std::move(value))
		{
		}

		tree_node(index_type parent, index_type child, index_type prev, index_type next, value_type const& value = value_type())
			: m_parent(parent)
			, m_child(child)
			, m_prev(prev)
			, m_next(next)
			, m_value(value)
		{
		}
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
	// basic_tree_node_iterator
	// ------------------------------------------------------------------------

	template <typename T, typename Tree>
	class basic_tree_node_iterator
	{
		template <typename T1, typename Allocator1>
		friend class wordring::tree;

		friend class basic_tree_node_iterator<std::remove_const_t<T>, std::remove_const_t<Tree>>;
		friend class basic_tree_node_iterator<std::add_const_t<T>, std::add_const_t<Tree>>;

		template <typename T1, typename Tree1, typename T2, typename Tree2>
		friend bool operator==(basic_tree_node_iterator<T1, Tree1> const& lhs, basic_tree_node_iterator<T2, Tree2> const& rhs);

		template <typename T1, typename Tree1, typename T2, typename Tree2>
		friend bool operator!=(basic_tree_node_iterator<T1, Tree1> const& lhs, basic_tree_node_iterator<T2, Tree2> const& rhs);

	public:
		using value_type        = T;
		using difference_type   = std::ptrdiff_t;
		using reference         = value_type&;
		using pointer           = value_type*;
		using iterator_category = std::bidirectional_iterator_tag;
		
	protected:
		using node_type = tree_node<T>;
		using index_type = typename node_type::index_type;
		using tree_type = Tree;

		static constexpr index_type null_value = node_type::null_value;

	public:
		basic_tree_node_iterator(tree_type& tree, index_type parent, index_type index) noexcept
			: m_tree(std::addressof(tree))
			, m_parent(parent)
			, m_index(index)
		{
		}

		/* iteratorからconst_iteratorを作成する
		*/
		template <typename Iterator>
		basic_tree_node_iterator(Iterator const& other) noexcept
			: m_tree(other.m_tree)
			, m_parent(other.m_parent)
			, m_index(other.m_index)
		{
		}

		basic_tree_node_iterator(basic_tree_node_iterator const& rhs) noexcept = default;

		template <typename Iterator>
		basic_tree_node_iterator& operator=(Iterator const& other) noexcept
		{
			m_tree = other.m_tree;
			m_parent = other.m_parent;
			m_index = other.m_index;

			return *this;
		}

		basic_tree_node_iterator& operator=(basic_tree_node_iterator const& rhs) noexcept = default;

		reference operator*() const
		{
			assert(m_index != null_value);

			return (m_tree->m_heap.data() + m_index)->m_value;
		}

		pointer operator->() const { return std::addressof(operator*()); }

		basic_tree_node_iterator& operator++()
		{
			assert(m_index != null_value);

			m_index = (m_tree->m_heap.data() + m_index)->m_next;

			return *this;
		}

		basic_tree_node_iterator operator++(int)
		{
			assert(m_index != null_value);

			auto result = *this;
			operator++();

			return result;
		}

		basic_tree_node_iterator& operator--()
		{
			auto* h = m_tree->m_heap.data();

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
				assert(!m_tree->empty()); // 空のツリーで根をデクリメントしようとしている。
				m_index = m_tree->m_root; // 親が無い場合(rootは親が無い)
			}

			return *this;
		}

		basic_tree_node_iterator operator--(int)
		{
			auto result = *this;

			operator--();

			return result;
		}

		basic_tree_node_iterator begin() const
		{
			assert(m_index != null_value);
			return basic_tree_node_iterator(*m_tree, m_index, (m_tree->m_heap.data() + m_index)->m_child);
		}

		basic_tree_node_iterator end() const
		{
			assert(m_index != null_value);
			return basic_tree_node_iterator(*m_tree, m_index, null_value);
		}

		basic_tree_node_iterator parent() const
		{
			assert(m_index != null_value);

			auto* h = m_tree->m_heap.data();

			index_type p = (h + m_index)->m_parent;                             // 親
			index_type pp = (p == null_value) ? null_value : (h + p)->m_parent; // 親の親

			return basic_tree_node_iterator(*m_tree, pp, p);
		}

	protected:
		auto heap() { return m_tree->m_heap; }

		auto heap() const { return m_tree->m_heap; }

	protected:
		tree_type* m_tree;
		index_type m_parent;
		index_type m_index;
	};

	template <typename T1, typename Tree1, typename T2, typename Tree2>
	inline bool operator==(basic_tree_node_iterator<T1, Tree1> const& lhs, basic_tree_node_iterator<T2, Tree2> const& rhs)
	{
		assert(const_cast<Tree1 const*>(lhs.m_tree) == const_cast<Tree2 const*>(rhs.m_tree));
		return !(lhs != rhs);
	}

	template <typename T1, typename Tree1, typename T2, typename Tree2>
	inline bool operator!=(basic_tree_node_iterator<T1, Tree1> const& lhs, basic_tree_node_iterator<T2, Tree2> const& rhs)
	{
		assert(const_cast<Tree1 const*>(lhs.m_tree) == const_cast<Tree2 const*>(rhs.m_tree));
		return lhs.m_index != rhs.m_index || lhs.m_parent != rhs.m_parent;
	}
}

namespace wordring
{
	// ------------------------------------------------------------------------
	// tree
	// ------------------------------------------------------------------------

	/*! @brief 木コンテナ


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
	*/
	template <typename T, typename Allocator = std::allocator<detail::tree_node<T>>>
	class tree
	{
		template <typename T1, typename Tree1>
		friend class detail::basic_tree_node_iterator;

		template <typename T1, typename Allocator1>
		friend bool operator==(tree<T1, Allocator1> const&, tree<T1, Allocator1> const&);

		template <typename T1, typename Allocator1>
		friend bool operator!=(tree<T1, Allocator1> const&, tree<T1, Allocator1> const&);

	protected:
		using tree_type  = tree<T, Allocator>;
		using node_type  = detail::tree_node<T>;
		using index_type = typename node_type::index_type; 
		using node_proxy = detail::tree_node_proxy<T>;
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
		using iterator        = detail::basic_tree_node_iterator<value_type, tree_type>;
		using const_iterator  = detail::basic_tree_node_iterator<value_type const, tree_type const>;

	protected:
		index_type allocate()
		{
			index_type i = null_value;

			if (m_free != null_value)
			{
				i = m_free;
				m_free = m_heap.at(m_free).m_next;
			}
			else
			{
				i = m_heap.size();
				m_heap.emplace_back();
			}
			++m_size;

			return i;
		}

		template <typename Value>
		iterator allocate(const_iterator pos, Value value)
		{
			index_type i        = allocate();    // 挿入要素のインデックス
			node_type* p_heap   = m_heap.data(); // ヒープの先頭アドレス
			node_type* p        = p_heap + i;    // 挿入要素
			node_type* p_parent = nullptr;       // 親要素

			p->m_value = std::forward<Value>(value);

			if (pos.m_parent == null_value) // 根の要素
			{
				assert(m_root == null_value); // 根は一つの要素しか存在できない。
				p->m_prev = m_root = i;
			}
			else // この時点で親要素は存在が確定している。
			{
				p_parent    = p_heap + pos.m_parent;
				p->m_parent = pos.m_parent;

				if (p_parent->m_child == null_value) p_parent->m_child = p->m_prev = i; // 単独の子
				else if(pos.m_index == null_value)       // 最後の子
				{
					index_type i_tail = (p_heap + p_parent->m_child)->m_prev;
					node_type* p_tail = p_heap + i_tail; // （元）最後の要素
					p->m_prev         = i_tail;
					p_tail->m_next    = i;
					(p_heap + p_parent->m_child)->m_prev = i;
				}
				else
				{
					node_type* p_pos = p_heap + pos.m_index;

					if (p_parent->m_child == pos.m_index) // 先頭の子
					{
						p->m_prev         = p_pos->m_prev;
						p->m_next         = pos.m_index;;
						p_pos->m_prev     = i;
						p_parent->m_child = i;
					}
					else                                   // 中間の子
					{
						p->m_prev                        = p_pos->m_prev;
						p->m_next                        = pos.m_index;
						(p_heap + p_pos->m_prev)->m_next = i;
						p_pos->m_prev                    = i;
					}
				}
			}

			return iterator(*this, pos.m_parent, i);
		}

		void free(index_type i, bool free_next = false)
		{
			assert(i < m_heap.size());

			node_type& n = m_heap.at(i);

			if (n.m_child != null_value) free(n.m_child, true);
			if(free_next && n.m_next != null_value) free(n.m_next, true);

			n.m_parent = n.m_child = n.m_prev = null_value;
			n.m_next   = m_free;
			m_free     = i;

			--m_size;
		}

		iterator free(const_iterator pos)
		{
			iterator result(*this, pos.m_parent, pos.m_index);
			++result;

			node_type* p_heap = m_heap.data(); // ヒープの先頭アドレス

			index_type i        = pos.m_index;
			node_type* p        = p_heap + i;
			node_type* p_parent = nullptr;

			index_type i_parent = pos.m_parent;
			index_type i_prev   = p->m_prev;
			index_type i_next   = p->m_next;
			index_type i_tail   = null_value;

			if (i_parent != null_value)
			{
				p_parent = p_heap + i_parent;
				if (p_parent->m_child != null_value) i_tail = (p_heap + p_parent->m_child)->m_prev;
			}

			free(pos.m_index);

			if (i_parent == null_value) m_root = null_value;     // 根
			else
			{
				index_type i_child = p_parent->m_child;

				if (i_prev == i) p_parent->m_child = null_value; // 単独の要素
				else if (i_next == null_value)                   // 最後の要素
				{
					(p_heap + i_prev)->m_next  = null_value;
					(p_heap + i_child)->m_prev = i_prev;
				}
				else if (i_child == i)
				{
					(p_heap + i_next)->m_prev = i_prev;          // 先頭の要素
					p_parent->m_child         = i_next;
				}
				else                                             // 中間の要素
				{
					(p_heap + i_prev)->m_next = i_next;
					(p_heap + i_next)->m_prev = i_prev;
				}
			}

			return result;
		}

		static bool recursive_equal(const_iterator first1, const_iterator last1, const_iterator first2, const_iterator last2)
		{
			if (first1 == last1 && first2 == last2) return true;
			if (first1 == last1 && first2 != last2) return false;
			if (first1 != last1 && first2 == last2) return false;

			if (!std::equal(first1, last1, first2, last2)) return false; // 値の比較

																		 // 子の比較
			while (first1 != last1 && first2 != last2)
			{
				if (!recursive_equal(first1.begin(), first1.end(), first2.begin(), first2.end())) return false;
				++first1;
				++first2;
			}

			return first1 == last1 && first2 == last2;
		}

		static bool recursive_not_equal(const_iterator first1, const_iterator last1, const_iterator first2, const_iterator last2)
		{
			if (first1 == last1 && first2 == last2) return false;
			if (first1 == last1 && first2 != last2) return true;
			if (first1 != last1 && first2 == last2) return true;

			if (!std::equal(first1, last1, first2, last2)) return true; // 値の比較

																		// 子の比較
			while (first1 != last1 && first2 != last2)
			{
				if (recursive_not_equal(first1.begin(), first1.end(), first2.begin(), first2.end())) return true;
				++first1;
				++first2;
			}

			return !(first1 == last1 && first2 == last2);
		}

	public:
		tree()
			: m_heap(allocator_type())
			, m_root(null_value)
			, m_free(null_value)
			, m_size(0)
		{
		}

		explicit tree(allocator_type const& alloc)
			: m_heap(alloc)
			, m_root(null_value)
			, m_free(null_value)
			, m_size(0)
		{
		}

		/*! 要素から構築する
		*/
		explicit tree(value_type const& value, allocator_type const& alloc = allocator_type())
			: m_heap(alloc)
			, m_root(null_value)
			, m_free(null_value)
			, m_size(0)
		{
			insert(begin(), value);
		}

		/*! 要素から構築する
		*/
		explicit tree(value_type&& value, allocator_type const& alloc = allocator_type())
			: m_heap(alloc)
			, m_root(null_value)
			, m_free(null_value)
			, m_size(0)
		{
			insert(begin(), std::move(value));
		}

		tree(tree const&) = default;

		tree(tree&&) noexcept = default;

		explicit tree(node_proxy proxy, allocator_type const& alloc = allocator_type())
			: m_heap(alloc)
			, m_root(null_value)
			, m_free(null_value)
			, m_size(0)
		{
			insert(begin(), proxy);
		}

		/*! デストラクタ
		*/
		~tree() = default;

		tree& operator=(tree const&) = default;

		tree& operator=(tree&&) noexcept = default;

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

		allocator_type get_allocator() const { return m_heap.get_allocator(); }

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

		iterator begin() noexcept { return iterator(*this, null_value, m_root); }

		const_iterator begin() const noexcept { return cbegin(); }

		const_iterator cbegin() const noexcept { return const_iterator(*this, null_value, m_root); }

		iterator end() noexcept { return iterator(*this, null_value, null_value); }

		const_iterator end() const noexcept { return cend(); }

		const_iterator cend() const noexcept { return const_iterator(*this, null_value, null_value); }

		// 容量 ---------------------------------------------------------------

		bool empty() const noexcept { return m_size == 0; }

		size_type size() const noexcept { return m_size; }

		size_type max_size() const noexcept { return std::min(static_cast<size_type>(null_value - 1), m_heap.max_size()); }

		// 変更 ---------------------------------------------------------------

		void clear() noexcept
		{
			m_heap.clear();
			m_root = null_value;
			m_free = null_value;
			m_size = 0;
		}

		/*! posの前にvalueを追加する
		*/
		iterator insert(const_iterator pos, value_type const& value)
		{
			return allocate(pos, value);
		}

		iterator insert(const_iterator pos, value_type&& value)
		{
			return allocate(pos, std::move(value));
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

		iterator erase(const_iterator pos)
		{
			assert(pos.m_index != null_value);
			return free(pos);
		}

		iterator erase(const_iterator first, const_iterator last)
		{
			assert(first.m_parent == last.m_parent);

			while (first != last) erase(first++);

			return iterator(*this, first.m_parent, first.m_index);
		}

	protected:
		container  m_heap;
		index_type m_root;
		index_type m_free;
		index_type m_size;
	};

	template <typename T1, typename Allocator1>
	inline bool operator==(tree<T1, Allocator1> const& lhs, tree<T1, Allocator1> const& rhs)
	{
		return !(lhs != rhs);
	}

	template <typename T1, typename Allocator1>
	inline bool operator!=(tree<T1, Allocator1> const& lhs, tree<T1, Allocator1> const& rhs)
	{
		return tree<T1, Allocator1>::recursive_not_equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
	}
}
