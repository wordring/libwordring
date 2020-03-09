#pragma once

#include <array>
#include <iterator>
#include <type_traits>

namespace wordring
{
	template <typename Label, typename Base>
	class basic_trie;
}

namespace wordring::detail
{
	/*! @brief basic_trie のイテレータ

	@tparam Label ラベルとして使用する任意の整数型
	@tparam Base  元となる trie_base::const_iterator あるいは stable_trie_base::const_iterator

	ダブル・アレイのイテレータを任意の整数型に拡張する。

	バイト単位の遷移から複数バイト単位の遷移にするために、いくつかのアルゴリズムを実装する。
	*/
	template <typename Label, typename Base>
	class const_trie_iterator : public Base
	{
		template <typename Label1, typename Base1>
		friend class wordring::basic_trie;

		template <typename Label1, typename Base1>
		friend bool operator==(const_trie_iterator<Label1, Base1> const&, const_trie_iterator<Label1, Base1> const&);

		template <typename Label1, typename Base1>
		friend bool operator!=(const_trie_iterator<Label1, Base1> const&, const_trie_iterator<Label1, Base1> const&);

	protected:
		using base_type = Base;

		using typename base_type::index_type;
		using typename base_type::node_type;
		using typename base_type::container;

		using unsigned_type = std::make_unsigned_t<Label>;

	public:
		using difference_type   = std::ptrdiff_t;
		using value_type        = Label;
		using pointer           = value_type*;
		using reference         = value_type&;
		using iterator_category = std::input_iterator_tag;

		static constexpr std::uint16_t null_value = 256u;
		static std::uint32_t constexpr coefficient = sizeof(value_type) / sizeof(typename base_type::value_type);

		static_assert(1 <= coefficient);

	public:
		using base_type::operator bool;
		using base_type::operator!;

	protected:
		using base_type::value;
		using base_type::limit;
		using base_type::find;

		using base_type::m_c;
		using base_type::m_index;

	public:
		const_trie_iterator()
			: base_type()
		{
		}

	protected:
		const_trie_iterator(container& c, index_type index)
			: base_type(c, index)
		{
		}

		const_trie_iterator(base_type const& it)
			: base_type(it)
		{
		}

	public:
		value_type operator*() const
		{
			assert(1 < m_index && m_index < limit());

			value_type result = 0;

			if constexpr (coefficient == 1) return base_type::value();
			else
			{
				node_type const* d = m_c->data();
				index_type idx = m_index;

				for (std::uint32_t i = 0; i < coefficient; ++i)
				{
					index_type parent = (d + idx)->m_check;
					assert(1 <= parent && parent < limit());

					index_type base = (d + parent)->m_base;
					assert(1 <= base && base < limit());

					result += (idx - base) << (i * 8);

					idx = parent;
				}
			}

			return result;
		}

		/*! @brief ラベルで遷移できる子を返す

		@param [in] label 遷移ラベル

		@return 遷移先のノードを指すイテレータ

		イテレータを使ってノード間を移動する場合、この関数と parent() が最速。
		operator++() は若干の探索が発生する。

		@par 例
		@code
			// Trie木を作成
			std::vector<std::u32string> v{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
			auto t = trie<char32_t>(v.begin(), v.end());

			// 根を指すイテレータを取得する
			auto it = t.begin();

			// ラベル「う」で遷移する子を取得する
			auto it1 = it[U'う'];

			// 検証
			assert(*it1 == U'う');
		@endcode
		*/
		const_trie_iterator operator[](value_type label) const
		{
			index_type idx = 0;

			if constexpr (coefficient == 1) idx = base_type::at_index(label);
			else
			{
				index_type parent = m_index;
				assert(1 <= parent && parent < base_type::limit());

				node_type const* d = m_c->data();
				for (std::uint32_t i = 0; i < coefficient; ++i)
				{
					std::uint8_t ch = static_cast<unsigned_type>(label) >> (coefficient - i - 1) * 8 & 0xFFu;
					index_type base = (d + parent)->m_base;
					idx = base + ch;

					if ((d + idx)->m_check != parent)
					{
						idx = 0;
						break;
					}

					parent = idx;
				}
			}

			return const_trie_iterator(*m_c, idx);
		}

