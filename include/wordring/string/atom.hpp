#pragma once

#include <wordring/compatibility.hpp>
#include <wordring/trie/trie.hpp>

#include <string>
#include <string_view>
#include <type_traits>

namespace wordring
{
	template <typename String, typename Allocator>
	class basic_atom_set;
}

namespace wordring::detail
{
	/*! @class basic_atom atom.hpp wordring/string/atom.hpp

	@brief 文字列アトム

	このクラスの実体は整数値と文字列を返す。
	この文字列と整数値は一対一に対応する。

	@sa wordring::basic_atom_set
	*/
	template <typename String, typename Allocator = std::allocator<detail::trie_node>>
	class basic_atom : public wordring::stable_trie<typename String::value_type>::const_iterator
	{
		template <typename String1, typename Allocator1>
		friend class wordring::basic_atom_set;

		template <typename String1, typename Allocator1>
		friend bool operator==(basic_atom<String1, Allocator1> const&, basic_atom<String1, Allocator1> const&);

		template <typename String1, typename Allocator1>
		friend bool operator!=(basic_atom<String1, Allocator1> const&, basic_atom<String1, Allocator1> const&);

	protected:
		using base_type = typename stable_trie<typename String::value_type>::const_iterator;

		using typename base_type::container;
		using typename base_type::index_type;
		using typename base_type::node_type;

	public:
		using string_type = String;
		using label_type  = typename string_type::value_type;

		using base_type::operator bool;
		using base_type::operator !;
		using base_type::string;

	protected:
		using base_type::m_c;
		using base_type::m_index;

	protected:
		/*! @brief アトムを構築する

		@param [in] it 基本クラスのイテレータ
		*/
		basic_atom(base_type it)
			: base_type(it)
		{
		}

		/*! @brief アトムを構築する

		@param [in] c   ダブル・アレイの内部コンテナ
		@param [in] idx ノードの索引
		*/
		basic_atom(container const& c, index_type idx)
			: base_type(c, idx)
		{
		}

	public:
		/*! @brief 空のアトムを構築する

		@par 例
		@code
			// 空のアトムを構築する
			basic_atom<std::string> a;
		@endcode
		*/
		basic_atom()
			: base_type()
		{
		}

		/*! @brief アトムをIDに変換する
		
		@return アトムが文字列を持つ場合、IDを示す整数値、それ以外の場合0

		アトムのIDが0になることは無いため、0を無効な値として使用する。

		@par 例
		@code
			// コンテナを構築する
			std::vector<std::u32string> v{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
			auto as = basic_atom_set<std::u32string>(v.begin(), v.end());

			// コンテナからアトムを取得する
			auto a = as.at(U"い");

			// アトムをIDに変換する
			auto i = static_cast<std::uint32_t>(a);

			// 検証
			assert(i != 0);
		@endcode
		*/
		operator std::uint32_t() const
		{
			typename base_type::index_type idx = 0;

			if (m_index != 0)
			{
				node_type const* d = m_c->data();

				idx = (d + m_index)->m_base + base_type::null_value;
				assert(1 < idx && idx < base_type::limit());
				assert((d + idx)->m_check == m_index);
			}

			return idx;
		}

		/*! @brief アトムを文字列に変換する

		@return アトムが文字列を持つ場合、その文字列、それ以外の場合、空の文字列

		@par 例
		@code
			// コンテナを構築する
			std::vector<std::u32string> v{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
			auto as = basic_atom_set<std::u32string>(v.begin(), v.end());

			// コンテナからアトムを取得する
			auto a = as.at(U"い");

			// アトムを文字列に変換する
			auto s = static_cast<std::u32string>(a);

			// 検証
			assert(s == U"い");
		@endcode
		*/
		operator string_type() const
		{
			string_type result;
			base_type::string(result);
			return result;
		}

