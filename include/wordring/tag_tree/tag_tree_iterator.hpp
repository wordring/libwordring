#pragma once

#include <wordring/tag_tree/tag_node.hpp>

namespace wordring
{
	template <typename Value>
	class tag_tree;
}

namespace wordring::detail
{
	template <typename Value> class const_tag_tree_serial_iterator;
	template <typename Value> class tag_tree_serial_iterator;

	template <typename Value> class const_tag_tree_character_iterator;
	template <typename Value> class tag_tree_character_iterator;

	template <typename Value> class tag_tree_iterator;

	template <typename Value>
	class const_tag_tree_iterator
	{
		friend class const_tag_tree_serial_iterator<Value>;
		friend class tag_tree_serial_iterator<Value>;

		friend class const_tag_tree_character_iterator<Value>;
		friend class tag_tree_character_iterator<Value>;

		friend class tag_tree_iterator<Value>;

		friend class wordring::tag_tree<Value>;

	public:
		using value_type = std::remove_cv_t<Value>;
		using wrapper    = tag_node<value_type>;

		using difference_type   = std::ptrdiff_t;
		using reference         = value_type const&;
		using pointer           = value_type const*;
		using iterator_category = std::bidirectional_iterator_tag;

		using container = std::vector<detail::tag_node<value_type>>;

	public:
		const_tag_tree_iterator()
			: m_c(nullptr)
			, m_i(0) {}


		const_tag_tree_iterator(container* c, std::uint32_t i)
			: m_c(c)
			, m_i(i) {}

		const_tag_tree_iterator(tag_tree_iterator<Value> const& x)
			: m_c(x.m_c)
			, m_i(x.m_i) {}

		reference operator*() const { return (m_c->data() + m_i)->m_value; }

		pointer operator->() const { return &(m_c->data() + m_i)->m_value; }

		/*! @brief 一つ後ろの兄弟ノードを指すように進める
		*
		* @internal
		* <hr>
		*
		* - 対象ノードに終了タグが無ければ「TAIL」が 0 で「NEXT」が次の兄弟ノード。
		*   終了タグがある場合、「TAIL」の「NEXT」が次の兄弟ノード。
		*/
		const_tag_tree_iterator& operator++()
		{
			assert(m_i != 0);
			wrapper* d = m_c->data();

			std::uint32_t tail = (d + m_i)->m_tail;
			m_i = tail == 0 ? (d + m_i)->m_next : (d + tail)->m_next;

			return *this;
		}

		const_tag_tree_iterator operator++(int)
		{
			const_tag_tree_iterator it = *this;
			operator++();
			return it;
		}

		const_tag_tree_iterator& operator--()
		{
			wrapper* d = m_c->data();
			std::uint32_t prev = (d + m_i)->m_prev;
			assert(prev != 0);
			std::uint32_t head = (d + prev)->m_head;

			m_i = head == 0 ? prev : head;

			return *this;
		}

		/*! @brief 親ノードへのイテレータを返す
		*
		* @internal
		* <hr>
		*
		* 対象ノードのひとつ前のノードが開始タグなら、その開始タグは「親ノード」。
		* それ以外の場合、それはひとつ前の兄弟ノードの終了タグ、あるいはシングルノード。
		* 「あれば開始タグ無ければ自身」の前のノードを対象ノードとして、開始タグが見つかるまで上のアルゴリズムを繰り返せば、
		* いずれ、「親ノード」に行き着く。
		*
		* - 開始タグは「TAIL」があるノード。
		* - 終了タグは「HEAD」があるノード。
		* - 根に「親ノード」は無い。
		* - 根ノードは「PREV」が 0 のノード。
		*/
		const_tag_tree_iterator parent() const
		{
			wrapper* d = m_c->data();

			std::uint32_t idx = (d + m_i)->m_prev;
			while (idx != 0)
			{
				if ((d + idx)->m_tail != 0) return const_tag_tree_iterator(m_c, idx);

				std::uint32_t head = (d + idx)->m_head;
				if (head) idx = head;
				idx = (d + idx)->m_prev;
			}

			return const_tag_tree_iterator();
		}

