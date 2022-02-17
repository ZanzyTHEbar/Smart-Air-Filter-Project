#pragma once
#ifndef PUMP_hpp
#define PUMP_hpp
#include "defines.hpp"

class Pump
{
public:
    // Functions
    Pump(void);
    virtual ~Pump(void);
    void SetupPump();
    void PumpLoop();
    void checkTurnOffPump();
    void Man_Aut();
    void Manual();
    void Automatic();

private:
};

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_PUMP)
extern Pump pump;
#endif
#endif