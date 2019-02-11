#pragma once

// https://infra.spec.whatwg.org/
// https://triple-underscore.github.io/infra-ja.html

#include <cstdint>

namespace wordring::whatwg::infra
{

	class scalar_value
	{
	public:
		scalar_value() = default;

		scalar_value(scalar_value const & sv) : ch_{ sv.ch_ } { assert(!(0xD800 <= ch_ && ch_ <= 0xDFFF)); }

		explicit scalar_value(char32_t ch) : ch_{ (0xD800 <= ch && ch <= 0xDFFF) ? 0xFFFD : ch } { assert(!(0xD800 <= ch_ && ch_ <= 0xDFFF)); }

		~scalar_value() = default;

		// --------------------------------------------------------------------

		explicit operator char32_t() const
		{
			assert(!(0xD800 <= ch_ && ch_ <= 0xDFFF));
			return ch_;
		}

		// 算術演算子 ----------------------------------------------------------

		/*! 単純代入（Basic assignment）
		*/
		scalar_value & operator=(char32_t ch) &
		{
			ch_ = (0xD800 <= ch && ch <= 0xDFFF) ? 0xFFFD : ch;
			assert(!(0xD800 <= ch_ && ch_ <= 0xDFFF));
			return *this;
		}

		scalar_value & operator=(scalar_value const & sv) &
		{
			ch_ = sv.ch_;
			assert(!(0xD800 <= ch_ && ch_ <= 0xDFFF));
			return *this;
		}

		scalar_value & operator=(scalar_value && sv) & noexcept
		{
			ch_ = sv.ch_;
			assert(!(0xD800 <= ch_ && ch_ <= 0xDFFF));
			return *this;
		}

		/*! 単項プラス（Unary plus）
		*/
		scalar_value operator+() const { return scalar_value{ ch_ }; }

		/*! 単項マイナス（Unary minus）
		*/
		scalar_value operator-() const { return scalar_value{ static_cast<char32_t>(-static_cast<int32_t>(ch_)) }; }

		/*! 前置インクリメント（Prefix increment）
		*/
		scalar_value & operator++()
		{
			operator=(++ch_);
			return *this;
		}

		/*! 後置インクリメント（Postfix increment）
		*/
		scalar_value operator++(int)
		{
			scalar_value result{ ch_ };
			operator++();
			return result;
		}

		/*! 前置デクリメント（Prefix decrement）
		*/
		scalar_value & operator--()
		{
			operator=(--ch_);
			return *this;
		}

		/*! 後置デクリメント（Postfix decrement）
		*/
		scalar_value operator--(int)
		{
			scalar_value result{ ch_ };
			operator--();
			return result;
		}

		// 論理演算子 ----------------------------------------------------------

		/*!
		*/

		/*!
		*/

		/*!
		*/

		/*!
		*/

		// Compound assignment operators --------------------------------------

		/*! 加算代入（Addition assignment）
		*/
		scalar_value & operator+=(char32_t ch)
		{
			operator=(ch_ + ch);
			return  *this;
		}

		scalar_value & operator+=(scalar_value const & sv)
		{
			operator=(ch_ + sv.ch_);
			return  *this;
		}

		/*! 減算代入（Subtraction assignment）
		*/
		scalar_value & operator-=(char32_t ch)
		{
			operator=(ch_ - ch);
			return  *this;
		}

		scalar_value & operator-=(scalar_value const & sv)
		{
			operator=(ch_ - sv.ch_);
			return  *this;
		}

		/*! 乗算代入（Multiplication assignment）
		*/
		scalar_value & operator*=(char32_t ch)
		{
			operator=(ch_ * ch);
			return  *this;
		}

		scalar_value & operator*=(scalar_value const & sv)
		{
			operator=(ch_ * sv.ch_);
			return  *this;
		}

		/*! 除算代入（Division assignment）
		*/

		scalar_value & operator/=(char32_t ch)
		{
			operator=(ch_ / ch);
			return  *this;
		}

		scalar_value & operator/=(scalar_value const & sv)
		{
			operator=(ch_ / sv.ch_);
			return  *this;
		}

		/*! 剰余代入（Modulo assignment）
		*/
		scalar_value & operator%=(char32_t ch)
		{
			operator=(ch_ % ch);
			return  *this;
		}

		scalar_value & operator%=(scalar_value const & sv)
		{
			operator=(ch_ % sv.ch_);
			return  *this;
		}

		// その他の演算子 ------------------------------------------------------

	private:
		char32_t ch_;
	};

	// 比較演算子 ----------------------------------------------------------

	/*! 等価（Equal to）
	*/
	bool operator==(scalar_value const & sv1, scalar_value const & sv2) { return static_cast<char32_t>(sv1) == static_cast<char32_t>(sv2); }

	bool operator==(scalar_value const & sv, char32_t ch) { return static_cast<char32_t>(sv) == ch; }

	bool operator==(char32_t ch, scalar_value const & sv) { return ch == static_cast<char32_t>(sv); }

	/*! 非等価（Not equal to）
	*/
	bool operator!=(scalar_value const & sv1, scalar_value const & sv2) { return static_cast<char32_t>(sv1) != static_cast<char32_t>(sv2); }

	/*! 大なり（Greater than）
	*/
	bool operator>(scalar_value const & sv1, scalar_value const & sv2) { return static_cast<char32_t>(sv1) > static_cast<char32_t>(sv2); }

	/*! 小なり（Less than）
	*/
	bool operator<(scalar_value const & sv1, scalar_value const & sv2) { return static_cast<char32_t>(sv1) < static_cast<char32_t>(sv2); }

	/*! 大なりイコール（Greater than or equal to）
	*/
	bool operator>=(scalar_value const & sv1, scalar_value const & sv2) { return static_cast<char32_t>(sv1) >= static_cast<char32_t>(sv2); }

	/*! 小なりイコール（Less than or equal to）
	*/
	bool operator<=(scalar_value const & sv1, scalar_value const & sv2) { return static_cast<char32_t>(sv1) <= static_cast<char32_t>(sv2); }

	// 算術演算子 --------------------------------------------------------------

	/*! 加算（Addition）
	*/
	scalar_value operator+(scalar_value const & sv1, scalar_value const & sv2) { return scalar_value{ sv1 } += sv2; }

	scalar_value operator+(scalar_value const & sv, char32_t ch) { return scalar_value{ sv } += ch; }

	scalar_value operator+(char32_t ch, scalar_value const & sv) { return scalar_value{ ch } += sv; }

	/*! 減算（Subtraction）
	*/
	scalar_value operator-(scalar_value const & sv1, scalar_value const & sv2) { return scalar_value{ sv1 } -= sv2; }

	scalar_value operator-(scalar_value const & sv, char32_t ch) { return scalar_value{ sv } -= ch; }

	scalar_value operator-(char32_t ch, scalar_value const & sv) { return scalar_value{ ch } -= sv; }

	/*! 乗算（Multiplication）
	*/
	scalar_value operator*(scalar_value const & sv1, scalar_value const & sv2) { return scalar_value{ sv1 } *= sv2; }

	/*! 除算（Division）
	*/
	scalar_value operator/(scalar_value const & sv1, scalar_value const & sv2) { return scalar_value{ sv1 } /= sv2; }

	/*! 剰余（Modulo）
	*/
	scalar_value operator%(scalar_value const & sv1, scalar_value const & sv2) { return scalar_value{ sv1 } %= sv2; }

	// ------------------------------------------------------------------------

	using scalar_value_string = std::basic_string<scalar_value>;
}
