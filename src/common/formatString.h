#include <string>

std::string parse(const char* cstring);
std::string parse(int intArgument);
std::string parse(unsigned int uint);
std::string parse(float floatArgument);
std::string parse(double doubleArgument);
std::string parse(bool boolean);
std::string parse(char character);
std::string format();

template<typename Head, typename... Tail>
std::string format(Head head, Tail... tail) {
	return parse(head) + format(tail...);
}

template<typename Last>
std::string format(Last last) {
	return parse(last);
}
