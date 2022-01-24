#include "neopixel.hpp"

Neopixel::Neopixel()
{
    SetupNeopixel();
    // InitNeopixelJson();
}

Neopixel::~Neopixel()
{
}

void Neopixel::SetupNeopixel()
{
    strip.begin();
    strip.setBrightness(50);
    strip.show(); // Initialize all pixels to 'off'
}

// Settings Code
void Neopixel::Settings()
{
  for (int i = 2; i < strip.numPixels(); i++)
  {
    strip.setPixelColor(i, strip.Color(0, 0, 0));
    strip.show();
  }
  for (int i = 2; i < PumpSetting; i++)
  {
    strip.setPixelColor(i, strip.Color(255, 255, 255));
    strip.show();
  }
  DelayStartSettings = millis();
  DelayRunningSettings = true;
  S_Menu = true;
}

void Neopixel::Plus()
{
  if (PumpSetting < 12)
  {
    PumpSetting = PumpSetting + 1;
    DelayStartSettings = millis();
    for (int i = 2; i < PumpSetting; i++)
    {
      strip.setPixelColor(i, strip.Color(255, 255, 255));
      strip.show();
    }
  }
}

void Neopixel::Minus()
{
  if (PumpSetting > 3)
  {
    PumpSetting = PumpSetting - 1;
    DelayStartSettings = millis();
    for (int i = PumpSetting; i < strip.numPixels(); i++)
    {
      strip.setPixelColor(i, strip.Color(0, 0, 0));
      strip.show();
    }
  }
}

// NeoPixel Codes-------------------------------------------------
void Neopixel::colorWipe(uint32_t c, uint8_t wait)
{
  for (uint16_t i = 2; i < strip.numPixels(); i++)
  {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}
void Neopixel::colorWipeAll(uint32_t c, uint8_t wait)
{
  for (uint16_t i = 0; i < strip.numPixels(); i++)
  {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

//----------------------------------------------------------

