/*
 mqttconfig.h - Purbright library
 Copyright (c) 2021 Zacariah Austin Heim.
*/
#include "config.hpp"

// save last "timestamp" the config has been saved
auto last_config = 0;

Config::Config(void)
{
    last_config_change = false;
}

Config::~Config(void)
{
}

void Config::CreateDefaultConfig()
{
    config.hostname = NULL;
    config.MQTTEnabled = 0;
    config.MQTTPort = NULL;     // Port to use for unsecured MQTT
    config.MQTTPort_Secure = 0; // port to use if Secured MQTT is enabled
    config.MQTTUser = NULL;
    config.MQTTPass = NULL;
    config.MQTTTopic = NULL;
    config.MQTTSetTopic = NULL;
    config.MQTTDeviceName = NULL;
    config.last_mqtt_connect_attempt = 0;
    config.last_mqtt_publish_attempt = 0;
    config.lastMillis = 0;
    config.clientIP = NULL;
    config.MQTTSecureState = false;
    config.MQTTBroker = NULL;
    config.lastMsg = 0;
    config.msg = NULL;
    config.value = 0;
    config.WIFISSID = NULL;
    config.WIFIPASS = NULL;
    config.MQTTConnectedState = false;
    config.configData = "";
}

// Initialize SPIFFS
void Config::initSPIFFS()
{
  if (!SPIFFS.begin(true))
  {
    Serial.println("An error has occurred while mounting SPIFFS");
  }
  Serial.println("SPIFFS mounted successfully");
}

bool Config::loadConfig()
{
    Serial.println("Loading Config File");
    // load the config file
    initSPIFFS();
    File configFile = SPIFFS.open("/config.json", "r");
    if (!configFile)
    {
        Serial.println("Failed to open config file");
        CreateDefaultConfig();
        return false;
    }
    size_t size = configFile.size();
    Serial.print("Config file size: ");
    Serial.println(size);
    if (size > 1024)
    {
        Serial.println("Config file size is too large");
        CreateDefaultConfig();
        return false;
    }
    // Allocate a buffer to store contents of the file.
    std::unique_ptr<char[]> buf(new char[size]);

    // We don't use String here because ArduinoJson library requires the input
    // to be mutable. If you don't use ArduinoJson, you may as well
    // use configFile.readString instead.
    configFile.readBytes(buf.get(), size);
    Serial.println("Config file content:");
    Serial.println(buf.get());
    // Parse the buffer into an object
    StaticJsonDocument<1024> jsonBuffer;
    // Deserialize the JSON document
    auto error = deserializeJson(jsonBuffer, buf.get());
    if (error)
    {
        Serial.println("Failed to parse config file");
        Serial.print(F("deserializeJson() failed with code "));
        Serial.println(error.c_str());
        CreateDefaultConfig();
        return false;
    }
    
    heapStr(&config.hostname, jsonBuffer["hostname"]);
    config.MQTTEnabled = jsonBuffer["MQTTEnabled"]; //
    heapStr(&config.MQTTPort, jsonBuffer["MQTTPort"]);
    config.MQTTPort_Secure = jsonBuffer["MQTTPort_Secure"];
    heapStr(&config.MQTTUser, jsonBuffer["MQTTUser"]);
    heapStr(&config.MQTTPass, jsonBuffer["MQTTPass"]);
    heapStr(&config.MQTTTopic, jsonBuffer["MQTTTopic"]);
    heapStr(&config.MQTTSetTopic, jsonBuffer["MQTTSetTopic"]);
    heapStr(&config.MQTTDeviceName, jsonBuffer["MQTTDeviceName"]);
    config.last_mqtt_connect_attempt = jsonBuffer["last_mqtt_connect_attempt"];
    config.last_mqtt_publish_attempt = jsonBuffer["last_mqtt_publish_attempt"];
    config.lastMillis = jsonBuffer["lastMillis"];
    heapStr(&config.clientIP, jsonBuffer["clientIP"]);
    config.MQTTSecureState = jsonBuffer["MQTTSecureState"];
    heapStr(&config.MQTTBroker, jsonBuffer["MQTTBroker"]);
    config.lastMsg = jsonBuffer["lastMsg"];
    heapStr(&config.msg, jsonBuffer["msg"]);
    config.value = jsonBuffer["value"];
    heapStr(&config.WIFISSID, jsonBuffer["WIFISSID"]);
    heapStr(&config.WIFIPASS, jsonBuffer["WIFIPASS"]);
    config.MQTTConnectedState = jsonBuffer["MQTTConnectedState"];
    return true;
}

