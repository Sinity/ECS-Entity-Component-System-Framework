#pragma once

#include <string>

std::string parse(const char* cstring);
std::string parse(const std::string& string);
std::string parse(int intArgument);
std::string parse(long long int64);
std::string parse(unsigned int uint);
std::string parse(unsigned long long uint64);
std::string parse(float floatArgument);
std::string parse(double doubleArgument);
std::string parse(bool boolean);
std::string parse(char character);
std::string format();

#define PEXPR(expr) #expr , "=", expr

template<typename Head, typename... Tail>
std::string format(Head head, Tail... tail) {
	return parse(head) + format(tail...);
}

template<typename Last>
std::string format(Last last) {
	return parse(last);
}
