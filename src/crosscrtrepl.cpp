#include "crosscrtrepl.h"

void* __memcpy(void* dest, const void* src, const size_t srcCount) {
#ifdef _MSC_VER
    memcpy_s(dest, srcCount, src, srcCount);
    return (void*)0;
#else
    return memcpy(dest, src, srcCount);
#endif
};

FILE* __fopen(const char* filename, const char* restrictions) {
#ifdef _MSC_VER
    FILE* f;
    fopen_s(&f, filename, restrictions);
    return f;
#else
    return fopen(filename, restrictions);
#endif
};

char* __strerror(int errnum) {
#ifdef _MSC_VER
    char errBuf[512];
    strerror_s(errBuf, errnum);
    return errBuf;
#else
    return strerror(errnum);
#endif
};

int __vsprintf(char* dest, size_t maxLength, const char* format, va_list va) {
#ifdef _MSC_VER
    return vsprintf_s(dest, maxLength, format, va);
#else
    return vsprintf(dest, format, va);
#endif
};

char* __strdup(const char* source) {
#ifdef _MSC_VER
    return _strdup(source);
#else
    return strdup(source);
#endif
};