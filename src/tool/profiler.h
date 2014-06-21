#pragma once
#include <string>
#include "tool/logger.h"

namespace sf {
    class Time;
    class Clock;
}
class Profile;
class Logger;
class Profiler {
public:
    Profiler(const std::string& filename);
    ~Profiler();

    void start(const char* name);
    void stop();

    void saveResults();
    void saveSamples(Profile* profile, bool childsToo);

	Logger logger;
private:
    Profile* main;
    Profile* current;

    std::string filename;


	Profiler& operator=(Profiler& profiler) = delete;
};

