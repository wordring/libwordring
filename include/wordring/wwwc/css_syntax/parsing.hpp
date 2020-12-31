#pragma once

#include <wordring/wwwc/css_syntax/input_stream.hpp>
#include <wordring/wwwc/css_syntax/tokenization.hpp>

#include <wordring/compatibility.hpp>

#include <algorithm>
#include <any>
#include <memory>
#include <optional>
#include <string>
#include <type_traits>
#include <vector>

namespace wordring::wwwc::css
{

	// --------------------------------------------------------------------------------------------
	// 5. Parsing
	//
	// https://drafts.csswg.org/css-syntax-3/#parsing
	// https://triple-underscore.github.io/css-syntax-ja.html#parsing
	// --------------------------------------------------------------------------------------------

	using css_component = std::any;

	/*! @brief <b>CSSコンポーネント値</b>を表現する AST ノード
	
	- preserved tokens
	- function
	- simple_block
	*/
	using component_value = std::any;

	/*! @brief <b>関数</b>を表現する AST ノード
	*/
	struct function
	{
		std::u32string m_name;
		std::vector<component_value> m_value;
	};

	/*! @brief <b>単純ブロック</b>を表現する AST ノード
	*/
	struct simple_block
	{
		css_token m_associated_token;
		std::vector<component_value> m_value;
	};

	/*! @brief <b>\@規則</b>を表現する AST ノード
	
	@par 例
	@code
		@import "my-styles.css";
	@endcode

	@par 例
	@code
		@page :left {
			margin-left: 4cm;
			margin-right: 3cm;
		}
	@endcode

	@par 例
	@code
		@media print {
			body { font-size: 10pt }
		}
	@endcode

	@sa https://drafts.csswg.org/css-syntax-3/#syntax-description
	*/
	struct at_rule
	{
		std::u32string m_name;
		std::vector<component_value> m_prelude;
		std::optional<simple_block> m_block;
	};

	/*! @brief <b>修飾規則</b>を表現する AST ノード
	
	@par 例
	@code
		p > a {
			color: blue;
			text-decoration: underline;
		}
	@endcode

	例で「p > a」の部分が m_prelude 、「{」と「}」で囲まれた部分全体が	 m_block となる。

	@sa https://drafts.csswg.org/css-syntax-3/#syntax-description
	*/
	struct qualified_rule
	{
		std::vector<component_value> m_prelude;
		simple_block m_block;
	};

	/*! @brief <b>宣言</b>を表現する AST ノード

	@par 例
	@code
		background-color: red
	@endcode

	@sa https://developer.mozilla.org/ja/docs/Web/CSS/Syntax#CSS_declarations
	*/
	struct declaration
	{
		declaration() : m_important_flag(false) {}

		std::u32string m_name;
		std::vector<component_value> m_value;
		bool m_important_flag;
	};

	/*! @brief 構文エラー
	*/
	struct syntax_error {};

	inline bool is_at_rule(std::any const& val)
	{
		return val.type() == typeid(at_rule const);
	}

	inline bool is_qualified_rule(std::any const& val)
	{
		return val.type() == typeid(qualified_rule const);
	}

	inline bool is_declaration(std::any const& val)
	{
		return val.type() == typeid(declaration const);
	}

	inline bool is_function(std::any const& val)
	{
		return val.type() == typeid(function const);
	}

	inline bool is_simple_block(std::any const& val)
	{
		return val.type() == typeid(simple_block const);
	}

	/*! @brief [-block */
	inline bool is_open_square_block(std::any const& val)
	{
		return is_simple_block(val) && is_open_square_token(std::any_cast<simple_block>(&val)->m_associated_token);
	}

	/*! @brief (-block */
	inline bool is_open_paren_block(std::any const& val)
	{
		return is_simple_block(val) && is_open_paren_token(std::any_cast<simple_block>(&val)->m_associated_token);
	}

	/*! @brief {-block */
	inline bool is_open_curly_block(std::any const& val)
	{
		return is_simple_block(val) && is_open_curly_token(std::any_cast<simple_block>(&val)->m_associated_token);
	}

	inline bool is_preserved_token(std::any const& val)
	{
		return is_css_token(val)
			&& !(  is_function_token(val)
				|| is_open_curly_token(val)
				|| is_open_paren_token(val)
				|| is_open_square_token(val));
	}

	inline bool is_component_value(std::any const& val)
	{
		return is_preserved_token(val) || is_function(val) || is_simple_block(val);
	}

	bool is_syntax_error(std::any const& val)
	{
		return val.type() == typeid(syntax_error);
	}

	// --------------------------------------------------------------------------------------------
	// 5.2. Definitions
	//
	// https://drafts.csswg.org/css-syntax-3/#parser-definitions
	// https://triple-underscore.github.io/css-syntax-ja.html#parser-definitions
	// --------------------------------------------------------------------------------------------

	/*! @brief トークン列をストリームとしてアクセスするためのアダプタ
	
	@tparam BidirectionalIterator

	規格で明示されていないが、ストリーム構築後 consume() を呼び出した後に、最初のトークンが Current token として現れる仕様のよう。
	
	*/
	class token_stream
	{
	public:
		using container = std::vector<css_token> const;
		using const_iterator = typename container::const_iterator;

	public:
		token_stream(container&& c)
			: m_v(std::move(c))
			, m_it()
			, m_eof(eof_token())
			, m_reconsume_flag(true)
		{
			m_it = m_v.cbegin();
		}

		css_token const& current_input_token()
		{
			return m_it != m_v.cend() ? *m_it : m_eof;
		}

		css_token const& next_input_token()
		{
			if (m_it == m_v.cend()) return m_eof;

			const_iterator it = std::next(m_it);

			return it != m_v.cend() ? *it : m_eof;
		}

