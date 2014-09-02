#include "logger.h"
#include <fstream>

class FileOutput : public LoggerOutput {
public:
	FileOutput(const std::string& filename, bool appendTimestamp) {
		std::string finalFilename = filename;
		if(appendTimestamp) {
			time_t currTime = time(0);
			tm currentTime = *localtime(&currTime);
			std::string timestamp = format("_", currentTime.tm_mday, "-", currentTime.tm_mon + 1, "@", currentTime.tm_hour, "-", currentTime.tm_min, "-", currentTime.tm_sec);
			finalFilename += timestamp;
		}
		finalFilename += ".log";

		file.open(finalFilename, std::ios::trunc);
		if(!file.is_open()) {
			fprintf(stderr, "Cannot open file %s\n", finalFilename.c_str());
		}
	}

	void write(std::string message) final {
		file << message;
	}

private:
	std::ofstream file;
};
