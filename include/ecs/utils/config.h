#pragma once
#include <unordered_map>
#include <boost/lexical_cast.hpp>
#include "src/utils/stringUtils.h"
#include "logger.h"

/** \brief configuration module, consisting of all child modules and settings at this module.*/
struct ConfigNode {
	std::unordered_map<std::string, std::string> settings;
	std::unordered_map<std::string, ConfigNode*> childs;

	~ConfigNode() {
		clear();
	}

	void clear() {
		for(auto& child : childs) {
			delete child.second;
		}

		settings.clear();
		childs.clear();
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
	/** \brief loads configuration from memory(string)
	*
	*  \param configuration string with configuration to load
	*
	*  It deletes old configuration.
	*/
	void loadFromMemory(std::string configuration);

	/** \brief loads configuration form file on disc
	*
	* \param filename name of file with configuration to read
	*
	* It deletes old configuration.
	*
	* \returns true if file was opened successfully, false otherwise
	*/
	bool load(const std::string& filename);

	/** \brief get particular setting's value
	*
	* \param settingPath path to setting's value consisting of all parent modules and setting's name.
	* \param defaultValue will be returned in case that real setting's value can't be. Also used for type deduction.
	*
	* Routine will try to identify setting's value by supplied default value. In case that this isn't what you
	* need, or it can't, you can specify type explictly, in template parameter.
	*
	* By default, default value will be default value of setting type(in this case, specified explictly in template)
	* For basic types like ints or pointers, it will be 0. For user-defined types, it will be defualt constructor.
	*
	* setting path is all modules all the way to setting in interest, separated by dots.
	*
	* \returns desired setting value, or default value if setting pointed by path don't exist.
	*/
	template<typename SettingType = std::string>
	SettingType get(const std::string& settingPath, SettingType defaultValue = SettingType()) {
		std::vector<std::string> splitted = split(settingPath, '.');
		if(splitted.empty()) {
			return defaultValue;
		}

		ConfigNode* currentNode = &main;
		for(unsigned int i = (splitted[0] == "main" ? 1 : 0); i < splitted.size() - 1; i++) {
			currentNode = currentNode->childs[splitted[i]];
			if(!currentNode) {
				logger.warn("Configuration: Requested module \"", splitted[i],
				            "\" doesn't exist. Whole settingPath: ", settingPath);
				return defaultValue;
			}
		}

		if(currentNode->settings.find(splitted.back()) == currentNode->settings.end()) {
			logger.warn("Configuration: All modules exist, but requested setting not. Requested settingPath: ",
			            settingPath);
			return defaultValue;
		}

		std::string result = currentNode->settings[splitted.back()];
		return boost::lexical_cast<SettingType>(result);
	}

	/** \brief get particular setting's value interpreted as std::string
	*
	* \param settingPath setting's name and path to it. In form module0.module1.(...).modulen-1.modulen.settingName
	* \param defaultValue value that will be supplied when real value can't be found.
	*
	* It's purpose is to interpret calls like: config.get("window.name", "Unnamed Window"), with std::string
	* instead of const char*
	*/
	std::string get(const std::string& settingPath, const char* defaultValue);

	/** \brief gives ConfigNode pointed by given path. It consists of all child nodes below it and settings in it.
	*
	* \param nodePath path to desired node, with node name.
	*
	* \returns pointer to desired node.
	*/
	ConfigNode* getNode(const std::string& nodePath);

	/** \brief sets desired setting to desired value
	*
	* \param settingPath full path to setting, with setting's name
	* \param value desired setting value, any type castable to std::string allowed.
	*
	* Will create any modules that don't exist yet ane are in path.
	* Uses boost::lexical_cast to cast value to std::string.
	*/
	template<typename ValType>
	void set(const std::string& settingPath, ValType value) {
		set(settingPath, boost::lexical_cast<std::string>(value));
	}
	void set(const std::string& setting, const std::string& value);

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
