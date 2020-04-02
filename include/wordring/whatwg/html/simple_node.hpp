#pragma once

#include <wordring/whatwg/html/simple_defs.hpp>

#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace wordring::whatwg::html::simple
{/*
	using namespaces = wordring::whatwg::html::parsing::namespaces;

	using DOCTYPE_token = wordring::whatwg::html::parsing::DOCTYPE_token;
	using tag_token = wordring::whatwg::html::parsing::tag_token;
	using start_tag_token   = wordring::whatwg::html::parsing::start_tag_token;
	using end_tag_token     = wordring::whatwg::html::parsing::end_tag_token;
	using comment_token     = wordring::whatwg::html::parsing::comment_token;
	using character_token   = wordring::whatwg::html::parsing::character_token;
	using end_of_file_token = wordring::whatwg::html::parsing::end_of_file_token;*/


	template <typename String>
	class basic_attr
	{

	};

	template <typename String>
	class basic_document
	{
	public:
		using string_type = String;

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

		using attr_type = basic_attr<string_type>;

	public:
		std::optional<string_type> namespace_uri() const { return m_namespace_uri; }

		void namespace_uri(string_type const& uri)
		{
			namespace parsing = wordring::whatwg::html::parsing;

			m_namespace_uri = std::optional<string_type>(uri);
			m_namespace_uri_atom = atom_tbl.at(uri);
		}

		bool operator==(namespaces ns) const
		{
			return m_namespace_uri_atom == static_cast<std::uint32_t>(ns);
		}

	private:
		std::optional<string_type> m_namespace_uri;
		std::uint32_t m_namespace_uri_atom;

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
