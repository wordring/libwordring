#pragma once

#include <wordring/html/html_atom.hpp>
#include <wordring/html/html_defs.hpp>

#include <wordring/encoding/encoding.hpp>

#include <wordring/whatwg/infra/infra.hpp>

#include <iterator>
#include <optional>
#include <variant>
#include <vector>

namespace wordring::html
{
	// ---------------------------------------------------------------------------------------------
	// 属性
	// ---------------------------------------------------------------------------------------------

	/*! @brief simple_html 用の属性ノード

	@tparam String 取り扱う文字列の型
	*/
	template <typename String>
	class simple_attr
	{
		template <typename String1>
		friend bool operator==(simple_attr<String1> const&, simple_attr<String1> const&);

		template <typename String1>
		friend bool operator==(simple_attr<String1> const&, attribute_name);

		template <typename String1>
		friend bool operator==(attribute_name, simple_attr<String1> const&);

		template <typename String1>
		friend bool operator==(simple_attr<String1> const&, String1 const&);

		template <typename String1>
		friend bool operator==(String1 const&, simple_attr<String1> const&);

		template <typename String1>
		friend bool operator!=(simple_attr<String1> const&, simple_attr<String1> const&);

		template <typename String1>
		friend bool operator!=(simple_attr<String1> const&, attribute_name);

		template <typename String1>
		friend bool operator!=(attribute_name, simple_attr<String1> const&);

		template <typename String1>
		friend bool operator!=(simple_attr<String1> const&, String1 const&);

		template <typename String1>
		friend bool operator!=(String1 const&, simple_attr<String1> const&);

	public:
		using string_type = String;

		using namespace_uri_type = basic_html_atom<string_type, ns_name>;
		using local_name_type    = basic_html_atom<string_type, attribute_name>;

	public:
		simple_attr()
			: m_namespace_uri(static_cast<ns_name>(0))
			, m_prefix()
			, m_local_name(static_cast<attribute_name>(0))
			, m_value()
		{
		}

		simple_attr(ns_name ns, string_type const& prefix, attribute_name name, string_type const& val = string_type())
			: m_namespace_uri(ns)
			, m_prefix(prefix)
			, m_local_name(name)
			, m_value(val)
		{
		}

		simple_attr(ns_name ns, string_type const& prefix, string_type const& name, string_type const& val = string_type())
			: m_namespace_uri(ns)
			, m_prefix(prefix)
			, m_local_name(name)
			, m_value(val)
		{
		}

		simple_attr(attribute_name name, string_type const& val = string_type())
			: m_namespace_uri(ns_name::HTML)
			, m_prefix()
			, m_local_name(name)
			, m_value(val)
		{
		}

		simple_attr(string_type const& name, string_type const& val = string_type())
			: m_namespace_uri(ns_name::HTML)
			, m_prefix()
			, m_local_name(name)
			, m_value(val)
		{
		}

		string_type namespace_uri() const { return static_cast<string_type>(m_namespace_uri); }

		void namespace_uri(string_type const& uri) { m_namespace_uri = uri; }

		ns_name namespace_uri_id() const { return m_namespace_uri; }

		void namespace_uri_id(ns_name uri) { m_namespace_uri = uri; }

		string_type const& prefix() const { return m_prefix; }

		void prefix(string_type const& s) { m_prefix = s; }

		string_type local_name() const { return static_cast<string_type>(m_local_name); }

		void local_name(string_type const& name) { m_local_name = name; }

		attribute_name local_name_id() const { return m_local_name; }

		void local_name_id(attribute_name name) { m_local_name = name; }

		string_type qualified_name() const
		{
			string_type s = prefix();
			if (!s.empty()) wordring::to_string(U':', std::back_inserter(s));
			s.append(local_name());

			return s;
		}

		string_type const& value() const { return m_value; }

		void value(string_type const& s) { m_value = s; }

	protected:
		namespace_uri_type m_namespace_uri;
		string_type        m_prefix;
		local_name_type    m_local_name;

