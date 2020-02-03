#pragma once

#include <wordring/static_vector/static_vector.hpp>
//#include <wordring/tree/tree_iterator.hpp>
#include <wordring/trie/list_trie_iterator.hpp>
#include <wordring/trie/trie_construct_iterator.hpp>

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

	protected:
		using index_type = typename trie_node::index_type;
		using container  = Container const;

		static constexpr std::uint16_t null_value = 256;

	public:
		const_trie_base_iterator()
			: m_c(nullptr)
			, m_index(0)
		{
		}

	protected:
		const_trie_base_iterator(container& c, index_type index)
			: m_c(std::addressof(c))
			, m_index(index)
		{
		}

	public:
		/*! 文字列終端の場合trueを返す*/
		operator bool() const
		{
			if (m_index <= 1) return false;

			auto data = m_c->data();
			index_type idx = (data + m_index)->m_base; // 子のBASEインデックス。

			// 子が無い（BASEが1未満）場合。
			if (idx <= 0) return true;
			// 子はあるが、途中で終端もある（文字null_valueによる遷移）場合。
			if ((idx + null_value) < tail() && (data + idx + null_value)->m_check == m_index) return true;
			
			return false;
		}

		bool operator!() const { return operator bool() == false; }

		value_type operator*() const
		{
			assert(base() <= m_index && m_index < base() + null_value);

			return m_index - base();
		}

		const_trie_base_iterator operator[](value_type label) const
		{
			auto data = m_c->data();

			index_type idx = (data + m_index)->m_base + label;
			idx = (idx < tail() && (data + idx)->m_check == m_index) ? idx
				                                                     : 0;
			return const_trie_base_iterator(*m_c, idx);
		}

		const_trie_base_iterator& operator++()
		{
			m_index = find(m_index + 1, base() + null_value, mother());

			return *this;
		}

		const_trie_base_iterator operator++(int)
		{
			auto result = *this;

			operator++();

			return result;
		}

		template <typename String>
		String string() const
		{
			std::basic_string<value_type> tmp;
			for (auto p = *this; 1 < p.m_index; p = *p.parent()) tmp.push_back(*p);
			return String(tmp.rbegin(), tmp.rend());
		}

		std::optional<const_trie_base_iterator> parent() const
		{
			return 1 < m_index ? std::optional<const_trie_base_iterator>(const_trie_base_iterator(*m_c, (m_c->data() + m_index)->m_check))
				               : std::nullopt;
		}

		/*! 0-255に相当する文字で遷移できる最初の子を指すイテレータを返す
		- 256による空遷移は含めない。
		- 遷移先（子）が無い場合、end()を返す。
		*/
		const_trie_base_iterator begin() const
		{
			assert(1 <= m_index);

			index_type i = (m_c->data() + m_index)->m_base;
			i = (0 < i) ? find(i, i + null_value, m_index) : 0;
			return const_trie_base_iterator(*m_c, i);
		}

		const_trie_base_iterator end() const
		{
			return const_trie_base_iterator(*m_c, 0);
		}
		
	protected:
		/*! 引数firstからlastの範囲で、m_checkが引数checkと一致する状態番号を返す

		- 見つからない場合、0を返す。
		- 状態番号は1から始まるので0と衝突しない。
		- 引数lastがコンテナの大きさを超える場合、そこで検索を終了する。
		*/
		index_type find(index_type first, index_type last, index_type check) const
		{
			index_type limit = std::min(last, static_cast<index_type>(m_c->size()));
			for (; first < limit; ++first) if ((m_c->data() + first)->m_check == check) break;

			return (first != limit) ? first : 0;
		}

		/*! 親の状態番号を返す
		*/
		index_type mother() const
		{
			assert(1 <= m_index && m_index < tail());

			auto data = m_c->data();
			index_type parent = (data + m_index)->m_check;

			return 1 <= parent ? parent : 0;
		}

		/*! 親のBASEを返す
		- つまりコレはm_indexからラベルを差し引いた値にに相当する。
		*/
		index_type base() const
		{
			auto data = m_c->data();
			
			index_type parent = mother();
			assert(parent != 0);

			index_type idx = (parent != 0) ? (data + parent)->m_base : 0;
			assert(1 <= idx); // thisが子である以上、親のBASEが存在する。
			
			return idx;
		}

		/*! コンテナの最終状態番号の次を返す
		- キャストで行が増えるのを抑制するために用意した。
		*/
		index_type tail() const { return m_c->size(); }

		/*! 空遷移がある場合trueを返す
		*/
		bool has_null() const
		{
			if (m_index == 1) return false;

			auto data = m_c->data();
			index_type idx = (data + m_index)->m_base + null_value;

			return (idx < tail() && (data + idx)->m_check == m_index);
		}

		/*! 兄弟にあたる状態がある場合、trueを返す
		*/
		bool has_sibling() const
		{
			if (m_index == 1) return false;

			index_type check = (m_c->data() + m_index)->m_check;

			index_type first = base();
			index_type last = first + null_value;

			while(true)
			{
				first = find(first, last, check);
				if (first == 0) return false;
				if (first != m_index) return true;
				++first;
			}

			return false;
		}

		/*! 遷移可能な子ラベルを列挙する

		- null_value を含む
		*/
		void children(static_vector<std::uint16_t, 257>& labels) const
		{
			assert(1 <= m_index);
			assert(labels.empty());

			index_type base = (m_c->data() + m_index)->m_base; // 子の開始
			if (base <= 0) return; // 0以下は子が無い

			index_type first = base;
			index_type last = std::min(base + null_value + 1, static_cast<index_type>(m_c->size()));
			for (; first < last; ++first)
			{
				index_type check = (m_c->data() + first)->m_check;
				if (check == m_index) labels.push_back(static_cast<std::uint16_t>(first - base));
			}
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

	/*!
	- 挿入や削除によってすべてのイテレータが無効となる。
	*/
	template <typename Allocator = std::allocator<trie_node>>
	class trie_base
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

	protected:
		using index_type = typename trie_node::index_type;
		using container  = std::vector<trie_node, Allocator>;

		using label_vector = static_vector<std::uint16_t, 257>;

	public:
		using value_type      = trie_base_value_proxy;
		using size_type       = typename container::size_type;
		using allocator_type  = Allocator;
		using reference       = trie_base_value_proxy;
		using const_reference = trie_base_value_proxy const;
		using const_iterator  = const_trie_base_iterator<container const>;

	protected:
		static constexpr std::uint16_t null_value = const_iterator::null_value;

	public:
		trie_base()
			: m_c({ { 0, 0 }, { 0, 0 } })
			, m_size(0)
		{
			reserve();
		}

		template <typename InputIterator>
		void assign(InputIterator first, InputIterator last)
		{
			clear();

			std::sort(first, last);
			last = std::unique(first, last);

			using list_iterator = const_list_trie_iterator<InputIterator>;
			using construct_iterator = trie_construct_iterator<list_iterator>;

			auto li = list_iterator(first, last);
			auto it = construct_iterator(li);

			while (!it.empty() && !it.children().empty())
			{
				auto view = it.parent();
				auto it1 = search(view.begin(), view.end());
				if (it1.first.m_index == 0) it1.first.m_index = 1; // rootの場合。
				add(it1.first, it.children());
				++it;
			}

			m_size = std::distance(first, last);
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

		size_type size() const noexcept { return m_size; }

		size_type max_size() const noexcept { return std::numeric_limits<std::int32_t>::max() - 2; }

		// 変更 ---------------------------------------------------------------

		void clear() noexcept
		{
			m_c.clear();
			m_c.insert(m_c.begin(), 2, trie_node{ 0, 0 });
			m_size = 0;
		}

		template <typename Key>
		const_iterator insert(Key const& key)
		{
			auto it = find(key);
			if (it) return it;

			auto it1 = std::begin(key);
			auto it2 = std::end(key);

			const_iterator it3 = cbegin();
			const_iterator it4 = cend();

			// 登録済み遷移をスキップする
			for (; it1 != it2; ++it1)
			{
				auto it = it3[static_cast<std::uint8_t>(*it1)];
				if (it == it4) break;
				it3 = it;
			}

			if (it1 != it2) ++m_size;

			// 空遷移
			const_iterator it5 = it3.begin(); // 後続の遷移。
			const_iterator it6 = it3.end();

			// 既存遷移有り、新規文字列無し
			if (it5 != it6 && it1 == it2)
			{
				add(it3, null_value);
				++m_size;
			}
			// 後続遷移無し、新規文字列有り
			if (it5 == it6 && !it3.has_null() && it1 != it2)
			{
				if(it3.m_index != 1) add(it3, null_value);
			}

			// 新規文字列による遷移を追加
			for (; it1 != it2; ++it1)
			{
				std::uint16_t label = static_cast<std::uint8_t>(*it1);
				it3 = const_iterator(m_c, add(it3, label) + label);
			}

			return it3 == cbegin() ? cend() : it3;
		}

		template <typename InputIterator>
		void insert(InputIterator first, InputIterator last)
		{
			while (first != last) insert(*first++);
		}

		void erase(const_iterator pos)
		{
			if (!pos) return;

			while (true)
			{
				if (pos.has_null())
				{
					index_type idx = (m_c.data() + pos.m_index)->m_base + null_value;
					assert((m_c.data() + idx)->m_check == pos.m_index);

					free(idx);
					if (pos.begin() == pos.end()) (m_c.data() + pos.m_index)->m_base = 0;
					
					break;
				}

				std::optional<const_iterator> parent;
				if(!pos.has_sibling()) parent = pos.parent(); // 兄弟が無い場合

				free(pos.m_index);

				if (!parent.has_value()) break;
				pos = parent.value();
			}
		}

		template <typename Key>
		void erase(Key const& key) { erase(find(key)); }

		// 検索 ---------------------------------------------------------------

		/*! 部分一致検索

		- 引数first, lastは検索文字列のbegin(), end()。
		- 戻り値は一致した最後の状態と文字を指すイテレータのペア。
		*/
		template <typename Iterator>
		auto search(Iterator first, Iterator last) const->std::pair<const_iterator, Iterator>
		{
			const_iterator it1 = cbegin();
			const_iterator it2 = cend();

			for (; first != last; ++first)
			{
				auto it = it1[static_cast<std::uint8_t>(*first)];
				if (it == it2) break;
				it1 = it;
			}

			return std::make_pair(it1 == cbegin() ? cend() : it1, first);
		}

		/*! 前方一致検索
		*/
		template <typename Key>
		const_iterator search(Key const& key) const
		{
			auto pair = search(key.begin(), key.end());
			return pair.second == key.end() ? pair.first : cend();
		}

		/*! 完全一致検索

		- 子遷移があり、文字列終端でもある場合に注意。
		*/
		template <typename Key>
		const_iterator find(Key const& key) const
		{
			auto it = search(key);
			return it ? it : cend();
		}

	protected:
		void reserve(std::size_t n = 1)
		{
			index_type const i = m_c.size(); // reserveする先頭の番号

			for (index_type j = i; j < i + n; ++j) m_c.emplace_back(0, -(j + 1));

			auto data = m_c.data();
			if ((data + -(data->m_base))->m_check == 0) (data + -(data->m_base))->m_check = -i; // 
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

			assert(index < m_c.size());

			auto data = m_c.data();

			index_type i = -data->m_check;
			while ((data + i)->m_check != -index) i = -(data + i)->m_check;

			(data + i)->m_check = (data + index)->m_check;
			if ((data + index)->m_check == 0) data->m_base = -i;

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

		bool is_free(index_type base, label_vector const& labels)
		{
			if (base <= 0) return false; // マイナスの位置は使用できないため、ここで弾く。

			if (m_c.size() <= static_cast<std::uint32_t>(base + null_value)) reserve(null_value);

			auto const* data = m_c.data();
			for (std::uint16_t label : labels)
			{
				index_type check = (data + base + label)->m_check;
				if (0 < check || (0 == check && -data->m_base != base + label)) return false;
			}

			return true;
		}

		index_type modify(const_iterator parent, label_vector& labels)
		{
			index_type from = (m_c.data() + parent.m_index)->m_base; // 旧子の開始

			static_vector<std::uint16_t, 257> children; // 旧ラベル集合
			parent.children(children);

			labels.insert(labels.end(), children.begin(), children.end());
			std::sort(labels.begin(), labels.end());
			index_type to = allocate(labels); // 新子の開始

			static_vector<std::uint16_t, 257> gchildren;
			auto data = m_c.data();
			for (std::uint16_t label : children)
			{
				(data + to + label)->m_base = (data + from + label)->m_base; // 子のBASEを置換
				(data + to + label)->m_check = (data + from + label)->m_check; // 子のCHECKを置換

				// 孫のCHECKを置き換え
				index_type base = (data + from + label)->m_base;

				gchildren.clear();
				const_iterator(m_c, from + label).children(gchildren); // 孫ラベル集合

				for (std::uint16_t glabel : gchildren) (data + base + glabel)->m_check = to + label;
			}

			(data + parent.m_index)->m_base = to;

			for (auto const& label : children) free(from + label);

			return to;
		}

		index_type add(const_iterator parent, std::uint16_t label)
		{
			return add(parent, label_vector(1, label));
		}

		/*! parentの子としてラベル集合labels内の各ラベルによる遷移を挿入する

		- この配置起点を返す。
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

	protected:
		container m_c;
		size_type m_size;
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

		std::uint32_t base  = 0;
		std::uint32_t check = 0;

		std::int32_t i  = 0;
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

			if(j == 7) trie.m_c.emplace_back(base, check);
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
