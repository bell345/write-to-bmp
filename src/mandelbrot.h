//
// Created by thomas on 2015-05-19.
//

#ifndef WRITE_TO_BMP_MANDLEBROT_H
#define WRITE_TO_BMP_MANDLEBROT_H

// There is no mandlebrot.cpp because of the template functions - they *really* don't like being in a source file.
// For real: they need to be seen by a translation unit in order to deduce that "Yes, this type will work fine
// in this function" and so to achieve that, they need to be put in a header file.
//
// This is a note for Future Me. Use it wisely.
// 2015-05-19 21:31:44

#include <complex>
#include <vector>

namespace FractalGen {
    struct FractalSettings {
        double plane_boundary = 600;  // the max size of a complex number term until its considered "out of bounds" and escaping to infinity
        long   iteration_limit = 600; // how many times we should test the number for going out of bounds

        double hue_spread = 1.0/2.1;  // how intense the banding of hue should be (larger = more psychedelic)
        long   hue_offset = 140;      // initial hue value (1 try until escape = this hue)
        double hsv_saturation = 1.0;  // saturation of the colours (from HSV colour space)
        double hsv_value = 0.8;       // value of the colours (from HSV colour space)

        double query_x = 2;           // how wide the complex plane to test will be
        double query_y = 2;           // how high the complex plane to test will be
        double pan_x = -0.75;         // x location of the centre of the image on the complex plane
        double pan_y = 0;             // y location of the centre of the image on the complex plane
    };

    typedef std::complex<double> complex_t; // std::complex expects a subtype, so typedef'ing our default as double

    /**
     * Tests if a given complex number is out of bounds (one or more of its terms have escaped to infinity).
     * Uses an arbitrary fixed bound in fracset.
     * @param cmp The complex number to test.
     */
    bool isBounded(complex_t cmp, const double planeBoundary) {
        if (isinf(cmp.real()) || isinf(cmp.imag()) || isnan(cmp.real()) || isnan(cmp.imag())) return false;
        return fabs(cmp.imag()) < planeBoundary || fabs(cmp.real()) < planeBoundary;
    };

    /**
     * Returns how many iterations of the function f(z) = z^2 + a (where a is a given complex number)
     * it takes for (0 + 0i) to escape to infinity (i.e. escape past an arbitrary boundary limit)
     *
     * Returns -1 if the number doesn't escape to infinity within an arbitary iteration limit.
     * @param juliaOffset The complex number a in f(z) = z^2 + a; corresponds to its own Julia set and pixel of the Mandelbrot set.
     */
    long testComplexJuliaFate(const complex_t juliaOffset, const long iterationLimit, const double planeBoundary) {
        complex_t fate(0, 0);
        for (unsigned long i=0;i<iterationLimit;i++) {
            fate = fate * fate + juliaOffset;
            if (!isBounded(fate, planeBoundary)) return i;
        }
        return -1;
    };

    /**
     * Converts a triple of hue, saturation and value variables to their corresponding RGB values (all [0-255]), encapsulated
     * in the given Colour3LT subtype.
     * @param h The hue of the colour (what colour it is). A short integer ranging from [0-360) degrees around the colour wheel.
     * @param s The saturation of the colour (what effect does the hue have on the colour). A float in the interval [0-1].
     * @param v The value of the colour (how much of the colour is there compared to zero). A float in the interval [0-1].
     */
    template <typename Colour3LT>
    Colour3LT hsvToRgb(const short h, const double s, const double v) {
        double c = s*v; // chroma value
        double x = c*(1 - fabs(fmod(h/60.0, 2) - 1)); // intermediary value between 2 of the 6 hue divisions
        double o = v-c; // offset from zero, brightness

        c = 255*(c + o); // adding offsets and standardising to 8 bit int (0-255)
        x = 255*(x + o);
        switch (h/60) {
            case 0: return Colour3LT(c, x, o);
            case 1: return Colour3LT(x, c, o);
            case 2: return Colour3LT(o, c, x);
            case 3: return Colour3LT(o, x, c);
            case 4: return Colour3LT(x, o, c);
            case 5: return Colour3LT(c, o, x);
            default: return Colour3LT(o, o, o);
        };
    };

    /**
     * Returns a bitmap (2D std::vector of a given RGB colour type) image of the given width and height
     * represeting a section of the Mandelbrot set fractal defined by the given FractalSettings struct.
     * @param width The size of the image horizontally, in pixels
     * @param height The size of the image vertically, in pixels
     * @param fracset A FractalSettings struct that is used to specify the fractal.
     */
    template <typename Colour3LT>
    std::vector<std::vector<Colour3LT> > calculateMandelbrot(const unsigned width, const unsigned height, const FractalSettings& fracset) {
        std::vector<std::vector<Colour3LT> > grid(height, std::vector<Colour3LT>(width, Colour3LT(0, 0, 0)));

        double start = sinceStart(), lastTime = sinceStart();
        double fpanX = fracset.pan_x - (fracset.query_x / 2); // pan values for the top-left corner, adjusted from the centre
        double fpanY = fracset.pan_y - (fracset.query_y / 2);
        const int timerLimit = 100;

        #pragma omp parallel for
        for (unsigned y = 0; y < height; y++) {
            std::vector<Colour3LT> row; // one row at a time
            for (unsigned x = 0; x < width; x++) {
                long result = testComplexJuliaFate(complex_t(
                        ((double)x / width)  * fracset.query_x + fpanX,  // get complex number from bitmap coordinates and pan + scale settings
                        ((double)y / height) * fracset.query_y + fpanY),
                fracset.iteration_limit, fracset.plane_boundary);

                if (result == -1) row.push_back(Colour3LT(0, 0, 0));    // if it hasn't escaped the boundary, give the pixel a black colour value
                else row.push_back(hsvToRgb<Colour3LT>(                 // otherwise, give the pixel a value based upon how many tries it took to escape the boundary
                            short(360 - long(                           // reverse to go from blue -> green -> red instead of blue -> purple -> red
                                    (double)result * fracset.hue_spread // intensify/quiet the hue
                                    + fracset.hue_offset                // offset starting colour
                            ) % 360),
                            fracset.hsv_saturation,
                            fracset.hsv_value));

                // query progress (don't work too good now because parallelisation)
                if (sinceStart() - lastTime > timerLimit) { // query only after a given interval
                    lastTime = sinceStart();

                    double progress = ((double)y / height) + ((double)x / (width*height));
                    double duration = sinceStart() - start;
                    fullWidthLogWithReturn("[%3i%%] Last result: %-3i; %2i seconds remaining...",
                                           int(progress * 100), result, int((duration / progress) * (1 - progress)) / 1000);
                }
            }
            if (y < grid.size()) grid.at(y) = row; // since .push_back is bad with threads, direct access and a fixed std::vector size is necessary
        }

        fullWidthLogWithReturn("[100%%] Complete");
        printf("\n");

        return grid;
    };
};

#endif //WRITE_TO_BMP_MANDLEBROT_H
