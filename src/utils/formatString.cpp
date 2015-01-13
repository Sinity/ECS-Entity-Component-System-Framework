#include "ecs/utils/formatString.h"

std::string parse(const char* cstringArgument) {
	return std::string(cstringArgument);
}

std::string parse(const std::string& string) {
	return string;
}

std::string parse(int intArgument) {
	return std::to_string(intArgument);
}

std::string parse(long long int64) {
	return std::to_string(int64);
}

std::string parse(unsigned int uint) {
	return std::to_string(uint);
}

std::string parse(unsigned long long uint64) {
	return std::to_string(uint64);
}

std::string parse(float floatArgument) {
	return std::to_string(floatArgument);
}

std::string parse(double doubleArgument) {
	return std::to_string(doubleArgument);
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
