#pragma once

#include <wordring/serialize/serialize.hpp>
#include <wordring/serialize/serialize_iterator.hpp>
#include <wordring/static_vector/static_vector.hpp>

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <initializer_list>
#include <istream>
#include <iterator>
#include <ostream>
#include <type_traits>
#include <vector>

namespace wordring
{
	// ------------------------------------------------------------------------
	// trie_node
	// ------------------------------------------------------------------------

	struct trie_node
	{
		using index_type = std::int32_t;

		index_type m_base;
		index_type m_check;
	};

	inline bool operator==(trie_node const& lhs, trie_node const& rhs)
	{
		return lhs.m_base == rhs.m_base && lhs.m_check == rhs.m_check;
	}

	// ------------------------------------------------------------------------
	// trie_value_proxy
	// ------------------------------------------------------------------------

	struct trie_value_proxy
	{
		using index_type = typename trie_node::index_type;

		index_type* m_base;

		trie_value_proxy()
			: m_base(nullptr)
		{
		}

		trie_value_proxy(index_type* base)
			: m_base(base)
		{
		}

		void operator=(index_type val)
		{
			if (val < 0) throw std::invalid_argument("");
			*m_base = -val;
		}

		operator index_type() const
		{
			assert(*m_base <= 0);
			return -*m_base;
		}
	};

	// ------------------------------------------------------------------------
	// trie_heap_serialize_iterator
	// ------------------------------------------------------------------------

	/*! trie_heap を直列化するためのイテレータ
	- 値として32bit整数を返す。
	*/
	template <typename Container>
	class trie_heap_serialize_iterator
	{
		template <typename Allocator1>
		friend class trie_heap;

		template <typename Container1>
		friend bool operator==(trie_heap_serialize_iterator<Container1> const&, trie_heap_serialize_iterator<Container1> const&);

		template <typename Container1>
		friend bool operator!=(trie_heap_serialize_iterator<Container1> const&, trie_heap_serialize_iterator<Container1> const&);

	public:
		using difference_type   = std::ptrdiff_t;
		using value_type        = std::uint32_t;
		using pointer           = value_type*;
		using reference         = value_type&;
		using iterator_category = std::input_iterator_tag;

	protected:
		using node_type = trie_node;
		using container = Container const;

	public:
		trie_heap_serialize_iterator()
			: m_c(nullptr)
			, m_index(0)
		{
		}

	protected:
		trie_heap_serialize_iterator(container const& c, std::uint32_t n)
			: m_c(std::addressof(c))
			, m_index(n * 2)
		{
		}

	public:
		value_type operator*() const
		{
			node_type const* p = m_c->data() + (m_index / 2);
			return (m_index % 2) ? p->m_check : p->m_base;
		}

		trie_heap_serialize_iterator& operator++()
		{
			++m_index;
			return *this;
		}

		trie_heap_serialize_iterator operator++(int)
		{
			auto result = *this;
			++m_index;
			return result;
		}

	protected:
		container*    m_c;
		std::uint32_t m_index;
	};

	template <typename Container1>
	inline bool operator==(trie_heap_serialize_iterator<Container1> const& lhs, trie_heap_serialize_iterator<Container1> const& rhs)
	{
		assert(lhs.m_c == rhs.m_c);
		return lhs.m_index == rhs.m_index;
	}

	template <typename Container1>
	inline bool operator!=(trie_heap_serialize_iterator<Container1> const& lhs, trie_heap_serialize_iterator<Container1> const& rhs)
	{
		assert(lhs.m_c == rhs.m_c);
		return lhs.m_index != rhs.m_index;
	}

	// ------------------------------------------------------------------------
	// trie_heap
	// ------------------------------------------------------------------------

	template <typename Allocator>
	class trie_heap
	{
		template <typename Allocator1>
		friend std::ostream& operator<<(std::ostream&, trie_heap<Allocator1> const&);

		template <typename Allocator1>
		friend std::istream& operator>>(std::istream&, trie_heap<Allocator1>&);

