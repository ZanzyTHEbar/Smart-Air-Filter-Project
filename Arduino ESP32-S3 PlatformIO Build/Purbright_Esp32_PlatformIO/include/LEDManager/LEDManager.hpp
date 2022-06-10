#ifndef LEDMANAGER_HPP
#define LEDMANAGER_HPP
#include <Arduino.h>

namespace AirFilter
{
    class LEDManager
    {
    private:
        uint8_t ledPin;

    public:
        explicit LEDManager(uint8_t pin) : ledPin(pin) {}

        void setupLED() const;
        void on() const;
        void off() const;
        void blink(unsigned int time);
        void displayStatus();
    };
}

#endif // LEDMANAGER_HPP