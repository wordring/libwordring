#pragma once

#include <wordring/whatwg/html/parsing/atom_defs.hpp>
#include <wordring/whatwg/html/parsing/atom_tbl.hpp>

#include <wordring/whatwg/infra/unicode.hpp>

#include <string>
#include <string_view>
#include <type_traits>

namespace wordring::whatwg::html::parsing
{
	/*! @brief 列挙体と文字列の相互変換を行うクラス

	@tparam String 文字列の型
	@tparam Atom   列挙体の型

	対応する列挙体は以下の通り。

	- ns_name
	- tag_name
	- attribute_name

	名前空間、タグ名、属性名の列挙体と文字列を相互に変換する。
	*/
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
		basic_html_atom()
			: m_string()
			, m_i(static_cast<atom_type>(0))
		{
		}

		template <typename String1>
		basic_html_atom(String1 const& s)
			: m_string()
			, m_i(static_cast<atom_type>(0))
		{
			assign(s);
		}

		basic_html_atom(atom_type i)
			: m_string()
			, m_i(i)
		{
		}

		template <typename String1>
		void operator=(String1 const& s)
		{
			m_string.clear();
			assign(s);
		}

		void operator=(atom_type i)
		{
			m_string.clear();
			m_i = i;
		}

		operator string_type() const
		{
			if (m_string.empty()) return encoding_cast<string_type>(find(m_i));

			return m_string;
		}

		operator atom_type() const
		{
			return m_i;
		}

	protected:
		template <typename String1, typename std::enable_if_t<std::negation_v<std::is_same<String1, std::u32string>>, std::nullptr_t> = nullptr>
		void assign(String1 const& s)
		{
			std::u32string buf;
			encoding_cast(s, std::back_inserter(buf));

			m_i = find(buf);

			if (m_i == static_cast<atom_type>(0))
			{
				if constexpr (sizeof(decltype(*std::begin(s))) == sizeof(typename string_type::value_type)) m_string = s;
				else if constexpr (sizeof(decltype(*std::begin(buf))) == sizeof(typename string_type::value_type)) m_string = buf;
				else m_string = encoding_cast<string_type>(buf);
			}
		}

		void assign(std::u32string const& s)
		{
			m_i = find(s);

			if (m_i == static_cast<atom_type>(0))
			{
				if constexpr (std::is_same_v<string_type, std::u32string>) m_string = s;
				else encoding_cast(s, std::back_inserter(m_string));
			}
		}

		atom_type find(std::u32string const& s) const
		{
			using wordring::whatwg::html::parsing::ns_uri_atom_tbl;

			if constexpr (std::is_same_v<atom_type, ns_name>)
			{
				auto it = ns_uri_atom_tbl.find(s);
				if (it != ns_uri_atom_tbl.end()) return static_cast<atom_type>(it->second);
			}
			else if constexpr (std::is_same_v<atom_type, tag_name>)
			{
				auto it = tag_atom_tbl.find(s);
				if (it != tag_atom_tbl.end()) return static_cast<atom_type>(it->second);
			}
			else if constexpr (std::is_same_v<atom_type, attribute_name>)
			{
				auto it = attribute_atom_tbl.find(s);
				if (it != attribute_atom_tbl.end()) return static_cast<atom_type>(it->second);
			}

			return static_cast<atom_type>(0);
		}

		std::u32string const& find(atom_type i) const
		{
			if constexpr (std::is_same_v<atom_type, ns_name>)
				return ns_uri_tbl[static_cast<std::uint32_t>(m_i)];
			else if constexpr (std::is_same_v<atom_type, tag_name>)
				return tag_name_tbl[static_cast<std::uint32_t>(m_i)];
			
			return attribute_name_tbl[static_cast<std::uint32_t>(m_i)];
		}

	protected:
		string_type m_string;
		atom_type   m_i;
	};

	template <typename String1, typename Atom1>
	inline bool operator==(basic_html_atom<String1, Atom1> const& lhs, basic_html_atom<String1, Atom1> const& rhs)
	{
		if (lhs.m_i != static_cast<Atom1>(0) || rhs.m_i != static_cast<Atom1>(0)) return lhs.m_i == rhs.m_i;
		return static_cast<String1>(lhs) == static_cast<String1>(rhs);
	}

	template <typename String1, typename Atom1>
	inline bool operator!=(basic_html_atom<String1, Atom1> const& lhs, basic_html_atom<String1, Atom1> const& rhs)
	{
		return !(lhs == rhs);
	}

	template <typename String1, typename Atom1>
	inline bool operator==(basic_html_atom<String1, Atom1> const& lhs, Atom1 i)
	{
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
		return i == rhs.m_i;

	}

	template <typename String1, typename Atom1>
	inline bool operator!=(Atom1 i, basic_html_atom<String1, Atom1> const& rhs)
	{
		return !(i == rhs);
	}
}
