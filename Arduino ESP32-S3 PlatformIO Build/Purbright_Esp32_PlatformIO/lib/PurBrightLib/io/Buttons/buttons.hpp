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

        _Button_states OnOff();

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