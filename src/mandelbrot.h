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
        double plane_boundary = 600;
        long   iteration_limit = 600;

        double hue_spread = 1.0/2.1;
        long   hue_offset = 140;
        double hsv_saturation = 1.0;
        double hsv_value = 0.8;

        double query_x = 2;
        double query_y = 2;
        double pan_x = -0.75;
        double pan_y = 0;
    } fracset;

    typedef std::complex<double> complex_t;

    bool isBounded(complex_t cmp) {
        if (isinf(cmp.real()) || isinf(cmp.imag()) || isnan(cmp.real()) || isnan(cmp.imag())) return false;
        return fabs(cmp.imag()) < fracset.plane_boundary || fabs(cmp.real()) < fracset.plane_boundary;
    };

    long testComplexJuliaFate(const complex_t juliaOffset) {
        complex_t fate(0, 0);
        for (unsigned long i=0;i<fracset.iteration_limit;i++) {
            fate = fate * fate + juliaOffset;
            if (!isBounded(fate)) return i;
        }
        return -1;
    };

    template <typename Colour3LT>
    Colour3LT hsvToRgb(const long h, const double s, const double v) {
        double c = s*v;
        double x = c*(1 - fabs(fmod(h/60.0, 2) - 1));
        double o = v-c;

        c = 255*(c + o);
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

    template <typename Colour3LT>
    std::vector<std::vector<Colour3LT> > calculateMandelbrot(const unsigned width, const unsigned height) {
        std::vector<std::vector<Colour3LT> > grid(height, std::vector<Colour3LT>(width, Colour3LT(0, 0, 0)));

        double start = sinceStart(), lastTime = sinceStart();
        double fpanX = fracset.pan_x - (fracset.query_x / 2);
        double fpanY = fracset.pan_y - (fracset.query_y / 2);
        const int timerLimit = 20;

        #pragma omp parallel for
        for (unsigned y = 0; y < height; y++) {
            std::vector<Colour3LT> row;
            for (unsigned x = 0; x < width; x++) {
                long result = testComplexJuliaFate(complex_t(
                        ((double)x / width)  * fracset.query_x + fpanX,
                        ((double)y / height) * fracset.query_y + fpanY));

                if (result == -1) row.push_back(Colour3LT(0, 0, 0));
                else row.push_back(hsvToRgb<Colour3LT>(
                            int(360 - long((double)result * fracset.hue_spread + fracset.hue_offset) % 360),
                            fracset.hsv_saturation,
                            fracset.hsv_value));
                if (sinceStart() - lastTime > timerLimit) {
                    lastTime = sinceStart();

                    double progress = ((double)y / height) + ((double)x / (width*height));
                    double duration = sinceStart() - start;
                    fullWidthLogWithReturn("[%3i%%] Last result: %-3i; %2i seconds remaining...",
                                           int(progress * 100), result, int((duration / progress) * (1 - progress)) / 1000);
                }
            }
            if (y < grid.size()) grid.at(y) = row;
        }

        fullWidthLogWithReturn("[100%%] Complete");
        printf("\n");

        return grid;
    };
};

#endif //WRITE_TO_BMP_MANDLEBROT_H
