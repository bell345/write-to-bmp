#ifndef WRITE_TO_BMP_CROSSCRTREPL_H
#define WRITE_TO_BMP_CROSSCRTREPL_H

/** crosscrtrepl.h
 * A magical file for getting around MSVC++'s warnings about not using _s functions.
 * I probably shouldn't be using C style I/O and string ops anyway, but this was easier.
 */

#include <stdio.h>
#include <string.h>

void* __memcpy(void* dest, const void* src, const size_t srcCount);
FILE* __fopen(const char* filename, const char* restrictions);
char* __strerror(int errnum);
int __vsprintf(char* dest, size_t maxLength, const char* format, va_list va);
char* __strdup(const char* source);

#endif