#ifndef ACCUMULATEDATA_HPP
#define ACCUMULATEDATA_HPP
#include <defines.hpp>

class AccumulateData
{
public:
    AccumulateData();
    virtual ~AccumulateData();

    void InitAccumulateData();
    String InitAccumulateDataJson();
    bool SendData();

private:
};

extern AccumulateData accumulatedata;

#endif