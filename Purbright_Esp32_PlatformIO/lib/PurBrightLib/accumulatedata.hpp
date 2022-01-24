/*
 accumulatedata.h - PurBright library
 Copyright (c) 2021 Zacariah Austin Heim.
 */
#pragma once
#ifndef ACCUMULATEDATA_hpp
#define ACCUMULATEDATA_hpp
#include "defines.hpp"

//#include <MemoryFree.h>

// DynamicJsonDocument  doc(200);

class AccumulateData
{
public:
    // Functions
    AccumulateData();
    virtual ~AccumulateData(void);
    void SetupMainLoop();
    void InitAccumulateDataJson();
private:
};
#endif