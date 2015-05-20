#include "writer.h"
#include "logutil.h"
#include "mandelbrot.h"
#include "bmpformat.h"

#ifndef USE_CXX_FILESTREAMS
#define USE_CXX_FILESTREAMS false
#endif

int writeToBMP(BMPFile* bmp, const char* filename) {
    FILE* f = __fopen(filename, "w");
    if (f != NULL) {
        bmp->write(f);
        fclose(f);
    }
    else {
        reportError("An error occured when writing a bitmap to file %s: ", filename);
    }
    return 0;
};

int main(int argc, char** argv) {
    srand((unsigned)time(NULL));
    startTiming();

    

    std::vector<std::vector<BMPColour> > matrix;
    BMPFile bmp(256, 256);
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

    char filenameBuf[512];
    printf("Enter a filename: ");
    std::cin.getline(filenameBuf, 512);

    if (strlen(filenameBuf) == 0) writeToBMP(&bmp, "test2.bmp");
    else writeToBMP(&bmp, filenameBuf);

    customLog("Finished!");
    #ifdef _MSC_VER
    printf("MSVC++ users: Press RETURN to finish...");
    std::cin.get();
    #endif

    return 0;
};
