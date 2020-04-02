#pragma once

// https://html.spec.whatwg.org/multipage/parsing.html
// https://triple-underscore.github.io/HTML-parsing-ja.html

#include <cstdint>
#include <string>
#include <vector>

namespace wordring::whatwg::html::parsing
{
	// DOCTYPE ----------------------------------------------------------------

	struct DOCTYPE_token
	{
		std::u32string m_name;
		std::u32string m_public_identifier;
		std::u32string m_system_identifier;
		bool m_force_quirks_flag;

		void clear()
		{
			m_name.clear();
			m_public_identifier.clear();
			m_system_identifier.clear();

			m_force_quirks_flag = false;
		}
	};

	// start/end tag ----------------------------------------------------------

	struct tag_token
	{
		struct attribute
		{
			std::u32string m_name;
			std::u32string m_value;

			bool m_omitted;

			attribute()
				: m_omitted(false)
			{
			}

			void clear()
			{
				m_name.clear();
				m_value.clear();
				m_omitted = false;
			}
		};

		struct attribute_list
		{
			using container = std::vector<attribute>;
			using iterator  = typename container::iterator;

			container m_c;
			/*! @brief 最後の要素の次 */
			std::uint32_t m_last;

			attribute_list()
				: m_last(0)
			{
			}

			void clear() { m_last = 0; }

			/*! @brief 新しい属性を開始する
			*/
			attribute& create()
			{
				if (m_c.size() <= m_last) m_c.emplace_back();
				else m_c[m_last].clear();

				return m_c[m_last++];
			}

			attribute& current()
			{
				return m_c[m_last - 1];
			}

			iterator begin() { return m_c.begin(); }

			iterator end() { return std::next(begin(), m_last); }
		};

		std::u32string m_tag_name;
		std::uint32_t m_tag_name_id;

		bool m_self_closing_flag;

		attribute_list m_attributes;

		void clear()
		{
			m_tag_name.clear();
			m_tag_name_id = 0;
			m_self_closing_flag = false;
			m_attributes.clear();
		}

		// 属性 ---------------------------------------------------------------

		attribute_list& attributes() { return m_attributes; }
	};

	struct start_tag_token : tag_token {};

	struct end_tag_token : tag_token {};

	// comment ----------------------------------------------------------------

	struct comment_token
	{
		std::u32string m_data;
	};

	// character --------------------------------------------------------------

	struct character_token
	{
		char32_t m_data;
	};

	// eof --------------------------------------------------------------------

	struct end_of_file_token
	{
	};
}
