﻿
#include <boost/test/unit_test.hpp>

#include <wordring/whatwg/infra/scalar_value.hpp>

BOOST_AUTO_TEST_SUITE(whatwg_infra_scalar_value_test)

BOOST_AUTO_TEST_CASE(whatwg_infra_scalar_value__construct)
{
	wordring::whatwg::scalar_value sv0{};

	wordring::whatwg::scalar_value sv1{ U'a' };
	BOOST_CHECK(sv1 == U'a');

	wordring::whatwg::scalar_value sv2{ sv1 };
	BOOST_CHECK(sv2 == U'a');

	wordring::whatwg::scalar_value sv3{ 0xD800 };
	BOOST_CHECK(sv3 == 0xFFFD);
}

BOOST_AUTO_TEST_CASE(whatwg_infra_scalar_value__char32_t)
{
	BOOST_CHECK(static_cast<char32_t>(wordring::whatwg::scalar_value{ 0 }) == 0);
	BOOST_CHECK(static_cast<char32_t>(wordring::whatwg::scalar_value{ 0x1 }) == 0x1);
	BOOST_CHECK(static_cast<char32_t>(wordring::whatwg::scalar_value{ 0xD800 }) == 0xFFFD);
}

BOOST_AUTO_TEST_CASE(whatwg_infra_scalar_value__bool)
{
	BOOST_CHECK(static_cast<bool>(wordring::whatwg::scalar_value{ 0 }) == false);
	BOOST_CHECK(static_cast<bool>(wordring::whatwg::scalar_value{ 0x1 }));
	BOOST_CHECK(static_cast<bool>(wordring::whatwg::scalar_value{ 0xD800 }));
}

// Arithmetic operators -------------------------------------------------------

// a = b
BOOST_AUTO_TEST_CASE(whatwg_infra_scalar_value__basic_assignment)
{
	wordring::whatwg::scalar_value sv1{};
	wordring::whatwg::scalar_value sv2{ U'A' };

	sv1 = U'あ';
	BOOST_CHECK(sv1== U'あ');

	sv1 = 0xD800;
	BOOST_CHECK(sv1 == 0xFFFD);

	sv1 = sv2;
	BOOST_CHECK(sv1 == U'A');

	sv1 =wordring::whatwg::scalar_value{ U'B' };
	BOOST_CHECK(sv1 == U'B');
}

// a + b
BOOST_AUTO_TEST_CASE(whatwg_infra_scalar_value__addition)
{
	BOOST_CHECK(wordring::whatwg::scalar_value{ 0x1 } +wordring::whatwg::scalar_value{ 0x1 } == 0x2);
	BOOST_CHECK(wordring::whatwg::scalar_value{ 0xD7FF } +wordring::whatwg::scalar_value{ 0x1 } == 0xFFFD);
	BOOST_CHECK((wordring::whatwg::scalar_value{ 0x1 } +wordring::whatwg::scalar_value{ 0xD7FF }) == 0xFFFD);

	BOOST_CHECK(0x1 +wordring::whatwg::scalar_value{ 0x1 } == 0x2);
	BOOST_CHECK(0xD7FF +wordring::whatwg::scalar_value{ 0x1 } == 0xFFFD);
	BOOST_CHECK(0x1 +wordring::whatwg::scalar_value{ 0xD7FF } == 0xFFFD);

	BOOST_CHECK(wordring::whatwg::scalar_value{ 0x1 } + 0x1 == 0x2);
	BOOST_CHECK(wordring::whatwg::scalar_value{ 0xD7FF } + 0x1 == 0xFFFD);
	BOOST_CHECK(wordring::whatwg::scalar_value{ 0x1 } + 0xD7FF == 0xFFFD);
}

// a - b
BOOST_AUTO_TEST_CASE(whatwg_infra_scalar_value__subtraction)
{
	BOOST_CHECK(wordring::whatwg::scalar_value{ 0x2 } -wordring::whatwg::scalar_value{ 0x1 } == 0x1);
	BOOST_CHECK(wordring::whatwg::scalar_value{ 0xE000 } -wordring::whatwg::scalar_value{ 0x1 } == 0xFFFD);

	BOOST_CHECK(0x2 -wordring::whatwg::scalar_value{ 0x1 } == 0x1);
	BOOST_CHECK(0xE000 -wordring::whatwg::scalar_value{ 0x1 } == 0xFFFD);

	BOOST_CHECK(wordring::whatwg::scalar_value{ 0x2 } - 0x1 == 0x1);
	BOOST_CHECK(wordring::whatwg::scalar_value{ 0xE000 } - 0x1 == 0xFFFD);
}

