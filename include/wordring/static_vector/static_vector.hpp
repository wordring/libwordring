#pragma once

#include <algorithm>
#include <array>
#include <cstdint>
#include <initializer_list>
#include <iterator>
#include <memory>
#include <type_traits>

namespace wordring
{
	template <typename T, std::size_t N>
	class static_vector;
}

namespace wordring::detail
{
	template <typename Container>
	class static_vector_iterator
	{
		template <typename T, std::size_t N>
		friend class wordring::static_vector;

		friend class static_vector_iterator<Container const>;
		friend class static_vector_iterator<std::remove_const_t<Container>>;

		template <typename Container1, typename Container2>
		friend auto operator+(static_vector_iterator<Container1> const&, static_vector_iterator<Container2> const&);

		template <typename Container1>
		friend static_vector_iterator<Container1> operator+(static_vector_iterator<Container1> const&, std::size_t n);

		template <typename Container1>
		friend static_vector_iterator<Container1> operator+(std::size_t n, static_vector_iterator<Container1> const&);

		template <typename Container1, typename Container2>
		friend auto operator-(static_vector_iterator<Container1> const&, static_vector_iterator<Container2> const&);

		template <typename Container1>
		friend static_vector_iterator<Container1> operator-(static_vector_iterator<Container1> const&, std::size_t n);

		template <typename Container1, typename Container2>
		friend bool operator==(static_vector_iterator<Container1> const&, static_vector_iterator<Container2> const&);

		template <typename Container1, typename Container2>
		friend bool operator!=(static_vector_iterator<Container1> const&, static_vector_iterator<Container2> const&);

		template <typename Container1, typename Container2>
		friend bool operator>(static_vector_iterator<Container1> const&, static_vector_iterator<Container2> const&);

		template <typename Container1, typename Container2>
		friend bool operator<(static_vector_iterator<Container1> const&, static_vector_iterator<Container2> const&);

		template <typename Container1, typename Container2>
		friend bool operator>=(static_vector_iterator<Container1> const&, static_vector_iterator<Container2> const&);

		template <typename Container1, typename Container2>
		friend bool operator<=(static_vector_iterator<Container1> const&, static_vector_iterator<Container2> const&);

	public:
		using container = Container;

		using value_type = typename container::value_type;
		using size_type = typename container::size_type;
		using difference_type = typename container::difference_type;
		using reference = std::conditional_t<std::is_const_v<container>, value_type const&, value_type&>;
		using const_reference = typename container::const_reference;
		using pointer = std::conditional_t<std::is_const_v<container>, value_type const*, value_type*>;
		using const_pointer = typename container::const_pointer;

		using iterator_category = std::random_access_iterator_tag;

	public:
		static_vector_iterator()
			: m_container(nullptr)
			, m_position(0)
		{
		}

		static_vector_iterator(container& c, difference_type pos)
			: m_container(std::addressof(c))
			, m_position(pos)
		{
		}

		template <typename Container1>
		static_vector_iterator(static_vector_iterator<Container1> const& rhs)
			: m_container(rhs.m_container)
			, m_position(rhs.m_position)
		{
		}

		static_vector_iterator(static_vector_iterator const&) = default;

		static_vector_iterator(static_vector_iterator&&) = default;

		~static_vector_iterator() noexcept = default;

		template <typename Container1>
		static_vector_iterator& operator=(static_vector_iterator<Container1> const& rhs)
		{
			static_assert(std::is_const_v<container>);

			m_container = rhs.m_container;
			m_position = rhs.m_position;
			return *this;
		}

		static_vector_iterator& operator=(static_vector_iterator const&) = default;

		static_vector_iterator& operator=(static_vector_iterator&&) = default;

		reference operator*()
		{
			return *(m_container->data() + m_position);
		}

		const_reference operator*() const { return *(m_container->data() + m_position); }

		pointer operator->() { return m_container->data() + m_position; }

		const_pointer operator->() const { return m_container->data() + m_position; }

		reference operator[](int n) { return *(m_container->data() + m_position + n); }

		const_reference operator[](int n) const { return *(m_container->data() + m_position + n); }

		static_vector_iterator& operator++()
		{
			assert(static_cast<std::make_unsigned_t<decltype(m_position)>>(m_position) < m_container->size());
			++m_position;
			return *this;
		}

		static_vector_iterator operator++(int)
		{
			static_vector_iterator result = *this;
			operator++();
			return result;
		}

		static_vector_iterator& operator--()
		{
			assert(static_cast<std::make_unsigned_t<difference_type>>(m_position) <= m_container->size());
			--m_position;
			return *this;
		}

