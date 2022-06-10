#pragma once
#include <WiFi.h>
#include <storage.hpp>
#include <StateManager.hpp>

namespace AirFilter
{
    namespace WiFiHandler
    {
        void setupWifi(AirFilter::StateManager *stateManager, AirFilter::Configuration *trackerConfig);
    }
}