
#include "PIR.hpp"

PIR::PIR()
{
    _motion_sensor_pin = 21;
}

PIR::~PIR()
{
}

void PIR::SetupPIR()
{
    pinMode(_motion_sensor_pin, INPUT);
}

void PIR::run()
{
    // Motion indication
    if (!S_OnOff)
    {
        strip.setPixelColor(0, strip.Color(233, 225, 85));
        strip.show();
    }
    else
    {
        if (digitalRead(_motion_sensor_pin))
        {
            strip.setPixelColor(0, strip.Color(0, 255, 0));
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