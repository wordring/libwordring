#pragma once

#include <algorithm>
#include <iterator>
#include <memory>
#include <type_traits>
#include <utility>

namespace wordring
{
	template <typename T, typename Allocator, typename Base>
	class basic_tree : public Base
	{
		template <typename T1, typename Allocator1, typename Base1>
		friend bool operator==(basic_tree<T1, Allocator1, Base1> const& lhs, basic_tree<T1, Allocator1, Base1> const& rhs);

		template <typename T1, typename Allocator1, typename Base1>
		friend bool operator!=(basic_tree<T1, Allocator1, Base1> const& lhs, basic_tree<T1, Allocator1, Base1> const& rhs);

		template <typename T1, typename Allocator1, typename Base1>
		friend bool operator<(basic_tree<T1, Allocator1, Base1> const& lhs, basic_tree<T1, Allocator1, Base1> const& rhs);

		template <typename T1, typename Allocator1, typename Base1>
		friend bool operator<=(basic_tree<T1, Allocator1, Base1> const& lhs, basic_tree<T1, Allocator1, Base1> const& rhs);

		template <typename T1, typename Allocator1, typename Base1>
		friend bool operator>(basic_tree<T1, Allocator1, Base1> const& lhs, basic_tree<T1, Allocator1, Base1> const& rhs);

		template <typename T1, typename Allocator1, typename Base1>
		friend bool operator>=(basic_tree<T1, Allocator1, Base1> const& lhs, basic_tree<T1, Allocator1, Base1> const& rhs);

	public:
		using base_type  = Base;
		using container  = typename base_type::container;

		// Tに対するメンバ型
		using element_type            = T;
		using element_reference       = std::add_lvalue_reference_t<T>;
		using const_element_reference = std::add_lvalue_reference_t<T const>;
		using element_pointer         = T*;
		using const_element_pointer   = T const*;

		// コンテナに対するメンバ型
		using value_type             = typename container::value_type;
		using allocator_type         = typename container::allocator_type;
		using size_type              = typename container::size_type;
		using difference_type        = typename container::difference_type;
		using reference              = typename container::reference;
		using const_reference        = typename container::const_reference;
		using pointer                = typename container::pointer;
		using const_pointer          = typename container::const_pointer;
		using iterator               = typename container::iterator;
		using const_iterator         = typename container::const_iterator;
		using reverse_iterator       = typename container::reverse_iterator;
		using const_reverse_iterator = typename container::const_reverse_iterator;

	private:
		// 型の特性
		static constexpr bool is_nothrow_move_constructible_v
			= std::conjunction_v<std::is_nothrow_move_constructible<container>, std::is_nothrow_move_constructible<element_type>>;

		static constexpr bool is_nothrow_move_assignable_v
			= std::conjunction_v<std::is_nothrow_move_assignable<container>, std::is_nothrow_move_assignable<element_type>>;

		static constexpr bool is_nothrow_swappable_v
			= std::conjunction_v<std::is_nothrow_swappable<container>, std::is_nothrow_swappable<element_type>>;

	protected:
		using base_type::m_children;

	public:
		basic_tree() : base_type(allocator_type()), m_value() {}

		explicit basic_tree(allocator_type const& alloc) : base_type(alloc), m_value() {}

		/*! 要素から構築する
		要素型からツリー型へ暗黙の型変換を有効にするため、explicitではない。
		*/
		basic_tree(element_type const& value, allocator_type const& alloc = allocator_type()) : base_type(alloc), m_value(value) {}

		/*! 要素から構築する
		要素型からツリー型へ暗黙の型変換を有効にするため、explicitではない。
		*/
		basic_tree(element_type&& value, allocator_type const& alloc = allocator_type()) : base_type(alloc), m_value(std::move(value)) {}

		template <typename InputIterator>
		basic_tree(element_type const& value, InputIterator first, InputIterator last, allocator_type const& alloc = allocator_type())
			: base_type(alloc)
			, m_value(value)
		{
			m_children.assign(first, last);
		}

		basic_tree(basic_tree const&) = default;

		/*!
		- C26439 std::listがnoexceptでないため対応できない。
		*/
		basic_tree(basic_tree&&) noexcept(is_nothrow_move_constructible_v) = default;

		basic_tree(element_type const& value, std::initializer_list<element_type> ilist, allocator_type const& alloc = allocator_type())
			: base_type(alloc)
			, m_value(value)
		{
			m_children.assign(ilist.begin(), ilist.end());
		}

		basic_tree(element_type const& value, std::initializer_list<value_type> ilist, allocator_type const& alloc = allocator_type())
			: base_type(ilist, alloc)
			, m_value(value) {}

		/*! デストラクタ
		- TODO: 大きなツリーを破棄するとき、葉から削除していかないと、再帰的なデスクトラクタ呼び出しが発生し、マシンスタックを大量に消費する。
		*/
		~basic_tree() = default;

		basic_tree& operator=(basic_tree const&) = default;

		/*!
		- C26439 std::listがnoexceptでないため対応できない。
		*/
		basic_tree& operator=(basic_tree&&) noexcept(is_nothrow_move_assignable_v) = default;

		basic_tree& operator=(std::initializer_list<element_type> ilist)
		{
			assign(ilist);
			return *this;
		}

		basic_tree& operator=(std::initializer_list<value_type> ilist)
		{
			assign(ilist);
			return *this;
		}

