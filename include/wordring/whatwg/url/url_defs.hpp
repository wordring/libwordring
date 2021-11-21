#pragma once

#include <system_error>

namespace wordring::whatwg
{
	enum class url_error_name : int
	{
		/*! IDNA （国際化ドメイン名）変換中のエラー */
		IdnaTranslationFailed = 1,
		/*! 解析に失敗 */
		ParseFailed,
		ValidationError,
	};
}

namespace std
{
	template<> struct is_error_code_enum<wordring::whatwg::url_error_name> : true_type {};
}

namespace wordring::whatwg
{
	class url_error_category : public std::error_category
	{
	public:
		char const* name() const noexcept override { return "url"; }

		std::string message(int ec) const override
		{
			switch (static_cast<url_error_name>(ec))
			{
			case url_error_name::IdnaTranslationFailed:
				return "idna translation failed";
			case url_error_name::ParseFailed:
				return "URL parse failed";
			case url_error_name::ValidationError:
				return "URL validation error";
			default:
				break;
			}
			return "";
		}
	};

	inline std::error_category const& url_category() noexcept
	{
		static url_error_category cat;
		return cat;
	}

	inline std::error_code make_error_code(url_error_name name)
	{
		return std::error_code{ static_cast<int>(name), url_category() };
	}
}