		string_type        m_value;
	};

	/*! @brief 名前空間、接頭辞、ローカル名が一致する場合、true を返す
	
	検索用に定義した演算子のため、値を無視して比較する。
	*/
	template <typename String1>
	inline bool operator==(simple_attr<String1> const& lhs, simple_attr<String1> const& rhs)
	{
		return lhs.m_namespace_uri == rhs.m_namespace_uri
			&& lhs.m_prefix == rhs.m_prefix
			&& lhs.m_local_name == rhs.m_local_name;
	}

	template <typename String1>
	inline bool operator==(simple_attr<String1> const& lhs, attribute_name local_name)
	{
		return lhs == simple_attr<String1>(local_name);
	}

	template <typename String1>
	inline bool operator==(attribute_name local_name, simple_attr<String1> const& rhs)
	{
		return simple_attr<String1>(local_name) == rhs;
	}

	template <typename String1>
	inline bool operator==(simple_attr<String1> const& lhs, String1 const& local_name)
	{
		return lhs == simple_attr<String1>(local_name);
	}

	template <typename String1>
	inline bool operator==(String1 const& local_name, simple_attr<String1> const& rhs)
	{
		return simple_attr<String1>(local_name) == rhs;
	}

	template <typename String1>
	inline bool operator!=(simple_attr<String1> const& lhs, simple_attr<String1> const& rhs)
	{
		return !(lhs == rhs);
	}

	template <typename String1>
	inline bool operator!=(simple_attr<String1> const& lhs, attribute_name local_name)
	{
		return !(lhs == local_name);
	}

	template <typename String1>
	inline bool operator!=(attribute_name local_name, simple_attr<String1> const& rhs)
	{
		return !(local_name == rhs);
	}

	template <typename String1>
	inline bool operator!=(simple_attr<String1> const& lhs, String1 const& local_name)
	{
		return !(lhs == local_name);
	}

	template <typename String1>
	inline bool operator!=(String1 const& local_name, simple_attr<String1> const& rhs)
	{
		return !(local_name == rhs);
	}

	// --------------------------------------------------------------------------------------------
	// 文書
	// --------------------------------------------------------------------------------------------

	/*! @brief simple_html 用の Document ノード

	@tparam String 取り扱う文字列の型
	*/
	template <typename String>
	class simple_document
	{
		template <typename String1>
		friend bool operator==(simple_document<String1> const&, simple_document<String1> const&);

		template <typename String1>
		friend bool operator!=(simple_document<String1> const&, simple_document<String1> const&);

	public:
		using string_type = String;
		using usv_string_type = std::u32string;

	public:
		simple_document()
			: m_document_type(document_type_name::Html)
			, m_document_mode(document_mode_name::NoQuirks)
		{
		}

		document_type_name document_type() const { return m_document_type; }

		void document_type(document_type_name type)
		{
			m_document_type = type;
		}

		document_mode_name document_mode() const { return m_document_mode; }

		void document_mode(document_mode_name mode)
		{
			m_document_mode = mode;
		}

	protected:
		document_type_name m_document_type;
		document_mode_name m_document_mode;
	};

	template <typename String1>
	inline bool operator==(simple_document<String1> const&, simple_document<String1> const&)
	{
		assert(false);
		return false;
	}

	template <typename String1>
	inline bool operator!=(simple_document<String1> const& lhs, simple_document<String1> const& rhs)
	{
		return !(lhs == rhs);
	}

	// --------------------------------------------------------------------------------------------
	// 文書型
	// --------------------------------------------------------------------------------------------

	/*! @brief simple_html 用の DocumentType ノード

	@tparam String 取り扱う文字列の型
	*/
	template <typename String>
	class simple_document_type
	{
		template <typename String1>
		friend bool operator==(simple_document_type<String1> const&, simple_document_type<String1> const&);

		template <typename String1>
		friend bool operator!=(simple_document_type<String1> const&, simple_document_type<String1> const&);

	public:
		using string_type = String;

