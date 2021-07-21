#ifndef PDS_LAB_1_DURATIONLOGGER_H
#define PDS_LAB_1_DURATIONLOGGER_H

#include <chrono>
#include <iostream>
#include <string>

class DurationLogger {
    std::string name;
    std::chrono::time_point<std::chrono::high_resolution_clock> start;
    std::chrono::time_point<std::chrono::high_resolution_clock> stop;

public:
    DurationLogger(std::string name);

    ~DurationLogger();
};

#endif //PDS_LAB_1_DURATIONLOGGER_H
