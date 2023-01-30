#pragma once

#include <wordring/tag_tree/cast_iterator.hpp>
#include <wordring/tag_tree/character_iterator.hpp>
#include <wordring/tag_tree/serial_iterator.hpp>
#include <wordring/tag_tree/tag_node.hpp>
#include <wordring/tag_tree/tag_tree_iterator.hpp>

#include <wordring/html/html_defs.hpp>

#include <cassert>
#include <memory>
#include <utility>
#include <vector>

namespace wordring
{
	/*! @brief HTML/XML 用の木
	*/
	template <typename Value>
	class tag_tree
	{
	public:
		using value_type      = std::remove_cv_t<Value>;
		using size_type       = std::size_t;
		using difference_type = std::ptrdiff_t;
		using reference       = value_type&;
		using const_reference = value_type const&;
		using pointer         = value_type*;
		using const_pointer   = value_type const*;
		using iterator        = detail::tag_tree_iterator<value_type>;
		using const_iterator  = detail::const_tag_tree_iterator<value_type>;

		using wrapper   = detail::tag_node<value_type>;
		using container = std::vector<wrapper>;

		using node_traits       = html::node_traits<iterator>;
		using const_node_traits = html::node_traits<const_iterator>;
		using node_pointer      = typename node_traits::node_pointer;

		using element_type                = typename node_traits::element_type;                 // 1.  子ノード有り
		using text_type                   = typename node_traits::text_type;                    // 3.
		using processing_instruction_type = typename node_traits::processing_instruction_type;  // 7.  子ノード有り
		using comment_type                = typename node_traits::comment_type;                 // 8.
		using document_type               = typename node_traits::document_type;                // 9.  子ノード有り
		using document_type_type          = typename node_traits::document_type_type;           // 10.
		using document_fragment_type      = typename node_traits::document_fragment_type;       // 11. 子ノード有り

		using attribute_type    = typename element_type::attribute_type;
		using attribute_pointer = typename element_type::const_iterator;

		using string_type = typename element_type::string_type;

		/*! @brief 行きがかり順にノードを走査するイテレータ
		* （空の値を持つ）終了タグ相当の位置へも移動します。
		*/
		using const_serial_iterator = detail::const_tag_tree_serial_iterator<Value>;

		/*! @brief 行きがかり順にノードを走査するイテレータ
		* （空の値を持つ）終了タグ相当の位置へも移動します。
		*/
		using serial_iterator = detail::tag_tree_serial_iterator<Value>;

		/*! @brief 行きがかり順にテキスト・ノードの文字を走査するイテレータ
		*/
		using const_character_iterator = detail::const_tag_tree_character_iterator<Value>;

		/*! @brief 行きがかり順にテキスト・ノードの文字を走査するイテレータ
		*/
		using character_iterator = detail::tag_tree_character_iterator<Value>;

		/*! @brief 逆参照が wchar_t を返す、行きがかり順にテキスト・ノードの文字を走査するイテレータ
		* std::regex で必要になり用意しました。
		*/
		using const_wchar_iterator = detail::const_tag_tree_cast_iterator<wchar_t, const_iterator>;

		/*! @brief 逆参照が wchar_t を返す、行きがかり順にテキスト・ノードの文字を走査するイテレータ
		* std::regex で必要になり用意しました。
		*/
		using wchar_iterator = detail::tag_tree_cast_iterator<wchar_t, iterator>;

	public:
		tag_tree()
			: m_c(std::make_unique<container>(1, wrapper{ 0, 0, 0, 0 }))
		{}

		tag_tree(tag_tree const& rhs)
			: m_c(std::make_unique<container>(*rhs.m_c))
		{}

		tag_tree(tag_tree&& rhs)
			: m_c(std::move(rhs.m_c))
		{}

		tag_tree& operator=(tag_tree const& rhs)
		{
			m_c = std::make_unique<container>(*rhs.m_c);
			return *this;
		}

		tag_tree& operator=(tag_tree&& rhs)
		{
			m_c = std::move(rhs.m_c);
			return *this;
		}

		iterator begin() { return iterator(m_c.get(), m_c->front().m_next); }

		const_iterator begin() const { return const_iterator(m_c.get(), m_c->front().m_next); }

		const_iterator cbegin() const { return begin(); }

		iterator end() { return iterator(m_c.get(), 0); }

		const_iterator end() const { return const_iterator(m_c.get(), 0); }

		const_iterator cend() const { return end(); }

		serial_iterator sbegin() { return serial_iterator(m_c.get(), m_c->front().m_next); }

		const_serial_iterator sbegin() const { return serial_iterator(m_c.get(), m_c->front().m_next); }

		serial_iterator send() { return serial_iterator(m_c.get(), 0); }

		const_serial_iterator send() const { return serial_iterator(m_c.get(), 0); }

		bool empty() const { return m_c->data()->m_next == 0; }

		size_type size() const
		{
			size_type n = 0;

			wrapper* d = m_c->data();
			for (std::uint32_t idx = d->m_next; idx != 0; idx = (d + idx)->m_next)
			{
				std::uint32_t tail = (d + idx)->m_tail;
				std::uint32_t head = (d + idx)->m_head;
				if (tail == 0 && head != 0) continue; // 終了タグ
				++n;
			}

			return n;
		}

