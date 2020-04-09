#pragma once

#include <wordring/whatwg/html/simple_defs.hpp>

#include <wordring/whatwg/html/dom_defs.hpp>

#include <wordring/whatwg/infra/infra.hpp>

#include <iterator>
#include <optional>
#include <variant>
#include <vector>

namespace wordring::whatwg::html::simple
{
	template <typename String>
	class basic_attr
	{
		template <typename String1>
		friend bool operator==(basic_attr<String1> const&, basic_attr<String1> const&);

		template <typename String1>
		friend bool operator!=(basic_attr<String1> const&, basic_attr<String1> const&);

	public:

		using string_type = String;

		using namespace_uri_type = basic_html_atom<string_type, ns_name>;
		using lacal_name_type    = basic_html_atom<string_type, tag_name>;

	public:
		string_type namespace_uri() const { return static_cast<string_type>(m_namespace_uri); }

		void namespace_uri(string_type const& uri) { m_namespace_uri = uri; }

		namespace_uri_type const& namespace_uri_atom() const { return m_namespace_uri; }

		string_type local_name() const { return static_cast<string_type>(m_local_name); }

		void local_name(string_type const& name) { m_local_name = name; }

		lacal_name_type const& local_name_atom() const { return m_local_name; }

		string_type const& value() const { return m_value; }

		void value(string_type const& s) { m_value = s; }

	protected:
		namespace_uri_type m_namespace_uri;
		string_type        m_prefix;
		lacal_name_type    m_local_name;

		string_type        m_value;
	};

	template <typename String1>
	inline bool operator==(basic_attr<String1> const& lhs, basic_attr<String1> const& rhs)
	{
		return lhs.m_namespace_uri == rhs.m_namespace_uri
			&& lhs.m_local_name    == rhs.m_local_name
			&& lhs.m_value         == rhs.m_value;
	}

	template <typename String1>
	inline bool operator!=(basic_attr<String1> const& lhs, basic_attr<String1> const& rhs)
	{
		return !(lhs == rhs);
	}

	template <typename String>
	class basic_document
	{
		template <typename String1>
		friend bool operator==(basic_document<String1> const&, basic_document<String1> const&);

		template <typename String1>
		friend bool operator!=(basic_document<String1> const&, basic_document<String1> const&);

	public:
		using string_type     = String;
		using usv_string_type = std::u32string;

	public:
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
	inline bool operator==(basic_document<String1> const&, basic_document<String1> const&)
	{
		assert(false);
		return false;
	}

	template <typename String1>
	inline bool operator!=(basic_document<String1> const&, basic_document<String1> const&)
	{
		return !(lhs == rhs);
	}

	template <typename String>
	class basic_document_type
	{
		template <typename String1>
		friend bool operator==(basic_document_type<String1> const&, basic_document_type<String1> const&);

		template <typename String1>
		friend bool operator!=(basic_document_type<String1> const&, basic_document_type<String1> const&);

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
	inline bool operator==(basic_document_type<String1> const&, basic_document_type<String1> const&)
	{
		assert(false);
		return false;
	}

	template <typename String1>
	inline bool operator!=(basic_document_type<String1> const&, basic_document_type<String1> const&)
	{
		return !(lhs == rhs);
	}


	template <typename String>
	class basic_document_fragment
	{
		template <typename String1>
		friend bool operator==(basic_document_fragment<String1> const&, basic_document_fragment<String1> const&);

		template <typename String1>
		friend bool operator!=(basic_document_fragment<String1> const&, basic_document_fragment<String1> const&);

	public:
		using string_type = String;


	};

	template <typename String1>
	inline bool operator==(basic_document_fragment<String1> const&, basic_document_fragment<String1> const&)
	{
		assert(false);
		return false;
	}

	template <typename String1>
	inline bool operator!=(basic_document_fragment<String1> const&, basic_document_fragment<String1> const&)
	{
		return !(lhs == rhs);
	}


	template <typename String>
	class basic_element
	{
		template <typename String1>
		friend bool operator==(basic_element<String1> const&, basic_element<String1> const&);

		template <typename String1>
		friend bool operator!=(basic_element<String1> const&, basic_element<String1> const&);

	public:
		using string_type     = String;
		using usv_string_type = std::u32string;

