#ifndef ACCUMULATEDATA_HPP
#define ACCUMULATEDATA_HPP
#include <defines.hpp>

namespace AirFilter
{
    class AccumulateData : public exeClass
    {
    public:
        AccumulateData(float seconds);
        AccumulateData(void);
        virtual ~AccumulateData(void);

        virtual void execute(void);

        void InitAccumulateData();
        String InitAccumulateDataJson();
        bool SendData();
        void begin(PIR *pir_sensor);
        timeObj Timer_5s;

    private:
        PIR *_pir_sensor;
    };

    extern AccumulateData accumulatedata;
}

#endif