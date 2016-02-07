#pragma once

#include <string>

std::string parse(const char* cstring);
std::string parse(const std::string& string);
std::string parse(std::string&& string);
std::string parse(int intArgument);
std::string parse(long long int64);
std::string parse(unsigned int uint);
std::string parse(unsigned long long uint64);
std::string parse(float floatArgument);
std::string parse(double doubleArgument);
std::string parse(bool boolean);
std::string parse(char character);
std::string format();

/** \brief produces sequence of format arguments that prints given expression name and it's value.
*
* \param expr experience to print
* Example:
* int i = 42;
* format(PEXPR(i)) == "i=42"
*
* It evaluates expression once.
*/
#define PEXPR(expr) #expr, "=", expr

/** \brief produce string from multiple strings and other types
*
* Usage is simple:
* format("Answer", " ", "= ", 42, ".") == "Answer = 42."
*
* It supports all common types. For bools, it will print literals "true" and "false", not "1" and "0".
*/
template <typename Head, typename... Tail>
std::string format(Head&& head, Tail&&... tail) {
    return parse(std::forward<Head>(head)) + format(std::forward<Tail>(tail)...);
}
