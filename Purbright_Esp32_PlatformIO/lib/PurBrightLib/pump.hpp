#pragma once
#ifndef PUMP_hpp
#define PUMP_hpp
#include "defines.hpp"

class Pump
{
public:
    // Functions
    Pump();
    virtual ~Pump(void);
    void SetupPump();
    void SetupMainLoop();
    void InitPumpJson();
    void checkTurnOffPump();
    void Man_Aut();
    void Manual();
    void Automatic();
    
private:
};
#endif