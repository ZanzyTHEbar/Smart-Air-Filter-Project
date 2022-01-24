#pragma once
#ifndef PIR_hpp
#define PIR_hpp
#include "defines.hpp"

class PIR
{
public:
    // Functions
    PIR();
    virtual ~PIR(void);
    int SetupPIR();
    void SetupMainLoop();
    void InitPIRJson();
private:
};
#endif