// wwwc/css_syntax/token.cpp

#include <wordring/wwwc/css_syntax/token.hpp>

#include <cassert>
#include <iterator>

using namespace wordring::wwwc::css;

// ------------------------------------------------------------------------------------------------

at_rule::~at_rule() = default;

// ------------------------------------------------------------------------------------------------

component_value::container_fn::container_fn(component_value::container_fn const& rhs)
	: m_value(std::make_unique<function>(*rhs.m_value.get()))
{
}

component_value::container_fn::container_fn(function const& fn)
	: m_value(std::make_unique<function>(fn))
{
}

component_value::container_fn::~container_fn() = default;

component_value::container_fn& component_value::container_fn::operator=(container_fn const& rhs)
{
	m_value = std::make_unique<function>(*rhs.m_value);
	return *this;
}

// ------------------------------------------------------------------------------------------------

component_value::container_sb::container_sb(component_value::container_sb const& rhs)
	: m_value(std::make_unique<simple_block>(*rhs.m_value.get()))
{
}

component_value::container_sb::container_sb(simple_block const& fn)
	: m_value(std::make_unique<simple_block>(fn))
{
}

component_value::container_sb::~container_sb() = default;

component_value::container_sb& component_value::container_sb::operator=(container_sb const& rhs)
{
	m_value = std::make_unique<simple_block>(*rhs.m_value);
	return *this;
}

// ------------------------------------------------------------------------------------------------

component_value::~component_value() = default;

component_value::component_value(syntax_primitive const& value)
{
	switch (value.type())
	{
	case syntax_primitive_name::IdentToken:
		m_value = value.get<ident_token>();
		break;
	case syntax_primitive_name::AtKeywordToken:
		m_value = value.get<at_keyword_token>();
		break;
	case syntax_primitive_name::HashToken:
		m_value = value.get<hash_token>();
		break;
	case syntax_primitive_name::StringToken:
		m_value = value.get<string_token>();
		break;
	case syntax_primitive_name::BadStringToken:
		m_value = value.get<bad_string_token>();
		break;
	case syntax_primitive_name::UrlToken:
		m_value = value.get<url_token>();
		break;
	case syntax_primitive_name::BadUrlToken:
		m_value = value.get<bad_url_token>();
		break;
	case syntax_primitive_name::DelimToken:
		m_value = value.get<delim_token>();
		break;
	case syntax_primitive_name::NumberToken:
		m_value = value.get<number_token>();
		break;
	case syntax_primitive_name::PercentageToken:
		m_value = value.get<percentage_token>();
		break;
	case syntax_primitive_name::DimensionToken:
		m_value = value.get<dimension_token>();
		break;
	case syntax_primitive_name::WhitespaceToken:
		m_value = value.get<whitespace_token>();
		break;
	case syntax_primitive_name::CdoToken:
		m_value = value.get<CDO_token>();
		break;
	case syntax_primitive_name::CdcToken:
		m_value = value.get<CDC_token>();
		break;
	case syntax_primitive_name::ColonToken:
		m_value = value.get<colon_token>();
		break;
	case syntax_primitive_name::SemicolonToken:
		m_value = value.get<semicolon_token>();
		break;
	case syntax_primitive_name::CommaToken:
		m_value = value.get<comma_token>();
		break;
	case syntax_primitive_name::CloseSquareToken:
		m_value = value.get<close_square_token>();
		break;
	case syntax_primitive_name::CloseParenToken:
		m_value = value.get<close_paren_token>();
		break;
	case syntax_primitive_name::CloseCurlyToken:
		m_value = value.get<close_curly_token>();
		break;
	case syntax_primitive_name::EofToken:
		m_value = value.get<eof_token>();
		//assert(false);
		break;

	case syntax_primitive_name::Function:
		m_value = value.get<function>();
		break;
	case syntax_primitive_name::SimpleBlock:
		m_value = value.get<simple_block>();
		break;

	default:
		assert(false);
		break;
	}
}

// ------------------------------------------------------------------------------------------------

