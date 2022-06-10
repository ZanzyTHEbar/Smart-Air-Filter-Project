#include "LEDManager.hpp"

void AirFilter::LEDManager::setupLED() const
{
    pinMode(ledPin, OUTPUT);
}

void AirFilter::LEDManager::on() const
{
    digitalWrite(ledPin, LOW);
}

void AirFilter::LEDManager::off() const
{
    digitalWrite(ledPin, HIGH);
}

void AirFilter::LEDManager::blink(unsigned int time)
{
    on();
    delay(time);
    off();
}

void AirFilter::LEDManager::displayStatus()
{
}