// +a
BOOST_AUTO_TEST_CASE(whatwg_infra_scalar_value__unary_plus)
{
	BOOST_CHECK(+wordring::whatwg::scalar_value{ 0x1 } == 0x1);
	BOOST_CHECK(+wordring::whatwg::scalar_value{ 0xD800 } == 0xFFFD);
}

// -a
BOOST_AUTO_TEST_CASE(whatwg_infra_scalar_value__unary_minus)
{
	BOOST_CHECK(-wordring::whatwg::scalar_value{ 0x1 } == -0x1);
	BOOST_CHECK(-wordring::whatwg::scalar_value{ 0xFFFF2800 } == 0xFFFD);
}

// a * b
BOOST_AUTO_TEST_CASE(whatwg_infra_scalar_value__multiplication)
{
	BOOST_CHECK(wordring::whatwg::scalar_value{ 0x2 } *wordring::whatwg::scalar_value{ 0x2 } == 0x4);
	BOOST_CHECK(wordring::whatwg::scalar_value{ 0x6C00 } *wordring::whatwg::scalar_value{ 0x2 } == 0xFFFD);

	BOOST_CHECK(0x2 *wordring::whatwg::scalar_value{ 0x2 } == 0x4);
	BOOST_CHECK(0x6C00 *wordring::whatwg::scalar_value{ 0x2 } == 0xFFFD);

	BOOST_CHECK(wordring::whatwg::scalar_value{ 0x2 } * 0x2 == 0x4);
	BOOST_CHECK(wordring::whatwg::scalar_value{ 0x6C00 } * 0x2 == 0xFFFD);
}

// a / b
BOOST_AUTO_TEST_CASE(whatwg_infra_scalar_value__division)
{
	BOOST_CHECK(wordring::whatwg::scalar_value{ 0x4 } /wordring::whatwg::scalar_value{ 0x2 } == 0x2);
	BOOST_CHECK(wordring::whatwg::scalar_value{ 0x1BFFE } /wordring::whatwg::scalar_value{ 0x2 } == 0xFFFD);

	BOOST_CHECK(0x4 /wordring::whatwg::scalar_value{ 0x2 } == 0x2);
	BOOST_CHECK(0x1BFFE /wordring::whatwg::scalar_value{ 0x2 } == 0xFFFD);

	BOOST_CHECK(wordring::whatwg::scalar_value{ 0x4 } / 0x2 == 0x2);
	BOOST_CHECK(wordring::whatwg::scalar_value{ 0x1BFFE } / 0x2 == 0xFFFD);
}

// a % b
BOOST_AUTO_TEST_CASE(whatwg_infra_scalar_value__modulo)
{
	BOOST_CHECK(wordring::whatwg::scalar_value{ 0x5 } %wordring::whatwg::scalar_value{ 0x3 } == 0x2);
	BOOST_CHECK(wordring::whatwg::scalar_value{ 0x1BFFF } %wordring::whatwg::scalar_value{ 0xE000 } == 0xFFFD);

	BOOST_CHECK(0x5 %wordring::whatwg::scalar_value{ 0x3 } == 0x2);
	BOOST_CHECK(0x1BFFF %wordring::whatwg::scalar_value{ 0xE000 } == 0xFFFD);

	BOOST_CHECK(wordring::whatwg::scalar_value{ 0x5 } % 0x3 == 0x2);
	BOOST_CHECK(wordring::whatwg::scalar_value{ 0x1BFFF } % 0xE000 == 0xFFFD);
}

// ++a
BOOST_AUTO_TEST_CASE(whatwg_infra_scalar_value__prefix_increment)
{
	BOOST_CHECK(++wordring::whatwg::scalar_value{ 0x2 } == 0x3);
	BOOST_CHECK(++wordring::whatwg::scalar_value{ 0xD7FF } == 0xFFFD);
}

// a++
BOOST_AUTO_TEST_CASE(whatwg_infra_scalar_value__postfix_increment)
{
	wordring::whatwg::scalar_value sv1{ 0x2 };
	BOOST_CHECK(sv1++ == 0x2);
	BOOST_CHECK(sv1 == 0x3);

	wordring::whatwg::scalar_value sv2{ 0xD7FF };
	BOOST_CHECK(sv2++ == 0xD7FF);
	BOOST_CHECK(sv2 == 0xFFFD);
}

