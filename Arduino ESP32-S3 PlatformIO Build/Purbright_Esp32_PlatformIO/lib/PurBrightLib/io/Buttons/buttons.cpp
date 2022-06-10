#include "buttons.hpp"

namespace AirFilter
{
    _Button_states _button_states_t = Buttons_OFF;

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

    void Buttons::ReadPowerButton()
    {
        // Button Check
        if (!digitalRead(TouchOnOff) && TouchOnOffOld)
        {
            TouchOnOffPN = 1;
            _button_states_t = Buttons_ON;
        }
        else
        {
            TouchOnOffPN = 0;
            _button_states_t = Buttons_OFF;
        }

        TouchOnOffOld = digitalRead(TouchOnOff);

        if (TouchOnOffPN)
        {
            _button_states_t = OnOff();
        }
    }

    void Buttons::ReadPlusButton()
    {
        // Button Check
        if (!digitalRead(TouchPlus) && TouchPlusOld)
        {
            TouchPlusPN = 1;
        }
        else
        {
            TouchPlusPN = 0;
        }

        TouchPlusOld = digitalRead(TouchPlus);
    }

    void Buttons::ReadMinusButton()
    {
        // Button Check
        if (!digitalRead(TouchMinus) && TouchMinusOld)
        {
            TouchMinusPN = 1;
        }
        else
        {
            TouchMinusPN = 0;
        }

        TouchMinusOld = digitalRead(TouchMinus);
    }

    void Buttons::ReadManAutButton()
    {
        // Button Check
        if (!digitalRead(TouchManAut) && TouchManAutOld)
        {
            TouchManAutPN = 1;
        }
        else
        {
            TouchManAutPN = 0;
        }

        TouchManAutOld = digitalRead(TouchManAut);
    }

    void Buttons::ButtonLoop()
    {
        // Button Check
        ReadPowerButton();

        if (S_OnOff)
        {
            ReadManAutButton();

            ReadPlusButton();

            ReadMinusButton();

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
        pump.setPump(false);
        S_ManAut = false;
        Step_Manual = 10;
        Step_Automatic = 20;
        S_Menu = false;
        Step_Menu = 40;
        neopixel.colorWipeAll(strip.Color(0, 0, 0), 50); // Off
    }

    // OnOff Button Code
    _Button_states Buttons::OnOff()
    {
        if (S_OnOff == 0)
        {
            S_OnOff = 1;
            neopixel.colorWipeAll(strip.Color(0, 0, 127), 50); // Soft Blue
            S_ManAut = true;
            _button_states_t = Buttons_ON;
            return _button_states_t;
        }
        else
        {
            S_OnOff = false;
            All_Off();
            _button_states_t = Buttons_OFF;
            return _button_states_t;
        }
        return Buttons_OFF;
    }

    Buttons buttons;
}
