#pragma once

#include "logger.h"
#include <cstdio>

/** \brief Logger Output that outputs logs to stdout */
class ConsoleOutput : public LoggerOutput {
    void write(std::string message) final {
        printf(message.c_str());
    }
};
