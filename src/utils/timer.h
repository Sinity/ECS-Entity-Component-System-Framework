#pragma once

#include <chrono>

class Timer {
public:
    /** \brief default constructor that starts timer immmediately */
    Timer() : startTime(std::chrono::high_resolution_clock::now()) { };

    /** \brief Timer will start counting from 0 */
    void start() {
        startTime = std::chrono::high_resolution_clock::now();
    }

    /** \brief returns elapsed time without restarting Timer. */
    std::chrono::milliseconds elapsed() {
        std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
        auto elapsedTime = now - startTime;
        return std::chrono::duration_cast<std::chrono::milliseconds>(elapsedTime);
    }

    /** \brief returns elaped time and restarts Timer that it will start counting from 0. */
    std::chrono::milliseconds reset() {
        std::chrono::milliseconds elapsedTime = elapsed();
        startTime = std::chrono::high_resolution_clock::now();
        return elapsedTime;
    }

private:
    std::chrono::high_resolution_clock::time_point startTime;
};
