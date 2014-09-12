#pragma once

#include <unordered_map>
#include <boost/lexical_cast.hpp>
#include "common/utils.h"
#include "tool/logger.h"

/** \brief configuration module, consisting of all child modules and settings at this module.*/
struct ConfigNode {
	std::unordered_map<std::string, std::string> settings;
	std::unordered_map<std::string, ConfigNode*> childs;

	~ConfigNode() {
		for(auto& child : childs) {
			delete child.second;
		}
	}
};

/** \brief class for reading and managing program's configuration.
*
* Sample configuration file:
* Graphics {
*   Resolution {
*       x = 1920
*       y = 1080
*   }
*   fullscreen = true
*   windowName = Some Application Window
* }
* Physics {
*   updatesPerSecond = 100
* }
*
* Sample call to retrieve information:
* configuration.get("Graphics.Resolution.x", 800);
* configuration.get<std::string>("Graphics.windowName", "Unknown Window");
*
* Dots and whitespaces aren't allowed inside modules(Graphics/Resolution etc. in previous example). Setting value
* is everything beyond equality sign to the end of line, except preceding and following whitespaces.
*
* Configuration can be loaded from file or memory(std::string).
* Class allows for setting particular settings and getting settings values.
*
* Value types are specified by get caller. For example, config.get("path.to.some.setting", 0u), will return
* unsigned because defult argument provided by caller is unsigned. When type can't be interpreted, caller can
* specify it explicte by passing type in template parameter: config.get<unsigned int>("path.to.some.setting").
*
* Class allows also to get ConfigurationNode. It will have all settings and nested levels from given setting path.
*/
class Configuration {
public:
	void loadFromMemory(std::string configuration);
	bool load(const std::string& filename);
	void set(const std::string& setting, const std::string& value);

	/** \brief get particular setting's value
	*
	* \param setting path to setting's value consisting of all parent modules and setting's name.
	* \param defaultValue will be returned in case that real setting's vlaue can't be. Also used for type deduction.
	*
	* Routine will try to identify setting's value by supplied default value. In case that this isn't what you
	* need, or it can't, you can specify type explictly, in template parameter.
	*
	* By default, default value will be default value of setting type(in this case, specified explictly in template)
	* For basic types like ints or pointers, it will be 0. For user-defined types, it will be defualt constructor.
	*
	* setting path is all modules all the way to setting in interest, separated by dots.
	*/
	template<typename SettingType = std::string>
	SettingType get(const std::string& setting, SettingType defaultValue = SettingType()) {
		std::vector<std::string> splitted = split(setting, '.');
		if(splitted.empty()) {
			return defaultValue;
		}

		ConfigNode* currentNode = &main;
		for(unsigned int i = (splitted[0] == "main" ? 1 : 0); i < splitted.size() - 1; i++) {
			currentNode = currentNode->childs[splitted[i]];
			if(!currentNode) {
				logger.warn("Configuration: Requested module \"", splitted[i],
				            "\" doesn't exist. Whole setting: ", setting);
				return defaultValue;
			}
		}

		if(currentNode->settings.find(splitted.back()) == currentNode->settings.end()) {
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
		if(splitted.empty()) {
			return &main;
		}

		ConfigNode* currentNode = &main;
		for(unsigned int i = (splitted[0] == "main" ? 1 : 0); i < splitted.size(); i++) {
			currentNode = currentNode->childs[splitted[i]];
			if(!currentNode) {
				logger.warn("Configuration: getNode: Requested module \"", splitted[i], "\" doesn't exist. Whole path: ", nodePath);
				return nullptr;
			}
		}
		return currentNode;
	}

	template<typename ValType>
	void set(const std::string& setting, ValType value) {
		set(setting, boost::lexical_cast<std::string>(value));
	}

public:
	Logger logger{"Config"};

private:
	ConfigNode main;
	std::string configurationSource;
	unsigned int cursor = 0;

	unsigned int parseModule(ConfigNode* module);
	std::string parseSettingValue();
	std::string parseString();
	void skipWhitechars();
	void removeComments();
	std::string loadEntireFile(const std::string& filename);
};

