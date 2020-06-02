#pragma once

#include <wordring/whatwg/html/syntax/syntax.hpp>

#include <wordring/html/html_defs.hpp>
#include <wordring/html/html_traits.hpp>

#include <wordring/encoding/encoding.hpp>

#include <deque>
#include <iterator>

namespace wordring::whatwg::html
{
	/*! VOIDとして直列化する要素か調べる

	@sa https://html.spec.whatwg.org/multipage/parsing.html#serializes-as-void
	@sa https://triple-underscore.github.io/HTML-writing-ja.html#serializes-as-void
	*/
	template <typename NodePointer>
	inline bool serializes_as_void(NodePointer it)
	{
		using namespace wordring::html;
		using traits = node_traits<NodePointer>;

		if (is_void_element(it)) return true;

		if (traits::is_element(it) && traits::get_namespace_id(it) == ns_name::HTML)
		{
			switch (traits::get_local_name_id(it))
			{
			case tag_name::Basefont: case tag_name::Bgsound: case tag_name::Frame: case tag_name::Keygen:
				return true;
			default:
				break;
			}
		}

		return false;
	}
}

namespace wordring::whatwg::html::detail
{
	template <typename NodePointer, typename OutputIterator>
	inline void get_tag_name(NodePointer it, OutputIterator out)
	{
		using node_pointer = NodePointer;
		using traits = wordring::html::node_traits<node_pointer>;

		ns_name ns = traits::get_namespace_id(it);
		if (ns == ns_name::HTML || ns == ns_name::MathML || ns == ns_name::SVG)
		{
			wordring::encoding_cast(traits::get_local_name(it), out);
		}
		else
		{
			wordring::encoding_cast(traits::get_qualified_name(it), out);
		}
	}

	template <typename Traits, typename Attribute, typename OutputIterator>
	inline void get_attributes_serialized_name(Attribute const& attr, OutputIterator out)
	{
		using traits = Traits;

		ns_name ns = get_namespace_id(attr);

		if (ns == static_cast<ns_name>(0))
		{
			wordring::encoding_cast(traits::get_local_name(attr), out);
			return;
		}

		switch (ns)
		{
		case ns_name::XML:
			wordring::encoding_cast(std::u32string_view(U"xml:"), out);
			wordring::encoding_cast(traits::get_local_name(attr), out);
			return;
		case ns_name::XMLNS:
			if (traits::get_local_name_id(attr) == attribute_name::Xmlns)
			{
				wordring::encoding_cast(std::u32string_view(U"xmlns"), out);
				return;
			}
			wordring::encoding_cast(std::u32string_view(U"xmlns:"), out);
			wordring::encoding_cast(traits::get_local_name(attr), out);
			return;
		case ns_name::XLink:
			wordring::encoding_cast(std::u32string_view(U"xlink:"), out);
			wordring::encoding_cast(traits::get_local_name(attr), out);
			return;
		default:
			wordring::encoding_cast(traits::get_qualified_name(attr), out);
			return;
		}
	}

	template <typename String, typename OutputIterator>
	inline void escape_string(String s, OutputIterator out, bool attr_mode = false)
	{
		auto it1 = std::begin(s);
		auto it2 = std::end(s);
		while (it1 != it2)
		{
			char32_t cp;
			it1 = to_code_point(it1, it2, cp);
			if (attr_mode)
			{
				switch (cp)
				{
				case U'&':
					encoding_cast(std::u32string_view(U"&amp;"), out);
					break;
				case U'\U000000A0':
					encoding_cast(std::u32string_view(U"&nbsp;"), out);
					break;
				case U'\"':
					encoding_cast(std::u32string_view(U"&quot;"), out);
					break;
				default:
					wordring::to_string(cp, out);
					break;
				}
			}
			else
			{
				switch (cp)
				{
				case U'&':
					encoding_cast(std::u32string_view(U"&amp;"), out);
					break;
				case U'\U000000A0':
					encoding_cast(std::u32string_view(U"&nbsp;"), out);
					break;
				case U'<':
					encoding_cast(std::u32string_view(U"&lt;"), out);
					break;
				case U'>':
					encoding_cast(std::u32string_view(U"&gt;"), out);
					break;
				default:
					wordring::to_string(cp, out);
					break;
				}
			}
		}
	}
}

