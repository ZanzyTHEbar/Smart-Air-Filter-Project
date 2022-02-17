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

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_PIR)
extern PIR pir;
#endif
#endif