#include "writer.h"
#include "logutil.h"
#include "crosscrtrepl.h"
#include "bmpformat.h"

#ifndef USE_CXX_FILESTREAMS
#define USE_CXX_FILESTREAMS false
#endif

int writeToBMP(const uint32_t width, const uint32_t height) {
    customLog("Writing new BMP file of dimensions %i x %i", width, height);
    BMPFile bmp = BMPFile(width, height);
    bmp.info.width = width;
    bmp.info.height = height;

    const size_t rowLength = (size_t)ceil((float)(width * sizeof(BMPColour)) / 4) * 4;
    bmp.head.size = uint32_t(sizeof(BMPFileHeader) + sizeof(BMPInfoHeader) + rowLength * height);

    for (unsigned i = 0; i < height; i++) {
        std::vector<BMPColour> colours;
        for (unsigned j = 0; j < width; j++)
            colours.push_back(BMPColour(uint8_t(rand() % 255),
                                        uint8_t(rand() % 255),
                                        uint8_t(rand() % 255)
            ));
        __memcpy(&bmp.bitmapData[i*rowLength],
                 colours.data(),
                 rowLength);
    };

    if (!USE_CXX_FILESTREAMS) {
        FILE* f = __fopen("test.bmp", "w");
        if (f != NULL) {
            bmp.head.write(f);
            bmp.info.write(f);
            fwrite(&bmp.bitmapData[0], sizeof(uint8_t), rowLength * height, f);
            fclose(f);
        }
    } else {
        std::fstream f;
        f.open("test.bmp", std::ios::out);
        f.write(reinterpret_cast<char*>(&bmp.head), sizeof(bmp.head));
        f.write(reinterpret_cast<char*>(&bmp.info), sizeof(bmp.info));
        f.write(reinterpret_cast<char*>(&bmp.bitmapData[0]), rowLength * height);
        f.close();
    }

    return 0;
};

int main(int argc, char** argv) {
    srand((unsigned)time(NULL));
    startTiming();

    writeToBMP(255, 255);
    customLog("Finished!");
    printf("Press any key to continue...");
    std::cin.get();

    return 0;
};
