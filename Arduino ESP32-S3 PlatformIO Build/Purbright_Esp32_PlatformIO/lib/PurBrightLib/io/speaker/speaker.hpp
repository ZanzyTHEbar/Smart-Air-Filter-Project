#pragma once
#ifndef SPEAKER_hpp
#define SPEAKER_hpp
#include "defines.hpp"

class Speaker
{
public:
    // Functions
    Speaker();
    virtual ~Speaker(void);
    // Variables
private:
    const byte buzzer_pin;
    const char* _itchy;
};

extern Speaker speaker;
#endif