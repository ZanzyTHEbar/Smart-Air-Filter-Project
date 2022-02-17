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
};

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_SPEAKER)
extern Speaker speaker;
#endif

#endif