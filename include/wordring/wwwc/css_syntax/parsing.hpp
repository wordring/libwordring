#pragma once

#include <wordring/wwwc/css_syntax/input_stream.hpp>
#include <wordring/wwwc/css_syntax/tokenization.hpp>

#include <wordring/wwwc/css_defs.hpp>

#include <wordring/compatibility.hpp>
#include <wordring/whatwg/infra/infra.hpp>

#include <algorithm>
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


	// --------------------------------------------------------------------------------------------
	// 5.2. Definitions
	//
	// https://drafts.csswg.org/css-syntax-3/#parser-definitions
	// https://triple-underscore.github.io/css-syntax-ja.html#parser-definitions
	// --------------------------------------------------------------------------------------------

	/*! @brief トークン列をストリームとしてアクセスするためのアダプタ
	*
	* @tparam BidirectionalIterator
	*
	* 規格で明示されていないが、ストリーム構築後 consume() を呼び出した後に、最初のトークンが
	* Current token として現れる仕様のよう。
	*/
	class token_stream
	{
	public:
		using container = std::vector<syntax_primitive>;
		using const_iterator = typename container::const_iterator;

	public:
		token_stream(container&& c)
			: m_v(std::move(c))
			, m_i(-1)
		{
			m_v.emplace_back(eof_token{});
		}

		syntax_primitive const& current_input_token()
		{
			if (!(0 <= m_i && m_i < static_cast<std::int32_t>(m_v.size()))) return m_v.back();
			return m_v[m_i];
		}

		syntax_primitive const& next_input_token()
		{
			assert(m_i < int(m_v.size()) - 1);
			return m_v[m_i + 1];
		}

		syntax_primitive const& consume()
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

	template <typename Syntax, typename ErrorHandler>
	inline std::vector<Syntax> parse_comma_list(token_stream& in, parse_context& ctx, ErrorHandler handler);

	template <typename ErrorHandler>
	inline std::vector<syntax_primitive> parse_list_of_component_values(token_stream& in, ErrorHandler handler);

	/*! @brief 入力をトークン列へ正規化する
	*
	* @param [in] in      トークンあるいはコンポーネント値のリスト
	* @param [in] handler [オプション] エラーハンドラー
	*
	* @return トークンあるいはコンポーネント値のリスト
	*
	* 入力として文字列が与えられた場合、オーバーロードが呼び出される。
	*
	* オーバーロード解決に参加させるためダミーのエラーハンドラーを用意しているが、呼び出されることはない。
	*
	* @sa https://drafts.csswg.org/css-syntax-3/#normalize-into-a-token-stream
	* @sa https://triple-underscore.github.io/css-syntax-ja.html#normalize-into-a-token-stream
	*/
	template <typename ErrorHandler = std::nullptr_t>
	inline std::vector<syntax_primitive>
		normalize_into_token_stream(std::vector<syntax_primitive>&& in, ErrorHandler handler = nullptr)
	{
		return std::move(in);
	}

	/*! @brief 入力をトークン列へ正規化する
	*
	* @param [in] in      スタイルシートを表すコード・ポイント列
	* @param [in] handler [オプション] エラーハンドラー
	* 
	* @return トークンのリスト
	*
	* 入力として文字列が与えられた場合、このバージョンの関数が呼び出される。
	* その他の入力が与えられた場合、オーバーロードが呼び出される。
	*
	* 有効なエラーハンドラが引数で供給された場合、トークン化で発生したエラーを報告するために、
	* 発生個所を指すイテレータを引数にエラーハンドラーが呼び出される。
	* 呼び出され方は、以下の通り。
	*
	* @code
	* 	handler(in.begin() + n);
	* @endcode
	*
	* @sa normalize_into_token_stream(std::vector<syntax_primitive>&&, ErrorHandler)
	*/
	template <typename ErrorHandler = std::nullptr_t>
	inline std::vector<syntax_primitive> normalize_into_token_stream(std::u32string&& in, ErrorHandler handler = nullptr)
	{
		std::vector<syntax_primitive> out;

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
	*
	* @param [in] in      入力ストリーム
	* @param [in] ctx     パーサー・コンテキスト
	* @param [in] handler エラー・ハンドラ
	*
	* @return コンポーネント値のリスト（std::vector<component_value>）、あるいは失敗。
	*
	* 入力をコンポーネント値のリストへ解析する。
	*
	* 引数sで供給される文法にマッチしない場合、戻り値は failure 状態となる。
	* 文法は、コンポーネント値のリストを引数に呼び出すと、マッチするとtrueを返し、マッチしないとfalseを返す関数類。
	* 文法のシグネチャは以下の通り。
	* @code
	* 	bool syntax(std::vector<syntax_primitive> const& v);
	* @endcode
	*
	* エラーハンドラが供給された場合、エラーを報告するために、
	* 発生個所を指す token_stream を引数にエラーハンドラーが呼び出される。
	* エラーハンドラは、以下の通り。
	*
	* @code
	* 	void handler(token_stream const& s);
	* @endcode
	*
	* この関数は、CSS Syntax Moduleの外から呼び出されることを想定している。
	*
	* @sa std::optional
	*
	* @sa https://drafts.csswg.org/css-syntax-3/#css-parse-something-according-to-a-css-grammar
	* @sa https://triple-underscore.github.io/css-syntax-ja.html#css-parse-something-according-to-a-css-grammar
	*/
	template <typename Syntax, typename ErrorHandler = std::nullptr_t>
	inline Syntax parse_grammar(token_stream& in, parse_context& ctx, ErrorHandler handler = nullptr)
	{
		std::vector<syntax_primitive> v = parse_list_of_component_values(in, handler);
		syntax_primitive_stream s(v);
		return Syntax::consume(s, ctx);
	}

	template <typename Syntax>
	inline Syntax parse_grammar(std::vector<syntax_primitive>&& in, parse_context& ctx)
	{
		token_stream s(normalize_into_token_stream(std::move(in), nullptr));
		return parse_grammar<Syntax>(s, ctx, nullptr);
	}

	template <typename Syntax>
	inline Syntax parse_grammar(std::u32string&& in, parse_context& ctx)
	{
		token_stream s(normalize_into_token_stream(std::move(in), nullptr));
		return parse_grammar<Syntax>(s, ctx, nullptr);
	}

	// --------------------------------------------------------------------------------------------
	// 5.3.2. Parse A Comma-Separated List According To A CSS Grammar
	//
	// https://drafts.csswg.org/css-syntax-3/#parse-comma-list
	// https://triple-underscore.github.io/css-syntax-ja.html#parse-comma-list
	// --------------------------------------------------------------------------------------------

	/*! @brief カンマ区切りのリストをCSS文法に沿って構文解析する
	*
	* @param [in] in      入力ストリーム
	* @param [in] ctx     パーサー・コンテキスト
	* @param [in] handler エラー・ハンドラ
	*
	* @return 「コンポーネント値のリスト（std::vector<syntax_primitive>）、あるいは失敗」のリスト
	*
	* 入力をコンポーネント値のリストのリストへ解析する。
	* 文法の検査も行う。
	*
	* 文法は、コンポーネント値のリストを引数に呼び出すと、マッチするとtrueを返し、マッチしないとfalseを返す関数類。
	* 文法のシグネチャは以下の通り。
	* @code
	* 	bool syntax(std::vector<component_value> const& v);
	* @endcode
	*
	* エラーハンドラが供給された場合、エラーを報告するために、
	* 発生個所を指す token_stream を引数にエラーハンドラーが呼び出される。
	* エラーハンドラは、以下の通り。
	*
	* @code
	* 	void handler(token_stream const& s);
	* @endcode
	*
	* この関数は、CSS Syntax Moduleの外から呼び出されることを想定している。
	*
	* @sa std::optional
	* @sa parse_grammar(token_stream&, Syntax, ErrorHandler)
	*
	* @sa https://drafts.csswg.org/css-syntax-3/#css-parse-a-comma-separated-list-according-to-a-css-grammar
	* @sa https://triple-underscore.github.io/css-syntax-ja.html#css-parse-a-comma-separated-list-according-to-a-css-grammar
	*/
	template <typename Syntax, typename ErrorHandler>
	inline std::vector<Syntax> parse_comma_list(token_stream& in, parse_context& ctx, ErrorHandler handler)
	{
		std::vector<Syntax> result;

		std::vector<std::vector<syntax_primitive>> v = parse_comma_separated_list_of_component_values(in, handler);
		auto it1 = v.begin();
		auto it2 = v.end();
		while (it1 != it2)
		{
			std::vector<syntax_primitive> tmp;
			std::swap(*it1++, tmp);
			token_stream s(std::move(tmp));
			result.emplace_back(parse_grammar<Syntax>(s, ctx, handler));
		}

		return result;
	}

	template <typename Syntax>
	inline std::vector<Syntax> parse_comma_list(std::vector<syntax_primitive>&& in, parse_context& ctx)
	{
		token_stream s(normalize_into_token_stream(std::move(in), nullptr));
		return parse_comma_list<Syntax>(s, ctx, nullptr);
	}

	template <typename Syntax>
	inline std::vector<Syntax> parse_comma_list(std::u32string&& in, parse_context& ctx)
	{
		token_stream s(normalize_into_token_stream(std::move(in), nullptr));
		return parse_comma_list<Syntax>(s, ctx, nullptr);
	}

	// --------------------------------------------------------------------------------------------
	// 5.3.3. Parse a stylesheet
	//
	// https://drafts.csswg.org/css-syntax-3/#parse-stylesheet
	// https://triple-underscore.github.io/css-syntax-ja.html#parse-stylesheet
	// --------------------------------------------------------------------------------------------

	/*! @brief スタイルシートを構文解析する
	*
	* @param [in] in      入力ストリーム
	* @param [in] handler エラーハンドラ
	*
	* @return 規則のリスト
	*
	* 入力を規則のリストへ解析する。
	*
	* エラーハンドラが供給された場合、エラーを報告するために、
	* 発生個所を指す token_stream を引数にエラーハンドラーが呼び出される。
	* エラーハンドラは、以下の通り。
	*
	* @code
	* 	void handler(token_stream const& s);
	* @endcode
	*
	* この関数は、CSS Syntax Moduleの外から呼び出されることを想定している。
	*
	* @sa parse_grammar(token_stream&, Syntax, ErrorHandler)
	*
	* @sa https://drafts.csswg.org/css-syntax-3/#parse-a-stylesheet
	* @sa https://triple-underscore.github.io/css-syntax-ja.html#parse-a-stylesheet
	*/
	template <typename ErrorHandler>
	inline std::vector<syntax_primitive> parse_stylesheet(token_stream& in, ErrorHandler handler)
	{
		std::vector<syntax_primitive> v = consume_list_of_rules(in, true, handler);
		return v;
	}

	inline std::vector<syntax_primitive> parse_stylesheet(std::vector<syntax_primitive>&& in)
	{
		token_stream s(normalize_into_token_stream(std::move(in), nullptr));
		return parse_stylesheet(s, nullptr);
	}

	/*! @brief スタイルシートを構文解析する
	*/
	inline std::vector<syntax_primitive> parse_stylesheet(std::u32string&& in)
	{
		token_stream s(normalize_into_token_stream(std::move(in), nullptr));
		return parse_stylesheet(s, nullptr);
	}

	/*! @brief スタイルシートを構文解析する
	*
	* この関数は、8bit文字列を入力として受け付ける。
	* エンコーディングを自動決定することが出来る。
	*/
	inline std::vector<syntax_primitive>
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
	*
	* @param [in] in      入力ストリーム
	* @param [in] handler エラーハンドラ
	*
	* @return 規則のリスト
	*
	* 入力を規則のリストへ解析する。
	*
	* エラーハンドラが供給された場合、エラーを報告するために、
	* 発生個所を指す token_stream を引数にエラーハンドラーが呼び出される。
	* エラーハンドラは、以下の通り。
	*
	* @code
	* 	void handler(token_stream const& s);
	* @endcode
	*
	* この関数は、CSS Syntax Moduleの外から呼び出されることを想定している。
	*
	* @sa parse_grammar(token_stream&, Syntax, ErrorHandler)
	*
	* @sa https://drafts.csswg.org/css-syntax-3/#parse-a-list-of-rules
	* @sa https://triple-underscore.github.io/css-syntax-ja.html#parse-a-list-of-rules
	*/
	template <typename ErrorHandler>
	inline std::vector<syntax_primitive> parse_list_of_rules(token_stream& in, ErrorHandler handler)
	{
		std::vector<syntax_primitive> v = consume_list_of_rules(in, true, handler);
		return v;
	}

	inline std::vector<syntax_primitive> parse_list_of_rules(std::vector<syntax_primitive>&& in)
	{
		token_stream s(normalize_into_token_stream(std::move(in), nullptr));
		return parse_list_of_rules(s, nullptr);
	}

	inline std::vector<syntax_primitive> parse_list_of_rules(std::u32string&& in)
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
	*
	* @param [in] in      入力ストリーム
	* @param [in] handler エラーハンドラ
	*
	* @return 規則、あるいは失敗
	*
	* 入力を規則へ解析する。
	*
	* エラーハンドラが供給された場合、エラーを報告するために、
	* 発生個所を指す token_stream を引数にエラーハンドラーが呼び出される。
	* エラーハンドラは、以下の通り。
	*
	* @code
	* 	void handler(token_stream const& s);
	* @endcode
	*
	* この関数は、CSS Syntax Moduleの外から呼び出されることを想定している。
	*
	* @sa parse_grammar(token_stream&, Syntax, ErrorHandler)
	*
	* @sa https://drafts.csswg.org/css-syntax-3/#parse-a-rule
	* @sa https://triple-underscore.github.io/css-syntax-ja.html#parse-a-rule
	*/
	template <typename ErrorHandler>
	inline std::optional<syntax_primitive> parse_rule(token_stream& in, ErrorHandler handler)
	{
		while (in.next_input_token().type() == syntax_primitive_name::WhitespaceToken) in.consume();
		if (in.next_input_token().type() == syntax_primitive_name::EofToken) return std::optional<syntax_primitive>();

		syntax_primitive rule;
		if (in.next_input_token().type() == syntax_primitive_name::AtKeywordToken)
		{
			rule = consume_at_rule(in, handler);
		}
		else
		{
			std::optional<qualified_rule> q = consume_qualified_rule(in, handler);
			if (!q) return std::optional<syntax_primitive>();
			rule = std::move(*q);
		}

		while (in.next_input_token().type() == syntax_primitive_name::WhitespaceToken) in.consume();
		if (in.next_input_token().type() == syntax_primitive_name::EofToken)
		{
			return std::make_optional<syntax_primitive>(std::move(rule));
		}

		return std::optional<syntax_primitive>();
	}

	inline std::optional<syntax_primitive> parse_rule(std::vector<syntax_primitive>&& in)
	{
		token_stream s(normalize_into_token_stream(std::move(in), nullptr));
		return parse_rule(s, nullptr);
	}

	inline std::optional<syntax_primitive> parse_rule(std::u32string&& in)
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
	*
	* @param [in] in      入力ストリーム
	* @param [in] handler エラーハンドラ
	*
	* @return 宣言、あるいは失敗
	*
	* 入力をコンポーネント値のリストへ解析する。
	*
	* エラーハンドラが供給された場合、エラーを報告するために、
	* 発生個所を指す token_stream を引数にエラーハンドラーが呼び出される。
	* エラーハンドラは、以下の通り。
	*
	* @code
	* 	void handler(token_stream const& s);
	* @endcode
	*
	* この関数は、CSS Syntax Moduleの外から呼び出されることを想定している。
	*
	* @sa parse_grammar(token_stream&, Syntax, ErrorHandler)
	*
	* @sa https://drafts.csswg.org/css-syntax-3/#parse-a-declaration
	* @sa https://triple-underscore.github.io/css-syntax-ja.html#parse-a-declaration
	*/
	template <typename ErrorHandler>
	inline std::optional<declaration> parse_declaration(token_stream& in, ErrorHandler handler)
	{
		while (in.next_input_token().type() == syntax_primitive_name::WhitespaceToken) in.consume();
		if (in.next_input_token().type() != syntax_primitive_name::IdentToken) return std::make_optional<declaration>();

		std::optional<declaration> decl = consume_declaration(in, handler);
		if (decl) return decl;

		return std::optional<declaration>();
	}

	inline std::optional<declaration> parse_declaration(std::vector<syntax_primitive>&& in)
	{
		token_stream s(normalize_into_token_stream(std::move(in), nullptr));
		return parse_declaration(s, nullptr);
	}

	inline std::optional<declaration> parse_declaration(std::u32string&& in)
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
	*
	* @param [in] in      入力ストリーム
	* @param [in] handler エラーハンドラ
	*
	* @return 「 Declaration あるいは  @-rule 」の混在リスト
	*
	* 入力を宣言リストへ解析する。
	*
	* エラーハンドラが供給された場合、エラーを報告するために、
	* 発生個所を指す token_stream を引数にエラーハンドラーが呼び出される。
	* エラーハンドラは、以下の通り。
	*
	* @code
	* 	void handler(token_stream const& s);
	* @endcode
	*
	* この関数は、CSS Syntax Moduleの外から呼び出されることを想定している。
	*
	* @sa parse_grammar(token_stream&, Syntax, ErrorHandler)
	*
	* @sa https://drafts.csswg.org/css-syntax-3/#parse-a-list-of-declarations
	* @sa https://triple-underscore.github.io/css-syntax-ja.html#parse-a-list-of-declarations
	*/
	template <typename ErrorHandler>
	inline std::vector<syntax_primitive> parse_list_of_declarations(token_stream& in, ErrorHandler handler)
	{
		return consume_list_of_declarations(in, handler);
	}

	inline std::vector<syntax_primitive> parse_list_of_declarations(std::vector<syntax_primitive>&& in)
	{
		token_stream s(normalize_into_token_stream(std::move(in), nullptr));
		return parse_list_of_declarations(s, nullptr);
	}

	inline std::vector<syntax_primitive> parse_list_of_declarations(std::u32string&& in)
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
	*
	* @param [in] in      入力ストリーム
	* @param [in] handler エラーハンドラ
	*
	* @return コンポーネント値、あるいは失敗
	*
	* 入力をコンポーネント値へ解析する。
	*
	* エラーハンドラが供給された場合、エラーを報告するために、
	* 発生個所を指す token_stream を引数にエラーハンドラーが呼び出される。
	* エラーハンドラは、以下の通り。
	*
	* @code
	* 	void handler(token_stream const& s);
	* @endcode
	*
	* この関数は、CSS Syntax Moduleの外から呼び出されることを想定している。
	*
	* @sa parse_grammar(token_stream&, Syntax, ErrorHandler)
	*
	* @sa https://drafts.csswg.org/css-syntax-3/#parse-a-component-value
	* @sa https://triple-underscore.github.io/css-syntax-ja.html#parse-a-component-value
	*/
	template <typename ErrorHandler>
	inline std::optional<component_value> parse_component_value(token_stream& in, ErrorHandler handler)
	{
		using result_type = std::optional<component_value>;

		while (in.next_input_token().type() == syntax_primitive_name::WhitespaceToken) in.consume();
		if (in.next_input_token().type() == syntax_primitive_name::EofToken) return result_type();

		component_value c = consume_component_value(in, handler);

		while (in.next_input_token().type() == syntax_primitive_name::WhitespaceToken) in.consume();
		if (in.next_input_token().type() == syntax_primitive_name::EofToken) return result_type(std::move(c));

		return result_type();
	}

	inline std::optional<component_value> parse_component_value(std::vector<syntax_primitive>&& in)
	{
		token_stream s(normalize_into_token_stream(std::move(in), nullptr));
		return parse_component_value(s, nullptr);
	}

	inline std::optional<component_value> parse_component_value(std::u32string&& in)
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
	* 
	* @param [in] in      入力ストリーム
	* @param [in] handler エラーハンドラ
	* 
	* @return コンポーネント値のリスト
	* 
	* 入力をコンポーネント値のリストへ解析する。
	*
	* エラーハンドラが供給された場合、エラーを報告するために、
	* 発生個所を指す token_stream を引数にエラーハンドラーが呼び出される。
	* エラーハンドラは、以下の通り。
	*
	* @code
	* 	void handler(token_stream const& s);
	* @endcode
	*
	* この関数は、CSS Syntax Moduleの外から呼び出されることを想定している。
	*
	* @sa parse_grammar(token_stream&, Syntax, ErrorHandler)
	*
	* @sa https://drafts.csswg.org/css-syntax-3/#parse-a-list-of-component-values
	* @sa https://triple-underscore.github.io/css-syntax-ja.html#parse-a-list-of-component-values
	*/
	template <typename ErrorHandler>
	inline std::vector<syntax_primitive> parse_list_of_component_values(token_stream& in, ErrorHandler handler)
	{
		std::vector<syntax_primitive> list;

		while (true)
		{
			component_value c = consume_component_value(in, handler);
			if (c.type() == syntax_primitive_name::EofToken) break;
			list.push_back(c);
		}

		return list;
	}

	inline std::vector<syntax_primitive> parse_list_of_component_values(std::vector<syntax_primitive>&& in)
	{
		token_stream s(normalize_into_token_stream(std::move(in), nullptr));
		return parse_list_of_component_values(s, nullptr);
	}

	inline std::vector<syntax_primitive> parse_list_of_component_values(std::u32string&& in)
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
	*
	* @param [in] in      入力ストリーム
	* @param [in] handler エラーハンドラ
	*
	* @return コンポーネント値のリストのリスト
	*
	* 入力をコンポーネント値のリストのリストへ解析する。
	*
	* エラーハンドラが供給された場合、エラーを報告するために、
	* 発生個所を指す token_stream を引数にエラーハンドラーが呼び出される。
	* エラーハンドラは、以下の通り。
	*
	* @code
	* 	void handler(token_stream const& s);
	* @endcode
	*
	* この関数は、CSS Syntax Moduleの外から呼び出されることを想定している。
	*
	* @sa parse_grammar(token_stream&, Syntax, ErrorHandler)
	*
	* @sa https://drafts.csswg.org/css-syntax-3/#parse-comma-separated-list-of-component-values
	* @sa https://triple-underscore.github.io/css-syntax-ja.html#parse-comma-separated-list-of-component-values
	*/
	template <typename ErrorHandler>
	inline std::vector<std::vector<syntax_primitive>>
		parse_comma_separated_list_of_component_values(token_stream& in, ErrorHandler handler)
	{
		std::vector<std::vector<syntax_primitive>> lists;

		while(true)
		{
			std::vector<syntax_primitive> v;
			while (true)
			{
				component_value c = consume_component_value(in, handler);
				switch (c.type())
				{
				case syntax_primitive_name::EofToken:
					lists.push_back(std::move(v));
					return lists;
				case syntax_primitive_name::CommaToken:
					lists.push_back(std::move(v));
					break;
				default:
					v.push_back(c);
				}
			}
		}

		return lists;
	}

	/*! @brief コンマ区切りのコンポーネント値のリストを構文解析する
	*
	* @param [in] in 入力トークン列
	*
	* @return コンポーネント値のリストのリスト
	*
	* @sa parse_comma_separated_list_of_component_values(token_stream&, ErrorHandler)
	*/
	inline std::vector<std::vector<syntax_primitive>>
		parse_comma_separated_list_of_component_values(std::vector<syntax_primitive>&& in)
	{
		token_stream s(normalize_into_token_stream(std::move(in), nullptr));
		return parse_comma_separated_list_of_component_values(s, nullptr);
	}

	/*! @brief コンマ区切りのコンポーネント値のリストを構文解析する
	*
	* @param [in] in 入力文字列
	*
	* @return コンポーネント値のリストのリスト
	*
	* @sa parse_comma_separated_list_of_component_values(token_stream&, ErrorHandler)
	*/
	inline std::vector<std::vector<syntax_primitive>>
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

	template <typename ErrorHandler>
	inline std::vector<syntax_primitive> consume_list_of_rules(token_stream& in, bool top_level = false, ErrorHandler handler = nullptr);

	template <typename ErrorHandler>
	inline at_rule consume_at_rule(token_stream& in, ErrorHandler handler);

	template <typename ErrorHandler>
	inline std::optional<qualified_rule> consume_qualified_rule(token_stream& in, ErrorHandler handler);

	template <typename ErrorHandler>
	inline std::vector<syntax_primitive> consume_list_of_declarations(token_stream& in, ErrorHandler handler);

	template <typename ErrorHandler>
	inline std::optional<declaration> consume_declaration(token_stream& in, ErrorHandler handler);

	template <typename ErrorHandler>
	inline component_value consume_component_value(token_stream& in, ErrorHandler handler);

	template <typename ErrorHandler>
	inline simple_block consume_simple_block(token_stream& in, ErrorHandler handler);

	template <typename ErrorHandler>
	inline function consume_function(token_stream& in, ErrorHandler handler);

	// --------------------------------------------------------------------------------------------
	// 5.4.1. Consume a list of rules
	//
	// https://drafts.csswg.org/css-syntax-3/#consume-list-of-rules
	// https://triple-underscore.github.io/css-syntax-ja.html#consume-list-of-rules
	// --------------------------------------------------------------------------------------------

	/*! @brief Ruleのリストを消費する
	* 
	* @param [in] in        トークン列のストリーム
	* @param [in] top_level トップレベルフラグ（規格内の呼び出し側で指定される）
	* @param [in] handler   エラー処理を行うユーザー定義の関数
	*
	* @return At-rule・Qualified rule の混在リスト
	*
	* この関数は、CSS Syntax Moduleの外から呼び出されることを想定していない。
	* 
	* @sa https://drafts.csswg.org/css-syntax-3/#consume-a-list-of-rules
	* @sa https://triple-underscore.github.io/css-syntax-ja.html#consume-a-list-of-rules
	*/
	template <typename ErrorHandler>
	inline std::vector<syntax_primitive>
		consume_list_of_rules(token_stream& in, bool top_level, ErrorHandler handler)

	{
		std::vector<syntax_primitive> rules;

		while (true)
		{
			syntax_primitive const& tkn = in.consume();

			switch (tkn.type())
			{
			case syntax_primitive_name::WhitespaceToken:
				continue;
			case syntax_primitive_name::EofToken:
				return rules;
			case syntax_primitive_name::CdoToken:
			case syntax_primitive_name::CdcToken:
				if (top_level)
				{
					continue;
				}
				else
				{
					in.reconsume();
					std::optional<qualified_rule> rule = consume_qualified_rule(in, handler);
					if (rule.has_value()) rules.push_back(std::move(*rule));
				}
			case syntax_primitive_name::AtKeywordToken:
				in.reconsume();
				rules.push_back(consume_at_rule(in, handler));
				continue;
			default:
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
	* 
	* @param [in] in      トークン列のストリーム
	* @param [in] handler エラー処理を行うユーザー定義の関数
	*
	* @return rule
	*
	* この関数は、CSS Syntax Moduleの外から呼び出されることを想定していない。
	*
	* この関数は、入力の先頭が at_keyword_token と仮定している。
	*
	* @sa At-rule
	*
	* @sa https://drafts.csswg.org/css-syntax-3/#consume-an-at-rule
	* @sa https://triple-underscore.github.io/css-syntax-ja.html#consume-an-at-rule
	*/
	template <typename ErrorHandler>
	inline at_rule consume_at_rule(token_stream& in, ErrorHandler handler)
	{
		syntax_primitive const& tkn = in.consume();
		assert(in.current_input_token().type() == syntax_primitive_name::AtKeywordToken);

		at_rule rule;
		rule.m_name = tkn.get<at_keyword_token>().m_value;

		while (true)
		{
			syntax_primitive const& tkn = in.consume();
			switch (tkn.type())
			{
			case syntax_primitive_name::SemicolonToken:
				return rule;
			case syntax_primitive_name::EofToken:
				if constexpr (std::is_invocable_v<ErrorHandler, token_stream&>) handler(in);
				return rule;
			case syntax_primitive_name::OpenCurlyToken:
				rule.m_block = std::make_unique<simple_block>(consume_simple_block(in, handler));
				return rule;
			case syntax_primitive_name::SimpleBlock:
				if (tkn.get<simple_block>().m_associated_token == U'{')
				{
					rule.m_block = std::make_unique<simple_block>(tkn.get<simple_block>());
					return rule;
				}
			default:
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
	*
	* @param [in] in      トークン列のストリーム
	* @param [in] handler エラー処理を行うユーザー定義の関数
	*
	* @return rule
	*
	* この関数は、CSS Syntax Moduleの外から呼び出されることを想定していない。
	*
	* @sa Qualified rule あるいは何も返さない
	*
	* @sa https://drafts.csswg.org/css-syntax-3/#consume-a-qualified-rule
	* @sa https://triple-underscore.github.io/css-syntax-ja.html#consume-a-qualified-rule
	*/
	template <typename ErrorHandler>
	inline std::optional<qualified_rule> consume_qualified_rule(token_stream& in, ErrorHandler handler)
	{
		qualified_rule rule;

		while (true)
		{
			syntax_primitive const& tkn = in.consume();
			switch (tkn.type())
			{
			case syntax_primitive_name::EofToken:
				if constexpr (std::is_invocable_v<ErrorHandler, token_stream&>) handler(in);
				return std::optional<qualified_rule>();
			case syntax_primitive_name::OpenCurlyToken:
				rule.m_block = consume_simple_block(in, handler);
				return std::make_optional(std::move(rule));
			case syntax_primitive_name::SimpleBlock:
				if (tkn.get<simple_block>().m_associated_token == U'{')
				{
					rule.m_block = tkn.get<simple_block>();
					return std::make_optional(std::move(rule));
				}
			default:
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
	*
	* @param [in] in      トークン列のストリーム
	* @param [in] handler エラー処理を行うユーザー定義の関数
	*
	* @return 「 Declaration あるいは  @-rule 」の混在リスト
	*
	* @par 入力例
	* @code
	* 	background-color: red;
	* 	color: blue !important;
	* @endcode
	*
	* この関数は、CSS Syntax Moduleの外から呼び出されることを想定していない。
	*
	* @sa declaration
	*
	* @sa https://drafts.csswg.org/css-syntax-3/#consume-a-list-of-declarations
	* @sa https://triple-underscore.github.io/css-syntax-ja.html#consume-a-list-of-declarations
	*/
	template <typename ErrorHandler>
	inline std::vector<syntax_primitive> consume_list_of_declarations(token_stream& in, ErrorHandler handler)
	{
		std::vector<syntax_primitive> declarations;

		while (true)
		{
			syntax_primitive const& tkn = in.consume();
			switch (tkn.type())
			{
			case syntax_primitive_name::WhitespaceToken:
			case syntax_primitive_name::SemicolonToken:
				continue;
			case syntax_primitive_name::EofToken:
				return declarations;
			case syntax_primitive_name::AtKeywordToken:
				in.reconsume();
				declarations.push_back(consume_at_rule(in, handler));
				continue;
			case syntax_primitive_name::IdentToken:
			{
				std::vector<syntax_primitive> tmp(1, tkn);

				while (true)
				{
					syntax_primitive_name type = in.next_input_token().type();

					if (type == syntax_primitive_name::SemicolonToken
					 || type == syntax_primitive_name::EofToken) break;
					
					tmp.push_back(consume_component_value(in, handler));
				}

				token_stream in2(std::move(tmp));
				std::optional<declaration> decl = consume_declaration(in2, handler);
				if (decl.has_value()) declarations.push_back(std::move(*decl));

				continue;
			}
			default:
				if constexpr (std::is_invocable_v<ErrorHandler, token_stream&>) handler(in);
				in.reconsume();
				while (true)
				{
					syntax_primitive_name type = in.next_input_token().type();

					if (type == syntax_primitive_name::SemicolonToken
						|| type == syntax_primitive_name::EofToken) break;

					consume_component_value(in, handler);
				}
				continue;
			}
		}

		assert(false);
		return std::vector<syntax_primitive>();
	}

	// --------------------------------------------------------------------------------------------
	// 5.4.5. Consume a declaration
	//
	// https://drafts.csswg.org/css-syntax-3/#consume-declaration
	// https://triple-underscore.github.io/css-syntax-ja.html#consume-declaration
	// --------------------------------------------------------------------------------------------
	
	/*! 末尾の !important を削除する
	* 
	* @return 削除した場合、 true を返す
	*
	* consume_declaration() から呼び出される。
	*
	* @sa consume_declaration()
	*/
	inline bool process_important(std::vector<component_value>& v)
	{
		std::u32string_view const sv = U"important";

		std::uint32_t st = 0;
		for (std::size_t i = v.size() - 1; 0 <= i; --i)
		{
			using wordring::whatwg::is_ascii_case_insensitive_match;

			if (v[i].type() == syntax_primitive_name::WhitespaceToken)
			{
				continue;
			}
			else if (st == 0 && v[i].type() == syntax_primitive_name::IdentToken)
			{
				ident_token const& tkn = v[i].get<ident_token>();
				if (is_ascii_case_insensitive_match(tkn.m_value.begin(), tkn.m_value.end(), sv.begin(), sv.end()))
				{
					st = 1;
				}
				else break;
			}
			else if (st == 1 && v[i].type() == syntax_primitive_name::DelimToken)
			{
				delim_token const& tkn = v[i].get<delim_token>();
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
	*
	* @param [in] in      トークン列のストリーム
	* @param [in] handler エラー処理を行うユーザー定義の関数
	*
	* @return declaration
	*
	* この関数は、CSS Syntax Moduleの外から呼び出されることを想定していない。
	*
	* @sa declaration
	*
	* @sa https://drafts.csswg.org/css-syntax-3/#consume-a-declaration
	* @sa https://triple-underscore.github.io/css-syntax-ja.html#consume-a-declaration
	*/
	template <typename ErrorHandler>
	inline std::optional<declaration> consume_declaration(token_stream& in, ErrorHandler handler)
	{
		in.consume();
		assert(in.current_input_token().type() == syntax_primitive_name::IdentToken);
		declaration decl;
		decl.m_name = in.current_input_token().get<ident_token>().m_value;

		while (in.next_input_token().type() == syntax_primitive_name::WhitespaceToken) in.consume();

		if (in.next_input_token().type() != syntax_primitive_name::ColonToken)
		{
			if constexpr (std::is_invocable_v<ErrorHandler, token_stream&>) handler(in);
			return std::optional<declaration>();
		}

		in.consume();

		while (in.next_input_token().type() == syntax_primitive_name::WhitespaceToken) in.consume();

		while (in.next_input_token().type() != syntax_primitive_name::EofToken)
		{
			decl.m_value.push_back(consume_component_value(in, handler));
		}

		std::vector<component_value>& v = decl.m_value;
		if (process_important(v))
		{
			decl.m_important_flag = true;
		}

		while (!v.empty() && v.back().type() == syntax_primitive_name::WhitespaceToken) v.pop_back();

		return std::make_optional(std::move(decl));
	}

	// --------------------------------------------------------------------------------------------
	// 5.4.6. Consume a component value
	//
	// https://drafts.csswg.org/css-syntax-3/#consume-component-value
	// https://triple-underscore.github.io/css-syntax-ja.html#consume-component-value
	// --------------------------------------------------------------------------------------------

	template <typename ErrorHandler>
	inline component_value consume_component_value(token_stream& in, ErrorHandler handler)
	{
		in.consume();

		syntax_primitive const& tkn = in.current_input_token();
		switch (tkn.type())
		{
		case syntax_primitive_name::OpenCurlyToken:
		case syntax_primitive_name::OpenSquareToken:
		case syntax_primitive_name::OpenParenToken:
			return component_value(consume_simple_block(in, handler));
		case syntax_primitive_name::FunctionToken:
			return component_value(consume_function(in, handler));
		default:
			break;
		}

		return component_value(tkn);
	}

	// --------------------------------------------------------------------------------------------
	// 5.4.7. Consume a simple block
	//
	// https://drafts.csswg.org/css-syntax-3/#consume-simple-block
	// 
	// --------------------------------------------------------------------------------------------

	template <typename ErrorHandler>
	inline simple_block consume_simple_block(token_stream& in, ErrorHandler handler)
	{
		simple_block block;

		syntax_primitive_name ending = static_cast<syntax_primitive_name>(0);

		switch (in.current_input_token().type())
		{
		case syntax_primitive_name::OpenCurlyToken:
			block.m_associated_token = U'{';
			ending = syntax_primitive_name::CloseCurlyToken;
			break;
		case syntax_primitive_name::OpenSquareToken:
			block.m_associated_token = U'[';
			ending = syntax_primitive_name::CloseSquareToken;
			break;
		case syntax_primitive_name::OpenParenToken:
			block.m_associated_token = U'(';
			ending = syntax_primitive_name::CloseParenToken;
			break;
		default:
			break;
		}

		while (true)
		{
			syntax_primitive const& tkn = in.consume();
			switch (tkn.type())
			{
			case syntax_primitive_name::CloseCurlyToken:
			case syntax_primitive_name::CloseSquareToken:
			case syntax_primitive_name::CloseParenToken:
				if (ending == tkn.type()) return block;
			case syntax_primitive_name::EofToken:
				if constexpr (std::is_invocable_v<ErrorHandler, token_stream&>) handler(in);
				return block;
			default:
				break;
			}

			in.reconsume();
			block.m_value.push_back(consume_component_value(in, handler));
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

	template <typename ErrorHandler>
	inline function consume_function(token_stream& in, ErrorHandler handler)
	{
		assert(in.current_input_token().type() == syntax_primitive_name::FunctionToken);

		function fn{ in.current_input_token().get<function_token>().m_value };

		while (true)
		{
			syntax_primitive const& tkn = in.consume();
			switch (tkn.type())
			{
			case syntax_primitive_name::CloseParenToken:
				return fn;
			case syntax_primitive_name::EofToken:
				if constexpr (std::is_invocable_v<ErrorHandler, token_stream&>) handler(in);
				return fn;
			default:
				in.reconsume();
				fn.m_value.push_back(consume_component_value(in, handler));
			}
		}

		assert(false);
		return function();
	}
}
