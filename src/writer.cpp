#include "writer.h"
#include "logutil.h"
#include "mandelbrot.h"
#include "bmpformat.h"

#ifndef USE_CXX_FILESTREAMS
#define USE_CXX_FILESTREAMS false
#endif

const char* copyright =
        "This program is (C) Thomas Bell 2015, licensed under the MIT License.\n"
                "For more information, visit http://opensource.org/licenses/MIT";

void bailUsage(const char* exeName) {
    printf("\nUsage:\n %s [-o <FILENAME>] [--output=<FILENAME>]\n", exeName);
    printf(" %s [-h] [--help]\n\n", exeName);
    printf("%s", copyright);
    exit(EXIT_FAILURE);
};

int writeToBMP(BMPFile* bmp, const char* filename) {
    FILE* existTest = __fopen(filename, "r");
    if (existTest != NULL) {
        printf("This file (%s) already exists. Are you sure you want to overwrite this file? (Y/N): ", filename);
        char result[2];
        std::cin.getline(&result[0], 2);

        if (result[0] == 'N' || result[0] == 'n') {
            fclose(existTest);
            return ECANCELED;
        }
    }
    FILE* f = __fopen(filename, "w");
    if (f != NULL) {
        bmp->write(f);
        fclose(f);
    }
    else {
        reportError("An error occured when writing a bitmap to file %s: ", filename);
        return errno;
    }
    return 0;
};

int main(int argc, char** argv) {
    srand((unsigned)time(NULL));
    startTiming();

    std::vector<std::vector<BMPColour> > matrix;
    char* filename = "";

    #ifdef _GETOPT_H
    struct option opts[2] = {
            {"help", 0, NULL, 'h'},
            {"output", 1, NULL, 'o'}
    };

    int c;
    while ((c = getopt_long(argc, argv, "o:", opts, NULL)) != EOF) {
        switch (c) {
            case 'o':
                filename = strdup(optarg);
                break;
            default: bailUsage(argv[0]);
        }
    }
    #endif

    BMPFile bmp(64, 64);
    customLog("Declared new BMP file (%i x %i)", bmp.width, bmp.height);
    customLog("Generating new Mandelbrot Set\n(%f x %f, offset at %f, %f)...", 
        FractalGen::QUERY_WIDTH,
        FractalGen::QUERY_HEIGHT, 
        FractalGen::PAN_X, 
        FractalGen::PAN_Y);
    matrix = FractalGen::calculateMandelbrot<BMPColour>((unsigned)bmp.width, (unsigned)bmp.height);
    customLog("Generated fractal! Now assigning to bitmap...");
    bmp.setBitmap(matrix, true);
    customLog("Assigned fractal to bitmap. Writing to file...");

    if (strlen(filename) < 1) {
        char filenameBuf[512];
        printf("Enter a filename: ");
        std::cin.getline(filenameBuf, 512);

        filename = strdup(filenameBuf);
    }

    if (strlen(filename) < 1) filename = strdup("fractal.bmp");

    int res = writeToBMP(&bmp, filename);

    if (res == 0) customLog("Finished!");
    #ifdef _MSC_VER
    printf("MSVC++ users: Press RETURN to finish...");
    std::cin.get();
    #endif

    return 0;
};
