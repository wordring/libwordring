#pragma once

// https://html.spec.whatwg.org/multipage/parsing.html
// https://triple-underscore.github.io/HTML-parsing-ja.html

#include <wordring/whatwg/html/parsing/atom_defs.hpp>

#include <cstdint>
#include <string>
#include <vector>

namespace wordring::whatwg::html::parsing
{

	// --------------------------------------------------------------------------------------------
	// DOCTYPE
	//
	// 12.2.5 Tokenization
	// https://html.spec.whatwg.org/multipage/parsing.html#tokenization
	// --------------------------------------------------------------------------------------------

	struct DOCTYPE_token
	{
		DOCTYPE_token()
			: m_force_quirks_flag(false)
		{
		}

		void clear()
		{
			m_name.clear();
			m_public_identifier.clear();
			m_system_identifier.clear();

			m_force_quirks_flag = false;
		}

		std::u32string m_name;
		std::u32string m_public_identifier;
		std::u32string m_system_identifier;
		bool m_force_quirks_flag;

	};

	// --------------------------------------------------------------------------------------------
	// start/end tag
	//
	// 12.2.5 Tokenization
	// https://html.spec.whatwg.org/multipage/parsing.html#tokenization
	// --------------------------------------------------------------------------------------------

	struct token_attribute
	{
		token_attribute()
			: m_namespace(static_cast<ns_name>(0))
			, m_omitted(false)
		{
		}

		void clear()
		{
			m_name.clear();
			m_value.clear();
			m_omitted = false;
		}

		ns_name        m_namespace;
		std::u32string m_prefix;
		std::u32string m_name;

		std::u32string m_value;

		bool m_omitted;
	};

	struct token_attribute_list
	{
		using container = std::vector<token_attribute>;
		using iterator = typename container::iterator;
		using const_iterator = typename container::const_iterator;

		token_attribute_list()
			: m_last(0)
		{
		}

		void clear() { m_last = 0; }

		/*! @brief 新しい属性を開始する
		*/
		token_attribute& create()
		{
			if (m_c.size() <= m_last) m_c.emplace_back();
			else m_c[m_last].clear();

			return m_c[m_last++];
		}

		token_attribute& current()
		{
			return m_c[m_last - 1];
		}

		bool empty() const { return m_last == 0; }

		iterator begin() { return m_c.begin(); }

		const_iterator begin() const { return m_c.begin(); }

		iterator end() { return std::next(begin(), m_last); }

		const_iterator end() const { return std::next(begin(), m_last); }

		container m_c;
		/*! @brief 最後の要素の次 */
		std::uint32_t m_last;

	};

	struct tag_token
	{
		using iterator = typename token_attribute_list::iterator;
		using const_iterator = typename token_attribute_list::const_iterator;

		tag_token()
			: m_tag_name_id(static_cast<tag_name>(0))
			, m_self_closing_flag(false)
			, m_acknowledged_self_closing_flag(false)
		{
		}

		tag_token(tag_name tag)
			: m_tag_name_id(tag)
			, m_self_closing_flag(false)
			, m_acknowledged_self_closing_flag(false)
		{
		}

		void clear()
		{
			m_tag_name.clear();
			m_self_closing_flag = false;
			m_acknowledged_self_closing_flag = false;
			m_attributes.clear();
		}

		//iterator begin() { return m_attributes.begin(); }

		const_iterator begin() const { return m_attributes.begin(); }

		//iterator end() { return m_attributes.end(); }

		const_iterator end() const { return m_attributes.end(); }

		const_iterator find(std::u32string const& name) const
		{
			const_iterator it1 = begin();
			const_iterator it2 = end();
			while (it1 != it2)
			{
				if (!it1->m_omitted) { if (name == it1->m_name) return it1; }
				++it1;
			}
			return it2;
		}

		/*
		iterator find(attribute_name name)
		{
			iterator it1 = begin();
			iterator it2 = end();
			while (it1 != it2)
			{
				if (!it1->m_omitted) { if (name == it1->m_name_id) return it1; }
				++it1;
			}
			return it2;
		}

		const_iterator find(attribute_name name) const
		{
			const_iterator it1 = begin();
			const_iterator it2 = end();
			while (it1 != it2)
			{
				if (!it1->m_omitted) { if (name == it1->m_name_id) return it1; }
				++it1;
			}
			return it2;
		}
		*/

		std::u32string m_tag_name;
		tag_name       m_tag_name_id;
		bool           m_self_closing_flag;
		bool           m_acknowledged_self_closing_flag;

		token_attribute_list m_attributes;
	};

	struct start_tag_token : tag_token
	{
		start_tag_token()
			: tag_token()
		{
		}
		
		start_tag_token(tag_name tag)
			: tag_token(tag)
		{
		}
	};

	struct end_tag_token : tag_token
	{
		end_tag_token()
			: tag_token()
		{
		}

		end_tag_token(tag_name tag)
			: tag_token(tag)
		{
		}
	};

	// --------------------------------------------------------------------------------------------
	// comment
	//
	// 12.2.5 Tokenization
	// https://html.spec.whatwg.org/multipage/parsing.html#tokenization
	// --------------------------------------------------------------------------------------------

	struct comment_token
	{
		std::u32string m_data;
	};

	// --------------------------------------------------------------------------------------------
	// character
	//
	// 12.2.5 Tokenization
	// https://html.spec.whatwg.org/multipage/parsing.html#tokenization
	// --------------------------------------------------------------------------------------------

	struct character_token
	{
		char32_t m_data;
	};

	// --------------------------------------------------------------------------------------------
	// eof
	//
	// 12.2.5 Tokenization
	// https://html.spec.whatwg.org/multipage/parsing.html#tokenization
	// --------------------------------------------------------------------------------------------

	struct end_of_file_token {};
}
