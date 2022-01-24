#include "buttons.hpp"

Buttons::Buttons()
{
    SetupButtons();
}

Buttons::~Buttons()
{
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
    // Class variables
    Pump *pump = new Pump;
    Neopixel *neopixel = new Neopixel;
    // Button Check
    if (digitalRead(TouchOnOff)==1) {Serial.write("ON ");}
    if (digitalRead(TouchManAut)==1) {Serial.write("MANAUT ");}
    if (digitalRead(TouchPlus)==1) {Serial.write("PLUS ");}
    if (digitalRead(TouchMinus)==1) {Serial.write("MINUS ");}
    if (digitalRead(MotionSensor)==1) {Serial.write("MOTION ");}
    if (S_Menu==false) {Serial.write(S_Menu ? "OFF " : "ON ");}

    // On Off
    if ((digitalRead(TouchOnOff) == 0) && (TouchOnOffOld == 1))
    {
        TouchOnOffPN = 1;
    }
    else
    {
        TouchOnOffPN = 0;
    }
    TouchOnOffOld = digitalRead(TouchOnOff);

    if (S_OnOff == true)
    {
        // Manual
        if ((digitalRead(TouchManAut) == 0) && (TouchManAutOld == 1))
        {
            TouchManAutPN = 1;
        }
        else
        {
            TouchManAutPN = 0;
        }
        TouchManAutOld = digitalRead(TouchManAut);

        // Plus
        if ((digitalRead(TouchPlus) == 0) && (TouchPlusOld == 1))
        {
            TouchPlusPN = 1;
        }
        else
        {
            TouchPlusPN = 0;
        }
        TouchPlusOld = digitalRead(TouchPlus);

        // Minus
        if ((digitalRead(TouchMinus) == 0) && (TouchMinusOld == 1))
        {
            TouchMinusPN = 1;
        }
        else
        {
            TouchMinusPN = 0;
        }
        TouchMinusOld = digitalRead(TouchMinus);
    }
    else
    {
        // Powerled
        strip.setPixelColor(0, strip.Color(255, 225, 255));
        strip.show();
    }

    // BUTTONS
    if (TouchOnOffPN == 1)
    {
        OnOff();
    }
    if ((S_Menu == false) && (TouchManAutPN == 1))
    {
        pump->Man_Aut();
    }
    if ((S_Menu == false) && (TouchPlusPN == 1))
    {
        neopixel->Settings();
    }
    if ((S_Menu == true) && (TouchPlusPN == 1))
    {
        neopixel->Plus();
    }
    if ((S_Menu == false) && (TouchMinusPN == 1))
    {
        neopixel->Settings();
    }
    if ((S_Menu == true) && (TouchMinusPN == 1))
    {
        neopixel->Minus();
    }

    delete pump;
    delete neopixel;
}

// All Off
void Buttons::All_Off()
{
    Neopixel *neopixel = new Neopixel;
    digitalWrite(PumpRelay, false);
    S_ManAut = false;
    Step_Manual = 10;
    Step_Automatic = 20;
    S_Menu = false;
    Step_Menu = 40;
    neopixel->colorWipeAll(strip.Color(0, 0, 0), 50); // Off
    delete neopixel;
}

// OnOff Button Code
void Buttons::OnOff()
{
    Neopixel *neopixel = new Neopixel;
    if (S_OnOff == 0)
    {
        S_OnOff = 1;
        neopixel->colorWipeAll(strip.Color(0, 0, 127), 50); // Soft Blue
        S_ManAut = true;
    }
    else
    {
        S_OnOff = false;
        All_Off();
    }
    delete neopixel;
}
