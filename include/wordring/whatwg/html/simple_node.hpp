#pragma once

#include <wordring/whatwg/html/simple_defs.hpp>

#include <wordring/whatwg/infra/unicode.hpp>

#include <optional>
#include <string>
#include <type_traits>
#include <variant>
#include <vector>

namespace wordring::whatwg::html::simple
{
	using wordring::whatwg::html::parsing::atom_tbl;
	using wordring::whatwg::encoding_cast;

	template <typename String, typename Atom>
	class basic_html_atom
	{
		template <typename String1, typename Atom1>
		friend bool operator==(basic_html_atom<String1, Atom1> const&, basic_html_atom<String1, Atom1> const&);

		template <typename String1, typename Atom1>
		friend bool operator!=(basic_html_atom<String1, Atom1> const&, basic_html_atom<String1, Atom1> const&);

		template <typename String1, typename Atom1>
		friend bool operator==(basic_html_atom<String1, Atom1> const&, Atom1);

		template <typename String1, typename Atom1>
		friend bool operator!=(basic_html_atom<String1, Atom1> const&, Atom1);

		template <typename String1, typename Atom1>
		friend bool operator==(Atom1, basic_html_atom<String1, Atom1> const&);

		template <typename String1, typename Atom1>
		friend bool operator!=(Atom1, basic_html_atom<String1, Atom1> const&);

	public:
		using string_type = String;
		using atom_type   = Atom;

	public:
		basic_html_atom() = default;

		basic_html_atom(string_type const& s)
			: m_string(encoding_cast<std::u32string>(s))
			, m_i(atom_tbl.at(s))
		{
		}

		basic_html_atom(atom_type i)
			: m_string(atom_tbl.at(static_cast<std::uint32_t>(i)))
			, m_i(i)
		{
		}

		operator string_type const&() const
		{
			if (!m_string.empty()) return m_string;

			std::u32string s;

			if constexpr (std::is_same_v<atom_type, namespaces>) s = atom_tbl.at(static_cast<std::uint32_t>(m_i));

			return encoding_cast<string_type>(s);
		}

	protected:
		string_type m_string;
		atom_type   m_i;
	};

	template <typename String1, typename Atom1>
	inline bool operator==(basic_html_atom<String1, Atom1> const& lhs, basic_html_atom<String1, Atom1> const& rhs)
	{
		if (static_cast<std::uint32_t>(lhs.m_i) != 0 && static_cast<std::uint32_t>(rhs.m_i) != 0) return lhs.m_i == rhs.m_i;
		return lhs.m_string == rhs.m_string;
	}


	template <typename String1, typename Atom1>
	inline bool operator!=(basic_html_atom<String1, Atom1> const& lhs, basic_html_atom<String1, Atom1> const& rhs)
	{
		return !(lhs == rhs);
	}

	template <typename String1, typename Atom1>
	inline bool operator==(basic_html_atom<String1, Atom1> const& lhs, Atom1 i)
	{
		assert(static_cast<std::uint32_t>(lhs.m_i) != 0);
		return lhs.m_i == i;
	}

	template <typename String1, typename Atom1>
	inline bool operator!=(basic_html_atom<String1, Atom1> const& lhs, Atom1 i)
	{
		return !(lhs == i);
	}

	template <typename String1, typename Atom1>
	inline bool operator==(Atom1 i, basic_html_atom<String1, Atom1> const& rhs)
	{
		assert(static_cast<std::uint32_t>(rhs.m_i) != 0);
		return i != rhs.m_i;

	}

	template <typename String1, typename Atom1>
	inline bool operator!=(Atom1 i, basic_html_atom<String1, Atom1> const& rhs)
	{
		return !(i == rhs);
	}


	template <typename String>
	class basic_attr
	{
	public:
		using string_type = String;

		using name_atom = basic_html_atom<string_type, int>;

	protected:

	};

	template <typename String>
	class basic_document
	{
	public:
		using string_type     = String;
		using usv_string_type = std::u32string;

	public:

	protected:
	};

	template <typename String>
	class basic_document_type
	{
	public:
		using string_type = String;

	};

	template <typename String>
	class basic_document_fragment
	{
	public:
		using string_type = String;

	};

	template <typename String>
	class basic_element
	{
	public:
		using string_type     = String;
		using usv_string_type = std::u32string;

		using namespace_uri_type    = basic_html_atom<string_type, namespaces>;
		using namespace_prefix_type = basic_html_atom<string_type, namespaces>;

		using attr_type = basic_attr<string_type>;

	public:
		namespace_uri_type const& namespace_uri() const { return m_namespace_uri; }

		void namespace_uri(string_type const& uri)
		{
			namespace parsing = wordring::whatwg::html::parsing;

			m_namespace_uri = std::optional<string_type>(uri);
			m_namespace_uri_atom = atom_tbl.at(uri);
		}


	private:
		namespace_uri_type m_namespace_uri;

		std::optional<string_type> m_prefix;
		string_type m_local_name;
		string_type m_tag_name;

		string_type m_id;
		string_type m_class_name;

		std::vector<attr_type> m_attributes;
	};

	template <typename String>
	class basic_text
	{
	public:
		using string_type = String;

	};

	template <typename String>
	class basic_processing_instruction
	{
	public:
		using string_type = String;

	};

	template <typename String>
	class basic_comment
	{
	public:
		using string_type = String;

	public:
		basic_comment()
			: m_node_document(nullptr)
		{
		}

	protected:

		basic_document<string_type>* m_node_document;
		string_type m_data;
	};

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
