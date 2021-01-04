#pragma once

#include <wordring/wwwc/css_syntax/input_stream.hpp>
#include <wordring/wwwc/css_syntax/tokenization.hpp>

#include <wordring/compatibility.hpp>
#include <wordring/whatwg/infra/infra.hpp>

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
	//struct syntax_error {};

	/*! @brief 構文解析の結果
	
	結果の値あるいは失敗を表すクラス。
	*/
	template <typename T>
	class parse_result
	{
	public:
		using value_type = T;

	public:
		parse_result() : m_failure(true) {}

		parse_result(value_type const& value)
			: m_value(value)
			, m_failure(false)
		{
		}

		parse_result(value_type&& value)
			: m_value(std::move(value))
			, m_failure(false)
		{
		}

		parse_result(parse_result&&) = default;
		parse_result(parse_result const&) = default;

		void operator =(value_type const& value)
		{
			m_value = value;
			m_failure = false;
		}

		void operator =(value_type&& value)
		{
			m_value = std::move(value);
			m_failure = false;
		}

		operator bool() const { return !m_failure; }

		bool operator !() const { return m_failure; }

		value_type const& operator *() const
		{
			assert(!m_failure);
			return m_value;
		}

		value_type& operator *()
		{
			assert(!m_failure);
			return m_value;
		}

		value_type const* operator ->() const
		{
			assert(!m_failure);
			return &m_value;
		}

		value_type* operator ->()
		{
			assert(!m_failure);
			return &m_value;
		}


	private:
		value_type m_value;
		bool m_failure;
	};

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

	/*inline bool is_syntax_error(std::any const& val)
	{
		return val.type() == typeid(syntax_error);
	}*/

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
		using container = std::vector<css_token>;
		using const_iterator = typename container::const_iterator;

	public:
		token_stream(container&& c)
			: m_v(std::move(c))
			, m_i(-1)
		{
			m_v.push_back(eof_token());
		}

		css_token const& current_input_token()
		{
			if (!(0 <= m_i && m_i < m_v.size())) return m_v.back();
			return m_v[m_i];
		}

		css_token const& next_input_token()
		{
			assert(m_i < int(m_v.size()) - 1);
			return m_v[m_i + 1];
		}

		css_token const& consume()
		{
			++m_i;
			return current_input_token();
		}

		void reconsume()
		{
			--m_i;
		}

	private:
		container m_v;
		std::int32_t m_i;
	};

	// --------------------------------------------------------------------------------------------
	// 5.3. Parser Entry Points
	//
	// https://drafts.csswg.org/css-syntax-3/#parser-entry-points
	// https://triple-underscore.github.io/css-syntax-ja.html#parser-entry-points
	// --------------------------------------------------------------------------------------------

	template <typename Syntax, typename ErrorHandler = std::nullptr_t>
	inline std::vector<parse_result<std::vector<component_value>>>
		parse_comma_list(token_stream& in, Syntax s, ErrorHandler handler = nullptr);

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
	
	@return コンポーネント値のリスト（std::vector<component_value>）、あるいは失敗。
	
	入力をコンポーネント値のリストへ解析する。

	引数sで供給される文法にマッチしない場合、戻り値は failure 状態となる。
	文法は、コンポーネント値のリストを引数に呼び出すと、マッチするとtrueを返し、マッチしないとfalseを返す関数類。
	文法のシグネチャは以下の通り。
	@code
		bool syntax(std::vector<component_value> const& v);
	@endcode

	エラーハンドラが供給された場合、エラーを報告するために、
	発生個所を指す token_stream を引数にエラーハンドラーが呼び出される。
	エラーハンドラは、以下の通り。

	@code
		void handler(token_stream const& s);
	@endcode

	この関数は、CSS Syntax Moduleの外から呼び出されることを想定している。

	@sa parse_result

	@sa https://drafts.csswg.org/css-syntax-3/#css-parse-something-according-to-a-css-grammar
	@sa https://triple-underscore.github.io/css-syntax-ja.html#css-parse-something-according-to-a-css-grammar
	*/
	template <typename Syntax, typename ErrorHandler = std::nullptr_t>
	inline parse_result<std::vector<component_value>>
		parse_grammar(token_stream& in, Syntax syntax, ErrorHandler handler = nullptr)
	{
		using result_type = parse_result<std::vector<component_value>>;

		std::vector<component_value> v = parse_list_of_component_values(in, handler);

		return syntax(v) ? result_type(std::move(v)) : result_type();
	}

	template <typename Syntax>
	inline parse_result<std::vector<component_value>> parse_grammar(std::vector<css_token>&& in, Syntax syntax)
	{
		token_stream s(normalize_into_token_stream(std::move(in), nullptr));
		return parse_grammar(s, syntax, nullptr);
	}

	template <typename Syntax>
	inline parse_result<std::vector<component_value>> parse_grammar(std::u32string&& in, Syntax syntax)
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
	
	@return 「コンポーネント値のリスト（std::vector<component_value>）、あるいは失敗」のリスト
	
	入力をコンポーネント値のリストのリストへ解析する。
	文法の検査も行う。

	文法は、コンポーネント値のリストを引数に呼び出すと、マッチするとtrueを返し、マッチしないとfalseを返す関数類。
	文法のシグネチャは以下の通り。
	@code
		bool syntax(std::vector<component_value> const& v);
	@endcode

	エラーハンドラが供給された場合、エラーを報告するために、
	発生個所を指す token_stream を引数にエラーハンドラーが呼び出される。
	エラーハンドラは、以下の通り。

	@code
		void handler(token_stream const& s);
	@endcode

	この関数は、CSS Syntax Moduleの外から呼び出されることを想定している。

	@sa parse_result
	@sa parse_grammar(token_stream&, Syntax, ErrorHandler)

	@sa https://drafts.csswg.org/css-syntax-3/#css-parse-a-comma-separated-list-according-to-a-css-grammar
	@sa https://triple-underscore.github.io/css-syntax-ja.html#css-parse-a-comma-separated-list-according-to-a-css-grammar
	*/
	template <typename Syntax, typename ErrorHandler>
	inline std::vector<parse_result<std::vector<component_value>>>
		parse_comma_list(token_stream& in, Syntax syntax, ErrorHandler handler)
	{
		using result_type = std::vector<parse_result<std::vector<component_value>>>;

		result_type result;

		std::vector<std::vector<component_value>> v = parse_comma_separated_list_of_component_values(in, handler);
		auto it1 = v.begin();
		auto it2 = v.end();
		while (it1 != it2)
		{
			std::vector<component_value> tmp;
			std::swap(*it1++, tmp);
			token_stream s(std::move(tmp));
			result.emplace_back(parse_grammar(s, syntax, handler));
		}

		return result;
	}

	template <typename Syntax>
	inline std::vector<parse_result<std::vector<component_value>>> parse_comma_list(std::vector<css_token>&& in, Syntax syntax)
	{
		token_stream s(normalize_into_token_stream(std::move(in), nullptr));
		return parse_comma_list(s, syntax, nullptr);
	}

	template <typename Syntax>
	inline std::vector<parse_result<std::vector<component_value>>> parse_comma_list(std::u32string&& in, Syntax syntax)
	{
		token_stream s(normalize_into_token_stream(std::move(in), nullptr));
		return parse_comma_list(s, syntax, nullptr);
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

	@return 規則のリスト

	入力を規則のリストへ解析する。

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
	
	*/
	inline std::vector<component_value> parse_stylesheet(std::u32string&& in)
	{
		token_stream s(normalize_into_token_stream(std::move(in), nullptr));
		return parse_stylesheet(s, nullptr);
	}

	/*! @brief スタイルシートを構文解析する

	この関数は、8bit文字列を入力として受け付ける。
	エンコーディングを自動決定することが出来る。

	*/
	inline std::vector<component_value>
		parse_stylesheet(std::string const& in, encoding_name fallback = static_cast<encoding_name>(0))
	{
		std::u32string buf;
		decode(in.begin(), in.end(), std::back_inserter(buf), fallback);
		token_stream s(normalize_into_token_stream(std::move(buf), nullptr));
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

	@return 規則のリスト

	入力を規則のリストへ解析する。

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

	@return 規則、あるいは失敗

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
	inline parse_result<css_component> parse_rule(token_stream& in, ErrorHandler handler)
	{
		using result_type = parse_result<css_component>;

		while (is_whitespace_token(in.next_input_token())) in.consume();
		if (is_eof_token(in.next_input_token())) return result_type();

		css_component rule;
		if (is_at_keyword_token(in.next_input_token()))
		{
			rule = consume_at_rule(in, handler);
		}
		else
		{
			std::optional<qualified_rule> q = consume_qualified_rule(in, handler);
			if (!q) return result_type();
			rule = std::move(*q);
		}

		while (is_whitespace_token(in.next_input_token())) in.consume();
		if (is_eof_token(in.next_input_token())) return result_type(std::move(rule));

		return result_type();
	}

	inline parse_result<css_component> parse_rule(std::vector<css_token>&& in)
	{
		token_stream s(normalize_into_token_stream(std::move(in), nullptr));
		return parse_rule(s, nullptr);
	}

	inline parse_result<css_component> parse_rule(std::u32string&& in)
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

	@return 宣言、あるいは失敗

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
	inline parse_result<component_value>
		parse_declaration(token_stream& in, ErrorHandler handler)
	{
		using result_type = parse_result<component_value>;

		while (is_whitespace_token(in.next_input_token())) in.consume();
		if (!is_ident_token(in.next_input_token())) return result_type();

		std::optional<declaration> decl = consume_declaration(in, handler);
		if (decl) return result_type(std::move(*decl));

		return result_type();
	}

	inline parse_result<component_value> parse_declaration(std::vector<css_token>&& in)
	{
		token_stream s(normalize_into_token_stream(std::move(in), nullptr));
		return parse_declaration(s, nullptr);
	}

	inline parse_result<component_value> parse_declaration(std::u32string&& in)
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

	@return 宣言リスト

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
	inline std::vector<component_value> parse_list_of_declarations(token_stream& in, ErrorHandler handler)
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

	@return コンポーネント値、あるいは失敗

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
	inline parse_result<component_value> parse_component_value(token_stream& in, ErrorHandler handler)
	{
		using result_type = parse_result<component_value>;

		while (is_whitespace_token(in.next_input_token())) in.consume();
		if (is_eof_token(in.next_input_token())) return result_type();

		component_value c = consume_component_value(in, handler);

		while (is_whitespace_token(in.next_input_token())) in.consume();
		if (is_eof_token(in.next_input_token())) return result_type(std::move(c));

		return result_type();
	}

	inline parse_result<component_value> parse_component_value(std::vector<css_token>&& in)
	{
		token_stream s(normalize_into_token_stream(std::move(in), nullptr));
		return parse_component_value(s, nullptr);
	}

	inline parse_result<component_value> parse_component_value(std::u32string&& in)
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

		//while (!is_eof_token(in.current_input_token()))
		while(true)
		{
			std::vector<component_value> v;
			while (true)
			{
				component_value c = consume_component_value(in, handler);
				if (is_eof_token(c))
				{
					lists.push_back(v);
					return lists;
				}
				else if (is_comma_token(c))
				{
					lists.push_back(v);
					break;
				}
				else
				{
					v.push_back(c);
				}
			}
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

	@par 入力例
	@code
		background-color: red;
		color: blue !important;
	@endcode

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
	
	/*! 末尾の !important を削除する
	
	@return 削除した場合、 true を返す

	consume_declaration() から呼び出される。

	@sa consume_declaration()
	*/
	inline bool process_important(std::vector<component_value>& v)
	{
		std::u32string_view const sv = U"important";

		std::uint32_t st = 0;
		for (std::uint32_t i = v.size() - 1; 0 <= i; --i)
		{
			using wordring::whatwg::is_ascii_case_insensitive_match;

			if (is_whitespace_token(v[i]))
			{
				continue;
			}
			else if (st == 0 && is_ident_token(v[i]))
			{
				ident_token const& tkn = std::any_cast<ident_token>(v[i]);
				if (is_ascii_case_insensitive_match(tkn.m_value.begin(), tkn.m_value.end(), sv.begin(), sv.end()))
				{
					st = 1;
				}
				else break;
			}
			else if (st == 1 && is_delim_token(v[i]))
			{
				delim_token const& tkn = std::any_cast<delim_token>(v[i]);
				if (tkn.m_value == U'!')
				{
					v.erase(std::next(v.begin(), i), v.end());
					return true;
				}
				else break;
			}
			else break;
		}

		return false;
	}

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
		if (process_important(v))
		{
			decl.m_important_flag = true;
		}

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
