#define BOOST_TEST_MODULE FormatString
#include <boost/test/unit_test.hpp>
#include "common/formatString.h"

BOOST_AUTO_TEST_CASE(EmptyReturnsEmpty) {
	std::string empty = format();
	BOOST_CHECK_EQUAL(empty, "");
}

BOOST_AUTO_TEST_CASE(CStringArgumentAloneReturnsHisValueItself) {
	std::string hello = format("Hello World!");
	BOOST_CHECK_EQUAL(hello, "Hello World!");
}

BOOST_AUTO_TEST_CASE(SeveralCStringArgumentsGetsGluedTogether) {
	std::string glued = format("Lorem", " ", "Ipsum", " ", "Dolor", " ", "Sit", " ", "Amet", ".");
	BOOST_CHECK_EQUAL(glued, "Lorem Ipsum Dolor Sit Amet.");
}

BOOST_AUTO_TEST_CASE(BoolProducesTrueOrFalseString) {
	std::string boolValues = format(true, false, true);
	BOOST_CHECK_EQUAL(boolValues, "truefalsetrue");
}

BOOST_AUTO_TEST_CASE(IntValueProducesStringsWithThese) {
	std::string answer = format(42);
	BOOST_CHECK_EQUAL(answer, "42");
}

BOOST_AUTO_TEST_CASE(NegativeIntsProducesStringWithNegativeSign) {
	std::string negative = format(-1);
	BOOST_CHECK_EQUAL(negative, "-1");
}

BOOST_AUTO_TEST_CASE(UIntProducesProperaluesWhereIntNot) {
	unsigned int largeValue = 4294967295u; //2^32 - 1
	std::string large = format(largeValue);
	BOOST_CHECK_EQUAL(large, "4294967295");
}

BOOST_AUTO_TEST_CASE(FloatBechavesNormal) {
	std::string pi = format(3.1415926);
	bool ok = pi[0] == '3' && pi[1] == '.' && pi[2] == '1' && pi[3] == '4';
	BOOST_ASSERT(ok);
}

BOOST_AUTO_TEST_CASE(CombinationOfManyTypesWorksAsExpected) {
	std::string example = format(42, " is ", true, " answer", ",", " not ", 0.0);
	BOOST_CHECK_EQUAL(example, "42 is true answer, not 0.000000");
}

