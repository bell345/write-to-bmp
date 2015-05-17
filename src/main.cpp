#include "main.h"
#include "util.h"

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
};

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

int writeToBMP(const int width, const int height) {
    BMPFile bmp = BMPFile(width, height);
    bmp.info.width = width;
    bmp.info.height = height;

    const size_t rowLength = (size_t)ceil((float)(width * sizeof(BMPColour)) / 4) * 4;
    bmp.head.size = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader) + rowLength * height;

    for (int i = 0; i < height; i++) {
        std::vector<BMPColour> colours;
        for (int j = 0; j < width; j++)
            colours.push_back(BMPColour(rand() % 255, rand() % 255, rand() % 255));
        memcpy_s(&bmp.bitmapData[i*rowLength], colours.size()*sizeof(BMPColour), colours.data(), colours.size()*sizeof(BMPColour));
    };

    cout << sizeof(bmp.head) << endl;
    for (int i = 0; i < sizeof(bmp.head); i++) {
        uint8_t* ptr = (uint8_t*)&bmp.head + i*sizeof(uint8_t);
        printf("0x%x\n", *ptr);
    }

    /*FILE* f;
    fopen_s(&f, "test.bmp", "w");
    if (f != NULL) {
        bmp.head.write(f);
        bmp.info.write(f);
        cout << rowLength * height << endl;
        fwrite(&bmp.bitmapData, sizeof(uint8_t), rowLength * height, f);
        fclose(f);
    }*/
    std::fstream f;
    f.open("test.bmp", std::ios::out);
    f.write(reinterpret_cast<char*>(&bmp.head), sizeof(bmp.head));
    f.write(reinterpret_cast<char*>(&bmp.info), sizeof(bmp.info));
    f.write(reinterpret_cast<char*>(&bmp.bitmapData), rowLength * height);
    f.close();

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
    fopen_s(&file, fileName, restrictions);
    if (file == NULL) {
        reportError("The file %s could not be opened due to", fileName);
        return file;
    }
    return file;
}

int main(int argc, char** argv) {
    startTiming();

    writeToBMP(20, 20);

    return 0;
}