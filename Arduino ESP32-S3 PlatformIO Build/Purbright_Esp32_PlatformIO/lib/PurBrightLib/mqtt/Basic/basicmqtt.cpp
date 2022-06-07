#include "basicmqtt.hpp"

IPAddress broker_ip;

void callback(char *topic, byte *payload, unsigned int length);
String getBroker();

#if MQTT_SECURE
PubSubClient mqttClient(broker_ip.fromString(getBroker()), MQTT_SECURE_PORT, callback, espClient); // Local Mosquitto Connection
#else
PubSubClient mqttClient(broker_ip.fromString(getBroker()), MQTT_PORT, callback, espClient); // Local Mosquitto Connection
#endif // MQTT_SECURE

//***********************************************************************************************************************
// * Class Global Variables
// * Please only make changes to the following class variables within the ini file. Do not change them here.
//************************************************************************************************************************

BASEMQTT::BASEMQTT() : _interval(60000), _user_data{0}, _previousMillis(0), _user_bytes_received(0), _publishState(false)
{
}

BASEMQTT::~BASEMQTT()
{
    // Destructor
}

bool BASEMQTT::Setup()
{
    log_i("Setting up MQTT...");

    // Local Mosquitto Connection -- Start
    if (mqttClient.connect(DEFAULT_HOSTNAME))
    {
        // connection succeeded
        log_i("Connection succeeded. Subscribing to the topic [%s]", _pumpTopic);
        mqttClient.subscribe(_pumpTopic);
        log_i("Successfully subscribed to the topic.");
        return true;
    }
    else
    {
        // connection failed
        log_i("Connection failed. MQTT client state is: %d", mqttClient.state());
        return false;
    }
    // Local Mosquitto Connection -- End
    cfg.config.MQTTConnectedState = mqttClient.state();
    _pumpTopic = PUMP_TOPIC;
    _pumpInTopic = PUMP_IN_TOPIC;
    _menuTopic = MENU_TOPIC;
    _ledTopic = LED_TOPIC;
    /* _speakerTopic = SPEAKER_TOPIC;
    _waterlevelTopic = WATER_LEVEL_TOPIC; */
    return mqttClient.state();
}

String getBroker()
{
#if ENABLE_MDNS_SUPPORT
#pragma message(Feature "mDNS Enabled: " STR(ENABLE_MDNS_SUPPORT " - Yes"))
    if (mDNSDiscovery::DiscovermDNSBroker())
    {
        Serial.println(F("[mDNS responder started] Setting up Broker..."));
        String BROKER_ADDR = cfg.config.MQTTBroker; // IP address of the MQTT broker - change to your broker IP address or enable MDNS support
        return BROKER_ADDR;
    }
    else
    {
        Serial.println(F("[mDNS responder failed]"));
        String BROKER_ADDR = MQTT_BROKER;
        return BROKER_ADDR;
    }
    return String(MQTT_BROKER);
#else
#pragma message(Feature "mDNS Enabled: " STR(ENABLE_MDNS_SUPPORT " - No"))
    return String(MQTT_BROKER);
#endif // ENABLE_MDNS_SUPPORT
}

// Handles messages arrived on subscribed topic(s)
void callback(char *topic, byte *payload, unsigned int length)
{
    String result;
    log_i("Message arrived on topic: [%s] ", topic);
    for (int i = 0; i < length; i++)
    {
        log_i("payload is: [%s]", (char)payload[i]);
        result += (char)payload[i];
    }
    log_i("Message: [%s]", result.c_str());

    // Check if the message is for the current topic
    if (strcmp(topic, basemqtt._pumpTopic) == 0)
    {
        switch (pump.CheckState(result.c_str()))
        {
        case 0:
            log_d("Pump is in an undefined state.");
            break;
        case 1:
            log_d("Pump is not running.");
            break;
        case 2:
            log_d("Pump is running.");
            break;
        case 3:
            log_d("Pump is in Manual.");
            break;
        case 4:
            log_d("Pump is in Automatic.");
            break;
        default:
            break;
        }
    }
    else if (strcmp(topic, basemqtt._menuTopic) == 0)
    {
        switch (buttons.CheckState(result.c_str()))
        {
        case 0:
            log_d("Litter Box has been turned off.");
            break;
        case 1:
            log_d("Litter Box has been turned on.");
            break;
        case 2:
            log_d("");
            break;
        case 3:
            log_d("Pump is in Manual.");
            break;
        case 4:
            log_d("Pump is in Automatic.");
            break;
        default:
            break;
        }
    }
    
    /* else if (strcmp(topic, basemqtt._speakerTopic) == 0)
    {
        switch (neopixel.CheckState(result.c_str()))
        {
        case 0:
            log_d("Pump is in an undefined state.");
            break;
        case 1:
            log_d("Pump is not running.");
            break;
        case 2:
            log_d("Pump is running.");
            break;
        case 3:
            log_d("Pump is in Manual.");
            break;
        case 4:
            log_d("Pump is in Automatic.");
            break;
        default:
            break;
        }
    }
    else if (strcmp(topic, basemqtt._waterlevelTopic) == 0)
    {
        switch (neopixel.CheckState(result.c_str()))
        {
        case 0:
            log_d("Pump is in an undefined state.");
            break;
        case 1:
            log_d("Pump is not running.");
            break;
        case 2:
            log_d("Pump is running.");
            break;
        case 3:
            log_d("Pump is in Manual.");
            break;
        case 4:
            log_d("Pump is in Automatic.");
            break;
        default:
            break;
        }
    } */
}