		css_token const& consume()
		{
			if (m_reconsume_flag) m_reconsume_flag = false;
			else if (m_it != m_v.cend()) ++m_it;

			return current_input_token();
		}

		void reconsume()
		{
			assert(m_reconsume_flag == false);

			m_reconsume_flag = true;
		}

	private:
		container m_v;
		const_iterator m_it;

		std::any m_eof;

		bool m_reconsume_flag;
	};

	// --------------------------------------------------------------------------------------------
	// 5.3. Parser Entry Points
	//
	// https://drafts.csswg.org/css-syntax-3/#parser-entry-points
	// https://triple-underscore.github.io/css-syntax-ja.html#parser-entry-points
	// --------------------------------------------------------------------------------------------

	template <typename Input, typename Syntax, typename ErrorHandler = std::nullptr_t>
	inline std::vector<std::any> parse_list(Input&& in, Syntax s, ErrorHandler handler = nullptr);

	template <typename Input, typename ErrorHandler = std::nullptr_t>
	inline std::vector<std::any> parse_list_of_component_values(Input&& in, ErrorHandler handler = nullptr);

	/*! @brief 入力をトークン列へ正規化する
	
	@param [in] in      トークンあるいはコンポーネント値のリスト
	@param [in] handler [オプション] エラーハンドラー

	@return トークンあるいはコンポーネント値のリスト

	入力として文字列が与えられた場合、オーバーロードが呼び出される。

	オーバーロード解決に参加させるためダミーのエラーハンドラーを用意しているが、呼び出されることはない。

	@sa https://drafts.csswg.org/css-syntax-3/#normalize-into-a-token-stream
	@sa https://triple-underscore.github.io/css-syntax-ja.html#normalize-into-a-token-stream
	*/
	template <typename ErrorHandler = std::nullptr_t>
	inline std::vector<css_token> normalize_into_token_stream(std::vector<css_token>&& in, ErrorHandler handler = nullptr)
	{
		return std::move(in);
	}

	/*! @brief 入力をトークン列へ正規化する

	@param [in] in      スタイルシートを表すコード・ポイント列
	@param [in] handler [オプション] エラーハンドラー

	@return トークンのリスト

	入力として文字列が与えられた場合、このバージョンの関数が呼び出される。
	その他の入力が与えられた場合、オーバーロードが呼び出される。

	有効なエラーハンドラが引数で供給された場合、トークン化で発生したエラーを報告するために、
	発生個所を指すイテレータを引数にエラーハンドラーが呼び出される。
	呼び出され方は、以下の通り。

	@code
		handler(in.begin() + n);
	@endcode

	@sa normalize_into_token_stream(std::vector<std::any>&&, ErrorHandler)
	*/
	template <typename ErrorHandler = std::nullptr_t>
	inline std::vector<css_token> normalize_into_token_stream(std::u32string&& in, ErrorHandler handler = nullptr)
	{
		std::vector<css_token> out;

		in.erase(filter_code_points(in.begin(), in.end()), in.end());
		tokenize(in.begin(), in.end(), std::back_inserter(out), handler);

		return out;
	}

	// --------------------------------------------------------------------------------------------
	// 5.3.1. Parse something according to a CSS grammar
	//
	// https://drafts.csswg.org/css-syntax-3/#parse-grammar
	// https://triple-underscore.github.io/css-syntax-ja.html#parse-grammar
	// --------------------------------------------------------------------------------------------

	/*! @brief CSS文法に沿って解析する
	
	@param [in] in      入力ストリーム
	@param [in] s       各種CSS規格で定義される文法
	@param [in] handler エラーハンドラ
	
	@return コンポーネント値のリスト、あるいは失敗を表す空のリスト
	
	入力をコンポーネント値のリストへ解析する。

	引数sで供給される文法にマッチしない場合、戻り値は空となる。
	文法は、コンポーネント値のリストを引数に呼び出すと、マッチするとtrueを返し、マッチしないとfalseを返す関数類。
	文法のシグネチャは以下の通り。
	@code
		bool syntax(std::vector<std::any> const& v);
	@endcode

	エラーハンドラが供給された場合、エラーを報告するために、
	発生個所を指す token_stream を引数にエラーハンドラーが呼び出される。
	エラーハンドラは、以下の通り。

	@code
		void handler(token_stream const& s);
	@endcode

	この関数は、CSS Syntax Moduleの外から呼び出されることを想定している。

	@sa parse_grammar(token_stream&, Syntax, ErrorHandler)

	@sa https://drafts.csswg.org/css-syntax-3/#css-parse-something-according-to-a-css-grammar
	@sa https://triple-underscore.github.io/css-syntax-ja.html#css-parse-something-according-to-a-css-grammar
	*/
	template <typename Syntax, typename ErrorHandler = std::nullptr_t>
	inline std::vector<component_value> parse_grammar(token_stream& in, Syntax syntax, ErrorHandler handler = nullptr)
	{
		std::vector<component_value> v = parse_list_of_component_values(in, handler);

		if (syntax(v) == false) v.clear();

		return v;
	}

	template <typename Syntax>
	inline std::vector<component_value> parse_grammar(std::vector<css_token>&& in, Syntax syntax)
	{
		token_stream s(normalize_into_token_stream(std::move(in), nullptr));
		return parse_grammar(s, syntax, nullptr);
	}

	template <typename Syntax>
	inline std::vector<component_value> parse_grammar(std::u32string&& in, Syntax syntax)
	{
		token_stream s(normalize_into_token_stream(std::move(in), nullptr));
		return parse_grammar(s, syntax, nullptr);
	}

	// --------------------------------------------------------------------------------------------
	// 5.3.2. Parse A Comma-Separated List According To A CSS Grammar
	//
	// https://drafts.csswg.org/css-syntax-3/#parse-comma-list
	// https://triple-underscore.github.io/css-syntax-ja.html#parse-comma-list
	// --------------------------------------------------------------------------------------------