	public:
		simple_document_type(string_type const& name, string_type const& public_id, string_type const& system_id)
			: m_name(name)
			, m_public_id(public_id)
			, m_system_id(system_id)
		{
		}

		string_type const& name() const { return m_name; }

		void name(string_type const& s) { m_name = s; }

		string_type const& public_id() const { return m_public_id; }

		void public_id(string_type const& s) { m_public_id = s; }

		string_type const& system_id() const { return m_system_id; }

		void system_id(string_type const& s) { m_system_id = s; }

	protected:
		string_type m_name;
		string_type m_public_id;
		string_type m_system_id;
	};

	template <typename String1>
	inline bool operator==(simple_document_type<String1> const&, simple_document_type<String1> const&)
	{
		assert(false);
		return false;
	}

	template <typename String1>
	inline bool operator!=(simple_document_type<String1> const& lhs, simple_document_type<String1> const& rhs)
	{
		return !(lhs == rhs);
	}

	// --------------------------------------------------------------------------------------------
	// 文書片
	// --------------------------------------------------------------------------------------------

	/*! @brief simple_html 用の DocumentFragment ノード

	@tparam String 取り扱う文字列の型
	*/
	template <typename String>
	class simple_document_fragment
	{
		template <typename String1>
		friend bool operator==(simple_document_fragment<String1> const&, simple_document_fragment<String1> const&);

		template <typename String1>
		friend bool operator!=(simple_document_fragment<String1> const&, simple_document_fragment<String1> const&);

	public:
		using string_type = String;


	};

	template <typename String1>
	inline bool operator==(simple_document_fragment<String1> const&, simple_document_fragment<String1> const&)
	{
		assert(false);
		return false;
	}

	template <typename String1>
	inline bool operator!=(simple_document_fragment<String1> const& lhs, simple_document_fragment<String1> const& rhs)
	{
		return !(lhs == rhs);
	}

	// --------------------------------------------------------------------------------------------
	// 要素
	// --------------------------------------------------------------------------------------------

	/*! @brief simple_html 用の Element ノード

	@tparam String 取り扱う文字列の型
	*/
	template <typename String>
	class simple_element
	{
		template <typename String1>
		friend bool operator==(simple_element<String1> const&, simple_element<String1> const&);

		template <typename String1>
		friend bool operator!=(simple_element<String1> const&, simple_element<String1> const&);

	public:
		using string_type = String;

		using namespace_uri_type = basic_html_atom<string_type, ns_name>;
		using local_name_type    = basic_html_atom<string_type, tag_name>;

		using attribute_type = simple_attr<string_type>;
		using container      = std::vector< attribute_type>;
		using iterator       = typename container::iterator;
		using const_iterator = typename container::const_iterator;

	public:
		simple_element()
			: m_namespace_uri(static_cast<ns_name>(0))
			, m_namespace_prefix()
			, m_local_name(static_cast<tag_name>(0))
			, m_attributes()
		{
		}

		simple_element(string_type const& ns, string_type const& prefix, string_type const& name)
			: m_namespace_uri(ns)
			, m_namespace_prefix(prefix)
			, m_local_name(name)
			, m_attributes()
		{
		}

		simple_element(ns_name ns, string_type const& prefix, string_type const& name)
			: m_namespace_uri(ns)
			, m_namespace_prefix(prefix)
			, m_local_name(name)
			, m_attributes()
		{
		}

		simple_element(string_type const& name)
			: m_namespace_uri(ns_name::HTML)
			, m_namespace_prefix()
			, m_local_name(name)
			, m_attributes()
		{
		}

		simple_element(ns_name ns, string_type const& prefix, tag_name name)
			: m_namespace_uri(ns)
			, m_namespace_prefix(prefix)
			, m_local_name(name)
			, m_attributes()
		{
		}

		simple_element(tag_name name)
			: m_namespace_uri(ns_name::HTML)
			, m_namespace_prefix()
			, m_local_name(name)
			, m_attributes()
		{
		}

