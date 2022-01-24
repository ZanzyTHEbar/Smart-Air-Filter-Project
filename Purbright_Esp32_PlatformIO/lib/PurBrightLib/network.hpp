/*
 Network.h - HBAT library
 Copyright (c) 2021 Zacariah Austin Heim.
 */
#pragma once
#ifndef NETWORK_hpp
#define NETWORK_hpp

#include "defines.hpp"

class Network
{
public:
    // constructors
    Network();
    virtual ~Network();
    // Functions
    int CheckWifiState();
    void connectToApWithFailToStation();

    int DiscovermDNSBroker();
    void SetupmDNSServer();
    void SetupmDNSLoop();
    void loadConfig();
    void SetupServer();
    void CheckNetworkLoop();
    void updateCurrentData();

    // variables
private:
};

#endif