// --a
BOOST_AUTO_TEST_CASE(whatwg_infra_scalar_value__prefix_decrement)
{
	BOOST_CHECK(--wordring::whatwg::scalar_value{ 0x3 } == 0x2);
	BOOST_CHECK(--wordring::whatwg::scalar_value{ 0xE000 } == 0xFFFD);
}

// a--
BOOST_AUTO_TEST_CASE(whatwg_infra_scalar_value__postfix_decrement)
{
	wordring::whatwg::scalar_value sv1{ 0x3 };
	BOOST_CHECK(sv1-- == 0x3);
	BOOST_CHECK(sv1 == 0x2);

	wordring::whatwg::scalar_value sv2{ 0xE000 };
	BOOST_CHECK(sv2-- == 0xE000);
	BOOST_CHECK(sv2 == 0xFFFD);
}

// Comparison operators/relational operators ----------------------------------

// a == b
BOOST_AUTO_TEST_CASE(whatwg_infra_scalar_value__equal_to)
{
	BOOST_CHECK(wordring::whatwg::scalar_value{ 0x2 } ==wordring::whatwg::scalar_value{ 0x2 });
	BOOST_CHECK(wordring::whatwg::scalar_value{ 0xD800 } ==wordring::whatwg::scalar_value{ 0xFFFD });
	BOOST_CHECK(wordring::whatwg::scalar_value{ 0xD800 } ==wordring::whatwg::scalar_value{ 0xDFFF });
	BOOST_CHECK((wordring::whatwg::scalar_value{ 0x2 } ==wordring::whatwg::scalar_value{ 0x3 }) == false);

	BOOST_CHECK(wordring::whatwg::scalar_value{ 0x2 } == 0x2);
	BOOST_CHECK(wordring::whatwg::scalar_value{ 0xD800 } == 0xFFFD);
	BOOST_CHECK((wordring::whatwg::scalar_value{ 0x2 } == 0x3) == false);

	BOOST_CHECK(0x2 ==wordring::whatwg::scalar_value{ 0x2 });
	BOOST_CHECK(0xFFFD ==wordring::whatwg::scalar_value{ 0xD800 });
	BOOST_CHECK((0x2 ==wordring::whatwg::scalar_value{ 0x3 }) == false);
}

// a != b
BOOST_AUTO_TEST_CASE(whatwg_infra_scalar_value__not_equal_to)
{
	BOOST_CHECK((wordring::whatwg::scalar_value{ 0x2 } !=wordring::whatwg::scalar_value{ 0x2 }) == false);
	BOOST_CHECK((wordring::whatwg::scalar_value{ 0xD800 } !=wordring::whatwg::scalar_value{ 0xFFFD }) == false);
	BOOST_CHECK((wordring::whatwg::scalar_value{ 0xD800 } !=wordring::whatwg::scalar_value{ 0xDFFF }) == false);
	BOOST_CHECK(wordring::whatwg::scalar_value{ 0x2 } !=wordring::whatwg::scalar_value{ 0x3 });

	BOOST_CHECK((wordring::whatwg::scalar_value{ 0x2 } != 0x2) == false);
	BOOST_CHECK((wordring::whatwg::scalar_value{ 0xD800 } != 0xFFFD) == false);
	BOOST_CHECK(wordring::whatwg::scalar_value{ 0x2 } != 0x3);

	BOOST_CHECK((0x2 !=wordring::whatwg::scalar_value{ 0x2 }) == false);
	BOOST_CHECK((0xFFFD !=wordring::whatwg::scalar_value{ 0xD800 }) == false);
	BOOST_CHECK(0x2 !=wordring::whatwg::scalar_value{ 0x3 });
}

// a > b
BOOST_AUTO_TEST_CASE(whatwg_infra_scalar_value__greater_than)
{
	BOOST_CHECK(wordring::whatwg::scalar_value{ 0x3 } >wordring::whatwg::scalar_value{ 0x2 });
	BOOST_CHECK((wordring::whatwg::scalar_value{ 0x2 } >wordring::whatwg::scalar_value{ 0x2 }) == false);
	BOOST_CHECK((wordring::whatwg::scalar_value{ 0x1 } >wordring::whatwg::scalar_value{ 0x2 }) == false);

	BOOST_CHECK(wordring::whatwg::scalar_value{ 0x3 } > 0x2);
	BOOST_CHECK((wordring::whatwg::scalar_value{ 0x2 } > 0x2) == false);
	BOOST_CHECK((wordring::whatwg::scalar_value{ 0x1 } > 0x2) == false);

	BOOST_CHECK(0x3 >wordring::whatwg::scalar_value{ 0x2 });
	BOOST_CHECK((0x2 >wordring::whatwg::scalar_value{ 0x2 }) == false);
	BOOST_CHECK((0x1 >wordring::whatwg::scalar_value{ 0x2 }) == false);
}