		using namespace_uri_type = basic_html_atom<string_type, ns_name>;
		using lacal_name_type    = basic_html_atom<string_type, tag_name>;


		using attr_type = basic_attr<string_type>;

	public:
		basic_element()
		{
		}

		basic_element(namespace_uri_type const& ns, string_type const& prefix, lacal_name_type const& name)
			: m_namespace_uri(ns)
			, m_namespace_prefix(prefix)
			, m_local_name(name)
		{
		}

		basic_element(string_type const& ns, string_type const& prefix, string_type const& name)
			: m_namespace_uri(ns)
			, m_namespace_prefix(prefix)
			, m_local_name(name)
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

	private:
		namespace_uri_type m_namespace_uri;
		string_type        m_namespace_prefix;
		lacal_name_type    m_local_name;





		string_type m_tag_name;

		string_type m_id;
		string_type m_class_name;

		std::vector<attr_type> m_attributes;
	};

	template <typename String1>
	inline bool operator==(basic_element<String1> const& lhs, basic_element<String1> const& rhs)
	{
		using attr_type = typename basic_element<String1>::attr_type;

		if (lhs.m_attributes.size() != rhs.m_attributes.size()) return false;

		for (attr_type const& a : lhs.m_attributes)
		{
			if (std::find(rhs.m_attributes.begin(), rhs.m_attributes.end(), a) == rhs.m_attributes.end()) return false;
		}

		return true;
	}

	template <typename String1>
	inline bool operator!=(basic_element<String1> const& lhs, basic_element<String1> const& rhs)
	{
		return !(lhs == rhs);
	}


	template <typename String>
	class basic_text
	{
		template <typename String1>
		friend bool operator==(basic_text<String1> const&, basic_text<String1> const&);

		template <typename String1>
		friend bool operator!=(basic_text<String1> const&, basic_text<String1> const&);

	public:
		using string_type = String;

	};

	template <typename String1>
	inline bool operator==(basic_text<String1> const&, basic_text<String1> const&)
	{
		assert(false);
		return false;
	}

	template <typename String1>
	inline bool operator!=(basic_text<String1> const&, basic_text<String1> const&)
	{
		return !(lhs == rhs);
	}


	template <typename String>
	class basic_processing_instruction
	{
		template <typename String1>
		friend bool operator==(basic_processing_instruction<String1> const&, basic_processing_instruction<String1> const&);

		template <typename String1>
		friend bool operator!=(basic_processing_instruction<String1> const&, basic_processing_instruction<String1> const&);

	public:
		using string_type = String;

	};

	template <typename String1>
	inline bool operator==(basic_processing_instruction<String1> const&, basic_processing_instruction<String1> const&)
	{
		assert(false);
		return false;
	}

	template <typename String1>
	inline bool operator!=(basic_processing_instruction<String1> const&, basic_processing_instruction<String1> const&)
	{
		return !(lhs == rhs);
	}

	template <typename String>
	class basic_comment
	{
		template <typename String1>
		friend bool operator==(basic_comment<String1> const&, basic_comment<String1> const&);

		template <typename String1>
		friend bool operator!=(basic_comment<String1> const&, basic_comment<String1> const&);

	public:
		using string_type = String;

	public:
		basic_comment()
		{
		}

		basic_comment(string_type const& s)
			: m_data(s)
		{
		}

		basic_comment(string_type&& s)
			: m_data(std::move(s))
		{
		}

		void data(string_type const& s) { m_data = s; }

		void data(string_type&& s) { m_data = std::move(s); }

		string_type const& data() const { return m_data; }

	protected:
		string_type m_data;
	};

	template <typename String1>
	inline bool operator==(basic_comment<String1> const&, basic_comment<String1> const&)
	{
		assert(false);
		return false;
	}

	template <typename String1>
	inline bool operator!=(basic_comment<String1> const&, basic_comment<String1> const&)
	{
		return !(lhs == rhs);
	}


	template <typename String>
	using basic_node = std::variant<
		basic_document<String>,
		basic_document_type<String>,
		basic_document_fragment<String>,
		basic_element<String>,
		basic_text<String>,
		basic_processing_instruction<String>,
		basic_comment<String>>;
}