	/*! @brief カンマ区切りのリストをCSS文法に沿って構文解析する
	
	@param [in] in      入力ストリーム
	@param [in] s       各種CSS規格で定義される文法
	@param [in] handler エラーハンドラ
	
	@return コンポーネント値のリスト、あるいは失敗を表す空のリスト
	
	入力をコンポーネント値のリストへ解析する。
	文法の検査も行う。

	エラーハンドラが供給された場合、エラーを報告するために、
	発生個所を指す token_stream を引数にエラーハンドラーが呼び出される。
	エラーハンドラは、以下の通り。

	@code
		void handler(token_stream const& s);
	@endcode

	この関数は、CSS Syntax Moduleの外から呼び出されることを想定している。

	@sa parse_grammar(token_stream&, Syntax, ErrorHandler)

	@sa https://drafts.csswg.org/css-syntax-3/#css-parse-a-comma-separated-list-according-to-a-css-grammar
	@sa https://triple-underscore.github.io/css-syntax-ja.html#css-parse-a-comma-separated-list-according-to-a-css-grammar
	*/
	template <typename Syntax, typename ErrorHandler>
	inline std::vector<component_value> parse_comma_list(token_stream& in, Syntax syntax, ErrorHandler handler)
	{
		std::vector<std::vector<component_value>> v = parse_comma_separated_list_of_component_values(in, handler);

		auto it1 = v.begin();
		auto it2 = v.end();
		while (it1 != it2)
		{
			std::vector<component_value> tmp;
			std::swap(*it1, tmp);
			token_stream s(std::move(tmp));
			*it1 = parse_grammar(s, syntax, handler);
		}

		return v;
	}

	template <typename Syntax>
	inline std::vector<component_value> parse_comma_list(std::vector<css_token>&& in, Syntax syntax)
	{
		token_stream s(normalize_into_token_stream(std::move(in), nullptr));
		return parse_list(s, syntax, nullptr);
	}

	template <typename Syntax>
	inline std::vector<component_value> parse_comma_list(std::u32string&& in, Syntax syntax)
	{
		token_stream s(normalize_into_token_stream(std::move(in), nullptr));
		return parse_list(s, syntax, nullptr);
	}

	// --------------------------------------------------------------------------------------------
	// 5.3.3. Parse a stylesheet
	//
	// https://drafts.csswg.org/css-syntax-3/#parse-stylesheet
	// https://triple-underscore.github.io/css-syntax-ja.html#parse-stylesheet
	// --------------------------------------------------------------------------------------------

	/*! @brief スタイルシートを構文解析する

	@param [in] in      入力ストリーム
	@param [in] handler エラーハンドラ

	@return コンポーネント値のリスト、あるいは失敗を表す空のリスト

	入力をコンポーネント値のリストへ解析する。

	エラーハンドラが供給された場合、エラーを報告するために、
	発生個所を指す token_stream を引数にエラーハンドラーが呼び出される。
	エラーハンドラは、以下の通り。

	@code
		void handler(token_stream const& s);
	@endcode

	この関数は、CSS Syntax Moduleの外から呼び出されることを想定している。

	@sa parse_grammar(token_stream&, Syntax, ErrorHandler)

	@sa https://drafts.csswg.org/css-syntax-3/#parse-a-stylesheet
	@sa https://triple-underscore.github.io/css-syntax-ja.html#parse-a-stylesheet
	*/
	template <typename ErrorHandler>
	inline std::vector<component_value> parse_stylesheet(token_stream& in, ErrorHandler handler)
	{
		std::vector<component_value> v = consume_list_of_rules(in, true, handler);
		return v;
	}

	inline std::vector<component_value> parse_stylesheet(std::vector<css_token>&& in)
	{
		token_stream s(normalize_into_token_stream(std::move(in), nullptr));
		return parse_stylesheet(s, nullptr);
	}

	/*! @brief スタイルシートを構文解析する

	この関数は、コード・ポイント列やトークン列の他に、任意のバイト列を入力として受け付ける。
	エンコーディングを自動決定することが出来る。
	
	*/
	inline std::vector<component_value> parse_stylesheet(std::u32string&& in)
	{
		token_stream s(normalize_into_token_stream(std::move(in), nullptr));
		return parse_stylesheet(s, nullptr);
	}

	// --------------------------------------------------------------------------------------------
	// 5.3.4. Parse a list of rules
	//
	// https://drafts.csswg.org/css-syntax-3/#parse-list-of-rules
	// https://triple-underscore.github.io/css-syntax-ja.html#parse-list-of-rules
	// --------------------------------------------------------------------------------------------

	/*! @brief 規則リストを構文解析する

	@param [in] in      入力ストリーム
	@param [in] handler エラーハンドラ

	@return 規則のリスト、あるいは失敗を表す syntax_error

	入力をコンポーネント値のリストへ解析する。

	エラーハンドラが供給された場合、エラーを報告するために、
	発生個所を指す token_stream を引数にエラーハンドラーが呼び出される。
	エラーハンドラは、以下の通り。

	@code
		void handler(token_stream const& s);
	@endcode

	この関数は、CSS Syntax Moduleの外から呼び出されることを想定している。

	@sa parse_grammar(token_stream&, Syntax, ErrorHandler)

	@sa https://drafts.csswg.org/css-syntax-3/#parse-a-list-of-rules
	@sa https://triple-underscore.github.io/css-syntax-ja.html#parse-a-list-of-rules
	*/
	template <typename ErrorHandler>
	inline std::vector<component_value> parse_list_of_rules(token_stream& in, ErrorHandler handler)
	{
		std::vector<component_value> v = consume_list_of_rules(in, true, handler);
		return v;
	}

	inline std::vector<component_value> parse_list_of_rules(std::vector<css_token>&& in)
	{
		token_stream s(normalize_into_token_stream(std::move(in), nullptr));
		return parse_list_of_rules(s, nullptr);
	}

