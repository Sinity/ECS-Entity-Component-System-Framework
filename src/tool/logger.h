#pragma once

#include <memory>
#include <vector>
#include <ctime>
#include <algorithm>
#include "common/formatString.h"

enum class LogType {
	Information,
	Warning,
	Error,
	Fatal,
	OFF
};

/** logger output base class. Derived class must only implement write method.
*
* All outputs have priority, which decides if particular log will be passed to it.
*
*/
class LoggerOutput {
public:
	/** \brief logger calls this when log should be written to given output.
	*
	* \param message formatted log that should be written in destination
	*/
	virtual void write(std::string message) = 0;

	bool isPrioritySufficient(LogType priority) const {
		return priority >= minPriority;
	}

	void setMinPriority(LogType priority) {
		minPriority = priority;
	}

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
* PEXPR is macro from formatString that can print any expression alongside it's value easily.
*
*/
class Logger {
public:
	Logger() = default;

	explicit Logger(std::string loggerName) :
			loggerName(std::move(loggerName)) {
	}

	template<typename... Args>
	void info(Args... args) const {
		log(LogType::Information, "INFO", args...);
	}

	template<typename... Args>
	void warn(Args... args) const {
		log(LogType::Warning, "WARN", args...);
	}

	template<typename... Args>
	void error(Args... args) const {
		log(LogType::Error, "ERROR", args...);
	}

	template<typename... Args>
	void fatal(Args... args) const {
		log(LogType::Fatal, "FATAL", args...);
	}

	void on() {
		loggerEnabled = true;
	}

	void off() {
		loggerEnabled = false;
	}

	void addOutput(std::shared_ptr<LoggerOutput> output) {
		outputs.push_back(std::move(output));
	}

	void removeOutput(const std::shared_ptr<LoggerOutput>& output) {
		auto it = std::find(outputs.begin(), outputs.end(), output);
		if(it != outputs.end()) {
			outputs.erase(it);
		}
	}

	/** \brief sets outputs that it haves all outputs from supplied other logger.
	*
	* \param other logger from where all outputs will be copied.
	*
	* It will remove all outputs that it currently have.
	*/
	void setOutputs(const Logger& other) {
		clearOutputs();

		for(const auto& output : other.outputs) {
			outputs.push_back(output);
		}
	}

	/** \brief removeds all outputs that are connected to this logger. */
	void clearOutputs() {
		outputs.clear();
	}

private:
	std::string loggerName = "Unnamed Logger";
	bool loggerEnabled = true;
	std::vector<std::shared_ptr<LoggerOutput>> outputs;

	template<typename... Args>
	void log(LogType logType, std::string logTypeRepresentation, Args... args) const {
		if(!loggerEnabled || std::none_of(outputs.begin(), outputs.end(),
		                                  [&logType](const std::shared_ptr<LoggerOutput>& output) {
		                                      return output->isPrioritySufficient(logType);
		                                  })) {
			return;
		}

		time_t currTime = time(0);
		tm currentTime = *localtime(&currTime);
		std::string timeTag = format("[", currentTime.tm_hour, ":", currentTime.tm_min, ":", currentTime.tm_sec, "]");

		std::string loggerNameTag = "[" + loggerName + "]";
		std::string logTypeTag = "[" + logTypeRepresentation + "]";
		std::string rawMessage = format(args...);
		std::string formattedMessage = timeTag + " " + loggerNameTag + " " + logTypeTag + " " + rawMessage + "\n";

		for(auto& output : outputs) {
			if(output->isPrioritySufficient(logType)) {
				output->write(std::move(formattedMessage));
			}
		}
	}
};
