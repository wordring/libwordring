#pragma once

#include <wordring/tag_tree/tag_node.hpp>

namespace wordring
{
	template <typename Value>
	class tag_tree;
}

namespace wordring::detail
{
	template <typename Value>
	class tag_tree_serial_iterator;

	template <typename Value>
	class tag_tree_character_iterator;

	template <typename Value>
	class tag_tree_iterator
	{
		friend class tag_tree_serial_iterator<std::remove_cv_t<Value>>;
		friend class tag_tree_serial_iterator<std::remove_cv_t<Value const>>;

		friend class tag_tree_character_iterator<std::remove_cv_t<Value>>;
		friend class tag_tree_character_iterator<std::remove_cv_t<Value const>>;

		friend class wordring::tag_tree<std::remove_cv_t<Value>>;
		friend class wordring::tag_tree<std::remove_cv_t<Value> const>;

		template <typename Value1, typename Value2>
		friend bool operator==(tag_tree_iterator<Value1> const&, tag_tree_iterator<Value2> const&);

		template <typename Value1, typename Value2>
		friend bool operator!=(tag_tree_iterator<Value1> const&, tag_tree_iterator<Value2> const&);

	public:
		using value_type = std::remove_cv_t<Value>;
		using wrapper = tag_node<value_type>;

		using difference_type   = std::ptrdiff_t;
		using reference         = std::conditional_t<std::is_const_v<Value>, value_type const&, value_type&>;
		using pointer           = std::conditional_t<std::is_const_v<Value>, value_type const*, value_type*>;
		using iterator_category = std::bidirectional_iterator_tag;

		using container = std::vector<detail::tag_node<value_type>>;

	public:
		tag_tree_iterator()
			: m_c(nullptr)
			, m_i(0) {}


		tag_tree_iterator(container* c, std::uint32_t i)
			: m_c(c)
			, m_i(i) {}

		operator tag_tree_iterator<value_type const>() const
		{
			return tag_tree_iterator<value_type const>(m_c, m_i);
		}

		reference operator*() const
		{
			return const_cast<std::remove_cv_t<reference>>((m_c->data() + m_i)->m_value);
		}

		pointer operator->() const
		{
			return const_cast<std::remove_cv_t<pointer>>(&(m_c->data() + m_i)->m_value);
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
			assert(m_i != 0);
			wrapper* d = m_c->data();

			std::uint32_t tail = (d + m_i)->m_tail;
			m_i = tail == 0 ? (d + m_i)->m_next : (d + tail)->m_next;

			return *this;
		}

		tag_tree_iterator operator++(int)
		{
			tag_tree_iterator it = *this;
			operator++();
			return it;
		}

		tag_tree_iterator& operator--()
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
		tag_tree_iterator parent() const
		{
			wrapper* d = m_c->data();

			std::uint32_t idx = (d + m_i)->m_prev;
			while (idx != 0)
			{
				if ((d + idx)->m_tail != 0) return tag_tree_iterator(m_c, idx);

				std::uint32_t head = (d + idx)->m_head;
				if (head) idx = head;
				idx = (d + idx)->m_prev;
			}

			return tag_tree_iterator();
		}

		tag_tree_iterator begin() const
		{
			wrapper* d = m_c->data();
			std::uint32_t idx = (d + m_i)->m_tail == 0 ? 0 : (d + m_i)->m_next;
			return tag_tree_iterator(m_c, idx);
		}

		tag_tree_iterator end() const
		{
			return tag_tree_iterator(m_c, (m_c->data() + m_i)->m_tail);
		}

	protected:
		container* m_c;
		std::uint32_t m_i;
	};

	template <typename Value1, typename Value2>
	inline bool operator==(tag_tree_iterator<Value1> const& lhs, tag_tree_iterator<Value2> const& rhs)
	{
		assert(lhs.m_c == nullptr || rhs.m_c == nullptr || lhs.m_c->data() == rhs.m_c->data());
		return lhs.m_i == rhs.m_i;
	}

	template <typename Value1, typename Value2>
	inline bool operator!=(tag_tree_iterator<Value1> const& lhs, tag_tree_iterator<Value2> const& rhs)
	{
		return !(lhs == rhs);
	}
}
