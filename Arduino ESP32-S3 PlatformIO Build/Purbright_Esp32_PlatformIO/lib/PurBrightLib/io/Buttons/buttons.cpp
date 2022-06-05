#include "buttons.hpp"

Buttons::Buttons()
{
}

Buttons::~Buttons()
{
    // Nothing to do here
}

void Buttons::SetupButtons()
{
    pinMode(TouchOnOff, INPUT);
    pinMode(TouchManAut, INPUT);
    pinMode(TouchPlus, INPUT);
    pinMode(TouchMinus, INPUT);
}

void Buttons::ButtonLoop()
{
    // Button Check
    if (!digitalRead(TouchOnOff) && TouchOnOffOld)
    {
        TouchOnOffPN = 1;
    }
    else
    {
        TouchOnOffPN = 0;
    }

    TouchOnOffOld = digitalRead(TouchOnOff);

    if (TouchOnOffPN)
    {
        OnOff();
    }

    if (S_OnOff)
    {
        if (!digitalRead(TouchManAut) && TouchManAutOld)
        {
            TouchManAutPN = 1;
        }
        else
        {
            TouchManAutPN = 0;
        }

        TouchManAutOld = digitalRead(TouchManAut);

        if (!digitalRead(TouchPlus) && TouchPlusOld)
        {
            TouchPlusPN = 1;
        }
        else
        {
            TouchPlusPN = 0;
        }

        TouchPlusOld = digitalRead(TouchPlus);

        if (!digitalRead(TouchMinus) && TouchMinusOld)
        {
            TouchMinusPN = 1;
        }
        else
        {
            TouchMinusPN = 0;
        }

        TouchMinusOld = digitalRead(TouchMinus);

        /*  ------------------------------------------------- */

        if (!S_Menu && TouchManAutPN)
        {
            pump.Man_Aut();
        }
        if (!S_Menu && TouchPlusPN)
        {
            neopixel.Settings();
        }
        if (S_Menu && TouchPlusPN)
        {
            neopixel.Plus();
        }
        if (S_Menu && TouchMinusPN)
        {
            neopixel.Settings();
        }
        if (S_Menu && TouchMinusPN)
        {
            neopixel.Minus();
        }
    }
}

// All Off
void Buttons::All_Off()
{
    Relay.RelayOnOff(pump._pump_relay_pin, false);
    S_ManAut = false;
    Step_Manual = 10;
    Step_Automatic = 20;
    S_Menu = false;
    Step_Menu = 40;
    neopixel.colorWipeAll(strip.Color(0, 0, 0), 50); // Off
}

// OnOff Button Code
void Buttons::OnOff()
{
    if (S_OnOff == 0)
    {
        S_OnOff = 1;
        neopixel.colorWipeAll(strip.Color(0, 0, 127), 50); // Soft Blue
        S_ManAut = true;
    }
    else
    {
        S_OnOff = false;
        All_Off();
    }
}

Buttons buttons;