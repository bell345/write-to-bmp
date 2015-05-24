//
// Created by thomas on 2015-05-14.
//

#ifndef WRITE_TO_BMP_UTIL_H
#define WRITE_TO_BMP_UTIL_H

#include <iostream>
#include <stdio.h>
#include <sstream>
#include <stdarg.h>
#include <stdint.h>
#ifdef _WIN32
#include <Windows.h>
#else
#include <sys/time.h>
#include <sys/ioctl.h>
#include <unistd.h>
#endif
#include "crosscrtrepl.h"

template <typename T>
const char* to_string(T input);

void startTiming(bool debug);
extern struct timeval prog_start_time;
double sinceStart();

void customLog(std::string format, ...);
void reportError(std::string errorMessage, ...);
void fullWidthLogWithReturn(std::string format, ...);

#endif //WRITE_TO_BMP_UTIL_H