		template <typename InputIterator>
		void assign(InputIterator first, InputIterator last) { m_children.assign(first, last); }

		void assign(std::initializer_list<element_type> ilist) { m_children.assign(ilist.begin(), ilist.end()); }

		void assign(std::initializer_list<value_type> ilist) { m_children.assign(ilist); }

		allocator_type get_allocator() const { return m_children.get_allocator(); }

		// 要素アクセス --------------------------------------------------------
		
		reference front() { return m_children.front(); }

		const_reference front() const { return m_children.front(); }

		reference back() { return m_children.back(); }

		const_reference back() const { return m_children.back(); }

		// イテレータ ----------------------------------------------------------

		iterator begin() noexcept { return m_children.begin(); }

		const_iterator begin() const noexcept { return m_children.begin(); }

		const_iterator cbegin() const noexcept { return m_children.cbegin(); }

		iterator end() noexcept { return m_children.end(); }

		const_iterator end() const noexcept { return m_children.end(); }

		const_iterator cend() const noexcept { return m_children.cend(); }

		reverse_iterator rbegin() noexcept { return m_children.rbegin(); }

		const_reverse_iterator rbegin() const noexcept { return m_children.rbegin(); }

		const_reverse_iterator crbegin() const noexcept { return m_children.crbegin(); }

		reverse_iterator rend() noexcept { return m_children.rend(); }

		const_reverse_iterator rend() const noexcept { return m_children.rend(); }

		const_reverse_iterator crend() const noexcept { return m_children.crend(); }

		// 容量 ---------------------------------------------------------------

		bool empty() const noexcept { return m_children.empty(); }

		size_type size() const noexcept { return m_children.size(); }

		size_type max_size() const noexcept { return m_children.max_size(); }

		// 変更 ---------------------------------------------------------------

		void clear() noexcept { m_children.clear(); }

		iterator insert(const_iterator pos, value_type const& value) { return m_children.insert(pos, value); }

		iterator insert(const_iterator pos, value_type&& value) { return m_children.insert(pos, std::move(value)); }

		iterator insert(const_iterator pos, size_type count, value_type const& value) { return m_children.insert(pos, count, value); }

		template <typename InputIterator>
		iterator insert(const_iterator pos, InputIterator first, InputIterator last) { return m_children.insert(pos, first, last); }

		iterator insert(const_iterator pos, std::initializer_list<value_type> ilist) { return m_children.insert(pos, ilist); }

		iterator insert(const_iterator pos, std::initializer_list<element_type> ilist)
		{
			return m_children.insert(pos, ilist.begin(), ilist.end());
		}

		template <typename... Args>
		iterator emplace(const_iterator pos, Args&&... args) { return m_children.emplace(pos, std::forward<Args>(args)...); }

		iterator erase(const_iterator pos) { return m_children.erase(pos); }

		iterator erase(const_iterator first, const_iterator last) { return m_children.erase(first, last); }

		void push_back(value_type const& value) { m_children.push_back(value); }

		void push_back(value_type&& value) { m_children.push_back(std::move(value)); }

		template <typename... Args>
		reference emplace_back(Args&&... args) { return m_children.emplace_back(std::forward<Args>(args)...); }

		void pop_back() { m_children.pop_back(); }

		void resize(size_type count) { m_children.resize(count); }

		void resize(size_type count, value_type const& value) { m_children.resize(count, value); }

		void swap(value_type& other) noexcept(is_nothrow_swappable_v)
		{
			std::swap(m_value, other.m_value);
			m_children.swap(other);
		}

		// 値 -----------------------------------------------------------------

		element_reference operator*() { return m_value; }

		const_element_reference operator*() const { return m_value; }

		element_pointer operator->() { return &m_value; }

		const_element_pointer operator->() const { return &m_value; }

		private:
			element_type m_value;
	};

	template <typename T, typename Allocator, typename Base>
	inline bool operator==(basic_tree<T, Allocator, Base> const& lhs, basic_tree<T, Allocator, Base> const& rhs)
	{
		if (lhs.m_value != rhs.m_value) return false;
		return lhs.m_children == rhs.m_children;
	}

	template <typename T, typename Allocator, typename Base>
	inline bool operator!=(basic_tree<T, Allocator, Base> const& lhs, basic_tree<T, Allocator, Base> const& rhs)
	{
		return !(lhs == rhs);
	}

	template <typename T, typename Allocator, typename Base>
	inline bool operator<(basic_tree<T, Allocator, Base> const& lhs, basic_tree<T, Allocator, Base> const& rhs)
	{
		return lhs.m_value < rhs.m_value || lhs.m_children < rhs.m_children;
	}

	template <typename T, typename Allocator, typename Base>
	inline bool operator<=(basic_tree<T, Allocator, Base> const& lhs, basic_tree<T, Allocator, Base> const& rhs)
	{
		return lhs < rhs|| lhs == rhs;
	}

	template <typename T, typename Allocator, typename Base>
	inline bool operator>(basic_tree<T, Allocator, Base> const& lhs, basic_tree<T, Allocator, Base> const& rhs)
	{
		return lhs.m_value > rhs.m_value || lhs.m_children > rhs.m_children;
	}

	template <typename T, typename Allocator, typename Base>
	inline bool operator>=(basic_tree<T, Allocator, Base> const& lhs, basic_tree<T, Allocator, Base> const& rhs)
	{
		return lhs > rhs || lhs == rhs;
	}

}