		const_trie_iterator& operator++()
		{
			if constexpr (coefficient == 1) base_type::advance();
			else
			{
				index_type    idx = 0;
				std::uint32_t lv = 0;

				node_type const* d = m_c->data();

				// 右、あるいは右上を探す
				for (index_type i = m_index; lv < coefficient; ++lv)
				{
					index_type parent = (d + i)->m_check;
					index_type base = (d + parent)->m_base;
					// 右兄弟を探す
					i = find(i + 1, base + null_value, parent);
					if (i != 0)
					{
						idx = i;
						break;
					}
					i = parent;
				}

				// 足の長さをそろえる
				if (idx != 0)
				{
					for (; 0 < lv; --lv)
					{
						index_type parent = idx;
						index_type base = (d + idx)->m_base;
						idx = find(base, base + null_value, parent);
						assert(idx != 0);
					}
				}

				m_index = idx;
			}

			return *this;
		}

		const_trie_iterator operator++(int)
		{
			auto result = *this;
			operator++();
			return result;
		}

		/*! @brief 根からイテレータが指すノードまでのラベル列を返す
		
		@param [out] result ラベル列を出力する先のコンテナ

		出力先を使いまわすために引数でコンテナへの参照を受け取るよう設計した。
		コンテナはBidirectionalIteratorを持つ必要がある。

		@par 例
		@code
			// Trie木を作成
			std::vector<std::u32string> v{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
			auto t = trie<char32_t>(v.begin(), v.end());

			// ラベル列「うあ」で遷移したノードを指すイテレータを取得する
			auto it = t.search(std::u32string(U"うあ"));

			// 根からイテレータ「it」迄のラベル列を取得する
			std::u32string s;
			it.string(s);

			// 検証
			assert(s == U"うあ");
		@endcode
		*/
		template <typename String>
		void string(String& result) const
		{
			result.clear();
			for (auto p = *this; 1 < p.m_index; p = p.parent()) result.push_back(*p);
			std::reverse(std::begin(result), std::end(result));
		}

		/*! @brief 親を取得する
		
		@return 親ノードを指すイテレータ

		@par 例
		@code
			// Trie木を作成
			std::vector<std::u32string> v{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
			auto t = trie<char32_t>(v.begin(), v.end());

			// ラベル列「うあ」で遷移したノードを指すイテレータを取得する
			auto it = t.search(std::u32string(U"うあ"));

			// イテレータ「it」の親を取得する
			auto it1 = it.parent();

			// 検証
			assert(*it1 == U'う');
		@endcode
		*/
		const_trie_iterator parent() const
		{
			index_type idx = m_index;

			if constexpr (coefficient == 1) idx = base_type::parent_index();
			else
			{
				for (std::uint32_t lv = 0; lv < coefficient; ++lv)
				{
					idx = (m_c->data() + idx)->m_check;
					assert(1 <= idx && idx < limit());
				}
			}

			return const_trie_iterator(*m_c, idx);
		}

		const_trie_iterator begin() const
		{
			index_type idx = m_index;

			if constexpr (coefficient == 1) idx = base_type::begin_index();
			else
			{
				std::uint32_t lv = 0;

				trie_node const* d = m_c->data();
				while (lv < coefficient && idx != 0)
				{
					index_type base = (d + idx)->m_base;
					idx = (1 <= base)
						? find(base, base + null_value, idx)
						: 0;
					++lv;
				}

				idx = (lv == coefficient)
					? idx
					: 0;
			}

			return const_trie_iterator(*m_c, idx);
		}

		const_trie_iterator end() const
		{
			return const_trie_iterator(*m_c, 0);
		}
	};

	template <typename Label1, typename Base1>
	inline bool operator==(const_trie_iterator<Label1, Base1> const& lhs, const_trie_iterator<Label1, Base1> const& rhs)
	{
		assert(lhs.m_c == rhs.m_c);
		return lhs.m_index == rhs.m_index;
	}

	template <typename Label1, typename Base1>
	inline bool operator!=(const_trie_iterator<Label1, Base1> const& lhs, const_trie_iterator<Label1, Base1> const& rhs)
	{
		return !(lhs == rhs);
	}
}