	inline std::vector<component_value> parse_list_of_rules(std::u32string&& in)
	{
		token_stream s(normalize_into_token_stream(std::move(in), nullptr));
		return parse_list_of_rules(s, nullptr);
	}

	// --------------------------------------------------------------------------------------------
	// 5.3.5. Parse a rule
	//
	// https://drafts.csswg.org/css-syntax-3/#parse-rule
	// https://triple-underscore.github.io/css-syntax-ja.html#parse-rule
	// --------------------------------------------------------------------------------------------

	/*! @brief 規則を構文解析する

	@param [in] in      入力ストリーム
	@param [in] handler エラーハンドラ

	@return 規則、あるいは失敗を表す syntax_error

	入力を規則へ解析する。

	エラーハンドラが供給された場合、エラーを報告するために、
	発生個所を指す token_stream を引数にエラーハンドラーが呼び出される。
	エラーハンドラは、以下の通り。

	@code
		void handler(token_stream const& s);
	@endcode

	この関数は、CSS Syntax Moduleの外から呼び出されることを想定している。

	@sa parse_grammar(token_stream&, Syntax, ErrorHandler)

	@sa https://drafts.csswg.org/css-syntax-3/#parse-a-rule
	@sa https://triple-underscore.github.io/css-syntax-ja.html#parse-a-rule
	*/
	template <typename ErrorHandler>
	inline css_component parse_rule(token_stream& in, ErrorHandler handler)
	{
		while (is_whitespace_token(in.next_input_token())) in.consume();
		if (is_eof_token(in.next_input_token())) return syntax_error();

		css_component rule;
		if (is_at_keyword_token(in.next_input_token()))
		{
			rule = consume_at_rule(in, handler);
		}
		else
		{
			std::optional<qualified_rule> q = consume_qualified_rule(in, handler);
			if (!q) return syntax_error();
			rule = std::move(*q);
		}

		while (is_whitespace_token(in.next_input_token())) in.consume();
		if (is_eof_token(in.next_input_token())) return rule;

		return syntax_error();
	}

	inline css_component parse_rule(std::vector<css_token>&& in)
	{
		token_stream s(normalize_into_token_stream(std::move(in), nullptr));
		return parse_rule(s, nullptr);
	}

	inline css_component parse_rule(std::u32string&& in)
	{
		token_stream s(normalize_into_token_stream(std::move(in), nullptr));
		return parse_rule(s, nullptr);
	}

	// --------------------------------------------------------------------------------------------
	// 5.3.6. Parse a declaration
	//
	// https://drafts.csswg.org/css-syntax-3/#parse-declaration
	// https://triple-underscore.github.io/css-syntax-ja.html#parse-declaration
	// --------------------------------------------------------------------------------------------

	/*! @brief 宣言を構文解析する

	@param [in] in      入力ストリーム
	@param [in] handler エラーハンドラ

	@return 宣言、あるいは失敗を表す syntax_error

	入力をコンポーネント値のリストへ解析する。

	エラーハンドラが供給された場合、エラーを報告するために、
	発生個所を指す token_stream を引数にエラーハンドラーが呼び出される。
	エラーハンドラは、以下の通り。

	@code
		void handler(token_stream const& s);
	@endcode

	この関数は、CSS Syntax Moduleの外から呼び出されることを想定している。

	@sa parse_grammar(token_stream&, Syntax, ErrorHandler)

	@sa https://drafts.csswg.org/css-syntax-3/#parse-a-declaration
	@sa https://triple-underscore.github.io/css-syntax-ja.html#parse-a-declaration
	*/
	template <typename ErrorHandler>
	inline component_value parse_declaration(token_stream& in, ErrorHandler handler)
	{
		while (is_whitespace_token(in.next_input_token())) in.consume();
		if (!is_ident_token(in.next_input_token())) return syntax_error();

		std::optional<declaration> decl = consume_declaration(in, handler);
		if (decl) return decl;

		return syntax_error();
	}

	inline component_value parse_declaration(std::vector<css_token>&& in)
	{
		token_stream s(normalize_into_token_stream(std::move(in), nullptr));
		return parse_declaration(s, nullptr);
	}

	inline component_value parse_declaration(std::u32string&& in)
	{
		token_stream s(normalize_into_token_stream(std::move(in), nullptr));
		return parse_declaration(s, nullptr);
	}

	// --------------------------------------------------------------------------------------------
	// 5.3.7. Parse a list of declarations
	//
	// https://drafts.csswg.org/css-syntax-3/#parse-list-of-declarations
	// https://triple-underscore.github.io/css-syntax-ja.html#parse-list-of-declarations
	// --------------------------------------------------------------------------------------------

	/*! @brief 宣言リストを構文解析する

	@param [in] in      入力ストリーム
	@param [in] handler エラーハンドラ

	@return 宣言リスト、あるいは失敗を表す syntax_error

	入力を宣言リストへ解析する。

	エラーハンドラが供給された場合、エラーを報告するために、
	発生個所を指す token_stream を引数にエラーハンドラーが呼び出される。
	エラーハンドラは、以下の通り。

	@code
		void handler(token_stream const& s);
	@endcode

	この関数は、CSS Syntax Moduleの外から呼び出されることを想定している。

	@sa parse_grammar(token_stream&, Syntax, ErrorHandler)

	@sa https://drafts.csswg.org/css-syntax-3/#parse-a-list-of-declarations
	@sa https://triple-underscore.github.io/css-syntax-ja.html#parse-a-list-of-declarations
	*/
	template <typename ErrorHandler>
	inline std::vector<std::any> parse_list_of_declarations(token_stream& in, ErrorHandler handler)
	{
		return consume_list_of_declarations(in, handler);
	}