// a < b
BOOST_AUTO_TEST_CASE(whatwg_infra_scalar_value__less_than)
{
	BOOST_CHECK(wordring::whatwg::scalar_value{ 0x2 } <wordring::whatwg::scalar_value{ 0x3 });
	BOOST_CHECK((wordring::whatwg::scalar_value{ 0x2 } <wordring::whatwg::scalar_value{ 0x2 }) == false);
	BOOST_CHECK((wordring::whatwg::scalar_value{ 0x2 } <wordring::whatwg::scalar_value{ 0x1 }) == false);

	BOOST_CHECK(wordring::whatwg::scalar_value{ 0x2 } < 0x3);
	BOOST_CHECK((wordring::whatwg::scalar_value{ 0x2 } < 0x2) == false);
	BOOST_CHECK((wordring::whatwg::scalar_value{ 0x2 } < 0x1) == false);

	BOOST_CHECK(0x2 <wordring::whatwg::scalar_value{ 0x3 });
	BOOST_CHECK((0x2 <wordring::whatwg::scalar_value{ 0x2 }) == false);
	BOOST_CHECK((0x2 <wordring::whatwg::scalar_value{ 0x1 }) == false);
}

// a >= b
BOOST_AUTO_TEST_CASE(whatwg_infra_scalar_value__greater_than_or_equal_to)
{
	BOOST_CHECK(wordring::whatwg::scalar_value{ 0x3 } >=wordring::whatwg::scalar_value{ 0x2 });
	BOOST_CHECK(wordring::whatwg::scalar_value{ 0x2 } >=wordring::whatwg::scalar_value{ 0x2 });
	BOOST_CHECK((wordring::whatwg::scalar_value{ 0x1 } >=wordring::whatwg::scalar_value{ 0x2 }) == false);

	BOOST_CHECK(wordring::whatwg::scalar_value{ 0x3 } >= 0x2);
	BOOST_CHECK(wordring::whatwg::scalar_value{ 0x2 } >= 0x2);
	BOOST_CHECK((wordring::whatwg::scalar_value{ 0x1 } >= 0x2) == false);

	BOOST_CHECK(0x3 >=wordring::whatwg::scalar_value{ 0x2 });
	BOOST_CHECK(0x2 >=wordring::whatwg::scalar_value{ 0x2 });
	BOOST_CHECK((0x1 >=wordring::whatwg::scalar_value{ 0x2 }) == false);
}

// a <= b
BOOST_AUTO_TEST_CASE(whatwg_infra_scalar_value__less_than_or_equal_to)
{
	BOOST_CHECK(wordring::whatwg::scalar_value{ 0x2 } <=wordring::whatwg::scalar_value{ 0x3 });
	BOOST_CHECK(wordring::whatwg::scalar_value{ 0x2 } <=wordring::whatwg::scalar_value{ 0x2 });
	BOOST_CHECK((wordring::whatwg::scalar_value{ 0x2 } <=wordring::whatwg::scalar_value{ 0x1 }) == false);

	BOOST_CHECK(wordring::whatwg::scalar_value{ 0x2 } <= 0x3);
	BOOST_CHECK(wordring::whatwg::scalar_value{ 0x2 } <= 0x2);
	BOOST_CHECK((wordring::whatwg::scalar_value{ 0x2 } <= 0x1) == false);

	BOOST_CHECK(0x2 <=wordring::whatwg::scalar_value{ 0x3 });
	BOOST_CHECK(0x2 <=wordring::whatwg::scalar_value{ 0x2 });
	BOOST_CHECK((0x2 <=wordring::whatwg::scalar_value{ 0x1 }) == false);
}

// Logical operators ----------------------------------------------------------

// !a
BOOST_AUTO_TEST_CASE(whatwg_infra_scalar_value__logical_not)
{
	BOOST_CHECK(!wordring::whatwg::scalar_value{ 0 });
	BOOST_CHECK((!wordring::whatwg::scalar_value{ 0x1 }) == false);
}

