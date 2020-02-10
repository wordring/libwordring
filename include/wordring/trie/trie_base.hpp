#pragma once

#include <wordring/static_vector/static_vector.hpp>
#include <wordring/trie/list_trie_iterator.hpp>
#include <wordring/trie/trie_base_iterator.hpp>
#include <wordring/trie/trie_construct_iterator.hpp>
#include <wordring/trie/trie_heap.hpp>

#include <algorithm>
#include <cassert>
#include <climits>
#include <cstdint>
#include <deque>
#include <istream>
#include <iterator>
#include <memory>
#include <optional>
#include <ostream>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

namespace wordring
{
	struct trie_base_value_proxy
	{
		using index_type = typename trie_node::index_type;

		index_type* m_base;

		trie_base_value_proxy(index_type* base)
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
			return -*m_base;
		}
	};


	// ------------------------------------------------------------------------
	// trie_base
	// ------------------------------------------------------------------------

	/*!
	- 挿入や削除によってすべてのイテレータが無効となる。
	*/
	template <typename Allocator = std::allocator<trie_node>>
	class trie_base : public trie_heap<Allocator>
	{
		template <typename Allocator1>
		friend std::basic_ostream<char>& operator<<(std::basic_ostream<char>&, trie_base<Allocator1> const&);

		template <typename Allocator1>
		friend std::basic_istream<char>& operator>>(std::basic_istream<char>&, trie_base<Allocator1>&);

		template <typename Allocator1>
		friend std::vector<std::uint8_t>& operator<<(std::vector<std::uint8_t>&, trie_base<Allocator1> const&);

		template <typename Allocator1>
		friend std::vector<std::uint8_t>& operator>>(std::vector<std::uint8_t>&, trie_base<Allocator1>&);

		template <typename Allocator1>
		friend std::vector<std::uint32_t>& operator<<(std::vector<std::uint32_t>&, trie_base<Allocator1> const&);

		template <typename Allocator1>
		friend std::vector<std::uint32_t>& operator>>(std::vector<std::uint32_t>&, trie_base<Allocator1>&);

	public:
		using base_type = trie_heap<Allocator>;
		using container = typename base_type::container;

		using label_vector = typename base_type::label_vector;

		using value_type = trie_base_value_proxy;
		using size_type = typename container::size_type;
		using allocator_type = Allocator;
		using reference = trie_base_value_proxy;
		using const_reference = trie_base_value_proxy const;
		using const_iterator = const_trie_base_iterator<container const>;

		using index_type = typename trie_node::index_type;

		static constexpr std::uint16_t null_value = base_type::null_value;

	protected:
		using typename base_type::node_type;

		using base_type::trie_heap;
		using base_type::limit;
		using base_type::reserve;
		using base_type::allocate;
		using base_type::relocate;
		using base_type::free;
		using base_type::locate;
		using base_type::is_free;

		using base_type::m_c;

	public:
		trie_base()
			: base_type()
		{
		}

		template <typename ForwardIterator>
		void assign(ForwardIterator first, ForwardIterator last)
		{
			clear();

			if (!std::is_sorted(first, last) || std::adjacent_find(first, last) != last)
			{
				insert(first, last);
				return;
			}

			using list_iterator = const_list_trie_iterator<ForwardIterator>;
			using construct_iterator = trie_construct_iterator<list_iterator>;

			auto li = list_iterator(first, last);
			auto it = construct_iterator(li);

			while (!it.empty() && !it.children().empty())
			{
				auto view = it.parent();
				auto it1 = search(view.begin(), view.end());
				if (it1.first.m_index == 0) it1.first.m_index = 1; // rootの場合。
				add(it1.first.m_index, it.children());
				++it;
			}

			m_c.front().m_base = std::distance(first, last);
		}

		// 要素アクセス --------------------------------------------------------

		template <typename Key>
		reference at(Key const& key)
		{
			auto it = find(key);
			if (it == cend()) throw std::out_of_range("");

			// 子遷移が有り、なおかつ文字列終端の場合に対応する。
			index_type idx = ((m_c.data() + it.m_index)->m_base <= 0)
				? it.m_index
				: it.base() + null_value;

			return reference(std::addressof((m_c.data() + idx)->m_base));
		}

		template <typename Key>
		const_reference const at(Key const& key) const
		{
			return const_cast<trie_base*>(this)->at(key);
		}

		template <typename Key>
		reference operator[](Key const& key)
		{
			auto it = find(key);
			if (it == cend()) it = insert(key);

			// 子遷移が有り、なおかつ文字列終端の場合に対応する。
			index_type idx = ((m_c.data() + it.m_index)->m_base <= 0)
				? it.m_index
				: it.base() + null_value;

			return reference(std::addressof((m_c.data() + idx)->m_base));
		}

		// イテレータ ----------------------------------------------------------

		const_iterator begin() const noexcept { return const_iterator(m_c, 1); }

		const_iterator cbegin() const noexcept { return const_iterator(m_c, 1); }

		const_iterator end() const noexcept { return const_iterator(m_c, 0); }

		const_iterator cend() const noexcept { return const_iterator(m_c, 0); }

		// 容量 ---------------------------------------------------------------

		bool empty() const noexcept { return size() == 0; }

		size_type size() const noexcept { return m_c.front().m_base; }

		static constexpr size_type max_size() noexcept
		{
			return std::numeric_limits<std::int32_t>::max() / sizeof(node_type);
		}

		// 変更 ---------------------------------------------------------------

		void clear() noexcept
		{
			m_c.clear();
			m_c.insert(m_c.begin(), 2, trie_node{ 0, 0 });
		}

		template <typename Key>
		const_iterator insert(Key const& key)
		{
			auto it1 = std::begin(key);
			auto it2 = std::end(key);
			if (it1 == it2) return cend();

			index_type parent = 1;

			// 登録済み遷移をスキップする
			for (index_type idx = at(parent, *it1); idx != 0; idx = at(parent, *it1))
			{
				parent = idx;
				++it1;
				if (it1 == it2) break;
			}
			if (it1 != it2) ++m_c.front().m_base;

			// 空遷移の解決
			if (has_child(parent)) // 既存遷移有り、新規文字列無し
			{
				if (it1 == it2 && !has_null(parent))
				{
					add(parent, null_value);
					++m_c.front().m_base;
				}
			}
			else if (parent != 1 && !has_null(parent) && it1 != it2) // 後続遷移無し、新規文字列有り
				add(parent, null_value);

			// 新規文字列による遷移を追加
			while (it1 != it2)
			{
				std::uint16_t label = static_cast<std::uint8_t>(*it1++);
				index_type base = add(parent, label);
				parent = base + label;
			}

			assert(parent != 1);

			return const_iterator(m_c, parent);
		}

		template <typename InputIterator>
		void insert(InputIterator first, InputIterator last)
		{
			while (first != last) insert(*first++);
		}

		void erase(const_iterator pos)
		{
			index_type idx = pos.m_index;
			assert(0 <= idx && idx < limit());

			if (idx <= 1 || !is_tail(idx)) return;

			while (true)
			{
				if (has_null(idx))
				{
					index_type i = (m_c.data() + idx)->m_base + null_value;
					assert((m_c.data() + i)->m_check == idx);

					free(i);
					if (!has_child(idx)) (m_c.data() + idx)->m_base = 0;

					break;
				}

				index_type parent = 0;
				if (!has_sibling(idx)) parent = (m_c.data() + idx)->m_check; // 兄弟が無い場合
				assert(0 <= parent && parent < limit());

				if (idx != 1) free(idx);
				if (parent == 0) break;

				idx = parent;
			}

			--m_c.front().m_base;
		}

		template <typename Key>
		void erase(Key const& key) { erase(find(key)); }

		// 検索 ---------------------------------------------------------------

		/*! 部分一致検索

		- 引数first, lastは検索文字列のbegin(), end()。
		- 戻り値は一致した最後の状態と次の文字を指すイテレータのペア。

		- 一文字も一致しない場合、cbegin()を返す。
		*/
		template <typename InputIterator>
		auto search(InputIterator first, InputIterator last) const
			->std::pair<const_iterator, InputIterator>
		{
			index_type parent = 1;

			while (first != last)
			{
				std::uint16_t ch = static_cast<std::uint8_t>(*first);
				index_type idx = at(parent, ch);
				if (idx == 0) break;
				++first;
				parent = idx;
				assert(1 <= parent && parent < limit());
			}

			return std::make_pair(const_iterator(m_c, parent), first);
		}

		/*! 前方一致検索

		- 空文字列に対してはcbegin()を返す。
		- この動作は再検討が必要かもしれない。
		*/
		template <typename Key>
		const_iterator search(Key const& key) const
		{
			auto it1 = std::begin(key);
			auto it2 = std::end(key);

			auto pair = search(it1, it2);

			return (pair.second == it2)
				? pair.first
				: cend();
		}

		/*! 完全一致検索
		*/
		template <typename Key>
		const_iterator find(Key const& key) const
		{
			auto it1 = std::begin(key);
			auto it2 = std::end(key);

			auto pair = search(it1, it2);

			auto it = pair.second;
			index_type idx = pair.first.m_index;

			return (it == it2 && is_tail(idx))
				? pair.first
				: cend();
		}

		template <typename Key>
		bool contains(Key const& key) const
		{
			auto it1 = std::begin(key);
			auto it2 = std::end(key);

			auto pair = search(it1, it2);

			auto it = pair.second;
			index_type idx = pair.first.m_index;

			return (it != it2 || !is_tail(idx))
				? false
				: true;
		}

	protected:
		/*! idxが遷移終端に達しているか、あるいはnull_valueによる遷移を持つ場合、trueを返す
		- has_childとhas_nullの組み合わせで表現できるが、検索高速化のため独立して用意した。
		*/
		bool is_tail(index_type idx) const
		{
			assert(1 <= idx && idx < limit());

			node_type const* d = m_c.data();
			index_type base = (d + idx)->m_base; // 子のBASEインデックス。

			return ((base <= 0 && idx != 1) || (base + null_value < limit() && (d + base + null_value)->m_check == idx))
				? true
				: false;
		}

		/*! parentからのラベルによる遷移がある場合、trueを返す
		*/
		bool has_child(index_type parent) const
		{
			assert(1 <= parent && parent < limit());

			node_type const* d = m_c.data();

			index_type base = (d + parent)->m_base;
			assert(0 <= base && base < limit());
			index_type last = std::min(base + null_value, limit());
			assert(1 <= last && last <= limit());

			for (index_type idx = base; 1 <= idx && idx < last; ++idx)
			{
				assert((d + idx)->m_check < limit());
				if ((d + idx)->m_check == parent) return true;
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
			assert(0 <= base && base < limit());
			index_type idx = base + null_value;

			return idx < limit() && (d + idx)->m_check == parent;
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

			index_type idx = (d + parent)->m_base + label;
			index_type check = (idx < limit()) ? (d + idx)->m_check : 0;

			return (check == parent) ? idx : 0;
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

			if (base == 0)
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
	};

	template <typename Allocator1>
	inline std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, trie_base<Allocator1> const& trie)
	{
		std::uint32_t n = trie.m_size;
		for (int i = 3; 0 <= i; --i) os.put(static_cast<std::uint8_t>((n >> i * 8) & 0xFFu));

		n = trie.m_c.size();
		for (int i = 3; 0 <= i; --i) os.put(static_cast<std::uint8_t>((n >> i * 8) & 0xFFu));

		for (auto const& p : trie.m_c)
		{
			std::uint32_t base = p.m_base;
			for (int i = 3; 0 <= i; --i) os.put(static_cast<std::uint8_t>((base >> i * 8) & 0xFFu));
			std::uint32_t check = p.m_check;
			for (int i = 3; 0 <= i; --i) os.put(static_cast<std::uint8_t>((check >> i * 8) & 0xFFu));
		}

		return os;
	}

	template <typename Allocator1>
	inline std::basic_istream<char>& operator>>(std::basic_istream<char>& is, trie_base<Allocator1>& trie)
	{
		trie.m_c.clear();

		char ch;
		std::uint32_t n = 0;

		std::uint32_t base = 0;
		std::uint32_t check = 0;

		std::int32_t i = 0;
		std::uint32_t j = 0;

		for (i = 3; 0 <= i; --i)
		{
			is.get(ch);
			if (!is || is.eof()) goto Error;

			trie.m_size += static_cast<std::uint8_t>(ch) << i * 8;
		}

		for (i = 3; 0 <= i; --i)
		{
			is.get(ch);
			if (!is || is.eof()) goto Error;

			n += static_cast<std::uint8_t>(ch) << i * 8;
		}
		n *= 8;

		for (i = 0; i < n; ++i)
		{
			is.get(ch);
			if (!is || is.eof()) goto Error;

			j = i % 8;

			if (j == 0) base = check = 0;

			if (j < 4) base += static_cast<std::uint8_t>(ch) << (3 - j) * 8;
			else check += static_cast<std::uint8_t>(ch) << (7 - j) * 8;

			if (j == 7) trie.m_c.emplace_back(base, check);
		}
		goto End;

	Error:
		trie.clear();
		throw std::invalid_argument("");
	End:
		return is;
	}

	template <typename Allocator1>
	inline std::vector<std::uint8_t>&
		operator<<(std::vector<std::uint8_t>& v, trie_base<Allocator1> const& trie)
	{
		v.reserve(trie.size() * 8 + 8);

		std::uint32_t n = trie.m_size;
		for (int i = 3; 0 <= i; --i) v.push_back(static_cast<std::uint8_t>((n >> i * 8) & 0xFFu));

		n = trie.m_c.size();
		for (int i = 3; 0 <= i; --i) v.push_back(static_cast<std::uint8_t>((n >> i * 8) & 0xFFu));

		for (auto const& p : trie.m_c)
		{
			std::uint32_t base = p.m_base;
			for (int i = 3; 0 <= i; --i) v.push_back(static_cast<std::uint8_t>((base >> i * 8) & 0xFFu));
			std::uint32_t check = p.m_check;
			for (int i = 3; 0 <= i; --i) v.push_back(static_cast<std::uint8_t>((check >> i * 8) & 0xFFu));
		}

		return v;
	}

	template <typename Allocator1>
	inline std::vector<std::uint8_t>&
		operator>>(std::vector<std::uint8_t>& v, trie_base<Allocator1>& trie)
	{
		trie.m_c.clear();

		std::uint32_t n = 0;

		std::uint32_t base = 0;
		std::uint32_t check = 0;

		std::int32_t i = 0;
		std::uint32_t j = 0;

		auto it1 = v.begin();
		auto it2 = v.end();

		for (i = 3; 0 <= i; --i)
		{
			if (it1 == it2) goto Error;
			trie.m_size += *it1 << i * 8;
			++it1;
		}

		for (i = 3; 0 <= i; --i)
		{
			if (it1 == it2) goto Error;
			n += *it1 << i * 8;
			++it1;
		}
		n *= 8;

		for (i = 0; i < n; ++i)
		{
			if (it1 == it2) goto Error;
			j = i % 8;

			if (j == 0) base = check = 0;

			if (j < 4) base += *it1 << (3 - j) * 8;
			else check += *it1 << (7 - j) * 8;

			if (j == 7) trie.m_c.emplace_back(base, check);

			++it1;
		}
		goto End;

	Error:
		trie.clear();
		throw std::invalid_argument("");
	End:
		return v;
	}

	template <typename Allocator1>
	inline std::vector<std::uint32_t>&
		operator<<(std::vector<std::uint32_t>& v, trie_base<Allocator1> const& trie)
	{
		v.reserve(trie.size() * 2 + 2);
		v.push_back(trie.size());
		v.push_back(trie.m_c.size());
		for (auto n : trie.m_c)
		{
			v.push_back(n.m_base);
			v.push_back(n.m_check);
		}

		return v;
	}

	template <typename Allocator1>
	inline std::vector<std::uint32_t>&
		operator>>(std::vector<std::uint32_t>& v, trie_base<Allocator1>& trie)
	{
		trie.m_c.clear();
		trie.m_c.reserve((v.size() - 2) / 2);

		auto it1 = v.begin();
		auto it2 = v.end();

		std::uint32_t size = 0;

		if (it1 != it2) trie.m_size = *it1++;
		if (it1 != it2) size = *it1++;

		if (size * 2 + 2 != v.size())
		{
			trie.clear();
			throw std::invalid_argument("");
		}

		while (it1 != it2)
		{
			std::int32_t base = *it1++;
			std::int32_t check = *it1++;
			trie.m_c.push_back(trie_node{ base, check });
		}

		return v;
	}
}
