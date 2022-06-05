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
    void serialReport();
    void SetupPumpClass();
    void PumpLoopAdvanced();
    void PumpLoop();
    void checkTurnOffPump();
    void Man_Aut();
    void Manual();
    void Automatic();
    String timeReport();

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
    const char *_pumpTopic;
    int _runInterval;
    int _pumpMaxRunTime;
    // const uint8_t flow_sensor_data_pin = 6;

    int _pumpOn;       // 0 off, 1 on
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