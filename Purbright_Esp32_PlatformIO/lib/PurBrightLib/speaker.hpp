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
    void SetupSpeaker();
    void InitSpeakersJson();
    // Variables
    String json;
    int maxVoltage;
    int maxTemp;
private:
};
#endif