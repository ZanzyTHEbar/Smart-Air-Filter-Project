#pragma once
#ifndef BUTTONS_hpp
#define BUTTONS_hpp
#include <defines.hpp>

namespace AirFilter
{
    class Buttons
    {
    public:
        Buttons();
        virtual ~Buttons();
        void All_Off();
        void ButtonLoop();
        void SetupButtons();

        _Buttons_state CheckState(const char *state);
        _Buttons_state OnOff();

    private:
        // functions
        void ReadPowerButton();
        void ReadPlusButton();
        void ReadMinusButton();
        void ReadManAutButton();
    };
    extern Buttons buttons;
}
#endif