		/*! @brief 名前空間 URI を返す
		 
		@sa https://triple-underscore.github.io/DOM4-ja.html#locate-a-namespace-prefix
		*/
		string_type namespace_uri() const { return static_cast<string_type>(m_namespace_uri); }

		void namespace_uri(string_type const& uri) { m_namespace_uri = uri; }

		ns_name  namespace_uri_id() const { return m_namespace_uri; }

		void  namespace_uri_id(ns_name ns) { m_namespace_uri = ns; }

		string_type namespace_prefix() const { return m_namespace_prefix; }

		void namespace_prefix(string_type const& prefix) { m_namespace_prefix = prefix; }

		string_type local_name() const { return static_cast<string_type>(m_local_name); }

		void local_name(string_type const& name) { m_local_name = name; }

		tag_name local_name_id() const { return m_local_name; }

		void local_name_id(tag_name name) { m_local_name = name; }

		string_type qualified_name() const
		{
			string_type s = namespace_prefix();
			if (!s.empty()) wordring::to_string(U':', std::back_inserter(s));
			s.append(local_name());
			
			return s;
		}

		// 属性
		void push_back(attribute_type const& attr) { m_attributes.push_back(std::move(attr)); }

		iterator begin() { return m_attributes.begin(); }

		const_iterator begin() const { return m_attributes.begin(); }

		iterator end() { return m_attributes.end(); }

		const_iterator end() const { return m_attributes.end(); }

		/*! @brief 属性を検索する
		*/
		const_iterator find(attribute_type const& attr) const
		{
			return std::find(m_attributes.begin(), m_attributes.end(), attr);
		}

		/*! @brief 属性を検索する
		*/
		const_iterator find(ns_name ns, string_type const& prefix, string_type const& name) const
		{
			return std::find_if(m_attributes.begin(), m_attributes.end(), [&](attribute_type const& a)->bool {
				return a == attribute_type(ns, prefix, name); });
		}

		/*! @brief 属性を検索する
		*/
		const_iterator find(string_type const& name) const
		{
			return find(ns_name::HTML, string_type(), name);
		}

		/*! @brief 属性を検索する
		*/
		const_iterator find(ns_name ns, string_type const& prefix, attribute_name name) const
		{
			return std::find_if(m_attributes.begin(), m_attributes.end(), [&](attribute_type const& a)->bool {
				return a == attribute_type(ns, prefix, name); });
		}

		/*! @brief 属性を検索する
		*/
		const_iterator find(attribute_name name) const
		{
			return find(ns_name::HTML, string_type(), name);
		}

	private:
		namespace_uri_type m_namespace_uri;
		string_type        m_namespace_prefix;
		local_name_type    m_local_name;

		container m_attributes;
	};

	template <typename String1>
	inline bool operator==(simple_element<String1> const& lhs, simple_element<String1> const& rhs)
	{
		using attribute_type = typename simple_element<String1>::attribute_type;

		if (lhs.m_attributes.size() != rhs.m_attributes.size()) return false;

		for (attribute_type const& a : lhs.m_attributes)
		{
			if (std::find(rhs.m_attributes.begin(), rhs.m_attributes.end(), a) == rhs.m_attributes.end()) return false;
		}

		return true;
	}

	template <typename String1>
	inline bool operator!=(simple_element<String1> const& lhs, simple_element<String1> const& rhs)
	{
		return !(lhs == rhs);
	}

	// --------------------------------------------------------------------------------------------
	// テキスト
	// --------------------------------------------------------------------------------------------

	/*! @brief simple_html 用の Text ノード

	@tparam String 取り扱う文字列の型
	*/
	template <typename String>
	class simple_text
	{
		template <typename String1>
		friend bool operator==(simple_text<String1> const&, simple_text<String1> const&);

		template <typename String1>
		friend bool operator!=(simple_text<String1> const&, simple_text<String1> const&);

	public:
		using string_type = String;
		using value_type = typename string_type::value_type;

	public:
		simple_text() = default;

