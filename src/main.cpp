#include "main.h"
#include "util.h"

/**
 * Opens a file at fileName for access through standard C I/O.
 * @param fileName The directory path + filename of the file to open.
 * @param restrictions An array of flag characters that indicate the restrictions on using the FILE object.
 * Examples include 'r' for read-only and 'rw' for read-write only.
 */
FILE* openFile(const char* fileName, const char* restrictions) {
    FILE* file;
    file = fopen(fileName, restrictions);
    if (file == NULL) {
        reportError("The file %s could not be opened due to", fileName);
        return file;
    }
    return file;
}

int main(int argc, char** argv) {
    customLog("Yay!");

    FILE* file = openFile("/doesntexist.txt", "r");
    if (file != NULL) fclose(file);

    return 0;
}