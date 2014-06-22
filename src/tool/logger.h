#pragma once
#include <string>
#include <memory>
#include <vector>
#include "common/formatString.h"

enum class LogType {
    Information,
    Warning,
    Error,
    Fatal,
    OFF
};

class LoggerOutput {
public:
	virtual void write(std::string message) = 0;

	bool isPrioritySufficient(LogType priority) { return priority >= minPriority; }
	void setMinPriority(LogType priority) { minPriority = priority; }

private:
	LogType minPriority = LogType::Information;
};

class Logger {
public:
	Logger() = default;
	Logger(std::string loggerName) :
		loggerName(std::move(loggerName)) {
	}

	template<typename... Args>
	void info(Args... args) {
		log(LogType::Information, "INFO", args...);
	}

	template<typename... Args>
	void warn(Args... args) {
		log(LogType::Warning, "WARN", args...);
	}

	template<typename... Args>
	void error(Args... args) {
		log(LogType::Error, "ERROR", args...);
	}

	template<typename... Args>
	void fatal(Args... args) {
		log(LogType::Fatal, "FATAL", args...);
	}

	void on() { loggerEnabled = true; }
	void off() { loggerEnabled = false; }

	void addOutput(std::shared_ptr<LoggerOutput> output) { outputs.push_back(std::move(output)); }
	void removeOutput(const std::shared_ptr<LoggerOutput>& output) {
		auto it = std::find(outputs.begin(), outputs.end(), output); 
		if (it != outputs.end()) {
			outputs.erase(it);
		}
	}

private:
	std::string loggerName = "Generic Logger";
	bool loggerEnabled = true;
	std::vector<std::shared_ptr<LoggerOutput>> outputs;

	template<typename... Args>
	void log(LogType logType, std::string logTypeRepresentation, Args... args) {
		if (!loggerEnabled) {
			return;
		}

		time_t currTime = time(0);
		tm currentTime;
		localtime_s(&currentTime, &currTime);
		std::string timeTag = format("[", currentTime.tm_hour, ":", currentTime.tm_min, ":", currentTime.tm_sec, "]");

		std::string loggerNameTag = "[" + loggerName + "]";
		std::string logTypeTag = "[" + logTypeRepresentation + "]";
		std::string rawMessage = format(args...);
		std::string formattedMessage = timeTag + " " + loggerNameTag + " " + logTypeTag + " " + rawMessage;

		for (auto& output : outputs) {
			if (output->isPrioritySufficient(logType)) {
				output->write(std::move(formattedMessage));
			}
		}
	}
};
