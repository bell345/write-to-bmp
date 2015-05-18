#ifndef WRITE_TO_BMP_CROSSCRTREPL_H
#define WRITE_TO_BMP_CROSSCRTREPL_H

#include <stdio.h>
#include <string.h>

void* __memcpy(void* dest, const void* src, const size_t srcCount);
FILE* __fopen(const char* filename, const char* restrictions);
char* __strerror(int errnum);

#endif