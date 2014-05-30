#include <cstdarg>
#include <ctime>
#include <cstring>
#include "config.h"

std::string fmt(const std::string& fmt, ...);
std::string ap_fmt(const std::string& fmt, va_list ap);

ConcreteLogger::ConcreteLogger(const std::string& module, const char* filename, bool appendTimestamp) : module(module) {
    time_t currTime = time(0);
    tm* currentDate = localtime(&currTime);

    if (filename) {
        std::string fullName;

        if (appendTimestamp) {
            fullName = filename + fmt("-%d-%d-%d@%d.%d.%d.txt",
                    currentDate->tm_hour, currentDate->tm_min, currentDate->tm_sec,
                    currentDate->tm_mday, currentDate->tm_mon + 1,
                    currentDate->tm_year + 1900);
        } else {
            fullName = std::string(filename) + ".txt";
        }

        logFile.open(fullName, std::ios::ate | std::ios::app);
        if (!logFile.is_open()) {
            fprintf(stderr, "Logger %s: Cannot open file: %s. Trying to open file with unique id in name...\n", module.c_str(), fullName.c_str());
            fullName += fmt("_%u", id);
            logFile.open(fullName, std::ios::ate | std::ios::app);
            if (!logFile.is_open()) {
                fprintf(stderr, "Logger %s: Failed to open file with unique id. Filename: %s\n", module.c_str(), fullName.c_str());
            }
        }
    }

    info("Logger %s (id: %u) initialized. Date: %d/%d/%d", module.c_str(), id, 1900 + currentDate->tm_year, currentDate->tm_mon + 1, currentDate->tm_mday);
    id++;
}

ConcreteLogger::~ConcreteLogger(void) {
    if (logFile.is_open()) {
        saveLogs();
    }
}

void ConcreteLogger::saveLogs() {
    if (logFile.is_open()) {
        logFile << fileBuffer;
        logFile.flush();
    }
    fileBuffer.clear();
}

void ConcreteLogger::log(LogType logType, const char* logTypeStr, const char* format, va_list ap) {
    if (logType < consoleLoglvl && logType < fileLoglvl)
        return;

    time_t currTime = time(0);
    tm* currentTime = localtime(&currTime);

    std::string logtxt = fmt("[%d:%d:%d] [%s] [%s] ", currentTime->tm_hour, currentTime->tm_min, currentTime->tm_sec, module.c_str(), logTypeStr);
    logtxt += ap_fmt(std::move(format), ap);

    if (logType >= consoleLoglvl) {
        printf("%s\n", logtxt.c_str());
    }
    if (logType >= fileLoglvl) {
        fileBuffer += logtxt + "\n";
        if (fileBuffer.size() > bufferSize) {
            saveLogs();
        }
    }
}

void ConcreteLogger::info(const char* format, ...) {
    va_list(ap);
    va_start(ap, format);
    log(LogType::Information, "INFO", format, ap);
    va_end(ap);
}

void ConcreteLogger::warn(const char* format, ...) {
    va_list(ap);
    va_start(ap, format);
    log(LogType::Warning, "WARN", format, ap);
    va_end(ap);
}

void ConcreteLogger::error(const char* format, ...) {
    va_list(ap);
    va_start(ap, format);
    log(LogType::Error, "ERROR", format, ap);
    va_end(ap);
}

void ConcreteLogger::fatal(const char* format, ...) {
    va_list(ap);
    va_start(ap, format);
    log(LogType::Fatal, "FATAL", format, ap);
    va_end(ap);
}

void ConcreteLogger::loadSettings(const std::string& module, Configuration& config) {
    bufferSize = config.get<unsigned int>((module + ".bufferSize").c_str(), bufferSize);
    std::string consoleLoglvlString = config.get((module + ".consoleLoglvl").c_str());
    if (consoleLoglvlString == "INFO") this->consoleLoglvl = LogType::Information;
    else if (consoleLoglvlString == "WARN") this->consoleLoglvl = LogType::Warning;
    else if (consoleLoglvlString == "ERROR") this->consoleLoglvl = LogType::Error;
    else if (consoleLoglvlString == "FATAL") this->consoleLoglvl = LogType::Fatal;
    else if (consoleLoglvlString == "OFF") this->consoleLoglvl = LogType::OFF;

    std::string fileLoglvlString = config.get((module + ".fileLoglvl").c_str());
    if (fileLoglvlString == "INFO") this->fileLoglvl = LogType::Information;
    else if (fileLoglvlString == "WARN") this->fileLoglvl = LogType::Warning;
    else if (fileLoglvlString == "ERROR") this->fileLoglvl = LogType::Error;
    else if (fileLoglvlString == "FATAL") this->fileLoglvl = LogType::Fatal;
    else if (fileLoglvlString == "OFF") this->fileLoglvl = LogType::OFF;
}

std::string fmt(const std::string& format, ...) {
    va_list(ap);
    va_start(ap, format); //fix this (change format to const char* maybe)
    std::string formatted = ap_fmt(format, ap);
    va_end(ap);
    return formatted;
}

std::string ap_fmt(const std::string& fmt, va_list ap) {
    int size = 200;
    std::string str;
    va_list a;
    while (1) {
        str.resize(size);
        va_copy(a, ap);
        int n = vsnprintf((char*) str.c_str(), size, fmt.c_str(), a);
        if (n > -1 && n < size) {
            str.resize(n);
            return str;
        }
        if (n > -1)
            size = n + 1;
        else
            size *= 2;
    }
}

unsigned int ConcreteLogger::id = 0;
