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
    printf("\nUsage:\n %s [-df] [-o <FILENAME>] [-w <WIDTH>] [-h <HEIGHT>] [-z <ZOOM>]\n", exeName);
    printf(" %s [--help]\n\n", exeName);
    printf("%s", copyright);
    exit(EXIT_FAILURE);
};

int writeToBMP(BMPFile* bmp, const char* filename, bool force = false) {
    if (!force) {
        FILE* existTest = __fopen(filename, "r");
        if (existTest != NULL) {
            printf("This file (%s) already exists. Are you sure you want to overwrite this file? (Y/N): ", filename);
            char result[2];
            std::cin.getline(&result[0], 2);
            fclose(existTest);

            if (result[0] == 'N' || result[0] == 'n') return ECANCELED;
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
    std::vector<std::vector<BMPColour> > matrix;
    FractalGen::FractalSettings fracset;

    // declaring the variables that can be set with CLI arguments
    // arranging stuff into columns is so satisfying
    char*    filename     = __strdup("fractal.bmp");
    bool     forceWriting = false;
    bool     debug        = false;
    unsigned width        = 512;
    unsigned height       = 512;
    double   xoffset      = fracset.pan_x;
    double   yoffset      = fracset.pan_y;
    double   zoom         = -0.5;

    #ifdef _GETOPT_H
    // the CLI long options
    struct option opts[9] = {
            // long option, using a parameter?, flags, short option
            {"help",    no_argument,       NULL, '\0'},
            {"output",  required_argument, NULL,  'o'},
            {"force",   no_argument,       NULL,  'f'},
            {"debug",   no_argument,       NULL,  'd'},
            {"width",   required_argument, NULL,  'w'},
            {"height",  required_argument, NULL,  'h'},
            {"zoom",    required_argument, NULL,  'z'},
            {"xoffset", required_argument, NULL,  'x'},
            {"yoffset", required_argument, NULL,  'y'}
    };

    int c;
    while ((c = getopt_long(argc, argv, "o:w:h:x:y:z:fd", opts, NULL)) != EOF) { // keep going until we don't have any more options
        switch (c) {
            case 'o': filename     = strdup(optarg);         break;
            case 'f': forceWriting = true;                   break;
            case 'd': debug        = true;                   break;
            case 'w': width        = (unsigned)atoi(optarg); break;
            case 'h': height       = (unsigned)atoi(optarg); break;
            case 'x': xoffset      = atof(optarg);           break;
            case 'y': yoffset      = atof(optarg);           break;
            case 'z': zoom         = atof(optarg);           break;
            default: bailUsage(argv[0]);
        }
    }
    #endif

    startTiming(debug);

    fracset.query_x = 1/pow(10, zoom);
    fracset.query_y = 1/pow(10, zoom);
    fracset.pan_x = xoffset;
    fracset.pan_y = yoffset;
    BMPFile bmp(width, height);

    customLog("Declared new BMP file (%i x %i)", bmp.width, bmp.height);
    customLog("Generating new Mandelbrot Set\n(%f x %f, offset at %f, %f)...", 
        fracset.query_x,
        fracset.query_y,
        fracset.pan_x,
        fracset.pan_y);

    matrix = FractalGen::calculateMandelbrot<BMPColour>((unsigned)bmp.width, (unsigned)bmp.height, fracset);

    customLog("Generated fractal! Now assigning to bitmap...");
    bmp.setBitmap(matrix, true);

    customLog("Assigned fractal to bitmap. Writing to file...");
    int res = writeToBMP(&bmp, filename, forceWriting);

    if (res == 0) customLog("Finished!");
    #ifdef _MSC_VER
    printf("MSVC++ users: Press RETURN to finish...");
    std::cin.get();
    #endif

    return 0;
};