		simple_text(string_type const& s)
			: m_data(s)
		{
		}

		string_type const& data() const { return m_data; }

		string_type& data() { return m_data; }

		void data(string_type const& s) { m_data = s; }

		void push_back(value_type ch) { m_data.push_back(ch); }

	protected:
		string_type m_data;
	};

	template <typename String1>
	inline bool operator==(simple_text<String1> const&, simple_text<String1> const&)
	{
		assert(false);
		return false;
	}

	template <typename String1>
	inline bool operator!=(simple_text<String1> const& lhs, simple_text<String1> const& rhs)
	{
		return !(lhs == rhs);
	}

	// ---------------------------------------------------------------------------------------------
	// 処理命令
	// ---------------------------------------------------------------------------------------------

	/*! @brief simple_html 用の ProcessingInstruction ノード

	@tparam String 取り扱う文字列の型
	*/
	template <typename String>
	class simple_processing_instruction
	{
		template <typename String1>
		friend bool operator==(simple_processing_instruction<String1> const&, simple_processing_instruction<String1> const&);

		template <typename String1>
		friend bool operator!=(simple_processing_instruction<String1> const&, simple_processing_instruction<String1> const&);

	public:
		using string_type = String;

	public:
		string_type const& data() const { return m_data; }

		string_type& data() { return m_data; }

		void data(string_type const& s) { m_data = s; }

		void data(string_type&& s) { m_data = std::move(s); }

		string_type const& target() const { return m_target; }

		string_type& target() { return m_target; }

		void target(string_type const& s) { m_target = s; }

		void target(string_type&& s) { m_target = std::move(s); }

	protected:
		string_type m_data;
		string_type m_target;
	};

	template <typename String1>
	inline bool operator==(simple_processing_instruction<String1> const&, simple_processing_instruction<String1> const&)
	{
		assert(false);
		return false;
	}

	template <typename String1>
	inline bool operator!=(simple_processing_instruction<String1> const& lhs, simple_processing_instruction<String1> const& rhs)
	{
		return !(lhs == rhs);
	}

	// ---------------------------------------------------------------------------------------------
	// コメント
	// ---------------------------------------------------------------------------------------------

	/*! @brief simple_html 用の コメント ノード

	@tparam String 取り扱う文字列の型
	*/
	template <typename String>
	class simple_comment
	{
		template <typename String1>
		friend bool operator==(simple_comment<String1> const&, simple_comment<String1> const&);

		template <typename String1>
		friend bool operator!=(simple_comment<String1> const&, simple_comment<String1> const&);

	public:
		using string_type = String;

	public:
		simple_comment()
		{
		}

		simple_comment(string_type const& s)
			: m_data(s)
		{
		}

		simple_comment(string_type&& s)
			: m_data(std::move(s))
		{
		}

		string_type const& data() const { return m_data; }

		string_type& data() { return m_data; }

		void data(string_type const& s) { m_data = s; }

		void data(string_type&& s) { m_data = std::move(s); }

	protected:
		string_type m_data;
	};

	template <typename String1>
	inline bool operator==(simple_comment<String1> const&, simple_comment<String1> const&)
	{
		assert(false);
		return false;
	}

	template <typename String1>
	inline bool operator!=(simple_comment<String1> const& lhs, simple_comment<String1> const& rhs)
	{
		return !(lhs == rhs);
	}

	// ---------------------------------------------------------------------------------------------
	// ノード
	// ---------------------------------------------------------------------------------------------

	/*! @brief HTML のノードに相当する共用体

	@tparam String 文字列の型

	インデックスが規格の NodeType と一致するように並べてある。
	*/
	template <typename String>
	using simple_node = std::variant<
		std::monostate,
		simple_element<String>,                // 1.
		std::monostate,
		simple_text<String>,                   // 3.
		std::monostate,
		std::monostate,
		std::monostate,
		simple_processing_instruction<String>, // 7.
		simple_comment<String>,                // 8.
		simple_document<String>,               // 9.
		simple_document_type<String>,          // 10.
		simple_document_fragment<String>,      // 11.
		std::monostate>;