		/*! @brief アトムを文字列に変換する

		@param [out] result 文字列を書き込むバッファ

		@return アトムが文字列を持つ場合、その文字列、それ以外の場合、何もしない

		バッファが十分であれば文字列を書き込むためのメモリーを確保しないため、繰り返し呼び出すための最速の方法となる。

		@par 例
		@code
			// コンテナを構築する
			std::vector<std::u32string> v{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
			auto as = basic_atom_set<std::u32string>(v.begin(), v.end());

			// コンテナからアトムを取得する
			auto a = as.at(U"い");

			std::u32string s;
			// アトムを文字列に変換する
			a.string(s);

			// 検証
			assert(s == U"い");
		@endcode
		*/
		string_type& string(string_type& result) const
		{
			if(m_index != 0) base_type::string(result);

			return result;
		}
	};

	/* @brief 二つのアトムが等しいか調べる

		@par 例
		@code
			// アトム・コンテナを構築する
			std::vector<std::u32string> v{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
			auto as = basic_atom_set<std::u32string>(v.begin(), v.end());

			// コンテナからアトムを取得する
			auto a1 = as.at(U"あ");
			auto a2 = as.at(U"あ");

			// 二つのアトムが等しいか調べる
			assert(a1 == a2);
		@endcode
	*/
	template <typename String1, typename Allocator1>
	inline bool operator==(basic_atom<String1, Allocator1> const& lhs, basic_atom<String1, Allocator1> const& rhs)
	{
		assert(lhs.m_c == rhs.m_c);

		return lhs.m_index == rhs.m_index;
	}

	/* @brief 二つのアトムが等しくないか調べる

		@par 例
		@code
			// アトム・コンテナを構築する
			std::vector<std::u32string> v{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
			auto as = basic_atom_set<std::u32string>(v.begin(), v.end());

			// コンテナからアトムを取得する
			auto a1 = as.at(U"あ");
			auto a2 = as.at(U"あう");

			// 二つのアトムが等しくないか調べる
			assert(a1 != a2);
		@endcode
	*/
	template <typename String1, typename Allocator1>
	inline bool operator!=(basic_atom<String1, Allocator1> const& lhs, basic_atom<String1, Allocator1> const& rhs)
	{
		assert(lhs.m_c == rhs.m_c);

		return lhs.m_index != rhs.m_index;
	}

}

namespace wordring
{
	/*! @class basic_atom_set atom.hpp wordring/string/atom.hpp
	
	@brief 文字列アトムのコンテナ

	単語に番号を割り当てる辞書のようなクラス。

	文字列から整数値への変換、整数値から文字列の変換が出来る。
	整数値は、文字列に対して自動的に割り当てられるため、変更できない。

	@par 直列化

	直列化には <b>ibegin()</b>, <b>iend()</b> を使う。

	- @ref detail::trie_heap::ibegin() const
	- @ref detail::trie_heap::iend() const

	ibegin() の逆参照は32ビット整数値を返す。 ファイルへ保存するためにバイト列を必要とする場合、直列化イテレータを使う。

	- @ref wordring::serialize_iterator
	*/
	template <typename String, typename Allocator = std::allocator<detail::trie_node>>
	class basic_atom_set : public stable_trie<typename String::value_type>
	{
	protected:
		using base_type = stable_trie<typename String::value_type>;

	public:
		using allocator_type = Allocator;

		using key_type   = String;
		using value_type = detail::basic_atom<key_type>;
		using label_type = typename String::value_type;

		using typename base_type::serialize_iterator;

		using base_type::get_allocator;
		using base_type::size;
		using base_type::ibegin;
		using base_type::iend;
		using base_type::clear;

	protected:
		using base_type::null_value;

	public:
		/*! @brief 空のコンテナを構築する

		@par 例
		@code
			// 空のコンテナを構築する
			basic_atom_set<std::string> as;
		@endcode
		*/
		basic_atom_set()
			: base_type()
		{
		}

		/*! @brief アロケータを指定して空のコンテナを構築する

		@param [in] alloc アロケータ

		@par 例
		@code
			// アロケータを指定して空のコンテナを構築する
			basic_atom_set<std::string> as(std::allocator<detail::trie_node>{});
		@endcode
		*/
		explicit basic_atom_set(allocator_type const& alloc)
			: base_type(alloc)
		{
		}

