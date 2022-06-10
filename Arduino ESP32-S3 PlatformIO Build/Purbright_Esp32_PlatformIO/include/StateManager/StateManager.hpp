#ifndef STATEMANAGER_HPP
#define STATEMANAGER_HPP

namespace AirFilter
{
    enum State
    {
        Starting,
        ConnectingToWifi,
        ConnectingToWifiError,
        ConnectingToWifiSuccess,
        ServerError,
        PumpError,
        MDNSSuccess,
        MDNSError,
        MQTTSuccess,
        MQTTError,
    };

    enum _Buttons_state
    {
        UNDEF,
        Buttons_OFF,
        Buttons_ON,
        Buttons_PLUS,
        Buttons_MINUS,
    };

    class StateManager
    {
    public:
        StateManager() : current_state(Starting) {}
        void setState(State state);
        State getCurrentState();

    private:
        State current_state;
    };

}

#endif // STATEMANAGER_HPP