	/*! @brief ノードの文字列データを参照する

	@internal
	std::back_inserterを使うには、可変な文字列コンテナへの参照が必要になる。
	*/
	template <typename String>
	inline String& data(simple_node<String>& node)
	{
		if (std::holds_alternative<simple_text<String>>(node))
		{
			return std::get<simple_text<String>>(node).data();
		}
		else if (std::holds_alternative<simple_comment<String>>(node))
		{
			return std::get<simple_comment<String>>(node).data();
		}
		return std::get<simple_processing_instruction<String>>(node).data();
	}
	
	/*! @brief ノードの文字列データを参照する
	*/
	template <typename String>
	inline String const& data(simple_node<String> const& node)
	{
		if (std::holds_alternative<simple_text<String>>(node))
		{
			return std::get<simple_text<String>>(node).data();
		}
		else if (std::holds_alternative<simple_comment<String>>(node))
		{
			return std::get<simple_comment<String>>(node).data();
		}
		return std::get<simple_processing_instruction<String>>(node).data();
	}

	template <typename String>
	inline String const& name(simple_node<String> const& node)
	{
		return std::get<simple_document_type<String>>(node).name();
	}

	template <typename String>
	inline String const& target(simple_node<String> const& node)
	{
		return std::get<simple_processing_instruction<String>>(node).target();
	}

	/*! @brief 属性の開始を返す
	*/
	template <typename String>
	inline auto begin(simple_node<String>& node)
	{
		return std::get<simple_element<String>>(node).begin();
	}

	/*! @brief 属性の開始を返す
	*/
	template <typename String>
	inline auto begin(simple_node<String> const& node)
	{
		return std::get<simple_element<String>>(node).begin();
	}

	/*! @brief 属性の終端を返す
	*/
	template <typename String>
	inline auto end(simple_node<String>& node)
	{
		return std::get<simple_element<String>>(node).end();
	}

	/*! @brief 属性の終端を返す
	*/
	template <typename String>
	inline auto end(simple_node<String> const& node)
	{
		return std::get<simple_element<String>>(node).end();
	}

	/*! @brief 属性を追加する
	*/
	template <typename String>
	inline void push_back(simple_node<String>& node, simple_attr<String> const& attr)
	{
		return std::get<simple_element<String>>(node).push_back(attr);
	}

	template <typename String>
	inline void push_back(simple_node<String>& node, simple_attr<String>&& attr)
	{
		return std::get<simple_element<String>>(node).push_back(std::move(attr));
	}

	/*! @brief 属性を検索する

	@param [in] node 要素を格納するノード
	@param [in] attr 属性

	@return 属性を指すイテレータ

	node に格納される要素から、attr と名前空間、接頭辞、名前が一致する属性を検索し返す。

	@sa simple_element::find()
	*/
	template <typename String>
	inline auto find(simple_node<String> const& node, simple_attr<String> const& attr)
	{
		return std::get<simple_element<String>>(node).find(attr);
	}

	/*! @brief 属性を検索する

	@param [in] node   要素を格納するノード
	@param [in] ns     名前空間
	@param [in] prefix 接頭辞
	@param [in] name   属性名

	@return 属性を指すイテレータ

	node に格納される要素から、名前空間、接頭辞、名前が一致する属性を検索し返す。

	@sa simple_element::find()
	*/
	template <typename String>
	inline auto find(simple_node<String> const& node, ns_name ns, String const& prefix, String const& name)
	{
		return std::get<simple_element<String>>(node).find(ns, prefix, name);
	}

	/*! @brief 属性を検索する

	@param [in] node 要素を格納するノード
	@param [in] name 属性名

	@return 属性を指すイテレータ

	node に格納される要素から、名前空間がHTML、接頭辞が空、そして名前が一致する属性を検索し返す。
	HTML要素用。

	@sa simple_element::find()
	*/
	template <typename String>
	inline auto find(simple_node<String> const& node, String const& name)
	{
		return std::get<simple_element<String>>(node).find(name);
	}

