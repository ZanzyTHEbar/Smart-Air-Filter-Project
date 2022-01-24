/*
 mqttconfig.h - Purbright library
 Copyright (c) 2021 Zacariah Austin Heim.
 */
#pragma once
#ifndef CONFIG_hpp
#define CONFIG_hpp
#include "defines.hpp"
#define EEPROM_SIZE 512
#define CONFIG_SAVE_MAX_DELAY 10 // delay in seconds when the settings are saved after last change occurs
#define CONFIG_COMMIT_DELAY 200  // commit delay in ms

typedef struct
{
    // Variables
    char hostname[33];
    uint8_t MQTTEnabled;
    uint16_t MQTTPort;        // Port to use for unsecured MQTT
    uint16_t MQTTPort_Secure; // port to use if Secured MQTT is enabled
    char MQTTUser[33];
    char MQTTPass[65];
    char MQTTTopic[65];
    char MQTTSetTopic[65];
    char MQTTDeviceName[33];
    uint8_t last_mqtt_connect_attempt;
    uint8_t last_mqtt_publish_attempt;
    unsigned long lastMillis;
    char clientIP;
    bool MQTTSecureState;
    char MQTTBroker;
    long lastMsg;
    char msg;
    int value;
    char *WIFISSID;
    char *WIFIPASS;
    bool MQTTConnectedState;
} configData_t;

configData_t cfg;
configData_t default_cfg;

// save last "timestamp" the config has been saved
unsigned long last_config_change = 0;

void initConfig()
{
    // load default config
    memcpy(&cfg, &default_cfg, sizeof(configData_t));

    // load config from EEPROM
    EEPROM.begin(EEPROM_SIZE);
    EEPROM.get(0, cfg);
    EEPROM.end();

    // check if config is valid
    if (cfg.MQTTEnabled == 0)
    {
        // load default config
        memcpy(&cfg, &default_cfg, sizeof(configData_t));
    }
}

void saveConfig(bool force = false)
{

    if (last_config_change == 0 && force == false)
    {
        return;
    }

    static bool write_config = false;
    static bool write_config_done = false;
    static bool commit_config = false;

    if (force == true)
    {
        write_config = true;
        commit_config = true;
    }

    if (last_config_change > 0)
    {

        if (last_config_change + (CONFIG_SAVE_MAX_DELAY * 1000) < millis())
        {

            // timer expired and config has not been written
            if (write_config_done == false)
            {
                write_config = true;

                // config has been written but we should wait 200ms to commit
            }
            else if (last_config_change + (CONFIG_SAVE_MAX_DELAY * 1000) + CONFIG_COMMIT_DELAY < millis())
            {
                commit_config = true;
            }
        }
    }

    // Save configuration from RAM into EEPROM
    if (write_config == true)
    {
        // SERIAL_DEBUG_LN(F("Saving Config"));
        EEPROM.begin(4095);
        EEPROM.put(0, cfg);
        write_config_done = true;
        write_config = false;
    }

    if (commit_config == true)
    {
        if (force == true)
            delay(CONFIG_COMMIT_DELAY);
        // SERIAL_DEBUG_LN(F("Committing config"));
        EEPROM.commit();
        EEPROM.end();

        // reset all triggers
        last_config_change = 0;
        write_config = false;
        write_config_done = false;
        commit_config = false;
    }
}

// trigger a config write/commit
void setConfigChanged()
{
    // start timer
    last_config_change = millis();
}

// overwrite all config settings with "0"
void resetConfig()
{

    // delete EEPROM config
    EEPROM.begin(4095);
    for (unsigned int i = 0; i < sizeof(cfg); i++)
    {
        EEPROM.write(i, 0);
    }
    delay(CONFIG_COMMIT_DELAY);
    EEPROM.commit();
    EEPROM.end();

    // set to default config
    cfg = default_cfg;
    saveConfig(true);
}

bool isValidHostname(char *hostname_to_check, long size)
{
    for (int i = 0; i < size; i++)
    {
        if (hostname_to_check[i] == '-' || hostname_to_check[i] == '.')
            continue;
        else if (hostname_to_check[i] >= '0' && hostname_to_check[i] <= '9')
            continue;
        else if (hostname_to_check[i] >= 'A' && hostname_to_check[i] <= 'Z')
            continue;
        else if (hostname_to_check[i] >= 'a' && hostname_to_check[i] <= 'z')
            continue;
        else if (hostname_to_check[i] == 0 && i > 0)
            break;

        return false;
    }

    return true;
}

// parse and set a new hostname to config
void setHostname(String new_hostname)
{
    int j = 0;
    for (unsigned int i = 0; i < new_hostname.length() && i < sizeof(cfg.hostname); i++)
    {
        if (new_hostname.charAt(i) == '-' or
            (new_hostname.charAt(i) >= '0' && new_hostname.charAt(i) <= '9') or
            (new_hostname.charAt(i) >= 'A' && new_hostname.charAt(i) <= 'Z') or
            (new_hostname.charAt(i) >= 'a' && new_hostname.charAt(i) <= 'z'))
        {

            cfg.hostname[j] = new_hostname.charAt(i);
            j++;
        }
    }
    cfg.hostname[j] = '\0';
    setConfigChanged();
}

// we can't assing wifiManager.resetSettings(); to reset, somehow it gets called straight away.
void setWiFiConf(String ssid, String password)
{
/* #if defined(ESP32)
    if (WiFiGenericClass::getMode() != WIFI_MODE_NULL)
    {

        wifi_config_t conf;
        esp_wifi_get_config(WIFI_IF_STA, &conf);

        memset(conf.sta.ssid, 0, sizeof(conf.sta.ssid));
        for (int i = 0; i < ssid.length() && i < sizeof(conf.sta.ssid); i++)
            conf.sta.ssid[i] = ssid.charAt(i);

        memset(conf.sta.password, 0, sizeof(conf.sta.password));
        for (int i = 0; i < password.length() && i < sizeof(conf.sta.password); i++)
            conf.sta.password[i] = password.charAt(i);

        esp_wifi_set_config(WIFI_IF_STA, &conf);
    }
#endif */
}

#endif
// EOF