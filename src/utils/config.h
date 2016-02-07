#pragma once

#include <boost/property_tree/ptree.hpp>
#include "stringUtils.h"
#include "logger.h"

using namespace std::string_literals;

/** \brief class for reading and managing program's configuration.
*
* Sample configuration file:
*
* Graphics {
*   Resolution { -- some comment
*       x = 1920
*       y = 1080
*   }
*   fullscreen = true
*   windowName = Some Application Window
* }
* --some other comment
* Physics {
*   updatesPerSecond = 100
* }
*
* Sample call to retrieve information:
* configuration.get<int>("Graphics.Resolution.x"); // retrieve this setting interpreting it as int.
*                                                  // If setting doesn't exist, returns 0(default-constructed variable)
*
* configuration.get("Graphics.Resolution.x", 800); // as above, type inferred from fallback value
* configuration.get("Graphics.windowName", "Unknown Window"); //std::string is default setting type
*
* Dots and whitespaces aren't allowed inside module and setting names.
* Setting value is everything beyond equality sign to the end of line, except preceding and following whitespaces.
*
* Configuration can be loaded from file or memory(std::string).
* Class allows for setting particular settings, getting setting values, explictly checking if setting exists,
*     serializing current configuration to the string and clearing config tree.
*
* Value types are specified by get caller. For example, config.get("path.to.some.setting", 0u), will return
* unsigned because fallback argument provided by caller is unsigned. When type can't be interpreted, caller can
* specify it explictly by passing type in template parameter: config.get<unsigned int>("path.to.some.setting").
*/
class Configuration {
   public:
    Configuration(std::string logfile = "config_log.txt", LogType consoleThreshold = LogType::Warning);

    bool load(const std::string& filename);
    bool loadFromMemory(std::string& config);

    template <typename T>
    T get(const std::string& settingPath, T&& fallbackValue = T()) {
        return configurationTree.get(settingPath, std::forward<T>(fallbackValue));
    }

    // For situations where there is no fallback value or it's C string literal(it converts it to std::string)
    std::string get(const std::string& settingPath, const char* fallbackValue = "");

    bool exists(const std::string& settingPath);

    template <typename T>
    void set(const std::string& settingPath, T&& value) {
        configurationTree.put(settingPath, std::forward<T>(value));
    }

    std::string serializeConfig();

    void clear();

   private:
    boost::property_tree::ptree configurationTree;
    Logger logger;

    bool parseModule(const std::string& modulePath, const std::string& input, size_t& cursor);

    template <typename PropertyTree>
    std::string serializeModule(PropertyTree& ptree, int indentCount = 0) {
        std::string result;

        // prepare proper indent
        std::string indent;
        for (auto i = 0; i < indentCount; i++) {
            indent += "\t";
        }

        for (const auto& e : ptree) {
            auto isModule = e.second.template get_value_optional<std::string>().value() == "";
            // get_value_optional always rets true, have to check if it's empty manually
            if (isModule) {
                result += "\n"s + indent + e.first + " {\n";
                result += indent + serializeModule(e.second, indentCount++);  // recursively serialize that module
                result += indent + "}\n\n";
            } else {
                auto settingVal = e.second.template get_value_optional<std::string>().value();
                result += (indentCount == 0 ? "" : indent + "\t") + e.first + " = "s + settingVal + "\n";
            }
        }

        return result;
    }
};
