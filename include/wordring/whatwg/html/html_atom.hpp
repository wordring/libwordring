#pragma once

#include <wordring/whatwg/html/parsing/atom_defs.hpp>
#include <wordring/whatwg/html/parsing/atom_tbl.hpp>

#include <wordring/whatwg/infra/unicode.hpp>

#include <string>
#include <string_view>
#include <type_traits>

namespace wordring::whatwg::html
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
	template <typename String, typename Name>
	class basic_html_atom
	{
		template <typename String1, typename Name1>
		friend bool operator==(basic_html_atom<String1, Name1> const&, basic_html_atom<String1, Name1> const&);

		template <typename String1, typename Name1>
		friend bool operator!=(basic_html_atom<String1, Name1> const&, basic_html_atom<String1, Name1> const&);

		template <typename String1, typename Name1>
		friend bool operator==(basic_html_atom<String1, Name1> const&, Name1);

		template <typename String1, typename Name1>
		friend bool operator!=(basic_html_atom<String1, Name1> const&, Name1);

		template <typename String1, typename Name1>
		friend bool operator==(Name1, basic_html_atom<String1, Name1> const&);

		template <typename String1, typename Name1>
		friend bool operator!=(Name1, basic_html_atom<String1, Name1> const&);

		template <typename String1, typename Name1>
		friend bool operator==(basic_html_atom<String1, Name1> const&, String1 const&);

		template <typename String1, typename Name1>
		friend bool operator!=(basic_html_atom<String1, Name1> const&, String1 const&);

		template <typename String1, typename Name1>
		friend bool operator==(String1 const&, basic_html_atom<String1, Name1> const&);

		template <typename String1, typename Name1>
		friend bool operator!=(String1 const&, basic_html_atom<String1, Name1> const&);

	public:
		using string_type = String;
		using name_type   = Name;

	public:
		basic_html_atom()
			: m_string()
			, m_i(static_cast<name_type>(0))
		{
		}

		basic_html_atom(string_type const& s)
			: m_string(s)
			, m_i(static_cast<name_type>(0))
		{
			using namespace wordring::whatwg::html::parsing;

			if constexpr (std::is_same_v<name_type, ns_name>)
			{
				auto it = ns_uri_atom_tbl.find(encoding_cast<std::u32string>(s));
				if (it != ns_uri_atom_tbl.end()) m_i = it->second;
			}
			else if constexpr (std::is_same_v<name_type, tag_name>)
			{
				auto it = tag_atom_tbl.find(encoding_cast<std::u32string>(s));
				if (it != tag_atom_tbl.end()) m_i = it->second;
			}
			else if constexpr (std::is_same_v<name_type, attribute_name>)
			{
				auto it = attribute_atom_tbl.find(encoding_cast<std::u32string>(s));
				if (it != attribute_atom_tbl.end()) m_i = it->second;
			}
		}

		basic_html_atom(name_type i)
			: m_string()
			, m_i(i)
		{
		}

		/*! @brief 文字列化する
		
		static_cast 用の定義。

		*/
		operator string_type() const
		{
			using namespace wordring::whatwg::html::parsing;

			if (!m_string.empty()) return m_string;

			std::u32string s;
			std::uint32_t i = static_cast<std::uint32_t>(m_i);

			if constexpr (std::is_same_v<name_type, ns_name>) s = ns_uri_tbl[i];
			else if constexpr (std::is_same_v<name_type, tag_name>) s = tag_name_tbl[i];
			else s = attribute_name_tbl[i];

			return encoding_cast<string_type>(s);
		}

		operator name_type() const { return m_i; }

	protected:
		string_type m_string;
		name_type   m_i;
	};

	template <typename String1, typename Name1>
	inline bool operator==(basic_html_atom<String1, Name1> const& lhs, basic_html_atom<String1, Name1> const& rhs)
	{
		if (lhs.m_i != static_cast<Name1>(0) && rhs.m_i != static_cast<Name1>(0)) return lhs.m_i == rhs.m_i;
		return static_cast<String1>(lhs) == static_cast<String1>(rhs);
	}

	template <typename String1, typename Name1>
	inline bool operator==(basic_html_atom<String1, Name1> const& lhs, Name1 i)
	{
		return lhs == basic_html_atom<String1, Name1>(i);
	}

	template <typename String1, typename Name1>
	inline bool operator==(Name1 i, basic_html_atom<String1, Name1> const& rhs)
	{
		return basic_html_atom<String1, Name1>(i) == rhs;
	}

	template <typename String1, typename Name1>
	inline bool operator==(basic_html_atom<String1, Name1> const& lhs, String1 const& s)
	{
		return lhs == basic_html_atom<String1, Name1>(s);
	}

	template <typename String1, typename Name1>
	inline bool operator==(String1 const& s, basic_html_atom<String1, Name1> const& rhs)
	{
		return basic_html_atom<String1, Name1>(s) == rhs;
	}

	template <typename String1, typename Name1>
	inline bool operator!=(basic_html_atom<String1, Name1> const& lhs, basic_html_atom<String1, Name1> const& rhs)
	{
		return !(lhs == rhs);
	}

	template <typename String1, typename Name1>
	inline bool operator!=(basic_html_atom<String1, Name1> const& lhs, Name1 i)
	{
		return !(lhs == i);
	}


	template <typename String1, typename Name1>
	inline bool operator!=(Name1 i, basic_html_atom<String1, Name1> const& rhs)
	{
		return !(i == rhs);
	}

	template <typename String1, typename Name1>
	inline bool operator!=(basic_html_atom<String1, Name1> const& lhs, String1 const& s)
	{
		return !(lhs == s);
	}

	template <typename String1, typename Name1>
	inline bool operator!=(String1 const& s, basic_html_atom<String1, Name1> const& rhs)
	{
		return !(s == rhs);
	}

}
