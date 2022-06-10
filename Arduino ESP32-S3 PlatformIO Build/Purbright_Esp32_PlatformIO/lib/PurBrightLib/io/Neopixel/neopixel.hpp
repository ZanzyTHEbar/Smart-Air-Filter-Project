#pragma once
#ifndef NEOPIXEL_hpp
#define NEOPIXEL_hpp
#include <defines.hpp>

namespace AirFilter
{
    class Neopixel
    {
    public:
        // Functions
        Neopixel();
        virtual ~Neopixel();
        void begin();
        void colorWipe(uint32_t c, uint8_t wait);
        void colorWipeAll(uint32_t c, uint8_t wait);
        void Settings();
        void Plus();
        void Minus();

    private:
    };

    extern Neopixel neopixel;
}
#endif