// a && b
BOOST_AUTO_TEST_CASE(whatwg_infra_scalar_value__logical_and)
{
	BOOST_CHECK(wordring::whatwg::scalar_value{ 0x1 } &&wordring::whatwg::scalar_value{ 0x1 });
	BOOST_CHECK((wordring::whatwg::scalar_value{ 0 } &&wordring::whatwg::scalar_value{ 0x1 }) == false);
	BOOST_CHECK((wordring::whatwg::scalar_value{ 0 } &&wordring::whatwg::scalar_value{ 0 }) == false);
}

// a || b
BOOST_AUTO_TEST_CASE(whatwg_infra_scalar_value__logical_or)
{
	BOOST_CHECK(wordring::whatwg::scalar_value{ 0x1 } ||wordring::whatwg::scalar_value{ 0x1 });
	BOOST_CHECK(wordring::whatwg::scalar_value{ 0 } ||wordring::whatwg::scalar_value{ 0x1 });
	BOOST_CHECK((wordring::whatwg::scalar_value{ 0 } ||wordring::whatwg::scalar_value{ 0 }) == false);
}

// Bitwise operators ----------------------------------------------------------

// ~a
BOOST_AUTO_TEST_CASE(whatwg_infra_scalar_value__bitwise_not)
{
	BOOST_CHECK((~wordring::whatwg::scalar_value{ 0 }) == 0xFFFFFFFF);
	BOOST_CHECK((~wordring::whatwg::scalar_value{ 0xFFFFFFFF }) == 0);
	BOOST_CHECK((~wordring::whatwg::scalar_value{ 0xD800 }) == 0xFFFF0002);
}

// a & b
BOOST_AUTO_TEST_CASE(whatwg_infra_scalar_value__bitwise_and)
{
	using namespace wordring::whatwg;
}

// a | b
BOOST_AUTO_TEST_CASE(whatwg_infra_scalar_value__bitwise_or)
{
	using namespace wordring::whatwg;
}

// a ^ b
BOOST_AUTO_TEST_CASE(whatwg_infra_scalar_value__bitwise_xor)
{
	using namespace wordring::whatwg;
}

// a << b
BOOST_AUTO_TEST_CASE(whatwg_infra_scalar_value__bitwise_left_shift)
{
	using namespace wordring::whatwg;
}

// a >> b
BOOST_AUTO_TEST_CASE(whatwg_infra_scalar_value__bitwise_right_shift)
{
	using namespace wordring::whatwg;
}

// Compound assignment operators ----------------------------------------------

// a += b
BOOST_AUTO_TEST_CASE(whatwg_infra_scalar_value__addition_assignment)
{
	using namespace wordring::whatwg;
}

// a -= b
BOOST_AUTO_TEST_CASE(whatwg_infra_scalar_value__subtraction_assignment)
{
	using namespace wordring::whatwg;
}

// a *= b
BOOST_AUTO_TEST_CASE(whatwg_infra_scalar_value__multiplication_assignment)
{
	using namespace wordring::whatwg;
}

// a /= b
BOOST_AUTO_TEST_CASE(whatwg_infra_scalar_value__division_assignment)
{
	using namespace wordring::whatwg;
}

// a %= b
BOOST_AUTO_TEST_CASE(whatwg_infra_scalar_value__modulo_assignment)
{
	using namespace wordring::whatwg;
}

// a &= b
BOOST_AUTO_TEST_CASE(whatwg_infra_scalar_value__bitwise_and_assignment)
{
	using namespace wordring::whatwg;
}

// a |= b
BOOST_AUTO_TEST_CASE(whatwg_infra_scalar_value__bitwise_or_assignment)
{
	using namespace wordring::whatwg;
}

// a ^= b
BOOST_AUTO_TEST_CASE(whatwg_infra_scalar_value__bitwise_xor_assignment)
{
	using namespace wordring::whatwg;
}

// a <<= b
BOOST_AUTO_TEST_CASE(whatwg_infra_scalar_value__bitwise_left_shift_assignment)
{
	using namespace wordring::whatwg;
}

// a >>= b
BOOST_AUTO_TEST_CASE(whatwg_infra_scalar_value__bitwise_right_shift_assignment)
{
	using namespace wordring::whatwg;
}

/*
BOOST_AUTO_TEST_CASE(whatwg_infra_scalar_value__)
{
}
*/

BOOST_AUTO_TEST_SUITE_END()

