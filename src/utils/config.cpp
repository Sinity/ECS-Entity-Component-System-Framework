#include "config.h"
#include <fstream>
#include <boost/property_tree/xml_parser.hpp>
#include "loggerConsoleOutput.h"
#include "loggerFileOutput.h"

using namespace std::literals;

std::string loadFile(const std::string& filename);
std::pair<unsigned int, unsigned int> locationInConfig(const std::string config, size_t position);
void skipWhitespace(const std::string& config, size_t& cursor);
void removeComments(std::string& config);
std::string parseWord(const std::string& config, size_t& cursor);
std::string parseSettingValue(const std::string& config, size_t& cursor);


Configuration::Configuration(std::string logfile, LogType consoleThreshold) :
      logger("CONFIG") {
    auto consoleOut = std::make_shared<ConsoleOutput>();
    consoleOut->setMinPriority(consoleThreshold);
    logger.addOutput(std::move(consoleOut));

    logger.addOutput(std::make_shared<FileOutput>(std::move(logfile), true));
}

bool Configuration::load(const std::string& filename) {
    auto config = loadFile(filename);
    return loadFromMemory(config);
}

bool Configuration::loadFromMemory(std::string& config) {
    removeComments(config);

    size_t cursor = 0;
    auto success = parseModule("", config, cursor);

    if(success) {
        logger.info("Configuration loaded successfully.");
    } else {
        logger.warn("Can't parse configuration properly.");
    }
    
    logger.info("Configuration state dump:\n\n", serializeConfig());

    return success;
}

std::string Configuration::get(const std::string& settingPath, const char* fallbackValue) {
    return get<std::string>(settingPath, fallbackValue);
}

bool Configuration::exists(const std::string& settingPath) {
    return (bool)configurationTree.get_optional<std::string>(settingPath);
        // std::string because this guarantees that there won't be any conversion problems
}

std::string Configuration::serializeConfig() {
    return serializeModule(configurationTree);
}

void Configuration::clear() {
    configurationTree.clear();
}

bool Configuration::parseModule(const std::string& modulePath, const std::string& config, size_t& cursor) {
    while(true) {
        //check if it's end of module
        skipWhitespace(config, cursor);
        auto endOfLocalModule = config[cursor] == '}';
        auto endOfGlobalModule = cursor == config.size() - 1;
        if(endOfLocalModule || endOfGlobalModule || config.size() == 0) {
            cursor++;
            return true;
        }

        //get the token(which can be either setting name or nested module name)
        auto token = parseWord(config, cursor);
        if(cursor == config.size() - 1) {
            logger.error("Configuration ended abruptly right before "
                "setting assignment or module opening brace. Current module: ", modulePath);
            return false;
        }

        //get symbol which identifies current construct
        auto tokenMeaning = config[cursor++];
        skipWhitespace(config, cursor);
        if(cursor == config.size() - 1) {
            logger.error("Configuration ended abruptly right after "
                 "setting assignment or module opening brace. Current module: ", modulePath);
            return false;
        }

        auto path = modulePath.size() != 0 ? modulePath + "." : ""; //global module special case
        if(tokenMeaning == '=') { //it's a setting
            set(path + std::move(token), parseSettingValue(config, cursor));
        } else if(tokenMeaning == '{') { //it's a nested module
            if (!parseModule(path + std::move(token), config, cursor)) {
                return false;
            }
        } else {
            auto location = locationInConfig(config, cursor);
            logger.error("Illegal character '", tokenMeaning, "' at line ", location.first,
                     ", column ", location.second, ", in module ", (modulePath.size() != 0 ? modulePath : "#global scope#"),
                     ". Allowed chars: = or {, stopping parsing!");
            return false;
        }
    }
}

//returns first word(alphanumeric sequence of chars) from the config[cursor]
//cursor position is at next non-whitespace char after this word or at the last char
std::string parseWord(const std::string& config, size_t& cursor) {
    skipWhitespace(config, cursor);

    auto beginning = cursor;
    while (isalnum(config[cursor]) && config.size() > cursor + 1) {
            cursor++;
    }
    auto end = isalnum(config[cursor]) ? cursor : cursor - 1;

    skipWhitespace(config, cursor);

    return config.substr(beginning, end - beginning + 1);
}

//returns substring <init cursor, \n), trimming whitespace at the begininng and at the end.
//new cursor position is at next char after \n, or at last char of the config
std::string parseSettingValue(const std::string& config, size_t& cursor) {
    assert(config.size() > cursor && "cursor is out of range!");

    //omit initial whitespace
    while (isspace(config[cursor])) {
        if (config[cursor] == '\n' || cursor + 1 == config.size()) { //it means that there is lack of setting's value
            return "";
        }

        cursor++;
    }
    auto beginning = cursor;

    //everything between end of initial whitespace and newline is setting's value
    while (config[cursor] != '\n' && config.size() > cursor + 1) {
        cursor++;
    }

    //trim whitespace at the end
    auto end = cursor;
    while (isspace(config[end])) {
        end--;
    }

    //set cursor to proper position
    if (config[cursor] == '\n' && config.size() > cursor + 1) {
        cursor++;
    }

    return config.substr(beginning, end - beginning + 1);
}

//push cursor forward until char under cursor is not whitespace.
//if it will reach end of config, it will halt at last char of config.
void skipWhitespace(const std::string& config, size_t& cursor) {
    assert(config.size() > cursor && "cursor is out of range!");

    while (isspace(config[cursor])) {
        if (cursor + 1 >= config.size()) {
            return;
        }

        cursor++;
    }
}

void removeComments(std::string& config) {
    if (config.size() == 0) {
        return;
    }

    for(auto i = 0u; i < config.size() - 1; i++) {
        if(config[i] == '-' && config[i+1] == '-') {
            for(; config[i] != '\n' && i < config.size(); i++) {
                config[i] = ' ';
            }
        }
    }
}

//loads whole content of file to std::string, in text mode(new lines translated to \n if necessary).
//if it can't open a file, returns empty string instead
std::string loadFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::in);
    if (!file) {
        return "";
    }

    std::string result;
    file.seekg(0, std::ios::end);
    result.resize((unsigned int)file.tellg());
    file.seekg(0, std::ios::beg);
    file.read(&result[0], result.size());

    return result;
}

//First: line, second: column
std::pair<unsigned int, unsigned int> locationInConfig(const std::string config, size_t position) {
    assert(config.size() > position && "cursor is out of range!");

    std::pair<unsigned int, unsigned int> location{1, 1};
    auto lastNewlinePosition = 0;

    for(auto i = 0u; i < position; i++) {
        if(config[i] == '\n') {
            lastNewlinePosition = i;
            location.first++;
        }
    }

    location.second = position - lastNewlinePosition;
    return location;
}

