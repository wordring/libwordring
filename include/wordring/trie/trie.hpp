#pragma once

#include <wordring/static_vector/static_vector.hpp>
#include <wordring/trie/list_trie.hpp>
#include <wordring/trie/trie_defs.hpp>
#include <wordring/utility/result_range.hpp>

#include <algorithm>
#include <bitset>
#include <cassert>
#include <cstdint>
#include <iterator>
#include <utility>
#include <vector>

namespace wordring
{
	struct trie_node
	{
		using index_type = std::int32_t;

		index_type m_base;
		index_type m_check;

		trie_node()
			: m_base(0)
			, m_check(0)
		{
		}

		trie_node(index_type base, index_type check)
			: m_base(base)
			, m_check(check)
		{
		}
	};

	template <typename Container>
	class const_trie_base_iterator
	{
		template <typename Allocator>
		friend class trie_base;

		template <typename Container1>
		friend bool operator==(const_trie_base_iterator<Container1> const&, const_trie_base_iterator<Container1> const&);

		template <typename Container1>
		friend bool operator!=(const_trie_base_iterator<Container1> const&, const_trie_base_iterator<Container1> const&);

	public:
		using difference_type   = std::ptrdiff_t;
		using value_type        = std::uint8_t;
		using pointer           = value_type*;
		using reference         = value_type&;
		using iterator_category = std::input_iterator_tag;

		using index_type = typename trie_node::index_type;
		using container  = Container const;

		static constexpr std::uint16_t null_value = 256;

		using result_range = result_range<const_trie_base_iterator>;

	public:
		const_trie_base_iterator()
			: m_c(nullptr)
			, m_index(0)
		{
		}

		const_trie_base_iterator(container& c, index_type index)
			: m_c(std::addressof(c))
			, m_index(index)
		{
		}

		operator bool() const
		{
			if (m_index <= 1) return false;

			auto data = m_c->data();
			index_type i = (data + m_index)->m_base;

			if (i <= 0) return true;
			if (static_cast<std::uint32_t>(i + null_value) < m_c->size() && (data + i + null_value)->m_check == m_index) return true;
			
			return false;
		}

		value_type operator*() const
		{
			assert(base() <= m_index && m_index < base() + null_value);

			return m_index - base();
		}

		const_trie_base_iterator operator[](value_type label) const
		{
			index_type i = (m_c->data() + m_index)->m_base + label;
			i = static_cast<std::uint32_t>(i) < m_c->size() && (m_c->data() + i)->m_check == m_index ? i : 0;

			return const_trie_base_iterator(*m_c, i);
		}

		const_trie_base_iterator& operator++()
		{
			index_type b = base();
			index_type i = find(m_index + 1, b + null_value, mother());

			m_index = i < b + null_value ? i : 0;

			return *this;
		}

		const_trie_base_iterator operator++(int)
		{
			auto result = *this;

			operator++();

			return result;
		}

		result_range parent() const
		{
			return result_range(begin(), end());
		}

		const_trie_base_iterator begin() const
		{
			index_type i = (m_c->data() + m_index)->m_base;
			i = 0 < i ? find(i, i + null_value, m_index) : 0;
			return const_trie_base_iterator(*m_c, i);
		}

		const_trie_base_iterator end() const
		{
			return const_trie_base_iterator(*m_c, 0);
		}
		
	protected:
		index_type find(index_type first, index_type last, index_type check) const
		{
			index_type limit = std::min(last, static_cast<index_type>(m_c->size()));
			for (; first < limit; ++first) if ((m_c->data() + first)->m_check == check) break;
		
			return first != limit ? first : base() + null_value;
		}

		index_type mother() const
		{
			auto data = m_c->data();
			return (data + m_index)->m_check;
		}

		index_type base() const
		{
			auto data = m_c->data();
			index_type parent = mother();
			return parent == 0 ? (data + m_index)->m_base : (data + parent)->m_base;
		}

	protected:
		container* m_c;
		index_type m_index;
	};

	template <typename Container1>
	inline bool operator==(const_trie_base_iterator<Container1> const& lhs, const_trie_base_iterator<Container1> const& rhs)
	{
		assert(lhs.m_c == rhs.m_c);
		return lhs.m_index == rhs.m_index;
	}

	template <typename Container1>
	inline bool operator!=(const_trie_base_iterator<Container1> const& lhs, const_trie_base_iterator<Container1> const& rhs)
	{
		return !(lhs == rhs);
	}

	// ------------------------------------------------------------------------
	// trie_base
	// ------------------------------------------------------------------------

	template <typename Allocator = std::allocator<trie_node>>
	class trie_base
	{
	public:
		using index_type = typename trie_node::index_type;
		using container = std::vector<trie_node, Allocator>;

		using label_vector = static_vector<std::uint16_t, 257>;

		using const_iterator = const_trie_base_iterator<container const>;

		static constexpr std::uint16_t null_value = const_iterator::null_value;

	public:
		trie_base()
			: m_c({ { 0, 0 }, { 0, 0 } })
			, m_size(0)
		{
			reserve(1022);
		}

	protected:
		void reserve(std::size_t n = 1024)
		{
			index_type const i = m_c.size(); // reserveする先頭の番号

			for (std::size_t j = i; j < i + n; ++j) m_c.emplace_back(0, -(j + 1)); 

			auto data = m_c.data();
			if((data + -(data->m_base))->m_check == 0) (data + -(data->m_base))->m_check = -i; // 
			data->m_base = -(m_c.size() - 1); // 最後の空きノード番号を設定
			m_c.back().m_check = 0;
		}

