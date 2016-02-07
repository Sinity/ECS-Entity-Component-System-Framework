#pragma once

#include <memory>
#include <vector>
#include <ctime>
#include <algorithm>
#include "formatString.h"

enum class LogType { Information, Warning, Error, Fatal, OFF };

/** logger output base class. Derived class must only implement write method.
*
* All outputs have priority, which decides if particular log will be logged by them.
* Default priority is lowest possible, that is, every log will be logged.
*
*/
class LoggerOutput {
   public:
    /** \brief logger calls this when log should be written to given output.
    *
    * \param message formatted log that should be written in destination
    */
    virtual void write(std::string message) = 0;

    /** \brief given priority, returns if this output satistify it.
    *
    * \param priority priority to check output's priority satistifies it.
    *
    * \returns if this output has sufficient priority returns true, false otherwise.
    */
    bool isPrioritySufficient(LogType priority) const { return priority >= minPriority; }

    /** \brief sets minimum priority of log, that it can be written to this output. */
    void setMinPriority(LogType priority) { minPriority = priority; }

   private:
    LogType minPriority = LogType::Information;
};

/** \brief class for formatting log messages and redirecting these to appropiate outputs
*
* Single logger can have arbitrary number of outputs, like console, file, custom ingame console, network etc.
*
* Logs have 4 different priorities, each is hardwired into particular method: info, warn, error, fatal.
*
* Usage of these method looks like this:
* logger.info("Value of i: ", i, ".");
* logger.info(PEXPR(i));
*
* PEXPR is macro from formatString that can print any expression alongside it's value conveniently.
*
*/
class Logger {
   public:
    /** \brief default constructor. Logger name will remain "Unnamed Logger" */
    Logger() = default;

    /** \brief constructor setting logger name
    *
    * \param loggerName desired name of this logger
    */
    explicit Logger(std::string loggerName) : loggerName(std::move(loggerName)) {}

    template <typename... Args>
    void info(Args&&... args) const {
        log(LogType::Information, "INFO", std::forward<Args>(args)...);
    }

    template <typename... Args>
    void warn(Args&&... args) const {
        log(LogType::Warning, "WARN", std::forward<Args>(args)...);
    }

    template <typename... Args>
    void error(Args&&... args) const {
        log(LogType::Error, "ERROR", std::forward<Args>(args)...);
    }

    template <typename... Args>
    void fatal(Args&&... args) const {
        log(LogType::Fatal, "FATAL", std::forward<Args>(args)...);
    }

    /** \brief turns on this Logger. By default Logger is turned on. */
    void on() { loggerEnabled = true; }

    /** \brief turns off this Logger. When Logger is turned off, it won't send any logs to outputs. */
    void off() { loggerEnabled = false; }

    /** \brief appends desired output to this logger outputs
    *
    * \param output smart ptr to output that should be added. Logger will keep reference to it.
    *
    * Outputs can he shared between different Loggers, so it's managed by shared pointers. Each logger keeps reference
    * to it, so it won't be destroyed untill deleted from all loggers(and of course other possible locations).
    */
    void addOutput(std::shared_ptr<LoggerOutput> output) { outputs.push_back(std::move(output)); }

    /** \brief removes output from this logger outputs
    *
    * \param output smart ptr to output that will be removed from outputs list
    *
    * It won't necessarily destroy output, because it can be shared with other Loggers. It just removes it from
    * this logger, and destroys only if it's last reference.
    */
    void removeOutput(const LoggerOutput* output) {
        auto it = std::find_if(
            outputs.begin(), outputs.end(),
            [output](const std::shared_ptr<LoggerOutput>& candidate) { return candidate.get() == output; });

        if (it != outputs.end()) {
            outputs.erase(it);
        }
    }

    /** \brief sets outputs that it haves all outputs from supplied other logger.
    *
    * \param other logger from where all outputs will be copied.
    *
    * It will remove all outputs that it currently have.
    */
    void mirrorOutputs(const Logger& other) {
        clearOutputs();

        for (const auto& output : other.outputs) {
            outputs.push_back(output);
        }
    }

    /** \brief removes all outputs that are connected to this logger. */
    void clearOutputs() { outputs.clear(); }

   private:
    std::string loggerName = "Unnamed Logger";
    bool loggerEnabled = true;
    std::vector<std::shared_ptr<LoggerOutput>> outputs;

    template <typename... Args>
    void log(LogType logType, std::string logTypeRepresentation, Args&&... args) const {
        if (!loggerEnabled || std::none_of(outputs.begin(), outputs.end(), [&logType](const auto& output) {
                return output->isPrioritySufficient(logType);
            })) {
            return;
        }

        time_t currTime = time(nullptr);
        tm currentTime = *localtime(&currTime);
        std::string timeTag = format("[", currentTime.tm_hour, ":", currentTime.tm_min, ":", currentTime.tm_sec, "]");

        std::string loggerNameTag = "[" + loggerName + "]";
        std::string logTypeTag = "[" + logTypeRepresentation + "]";
        std::string rawMessage = format(std::forward<Args>(args)...);
        std::string formattedMessage =
            timeTag + " " + loggerNameTag + " " + logTypeTag + " " + std::move(rawMessage) + "\n";

        for (auto& output : outputs) {
            if (output->isPrioritySufficient(logType)) {
                output->write(std::move(formattedMessage));
            }
        }
    }
};
