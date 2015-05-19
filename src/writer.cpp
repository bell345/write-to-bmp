#include "writer.h"
#include "logutil.h"
#include "mandelbrot.h"
#include "bmpformat.h"

#ifndef USE_CXX_FILESTREAMS
#define USE_CXX_FILESTREAMS false
#endif

int writeToBMP(BMPFile* bmp, const char* filename) {
    FILE* f = __fopen(filename, "w");
    bmp->write(f);
    fclose(f);
    return 0;
};

int main(int argc, char** argv) {
    srand((unsigned)time(NULL));
    startTiming();

    BMPFile bmp(512, 512);

    std::vector<std::vector<BMPColour> > matrix;
    matrix = FractalGen::calculateMandelbrot<BMPColour>((unsigned)bmp.width, (unsigned)bmp.height);

    bmp.setBitmap(matrix);
    writeToBMP(&bmp, "test2.bmp");

    customLog("Finished!");
    #ifdef _MSC_VER
    printf("Press RETURN to continue...");
    std::cin.get();
    #endif

    return 0;
};
