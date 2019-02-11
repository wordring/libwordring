
#include <boost/test/unit_test.hpp>

#include <wordring/whatwg/infra/scalar_value.hpp>

BOOST_AUTO_TEST_SUITE(whatwg_infra_scalar_value_test)

BOOST_AUTO_TEST_CASE(whatwg_infra_scalar_value__construct)
{
	wordring::whatwg::infra::scalar_value sv0{};

	wordring::whatwg::infra::scalar_value sv1{ U'a' };
	BOOST_CHECK(sv1 == U'a');

	wordring::whatwg::infra::scalar_value sv2{ sv1 };
	BOOST_CHECK(sv2 == U'a');

	wordring::whatwg::infra::scalar_value sv3{ 0xD800 };
	BOOST_CHECK(sv3 == 0xFFFD);
}

// Arithmetic operators -------------------------------------------------------

// a = b
BOOST_AUTO_TEST_CASE(whatwg_infra_scalar_value__basic_assignment)
{
	wordring::whatwg::infra::scalar_value sv1{};
	wordring::whatwg::infra::scalar_value sv2{ U'A' };

	sv1 = U'あ';
	BOOST_CHECK(sv1== U'あ');

	sv1 = 0xD800;
	BOOST_CHECK(sv1 == 0xFFFD);

	sv1 = sv2;
	BOOST_CHECK(sv1 == U'A');

	sv1 = wordring::whatwg::infra::scalar_value{ U'B' };
	BOOST_CHECK(sv1 == U'B');
}

// a + b
BOOST_AUTO_TEST_CASE(whatwg_infra_scalar_value__addition)
{
	BOOST_CHECK(wordring::whatwg::infra::scalar_value{ 0x1 } + wordring::whatwg::infra::scalar_value{ 0x1 } == 0x2);
	BOOST_CHECK(wordring::whatwg::infra::scalar_value{ 0xD7FF } + wordring::whatwg::infra::scalar_value{ 0x1 } == 0xFFFD);
	BOOST_CHECK((wordring::whatwg::infra::scalar_value{ 0x1 } + wordring::whatwg::infra::scalar_value{ 0xD7FF }) == 0xFFFD);

	BOOST_CHECK(0x1 + wordring::whatwg::infra::scalar_value{ 0x1 } == 0x2);
	BOOST_CHECK(0xD7FF + wordring::whatwg::infra::scalar_value{ 0x1 } == 0xFFFD);
	BOOST_CHECK(0x1 + wordring::whatwg::infra::scalar_value{ 0xD7FF } == 0xFFFD);

	BOOST_CHECK(wordring::whatwg::infra::scalar_value{ 0x1 } + 0x1 == 0x2);
	BOOST_CHECK(wordring::whatwg::infra::scalar_value{ 0xD7FF } + 0x1 == 0xFFFD);
	BOOST_CHECK(wordring::whatwg::infra::scalar_value{ 0x1 } + 0xD7FF == 0xFFFD);
}

// a - b
BOOST_AUTO_TEST_CASE(whatwg_infra_scalar_value__subtraction)
{
	BOOST_CHECK(wordring::whatwg::infra::scalar_value{ 0x2 } - wordring::whatwg::infra::scalar_value{ 0x1 } == 0x1);
	BOOST_CHECK(wordring::whatwg::infra::scalar_value{ 0xE000 } - wordring::whatwg::infra::scalar_value{ 0x1 } == 0xFFFD);

	BOOST_CHECK(0x2 - wordring::whatwg::infra::scalar_value{ 0x1 } == 0x1);
	BOOST_CHECK(0xE000 - wordring::whatwg::infra::scalar_value{ 0x1 } == 0xFFFD);

	BOOST_CHECK(wordring::whatwg::infra::scalar_value{ 0x2 } - 0x1 == 0x1);
	BOOST_CHECK(wordring::whatwg::infra::scalar_value{ 0xE000 } -0x1 == 0xFFFD);
}

// +a
BOOST_AUTO_TEST_CASE(whatwg_infra_scalar_value__unary_plus)
{
	BOOST_CHECK(+wordring::whatwg::infra::scalar_value{ 0x1 } == 0x1);
	BOOST_CHECK(+wordring::whatwg::infra::scalar_value{ 0xD800 } == 0xFFFD);
}

// -a
BOOST_AUTO_TEST_CASE(whatwg_infra_scalar_value__unary_minus)
{
	BOOST_CHECK(-wordring::whatwg::infra::scalar_value{ 0x1 } == -0x1);
}

// a * b
BOOST_AUTO_TEST_CASE(whatwg_infra_scalar_value__multiplication)
{
	using namespace wordring::whatwg::infra;
}

// a / b
BOOST_AUTO_TEST_CASE(whatwg_infra_scalar_value__division)
{
	using namespace wordring::whatwg::infra;
}

// a % b
BOOST_AUTO_TEST_CASE(whatwg_infra_scalar_value__modulo)
{
	using namespace wordring::whatwg::infra;
}

// ++a
BOOST_AUTO_TEST_CASE(whatwg_infra_scalar_value__prefix_increment)
{
	using namespace wordring::whatwg::infra;
}

