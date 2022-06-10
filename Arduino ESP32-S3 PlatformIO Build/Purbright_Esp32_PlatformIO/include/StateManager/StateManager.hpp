#ifndef STATEMANAGER_HPP
#define STATEMANAGER_HPP

namespace AirFilter
{
    /*
     * StateManager
     * All Project States are managed here
     */
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

    enum _Button_states
    {
        UNDEF,
        Buttons_OFF,
        Buttons_ON,
        Buttons_PLUS,
        Buttons_MINUS,
        Buttons_ManAut
    };

    /*
     * EventManager
     * All Project Events are managed here
     */

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