		static_vector_iterator operator--(int)
		{
			static_vector_iterator<container> result = *this;
			operator--();
			return result;
		}

		static_vector_iterator& operator+=(size_t n)
		{
			m_position += n;
			assert(static_cast<std::make_unsigned_t<difference_type>>(m_position) < m_container->size());
			return *this;
		}

		static_vector_iterator& operator-=(size_t n)
		{
			m_position -= n;
			assert(0 <= m_position);
			return *this;
		}

	protected:
		container* m_container;
		difference_type  m_position;
	};

	template <typename Container1, typename Container2>
	inline auto operator+(static_vector_iterator<Container1> const& lhs, static_vector_iterator<Container2> const& rhs)
	{
		return lhs.m_position + rhs.m_position;
	}

	template <typename Container1>
	inline static_vector_iterator<Container1> operator+(static_vector_iterator<Container1> const& it, std::size_t n)
	{
		return static_vector_iterator<Container1>(*it.m_container, it.m_position + n);
	}

	template <typename Container1>
	inline static_vector_iterator<Container1> operator+(std::size_t n, static_vector_iterator<Container1> const& it)
	{
		return static_vector_iterator<Container1>(*it.m_container, n + it.m_position);
	}

	template <typename Container1, typename Container2>
	inline auto operator-(static_vector_iterator<Container1> const& lhs, static_vector_iterator<Container2> const& rhs)
	{
		return lhs.m_position - rhs.m_position;
	}

	template <typename Container1>
	inline static_vector_iterator<Container1> operator-(static_vector_iterator<Container1> const& it, std::size_t n)
	{
		return static_vector_iterator<Container1>(*it.m_container, it.m_position - n);
	}

	template <typename Container1, typename Container2>
	inline bool operator==(static_vector_iterator<Container1> const& lhs, static_vector_iterator<Container2> const& rhs)
	{
		return lhs.m_position == rhs.m_position;
	}

	template <typename Container1, typename Container2>
	inline bool operator!=(static_vector_iterator<Container1> const& lhs, static_vector_iterator<Container2> const& rhs)
	{
		return !(lhs == rhs);
	}

	template <typename Container1, typename Container2>
	inline bool operator>(static_vector_iterator<Container1> const& lhs, static_vector_iterator<Container2> const& rhs)
	{
		return rhs < lhs;
	}

	template <typename Container1, typename Container2>
	inline bool operator<(static_vector_iterator<Container1> const& lhs, static_vector_iterator<Container2> const& rhs)
	{
		return rhs.m_position - lhs.m_position > 0;
	}

	template <typename Container1, typename Container2>
	inline bool operator>=(static_vector_iterator<Container1> const& lhs, static_vector_iterator<Container2> const& rhs)
	{
		return !(lhs < rhs);
	}

	template <typename Container1, typename Container2>
	inline bool operator<=(static_vector_iterator<Container1> const& lhs, static_vector_iterator<Container2> const& rhs)
	{
		return !(lhs > rhs);
	}
}

namespace wordring
{
	// ------------------------------------------------------------------------
	// static_vector
	// ------------------------------------------------------------------------

	/*! @class static_vector static_vector.hpp wordring/static_vector/static_vector.hpp
	
	@brief 最大長固定の静的vector

	メモリーの動的確保を避ける目的で用意した。
	*/
	template <typename T, std::size_t N>
	class static_vector
	{
		template <typename T1, std::size_t N1>
		friend bool operator==(static_vector<T1, N1> const& lhs, static_vector<T1, N1> const& rhs);

		template <typename T1, std::size_t N1>
		friend bool operator!=(static_vector<T1, N1> const& lhs, static_vector<T1, N1> const& rhs);

		template <typename T1, std::size_t N1>
		friend bool operator<(static_vector<T1, N1> const& lhs, static_vector<T1, N1> const& rhs);

		template <typename T1, std::size_t N1>
		friend bool operator<=(static_vector<T1, N1> const& lhs, static_vector<T1, N1> const& rhs);

		template <typename T1, std::size_t N1>
		friend bool operator>(static_vector<T1, N1> const& lhs, static_vector<T1, N1> const& rhs);

		template <typename T1, std::size_t N1>
		friend bool operator>=(static_vector<T1, N1> const& lhs, static_vector<T1, N1> const& rhs);

	protected:
		using container = std::array<T, N>;

	public:
		// メンバ型 -----------------------------------------------------------

