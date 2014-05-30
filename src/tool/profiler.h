#pragma once
#include <string>

namespace sf {
    class Time;
    class Clock;
}
class Profile;
class Logger;
class Profiler {
public:
    Profiler(const std::string& filename, Logger& logger);
    ~Profiler();

    void start(const char* name);
    void stop();

    void saveResults();
    void saveSamples(Profile* profile, bool childsToo);

private:
    Profile* main;
    Profile* current;

    std::string filename;

    Logger& logger;
};

