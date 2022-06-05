#pragma once
#ifndef PIR_hpp
#define PIR_hpp
#include <defines.hpp>

class PIR
{
public:
    // Functions
    PIR();
    virtual ~PIR(void);
    void run();
    void SetupPIR();

private:
    byte _motion_sensor_pin;
};

extern PIR pir;
#endif