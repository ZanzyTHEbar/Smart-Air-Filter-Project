
#include "PIR.hpp"

const uint8_t MotionSensor = 21;

PIR::PIR()
{
    
}

PIR::~PIR()
{
}

int PIR::SetupPIR()
{
    pinMode(MotionSensor, INPUT);
    return 1;
}

void PIR::SetupMainLoop()
{
    // Motion indication
    if ((digitalRead(MotionSensor) == true) && (S_OnOff == true))
    {
        strip.setPixelColor(0, strip.Color(0, 255, 0));
        strip.show();
    }
    else if (S_OnOff == true)
    {
        if (S_ManAut == 0)
        {
            strip.setPixelColor(0, strip.Color(233, 225, 85));
            strip.show();
        }
        else
        {
            strip.setPixelColor(0, strip.Color(0, 0, 127));
            strip.show();
        }
    }
}

PIR pir;

/*********************************************************
 * 
 * DELETE THIS AND ADD TO THE PUMP CLASS
 * 
 * *******************************************************/