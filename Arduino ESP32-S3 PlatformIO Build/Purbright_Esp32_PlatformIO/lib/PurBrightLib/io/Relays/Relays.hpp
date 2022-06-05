#ifndef RELAYS_HPP
#define RELAYS_HPP
#include <defines.hpp>

class Relays
{
public:
    Relays();
    virtual ~Relays();
    void RelayOnOff(byte relay, bool on);
    void SetupPID();
    void SetupRelays();
#if USE_SHT31_SENSOR
    void HumRelayOnOff();
#endif // USE_SHT31_SENSOR

private:

#if USE_PID
    double _Setpoint, _Input, _Output;
    int _WindowSize;
    unsigned long _windowStartTime;
    PID myPID;
#endif // USE_PID
};
extern Relays Relay;
#endif