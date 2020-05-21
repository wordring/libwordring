#pragma once

#include <wordring/whatwg/html/html_atom.hpp>
#include <wordring/whatwg/html/html_defs.hpp>

#include <wordring/whatwg/infra/infra.hpp>

#include <iterator>
#include <optional>
#include <variant>
#include <vector>

namespace wordring::whatwg::html
{
	// ---------------------------------------------------------------------------------------------
	// 属性
	// ---------------------------------------------------------------------------------------------

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

		simple_attr(ns_name ns, string_type const& prefix, string_type const& name, string_type const& s = string_type())
			: m_namespace_uri(ns)
			, m_prefix(prefix)
			, m_local_name(name)
			, m_value(s)
		{
		}

		simple_attr(string_type const& name, string_type const& s = string_type())
			: m_namespace_uri(ns_name::HTML)
			, m_prefix()
			, m_local_name(name)
			, m_value(s)
		{
		}

		simple_attr(ns_name ns, string_type const& prefix, attribute_name name, string_type const& s = string_type())
			: m_namespace_uri(ns)
			, m_prefix(prefix)
			, m_local_name(name)
			, m_value(s)
		{
		}

		simple_attr(attribute_name name, string_type const& s = string_type())
			: m_namespace_uri(ns_name::HTML)
			, m_prefix()
			, m_local_name(name)
			, m_value(s)
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
			: m_document_type(document_type_name::html)
			, m_document_mode(document_mode_name::no_quirks)
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

	template <typename String>
	class simple_processing_instruction
	{
		template <typename String1>
		friend bool operator==(simple_processing_instruction<String1> const&, simple_processing_instruction<String1> const&);

		template <typename String1>
		friend bool operator!=(simple_processing_instruction<String1> const&, simple_processing_instruction<String1> const&);

	public:
		using string_type = String;

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
	*/
	template <typename String>
	String& data(simple_node<String>& node)
	{
		if (std::holds_alternative<simple_text<String>>(node))
		{
			return std::get<simple_text<String>>(node).data();
		}

		return std::get<simple_comment<String>>(node).data();
	}

	/*! @brief ノードの文字列データを参照する
	*/
	template <typename String>
	String const& data(simple_node<String> const& node)
	{
		if (std::holds_alternative<simple_text<String>>(node))
		{
			return std::get<simple_text<String>>(node).data();
		}

		return std::get<simple_comment<String>>(node).data();
	}

	/*! @brief ノードを要素と解釈して属性の開始を返す
	*/
	template <typename String>
	typename simple_element<String>::iterator begin(simple_node<String>& node)
	{
		return std::get<simple_element<String>>(node).begin();
	}

	/*! @brief ノードを要素と解釈して属性の開始を返す
	*/
	template <typename String>
	typename simple_element<String>::const_iterator begin(simple_node<String> const& node)
	{
		return std::get<simple_element<String>>(node).begin();
	}

	/*! @brief ノードを要素と解釈して属性の終端を返す
	*/
	template <typename String>
	typename simple_element<String>::iterator end(simple_node<String>& node)
	{
		return std::get<simple_element<String>>(node).end();
	}

	/*! @brief ノードを要素と解釈して属性の終端を返す
	*/
	template <typename String>
	typename simple_element<String>::const_iterator end(simple_node<String> const& node)
	{
		return std::get<simple_element<String>>(node).end();
	}

	/*! @brief ノードを要素と解釈して属性を追加する
	*/
	template <typename String>
	void push_back(simple_node<String>& node, simple_attr<String> const& attr)
	{
		return std::get<simple_element<String>>(node).push_back(attr);
	}

	template <typename String>
	void push_back(simple_node<String>& node, simple_attr<String>&& attr)
	{
		return std::get<simple_element<String>>(node).push_back(std::move(attr));
	}

	/*! @brief ノードを要素と解釈して属性を検索する
	*/
	template <typename String>
	typename simple_element<String>::const_iterator find(simple_node<String> const& node, simple_attr<String> const& attr)
	{
		return std::get<simple_element<String>>(node).find(attr);
	}

	/*! @brief ノードを要素と解釈して属性を検索する
	*/
	template <typename String>
	typename simple_element<String>::const_iterator
		find(simple_node<String> const& node, ns_name ns, String const& prefix, String const& name)
	{
		return std::get<simple_element<String>>(node).find(ns, prefix, name);
	}

	/*! @brief ノードを要素と解釈して属性を検索する

	HTML要素用。
	*/
	template <typename String>
	typename simple_element<String>::const_iterator find(simple_node<String> const& node, String const& name)
	{
		return std::get<simple_element<String>>(node).find(name);
	}

	/*! @brief ノードを要素と解釈して属性を検索する
	*/
	template <typename String>
	typename simple_element<String>::const_iterator
		find(simple_node<String> const& node, ns_name ns, String const& prefix, attribute_name name)
	{
		return std::get<simple_element<String>>(node).find(ns, prefix, name);
	}

	/*! @brief ノードを要素と解釈して属性を検索する

	HTML要素用。
	*/
	template <typename String>
	typename simple_element<String>::const_iterator find(simple_node<String> const& node, attribute_name name)
	{
		return std::get<simple_element<String>>(node).find(name);
	}

}
