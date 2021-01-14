#pragma once

#include <deque>
#include <iterator>

namespace wordring::detail
{
	/*! brief プレ・オーダー走査用のコンテナ

	basic_tree_iterator のコンテナとして使われる。
	*/
	template <typename Iterator, typename Allocator>
	class tree_iterator_stack
	{
	public:
		explicit tree_iterator_stack(Allocator const& alloc = Allocator())
			: m_stack(alloc)
		{
		}

		explicit tree_iterator_stack(Iterator const& base, Allocator const& alloc = Allocator())
			: m_stack(1, base, alloc)
		{
		}

		tree_iterator_stack(Iterator first, Iterator last, Allocator const& alloc = Allocator())
			: m_stack(alloc)
		{
			while (first != last) m_stack.push_back(first++);
		}

		Iterator& top() const { return const_cast<std::deque<Iterator>&>(m_stack).front(); }

		void pop() { m_stack.erase(m_stack.begin()); }

		void push(Iterator const& it) { m_stack.push_front(it); }

		void push(Iterator first, Iterator last)
		{
			auto pos = m_stack.begin();
			while (first != last) pos = ++m_stack.insert(pos, first++);
		}

		bool empty() const { return m_stack.empty(); }

	private:
		std::deque<Iterator> m_stack;
	};

	/*! brief レベル・オーダー走査用のコンテナ

	basic_tree_iterator のコンテナとして使われる。
	*/
	template <typename Iterator, typename Allocator>
	class tree_iterator_queue
	{
	public:
		explicit tree_iterator_queue(Allocator const& alloc = Allocator())
			: m_queue(alloc)
		{
		}

		explicit tree_iterator_queue(Iterator const& base, Allocator const& alloc = Allocator())
			: m_queue(1, base, alloc)
		{
		}

		tree_iterator_queue(Iterator first, Iterator last, Allocator const& alloc = Allocator())
			: m_queue(alloc)
		{
			while (first != last) m_queue.push_back(first++);
		}

		Iterator& top() const
		{
			return const_cast<std::deque<Iterator>&>(m_queue).front();
		}

		void pop() { m_queue.erase(m_queue.begin()); }

		void push(Iterator const& it) { m_queue.push_back(it); }

		void push(Iterator first, Iterator last)
		{
			while (first != last) m_queue.push_back(first++);
		}

		bool empty() const { return m_queue.empty(); }

	private:
		std::deque<Iterator> m_queue;
	};
}

namespace wordring
{
	/*! @class basic_tree_iterator tree_iterator.hpp wordring/tree/tree_iterator.hpp
	
	@brief プレ・オーダーあるいはレベル・オーダーで木を走査するイテレータ・アダプター
	
	@tparam Iterator ベースとなる木のイテレータ
	@tparam Container detail::tree_iterator_stack あるいは detail::tree_iterator_queue
	@tparam Allocator アロケータ

	プレ・オーダー走査用の <b>tree_iterator</b> とレベル・オーダー走査用の
	<b>level_order_tree_iterator</b> が事前に定義されている。

	@code
		template <typename Iterator, typename  Allocator = std::allocator<Iterator>>
		using tree_iterator = basic_tree_iterator<Iterator, detail::tree_iterator_stack<Iterator, Allocator>, Allocator>;

		template <typename Iterator, typename  Allocator = std::allocator<Iterator>>
		using level_order_tree_iterator = basic_tree_iterator<Iterator, detail::tree_iterator_queue<Iterator, Allocator>, Allocator>;
	@endcode

	このクラスは、道順の調査に、スタックあるいはキューを使う。
	従って、コピーのコストは相応に高い。

	<b>operator++()</b> によって、走査順に従い次の要素へ移動する。
	<b>operator*()</b> によって、指している要素の逆参照を得る。
	元となる木のイテレータを得るには、 <b>base()</b> を使う。
	
	走査順のイメージを以下に示す。\n
	※要素についている番号は移動順を示す。

	@par プレ・オーダー

	@image html tree_traverse_pre_order.svg
	
	@par レベル・オーダー
	
	@image html tree_traverse_level_order.svg
	*/
	template <typename Iterator, typename Container, typename Allocator>
	class basic_tree_iterator
	{
		template <typename Iterator1, typename Container1, typename Allocator1>
		friend bool operator==(basic_tree_iterator<Iterator1, Container1, Allocator1> const&, basic_tree_iterator<Iterator1, Container1, Allocator1> const&);