		using value_type             = typename container::value_type;
		using size_type              = typename container::size_type;
		using difference_type        = typename container::difference_type;
		using reference              = typename container::reference;
		using const_reference        = typename container::const_reference;
		using pointer                = typename container::pointer;
		using const_pointer          = typename container::const_pointer;
		using iterator               = detail::static_vector_iterator<container>;
		using const_iterator         = detail::static_vector_iterator<container const>;
		using reverse_iterator       = std::reverse_iterator<iterator>;
		using const_reverse_iterator = std::reverse_iterator<const_iterator>;

	public:
		static_vector() noexcept
			: m_container()
			, m_size(0)
		{
		}

		static_vector(size_type count, value_type const& value)
			: m_container()
			, m_size(count)
		{
			assert(count <= N);
			for (size_type i = 0; i < count; ++i) *(m_container.data() + i) = value;
		}

		explicit static_vector(size_type count)
			: m_container()
			, m_size(count)
		{
			assert(count <= N);
			for (size_type i = 0; i < count; ++i) *(m_container.data() + i) = value_type();
		}

		template <typename InputIterator>
		static_vector(InputIterator first, InputIterator last)
			: m_container()
			, m_size(0)
		{
			assert(std::distance(first, last) <= N);
			for (; first != last; ++m_size) *(m_container.data() + m_size) = *first++;
		}

		static_vector(static_vector const&) = default;

		static_vector(static_vector&&) = default;

		static_vector(std::initializer_list<value_type> ilist)
			: m_container()
			, m_size(0)
		{
			assert(ilist.size() <= N);
			for (auto it = ilist.begin(); it != ilist.end(); ++m_size) *(m_container.data() + m_size) = *it++;
		}

		~static_vector() = default;

		static_vector& operator=(static_vector const&) = default;

		static_vector& operator=(static_vector&&) = default;

		static_vector& operator=(std::initializer_list<value_type> ilist)
		{
			assert(ilist.size() <= N);
			m_size = 0;
			for (auto it = ilist.begin(); it != ilist.end(); ++m_size) *(m_container.data() + m_size) = *it++;

			return *this;
		}

		void assign(size_type count, value_type const& value)
		{
			assert(count <= N);
			for (m_size = 0; m_size < count; ++m_size) *(m_container.data() + m_size) = value;
		}

		template <typename InputIterator>
		void assign(InputIterator first, InputIterator last)
		{
			assert(std::distance(first, last) <= N);
			for (m_size = 0; first != last; ++m_size) *(m_container.data() + m_size) = *first++;
		}

		void assign(std::initializer_list<value_type> ilist)
		{
			assert(ilist.size() <= N);
			m_size = 0;
			for (auto it = ilist.begin(); it != ilist.end(); ++m_size) *(m_container.data() + m_size) = *it++;
		}

		// 要素アクセス --------------------------------------------------------

		reference at(size_type pos) { return m_container.at(pos); }

		const_reference at(size_type pos) const { return m_container.at(pos); }

		reference operator[](size_type pos) { return m_container.operator[](pos); }

		const_reference operator[](size_type pos) const { return m_container.operator[](pos); }

		reference front() { return m_container.front(); }

		const_reference front() const { return m_container.front(); }

		reference back() { return *(m_container.data() + m_size - 1); }

		const_reference back() const { return *(m_container.data() + m_size - 1); }
		
		pointer data() noexcept { return m_container.data(); }

		const_pointer data() const noexcept { return m_container.data(); }

		// イテレータ ----------------------------------------------------------

		iterator begin() { return iterator(m_container, 0); }

		const_iterator begin() const { return const_iterator(m_container, 0); }

		const_iterator cbegin() const { return const_iterator(m_container, 0); }

		iterator end() { return iterator(m_container, m_size); }

		const_iterator end() const { return const_iterator(m_container, m_size); }

		const_iterator cend() const { return const_iterator(m_container, m_size); }

		reverse_iterator rbegin() { return std::make_reverse_iterator(end()); }

		const_reverse_iterator rbegin() const { return std::make_reverse_iterator(end()); }

		const_reverse_iterator crbegin() const { return std::make_reverse_iterator(cend()); }

		reverse_iterator rend() { return std::make_reverse_iterator(begin()); }

		const_reverse_iterator rend() const { return std::make_reverse_iterator(begin()); }

		const_reverse_iterator crend() const { return std::make_reverse_iterator(cbegin()); }

		// 容量 ---------------------------------------------------------------

		bool empty() const noexcept { return m_size == 0; }

		size_type size() const noexcept { return m_size; }

		size_type max_size() const noexcept { return N; }

		size_type capacity() const noexcept { return N; }

		// 変更 ---------------------------------------------------------------

