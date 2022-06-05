#pragma once
#ifndef BUTTONS_hpp
#define BUTTONS_hpp
#include <defines.hpp>

class Buttons
{
public:
    Buttons();
    virtual ~Buttons();
    void OnOff();
    void All_Off();
    void ButtonLoop();
    void SetupButtons();

private:
};
extern Buttons buttons;
#endif