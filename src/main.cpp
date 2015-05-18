#include "main.h"
#include "util.h"

#ifdef _MSC_VER
#define PACK_STRUCT
#else
#define PACK_STRUCT __attribute__((__packed__))
#endif

#ifdef _MSC_VER
#include <pshpack2.h>
#endif

struct BMPFileHeader {
    uint8_t type[2];
    uint32_t size;
    uint16_t reserved[2];
    uint32_t offBits;
    BMPFileHeader();
    void write(FILE* f) {
        fwrite(&type[0], sizeof(uint8_t), 2, f);
        fwrite(&size, sizeof(uint32_t), 3, f);
    }
} PACK_STRUCT;

#ifdef _MSC_VER
#include <poppack.h>
#endif

struct BMPInfoHeader {
    uint32_t size;
    uint32_t width;
    uint32_t height;
    uint16_t planes = 1;
    uint16_t bitCount = 24;
    uint32_t compression = 0;
    uint32_t sizeImage = 0;
    uint32_t xPxPerMetre = 3779;
    uint32_t yPxPerMetre = 3779;
    uint32_t coloursUsed = 0;
    uint32_t importantColour = 0;
    BMPInfoHeader();
    void write(FILE* f) {
        fwrite(&size, sizeof(uint32_t), 3, f);
        fwrite(&planes, sizeof(uint16_t), 2, f);
        fwrite(&compression, sizeof(uint32_t), 6, f);
    }
};
struct BMPColour {
    uint8_t blue;
    uint8_t green;
    uint8_t red;
    BMPColour() {
        blue = 0;
        green = 0;
        red = 0;
    }
    BMPColour(const uint8_t red, const uint8_t green, const uint8_t blue) {
        BMPColour::red = red;
        BMPColour::green = green;
        BMPColour::blue = blue;
    }
};
struct BMPFile {
    BMPFileHeader head;
    BMPInfoHeader info;
    uint8_t* bitmapData;
    BMPFile(const int width, const int height) {
        const size_t adjusted = (size_t)ceil((float)(width * sizeof(BMPColour)) / 4) * 4;
        bitmapData = new uint8_t[adjusted*height];
    };
    ~BMPFile() { delete[] bitmapData; };
};
BMPFileHeader::BMPFileHeader() {
    type[0] = 'B';
    type[1] = 'M';
    reserved[0] = 0;
    reserved[1] = 0;
    offBits = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader);
};
BMPInfoHeader::BMPInfoHeader() {
    size = sizeof(BMPInfoHeader);
};

static void* __memcpy(void* dest, const void* src, const size_t srcCount) {
    #ifdef _MSC_VER
    memcpy_s(dest, srcCount, src, srcCount);
    return (void*)0;
    #else
    return memcpy(dest, src, srcCount);
    #endif
};
static FILE* __fopen(const char* filename, const char* restrictions) {
    #ifdef _MSC_VER
    FILE* f;
    fopen_s(&f, filename, restrictions);
    return f;
    #else
    return fopen(filename, restrictions);
    #endif
};

int writeToBMP(const uint32_t width, const uint32_t height) {
    BMPFile bmp = BMPFile(width, height);
    bmp.info.width = width;
    bmp.info.height = height;

    const size_t rowLength = (size_t)ceil((float)(width * sizeof(BMPColour)) / 4) * 4;
    bmp.head.size = uint32_t(sizeof(BMPFileHeader) + sizeof(BMPInfoHeader) + rowLength * height);

    for (int i = 0; i < height; i++) {
        std::vector<BMPColour> colours;
        for (int j = 0; j < width; j++)
            colours.push_back(BMPColour(uint8_t(rand() % 255),
                                        uint8_t(rand() % 255),
                                        uint8_t(rand() % 255)
            ));
        __memcpy(&bmp.bitmapData[i*rowLength],
                 colours.data(),
                 rowLength);
    };

    cout << sizeof(bmp.head) << endl;
    for (int i = 0; i < sizeof(bmp.head); i++) {
        uint8_t* ptr = (uint8_t*)&bmp.head + i*sizeof(uint8_t);
        printf("0x%x\n", *ptr);
    }

    FILE* f = __fopen("test.bmp", "w");
    if (f != NULL) {
        bmp.head.write(f);
        bmp.info.write(f);
        fwrite(&bmp.bitmapData[0], sizeof(uint8_t), rowLength * height, f);
        fclose(f);
    }
    /*std::fstream f;
    f.open("test.bmp", std::ios::out);
    f.write(reinterpret_cast<char*>(&bmp.head), sizeof(bmp.head));
    f.write(reinterpret_cast<char*>(&bmp.info), sizeof(bmp.info));
    f.write(reinterpret_cast<char*>(&bmp.bitmapData), rowLength * height);
    f.close();*/

    return 0;
};

int main(int argc, char** argv) {
    srand((unsigned int)time(NULL));
    startTiming();

    writeToBMP(255, 255);

    return 0;
}
