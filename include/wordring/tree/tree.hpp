#pragma once

#include <algorithm>
#include <iterator>
#include <list>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

namespace wordring
{

	template <typename T, typename Allocator, typename Base>
	class basic_tree;

	template <typename T, typename Allocator>
	struct list_tree_base
	{
		using container = std::list<basic_tree<T, Allocator, list_tree_base<T, Allocator>>>;

		container m_children;

	protected:
		list_tree_base() = default;

		explicit list_tree_base(Allocator const& alloc) : m_children(alloc) {}

		list_tree_base(std::initializer_list<typename container::value_type> ilist, Allocator const& alloc)
			: m_children(ilist, alloc) {}

		~list_tree_base() = default;
	};

	template <typename T, typename Allocator>
	struct vector_tree_base
	{
		using container = std::vector<basic_tree<T, Allocator, vector_tree_base<T, Allocator>>>;

		using reference       = typename container::reference;
		using const_reference = typename container::const_reference;
		using size_type = typename container::size_type;

		container m_children;

	protected:
		vector_tree_base() = default;

		explicit vector_tree_base(Allocator const& alloc) : m_children(alloc) {}

		vector_tree_base(std::initializer_list<typename container::value_type> ilist, Allocator const& alloc)
			: m_children(ilist, alloc) {}

		~vector_tree_base() = default;

	public:

		// 要素アクセス --------------------------------------------------------

		reference at(size_type pos) { return m_children.at(); }

		const_reference at(size_type pos) const { return m_children.at(); }

	};

	template <typename T, typename Allocator, typename Base = list_tree_base<T, Allocator>>
	class basic_tree : public Base
	{
	public:
		using base_type = Base;
		using this_type = basic_tree<T, Allocator, Base>;
		using container = typename base_type::container;

		// Tに対するメンバ型
		using element_pointer = T*;
		using element_type    = T;

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

		basic_tree(basic_tree&&) = default;

		basic_tree(element_type const& value, std::initializer_list<element_type> ilist, allocator_type const& alloc = allocator_type())
			: base_type(alloc)
			, m_value(value)
		{
			m_children.assign(ilist.begin(), ilist.end());
		}

		basic_tree(element_type const& value, std::initializer_list<this_type> ilist, allocator_type const& alloc = allocator_type())
			: base_type(ilist, alloc)
			, m_value(value) {}

		/*! デストラクタ
		- TODO: 大きなツリーを破棄するとき、葉から削除していかないと、再帰的なデスクトラクタ呼び出しが発生し、マシンスタックを大量に消費する。
		*/
		~basic_tree() = default;

		basic_tree& operator=(basic_tree const&) = default;

		basic_tree& operator=(basic_tree&&) = default;

		template <typename InputIterator>
		void assign(InputIterator first, InputIterator last) { m_children.assign(first, last); }

		void assign(std::initializer_list<element_type> ilist) { m_children.assign(ilist.begin(), ilist.end()); }

		void assign(std::initializer_list<this_type> ilist) { m_children.assign(ilist); }

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

		iterator insert(const_iterator pos, this_type const& value) { return m_children.insert(pos, value); }

		iterator insert(const_iterator pos, this_type&& value) { return m_children.insert(pos, std::move(value)); }

		iterator insert(const_iterator pos, size_type count, this_type const& value) { return m_children.insert(pos, count, value); }

		template <typename InputIterator>
		iterator insert(const_iterator pos, InputIterator first, InputIterator last) { return m_children.insert(pos, first, last); }

		iterator insert(const_iterator pos, std::initializer_list<this_type> ilist) { return m_children.insert(pos, ilist); }

		iterator insert(const_iterator pos, std::initializer_list<element_type> ilist)
		{
			return m_children.insert(pos, ilist.begin(), ilist.end());
		}

		template <typename... Args>
		iterator emplace(const_iterator pos, Args&&... args) { return m_children.emplace(pos, std::forward<Args>(args)...); }

		iterator erase(const_iterator pos) { return m_children.erase(pos); }

		iterator erase(const_iterator first, const_iterator last) { return m_children.erase(first, last); }

		void push_back(this_type const& value) { m_children.push_back(value); }

		void push_back(this_type&& value) { m_children.push_back(std::move(value)); }

		template <typename... Args>
		reference emplace_back(Args&&... args) { return m_children.emplace_back(std::forward<Args>(args)...); }

		void pop_back() { m_children.pop_back(); }

		// 観察 ---------------------------------------------------------------

		std::add_lvalue_reference_t<element_type> operator*() { return m_value; }

		std::add_lvalue_reference_t<element_type const> operator*() const { return m_value; }

		private:
			element_type m_value;
	};

	template <typename T, typename Allocator = std::allocator<T>>
	using tree = basic_tree<T, Allocator, list_tree_base<T, Allocator>>;

	template <typename T, typename Allocator = std::allocator<T>>
	using vector_tree = basic_tree<T, Allocator, vector_tree_base<T, Allocator>>;
}