// trigger a config write/commit
bool Config::setConfigChanged()
{
    Serial.println("Should save config");
    last_config_change = true;
    return true;
}

bool Config::saveConfig()
{
    // check if the data in config is different from the data in the file
    if (!last_config_change)
    {
        Serial.println("Config has not changed");
        return true;
    }
    Serial.println("Saving Config");
    // create a json file from the config struct and save it using SPIFFs
    Serial.println("Writing config");
    // create a json file from the config struct
    StaticJsonDocument<1024> jsonConfig;
    JsonObject json = jsonConfig.to<JsonObject>();

    // create a json file from the config struct
    json["hostname"] = config.hostname;
    json["MQTTEnabled"] = config.MQTTEnabled;
    json["MQTTPort"] = config.MQTTPort;
    json["MQTTPort_Secure"] = config.MQTTPort_Secure;
    json["MQTTUser"] = config.MQTTUser;
    json["MQTTPass"] = config.MQTTPass;
    json["MQTTTopic"] = config.MQTTTopic;
    json["MQTTSetTopic"] = config.MQTTSetTopic;
    json["MQTTDeviceName"] = config.MQTTDeviceName;
    json["last_mqtt_connect_attempt"] = config.last_mqtt_connect_attempt;
    json["last_mqtt_publish_attempt"] = config.last_mqtt_publish_attempt;
    json["lastMillis"] = config.lastMillis;
    json["clientIP"] = config.clientIP;
    json["MQTTSecureState"] = config.MQTTSecureState;
    json["MQTTBroker"] = config.MQTTBroker;
    json["lastMsg"] = config.lastMsg;
    json["msg"] = config.msg;
    json["value"] = config.value;
    json["WIFISSID"] = config.WIFISSID;
    json["WIFIPASS"] = config.WIFIPASS;
    json["MQTTConnectedState"] = config.MQTTConnectedState;

    // Set the values in the document

    File configFile = SPIFFS.open("/config.json", "w");
    if (!configFile)
    {
        Serial.println("Failed to open config file for writing");
        return false;
    }
    if (serializeJson(json, configFile) == 0)
    {
        Serial.println(F("Failed to write to file"));
        return false;
    }
    configFile.print("\r\n");
    configFile.close();
    // end save
    Serial.println("Config written");

    return true;
}

bool Config::updateCurrentData()
{
    // call to save config if config has changed
    saveConfig();
    SERIAL_DEBUG_LNF("Heap: %d", system_get_free_heap_size());
    SERIAL_DEBUG_LN("Updating current data");
    return true;
    // update current data
}

// overwrite all config settings with "0"
void Config::resetConfig()
{
    SERIAL_DEBUG_LN("Resetting Config");
    // overwrite all config settings with "0"
    CreateDefaultConfig();
    // call to save config if config has changed
    setConfigChanged();
    saveConfig();
}

bool Config::isValidHostname(char *hostname_to_check, long size)
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
void Config::setHostname(String new_hostname)
{
    int j = 0;
    for (unsigned int i = 0; i < new_hostname.length() && i < sizeof(config.hostname); i++)
    {
        if (new_hostname.charAt(i) == '-' or
            (new_hostname.charAt(i) >= '0' && new_hostname.charAt(i) <= '9') or
            (new_hostname.charAt(i) >= 'A' && new_hostname.charAt(i) <= 'Z') or
            (new_hostname.charAt(i) >= 'a' && new_hostname.charAt(i) <= 'z'))
        {

            config.hostname[j] = new_hostname.charAt(i);
            j++;
        }
    }
    config.hostname[j] = '\0';
    setConfigChanged();
}

// we can't assing wifiManager.resetSettings(); to reset, somehow it gets called straight away.
void Config::setWiFiConf(String ssid, String password)
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

Config cfg;