	protected:
		using index_type   = typename trie_node::index_type;
		using node_type    = trie_node;
		using container    = std::vector<trie_node, Allocator>;
		using label_vector = static_vector<std::uint16_t, 257>;

		static constexpr std::uint16_t null_value = 256u;

	public:
		using label_type         = std::uint8_t;
		using allocator_type     = Allocator;
		using serialize_iterator = trie_heap_serialize_iterator<container const>;

	public:
		allocator_type get_allocator() const { return m_c.get_allocator(); }

		/*! 直列化データからの復元
		*/
		template <typename InputIterator, typename std::enable_if_t<std::is_integral_v<typename std::iterator_traits<InputIterator>::value_type>, std::nullptr_t> = nullptr>
		void assign(InputIterator first, InputIterator last)
		{
			using iterator_category = typename std::iterator_traits<InputIterator>::iterator_category;
			using value_type        = typename std::iterator_traits<InputIterator>::value_type;
			using unsigned_type     = std::make_unsigned_t<value_type>;

			std::uint32_t constexpr n = sizeof(index_type) / sizeof(value_type);

			static_assert(sizeof(value_type) <= sizeof(index_type));

			m_c.clear();
			if constexpr (std::is_same_v<iterator_category, std::random_access_iterator_tag>)
				m_c.reserve(std::distance(first, last) / 2);

			while (first != last)
			{
				std::make_unsigned_t<index_type> base = 0;
				std::make_unsigned_t<index_type> check = 0;

				if constexpr (n == 1)
				{
					base = *first++;
					check = *first++;
				}
				else
				{
					for (std::uint32_t j = 0; j < n && first != last; ++j) base = (base << 8) + static_cast<unsigned_type>(*first++);
					for (std::uint32_t j = 0; j < n && first != last; ++j) check = (check << 8) + static_cast<unsigned_type>(*first++);
				}

				m_c.push_back(node_type{ static_cast<index_type>(base), static_cast<index_type>(check) });
			}
		}

		/*! 直列化用のイテレータを返す
		*/
		serialize_iterator ibegin() const
		{
			return serialize_iterator(m_c, 0);
		}

		/*! 直列化用のイテレータを返す
		*/
		serialize_iterator iend() const
		{
			return serialize_iterator(m_c, m_c.size());
		}

		// 変更 ---------------------------------------------------------------

		void clear() noexcept
		{
			m_c.clear();
			m_c.insert(m_c.begin(), 2, trie_node{ 0, 0 });
		}

		void swap(trie_heap& other) { m_c.swap(other.m_c); }

	protected:
		trie_heap()
			: m_c(2, { 0, 0 })
		{
		}

		explicit trie_heap(allocator_type const& alloc)
			: m_c(2, { 0, 0 }, alloc)
		{
		}

		trie_heap(std::initializer_list<trie_node> il, allocator_type const& alloc = allocator_type())
			: m_c(2, { 0, 0 }, alloc)
		{
			m_c.insert(m_c.cend(), il);
		}

		index_type limit() const { return m_c.size(); }

		void reserve(std::size_t n, index_type before = 0)
		{
			assert(0 <= before  && before < limit());

			index_type id = m_c.size(); // reserveする先頭の番号
			m_c.insert(m_c.end(), n, { 0, 0 });

			node_type* d = m_c.data();
			// 値が0のINDEXを探す
			for (index_type i = -(d + before)->m_check; i != 0; i = -(d + before)->m_check)
			{
				assert(i < limit());
				before = i;
			}
			// CHECKを更新する
			for (index_type last = m_c.size(); id != last; before = id++)
			{
				assert(before < limit());
				(d + before)->m_check = -id;
			}
		}

		/*! idxで指定される空きノードを使用可能にする
		*/
		void allocate(index_type idx, index_type before = 0)
		{
			assert(1 < idx && idx < limit());
			assert(m_c[idx].m_check <= 0);
			assert(0 <= before && before < limit());

			node_type* d = m_c.data();
			// CHECKがidxと一致するINDEXを探す
			for (index_type i = -(d + before)->m_check; i != idx; i = -(d + before)->m_check)
			{
				assert(i < limit());
				before = i;
			}
			// CHECKを更新する
			(d + before)->m_check = (d + idx)->m_check;
			(d + idx)->m_check = 0;
		}

