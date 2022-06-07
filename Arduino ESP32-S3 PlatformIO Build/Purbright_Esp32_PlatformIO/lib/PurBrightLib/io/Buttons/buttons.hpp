#pragma once
#ifndef BUTTONS_hpp
#define BUTTONS_hpp
#include <defines.hpp>

class Buttons
{
public:
    Buttons();
    virtual ~Buttons();
    void All_Off();
    void ButtonLoop();
    void SetupButtons();

    enum _Buttons_state
    {
        UNDEF,
        Buttons_OFF,
        Buttons_ON,
        Buttons_PLUS,
        Buttons_MINUS,
    };

    _Buttons_state _buttons_state_t;

    _Buttons_state CheckState(const char *state);
    _Buttons_state OnOff();

private:
};
extern Buttons buttons;
#endif