		template <typename Iterator1, typename Container1, typename Allocator1>
		friend bool operator!=(basic_tree_iterator<Iterator1, Container1, Allocator1> const&, basic_tree_iterator<Iterator1, Container1, Allocator1> const&);

	public:
		using base_type       = Iterator;
		using container       = Container;
		using allocator_type  = Allocator;

		using value_type        = typename std::iterator_traits<base_type>::value_type;
		using difference_type   = typename std::iterator_traits<base_type>::difference_type;
		using reference         = typename std::iterator_traits<base_type>::reference;
		using pointer           = typename std::iterator_traits<base_type>::pointer;
		using iterator_category = std::input_iterator_tag;

	public:
		explicit basic_tree_iterator(allocator_type const& alloc = allocator_type())
			: m_c(alloc)
		{
		}

		/*! @brief 親を指すイテレータを指定して構築する
		
		@param it 親を指すイテレータ
		@param alloc 内部コンテナ用のアロケータ

		親は任意の要素で十分であり、木の根である必要はない。
		*/
		explicit basic_tree_iterator(base_type const& it, allocator_type const& alloc = allocator_type())
			: m_c(it, alloc)
		{
		}

		[[deprecated]]
		basic_tree_iterator(base_type const& first, base_type const& last, allocator_type const& alloc = allocator_type())
			: m_c(first, last, alloc)
		{
		}

		basic_tree_iterator(basic_tree_iterator const&) = default;

		basic_tree_iterator(basic_tree_iterator&&) = default;

		basic_tree_iterator& operator=(basic_tree_iterator const& rhs) = default;

		basic_tree_iterator& operator=(basic_tree_iterator&& rhs) = default;

		/*! @brief 現在の位置を指す、元となる木のイテレータを返す
		*/
		base_type base() const { return m_c.top(); }

		/*! @brief 要素の逆参照を返す
		*/
		reference operator*() const
		{
			return const_cast<reference>(*m_c.top());
		}

		/*! @brief 要素の参照を返す
		*/
		pointer operator->() const
		{
			return const_cast<pointer>(m_c.top().operator->());
		}

		/*! @brief イテレータを進める
		
		@return *this
		*/
		basic_tree_iterator& operator++()
		{
			base_type it = m_c.top();
			m_c.pop();
			m_c.push(it.begin(), it.end());
			return *this;
		}

		/*! @brief イテレータを進める
		
		@return 進める前の位置を指す元となる木のイテレータ

		この関数は、コピーのコストを避けるため basic_tree_iterator ではなく元となる木のイテレータを返す。
		*/
		base_type operator++(int)
		{
			base_type result = m_c.top();
			operator++();
			return result;
		}
		
	private:
		container m_c;
	};

	template <typename Iterator1, typename Container1, typename Allocator1>
	inline bool operator==(basic_tree_iterator<Iterator1, Container1, Allocator1> const& lhs, basic_tree_iterator<Iterator1, Container1, Allocator1> const& rhs)
	{
		return (lhs.m_c.empty() && rhs.m_c.empty())
			|| (!lhs.m_c.empty() && !rhs.m_c.empty() && lhs.base() == rhs.base());
	}

	template <typename Iterator1, typename Container1, typename Allocator1>
	inline bool operator!=(basic_tree_iterator<Iterator1, Container1, Allocator1> const& lhs, basic_tree_iterator<Iterator1, Container1, Allocator1> const& rhs)
	{
		return !(lhs == rhs);
	}

	template <typename Iterator, typename  Allocator = std::allocator<Iterator>>
	using tree_iterator = basic_tree_iterator<Iterator, detail::tree_iterator_stack<Iterator, Allocator>, Allocator>;

	template <typename Iterator, typename  Allocator = std::allocator<Iterator>>
	using level_order_tree_iterator = basic_tree_iterator<Iterator, detail::tree_iterator_queue<Iterator, Allocator>, Allocator>;
}