	inline std::vector<component_value> parse_list_of_declarations(std::vector<css_token>&& in)
	{
		token_stream s(normalize_into_token_stream(std::move(in), nullptr));
		return parse_list_of_declarations(s, nullptr);
	}

	inline std::vector<component_value> parse_list_of_declarations(std::u32string&& in)
	{
		token_stream s(normalize_into_token_stream(std::move(in), nullptr));
		return parse_list_of_declarations(s, nullptr);
	}

	// --------------------------------------------------------------------------------------------
	// 5.3.8. Parse a component value
	//
	// https://drafts.csswg.org/css-syntax-3/#parse-component-value
	// https://triple-underscore.github.io/css-syntax-ja.html#parse-component-value
	// --------------------------------------------------------------------------------------------

	/*! @brief コンポーネント値を構文解析する

	@param [in] in      入力ストリーム
	@param [in] handler エラーハンドラ

	@return コンポーネント値、あるいは失敗を表す syntax_error

	入力をコンポーネント値へ解析する。

	エラーハンドラが供給された場合、エラーを報告するために、
	発生個所を指す token_stream を引数にエラーハンドラーが呼び出される。
	エラーハンドラは、以下の通り。

	@code
		void handler(token_stream const& s);
	@endcode

	この関数は、CSS Syntax Moduleの外から呼び出されることを想定している。

	@sa parse_grammar(token_stream&, Syntax, ErrorHandler)

	@sa https://drafts.csswg.org/css-syntax-3/#parse-a-component-value
	@sa https://triple-underscore.github.io/css-syntax-ja.html#parse-a-component-value
	*/
	template <typename ErrorHandler>
	inline component_value parse_component_value(token_stream& in, ErrorHandler handler)
	{
		while (is_whitespace_token(in.next_input_token())) in.consume();
		if (is_eof_token(in.next_input_token())) return syntax_error();

		component_value c = consume_component_value(in, handler);

		while (is_whitespace_token(in.next_input_token())) in.consume();
		if (is_eof_token(in.next_input_token())) return c;

		return syntax_error();
	}

	inline component_value parse_component_value(std::vector<css_token>&& in)
	{
		token_stream s(normalize_into_token_stream(std::move(in), nullptr));
		return parse_component_value(s, nullptr);
	}

	inline component_value parse_component_value(std::u32string&& in)
	{
		token_stream s(normalize_into_token_stream(std::move(in), nullptr));
		return parse_component_value(s, nullptr);
	}

	// --------------------------------------------------------------------------------------------
	// 5.3.9. Parse a list of component values
	//
	// https://drafts.csswg.org/css-syntax-3/#parse-list-of-component-values
	// https://triple-underscore.github.io/css-syntax-ja.html#parse-list-of-component-values
	// --------------------------------------------------------------------------------------------

	/*! @brief コンポーネント値のリストを構文解析する
	
	@param [in] in      入力ストリーム
	@param [in] handler エラーハンドラ
	
	@return コンポーネント値のリスト
	
	入力をコンポーネント値のリストへ解析する。

	エラーハンドラが供給された場合、エラーを報告するために、
	発生個所を指す token_stream を引数にエラーハンドラーが呼び出される。
	エラーハンドラは、以下の通り。

	@code
		void handler(token_stream const& s);
	@endcode

	この関数は、CSS Syntax Moduleの外から呼び出されることを想定している。

	@sa parse_grammar(token_stream&, Syntax, ErrorHandler)

	@sa https://drafts.csswg.org/css-syntax-3/#parse-a-list-of-component-values
	@sa https://triple-underscore.github.io/css-syntax-ja.html#parse-a-list-of-component-values
	*/
	template <typename ErrorHandler>
	inline std::vector<component_value> parse_list_of_component_values(token_stream& in, ErrorHandler handler)
	{
		std::vector<std::any> list;

		while (true)
		{
			component_value c = consume_component_value(in, handler);
			if (is_eof_token(c)) break;
			list.push_back(c);
		}

		return list;
	}

	inline std::vector<component_value> parse_list_of_component_values(std::vector<css_token>&& in)
	{
		token_stream s(normalize_into_token_stream(std::move(in), nullptr));
		return parse_list_of_component_values(s, nullptr);
	}

	inline std::vector<component_value> parse_list_of_component_values(std::u32string&& in)
	{
		token_stream s(normalize_into_token_stream(std::move(in), nullptr));
		return parse_list_of_component_values(s, nullptr);
	}

	// --------------------------------------------------------------------------------------------
	// 5.3.10. Parse a comma-separated list of component values
	//
	// https://drafts.csswg.org/css-syntax-3/#parse-comma-separated-list-of-component-values
	// 
	// --------------------------------------------------------------------------------------------

	/*! @brief コンマ区切りのコンポーネント値のリストを構文解析する

	@param [in] in      入力ストリーム
	@param [in] handler エラーハンドラ

	@return コンポーネント値のリストのリスト

	入力をコンポーネント値のリストのリストへ解析する。

	エラーハンドラが供給された場合、エラーを報告するために、
	発生個所を指す token_stream を引数にエラーハンドラーが呼び出される。
	エラーハンドラは、以下の通り。

	@code
		void handler(token_stream const& s);
	@endcode

	この関数は、CSS Syntax Moduleの外から呼び出されることを想定している。

	@sa parse_grammar(token_stream&, Syntax, ErrorHandler)

	@sa https://drafts.csswg.org/css-syntax-3/#parse-comma-separated-list-of-component-values
	@sa https://triple-underscore.github.io/css-syntax-ja.html#parse-comma-separated-list-of-component-values
	*/
	template <typename ErrorHandler>
	inline std::vector<std::vector<component_value>>
		parse_comma_separated_list_of_component_values(token_stream& in, ErrorHandler handler)
	{
		std::vector<std::vector<component_value>> lists;

		while (!is_eof_token(in.current_input_token()))
		{
			std::vector<component_value> v;
			while (true)
			{
				component_value c = consume_component_value(in, handler);
				if (is_eof_token(c) || is_comma_token(c)) break;
				v.push_back(c);
			}
			lists.push_back(v);
		}

		return lists;
	}