		void clear()
		{
			m_c->clear();
			m_c->insert(m_c->begin(), wrapper{ 0, 0, 0, 0 });
		}

		void swap(tag_tree& rhs) { std::swap(m_c, rhs.m_c); }

		iterator insert(const_iterator pos, value_type&& val)
		{
			bool single = node_traits::is_single(val);

			std::uint32_t tag = allocate(std::move(val));
			link(pos.m_i, tag);
			if (!single)
			{
				std::uint32_t end_tag = allocate(value_type());
				link(pos.m_i, end_tag);

				wrapper* d = m_c->data();
				(d + tag)->m_tail = end_tag;
				(d + end_tag)->m_head = tag;
			}
			return iterator(m_c.get(), tag);
		}

		iterator insert(const_iterator pos, value_type const& val)
		{
			return insert(pos, value_type(val));
		}

		iterator move(const_iterator pos, const_iterator sub)
		{
			wrapper* d = m_c->data();

			std::uint32_t pos_before = (d + pos.m_i)->m_prev;
			std::uint32_t pos_idx = pos.m_i;

			std::uint32_t sub_before = (d + sub.m_i)->m_prev;
			std::uint32_t sub_head = sub.m_i;
			std::uint32_t sub_tail = (d + sub_head)->m_tail == 0 ? sub_head : (d + sub_head)->m_tail;
			std::uint32_t sub_after = (d + sub_tail)->m_next;

			(d + pos_before)->m_next = sub_head;
			(d + sub_head)->m_prev = pos_before;
			
			(d + pos_idx)->m_prev = sub_tail;
			(d + sub_tail)->m_next = pos_idx;

			(d + sub_before)->m_next = sub_after;
			(d + sub_after)->m_prev = sub_before;

			return iterator(m_c.get(), sub_head);
		}

		iterator erase(const_iterator pos)
		{
			wrapper* d = m_c->data();
			std::uint32_t idx    = pos.m_i;
			std::uint32_t before = (d + idx)->m_prev; // 番兵
			std::uint32_t tail   = (d + idx)->m_tail == 0 ? idx : (d + idx)->m_tail;
			std::uint32_t after  = (d + tail)->m_next;

			while (tail != before)
			{
				std::uint32_t prev = (d + tail)->m_prev;
				unlink(tail);
				free(tail);
				tail = prev;
			}

			return iterator(m_c.get(), after);
		}

	protected:
		std::uint32_t allocate(value_type&& val)
		{
			wrapper* d = m_c->data();
			std::uint32_t idx = d->m_tail;

			if (idx == 0) // 開放済みノードが無い
			{
				idx = static_cast<std::uint32_t>(m_c->size());
				m_c->emplace_back(wrapper{ 0, 0, 0, 0, std::move(val) });
			}
			else // 開放済みノードが有る
			{
				std::uint32_t before = (d + idx)->m_head;
				std::uint32_t after = (d + idx)->m_tail;
				(d + idx)->m_value = std::move(val);
				// 使わない項目を0に初期化
				(d + idx)->m_prev = 0;
				(d + idx)->m_next = 0;

				(d + before)->m_tail = after;
				(d + after)->m_head = before;
			}

			return idx;
		}

		void free(std::uint32_t idx)
		{
			assert(idx < m_c->size());

			wrapper* d = m_c->data();

			// idx を超えない最後の未使用ノードを探す
			std::uint32_t before = 0;
			for (std::uint32_t i = d->m_tail; i != 0 && i < idx; i = (d + before)->m_tail) before = i;

			// idx を超える最初の未使用ノードを探す
			std::uint32_t after = (d + before)->m_tail;

			// リンクを張り替える
			(d + before)->m_tail = idx;

			(d + idx)->m_head = before;
			(d + idx)->m_tail = after;

			(d + after)->m_head = idx;

			// 使わない項目を0に初期化
			(d + idx)->m_prev = 0;
			(d + idx)->m_next = 0;
		}

		void link(std::uint32_t pos, std::uint32_t idx)
		{
			assert(pos < m_c->size());
			assert(idx < m_c->size());

			wrapper* d = m_c->data();

			std::uint32_t before = (d + pos)->m_prev;
			std::uint32_t after = pos;

			// リンクを張り替える
			(d + before)->m_next = idx;

			(d + idx)->m_prev = before;
			(d + idx)->m_next = after;

			(d + after)->m_prev = idx;

			// 使わない項目を0に初期化
			(d + idx)->m_head = 0;
			(d + idx)->m_tail = 0;
		}

		void unlink(std::uint32_t idx)
		{
			assert(idx < m_c->size());

			wrapper* d = m_c->data();

			std::uint32_t before = (d + idx)->m_prev;
			std::uint32_t after = (d + idx)->m_next;

			// リンクを張り替える
			(d + before)->m_next = after;
			(d + after)->m_prev = before;

			// 使わない項目を0に初期化
			(d + idx)->m_prev = 0;
			(d + idx)->m_next = 0;
			(d + idx)->m_head = 0;
			(d + idx)->m_tail = 0;
		}

	protected:
		std::unique_ptr<container> m_c;
	};
}