syntax_primitive::syntax_primitive(component_value const& value)
{
	switch (value.type())
	{
	case syntax_primitive_name::IdentToken:
		m_value = value.get<ident_token>();
		break;
	case syntax_primitive_name::AtKeywordToken:
		m_value = value.get<at_keyword_token>();
		break;
	case syntax_primitive_name::HashToken:
		m_value = value.get<hash_token>();
		break;
	case syntax_primitive_name::StringToken:
		m_value = value.get<string_token>();
		break;
	case syntax_primitive_name::BadStringToken:
		m_value = value.get<bad_string_token>();
		break;
	case syntax_primitive_name::UrlToken:
		m_value = value.get<url_token>();
		break;
	case syntax_primitive_name::BadUrlToken:
		m_value = value.get<bad_url_token>();
		break;
	case syntax_primitive_name::DelimToken:
		m_value = value.get<delim_token>();
		break;
	case syntax_primitive_name::NumberToken:
		m_value = value.get<number_token>();
		break;
	case syntax_primitive_name::PercentageToken:
		m_value = value.get<percentage_token>();
		break;
	case syntax_primitive_name::DimensionToken:
		m_value = value.get<dimension_token>();
		break;
	case syntax_primitive_name::WhitespaceToken:
		m_value = value.get<whitespace_token>();
		break;
	case syntax_primitive_name::CdoToken:
		m_value = value.get<CDO_token>();
		break;
	case syntax_primitive_name::CdcToken:
		m_value = value.get<CDC_token>();
		break;
	case syntax_primitive_name::ColonToken:
		m_value = value.get<colon_token>();
		break;
	case syntax_primitive_name::SemicolonToken:
		m_value = value.get<semicolon_token>();
		break;
	case syntax_primitive_name::CommaToken:
		m_value = value.get<comma_token>();
		break;
	case syntax_primitive_name::CloseSquareToken:
		m_value = value.get<close_square_token>();
		break;
	case syntax_primitive_name::CloseParenToken:
		m_value = value.get<close_paren_token>();
		break;
	case syntax_primitive_name::CloseCurlyToken:
		m_value = value.get<close_curly_token>();
		break;
	case syntax_primitive_name::EofToken:
		m_value = value.get<eof_token>();
		//assert(false);
		break;

	case syntax_primitive_name::Function:
		m_value = value.get<function>();
		break;
	case syntax_primitive_name::SimpleBlock:
		m_value = value.get<simple_block>();
		break;

	default:
		assert(false);
		break;
	}
}

// ------------------------------------------------------------------------------------------------

syntax_primitive_stream::syntax_primitive_stream(syntax_primitive_stream::container& c)
	: m_first(c.begin())
	, m_last(c.end())
	, m_eof(eof_token{})
{
}

syntax_primitive_stream::syntax_primitive_stream(
	syntax_primitive_stream::const_iterator first, syntax_primitive_stream::const_iterator last)
	: m_first(first)
	, m_last(last)
	, m_eof(eof_token{})
{
}

syntax_primitive_stream::operator bool() const
{
	return m_first != m_last;
}

bool syntax_primitive_stream::operator !() const
{
	return m_first == m_last;
}

std::size_t syntax_primitive_stream::size() const
{
	return std::distance(m_first, m_last);
}

syntax_primitive_stream::const_iterator syntax_primitive_stream::begin() const
{
	return m_first;
}

syntax_primitive_stream::const_iterator syntax_primitive_stream::end() const
{
	return m_last;
}

syntax_primitive const& syntax_primitive_stream::current() const
{
	return static_cast<bool>(*this)  ? *m_first : m_eof;
}

syntax_primitive const& syntax_primitive_stream::consume()
{
	return  static_cast<bool>(*this) ? *m_first++ : m_eof;
}

void syntax_primitive_stream::reconsume()
{
	--m_first;
}

void syntax_primitive_stream::advance(const_iterator it)
{
	m_first = it;
}

bool syntax_primitive_stream::skip_whitespace()
{
	bool b = false;
	while (current().type() == syntax_primitive_name::WhitespaceToken)
	{
		b = true;
		consume();
	}
	return b;
}

// ------------------------------------------------------------------------------------------------