	/*! @brief コンマ区切りのコンポーネント値のリストを構文解析する

	@param [in] in 入力トークン列

	@return コンポーネント値のリストのリスト

	@sa parse_comma_separated_list_of_component_values(token_stream&, ErrorHandler)
	*/
	inline std::vector<std::vector<component_value>>
		parse_comma_separated_list_of_component_values(std::vector<css_token>&& in)
	{
		token_stream s(normalize_into_token_stream(std::move(in), nullptr));
		return parse_comma_separated_list_of_component_values(s, nullptr);
	}

	/*! @brief コンマ区切りのコンポーネント値のリストを構文解析する

	@param [in] in 入力文字列

	@return コンポーネント値のリストのリスト

	@sa parse_comma_separated_list_of_component_values(token_stream&, ErrorHandler)
	*/
	inline std::vector<std::vector<component_value>>
		parse_comma_separated_list_of_component_values(std::u32string&& in)
	{
		token_stream s(normalize_into_token_stream(std::move(in), nullptr));
		return parse_comma_separated_list_of_component_values(s, nullptr);
	}

	// --------------------------------------------------------------------------------------------
	// 5.4. Parser Algorithms
	//
	// https://drafts.csswg.org/css-syntax-3/#parser-algorithms
	// https://triple-underscore.github.io/css-syntax-ja.html#parser-algorithms
	// --------------------------------------------------------------------------------------------

	template <typename TokenStream, typename ErrorHandler>
	inline std::vector<css_component> consume_list_of_rules(TokenStream& in, bool top_level = false, ErrorHandler handler = nullptr);

	template <typename TokenStream, typename ErrorHandler>
	inline at_rule consume_at_rule(TokenStream& in, ErrorHandler handler);

	template <typename TokenStream, typename ErrorHandler>
	inline std::optional<qualified_rule> consume_qualified_rule(TokenStream& in, ErrorHandler handler);

	template <typename TokenStream, typename ErrorHandler>
	inline std::vector<css_component> consume_list_of_declarations(TokenStream& in, ErrorHandler handler);

	template <typename TokenStream, typename ErrorHandler>
	inline std::optional<declaration> consume_declaration(TokenStream& in, ErrorHandler handler);

	template <typename TokenStream, typename ErrorHandler>
	inline component_value consume_component_value(TokenStream& in, ErrorHandler handler);

	template <typename TokenStream, typename ErrorHandler>
	inline simple_block consume_simple_block(TokenStream& in, ErrorHandler handler);

	template <typename TokenStream, typename ErrorHandler>
	inline function consume_function(TokenStream& in, ErrorHandler handler);

	// --------------------------------------------------------------------------------------------
	// 5.4.1. Consume a list of rules
	//
	// https://drafts.csswg.org/css-syntax-3/#consume-list-of-rules
	// https://triple-underscore.github.io/css-syntax-ja.html#consume-list-of-rules
	// --------------------------------------------------------------------------------------------

	/*! @brief Ruleのリストを消費する
	
	@param [in] in        トークン列のストリーム
	@param [in] top_level トップレベルフラグ（規格内の呼び出し側で指定される）
	@param [in] handler   エラー処理を行うユーザー定義の関数

	@return Ruleのリスト

	この関数は、CSS Syntax Moduleの外から呼び出されることを想定していない。
	
	@sa https://drafts.csswg.org/css-syntax-3/#consume-a-list-of-rules
	@sa https://triple-underscore.github.io/css-syntax-ja.html#consume-a-list-of-rules
	*/
	template <typename TokenStream, typename ErrorHandler>
	inline std::vector<css_component>
		consume_list_of_rules(TokenStream& in, bool top_level, ErrorHandler handler)

	{
		std::vector<css_component> rules;

		while (true)
		{
			css_token const& tkn = in.consume();
			std::type_info const& type = tkn.type();

			if (type == typeid(whitespace_token))
			{
			}
			else if (type == typeid(eof_token))
			{
				return rules;
			}
			else if (type == typeid(CDO_token) || type == typeid(CDC_token))
			{
				if (top_level)
				{
				}
				else
				{
					in.reconsume();
					std::optional<qualified_rule> rule = consume_qualified_rule(in, handler);
					if (rule.has_value()) rules.push_back(std::move(*rule));
				}
			}
			else if (type == typeid(at_keyword_token))
			{
				in.reconsume();
				rules.push_back(consume_at_rule(in, handler));
			}
			else
			{
				in.reconsume();
				std::optional<qualified_rule> rule = consume_qualified_rule(in, handler);
				if (rule.has_value()) rules.push_back(std::move(*rule));
			}
		}

		assert(false);
		return rules;
	}

	// --------------------------------------------------------------------------------------------
	// 5.4.2. Consume an at-rule
	//
	// https://drafts.csswg.org/css-syntax-3/#consume-at-rule
	// https://triple-underscore.github.io/css-syntax-ja.html#consume-at-rule
	// --------------------------------------------------------------------------------------------

