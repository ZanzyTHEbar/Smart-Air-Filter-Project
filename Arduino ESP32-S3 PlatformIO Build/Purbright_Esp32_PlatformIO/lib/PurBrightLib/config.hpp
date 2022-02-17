/*
 mqttconfig.h - Purbright library
 Copyright (c) 2021 Zacariah Austin Heim.
*/

#pragma once
#ifndef CONFIG_hpp
#define CONFIG_hpp
#include "defines.hpp"

class Config
{
public:
    Config(void);
    virtual ~Config(void);
    typedef struct Config_t
    {
        // Variables
        char* hostname;
        uint8_t MQTTEnabled;
        char* MQTTPort;        // Port to use for unsecured MQTT
        uint16_t MQTTPort_Secure; // port to use if Secured MQTT is enabled
        char* MQTTUser;
        char* MQTTPass;
        char* MQTTTopic;
        char* MQTTSetTopic;
        char* MQTTDeviceName;
        uint8_t last_mqtt_connect_attempt;
        uint8_t last_mqtt_publish_attempt;
        unsigned long lastMillis;
        char* clientIP;
        bool MQTTSecureState;
        char* MQTTBroker;
        long lastMsg;
        char* msg;
        int value;
        char* WIFISSID;
        char* WIFIPASS;
        bool MQTTConnectedState;
        String configData;
    } configData_t;
    
    bool loadConfig();
    // trigger a config write/commit
    bool setConfigChanged();
    bool updateCurrentData();
    // overwrite all config settings with "0"
    void resetConfig();
    bool saveConfig();
    bool isValidHostname(char *hostname_to_check, long size);
    // parse and set a new hostname to config
    void setHostname(String new_hostname);
    // we can't assing wifiManager.resetSettings(); to reset, somehow it gets called straight away.
    void setWiFiConf(String ssid, String password);
    void InitDataStruct();
    void CreateDefaultConfig();
    void initSPIFFS();
    configData_t config;
    configData_t default_cfg;

    // save last "timestamp" the config has been saved
    bool last_config_change;
};
#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_CONFIG)
extern Config cfg;
#endif
#endif
// EOF