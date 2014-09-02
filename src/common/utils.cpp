#include "utils.h"
#include <sstream>

std::vector<std::string> split(const std::string& string, char delimiter) {
	std::vector<std::string> splitted;

	std::stringstream stream(string);
	std::string current;
	while(std::getline(stream, current, delimiter)) {
		if(!current.empty()) {
			splitted.emplace_back(current);
		}
	}
	return splitted;
}
