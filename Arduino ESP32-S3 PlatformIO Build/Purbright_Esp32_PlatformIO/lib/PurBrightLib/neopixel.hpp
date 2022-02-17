#pragma once
#ifndef NEOPIXEL_hpp
#define NEOPIXEL_hpp
#include "defines.hpp"

class Neopixel
{
public:
    // Functions
    Neopixel();
    virtual ~Neopixel(void);
    void SetupNeopixel();
    void colorWipe(uint32_t c, uint8_t wait);
    void colorWipeAll(uint32_t c, uint8_t wait);
    void Settings();
    void Plus();
    void Minus();
    
private:
};

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_NEOPIXEL)
extern Neopixel neopixel;
#endif
#endif