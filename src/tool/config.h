#pragma once
#include <unordered_map>
#include <string>
#include <boost/lexical_cast.hpp>
#include "common/utils.h"
#include "tool/logger.h"

struct ConfigNode {
    std::unordered_map<std::string, std::string> settings;
    std::unordered_map<std::string, ConfigNode*> childs;

    ~ConfigNode() {
        for (auto& child : childs)
            delete child.second;
    }
};

class Configuration {
public:
    void loadFromMemory(std::string configuration);
    bool load(const std::string& filename);
	void set(const std::string& setting, const std::string& value);

    template<typename SettingType = std::string>
    SettingType get(const std::string& setting, SettingType defaultValue = SettingType()) {
        std::vector<std::string> splitted = split(setting, '.');
		if (splitted.empty()) {
			return defaultValue;
		}

        ConfigNode* currentNode = &main;
        for (unsigned int i = (splitted[0] == "main" ? 1 : 0); i < splitted.size() - 1; i++) {
            currentNode = currentNode->childs[splitted[i]];
            if (!currentNode) {
                logger.warn("Configuration: Requested module \"", splitted[i], "\" doesn't exist. Whole setting: ", setting);
                return defaultValue;
            }
        }

        if (currentNode->settings.find(splitted.back()) == currentNode->settings.end()) {
            logger.warn("Configuration: All modules exist, but requested setting not. Requested setting: ", setting);
            return defaultValue;
        }

        std::string result = currentNode->settings[splitted.back()];
        return boost::lexical_cast<SettingType>(result);
    }

    std::string get(const std::string& setting, const char* defaultValue) {
        return get(setting, std::string(defaultValue));
    }

	ConfigNode* getNode(const std::string& nodePath) {
		std::vector<std::string> splitted = split(nodePath, '.');
		if (splitted.empty()) {
			return &main;
		}

		ConfigNode* currentNode = &main;
		for (unsigned int i = (splitted[0] == "main" ? 1 : 0); i < splitted.size(); i++) {
			currentNode = currentNode->childs[splitted[i]];
			if (!currentNode) {
				logger.warn("Configuration: getNode: Requested module \"", splitted[i], "\" doesn't exist. Whole path: ", nodePath);
				return nullptr;
			}
		}
		return currentNode;
	}

	template<typename ValType>
	void set(const std::string& setting, ValType value) {
		set(setting, std::to_string(value));
	}

public:
	Logger logger;

private:
    ConfigNode main;
	std::string data;
	unsigned int cursor = 0;

    unsigned int parseModule(ConfigNode* module); 
    void parseInclude(ConfigNode* module);
	std::string parseSetting();

	std::string parseFilename();
	std::string parseString();
	void skipWhitechars();
	void removeComments();
	std::string loadEntireFile(const std::string& filename);
};