		/*! @brief 直列化データからの構築

		@param [in]
			first 直列化データの先頭を指すイテレータ
		@param [in]
			last  直列化データの終端を指すイテレータ
		@param [in]
			alloc アロケータ

		@sa assign(InputIterator first, InputIterator last)

		@par 例
		@code
			// 元となるコンテナを作成
			std::vector<std::u32string> v1{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
			basic_atom_set<std::u32string> as1(v1.begin(), v1.end());

			//直列化データを作成
			auto v2 = std::vector<std::uint32_t>(as1.ibegin(), as1.iend());

			// 直列化データから構築
			auto as2 = basic_atom_set<std::u32string>(v2.begin(), v2.end());

			// 検証
			assert(as2.size() == 5);
		@endcode
		*/
		template <typename InputIterator, typename std::enable_if_t<std::is_integral_v<typename std::iterator_traits<InputIterator>::value_type>, std::nullptr_t> = nullptr>
		basic_atom_set(InputIterator first, InputIterator last, allocator_type const& alloc = allocator_type())
			: base_type(alloc)
		{
			assign(first, last);
		}

		/*! @brief 文字列リストからの構築

		@param [in]
			first 文字列リストの先頭を指すイテレータ
		@param [in]
			last 文字列リストの終端を指すイテレータ
		@param [in]
			alloc アロケータ

		@par 例
		@code
			// 文字列リスト
			std::vector<std::u32string> v{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
	
			// 文字列リストから構築
			auto as = basic_atom_set<std::u32string>(v.begin(), v.end());

			// 検証
			assert(as.size() == 5);
		@endcode
		*/
		template <typename InputIterator, typename std::enable_if_t<std::negation_v<std::is_integral<typename std::iterator_traits<InputIterator>::value_type>>, std::nullptr_t> = nullptr>
		basic_atom_set(InputIterator first, InputIterator last, allocator_type const& alloc = allocator_type())
			: base_type(alloc)
		{
			assign(first, last);
		}

		/*! @brief 初期化子リストから構築する

		@param [in]
			il ノード・データの初期化子リスト
		@param [in]
			alloc アロケータ

		@sa detail::trie_heap::trie_heap(std::initializer_list<trie_node>, allocator_type const&)
		*/
		basic_atom_set(std::initializer_list<detail::trie_node> il, allocator_type const& alloc = allocator_type())
			: base_type(il, alloc)
		{
		}

		/*! @brief 直列化データからの割り当て

		@param [in]
			first 直列化データの先頭を指すイテレータ
		@param [in]
			last 直列化データの終端を指すイテレータ

		@par 例
		@code
			// 元となるコンテナを作成
			std::vector<std::u32string> v1{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
			basic_atom_set<std::u32string> as1(v1.begin(), v1.end());

			//直列化データを作成
			auto v2 = std::vector<std::uint32_t>(as1.ibegin(), as1.iend());

			// 直列化データから割り当て
			auto as2 = basic_atom_set<std::u32string>(v2.begin(), v2.end());

			// 検証
			assert(as2.size() == 5);
		@endcode
		*/
		template <typename InputIterator, typename std::enable_if_t<std::is_integral_v<typename std::iterator_traits<InputIterator>::value_type>, std::nullptr_t> = nullptr>
		void assign(InputIterator first, InputIterator last)
		{
			base_type::assign(first, last);
		}

		/*! @brief 文字列リストからの割り当て

		@param [in]
			first 文字列リストの先頭を指すイテレータ
		@param [in]
			last  文字列リストの終端を指すイテレータ

		@par 例
		@code
			// 文字列リスト
			std::vector<std::u32string> v{ U"あ", U"あう", U"い", U"うあい", U"うえ" };

			basic_atom_set<std::u32string> as;
			// 文字列リストから割り当て
			as.assign(v.begin(), v.end());

			// 検証
			assert(as.size() == 5);
		@endcode
		*/
		template <typename InputIterator, typename std::enable_if_t<std::negation_v<std::is_integral<typename std::iterator_traits<InputIterator>::value_type>>, std::nullptr_t> = nullptr>
		void assign(InputIterator first, InputIterator last)
		{
			while (first != last) insert(*first++);
		}

