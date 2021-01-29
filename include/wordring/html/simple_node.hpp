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

		simple_attr(simple_attr const&) = default;
		simple_attr(simple_attr&&) = default;

		/*! @brief 名前空間付き属性を構築する
		* 
		* 外来属性で必要となる。
		*/
		simple_attr(ns_name ns, string_type const& prefix, attribute_name name, string_type const& val = string_type())
			: m_namespace_uri(ns)
			, m_prefix(prefix)
			, m_local_name(name)
			, m_value(val)
		{
		}

		/*! @brief 名前空間付き属性を構築する
		*
		* 外来属性で必要となる。
		*/
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

		simple_attr& operator=(simple_attr const& rhs) = default;
		simple_attr& operator=(simple_attr&& rhs) = default;
		
		string_type namespace_uri() const { return static_cast<string_type>(m_namespace_uri); }

		void namespace_uri(string_type const& uri) { m_namespace_uri = uri; }

		ns_name namespace_uri_name() const { return m_namespace_uri; }

		void namespace_uri_name(ns_name uri) { m_namespace_uri = uri; }

		string_type const& prefix() const { return m_prefix; }

		void prefix(string_type const& s) { m_prefix = s; }

		string_type local_name() const { return static_cast<string_type>(m_local_name); }

		void local_name(string_type const& name) { m_local_name = name; }

		attribute_name local_name_name() const { return m_local_name; }

		void local_name_name(attribute_name name) { m_local_name = name; }

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

		string_type m_value;
	};

	static_assert(std::is_copy_constructible_v<simple_attr<std::u32string>>);
	static_assert(std::is_copy_assignable_v<simple_attr<std::u32string>>);

	/*! @brief 名前空間、接頭辞、ローカル名が一致する場合、true を返す
	
	検索用に定義した演算子のため、属性値を無視して比較する。
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

	public:
		simple_document()
			: m_document_type(document_type_name::Html)
			, m_document_mode(document_mode_name::NoQuirks)
		{
		}

		simple_document(simple_document const&) = default;
		simple_document(simple_document&&) = default;

		simple_document& operator=(simple_document const&) = default;
		simple_document& operator=(simple_document&&) = default;

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

	static_assert(std::is_copy_constructible_v<simple_document<std::u32string>>);
	static_assert(std::is_copy_assignable_v<simple_document<std::u32string>>);

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

	static_assert(std::is_copy_constructible_v<simple_document_type<std::u32string>>);
	static_assert(std::is_copy_assignable_v<simple_document_type<std::u32string>>);

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

	static_assert(std::is_copy_constructible_v<simple_document_fragment<std::u32string>>);
	static_assert(std::is_copy_assignable_v<simple_document_fragment<std::u32string>>);

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
		using container      = std::vector<attribute_type>;
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

		simple_element(ns_name ns, string_type const& prefix, tag_name name)
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

		ns_name  namespace_uri_name() const { return m_namespace_uri; }

		void  namespace_uri_name(ns_name ns) { m_namespace_uri = ns; }

		string_type namespace_prefix() const { return m_namespace_prefix; }

		void namespace_prefix(string_type const& prefix) { m_namespace_prefix = prefix; }

		string_type local_name() const { return static_cast<string_type>(m_local_name); }

		void local_name(string_type const& name) { m_local_name = name; }

		tag_name local_name_name() const { return m_local_name; }

		void local_name_name(tag_name name) { m_local_name = name; }

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
		const_iterator find(ns_name ns, string_type const& prefix, attribute_name name) const
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

	static_assert(std::is_copy_constructible_v<simple_element<std::u32string>>);
	static_assert(std::is_copy_assignable_v<simple_element<std::u32string>>);

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

		/*
		* 文字列の編集はあらゆる方法で行われるため、生バッファを編集可能に公開する必要が有る。
		*/
		string_type& data() { return m_data; }

		void data(string_type const& s) { m_data = s; }

		/*! 消去検討
		*/
		void push_back(value_type ch) { m_data.push_back(ch); }

	protected:
		string_type m_data;
	};

	static_assert(std::is_copy_constructible_v<simple_text<std::u32string>>);
	static_assert(std::is_copy_assignable_v<simple_text<std::u32string>>);

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

	static_assert(std::is_copy_constructible_v<simple_processing_instruction<std::u32string>>);
	static_assert(std::is_copy_assignable_v<simple_processing_instruction<std::u32string>>);

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

	static_assert(std::is_copy_constructible_v<simple_comment<std::u32string>>);
	static_assert(std::is_copy_assignable_v<simple_comment<std::u32string>>);

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
	class simple_node
	{
		template <typename String1>
		friend bool operator==(simple_node<String1> const lhs, simple_node<String1> const& rhs);

		template <typename String1>
		friend bool operator!=(simple_node<String1> const lhs, simple_node<String1> const& rhs);

	public:
		using string_type = std::remove_cv_t<String>;

		using element_type                = simple_element<string_type>;
		using text_type                   = simple_text<string_type>;
		using processing_instruction_type = simple_processing_instruction<string_type>;
		using comment_type                = simple_comment<string_type>;
		using document_type               = simple_document<string_type>;
		using document_type_type          = simple_document_type<string_type>;
		using document_fragment_type      = simple_document_fragment<string_type>;

		using attribute_type           = simple_attr<string_type>;
		using attribute_iterator       = typename element_type::iterator;
		using const_attribute_iterator = typename element_type::const_iterator;

		/*! @brief HTML のノードに相当する共用体
		*/
		using value_type = std::variant<
			element_type,                // 0
			text_type,                   // 1
			processing_instruction_type, // 2
			comment_type,                // 3
			document_type,               // 4
			document_type_type,          // 5
			document_fragment_type       // 6
		>;

		/*! @brief ノードの型名を示す列挙体
		*
		* インデックスが規格の NodeType と一致するように並べてある。
		*
		* | type                          | index |
		* |-------------------------------|-------|
		* | simple_element                | 1     |
		* | simple_text                   | 3     |
		* | simple_processing_instruction | 7     |
		* | simple_comment                | 8     |
		* | simple_document               | 9     |
		* | simple_document_type          | 10    |
		* | simple_document_fragment      | 11    |
		*/
		enum class type_name : std::uint32_t
		{
			Element               = 1,
			Text                  = 3,
			ProcessingInstruction = 7,
			Comment               = 8,
			Document              = 9,
			DocumentType          = 10,
			DocumentFragment      = 11,
		};

	public:
		simple_node() = default;
		simple_node(simple_node const&) = default;
		simple_node(simple_node&&) = default;

		simple_node(element_type const& val) : m_value(val) {}
		simple_node(element_type && val) : m_value(std::move(val)) {}

		simple_node(text_type const& val) : m_value(val) {}
		simple_node(text_type && val) : m_value(std::move(val)) {}

		simple_node(processing_instruction_type const& val) : m_value(val) {}
		simple_node(processing_instruction_type && val) : m_value(std::move(val)) {}

		simple_node(comment_type const& val) : m_value(val) {}
		simple_node(comment_type && val) : m_value(std::move(val)) {}

		simple_node(document_type const& val) : m_value(val) {}
		simple_node(document_type && val) : m_value(std::move(val)) {}

		simple_node(document_type_type const& val) : m_value(val) {}
		simple_node(document_type_type && val) : m_value(std::move(val)) {}

		simple_node(document_fragment_type const& val) : m_value(val) {}
		simple_node(document_fragment_type && val) : m_value(std::move(val)) {}

		simple_node& operator=(simple_node const& rhs) = default;
		simple_node& operator=(simple_node&& rhs) = default;

		type_name type() const
		{
			switch (m_value.index())
			{
			case 0: return type_name::Element;
			case 1: return type_name::Text;
			case 2: return type_name::ProcessingInstruction;
			case 3: return type_name::Comment;
			case 4: return type_name::Document;
			case 5: return type_name::DocumentType;
			case 6: return type_name::DocumentFragment;
			default:
				break;
			}

			return static_cast<type_name>(0);
		}

		bool is_element() const { return std::holds_alternative<element_type>(m_value); }
		bool is_text() const { return std::holds_alternative<text_type>(m_value); }
		bool is_processing_instruction() const { return std::holds_alternative<processing_instruction_type>(m_value); }
		bool is_comment() const { return std::holds_alternative<comment_type>(m_value); }
		bool is_document() const { return std::holds_alternative<document_type>(m_value); }
		bool is_document_type() const { return std::holds_alternative<document_type_type>(m_value); }

		/*! @brief 要素の名前空間 URI を返す
		*/
		ns_name namespace_uri_name() const
		{
			if (is_element()) return std::get_if<element_type>(&m_value)->namespace_uri_name();
			assert(false);
			return static_cast<ns_name>(0);
		}

		/*! @brief 要素の名前空間を返す
		*/
		string_type namespace_uri() const
		{
			if (is_element()) return std::get_if<element_type>(&m_value)->namespace_uri();
			assert(false);
			return m_string;
		}

		/*! @brief 要素のローカル名を返す
		*/
		tag_name local_name_name() const
		{
			if (is_element()) return std::get_if<element_type>(&m_value)->local_name_name();
			assert(false);
			return static_cast<tag_name>(0);
		}

		/*! @brief 要素のローカル名を返す
		*/
		string_type local_name() const
		{
			if (is_element()) return std::get_if<element_type>(&m_value)->local_name();
			assert(false);
			return m_string;
		}

		string_type qualified_name() const
		{
			if (is_element()) return std::get_if<element_type>(&m_value)->qualified_name();
			assert(false);
			return m_string;
		}

		/*! @brief 属性の開始を返す
		*/
		attribute_iterator begin()
		{
			if (is_element()) return std::get_if<element_type>(&m_value)->begin();
			assert(false);
			return attribute_iterator();
		}

		/*! @brief 属性の開始を返す
		*/
		const_attribute_iterator begin() const
		{
			if (is_element()) return std::get_if<element_type>(&m_value)->begin();
			assert(false);
			return attribute_iterator();
		}

		/*! @brief 属性の終端を返す
		*/
		attribute_iterator end()
		{
			if (is_element()) return std::get_if<element_type>(&m_value)->end();
			assert(false);
			return attribute_iterator();
		}

		/*! @brief 属性の終端を返す
		*/
		const_attribute_iterator end() const
		{
			if (is_element()) return std::get_if<element_type>(&m_value)->end();
			assert(false);
			return attribute_iterator();
		}

		/*! @brief 属性を検索する
		*
		* @param [in] prefix 名前空間接頭辞
		* @param [in] name   属性名
		*
		* @return 属性を指すイテレータ
		*
		* node に格納される要素から、名前空間、接頭辞、名前が一致する属性を検索し返す。
		*
		* @sa simple_element::find()
		*/
		const_attribute_iterator find(ns_name ns, string_type const& prefix, string_type const& name) const
		{
			return std::find_if(begin(), end(), [&](attribute_type const& a)->bool {
				return a == attribute_type(ns, prefix, name); });
		}

		bool contains(ns_name ns, string_type const& prefix, string_type const& name)
		{
			const_attribute_iterator it = std::find_if(begin(), end(), [&](attribute_type const& a)->bool {
				return a == attribute_type(ns, prefix, name); });

			return it != end();
		}

		/*! @brief 属性を追加する
		*/
		void push_back(attribute_type&& attr)
		{
			if (is_element()) std::get_if<element_type>(&m_value)->push_back(std::move(attr));
			else assert(false);
		}

		wordring::html::document_type_name document_type_name() const
		{
			if (is_document()) return std::get_if<document_type>(&m_value)->document_type();
			assert(false);
			return static_cast<wordring::html::document_type_name>(0);
		}

		/*! @brief 文書ノードに文書形式を設定する
		*
		* @param [in] type 文書形式（ html あるいは xml ）
		*/
		void document_type_name(wordring::html::document_type_name type)
		{
			if (is_document()) std::get_if<document_type>(&m_value)->document_type(type);
			else assert(false);
		}

		wordring::html::document_mode_name document_mode_name() const
		{
			if (is_document()) return std::get_if<document_type>(&m_value)->document_mode();
			assert(false);
			return static_cast<wordring::html::document_mode_name>(0);
		}

		/*! @brief 文書ノードに文書モードを設定する
		*
		* @param [in] mode 文書モード（ no_quirks、quirks、あるいは limited_quirks ）
		*/
		void document_mode_name(wordring::html::document_mode_name mode)
		{
			if (is_document()) std::get_if<document_type>(&m_value)->document_mode(mode);
			else assert(false);
		}

		/*! @brief ノードの文字列データを参照する
		*/
		string_type const& data() const
		{
			switch (type())
			{
			case type_name::Text:
				return std::get_if<text_type>(&m_value)->data();
			case type_name::Comment:
				return std::get_if<comment_type>(&m_value)->data();
			case type_name::ProcessingInstruction:
				return std::get_if<processing_instruction_type>(&m_value)->data();
			default:
				break;
			}
			assert(false);
			return m_string;
		}

		/*! @brief ノードの文字列データを参照する

		文字列データを持つノードは
		- text_type
		- comment_type
		- processing_instruction_type

		@internal
		<hr>
		std::back_inserterを使うには、可変な文字列コンテナへの参照が必要になる。
		*/
		string_type& data()
		{
			switch (type())
			{
			case type_name::Text:
				return std::get_if<text_type>(&m_value)->data();
			case type_name::Comment:
				return std::get_if<comment_type>(&m_value)->data();
			case type_name::ProcessingInstruction:
				return std::get_if<processing_instruction_type>(&m_value)->data();
			default:
				break;
			}
			assert(false);
			return m_string;
		}

		string_type const& target() const
		{
			if (is_processing_instruction()) return std::get_if<processing_instruction_type>(&m_value)->target();
			assert(false);
			return m_string;
		}

		string_type const& name() const
		{
			if (is_document_type()) return std::get_if<document_type_type>(&m_value)->name();
			assert(false);
			return m_string;
		}

	private:
		value_type  m_value;
		string_type m_string;
	};

	static_assert(std::is_copy_constructible_v<simple_node<std::u32string>>);
	static_assert(std::is_copy_assignable_v<simple_node<std::u32string>>);

	template <typename String1>
	inline bool operator==(simple_node<String1> const lhs, simple_node<String1> const& rhs)
	{
		return lhs.m_value == rhs.m_value;
	}

	template <typename String1>
	inline bool operator!=(simple_node<String1> const lhs, simple_node<String1> const& rhs)
	{
		return !(lhs == rhs);
	}
}