		/*! base + labelsを使用可能にする
		*/
		void allocate(index_type base, label_vector const& labels, index_type before = 0)
		{
			assert(1 <= base);
			assert(!labels.empty());
			assert(std::is_sorted(labels.begin(), labels.end()));
			assert(before < limit());

			if (limit() <= base + labels.back()) reserve(base + labels.back() + 1 - m_c.size());

			trie_node* d = m_c.data();
			for (std::uint16_t label : labels)
			{
				index_type idx = base + label;
				assert(idx < limit());
				if (1 <= (d + idx)->m_check) continue; // 登録済み

				for (index_type i = before; i != idx; i = -(d + before)->m_check)
				{
					assert(i < limit());
					before = i;
				}

				(d + before)->m_check = (d + idx)->m_check;
				(d + idx)->m_check = 0;
			}
		}

		/*! ノードを移動させる
		*/
		index_type relocate(index_type parent, index_type from, label_vector const& labels)
		{
			assert(1 <= parent && parent < limit());
			assert(1 <= from && from < limit());

			label_vector children;

			index_type last = std::min(from + null_value, limit() - 1);
			for (index_type i = from; i <= last; ++i)
			{
				assert(i < limit());
				assert(0 <= i - from);
				if ((m_c.data() + i)->m_check == parent) children.push_back(i - from);
			}

			label_vector all;
			std::set_union(labels.begin(), labels.end(), children.begin(), children.end(), std::back_inserter(all));

			index_type before = 0;
			index_type to = locate(all, before);
			allocate(to, all, before);

			node_type* d = m_c.data();
			for (std::uint16_t label : children)
			{
				index_type idx = from + label; // 子の旧INDEX
				assert(idx < limit());

				assert(to + label < limit());
				(d + to + label)->m_base = (d + idx)->m_base; // 子のBASEを置換
				(d + to + label)->m_check = (d + idx)->m_check; // 子のCHECKを置換

				// 孫のCHECKを置き換え
				index_type base = (d + idx)->m_base;
				if (1 <= base)
				{
					index_type last = std::min(base + null_value, limit() - 1);
					for (index_type i = base; i <= last; ++i)
					{
						assert(i < limit());
						if ((d + i)->m_check == idx) (d + i)->m_check = to + label;
					}
				}
			}

			assert(parent < limit());
			(d + parent)->m_base = to;
			free(from, children);

			return to;
		}

		/*! idxを解放して未使用リンクにつなげる
		- before を返す。
		- これが次のfree呼び出しのヒントとして使える。
		*/
		index_type free(index_type idx, index_type before = 0)
		{
			assert(1 < idx && idx < limit());
			assert(0 <= before && before < limit());

			node_type* d = m_c.data();

			for (index_type i = -(d + before)->m_check; i != 0 && i < idx; i = -(d + before)->m_check)
			{
				assert(i < limit());
				before = i;
			}

			(d + idx)->m_base = 0;
			(d + idx)->m_check = (d + before)->m_check;
			(d + before)->m_check = -idx;

			return before;
		}

		/*! base + labelsを解放して未使用リンクにつなげる
		*/
		void free(index_type base, label_vector const& labels, index_type before = 0)
		{
			assert(1 <= base && base < limit());
			assert(1 <= base && base + labels.back() < limit());

			assert(!labels.empty());
			assert(std::is_sorted(labels.begin(), labels.end()));

			assert(0 <= before && before < limit());

			for (auto label : labels)
			{
				assert(base + label < limit());
				before = free(base + label, before);
			}
		}