namespace wordring::whatwg::html
{
	/*! @brief ノードを直列化する
	
	@sa https://html.spec.whatwg.org/multipage/parsing.html#html-fragment-serialisation-algorithm
	@sa https://triple-underscore.github.io/HTML-writing-ja.html#html-fragment-serialisation-algorithm
	*/
	template <typename NodePointer, typename OutputIterator>
	inline void to_string(NodePointer it, OutputIterator out)
	{
		using node_pointer = NodePointer;
		using traits = wordring::html::node_traits<node_pointer>;

		// 1.
		if (serializes_as_void(it)) return;
		// 4.
		std::deque<node_pointer> x; // 親スタック
		std::deque<node_pointer> y; // 探索スタック

		x.push_back(it);
		auto it1 = traits::begin(it);
		auto it2 = traits::end(it);
		auto pos = y.end();
		while(it1 != it2) pos = y.insert(pos, it1++);

		while (!y.empty())
		{
			// 探索スタックから開始タグをポップ
			node_pointer current_node = y.back();
			y.pop_back();

			// 親スタックからポップして閉じタグ処理
			while (x.back() != traits::parent(current_node))
			{
				if (!serializes_as_void(x.back()))
				{
					encoding_cast(std::u32string_view(U"</"), out);
					detail::get_tag_name(x.back(), out);
					wordring::to_string(U'>', out);
				}
				x.pop_back();
			}

			// スタックへプッシュ
			if (traits::is_element(current_node))
			{
				x.push_back(current_node);
				it1 = traits::begin(current_node);
				it2 = traits::end(current_node);
				pos = y.end();
				while (it1 != it2) pos = y.insert(pos, it1++);
			}

			if (traits::is_element(current_node))
			{
				wordring::to_string(U'<', out);
				detail::get_tag_name(x.back(), out);

				auto it3 = traits::abegin(current_node);
				auto it4 = traits::aend(current_node);
				while (it3 != it4)
				{
					wordring::to_string(U'\U00000020', out);
					detail::get_attributes_serialized_name<traits>(*it3, out);
					wordring::to_string(U'=', out);
					wordring::to_string(U'\"', out);
					detail::escape_string(traits::value(*it3), out, true);
					wordring::to_string(U'\"', out);
					++it3;
				}

				wordring::to_string(U'>', out);
			}
			else if (traits::is_text(current_node))
			{
				node_pointer parent = traits::parent(current_node);
				ns_name  ns  = traits::get_namespace_id(parent);
				tag_name tag = traits::get_local_name_id(parent);
				if (ns == ns_name::HTML)
				{
					switch (tag)
					{
					case tag_name::Style:   case tag_name::Script:   case tag_name::Xmp:       case tag_name::Iframe:
					case tag_name::Noembed: case tag_name::Noframes: case tag_name::Plaintext:
						encoding_cast(traits::data(current_node), out);
						return;
					default:
						break;
					}
				}
				detail::escape_string(traits::data(current_node), out);
			}
			else if (traits::is_comment(current_node))
			{
				encoding_cast(std::u32string_view(U"<!--"), out);
				encoding_cast(traits::data(current_node), out);
				encoding_cast(std::u32string_view(U"-->"), out);
			}
			else if (traits::is_processing_instruction(current_node))
			{
				encoding_cast(std::u32string_view(U"<?"), out);
				encoding_cast(traits::target(current_node), out);
				wordring::to_string(U'\U00000020', out);
				encoding_cast(traits::data(current_node), out);
				encoding_cast(std::u32string_view(U">"), out);
			}
			else if (traits::is_document_type(current_node))
			{
				encoding_cast(std::u32string_view(U"<!DOCTYPE "), out);
				encoding_cast(traits::name(current_node), out);
				encoding_cast(std::u32string_view(U">"), out);
			}
		}

		// 親スタックからポップして閉じタグ処理（探索スタックが空で親スタックが残っている場合）
		while (1 < x.size())
		{
			if (!serializes_as_void(x.back()))
			{
				encoding_cast(std::u32string_view(U"</"), out);
				detail::get_tag_name(x.back(), out);
				wordring::to_string(U'>', out);
			}
			x.pop_back();
		}
	}
}