	/*! @brief 入力から at_rule を消費する
	
	@param [in] in      トークン列のストリーム
	@param [in] handler エラー処理を行うユーザー定義の関数

	@return rule

	この関数は、CSS Syntax Moduleの外から呼び出されることを想定していない。

	この関数は、入力の先頭が at_keyword_token と仮定している。

	@sa at_rule

	@sa https://drafts.csswg.org/css-syntax-3/#consume-an-at-rule
	@sa https://triple-underscore.github.io/css-syntax-ja.html#consume-an-at-rule
	*/
	template <typename TokenStream, typename ErrorHandler>
	inline at_rule consume_at_rule(TokenStream& in, ErrorHandler handler)
	{
		css_token const& tkn = in.consume();
		assert(is_at_keyword_token(in.current_input_token()));

		at_rule rule;
		rule.m_name = std::any_cast<at_keyword_token>(&tkn)->m_value;

		while (true)
		{
			css_token const& tkn = in.consume();
			std::type_info const& type = tkn.type();

			if (type == typeid(semicolon_token const))
			{
				return rule;
			}
			else if (type == typeid(eof_token const))
			{
				if constexpr (std::is_invocable_v<ErrorHandler, TokenStream&>) handler(in);
				return rule;
			}
			else if (type == typeid(open_curly_token const))
			{
				rule.m_block = consume_simple_block(in, handler);
				return rule;
			}
			else if (is_open_curly_block(tkn))
			{
				rule.m_block = std::any_cast<simple_block>(tkn);
				return rule;
			}
			else
			{
				in.reconsume();
				rule.m_prelude.push_back(consume_component_value(in, handler));
			}
		}

		assert(false);
		return at_rule();
	}

	// --------------------------------------------------------------------------------------------
	// 5.4.3. Consume a qualified rule
	//
	// https://drafts.csswg.org/css-syntax-3/#consume-qualified-rule
	// https://triple-underscore.github.io/css-syntax-ja.html#consume-qualified-rule
	// --------------------------------------------------------------------------------------------

	/*! @brief 入力から qualified_rule を消費する

	@param [in] in      トークン列のストリーム
	@param [in] handler エラー処理を行うユーザー定義の関数

	@return rule

	この関数は、CSS Syntax Moduleの外から呼び出されることを想定していない。

	@sa qualified_rule

	@sa https://drafts.csswg.org/css-syntax-3/#consume-a-qualified-rule
	@sa https://triple-underscore.github.io/css-syntax-ja.html#consume-a-qualified-rule
	*/
	template <typename TokenStream, typename ErrorHandler>
	inline std::optional<qualified_rule> consume_qualified_rule(TokenStream& in, ErrorHandler handler)
	{
		qualified_rule rule;

		while (true)
		{
			css_token const& tkn = in.consume();
			std::type_info const& type = tkn.type();

			if (type == typeid(eof_token))
			{
				if constexpr (std::is_invocable_v<ErrorHandler, TokenStream&>) handler(in);
				return std::optional<qualified_rule>();
			}
			else if (type == typeid(open_curly_token))
			{
				rule.m_block = consume_simple_block(in, handler);
				return std::make_optional(std::move(rule));
			}
			else if (is_open_curly_block(tkn))
			{
				rule.m_block = *std::any_cast<simple_block>(&tkn);
				return std::make_optional(rule);
			}
			else
			{
				in.reconsume();
				rule.m_prelude.push_back(consume_component_value(in, handler));
			}
		}

		assert(false);
		return std::optional<qualified_rule>();
	}

	// --------------------------------------------------------------------------------------------
	// 5.4.4. Consume a list of declarations
	//
	// https://drafts.csswg.org/css-syntax-3/#consume-list-of-declarations
	// https://triple-underscore.github.io/css-syntax-ja.html#consume-list-of-declarations
	// --------------------------------------------------------------------------------------------

	/*! @brief 入力から declaration のリストを消費する

	@param [in] in      トークン列のストリーム
	@param [in] handler エラー処理を行うユーザー定義の関数

	@return 「 Declaration あるいは  @-rule 」の混在リスト

	この関数は、CSS Syntax Moduleの外から呼び出されることを想定していない。

	@sa declaration

	@sa https://drafts.csswg.org/css-syntax-3/#consume-a-list-of-declarations
	@sa https://triple-underscore.github.io/css-syntax-ja.html#consume-a-list-of-declarations
	*/
	template <typename TokenStream, typename ErrorHandler>
	inline std::vector<css_component> consume_list_of_declarations(TokenStream& in, ErrorHandler handler)
	{
		std::vector<css_component> declarations;

		while (true)
		{
			css_token const& tkn = in.consume();
			std::type_info const& type = tkn.type();

			if (type == typeid(whitespace_token) || type == typeid(semicolon_token))
			{
			}
			else if (type == typeid(eof_token))
			{
				return declarations;
			}
			else if (type == typeid(at_keyword_token))
			{
				in.reconsume();
				declarations.push_back(consume_at_rule(in, handler));
			}
			else if (type == typeid(ident_token))
			{
				std::vector<css_component> tmp(1, tkn);

				while (true)
				{
					css_token const& tkn = in.next_input_token();
					std::type_info const& type = tkn.type();
					if (type == typeid(semicolon_token) || type == typeid(eof_token)) break;

					tmp.push_back(consume_component_value(in, handler));
				}

				token_stream in2(std::move(tmp));
				std::optional<declaration> decl = consume_declaration(in2, handler);
				if (decl.has_value()) declarations.push_back(std::move(*decl));
			}
			else
			{
				if constexpr (std::is_invocable_v<ErrorHandler, TokenStream&>) handler(in);
				in.reconsume();
				while (true)
				{
					css_token const& tkn = in.next_input_token();
					std::type_info const& type = tkn.type();
					if (type == typeid(semicolon_token) || type == typeid(eof_token)) break;

					consume_component_value(in, handler);
				}
			}
		}

		assert(false);
		return std::vector<css_component>();
	}

	// --------------------------------------------------------------------------------------------
	// 5.4.5. Consume a declaration
	//
	// https://drafts.csswg.org/css-syntax-3/#consume-declaration
	// https://triple-underscore.github.io/css-syntax-ja.html#consume-declaration
	// --------------------------------------------------------------------------------------------