void BASEMQTT::loadMQTTConfig()
{
    log_i("Checking if hostname is set and valid");
    size_t size = sizeof(cfg.config.hostname);
    if (!cfg.isValidHostname(cfg.config.hostname, size - 1))
    {
        heapStr(&cfg.config.hostname, DEFAULT_HOSTNAME);
        cfg.setConfigChanged();
    }

    String MQTT_CLIENT_ID = generateDeviceID();
    const char *mqtt_user = MQTT_USER;
    const char *mqtt_pass = MQTT_PASS;
    char *mqtt_client_id = StringtoChar(MQTT_CLIENT_ID);
    heapStr(&cfg.config.MQTTUser, mqtt_user);
    heapStr(&cfg.config.MQTTPass, mqtt_pass);
    heapStr(&cfg.config.MQTTClientID, mqtt_client_id);
    WiFi.setHostname(cfg.config.hostname); // define hostname
    cfg.setConfigChanged();
    free(mqtt_client_id);

    log_i("Loaded config: hostname %s, MQTT enable relay %s, MQTT host %s, MQTT port %d, MQTT user %s, MQTT pass %s, MQTT topic %s, MQTT set topic %s, MQTT device name %s",
          cfg.config.hostname,
          cfg.config.MQTTBroker,
          cfg.config.MQTTPort,
          cfg.config.MQTTUser,
          cfg.config.MQTTPass,
          cfg.config.MQTTDeviceName);

    log_i("Loaded config: hostname %s", cfg.config.hostname);
}

void BASEMQTT::mqttReconnect()
{
    // Loop until we're reconnected
    while (!mqttClient.connected())
    {
        log_i("Attempting MQTT connection...");
        // Attempt to connect
        if (mqttClient.connect(DEFAULT_HOSTNAME))
        {
            log_i("Connected to MQTT broker.");
            // Subscribe
            mqttClient.subscribe(_pumpTopic);
        }
        else
        {
            log_i("failed, rc= %d", mqttClient.state());
            log_i(" try again in 5 seconds");
            // Wait 15 seconds before retrying
            my_delay(15L);
        }
    }
}

void BASEMQTT::Publish(const char *topic, const char *payload)
{
    if (!mqttClient.publish(topic, payload, true))
    {
        _publishState = false;
        return;
    }
    _publishState = true;
}

void BASEMQTT::mqttLoop()
{
    my_delay(1L);

    if (!mqttClient.connected())
    {
        mqttReconnect();
    }
    else
    {
        mqttClient.loop();
        callback;

        unsigned long currentMillis = millis();
        if (currentMillis - _previousMillis >= _interval)
        {
            _previousMillis = currentMillis;

            if (Serial.available() > 0)
            {
                _user_bytes_received = Serial.readBytesUntil(13, _user_data, sizeof(_user_data));
            }

            if (_user_bytes_received)
            {
                pump.serialReport();
                _user_bytes_received = 0;
                memset(_user_data, 0, sizeof(_user_data));

                Publish(_pumpInTopic, accumulatedata.InitAccumulateDataJson().c_str());
            }
        }
    }
}

BASEMQTT basemqtt;
