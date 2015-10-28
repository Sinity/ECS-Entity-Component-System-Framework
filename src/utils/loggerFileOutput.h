#pragma once

#include "logger.h"
#include <fstream>

/** \brief Logger Output that outputs to files on disc */
class FileOutput : public LoggerOutput {
public:
    /** \brief constructor that creates file to output logs and opens it
    *
    * \param filename name of log file
    * \param appendTimestamp if this is true, on the end of filename will be it's creation name.
    *
    * If file already exists, it will be truncated. File name format is filename[timestamp].log. Timestamp is
    * optional.
    *
    * If file can't be open, it will be stated on stderr.
    */
    FileOutput(const std::string& filename, bool appendTimestamp) {
        std::string finalFilename = filename;
        if (appendTimestamp) {
            time_t currTime = time(0);
            tm currentTime = *localtime(&currTime);
            std::string timestamp =
                    format("_", currentTime.tm_mday, "-", currentTime.tm_mon + 1, "@", currentTime.tm_hour, "-",
                           currentTime.tm_min, "-", currentTime.tm_sec);
            finalFilename += timestamp;
        }
        finalFilename += ".log";

        file.open(finalFilename, std::ios::trunc);
        if (!file.is_open()) {
            fprintf(stderr, "Cannot open file %s\n", finalFilename.c_str());
        }
    }

    void write(std::string message) final {
        file << message;
    }

private:
    std::ofstream file;
};
