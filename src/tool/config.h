#pragma once
#include <unordered_map>
#include <string>
#include <tool/logger.h>
#include <boost/lexical_cast.hpp>

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

    template<typename SettingType = std::string>
    SettingType get(const std::string& setting, SettingType defaultValue = SettingType()) {
        std::vector<std::string> splitted = split(setting, '.');

        ConfigNode* currentNode = &main;
        for (unsigned int i = (splitted[0] == "main" ? 1 : 0); i < splitted.size() - 1; i++) {
            currentNode = currentNode->childs[splitted[i]];
            if (!currentNode) {
                logger.warn("Configuration: Requested module \"%s\" doesn't exist. Whole setting: %s", splitted[i].c_str(), setting.c_str());
                return defaultValue;
            }
        }

        if (currentNode->settings.find(splitted.back()) == currentNode->settings.end()) {
            logger.warn("Configuration: All modules exist, but requested setting not. Requested setting: %s", setting.c_str());
            return defaultValue;
        }

        std::string result = currentNode->settings[splitted.back()];
        logger.info("Configuration: Setting %s = %s", setting.c_str(), result.c_str());
        return boost::lexical_cast<SettingType>(result);
    }

    std::string get(const std::string& setting, const char* defaultValue) {
        return get(setting, std::string(defaultValue));
    }

private:
    ConfigNode main;
    Logger logger;

    unsigned int parseModule(ConfigNode* module, char* config, unsigned int curr); 
    void parseInclude(char* config, unsigned int& curr, ConfigNode* module);
    std::string parseString(const char* startPos, unsigned int* position);
    char* loadEntireFile(const std::string& filename, unsigned int* filesize = nullptr);

    std::vector<std::string> split(const std::string& string, char delimiter);

	Configuration& operator=(Configuration& configuration) = delete;
};

