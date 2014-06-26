#include <string>
#include <vector>

//spilts string to strings separated by delimiter. In case of two delimiters touching, empty string willn't be included in result.
std::vector<std::string> split(const std::string& string, char delimiter);
