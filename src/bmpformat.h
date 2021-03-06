#ifndef WRITE_TO_BMP_BMPFORMAT_H
#define WRITE_TO_BMP_BMPFORMAT_H

#include <cmath>
#include <cstdio>
#include <cinttypes>
#include <vector>

#include "crosscrtrepl.h"

#ifndef min
template <typename T>
T min(T a, T b) { return a < b ? a : b; }
#endif

#ifndef max
template <typename T>
T max(T a, T b) { return a > b ? a : b; }
#endif

#ifdef _MSC_VER
#define PACK_STRUCT
#else
#define PACK_STRUCT __attribute__((__packed__))
#endif

// Header about a BMP file's information/metadata.
struct BMPInfoHeader {
    uint32_t size;                // Size of the info header in bytes
    uint32_t width;               // Width  of the image in pixels
    uint32_t height;              // Height    .    .    .    .
    uint16_t planes = 1;          // Number of pixel planes (1 for 24-bit colour)
    uint16_t bitCount = 24;       // Bits per colour (24 bit colour, like HTML colour codes)
    uint32_t compression = 0;     // Type of compression (0 for 24-bit colour)
    uint32_t sizeImage = 0;       // Size of the image data (automatic for 24-bit colour)
    uint32_t xPxPerMetre = 3779;  // Pixels in the horizontal direction per metre
    uint32_t yPxPerMetre = 3779;  //   .   .   .    vertical direction   .   .
    uint32_t coloursUsed = 0;     // Number of colours in the palette (no palette for 24-bit colour)
    uint32_t importantColour = 0; // The "important colour" in the palette (none for 24-bit colour)
    BMPInfoHeader() {
        size = sizeof(BMPInfoHeader);
    };
    /**
     * This function writes the contents of a BMPInfoHeader to a specified
     * C I/O FILE* stream.
     * @param f The C FILE pointer object.
     */
    void write(FILE* f) {
        fwrite(&size, sizeof(uint32_t), 3, f);
        fwrite(&planes, sizeof(uint16_t), 2, f);
        fwrite(&compression, sizeof(uint32_t), 6, f);
    }
};

#ifdef _MSC_VER
#include <pshpack2.h>
#endif

// The BMP file's main header.
struct BMPFileHeader {
    uint8_t type[2];      // Magic number, always "BM"
    uint32_t size;        // Size of the file, in bytes
    uint16_t reserved[2]; // Reserved fields, have to be zero
    uint32_t offBits;     // Offset from the beginning of the file to the bitmap data
    BMPFileHeader() {
        type[0] = 'B';
        type[1] = 'M';
        reserved[0] = 0;
        reserved[1] = 0;
        offBits = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader);
    };
    /**
     * This function writes the contents of a BMPFileHeader to a specified
     * C I/O FILE* stream.
     * @param f The C FILE pointer object.
     */
    void write(FILE* f) {
        fwrite(&type[0], sizeof(uint8_t), 2, f);
        fwrite(&size, sizeof(uint32_t), 3, f);
    }
} PACK_STRUCT; // To prevent the padding of the type field to four bytes instead of two.

#ifdef _MSC_VER
#include <poppack.h>
#endif

// A class for representation of 24-bit colour in BMP image files.
struct BMPColour {
    uint8_t blue;  // BGR byte order, very important
    uint8_t green;
    uint8_t red;
    BMPColour(const uint8_t red, const uint8_t green, const uint8_t blue) {
        BMPColour::red = red;
        BMPColour::green = green;
        BMPColour::blue = blue;
    }
};

// The full file object, in all its glory.
class BMPFile {
public:
    BMPFileHeader head;  // File header
    BMPInfoHeader info;  // Info header
    uint8_t* bitmapData; // Raw bitmap data, as written to the file

    size_t width;     // Width of the bitmap, in pixels
    size_t height;    // Height of the bitmap, in pixels
    size_t rowLength; // Length of each bitmap row, in bytes (width * sizeof(BMPColour) + padding to fit mod 4 == 0)
    BMPFile(const size_t width, const size_t height) {
        BMPFile::width = info.width = (uint32_t)width;    // Assigning class and header values
        BMPFile::height = info.height = (uint32_t)height;

        rowLength = (size_t)ceil((float)(width * sizeof(BMPColour)) / 4) * 4; // Row length calculation
        bitmapData = new uint8_t[rowLength*height]; // Allocating memory for the bitmap

        head.size = uint32_t(sizeof(head) + sizeof(info) + rowLength * height); // Calculating file size
    };
    /**
     * Assigns a new bitmap row to the BMPFile from a std::vector of BMPColours.
     * @param rowNum The index of the row you want to assign
     * @param colours A std::vector of BMPColours that represents the pixel values of that row
     * @param reverse Whether or not to treat the index as from top to bottom (true) or from bottom to top, like stored in the BMP file (false)
     */
    void assignRow(const size_t rowNum, std::vector<BMPColour> colours, bool reverse = false) {
        size_t row = min(rowNum, height);
        if (reverse) row = (height - 1) - row;
        const size_t copynum = min(colours.size() * sizeof(BMPColour), rowLength);
        __memcpy(&bitmapData[min(row, height) * rowLength], colours.data(), copynum);
    };
    /**
     * Assigns a full bitmap using a two-dimensional std::vector of BMPColours.
     * Bounds checking is automatically done in assignRow().
     * @param colourMatrix A vector of bitmap rows, which are themselves vectors of BMPColours
     */
    void setBitmap(std::vector<std::vector<BMPColour> > colourMatrix, bool reverse = false) {
        for (unsigned i=0;i<min(colourMatrix.size(), height);i++) assignRow(i, colourMatrix[i], reverse);
    };
    /**
     * Writes a BMPFile to a given C I/O FILE* stream.
     * @param f The FILE* stream that the file will be written to.
     */
    void write(FILE* f) {
        head.write(f);
        info.write(f);
        fwrite(&bitmapData[0], sizeof(uint8_t), rowLength*height, f);
    };
    ~BMPFile() { delete[] bitmapData; };
};

#endif