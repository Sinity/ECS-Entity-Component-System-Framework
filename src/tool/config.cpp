#include "config.h"
#include <fstream>

bool Configuration::load(const std::string& filename) {
	std::string config = loadEntireFile(filename);
	if(config.empty()) {
		logger.error("Configuration: Cannot read config file.");
		return false;
	}

	loadFromMemory(std::move(config));
	return true;
}


void Configuration::loadFromMemory(std::string configuration) {
	configurationSource = std::move(configuration);

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


void Configuration::set(const std::string& setting, const std::string& value) {
	std::vector<std::string> splitted = split(setting, '.');
	if(splitted.empty()) {
		return;
	}

	//find setting to set, create new nodes if these don't exist yet
	ConfigNode* currentNode = &main;
	for(unsigned int i = (splitted[0] == "main" ? 1 : 0); i < splitted.size() - 1; i++) {
		ConfigNode* oldNode = currentNode;
		currentNode = currentNode->childs[splitted[i]];
		if(!currentNode) {
			currentNode = oldNode;
			currentNode->childs[splitted[i]] = new ConfigNode;
			currentNode = currentNode->childs[splitted[i]];
		}
	}

	currentNode->settings[splitted.back()] = value;
}


unsigned int Configuration::parseModule(ConfigNode* thisModule) {
	while(true) {
		std::string id = parseString();

		if(id == "include") {
			parseInclude(thisModule);
			skipWhitechars();
			continue;
		} else if(id == "}") {
			break;
		}

		skipWhitechars();
		bool isSetting = configurationSource[cursor] == '=';
		bool isModule = configurationSource[cursor] == '{';
		cursor++;
		skipWhitechars();

		if(isSetting) {
			thisModule->settings[id] = parseSetting();
			logger.info("Configuration: Loaded setting \"", id, "\" = \"", thisModule->settings[id], "\"");
		} else if(isModule) {
			logger.info("Configuration: Loading module \"", id, "\"");
			thisModule->childs[id] = new ConfigNode;
			cursor = parseModule(thisModule->childs[id]);
		} else {
			logger.error("Configuration: Expected `{` or `=`, given `", configurationSource[cursor - 1], "`. Cursor position: ", cursor - 1);
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
	if(includedFile.empty()) {
		logger.error("Configuration: Cannot load file \"", filename, "\" that is included in configuration file");
		return;
	}

	logger.fatal("Configuration: parseInclude: NOT IMPLEMENTED YET!");
	assert(!"parseInclude: NOT IMPLEMENTED!");
	(void)module;
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
	while(isprint(configurationSource[cursor])) {
		if(configurationSource[cursor] == '}') {
			break;
		}
		setting += configurationSource[cursor];
		cursor++;
	}

	//skip tail spaces
	unsigned int last = setting.length() - 1;
	while(isspace(setting[last--])) {}

	std::string strippedSetting;
	for(unsigned int i = 0; i <= last + 1; i++) {
		strippedSetting += setting[i];
	}

	return strippedSetting;
}


std::string Configuration::parseString() {
	std::string result;
	for(; cursor < configurationSource.size() && isgraph(configurationSource[cursor]); cursor++) {
		if(configurationSource[cursor] == '.') {
			logger.error("Configuration: Illegal character \'.\' in identificator.");
			return result;
		}
		result += configurationSource[cursor];
	}
	return result;
}


std::string Configuration::parseFilename() {
	std::string str;
	while(isgraph(configurationSource[cursor])) {
		str += configurationSource[cursor];
		cursor++;
	}
	return str;
}


void Configuration::removeComments() {
	if(configurationSource.empty()) {
		return;
	}

	for(unsigned int i = 0; i < configurationSource.size() - 1; i++) {
		if(configurationSource[i] == '-' && configurationSource[i + 1] == '-') {
			for(; configurationSource[i] != '\n' && i < configurationSource.size(); i++) {
				configurationSource[i] = ' ';
			}
		}
	}
}

void Configuration::skipWhitechars() {
	while(isspace(configurationSource[cursor])) {
		cursor++;
	}
}


std::string Configuration::loadEntireFile(const std::string& filename) {
	std::ifstream file(filename, std::ios::ate | std::ios::binary);
	if(!file.is_open()) {
		logger.error("Configuration: Cannot open file ", filename);
		return "";
	}

	long long int size = file.tellg();
	file.seekg(0, std::ios::beg);

	char* buffer = new char[(unsigned int)size + 1];
	file.read(buffer, size);
	buffer[(unsigned int)size] = '\0';
	if(!file) {
		logger.error("Configuration: Cannot read content of config file ", filename);
		delete[] buffer;
		return "";
	}

	//temporal 'solution'
	std::string result = buffer;
	delete[] buffer;

	return result;
}



