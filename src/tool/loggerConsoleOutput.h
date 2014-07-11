#include "logger.h"
#include <cstdio>

class ConsoleOutput : public LoggerOutput {
	void write(std::string message) final{
        printf(message.c_str());
	}
};
