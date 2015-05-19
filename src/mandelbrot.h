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
    const double PLANE_BOUNDARY = 100;
    const long   ITERATION_LIMIT = 600;

    const double HUE_SPREAD = (double)1/4;
    const long   HUE_OFFSET = 190;
    const double HSV_SATURATION = 1.0;
    const double HSV_VALUE = 0.5;

    const double FACTOR = 170;
    const double PAN_X = -0.8;
    const double PAN_Y = -0.8;

    typedef std::complex<double> complex_t;

    bool isBounded(complex_t cmp) {
        if (isinf(cmp.real()) || isinf(cmp.imag()) || isnan(cmp.real()) || isnan(cmp.imag())) return false;
        return fabs(cmp.imag()) < PLANE_BOUNDARY || fabs(cmp.real()) < PLANE_BOUNDARY;
    };

    long testComplexJuliaFate(const complex_t juliaOffset) {
        complex_t fate(0, 0);
        for (unsigned long i=0;i<ITERATION_LIMIT;i++) {
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
        std::vector<std::vector<Colour3LT> > grid;

        for (unsigned i=0;i<height;i++) {
            std::vector<Colour3LT> row;
            for (unsigned j=0;j<width;j++) {
                long result = testComplexJuliaFate(complex_t(j/FACTOR + PAN_X, i/FACTOR + PAN_Y));
                if (result == -1) row.push_back(Colour3LT(0, 0, 0));
                else {
                    int hue = int(long(result * HUE_SPREAD) + HUE_OFFSET % 360);

                    row.push_back(hsvToRgb<Colour3LT>(hue, HSV_SATURATION, HSV_VALUE));
                }
            };
            grid.push_back(row);
        };

        return grid;
    };
};

#endif //WRITE_TO_BMP_MANDLEBROT_H
