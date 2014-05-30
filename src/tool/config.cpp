#include "config.h"
#include <tool/logger.h>

static void skipWhitechars(const char* where, unsigned int* position);
static void removeComments(char* config, unsigned int size);
static std::string parseSetting(const char* startPos, unsigned int* position);
static std::string parseFilename(const char* startPos, unsigned int* position);

bool Configuration::load(const std::string& filename) {
    char* configuration = loadEntireFile(filename);
    if (!configuration) {
        logger.error("Configuration: Cannot read config file.");
        return false;
    }

    loadFromMemory(configuration);

    delete[] configuration;
    return true;
}


void Configuration::loadFromMemory(std::string configuration) {       //TODO: remove ugly casting to char* from const char*
    size_t size = configuration.size();

    unsigned int currentPos = 0;

    removeComments((char*) configuration.c_str(), size);

    //find main module
    skipWhitechars(configuration.c_str(), &currentPos);
    currentPos += 4;
    skipWhitechars(configuration.c_str(), &currentPos);
    currentPos++;

    //parse main module
    skipWhitechars(configuration.c_str(), &currentPos);
    parseModule(&this->main, (char*) configuration.c_str(), currentPos);
}


//module -> current module
//config -> configuration in memory
//curr -> position after opening bracket of current module
unsigned int Configuration::parseModule(ConfigNode* module, char* config, unsigned int curr) {
    while (true) {
        std::string id = parseString(config, &curr);
        if (id == "include") {
            parseInclude(config, curr, module);
            skipWhitechars(config, &curr);
            continue;
        }

        if (id == "}") break;

        skipWhitechars(config, &curr);
        bool isSetting = config[curr] == '=';
        bool isModule = config[curr] == '{';
        curr++;

        if (isSetting) {
            skipWhitechars(config, &curr);
            module->settings[id] = parseSetting(config, &curr);
            logger.info("Configuration: Loaded setting \"%s\" = \"%s\"", id.c_str(), module->settings[id].c_str());
        } else if (isModule) {
            skipWhitechars(config, &curr);
            logger.info("Configuration: Loading module \"%s\"", id.c_str());
            module->childs[id] = new ConfigNode;
            curr = parseModule(module->childs[id], config, curr);
        } else {
            logger.error("Configuration: Expected `{` or `=`, given `%c`.", config[curr - 1]);
        }

        skipWhitechars(config, &curr);
    }

    return curr;
}

void Configuration::parseInclude(char* config, unsigned int& curr, ConfigNode* module) {
    skipWhitechars(config, &curr);

    std::string file = parseFilename(config, &curr);
    unsigned int includedSize;
    char* includedFile = loadEntireFile(file.c_str(), &includedSize);
    if (!includedFile) {
        logger.error("Configuration: Cannot load file \"%s\" that is included in configuration file", file.c_str());
        skipWhitechars(config, &curr);
        return;
    }

    unsigned int positionInIncluded = 0;
    removeComments(includedFile, includedSize);
    skipWhitechars(includedFile, &positionInIncluded);

    std::string moduleID = parseString(includedFile, &positionInIncluded);
    skipWhitechars(includedFile, &positionInIncluded);
    positionInIncluded++;
    skipWhitechars(includedFile, &positionInIncluded);

    logger.info("Configuration: Loading included module \"%s\" from file \"%s\"", moduleID.c_str(), file.c_str());
    module->childs[moduleID] = new ConfigNode;
    parseModule(module->childs[moduleID], includedFile, positionInIncluded);

    delete[] includedFile;
}

//startpos - ptr to string
//position - offset, will be behind setting
//willn't skip leading spaces!
std::string parseSetting(const char* startPos, unsigned int* position) {
    std::string setting;
    while (isprint(startPos[*position])) {
        if (startPos[*position] == '}')
            break;
        setting += startPos[*position];
        (*position)++;
    }

    //skip tail spaces
    unsigned int last = setting.length() - 1;
    while (isspace(setting[last--]));

    std::string strippedSetting;
    for (unsigned int i = 0; i <= last + 1; i++)
        strippedSetting += setting[i];

    return strippedSetting;
}


//startpos - ptr to string
//position - offset, will point to char after last letter
std::string Configuration::parseString(const char* startPos, unsigned int* position) {
    std::string str;
    while (startPos[*position] != '.' && isgraph(startPos[*position])) {
        str += startPos[*position];
        (*position)++;
    }
    if (startPos[*position] == '.')
        logger.error("Configuration: Illegal character \'.\' in identificator.");

    return str;
}


//startpos - ptr to string
//position - offset, will point to char after last letter
inline std::string parseFilename(const char* startPos, unsigned int* position) {
    std::string str;
    while (isgraph(startPos[*position])) {
        str += startPos[*position];
        (*position)++;
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


static void removeComments(char* config, unsigned int size) {
    for (unsigned int i = 0; i < size; i++) {
        if (config[i] == '-' && config[i + 1] == '-')
            while (config[i] != '\n' && i < size) {
                config[i] = ' ';
                i++;
            }
    }
}

static void skipWhitechars(const char* where, unsigned int* position) {
    while (isspace(where[*position]))
        (*position)++;
}

//returns buffer that contains entire file. Caller is responsible for releasing this memory. Size is touched only in case of success
char* Configuration::loadEntireFile(const std::string& filename, unsigned int* filesize) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);
    if (!file.is_open()) {
        logger.error("Configuration: Cannot open file %s.", filename.c_str());
        return nullptr;
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    char* buffer = new char[size];
    bool readed = file.read(buffer, size);
    if (!readed) {
        logger.error("Configuration: Cannot read content of config file %s.", filename.c_str());
        delete[] buffer;
        return nullptr;
    }

    if (filesize)
        *filesize = size;
    return buffer;
}

