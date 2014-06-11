#pragma once
#include <string>
#include <fstream>
#include "common/unused.h"

std::string fmt(const std::string& fmt, ...);

enum class LogType {
    Information,
    Warning,
    Error,
    Fatal,
    OFF
};

class Configuration;
class Logger {
public:
    virtual void loadSettings(const std::string& module, Configuration& config) = 0;
    virtual void saveLogs() = 0;

    virtual void info(const char* format, ...) = 0;
    virtual void warn(const char* format, ...) = 0;
    virtual void error(const char* format, ...) = 0;
    virtual void fatal(const char* format, ...) = 0;

    virtual void setConsolePriority(LogType priority) = 0;
    virtual void setFilePriority(LogType priority) = 0;
    virtual void setBufferSize(unsigned int size) = 0;
};

class ConcreteLogger : public Logger {
public:
    ConcreteLogger(const std::string& module, const char* filename = nullptr, bool appendTimestamp = true);
    ~ConcreteLogger();

    void loadSettings(const std::string& module, Configuration& config) override;
    void saveLogs() override;

    void info(const char* format, ...) override;
    void warn(const char* format, ...) override;
    void error(const char* format, ...) override;
    void fatal(const char* format, ...) override;

    void setConsolePriority(LogType priority) override { consoleLoglvl = priority; }
    void setFilePriority(LogType priority) override { fileLoglvl = priority; }
    void setBufferSize(unsigned int size) override { bufferSize = size; }

private:
    void log(LogType logType, const char* logTypeStr, const char* format, va_list ap);

    LogType consoleLoglvl = LogType::Warning;
    LogType fileLoglvl = LogType::Information;

    std::ofstream logFile;
    std::string fileBuffer;
    unsigned int bufferSize = 1024;

    static unsigned int id;
    std::string module;
};

class NullLogger : public Logger {
public:
	void loadSettings(const std::string& module, Configuration& config) override { (void)module; }
    void saveLogs() override {}

	void info(const char* format, ...) override { (void)format; }
	void warn(const char* format, ...) override { (void)format; }
	void error(const char* format, ...) override { (void)format; }
    void fatal(const char* format, ...) override { (void)format; }

	void setConsolePriority(LogType priority) override { (void)priority; }
	void setFilePriority(LogType priority) override { (void)priority; }
	void setBufferSize(unsigned int size) override { (void)size; }
};

