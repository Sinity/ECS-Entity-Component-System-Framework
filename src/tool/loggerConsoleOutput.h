#include "logger.h"
#include <fstream>

class ConsoleOutput : public LoggerOutput {
	void write(std::string message) final{
		fprintf(stdout, message.c_str());
	}
};
