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
        CameraError,
        MDNSSuccess,
        MDNSError,
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