		// 要素アクセス --------------------------------------------------------

		/*! @brief IDからアトムを返す

		@param [in] id アトムID

		@return 文字列アトム

		id と対応するアトムが格納されていない場合、空のアトムを返す

		@par 例
		@code
			// コンテナを作成
			std::vector<std::u32string> v1{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
			auto as = basic_atom_set<std::u32string>(v1.begin(), v1.end());

			// アトムIDを取得
			std::uint32_t id = as.at(std::u32string(U"あう"));

			// IDからアトムを取得する
			auto a = as.at(id);

			// 検証
			assert(static_cast<std::u32string>(a) == U"あう");
		@endcode
		*/
		value_type at(std::uint32_t id) const
		{
			typename base_type::node_type const* d = base_type::m_c.data();

			typename base_type::index_type idx = (d + id)->m_check;
			assert(1 < idx && idx < base_type::m_c.size());
			assert((d + idx)->m_base + base_type::null_value == id);

			return value_type(base_type::m_c, idx);
		}
			  
		/*! @brief 文字列からアトムを返す

		@param [in] first 文字列の先頭を指すイテレータ
		@param [in] last  文字列の終端を指すイテレータ

		@return 文字列アトム

		文字列と対応するアトムが格納されていない場合、空のアトムを返す

		@par 例
		@code
			// コンテナを作成
			std::vector<std::u32string> v1{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
			auto as = basic_atom_set<std::u32string>(v1.begin(), v1.end());

			auto s = std::u32string(U"あう");
			// 文字列からアトムを取得する
			auto a = as.at(s.begin(), s.end());

			// 検証
			assert(static_cast<std::u32string>(a) == U"あう");
		@endcode
		*/
		template <typename InputIterator>
		value_type at(InputIterator first, InputIterator last) const
		{
			return base_type::find(first, last);
		}

		/*! @brief 文字列からアトムを返す

		@param [in] sv 文字列

		@return 文字列アトム

		文字列と対応するアトムが格納されていない場合、空のアトムを返す

		@par 例
		@code
			// コンテナを作成
			std::vector<std::u32string> v1{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
			auto as = basic_atom_set<std::u32string>(v1.begin(), v1.end());

			// 文字列からアトムを取得する
			auto a = as.at(U"あう");

			// 検証
			assert(static_cast<std::u32string>(a) == U"あう");
		@endcode
		*/
		value_type at(std::basic_string_view<label_type> sv) const
		{
			return at(sv.begin(), sv.end());
		}

		/*! @brief 文字列からアトムを返す

		@param [in] sv 文字列

		@return 文字列アトム

		@par 例
		@code
			// コンテナを作成
			std::vector<std::u32string> v1{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
			auto as = basic_atom_set<std::u32string>(v1.begin(), v1.end());

			// 文字列からアトムを取得する
			auto a = as[U"え"];

			// 検証
			assert(static_cast<std::u32string>(a) == U"え");
		@endcode
		*/
		value_type operator[](std::basic_string_view<label_type> sv)
		{
			value_type it = base_type::find(sv.begin(), sv.end());
			if (!it) it = insert(sv.begin(), sv.end());
			return it;
		}
		
		// 変更 ---------------------------------------------------------------

		/*! @brief 文字列を挿入する

		@param [in] first 文字列の先頭を指すイテレータ
		@param [in] last  文字列の終端を指すイテレータ

		@return 文字列アトム

		@par 例
		@code
			// 空のコンテナを作成
			basic_atom_set<std::u32string> as;

			std::u32string s(U"あ");
			// 文字列を挿入する
			auto a = as.insert(s.begin(), s.end());

			// 検証
			assert(static_cast<std::u32string>(a) == U"あ");
			assert(as.size() == 1);
		@endcode
		*/
		template <typename InputIterator>
		value_type insert(InputIterator first, InputIterator last)
		{
			auto it = base_type::insert(first, last);

			typename base_type::index_type idx;
			
			if (it)
			{
				auto proxy = base_type::at(it, idx);
				proxy = idx;
			}

			return it;
		}