		/*! indexで指定されるフリー・ノードを使用可能状態にする
		*/
		void allocate(index_type index)
		{
			assert(0 < index);
			assert(m_c.data()->m_check < 0);
			assert((m_c.data() + index)->m_check <= 0);

			auto data = m_c.data();

			index_type i = -data->m_check;
			while ((data + i)->m_check != -index) i = -(data + i)->m_check;

			(data + i)->m_check = (data + index)->m_check;
			if((data + index)->m_check == 0) data->m_base = -i;

			(data + index)->m_check = 0; // 使用するノードを初期化
		}

		/*! 引数labelsで指定されるすべてのラベル位置を使用出来るbase位置を検索し返す。

		- 空きがない場合、新たに空きを確保する。
		- ラベルに対応するすべての使用位置はフリー・リストから削除され、使用状態となる。
		- 親のBASEに入る値が返る。
		*/
		index_type allocate(label_vector const& labels)
		{
			assert(std::distance(labels.begin(), labels.end()) != 0);

			for (index_type i = -m_c.data()->m_check; i != 0; i = -(m_c.data() + i)->m_check)
			{
				index_type base = i - *labels.begin();
				if (is_free(base, labels))
				{
					for (std::uint16_t label : labels) allocate(base + label);
					return base;
				}
			}

			reserve();

			return allocate(labels);
		}

		void free(index_type index)
		{
			assert(0 < index);

			auto data = m_c.data();

			(data + index)->m_check = 0;
			(data + index)->m_base = 0;

			(data - data->m_base)->m_check = -index;
			data->m_base = -index;
		}

		bool is_free(index_type base, label_vector const& labels) const
		{
			if (base <= 0) return false; // マイナスの位置は使用できないため、ここで弾く。

			auto const* d = m_c.data();
			for (std::uint16_t label : labels)
			{
				index_type i = base + label;
				if (m_c.size() <= static_cast<std::uint32_t>(i)) return false;
				if (0 <= (d + i)->m_check && -d->m_base != i) return false;
			}
			
			return true;
		}

		index_type modify(const_iterator parent, label_vector& labels)
		{
			struct child { std::uint16_t m_label;  index_type m_base; };

			static_vector<child, 257> children;
			index_type from = (m_c.data() + parent.m_index)->m_base;

			if (parent)
				children.push_back(child{ static_cast<std::uint16_t>(from + null_value), (m_c.data() + from + null_value)->m_base });
			for (std::uint16_t label : parent)
				children.push_back(child{ label, (m_c.data() + from + label)->m_base });

			for (auto const& c : children)
			{
				labels.push_back(c.m_label);
				free(from + c.m_label);
			}
			index_type to = allocate(labels);

			auto data = m_c.data();
			for (auto const& c : children)
			{
				(data + to + c.m_label)->m_base = c.m_base; // 子のBASEを置き換え
				// 孫のCHECKを置き換え
				if (0 < c.m_base)
				{
					for (index_type l = 0; l <= 256; ++l)
					{
						index_type& j = (data + c.m_base + l)->m_check;
						if(0 < j) j = to + c.m_label;
					}
				}
			}
			(data + parent.m_index)->m_base = to;

			return to;
		}

		index_type add(const_iterator parent, std::uint16_t label)
		{
			return add(parent, label_vector(1, label));
		}

		/*! parentの子としてラベル集合labels内の各ラベルによる遷移を挿入する
		*/
		index_type add(const_iterator parent, label_vector labels)
		{
			index_type base = (m_c.data() + parent.m_index)->m_base; // 遷移先配置の起点（遷移先が定義されていない場合0）

			if (0 < base && is_free(base, labels)) for (auto label : labels) allocate(base + label);
			else if (base == 0) base = allocate(labels);
			else base = modify(parent, labels);

			(m_c.data() + parent.m_index)->m_base = base;
			for (auto label : labels) (m_c.data() + base + label)->m_check = parent.m_index;

			return base;
		}

	public:
		template <typename Iterator>
		void assign(const_iterator parent, const_list_trie_iterator<Iterator> root)
		{

		}

		template <typename String>
		const_iterator insert(String const& string)
		{
			auto it1 = string.begin();
			auto it2 = string.end();

			const_iterator it3 = cbegin();
			const_iterator it4 = cend();

			// 登録済み遷移をスキップする
			for (; it1 != it2; ++it1)
			{
				auto it = it3[static_cast<std::uint8_t>(*it1)];
				if (it == it4) break;
				it3 = it;
			}

			// 空遷移
			if ((it3 && it1 != it2) || (it3.begin() != it3.end() && it1 == it2)) add(it3, null_value);

			// 残りを追加
			for (; it1 != it2; ++it1)
			{
				std::uint16_t label = static_cast<std::uint8_t>(*it1);
				it3 = const_iterator(m_c, add(it3, label) + label);
			}

			return it3;
		}

		void erase(uint32_t index);
		void find();
		void insert(uint32_t index, uint8_t label);

		template <typename Iterator>
		void insert(Iterator first, Iterator last)
		{
			//triplet_trie t(first, last);
			//std::sort(t.begin(), t.end());
			//auto c = t.children(t.root());
		}

		// イテレータ ----------------------------------------------------------

		const_iterator begin() const noexcept { return const_iterator(m_c, 1); }

		const_iterator cbegin() const noexcept { return const_iterator(m_c, 1); }

		const_iterator end() const noexcept { return const_iterator(m_c, 0); }

		const_iterator cend() const noexcept { return const_iterator(m_c, 0); }

	protected:
		container     m_c;
		std::uint32_t m_size;
	};

	template <typename Key, typename Value>
	class basic_trie : public trie_base<>
	{
	public:
		using key_type = Key;
		using value_type = Value;
		using character = typename key_type::value_type;
	};

	template <typename Key, typename Value>
	using trie = basic_trie<Key, Value>;
}