	/*! @brief 入力から declaration を消費する

	@param [in] in      トークン列のストリーム
	@param [in] handler エラー処理を行うユーザー定義の関数

	@return declaration

	この関数は、CSS Syntax Moduleの外から呼び出されることを想定していない。

	@sa declaration

	@sa https://drafts.csswg.org/css-syntax-3/#consume-a-declaration
	@sa https://triple-underscore.github.io/css-syntax-ja.html#consume-a-declaration
	*/
	template <typename TokenStream, typename ErrorHandler>
	inline std::optional<declaration> consume_declaration(TokenStream& in, ErrorHandler handler)
	{
		in.consume();
		assert(is_ident_token(in.current_input_token()));
		declaration decl;
		decl.m_name = std::any_cast<ident_token>(&in.current_input_token())->m_value;

		while (is_whitespace_token(in.next_input_token())) in.consume();

		if (!is_colon_token(in.next_input_token()))
		{
			if constexpr (std::is_invocable_v<ErrorHandler, TokenStream&>) handler(in);
			return std::optional<declaration>();
		}

		in.consume();

		while (is_whitespace_token(in.next_input_token())) in.consume();

		while (!is_eof_token(in.next_input_token()))
		{
			decl.m_value.push_back(consume_component_value(in, handler));
		}

		std::vector<component_value>& v = decl.m_value;


		// TODO

		while (!v.empty() && is_whitespace_token(v.back())) v.pop_back();

		return std::make_optional(std::move(decl));
	}

	// --------------------------------------------------------------------------------------------
	// 5.4.6. Consume a component value
	//
	// https://drafts.csswg.org/css-syntax-3/#consume-component-value
	// https://triple-underscore.github.io/css-syntax-ja.html#consume-component-value
	// --------------------------------------------------------------------------------------------

	template <typename TokenStream, typename ErrorHandler>
	inline component_value consume_component_value(TokenStream& in, ErrorHandler handler)
	{
		in.consume();

		css_token const& tkn = in.current_input_token();
		std::type_info const& type = tkn.type();
		if (type == typeid(open_curly_token) || type == typeid(open_square_token) || type == typeid(open_paren_token))
		{
			return std::make_any<simple_block>(consume_simple_block(in, handler));
		}

		if (is_function_token(in.current_input_token()))
		{
			return std::make_any<function>(consume_function(in, handler));
		}

		return tkn;
	}

	// --------------------------------------------------------------------------------------------
	// 5.4.7. Consume a simple block
	//
	// https://drafts.csswg.org/css-syntax-3/#consume-simple-block
	// 
	// --------------------------------------------------------------------------------------------

	template <typename TokenStream, typename ErrorHandler>
	inline simple_block consume_simple_block(TokenStream& in, ErrorHandler handler)
	{
		enum class ending_name { curly = 1, square, paren };

		css_token const& tkn = in.current_input_token();
		std::type_info const& type = tkn.type();

		ending_name ending = static_cast<ending_name>(0);
		if      (type == typeid(open_curly_token))  ending = ending_name::curly;
		else if (type == typeid(open_square_token)) ending = ending_name::square;
		else if (type == typeid(open_paren_token))  ending = ending_name::paren;

		simple_block block;
		block.m_associated_token = in.current_input_token();

		while (true)
		{
			css_token const& tkn = in.consume();
			std::type_info const& type = tkn.type();

			if (( (ending == ending_name::curly ) && (type == typeid(close_curly_token)) )
			 || ( (ending == ending_name::square) && (type == typeid(close_square_token)))
			 || ( (ending == ending_name::paren ) && (type == typeid(close_paren_token)) ))
			{
				return block;
			}
			else if (type == typeid(eof_token))
			{
				if constexpr (std::is_invocable_v<ErrorHandler, TokenStream&>) handler(in);
				return block;
			}
			else
			{
				in.reconsume();
				block.m_value.push_back(consume_component_value(in, handler));
			}
		}

		assert(false);
		return simple_block();
	}

	// --------------------------------------------------------------------------------------------
	// 5.4.8. Consume a function
	//
	// https://drafts.csswg.org/css-syntax-3/#consume-function
	// 
	// --------------------------------------------------------------------------------------------

	template <typename TokenStream, typename ErrorHandler>
	inline function consume_function(TokenStream& in, ErrorHandler handler)
	{
		assert(in.current_input_token().type() == typeid(function_token));

		function fn;
		fn.m_name = std::any_cast<function_token>(&in.current_input_token())->m_value;

		while (true)
		{
			css_token const& tkn = in.consume();
			std::type_info const& type = tkn.type();

			if (type == typeid(close_paren_token))
			{
				return fn;
			}
			else if (type == typeid(eof_token))
			{
				if constexpr (std::is_invocable_v<ErrorHandler, TokenStream&>) handler(in);
				return fn;
			}
			else
			{
				in.reconsume();
				fn.m_value.push_back(consume_component_value(in, handler));
			}
		}

		assert(false);
		return function();
	}

	// --------------------------------------------------------------------------------------------
	// 
	//
	// 
	// 
	// --------------------------------------------------------------------------------------------


	// --------------------------------------------------------------------------------------------
	// 
	//
	// 
	// 
	// --------------------------------------------------------------------------------------------


	// --------------------------------------------------------------------------------------------
	// 
	//
	// 
	// 
	// --------------------------------------------------------------------------------------------


	// --------------------------------------------------------------------------------------------
	// 
	//
	// 
	// 
	// --------------------------------------------------------------------------------------------


	// --------------------------------------------------------------------------------------------
	// 
	//
	// 
	// 
	// --------------------------------------------------------------------------------------------


	// --------------------------------------------------------------------------------------------
	// 
	//
	// 
	// 
	// --------------------------------------------------------------------------------------------


	// --------------------------------------------------------------------------------------------
	// 
	//
	// 
	// 
	// --------------------------------------------------------------------------------------------

	// --------------------------------------------------------------------------------------------
	// 
	//
	// 
	// 
	// --------------------------------------------------------------------------------------------
}
