#ifndef MDNSMANAGER_HPP
#define MDNSMANAGER_HPP
#include <ESPmDNS.h>
#include <StateManager.hpp>
#include <storage.hpp>

namespace AirFilter
{
    class MDNSHandler : public IObserver
    {
    private:
        StateManager *stateManager;
        Configuration *trackerConfig;

    public:
        MDNSHandler(StateManager *stateManager, Configuration *trackerConfig) : stateManager(stateManager), trackerConfig(trackerConfig) {}
        void startMDNS();
        void update(ObserverEvent::Event event);
    };
}

#endif // MDNSMANAGER_HPP