		void clear() noexcept { m_size = 0; }

		iterator insert(const_iterator pos, value_type const& value)
		{
			assert(size() < N);

			std::copy_backward(pos, cend(), end() + 1);
			*(m_container.data() + pos.m_position) = value;
			++m_size;

			return iterator(m_container, pos.m_position);
		}

		iterator insert(const_iterator pos, value_type&& value)
		{
			assert(size() < N);

			std::copy_backward(pos, cend(), end() + 1);
			*(m_container.data() + pos.m_position) = std::move(value);
			++m_size;

			return iterator(m_container, pos.m_position);
		}

		iterator insert(const_iterator pos, size_type count, value_type const& value)
		{
			assert(size() + count <= N);

			std::copy_backward(pos, cend(), end() + count);

			iterator p(m_container, pos.m_position);
			for (const_iterator last = pos + count; p != last; ++p) *p = value;
			
			m_size += count;

			return iterator(m_container, pos.m_position);
		}

		template <typename InputIterator>
		iterator insert(const_iterator pos, InputIterator first, InputIterator last)
		{
			size_type count = std::distance(first, last);

			assert(size() + count <= N);

			std::copy_backward(pos, cend(), end() + count);

			iterator p(m_container, pos.m_position);
			for (const_iterator last = pos + count; p != last; ++p) *p = *first++;

			m_size += count;

			return iterator(m_container, pos.m_position);;
		}

		iterator insert(const_iterator pos, std::initializer_list<value_type> ilist)
		{
			assert(size() + ilist.size() <= N);

			std::copy_backward(pos, cend(), end() + ilist.size());

			iterator p(m_container, pos.m_position);
			auto it = ilist.begin();
			while (it != ilist.end()) *p++ = *it++;

			m_size += ilist.size();

			return iterator(m_container, pos.m_position);
		}

		iterator erase(const_iterator pos)
		{
			assert(pos != cend());

			iterator p(m_container, pos.m_position);
			std::copy(pos + 1, cend(), p);

			--m_size;

			return iterator(m_container, pos.m_position);
		}

		iterator erase(const_iterator first, const_iterator last)
		{
			assert(first != cend());
			assert(first <= last);

			std::copy(last, cend(), iterator(m_container, first.m_position));
			m_size -= std::distance(first, last);

			return iterator(m_container, first.m_position);
		}

		void push_back(value_type const& value)
		{
			assert(m_size < m_container.size());

			*(m_container.data() + m_size) = value;
			++m_size;
		}

		void push_back(value_type&& value)
		{
			assert(m_size < m_container.size());

			*(m_container.data() + m_size) = std::move(value);
			++m_size;
		}

		void pop_back()
		{
			assert(m_size != 0);

			erase(--cend());
		}

		void resize(size_type count)
		{
			resize(count, value_type());
		}

		void resize(size_type count, value_type const& value)
		{
			assert(count <= m_container.max_size());

			if (count < m_size) erase(const_iterator(m_container, count), cend());
			else if(m_size < count) insert(cend(), count - m_size, value);
		}

		void swap(static_vector& other)
		{
			m_container.swap(other.m_container);
			std::swap(m_size, other.m_size);
		}

	protected:
		container m_container;
		size_type m_size;
	};

	template <typename T1, std::size_t N1>
	inline bool operator==(static_vector<T1, N1> const& lhs, static_vector<T1, N1> const& rhs)
	{
		return std::equal(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
	}

	template <typename T1, std::size_t N1>
	inline bool operator!=(static_vector<T1, N1> const& lhs, static_vector<T1, N1> const& rhs)
	{
		return !(lhs == rhs);
	}

	template <typename T1, std::size_t N1>
	inline bool operator<(static_vector<T1, N1> const& lhs, static_vector<T1, N1> const& rhs)
	{
		return std::lexicographical_compare(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
	}

	template <typename T1, std::size_t N1>
	inline bool operator<=(static_vector<T1, N1> const& lhs, static_vector<T1, N1> const& rhs)
	{
		return lhs < rhs || lhs == rhs;
	}

	template <typename T1, std::size_t N1>
	inline bool operator>(static_vector<T1, N1> const& lhs, static_vector<T1, N1> const& rhs)
	{
		return std::lexicographical_compare(rhs.cbegin(), rhs.cend(), lhs.cbegin(), lhs.cend());
	}

	template <typename T1, std::size_t N1>
	inline bool operator>=(static_vector<T1, N1> const& lhs, static_vector<T1, N1> const& rhs)
	{
		return lhs > rhs || lhs == rhs;
	}

}