// a++
BOOST_AUTO_TEST_CASE(whatwg_infra_scalar_value__postfix_increment)
{
	using namespace wordring::whatwg::infra;
}

// --a
BOOST_AUTO_TEST_CASE(whatwg_infra_scalar_value__prefix_decrement)
{
	using namespace wordring::whatwg::infra;
}

// a--
BOOST_AUTO_TEST_CASE(whatwg_infra_scalar_value__postfix_decrement)
{
	using namespace wordring::whatwg::infra;
}

// Comparison operators/relational operators ----------------------------------

// a == b
BOOST_AUTO_TEST_CASE(whatwg_infra_scalar_value__equal_to)
{
	using namespace wordring::whatwg::infra;
}

// a != b
BOOST_AUTO_TEST_CASE(whatwg_infra_scalar_value__not_equal_to)
{
	using namespace wordring::whatwg::infra;
}

// a > b
BOOST_AUTO_TEST_CASE(whatwg_infra_scalar_value__greater_than)
{
	using namespace wordring::whatwg::infra;
}

// a < b
BOOST_AUTO_TEST_CASE(whatwg_infra_scalar_value__less_than)
{
	using namespace wordring::whatwg::infra;
}

// a >= b
BOOST_AUTO_TEST_CASE(whatwg_infra_scalar_value__greater_than_or_equal_to)
{
	using namespace wordring::whatwg::infra;
}

// a <= b
BOOST_AUTO_TEST_CASE(whatwg_infra_scalar_value__less_than_or_equal_to)
{
	using namespace wordring::whatwg::infra;
}

// Logical operators ----------------------------------------------------------

// !a
BOOST_AUTO_TEST_CASE(whatwg_infra_scalar_value__logical_not)
{
	using namespace wordring::whatwg::infra;
}

// a && b
BOOST_AUTO_TEST_CASE(whatwg_infra_scalar_value__logical_and)
{
	using namespace wordring::whatwg::infra;
}

// a || b
BOOST_AUTO_TEST_CASE(whatwg_infra_scalar_value__logical_or)
{
	using namespace wordring::whatwg::infra;
}

// Bitwise operators ----------------------------------------------------------

// ~a
BOOST_AUTO_TEST_CASE(whatwg_infra_scalar_value__bitwise_not)
{
	using namespace wordring::whatwg::infra;
}

// a & b
BOOST_AUTO_TEST_CASE(whatwg_infra_scalar_value__bitwise_and)
{
	using namespace wordring::whatwg::infra;
}

// a | b
BOOST_AUTO_TEST_CASE(whatwg_infra_scalar_value__bitwise_or)
{
	using namespace wordring::whatwg::infra;
}

// a ^ b
BOOST_AUTO_TEST_CASE(whatwg_infra_scalar_value__bitwise_xor)
{
	using namespace wordring::whatwg::infra;
}

// a << b
BOOST_AUTO_TEST_CASE(whatwg_infra_scalar_value__bitwise_left_shift)
{
	using namespace wordring::whatwg::infra;
}

// a >> b
BOOST_AUTO_TEST_CASE(whatwg_infra_scalar_value__bitwise_right_shift)
{
	using namespace wordring::whatwg::infra;
}

// Compound assignment operators ----------------------------------------------

// a += b
BOOST_AUTO_TEST_CASE(whatwg_infra_scalar_value__addition_assignment)
{
	using namespace wordring::whatwg::infra;
}

// a -= b
BOOST_AUTO_TEST_CASE(whatwg_infra_scalar_value__subtraction_assignment)
{
	using namespace wordring::whatwg::infra;
}

// a *= b
BOOST_AUTO_TEST_CASE(whatwg_infra_scalar_value__multiplication_assignment)
{
	using namespace wordring::whatwg::infra;
}

// a /= b
BOOST_AUTO_TEST_CASE(whatwg_infra_scalar_value__division_assignment)
{
	using namespace wordring::whatwg::infra;
}

// a %= b
BOOST_AUTO_TEST_CASE(whatwg_infra_scalar_value__modulo_assignment)
{
	using namespace wordring::whatwg::infra;
}

// a &= b
BOOST_AUTO_TEST_CASE(whatwg_infra_scalar_value__bitwise_and_assignment)
{
	using namespace wordring::whatwg::infra;
}

// a |= b
BOOST_AUTO_TEST_CASE(whatwg_infra_scalar_value__bitwise_or_assignment)
{
	using namespace wordring::whatwg::infra;
}

// a ^= b
BOOST_AUTO_TEST_CASE(whatwg_infra_scalar_value__bitwise_xor_assignment)
{
	using namespace wordring::whatwg::infra;
}

// a <<= b
BOOST_AUTO_TEST_CASE(whatwg_infra_scalar_value__bitwise_left_shift_assignment)
{
	using namespace wordring::whatwg::infra;
}

// a >>= b
BOOST_AUTO_TEST_CASE(whatwg_infra_scalar_value__bitwise_right_shift_assignment)
{
	using namespace wordring::whatwg::infra;
}

/*
BOOST_AUTO_TEST_CASE(whatwg_infra_scalar_value__)
{
}
*/

BOOST_AUTO_TEST_SUITE_END()