		/*! 未使用ノードを検索する

		- labelsが配置される最初のノードの直前の空きノードをbeforeで返す。
		- これがallocate呼び出しのヒントとして使える。

		- INDEX1に子は配置されない。
		*/
		index_type locate(label_vector const& labels, index_type& before) const
		{
			assert(!labels.empty());
			assert(std::is_sorted(labels.begin(), labels.end()));

			index_type base = 0;
			before = 0;

			std::uint16_t offset = labels.front();

			trie_node const* d = m_c.data();
			index_type idx = -d->m_check;

			// BASEが負にならないよう、検索開始位置を設定する。
			for (; 0 < idx && idx <= offset; idx = -(d + idx)->m_check) before = idx;

			// BASEを正に調整可能な位置に一つでもラべルを配置可能な空きノードがある場合、それに基づき計算する。
			if (offset < idx)
			{
				for(; 0 != idx && !is_free(idx - offset, labels); idx = -(d + idx)->m_check) before = idx;
				if (idx != 0) base = idx - offset;
			}

			// そのような空きノードが無い場合、すべてのラベルが新規にreserveされるノードに配置される。
			if (base == 0) base = std::max(limit() - offset, 1);

			assert(1 <= base);
			assert(0 <= before && before < base + labels.front());

			return base;
		}

		/*! base + labelsがすべて未使用である場合、trueを返す
		*/
		bool is_free(index_type base, label_vector const& labels) const
		{
			assert(1 <= base);
			assert(!labels.empty());

			node_type const* d = m_c.data();
			for (std::uint16_t label : labels)
			{
				if (label == 0 && base == 1) return false; // index 1のcheckは常に0のため

				index_type idx = base + label;
				if (limit() <= idx) break;

				if (1 <= (d + idx)->m_check) return false;
			}

			return true;
		}

		/*! base + labelsすべてについて、未使用あるいはparentからの遷移である場合、trueを返す
		*/
		bool is_free(index_type parent, index_type base, label_vector const& labels) const
		{
			assert(1 <= parent && parent < limit());
			assert(1 <= base);
			assert(!labels.empty());

			node_type const* d = m_c.data();
			for (std::uint16_t label : labels)
			{
				if (label == 0 && base == 1) return false; // index 1のcheckは常に0のため

				index_type idx = base + label;
				if (limit() <= idx) break;

				index_type check = (d + idx)->m_check;
				if (1 <= check && parent != check) return false;
			}

			return true;
		}

		/*! idxが遷移終端に達しているか、あるいはnull_valueによる遷移を持つ場合、trueを返す
		- has_childとhas_nullの組み合わせで表現できるが、検索高速化のため独立して用意した。
		*/
		bool is_tail(index_type idx) const
		{
			assert(1 <= idx && idx < limit());

			node_type const* d = m_c.data();
			index_type base = (d + idx)->m_base; // 子のBASEインデックス。

			return (base <= 0 && idx != 1)
				|| (1 <= base && base + null_value < limit() && (d + base + null_value)->m_check == idx);
		}

		/*! parentからのラベルによる遷移がある場合、trueを返す
		*/
		bool has_child(index_type parent) const
		{
			assert(1 <= parent && parent < limit());

			node_type const* d = m_c.data();

			index_type base = (d + parent)->m_base;
			assert(base < limit());

			if (1 <= base)
			{
				index_type last = std::min(base + null_value, limit());
				assert(1 <= last && last <= limit());

				for (index_type idx = base; 1 <= idx && idx < last; ++idx)
				{
					assert((d + idx)->m_check < limit());
					if ((d + idx)->m_check == parent) return true;
				}
			}

			return false;
		}

		/*! parentからの空遷移がある場合、trueを返す
		*/
		bool has_null(index_type parent) const
		{
			assert(1 <= parent && parent < limit());

			node_type const* d = m_c.data();

			index_type base = (d + parent)->m_base;
			assert(base < limit());

			if (1 <= base)
			{
				index_type idx = base + null_value;
				return idx < limit() && (d + idx)->m_check == parent;
			}

			return false;
		}

		/*! idxにnull_value以外の兄弟がある場合、trueを返す
		*/
		bool has_sibling(index_type idx) const
		{
			assert(0 <= idx && idx < limit());
			if (idx <= 1) return false;

			node_type const* d = m_c.data();

			index_type parent = (d + idx)->m_check;
			assert(1 <= parent && parent < limit());

			index_type base = (d + parent)->m_base;
			assert(1 <= base && base < limit());

			index_type last = std::min(base + null_value, limit());
			assert(1 <= last && last <= limit());

			for (index_type i = base; i < last; ++i)
			{
				assert((d + i)->m_check < limit());
				index_type check = (d + i)->m_check;
				if (check == parent && i != idx) return true;
			}

			return false;
		}

