//
// Created by thomas on 2015-05-14.
//

#include "util.h"

template <typename T>
const char* to_string(T input) {
    std::stringstream ss;
    ss << input;
    return ss.str().c_str();
};

long sinceStart() {
    auto currtime = hresclock::now();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(currtime - starttime).count();
};

void customLog(std::string format, ...) {
    va_list va; va_start(va, format);

    printf("[%s] ", to_string(sinceStart()));
    vprintf(format.c_str(), va);
    printf("\n");

    va_end(va);
}

void reportError(std::string errorMessage, ...) {
    va_list va; va_start(va, errorMessage);

    errorMessage = "An error occured: " + errorMessage;

    fprintf(stderr, "[%s] ", to_string(sinceStart()));
    vfprintf(stderr, errorMessage.c_str(), va);
    fprintf(stderr, ": \n\t%s\n", strerror(errno));

    va_end(va);
};