		const_tag_tree_iterator begin() const
		{
			wrapper* d = m_c->data();
			std::uint32_t idx = (d + m_i)->m_tail == 0 ? 0 : (d + m_i)->m_next;
			return const_tag_tree_iterator(m_c, idx);
		}

		const_tag_tree_iterator end() const
		{
			return const_tag_tree_iterator(m_c, (m_c->data() + m_i)->m_tail);
		}

		bool operator==(const_tag_tree_iterator const& x) const
		{
			assert(m_c == nullptr || x.m_c == nullptr || m_c->data() == x.m_c->data());
			return m_i == x.m_i;
		}

		bool operator!=(const_tag_tree_iterator const& x) const
		{
			return !operator==(x);
		}

	protected:
		container*    m_c;
		std::uint32_t m_i;
	};

	template <typename Value>
	class tag_tree_iterator : public const_tag_tree_iterator<Value>
	{
		friend class const_tag_tree_serial_iterator<Value>;
		friend class tag_tree_serial_iterator<Value>;

		friend class const_tag_tree_character_iterator<Value>;
		friend class tag_tree_character_iterator<Value>;

		friend class const_tag_tree_iterator<Value>;
		friend class wordring::tag_tree<Value>;

	public:
		using value_type = std::remove_cv_t<Value>;
		using wrapper    = tag_node<value_type>;

		using difference_type   = std::ptrdiff_t;
		using reference         = value_type&;
		using pointer           = value_type*;
		using iterator_category = std::bidirectional_iterator_tag;

		using container = std::vector<detail::tag_node<value_type>>;

		using base_type = const_tag_tree_iterator<Value>;

	private:
		using base_type::m_c;
		using base_type::m_i;

	public:
		tag_tree_iterator()
			: base_type()
		{}

		tag_tree_iterator(container* c, std::uint32_t i)
			: base_type(c, i)
		{}

		operator const_tag_tree_iterator<Value>() const
		{
			return tag_tree_iterator<Value>(m_c, m_i);
		}

		reference operator*() const
		{
			return const_cast<reference>(base_type::operator*());
		}

		pointer operator->() const
		{
			return const_cast<pointer>(base_type::operator->());
		}

		/*! @brief 一つ後ろの兄弟ノードを指すように進める
		*
		* @internal
		* <hr>
		*
		* - 対象ノードに終了タグが無ければ「TAIL」が 0 で「NEXT」が次の兄弟ノード。
		*   終了タグがある場合、「TAIL」の「NEXT」が次の兄弟ノード。
		*/
		tag_tree_iterator& operator++()
		{
			base_type::operator++();
			return *this;
		}

		tag_tree_iterator operator++(int)
		{
			tag_tree_iterator it = *this;
			base_type::operator++();
			return it;
		}

		tag_tree_iterator& operator--()
		{
			base_type::operator--();
			return *this;
		}

		tag_tree_iterator operator--(int)
		{
			tag_tree_iterator it = *this;
			base_type::operator--();
			return it;
		}

		/*! @brief 親ノードへのイテレータを返す
		*
		* @internal
		* <hr>
		*
		* 対象ノードのひとつ前のノードが開始タグなら、その開始タグは「親ノード」。
		* それ以外の場合、それはひとつ前の兄弟ノードの終了タグ、あるいはシングルノード。
		* 「あれば開始タグ無ければ自身」の前のノードを対象ノードとして、開始タグが見つかるまで上のアルゴリズムを繰り返せば、
		* いずれ、「親ノード」に行き着く。
		*
		* - 開始タグは「TAIL」があるノード。
		* - 終了タグは「HEAD」があるノード。
		* - 根に「親ノード」は無い。
		* - 根ノードは「PREV」が 0 のノード。
		*/
		tag_tree_iterator parent() const
		{
			auto it = base_type::parent();
			return tag_tree_iterator(it.m_c, it.m_i);
		}

		tag_tree_iterator begin() const
		{
			auto it = base_type::begin();
			return tag_tree_iterator(it.m_c, it.m_i);
		}

		tag_tree_iterator end() const
		{
			auto it = base_type::end();
			return tag_tree_iterator(it.m_c, it.m_i);
		}
	};
}
