#include "config.h"
#include <fstream>
#include "tool/logger.h"

bool Configuration::load(const std::string& filename) {
    std::string config = loadEntireFile(filename);
    if (config.empty()) {
        logger.error("Configuration: Cannot read config file.");
        return false;
    }

    loadFromMemory(std::move(config));
    return true;
}


void Configuration::loadFromMemory(std::string configuration) {
	data = std::move(configuration);
    
	removeComments();

    //find main module
    skipWhitechars();
    cursor += 4; //skip 'main'
    skipWhitechars();
    cursor++; //skip '{'

    //parse main module
    skipWhitechars();
    parseModule(&main);
}


unsigned int Configuration::parseModule(ConfigNode* thisModule) {
    while (true) {
        std::string id = parseString();

        if (id == "include") {
            parseInclude(thisModule);
            skipWhitechars();
            continue;
        } else if (id == "}") {
			break;
		}

        skipWhitechars();
        bool isSetting = data[cursor] == '=';
		bool isModule = data[cursor] == '{';
        cursor++;
		skipWhitechars();

        if (isSetting) {
            thisModule->settings[id] = parseSetting();
            logger.info("Configuration: Loaded setting \"", id, "\" = \"", thisModule->settings[id], "\"");
        } else if (isModule) {
			logger.info("Configuration: Loading module \"", id, "\"");
            thisModule->childs[id] = new ConfigNode;
            cursor = parseModule(thisModule->childs[id]);
        } else {
            logger.error("Configuration: Expected `{` or `=`, given `", data[cursor - 1], "`. Cursor position: ", cursor - 1);
        }

        skipWhitechars();
    }

    return cursor;
}

void Configuration::parseInclude(ConfigNode* module) {
    skipWhitechars();

    std::string filename = parseFilename();
    std::string includedFile = loadEntireFile(filename);
	skipWhitechars();
    if (includedFile.empty()) {
        logger.error("Configuration: Cannot load file \"", filename, "\" that is included in configuration file"); 
        return;
    }

	logger.fatal("Configuration: parseInclude: NOT IMPLEMENTED YET!");
 //   unsigned int positionInIncluded = 0;
 //   removeComments(includedFile);
 //   skipWhitechars(includedFile.c_str(), &positionInIncluded);

 //   std::string moduleID = parseString(includedFile.c_str(), &positionInIncluded);
 //   skipWhitechars(includedFile.c_str(), &positionInIncluded);
 //   positionInIncluded++;
 //   skipWhitechars(includedFile.c_str(), &positionInIncluded);

 //   logger.info("Configuration: Loading included module \"", moduleID, "\" from file \"", filename, "\"");
 //   module->childs[moduleID] = new ConfigNode;
	//this->cursor = positionInIncluded;
 //   parseModule(module->childs[moduleID], (char*)includedFile.c_str());
}


//willn't skip leading spaces!
std::string Configuration::parseSetting() {
    std::string setting;
    while (isprint(data[cursor])) {
		if (data[cursor] == '}') {
			break;
		}
		setting += data[cursor];
        cursor++;
    }

    //skip tail spaces
    unsigned int last = setting.length() - 1;
    while (isspace(setting[last--]));

    std::string strippedSetting;
    for (unsigned int i = 0; i <= last + 1; i++)
        strippedSetting += setting[i];

    return strippedSetting;
}


std::string Configuration::parseString() {
    std::string result;
	for (; cursor < data.size() && isgraph(data[cursor]); cursor++) {
		if (data[cursor] == '.') {
			logger.error("Configuration: Illegal character \'.\' in identificator.");
			return result;
		}
		result += data[cursor];
    }
	return result;
}


std::string Configuration::parseFilename() {
    std::string str;
    while (isgraph(data[cursor])) {
        str += data[cursor];
        cursor++;
    }
    return str;
}


//spilts string to strings separated by delimiter. In case of two delimiters touching, empty string willn't be included in result.
std::vector<std::string> Configuration::split(const std::string& string, char delimiter) {
    std::vector<std::string> splitted;

    std::stringstream stream(string);
    std::string current;
    while (std::getline(stream, current, delimiter))
        if (!current.empty())
            splitted.emplace_back(current);
    return splitted;
}


void Configuration::removeComments() {
	if (data.empty()) {
		return;
	}

    for (unsigned int i = 0; i < data.size() - 1; i++) {
		if (data[i] == '-' && data[i + 1] == '-') {
			for (; data[i] != '\n' && i < data.size(); i++) {
				data[i] = ' ';
			}
		}
    }
}

void Configuration::skipWhitechars() {
	while (isspace(data[cursor])) {
		cursor++;
	}
}


std::string Configuration::loadEntireFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);
    if (!file.is_open()) {
        logger.error("Configuration: Cannot open file ", filename);
        return "";
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    char* buffer = new char[(unsigned int)size];
    file.read(buffer, size);
    if (!file) {
        logger.error("Configuration: Cannot read content of config file ", filename);
        delete[] buffer;
        return "";
    }

	//temporal 'solution'
	std::string result = buffer;
	delete[] buffer;

    return result;
}

