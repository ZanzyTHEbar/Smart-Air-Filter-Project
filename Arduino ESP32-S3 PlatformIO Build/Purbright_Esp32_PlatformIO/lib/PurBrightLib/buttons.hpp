#pragma once
#ifndef BUTTONS_hpp
#define BUTTONS_hpp
#include "defines.hpp"

class Buttons
{
public:
    // Functions
    Buttons();
    virtual ~Buttons(void);
    void SetupButtons();
    void OnOff();
    void All_Off();
    void ButtonLoop();
    
};
#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_BUTTONS)
extern Buttons buttons;
#endif
#endif