	/*! @brief 属性を検索する

	@param [in] node   要素を格納するノード
	@param [in] ns     名前空間
	@param [in] prefix 接頭辞
	@param [in] name   属性名

	@return 属性を指すイテレータ

	node に格納される要素から、名前空間、接頭辞、名前が一致する属性を検索し返す。

	@sa simple_element::find()
	*/
	template <typename String>
	inline auto find(simple_node<String> const& node, ns_name ns, String const& prefix, attribute_name name)
	{
		return std::get<simple_element<String>>(node).find(ns, prefix, name);
	}

	/*! @brief 属性を検索する

	@param [in] node 要素を格納するノード
	@param [in] name 属性名

	@return 属性を指すイテレータ

	node に格納される要素から、名前空間がHTML、接頭辞が空、そして名前が一致する属性を検索し返す。
	HTML要素用。

	@sa simple_element::find()
	*/
	template <typename String>
	inline auto find(simple_node<String> const& node, attribute_name name)
	{
		return std::get<simple_element<String>>(node).find(name);
	}

	template <typename String>
	inline ns_name get_namespace_id(simple_attr<String> const& attr)
	{
		return attr.namespace_uri_id();
	}

	template <typename String>
	inline String get_namespace(simple_attr<String> const& attr)
	{
		return attr.namespace_uri();
	}

	template <typename String>
	inline String get_local_name(simple_attr<String> const& attr)
	{
		return attr.local_name();
	}

	template <typename String>
	inline attribute_name get_local_name_id(simple_attr<String> const& attr)
	{
		return attr.local_name_id();
	}

	template <typename String>
	inline String get_qualified_name(simple_attr<String> const& attr)
	{
		return attr.local_name_id();
	}
	
	template <typename String>
	inline auto const& value(simple_attr<String> const& attr)
	{
		return attr.value();
	}

	/*! @brief 要素の名前空間を返す
	*/
	template <typename String>
	inline ns_name get_namespace_id(simple_node<String> const& node)
	{
		return std::get<simple_element<String>>(node).namespace_uri_id();
	}

	/*! @brief 要素の名前空間を返す
	*/
	template <typename String>
	inline String get_namespace(simple_node<String> const& node)
	{
		return std::get<simple_element<String>>(node).namespace_uri();
	}

	/*! @brief 要素のローカル名を返す
	*/
	template <typename String>
	inline tag_name get_local_name_id(simple_node<String> const& node)
	{
		return std::get<simple_element<String>>(node).local_name_id();
	}

	template <typename String>
	inline String get_local_name(simple_node<String> const& node)
	{
		return std::get<simple_element<String>>(node).local_name();
	}

	template <typename String>
	inline String get_qualified_name(simple_node<String> const& node)
	{
		return std::get<simple_element<String>>(node).qualified_name();
	}

	template <typename String>
	inline document_type_name get_document_type(simple_node<String> const& node)
	{
		return std::get<simple_document<String>>(node).document_type();
	}

	/*! @brief 文書ノードに文書形式を設定する

	@param [in] node 文書ノード
	@param [in] type 文書形式（ html あるいは xml ）
	*/
	template <typename String>
	inline void set_document_type(simple_node<String>& node, document_type_name type)
	{
		std::get<simple_document<String>>(node).document_type(type);
	}

	template <typename String>
	inline document_mode_name get_document_mode(simple_node<String> const& node)
	{
		return std::get<simple_document<String>>(node).document_mode();
	}

	/*! @brief 文書ノードに文書モードを設定する

	@param [in] node 文書ノード
	@param [in] mode 文書モード（ no_quirks、quirks、あるいは limited_quirks ）
	*/
	template <typename String>
	inline void set_document_mode(simple_node<String>& node, document_mode_name mode)
	{
		std::get<simple_document<String>>(node).document_mode(mode);
	}
}
