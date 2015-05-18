#ifndef WRITE_TO_BMP_BMPFORMAT_H
#define WRITE_TO_BMP_BMPFORMAT_H

#include <math.h>
#include <stdio.h>
#include <cinttypes>

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

#endif