		/*! idxにnull_value以外の兄弟がある場合、trueを返す
		*/
		bool has_sibling(index_type parent, index_type idx) const
		{
			assert(1 <= parent && parent < limit());
			assert(0 <= idx && idx < limit());

			if (idx <= 1) return false;

			node_type const* d = m_c.data();

			index_type base = (d + parent)->m_base;
			assert(1 <= base && base < limit());

			index_type last = std::min(base + null_value, limit());
			assert(1 <= last && last <= limit());

			for (index_type i = base; i < last; ++i)
			{
				assert((d + i)->m_check < limit());
				index_type check = (d + i)->m_check;
				if (check == parent && i != idx) return true;
			}

			return false;
		}

		/*! parentからlabelで遷移したINDEXを返す
		- 遷移先が無ければ0を返す。
		*/
		index_type at(index_type parent, std::uint16_t label) const
		{
			assert(1 <= parent && parent < limit());
			assert(static_cast<index_type>(label) <= null_value);

			node_type const* d = m_c.data();

			index_type base  = (d + parent)->m_base;
			index_type idx   = 0;
			index_type check = 0;
			
			if (1 <= base)
			{
				idx = base + label;
				if (idx < limit()) check = (d + idx)->m_check;
				if (check != parent) idx = 0;
			}

			return idx ;
		}

		index_type add(index_type parent, std::uint16_t label)
		{
			return add(parent, label_vector(1, label));
		}

		/*! parentの子としてラベル集合labels内の各ラベルによる遷移を挿入する

		- 配置起点を返す。
		*/
		index_type add(index_type parent, label_vector const& labels)
		{
			assert(parent < limit());
			assert(!labels.empty());
			assert(std::is_sorted(labels.begin(), labels.end()));

			index_type before = 0;
			index_type base = (m_c.data() + parent)->m_base; // 遷移先配置の起点（遷移先が定義されていない場合0）
			assert(base < limit());

			if (base <= 0) // 子が無い。
			{
				base = locate(labels, before);
				allocate(base, labels, before);
			}
			else if (is_free(parent, base, labels)) allocate(base, labels, before);
			else base = relocate(parent, base, labels);

			assert(base + static_cast<index_type>(labels.back()) < limit());
			node_type* d = m_c.data();

			(d + parent)->m_base = base;
			for (std::uint16_t label : labels) (d + base + label)->m_check = parent;

			assert(1 <= base && base < limit());

			return base;
		}

	protected:
		container m_c;
	};

	template <typename Allocator1>
	inline std::ostream& operator<<(std::ostream& os, trie_heap<Allocator1> const& heap)
	{
		std::uint64_t n = static_cast<std::uint64_t>(heap.m_c.size()) * sizeof(trie_node);
		auto length = serialize(n);
		auto it1 = length.begin();
		auto it2 = length.end();
		while (it1 != it2) os.put(*it1++);

		auto it3 = serialize_iterator(heap.ibegin());
		auto it4 = serialize_iterator(heap.iend());
		while (it3 != it4) os.put(*it3++);

		return os;
	}

	template <typename Allocator1>
	inline std::istream& operator>>(std::istream& is, trie_heap<Allocator1>& heap)
	{
		heap.m_c.clear();

		auto it1 = std::istreambuf_iterator<char>(is);
		auto it2 = std::istreambuf_iterator<char>();

		std::uint64_t n;
		it1 = deserialize(it1, it2, n);

		for (std::uint64_t i = 0; i < n && it1 != it2; ++i)
		{
			std::int32_t base, check;
			it1 = deserialize(it1, it2, base);
			it1 = deserialize(it1, it2, check);
			heap.m_c.push_back({ base, check });
		}

		return is;
	}
}
