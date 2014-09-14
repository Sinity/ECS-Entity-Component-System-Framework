#include "ecs/utils/formatString.h"
#include <boost/lexical_cast.hpp>

std::string parse(const char* cstringArgument) {
	return std::string(cstringArgument);
}

std::string parse(const std::string& string) {
	return string;
}

std::string parse(int intArgument) {
	return boost::lexical_cast<std::string>(intArgument);
}

std::string parse(long long int64) {
	return boost::lexical_cast<std::string>(int64);
}

std::string parse(unsigned int uint) {
	return boost::lexical_cast<std::string>(uint);
}

std::string parse(unsigned long long uint64) {
	return boost::lexical_cast<std::string>(uint64);
}

std::string parse(float floatArgument) {
	return boost::lexical_cast<std::string>(floatArgument);
}

std::string parse(double doubleArgument) {
	return boost::lexical_cast<std::string>(doubleArgument);
}

std::string parse(bool boolean) {
	return boolean ? "true" : "false";
}

std::string parse(char character) {
	return std::string("" + character);
}

std::string format() {
	return std::string("");
}
