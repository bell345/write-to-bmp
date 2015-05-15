//
// Created by thomas on 2015-05-14.
//

#ifndef WRITE_TO_BMP_UTIL_H
#define WRITE_TO_BMP_UTIL_H

#include "main.h"
#include <sstream>
#include <stdarg.h>
#include <sys/time.h>

template <typename T>
const char* to_string(T input);

void startTiming();
extern struct timeval prog_start_time;
double sinceStart();

void customLog(std::string format, ...);
void reportError(std::string errorMessage, ...);

#endif //WRITE_TO_BMP_UTIL_H
