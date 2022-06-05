#ifndef ACCUMULATEDATA_HPP
#define ACCUMULATEDATA_HPP
#include <defines.hpp>

class AccumulateData
{
public:
    AccumulateData();
    virtual ~AccumulateData();

    void InitAccumulateData();
    void InitAccumulateDataJson();
    bool SendData();

private:
};

extern AccumulateData accumulatedata;

#endif