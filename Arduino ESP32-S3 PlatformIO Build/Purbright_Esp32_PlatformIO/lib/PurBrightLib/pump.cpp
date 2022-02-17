
#include "pump.hpp"

const uint8_t pump_relay_pin = 14;
//const uint8_t flow_sensor_data_pin = 6;
const uint8_t MotionSensor = 21;

Pump::Pump(void)
{
}

Pump::~Pump(void)
{
}

void Pump::SetupPump()
{
    pinMode(pump_relay_pin, OUTPUT);
    digitalWrite(pump_relay_pin, LOW);
    //pinMode(flow_sensor_data_pin, INPUT);
    pinMode(MotionSensor, INPUT);
    /* pinMode(WaterLevelSensor, INPUT); */
}

void Pump::PumpLoop()
{
  // Pump indication
  if (digitalRead(pump_relay_pin) == true)
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
    digitalWrite(pump_relay_pin, false);
    strip.setPixelColor(0, strip.Color(0, 0, 0));
    strip.show();
  }
}

//TODO: Save settings to SPIFFS config file to have persistent settings across reboots
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
      digitalWrite(pump_relay_pin, false);
    }
  }
}

//TODO: Save settings to SPIFFS config file to have persistent settings across reboots
// Manual Code
void Pump::Manual()
{
  neopixel.colorWipe(strip.Color(233, 225, 85), 50);
  digitalWrite(pump_relay_pin, true);
  S_ManAut = 1;
}

//TODO: Save settings to SPIFFS config file to have persistent settings across reboots
// Automatic Code
void Pump::Automatic()
{
  neopixel.colorWipe(strip.Color(0, 0, 127), 70);
  if (digitalRead(MotionSensor) == 1)
  {
    DelayStartPump = millis();
    DelayRunningPump = true;
    digitalWrite(pump_relay_pin, true);
  }
  else
  {
    digitalWrite(pump_relay_pin, false);
  }
  S_ManAut = 0;
}

Pump pump;