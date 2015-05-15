#include "main.h"
#include "util.h"

struct BITMAPFILEHEADER {
    char type[2];
    char size[4];
    char reserved[4];
    char offBits[4];
};
struct BITMAPINFOHEADER {
    char size[4] = {0, 0, 0, 0x28};
    char width[4];
    char height[4];
    char planes[2];
    char bitCount[2];
    char compression[4];
    char sizeImage[4];
    char xPxPerMetre[4];
    char yPxPerMetre[4];
    char coloursUsed[4];
    char importantColour[4];
};

int writeToBMP() {
    BITMAPFILEHEADER head;

    return 0;
};

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
    BITMAPFILEHEADER head;
    startTiming();

    FILE* file = openFile("/doesntexist.txt", "r");
    if (file != NULL) fclose(file);

    return 0;
}