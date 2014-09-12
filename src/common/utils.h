#pragma once

#include <string>
#include <vector>

/** \brief spilts string to array of strings separated by delimiter.
*
* \param string whole string that will be splitted to chunks
* \param delimiter character that will split string in two parts, can be any value, will not be included in any part.
*
* In case of two delimiters touching, empty string willn't be included in result.
*/
std::vector<std::string> split(const std::string& string, char delimiter);