		/*! @brief 文字列を挿入する

		@param [in] sv 文字列

		@return 文字列アトム

		@par 例
		@code
			// 空のコンテナを作成
			basic_atom_set<std::u32string> as;

			// 文字列を挿入する
			auto a = as.insert(U"あ");

			// 検証
			assert(static_cast<std::u32string>(a) == U"あ");
			assert(as.size() == 1);
		@endcode
		*/
		value_type insert(std::basic_string_view<label_type> sv)
		{
			return insert(sv.begin(), sv.end());
		}

		/*! @brief アトムを削除する

		@param [in] id 削除するアトムID

		@par 例
		@code
			std::vector<std::u32string> v1{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
			// コンテナを構築する
			auto as = basic_atom_set<std::u32string>(v1.begin(), v1.end());

			// IDを取得する
			std::uint32_t id = as.at(U"い");

			// アトムを削除する
			as.erase(id);

			// 検証
			assert(as.contains(U"い") == false);
			assert(as.size() == 4);
		@endcode
		*/
		void erase(std::uint32_t id)
		{
			if(id != 0) base_type::erase(static_cast<typename base_type::const_iterator>(at(id)));
		}

		/*! @brief アトムを削除する

		@param [in] first 文字列の先頭を指すイテレータ
		@param [in] last  文字列の終端を指すイテレータ

		@par 例
		@code
			std::vector<std::u32string> v1{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
			// コンテナを構築する
			auto as = basic_atom_set<std::u32string>(v1.begin(), v1.end());

			auto s = std::u32string(U"うあい");
			// アトムを削除する
			as.erase(s.begin(), s.end());

			// 検証
			assert(as.contains(U"うあい") == false);
			assert(as.size() == 4);
		@endcode
		*/
		template <typename InputIterator>
		void erase(InputIterator first, InputIterator last)
		{
			base_type::erase(base_type::find(first, last));
		}

		/*! @brief アトムを削除する

		@param [in] sv 文字列

		@par 例
		@code
			std::vector<std::u32string> v1{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
			// コンテナを構築する
			auto as = basic_atom_set<std::u32string>(v1.begin(), v1.end());

			// アトムを削除する
			as.erase(U"うあい");

			// 検証
			assert(as.contains(U"うあい") == false);
			assert(as.size() == 4);
		@endcode
		*/
		void erase(std::basic_string_view<label_type> sv)
		{
			erase(sv.begin(), sv.end());
		}

		// 検索 ---------------------------------------------------------------

		/*! @brief 文字列が格納されているか調べる

		@param [in] first 文字列の先頭を指すイテレータ
		@param [in] last  文字列の終端を指すイテレータ

		@return 格納されている場合 true 、それ以外の場合 false
		*/
		template <typename InputIterator>
		bool contains(InputIterator first, InputIterator last) const
		{
			return base_type::contains(first, last);
		}

		/*! @brief 文字列が格納されているか調べる

		@param [in] sv 文字列

		@return 格納されている場合 true 、それ以外の場合 false

		@par 例
		@code
			std::vector<std::u32string> v1{ U"あ", U"あう", U"い", U"うあい", U"うえ" };
			// コンテナを構築する
			auto as = basic_atom_set<std::u32string>(v1.begin(), v1.end());

			// 文字列が格納されているか調べる
			bool b = as.contains(U"あ");

			// 検証
			assert(b == true);
		@endcode
		*/
		bool contains(std::basic_string_view<label_type> sv) const
		{
			return contains(sv.begin(), sv.end());
		}
	};

	template <typename Allocator = std::allocator<detail::trie_node>>
	using u8atom_set = basic_atom_set<std::u8string, Allocator>;

	template <typename Allocator = std::allocator<detail::trie_node>>
	using u16atom_set = basic_atom_set<std::u16string, Allocator>;

	template <typename Allocator = std::allocator<detail::trie_node>>
	using u32atom_set = basic_atom_set<std::u32string, Allocator>;
}
