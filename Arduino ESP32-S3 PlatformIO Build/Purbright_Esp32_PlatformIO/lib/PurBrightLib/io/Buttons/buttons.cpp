#include "buttons.hpp"

Buttons::_Buttons_state _buttons_state_t = Buttons::Buttons_OFF;

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

Buttons::_Buttons_state Buttons::CheckState(const char *state)
{
    switch (_buttons_state_t)
    {
    case Buttons_OFF: // Undefined
        break;
    case Buttons_ON:
        if (state == "OFF")
        {
            _buttons_state_t = Buttons_OFF;
            log_i("Turning off the buttons");
        }
        else if (state == "PLUS")
        {
            _buttons_state_t = Buttons_PLUS;
            neopixel.Plus();
        }
        else if (state == "MINUS")
        {
            _buttons_state_t = Buttons_MINUS;
            neopixel.Minus();
        }
        break;
    case Buttons_PLUS:
        if (state == "OFF")
        {
            _buttons_state_t = Buttons_OFF;
            log_i("Turning off the buttons");
        }
        else if (state == "ON")
        {
            _buttons_state_t = Buttons_ON;
            log_i("Turning on the buttons");
        }
        else if (state == "PLUS")
        {
            _buttons_state_t = Buttons_PLUS;
            neopixel.Plus();
        }
        else if (state == "MINUS")
        {
            _buttons_state_t = Buttons_MINUS;
            neopixel.Minus();
        }
        break;
    }
    return _buttons_state_t;
}

void Buttons::ButtonLoop()
{
    // Button Check
    if (!digitalRead(TouchOnOff) && TouchOnOffOld)
    {
        TouchOnOffPN = 1;
        _buttons_state_t = Buttons_ON;
    }
    else
    {
        TouchOnOffPN = 0;
        _buttons_state_t = Buttons_OFF;
    }

    TouchOnOffOld = digitalRead(TouchOnOff);

    if (TouchOnOffPN)
    {
        _buttons_state_t = OnOff();
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
    pump.setPump(false);
    S_ManAut = false;
    Step_Manual = 10;
    Step_Automatic = 20;
    S_Menu = false;
    Step_Menu = 40;
    neopixel.colorWipeAll(strip.Color(0, 0, 0), 50); // Off
}

// OnOff Button Code
Buttons::_Buttons_state Buttons::OnOff()
{
    if (S_OnOff == 0)
    {
        S_OnOff = 1;
        neopixel.colorWipeAll(strip.Color(0, 0, 127), 50); // Soft Blue
        S_ManAut = true;
        _buttons_state_t = Buttons_ON;
        return _buttons_state_t;
    }
    else
    {
        S_OnOff = false;
        All_Off();
        _buttons_state_t = Buttons_OFF;
        return _buttons_state_t;
    }
    return Buttons_OFF;
}

Buttons buttons;