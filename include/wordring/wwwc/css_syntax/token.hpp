#pragma once

#include <cassert>
#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace wordring::wwwc::css
{
	class syntax_primitive;

	// --------------------------------------------------------------------------------------------
	// 4. Tokenization
	//
	// https://drafts.csswg.org/css-syntax-3/#tokenization
	// https://triple-underscore.github.io/css-syntax-ja.html#tokenization
	// --------------------------------------------------------------------------------------------

	enum class syntax_primitive_name : std::size_t
	{
		Token = 1,        //!< これ以降 Rule までトークン

		IdentToken,       //!< 識別子
		FunctionToken,    //!< 関数
		AtKeywordToken,   //!< @xxx
		HashToken,        //!< #xxx
		StringToken,      //!< 文字列
		BadStringToken,   //!< 悪い文字列
		UrlToken,         //!< URL
		BadUrlToken,      //!< 悪いURL
		DelimToken,       //!< 区切り文字
		NumberToken,      //!< 数字
		PercentageToken,  //!< 数字%
		DimensionToken,   //!< 数字 単位
		WhitespaceToken,  //!< 空白文字
		CdoToken,         //!< "<!--"
		CdcToken,         //!< "-->"
		ColonToken,       //!< ':'
		SemicolonToken,   //!< ';'
		CommaToken,       //!< ','
		OpenSquareToken,  //!< '['
		CloseSquareToken, //!< ']'
		OpenParenToken,   //!< '('
		CloseParenToken,  //!< ')'
		OpenCurlyToken,   //!< '{'
		CloseCurlyToken,  //!< '}'
		EofToken,         //!< EOF

		SyntaxItem,        //!< これ以降、構文アイテム

		AtRule,          //!< at-rule          https://triple-underscore.github.io/css-syntax-ja.html#at-rule
		QualifiedRule,   //!< qualified rule   https://triple-underscore.github.io/css-syntax-ja.html#qualified-rule
		Declaration,     //!< declaration 
		ComponentValue,  //!< component value
		PreservedTokens, //!< preserved tokens
		Function,        //!< function
		SimpleBlock,     //!< simple block
	};

	/*! @brief 整数と小数を識別するためのフラグ値

	@sa number_token
	@sa dimension_token
	*/
	enum class number_type_flag_name { integer = 1, number };

	/*! @brief 識別子トークン */
	struct ident_token { std::u32string m_value; /*!< 識別子名 */ };

	/*! @brief 関数トークン */
	struct function_token { std::u32string m_value; /*!< 関数名 */ };

	/*! @brief @xxxトークン */
	struct at_keyword_token { std::u32string m_value; /*!<  */ };

	/*! @brief #xxxトークン */
	struct hash_token
	{
		enum class type_flag_name
		{
			id = 1,      /*!< ID */
			unrestricted /*!< 制限なし */
		};

		std::u32string m_value;
		type_flag_name m_type_flag = type_flag_name::unrestricted;
	};

	/*! @brief 文字列トークン */
	struct string_token { std::u32string m_value; /*!< 文字列 */ };

	/*! @brief 異常文字列トークン */
	struct bad_string_token {};

	/*! @brief URL トークン */
	struct url_token { std::u32string m_value; /*!< URL */ };

	/*! @brief 異常 URL トークン */
	struct bad_url_token {};

	/*! @brief 区切り文字トークン */
	struct delim_token { char32_t m_value = 0; /*!< 区切り文字 */ };

	/*! @brief 数値トークン */
	struct number_token
	{
		double m_value = 0; /*!< 数値 */
		number_type_flag_name m_type_flag = number_type_flag_name::integer; /*!< 整数と小数を識別するフラグ */
	};

	/*! @brief 数値 % トークン */
	struct percentage_token { double m_value; /*!< 数値 */ };

	struct dimension_token
	{
		double m_value = 0;    /*!< 数値 */
		std::u32string m_unit; /*!< 単位 */
		number_type_flag_name m_type_flag = number_type_flag_name::integer; /*!< 整数と小数を識別するフラグ */
	};

	/*! @brief 空白文字トークン */
	struct whitespace_token {};

	/*! @brief "<!--" トークン */
	struct CDO_token {};

	/*! @brief "-->" トークン */
	struct CDC_token {};

	/*! @brief ':' トークン */
	struct colon_token {};

	/*! @brief ';' トークン */
	struct semicolon_token {};

	/*! @brief ',' トークン */
	struct comma_token {};

	/*! @brief '[' トークン */
	struct open_square_token {};

	/*! @brief ']' トークン */
	struct close_square_token {};

	/*! @brief '(' トークン */
	struct open_paren_token {};

	/*! @brief ')' トークン */
	struct close_paren_token {};

	/*! @brief '{' トークン */
	struct open_curly_token {};

	/*! @brief '}' トークン */
	struct close_curly_token {};

	/*! @brief EOF トークン */
	struct eof_token {};

	// --------------------------------------------------------------------------------------------
	// 5. Parsing
	//
	// https://drafts.csswg.org/css-syntax-3/#parsing
	// https://triple-underscore.github.io/css-syntax-ja.html#parsing
	// --------------------------------------------------------------------------------------------

	class component_value;

	/*! @brief <b>関数</b>を表現する構文アイテム
	*/
	struct function
	{
		std::u32string               m_name;
		std::vector<component_value> m_value;
	};

	/*! @brief <b>単純ブロック</b>を表現する AST ノード
	*/
	struct simple_block
	{
		char32_t                     m_associated_token = 0;
		std::vector<component_value> m_value;
	};

	class preserved_tokens
	{
		using value_type = std::variant<
			ident_token,
			at_keyword_token,
			hash_token,
			string_token,
			bad_string_token,
			url_token,
			bad_url_token,
			delim_token,
			number_token,
			percentage_token,
			dimension_token,
			whitespace_token,
			CDO_token,
			CDC_token,
			colon_token,
			semicolon_token,
			comma_token,
			close_square_token,
			close_paren_token,
			close_curly_token,
			eof_token>;
	};

	/*! @brief <b>CSSコンポーネント値</b>を表現する AST ノード

	- preserved tokens
	- function
	- simple_block
	*/
	class component_value
	{
	public:
		struct container_fn
		{
			container_fn() = default;
			container_fn(container_fn const& rhs);
			container_fn(container_fn&& rhs) = default;
			container_fn(function const& fn);
			~container_fn();

			container_fn& operator=(container_fn const& rhs);
			container_fn& operator=(container_fn&& rhs) = default;

			std::unique_ptr<function> m_value;
		};

		struct container_sb
		{
			container_sb() = default;
			container_sb(container_sb const& rhs);
			container_sb(container_sb&& rhs) = default;
			container_sb(simple_block const& fn);
			~container_sb();

			container_sb& operator=(container_sb const& rhs);
			container_sb& operator=(container_sb&& rhs) = default;

			std::unique_ptr<simple_block> m_value;
		};

	public:
		using value_type = std::variant<
			std::monostate,
			ident_token,
			at_keyword_token,
			hash_token,
			string_token,
			bad_string_token,
			url_token,
			bad_url_token,
			delim_token,
			number_token,
			percentage_token,
			dimension_token,
			whitespace_token,
			CDO_token,
			CDC_token,
			colon_token,
			semicolon_token,
			comma_token,
			close_square_token,
			close_paren_token,
			close_curly_token,
			eof_token,

			container_fn,
			container_sb>;

	public:
		component_value() = default;
		component_value(component_value const&) = default;
		component_value(component_value&&) = default;

		explicit component_value(function const& value)
			: m_value(container_fn(value))
		{
		}

		explicit component_value(simple_block const& value)
			: m_value(container_sb(value))
		{
		}

		explicit component_value(syntax_primitive const& value);

		template <typename T>
		explicit component_value(T const& value)
			: m_value(value)
		{
		}

		~component_value();

		component_value& operator=(component_value const&) = default;

		syntax_primitive_name type() const
		{
			switch (m_value.index())
			{
			case 1:  return syntax_primitive_name::IdentToken;
			case 2:  return syntax_primitive_name::AtKeywordToken;
			case 3:  return syntax_primitive_name::HashToken;
			case 4:  return syntax_primitive_name::StringToken;
			case 5:  return syntax_primitive_name::BadStringToken;
			case 6:  return syntax_primitive_name::UrlToken;
			case 7:  return syntax_primitive_name::BadUrlToken;
			case 8:  return syntax_primitive_name::DelimToken;
			case 9:  return syntax_primitive_name::NumberToken;
			case 10: return syntax_primitive_name::PercentageToken;
			case 11: return syntax_primitive_name::DimensionToken;
			case 12: return syntax_primitive_name::WhitespaceToken;
			case 13: return syntax_primitive_name::CdoToken;
			case 14: return syntax_primitive_name::CdcToken;
			case 15: return syntax_primitive_name::ColonToken;
			case 16: return syntax_primitive_name::SemicolonToken;
			case 17: return syntax_primitive_name::CommaToken;
			case 18: return syntax_primitive_name::CloseSquareToken;
			case 19: return syntax_primitive_name::CloseParenToken;
			case 20: return syntax_primitive_name::CloseCurlyToken;
			case 21: return syntax_primitive_name::EofToken;

			case 22: return syntax_primitive_name::Function;
			case 23: return syntax_primitive_name::SimpleBlock;

			default:
				break;
			}
			return static_cast<syntax_primitive_name>(0);
		}

		bool has_value() const { return m_value.index() != 0; }

		template <typename T>
		T& get()
		{
			if constexpr (std::is_same_v<T, function>)
			{
				assert(std::holds_alternative<container_fn>(m_value));
				return *std::get_if<container_fn>(&m_value)->m_value;
			}
			else if constexpr (std::is_same_v<T, simple_block>)
			{
				assert(std::holds_alternative<container_sb>(m_value));
				return *std::get_if<container_sb>(&m_value)->m_value;
			}
			else
			{
				assert(std::holds_alternative<T>(m_value));
				return *std::get_if<T>(&m_value);
			}
		}

		template <typename T>
		T const& get() const
		{
			if constexpr (std::is_same_v<T, function>)
			{
				assert(std::holds_alternative<container_fn>(m_value));
				return *std::get_if<container_fn>(&m_value)->m_value;
			}
			else if constexpr (std::is_same_v<T, simple_block>)
			{
				assert(std::holds_alternative<container_sb>(m_value));
				return *std::get_if<container_sb>(&m_value)->m_value;
			}
			else
			{
				assert(std::holds_alternative<T>(m_value));
				return *std::get_if<T>(&m_value);
			}
		}

	private:
		value_type m_value;
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
		at_rule()
			: m_block()
		{
		}

		at_rule(at_rule const& rhs)
			: m_name(rhs.m_name)
			, m_prelude(rhs.m_prelude)
			, m_block()
		{
			if (rhs.m_block) m_block = std::make_unique<simple_block>(*rhs.m_block.get());
		}

		at_rule(at_rule&& rhs) = default;

		at_rule& operator=(at_rule const& rhs)
		{
			m_name = rhs.m_name;
			m_prelude = rhs.m_prelude;
			m_block = std::make_unique<simple_block>(*rhs.m_block);

			return *this;
		}

		at_rule& operator=(at_rule&& rhs) = default;

		~at_rule();

		std::u32string m_name;
		std::vector<component_value> m_prelude;
		std::unique_ptr<simple_block> m_block;
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
		std::u32string m_name;
		std::vector<component_value> m_value;
		bool m_important_flag = false;
	};

	/*! @brief トークンや構文アイテムを表現するクラス
	*/
	class syntax_primitive
	{
		using value_type = std::variant<
			std::monostate,
			ident_token,
			function_token,
			at_keyword_token,
			hash_token,
			string_token,
			bad_string_token,
			url_token,
			bad_url_token,
			delim_token,
			number_token,
			percentage_token,
			dimension_token,
			whitespace_token,
			CDO_token,
			CDC_token,
			colon_token,
			semicolon_token,
			comma_token,
			open_square_token,
			close_square_token,
			open_paren_token,
			close_paren_token,
			open_curly_token,
			close_curly_token,
			eof_token,

			preserved_tokens,
			function,
			simple_block,
			component_value,
			at_rule,
			qualified_rule,
			declaration>;

	public:
		syntax_primitive() = default;
		syntax_primitive(syntax_primitive const& rhs) = default;
		syntax_primitive(syntax_primitive&& rhs) = default;

		template <typename T>
		syntax_primitive(T const& token)
			: m_value(token)
		{
		}

		syntax_primitive(component_value const& token);

		void operator=(syntax_primitive const& rhs) { m_value = rhs.m_value; }

		syntax_primitive_name type() const
		{
			switch (m_value.index())
			{
			case 1:  return syntax_primitive_name::IdentToken;
			case 2:  return syntax_primitive_name::FunctionToken;
			case 3:  return syntax_primitive_name::AtKeywordToken;
			case 4:  return syntax_primitive_name::HashToken;
			case 5:  return syntax_primitive_name::StringToken;
			case 6:  return syntax_primitive_name::BadStringToken;
			case 7:  return syntax_primitive_name::UrlToken;
			case 8:  return syntax_primitive_name::BadUrlToken;
			case 9:  return syntax_primitive_name::DelimToken;
			case 10: return syntax_primitive_name::NumberToken;
			case 11: return syntax_primitive_name::PercentageToken;
			case 12: return syntax_primitive_name::DimensionToken;
			case 13: return syntax_primitive_name::WhitespaceToken;
			case 14: return syntax_primitive_name::CdoToken;
			case 15: return syntax_primitive_name::CdcToken;
			case 16: return syntax_primitive_name::ColonToken;
			case 17: return syntax_primitive_name::SemicolonToken;
			case 18: return syntax_primitive_name::CommaToken;
			case 19: return syntax_primitive_name::OpenSquareToken;
			case 20: return syntax_primitive_name::CloseSquareToken;
			case 21: return syntax_primitive_name::OpenParenToken;
			case 22: return syntax_primitive_name::CloseParenToken;
			case 23: return syntax_primitive_name::OpenCurlyToken;
			case 24: return syntax_primitive_name::CloseCurlyToken;
			case 25: return syntax_primitive_name::EofToken;

			case 26: return syntax_primitive_name::PreservedTokens;
			case 27: return syntax_primitive_name::Function;
			case 28: return syntax_primitive_name::SimpleBlock;
			case 29: return syntax_primitive_name::ComponentValue;
			case 30: return syntax_primitive_name::AtRule;
			case 31: return syntax_primitive_name::QualifiedRule;
			case 32: return syntax_primitive_name::Declaration;

			default:
				break;
			}
			return static_cast<syntax_primitive_name>(0);
		}

		bool has_value() const { return m_value.index() != 0; }

		template <typename T>
		T& get()
		{
			assert(std::holds_alternative<T>(m_value));
			return *std::get_if<T>(&m_value);
		}

		template <typename T>
		T const& get() const
		{
			assert(std::holds_alternative<T>(m_value));
			return *std::get_if<T>(&m_value);
		}

	private:
		value_type m_value;
	};

	/*! @brief syntax_primitive の配列をストリーム化するアダプタ
	*/
	class syntax_primitive_stream
	{
	public:
		using container      = std::vector<syntax_primitive>;
		//using iterator       = typename container::iterator;
		using const_iterator = typename container::const_iterator;
		using value_type     = syntax_primitive;

	public:
		syntax_primitive_stream() = default;
		syntax_primitive_stream(syntax_primitive_stream const&) = default;
		syntax_primitive_stream(syntax_primitive_stream&&) = default;

		syntax_primitive_stream(container& c);

		syntax_primitive_stream(const_iterator first, const_iterator last);

		operator bool() const;

		bool operator !() const;

		std::size_t size() const;

		const_iterator begin() const;

		const_iterator end() const;

		syntax_primitive const& current() const;

		/*! @brief トークンを一つ消費する
		
		@return 消費したトークン
		*/
		syntax_primitive const& consume();

		void reconsume();

		void advance(const_iterator it);

		bool skip_whitespace();

	private:
		const_iterator m_first;
		const_iterator m_last;

		syntax_primitive m_eof;
	};
}
