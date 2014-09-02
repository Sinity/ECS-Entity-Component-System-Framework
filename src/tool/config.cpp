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
	skipWhitechars();
	parseModule(&main);
}


void Configuration::set(const std::string& setting, const std::string& value) {
	std::vector<std::string> settingPath = split(setting, '.');
	if(settingPath.empty()) {
		return;
	}

	//find setting to set, create new nodes if these don't exist yet
	ConfigNode* currentNode = &main;
	for(unsigned int i = (settingPath[0] == "main" ? 1 : 0); i < settingPath.size() - 1; i++) {
		if(!currentNode->childs[settingPath[i]]) {
			currentNode->childs[settingPath[i]] = new ConfigNode;
		}
		currentNode = currentNode->childs[settingPath[i]];
	}

	currentNode->settings[settingPath.back()] = value;
}


unsigned int Configuration::parseModule(ConfigNode* thisModule) {
	while(true) {
		std::string token = parseString();
		if(token == "}" || token == "") {
			break;
		}

		skipWhitechars();
		bool isSetting = configurationSource[cursor] == '=';
		bool isModule = configurationSource[cursor] == '{';
		cursor++;
		skipWhitechars();

		if(isSetting) {
			thisModule->settings[token] = parseSettingValue();
			logger.info("Configuration: Loaded setting \"", token, "\" = \"", thisModule->settings[token], "\"");
		} else if(isModule) {
			logger.info("Configuration: Loading module \"", token, "\"");
			thisModule->childs[token] = new ConfigNode;
			cursor = parseModule(thisModule->childs[token]);
		} else {
			logger.error("Configuration: Expected `{` or `=`, given `", configurationSource[cursor - 1],
			             "`. Cursor position{orgin == 0}: ", cursor - 1);
		}

		skipWhitechars();
	}

	return cursor;
}


//expects that value starts immediately. If there will be any leading white chars, value will consist of it too.
std::string Configuration::parseSettingValue() {
	std::string setting;
	while(isprint(configurationSource[cursor])) {
		if(configurationSource[cursor] == '}') {
			break;
		}
		setting += configurationSource[cursor];
		cursor++;
	}

	//skip tail spaces
	unsigned int lastDesiredIndex = setting.length() - 1;
	while(isspace(setting[lastDesiredIndex--])) {}
	lastDesiredIndex++; //overshoot correction

	std::string strippedSetting;
	for(unsigned int i = 0; i <= lastDesiredIndex; i++) {
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
