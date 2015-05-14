//
// Created by thomas on 2015-05-14.
//

#ifndef WRITE_TO_BMP_UTIL_H
#define WRITE_TO_BMP_UTIL_H

#include "main.h"
#include <sstream>
#include <stdarg.h>

template <typename T>
const char* to_string(T input);

long sinceStart();

void customLog(std::string format, ...);
void reportError(std::string errorMessage, ...);

#endif //WRITE_TO_BMP_UTIL_H
