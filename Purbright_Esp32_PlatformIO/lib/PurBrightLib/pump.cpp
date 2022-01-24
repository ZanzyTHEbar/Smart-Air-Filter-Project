#include "pump.hpp"

Pump::Pump()
{
    SetupPump();
}

Pump::~Pump()
{
}

void Pump::SetupPump()
{
    pinMode(FlowSensor, INPUT);
    pinMode(PumpRelay, OUTPUT);
}

void Pump::SetupMainLoop()
{
    // Pump indication
    if (digitalRead(PumpRelay) == true)
    {
        strip.setPixelColor(1, strip.Color(0, 255, 0));
        strip.show();
    }
    else if (S_OnOff == true)
    {
        if (S_ManAut == 0)
        {
            strip.setPixelColor(1, strip.Color(233, 225, 85));
            strip.show();
        }
        else
        {
            strip.setPixelColor(1, strip.Color(0, 0, 127));
            strip.show();
        }
    }
}

// Timer delay Pump
void Pump::checkTurnOffPump()
{
  // check if delay has timed out
  if (DelayRunningPump && ((millis() - DelayStartPump) >= ((PumpSetting - 1) * 120000)))
  { // 2min per led
    DelayRunningPump = false;
    digitalWrite(PumpRelay, false);
    strip.setPixelColor(0, strip.Color(0, 0, 0));
    strip.show();
  }
}

// Manual-Automatic
void Pump::Man_Aut()
{
  if ((S_OnOff == 1) && (S_Menu == 0))
  {
    if (S_ManAut == 1)
    {
      S_ManAut = 0;
      Manual();
    }
    else
    {
      S_ManAut = 1;
      digitalWrite(PumpRelay, false);
    }
  }
}

// Manual Code
void Pump::Manual()
{
  Neopixel *neopixel = new Neopixel;
  neopixel->colorWipe(strip.Color(233, 225, 85), 50);
  digitalWrite(PumpRelay, true);
  S_ManAut = 1;
  delete neopixel;
}

// Automatic Code
void Pump::Automatic()
{
  Neopixel *neopixel = new Neopixel;
  neopixel->colorWipe(strip.Color(0, 0, 127), 70);
  if (digitalRead(MotionSensor) == 1)
  {
    DelayStartPump = millis();
    DelayRunningPump = true;
    digitalWrite(PumpRelay, true);
  }
  else
  {
    digitalWrite(PumpRelay, false);
  }
  S_ManAut = 0;
  delete neopixel;
}