#ifndef PUMP_HPP
#define PUMP_HPP
#include <defines.hpp>
#include <TimeLib.h> // For timekeeping#ifndef PUMP_HPP

class PUMP
{
public:
    PUMP();
    virtual ~PUMP();
    void serialControl();
    void scheduleFromUser();
    void setPump(bool state);
    void SetupPumpClass();
    void PumpLoopAdvanced();
    void PumpLoop();
    void checkTurnOffPump();
    void Man_Aut();
    void Manual();
    void Automatic();
    String timeReport();
    String serialReport();

    enum _pump_state
    {
        UNDEF,
        PUMP_OFF,
        PUMP_ON,
        PUMP_MANUAL,
        PUMP_AUTOMATIC,
        PUMP_SERIAL_REPORT
    };

    _pump_state _pump_state_t;

    _pump_state CheckState(const char *state);

    struct PumpData
    {
        int _runInterval;
        int _pumpInterval; // min interval between nozzle activation >60 == off
        int _pumpDuration; // sec active nozzle <0 == off
        int _oneReport;
        bool _pumpManAut;
        bool _pumpOn; // 0 off, 1 on
    } * pumpData;

    PumpData pumpdata;

    PumpData AggregateData();

    // Friends
    friend class BASEMQTT;
    friend void onMqttMessage(const char *topic, const uint8_t *payload, uint16_t length);
    friend void callback(char *topic, byte *payload, unsigned int length);
    friend void onMqttConnected();
    friend class Buttons;
    friend class HASSMQTT;

private:
    // LED pins (PWM pins: 3,5,6,9,10,11 on arduino UNO)
    byte _pump_relay_pin;
    byte _MotionSensorPin;
    int _runInterval;
    int _pumpMaxRunTime;
    // const uint8_t flow_sensor_data_pin = 6;

    bool _pumpOn;      // 0 off, 1 on
    int _pumpInterval; // min interval between nozzle activation >60 == off
    int _pumpDuration; // sec active nozzle <0 == off

    // Run preprogrammed setup, oneReport after nozzle on
    int _runProgram;
    int _oneReport;
    // Time
    time_t _t_;
    time_t _tDelay;
};
extern PUMP pump;
#endif // PUMP_HPP