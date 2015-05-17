//
// Created by thomas on 2015-05-14.
//

#include "util.h"

struct timeval prog_start_time;

template <typename T>
const char* to_string(T input) {
    std::stringstream ss;
    ss << input;
    return ss.str().c_str();
};

#ifdef _WIN32
// from http://stackoverflow.com/a/26085827
int gettimeofday(timeval* tp, void* tz) {
    static const uint64_t EPOCH = uint64_t(116444736000000000ULL);

    FILETIME   filetime;
    SYSTEMTIME systime;
    uint64_t   time;

    GetSystemTime(&systime);
    SystemTimeToFileTime(&systime, &filetime);
    time =  uint64_t(filetime.dwLowDateTime);
    time += uint64_t(filetime.dwHighDateTime) << 32;

    tp->tv_sec  = long((time - EPOCH) / 10000000L);
    tp->tv_usec = long(systime.wMilliseconds * 1000);
    return 0;
}
#endif

void startTiming() {
    gettimeofday(&prog_start_time, NULL);
}

double sinceStart() {
    struct timeval currtime;
    const int us_in_ms = (int)1e3; // microseconds in a millisecond
    const int ms_in_s = (int)1e3;  // milliseconds in a second

    gettimeofday(&currtime, NULL);
    return ((double)currtime.tv_usec        / (us_in_ms) + currtime.tv_sec        * ms_in_s) -
           ((double)prog_start_time.tv_usec / (us_in_ms) + prog_start_time.tv_sec * ms_in_s);
};

void customLog(std::string format, ...) {
    va_list va; va_start(va, format);

    printf("[%ldms] ", long(sinceStart()));
    vprintf(format.c_str(), va);
    printf("\n");

    va_end(va);
}

static char* __strerror(int errnum) {
    #ifdef _MSC_VER
    char errBuf[512];
    strerror_s(errBuf, errnum);
    return errBuf;
    #else
    return strerror(errnum);
    #endif
}

void reportError(std::string errorMessage, ...) {
    va_list va; va_start(va, errorMessage);

    errorMessage = "An error occured: " + errorMessage;

    fprintf(stderr, "[%ldms] ", long(sinceStart()));
    vfprintf(stderr, errorMessage.c_str(), va);
    fprintf(stderr, ": \n\t%s\n", __strerror(errno));

    va_end(va);
};