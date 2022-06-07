#include "hassmqtt.hpp"

long lastReconnectAttempt = 0;

/**
 * @brief Initialize the MQTT client
 * @param clientId The client ID to use
 * @param host The host to connect to
 * @param port The port to connect to
 * @param secure Whether to use TLS or not
 * @param callback The callback to use
 * <discovery_prefix>/<component>/[<node_id>/]<object_id>/config
 * <component>: One of the supported MQTT components, eg. binary_sensor.
 * <node_id> (Optional): ID of the node providing the topic, this is not used by Home Assistant but may be used to structure the MQTT topic.
 * The ID of the node must only consist of characters from the character class [a-zA-Z0-9_-] (alphanumerics, underscore and hyphen).
 * <object_id>: The ID of the device. This is only to allow for separate topics for each device and is not used for the entity_id.
 * The ID of the device must only consist of characters from the character class [a-zA-Z0-9_-] (alphanumerics, underscore and hyphen).
 * Best practice for entities with a unique_id is to set <object_id> to unique_id and omit the <node_id>.
 **/
//#define MQTT_DISCOVERY_PREFIX "homeassistant/"
// bool mqttProcessing = false;

IPAddress broker_ip;

HADevice device;
HAMqtt mqtt(espClient, device);
HASwitch relay("pump_relay", false); // is unique ID.
HASensor motion_sensor("motion_sensor");
HASensor water_level_sensor("motion_sensor");

HASwitch menu_OnOff("Power", false);
HASwitch menu_plus("plus", false);
HASwitch menu_minus("minus", false);
HASwitch menu_man_aut("man-aut", false);

/***********************************************************************************************************************
 * Class Global Variables
 * Please only make changes to the following class variables within the ini file. Do not change them here.
 **********************************************************************************************************************/
void onMqttMessage(const char *topic, const uint8_t *payload, uint16_t length);
void onMqttConnected();
void onBeforeStateChanged(bool state, HASwitch *s);
void onRelayStateChanged(bool state, HASwitch *s);
void onMqttConnectionFailed();
void onPHStateChanged(bool state, HASwitch *s);
String getBroker();

HASSMQTT::HASSMQTT() : lastReadAt(millis()), lastAvailabilityToggleAt(millis()), lastSentAt(millis())
{
}

HASSMQTT::~HASSMQTT()
{
    // Destructor
}

void HASSMQTT::begin()
{
    lastInputState = digitalRead(pump._pump_relay_pin);
    _pumpTopic = PUMP_TOPIC;
    _pumpInTopic = PUMP_IN_TOPIC;
    _menuTopic = MENU_TOPIC;
    _infoTopic = "user/info";

    // Unique ID must be set!
    String mac = WiFi.macAddress();
    byte buf[100];
    mac.getBytes(buf, sizeof(buf));
    device.setUniqueId(buf, sizeof(buf));

    for (int i = 0; i < 10; i++)
    {
        log_d("%c", buf[i]);
    }
    log_d("\n");

    // connect to broker
    log_i("Connecting to Broker");

    // set device's details (optional)
    strcmp(cfg.config.MQTTDeviceName, DEFAULT_HOSTNAME) != 0 ? device.setName(DEFAULT_HOSTNAME) : device.setName(cfg.config.MQTTDeviceName);
    device.setSoftwareVersion(String(VERSION).c_str());

    // handle switch state
    relay.onBeforeStateChanged(onBeforeStateChanged); // optional
    relay.onStateChanged(onRelayStateChanged);
    relay.setName("Pump Relay"); // optional

    // handle switch state
    menu_OnOff.onBeforeStateChanged(onBeforeStateChanged); // optional
    menu_OnOff.onStateChanged(onPowerStateChanged);
    menu_OnOff.setName(menu_OnOff.getState() ? "On" : "Off"); // optional

    // handle switch state
    menu_minus.onBeforeStateChanged(onBeforeStateChanged); // optional
    menu_minus.onStateChanged(onMinusStateChanged);
    menu_minus.setName("Minus"); // optional

    // handle switch state
    menu_plus.onBeforeStateChanged(onBeforeStateChanged); // optional
    menu_plus.onStateChanged(onPlusStateChanged);
    menu_plus.setName("Plus"); // optional

    // handle switch state
    menu_man_aut.onBeforeStateChanged(onBeforeStateChanged); // optional
    menu_man_aut.onStateChanged(onManAutStateChanged);
    menu_man_aut.setName("Man Aut"); // optional

    // This method enables availability for all device types registered on the device.
    // For example, if you have 5 sensors on the same device, you can enable
    // shared availability and change availability state of all sensors using
    // single method call "device.setAvailability(false|true)"
    device.enableSharedAvailability();

    // Optionally, you can enable MQTT LWT feature. If device will lose connection
    // to the broker, all device types related to it will be marked as offline in
    // the Home Assistant Panel.
    device.enableLastWill();

    // configure sensors
    motion_sensor.setUnitOfMeasurement("Motion");
    motion_sensor.setDeviceClass("motion");
    motion_sensor.setIcon("mdi:run");
    motion_sensor.setName("Purbright Motion Sensor");

    water_level_sensor.setUnitOfMeasurement("L");
    water_level_sensor.setDeviceClass("sensor");
    water_level_sensor.setIcon("mdi:water-percent");
    water_level_sensor.setName("Water Level");

    mqtt.onMessage(onMqttMessage);
    mqtt.onConnected(onMqttConnected);
    mqtt.onConnectionFailed(onMqttConnectionFailed);

    mqtt.setDiscoveryPrefix("Greenhouse_Tower");

#if MQTT_SECURE
    mqtt.begin(broker_ip.fromString(getBroker()), cfg.config.MQTTUser, cfg.config.MQTTPass);
#else
    mqtt.begin(broker_ip.fromString(getBroker()));
#endif // MQTT_SECURE
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

void onMqttMessage(const char *topic, const uint8_t *payload, uint16_t length)
{
    // This callback is called when message from MQTT broker is received.
    // Please note that you should always verify if the message's topic is the one you expect.
    // For example: if (memcmp(topic, "myCustomTopic") == 0) { ... }
    log_i("New message on topic: %s", topic);
    log_i("Data: %s", (const char *)payload);

    String result;
    log_i("Message arrived on topic: [%s] ", topic);
    for (int i = 0; i < length; i++)
    {
        log_i("payload is: [%s]", (char)payload[i]);
        result += (char)payload[i];
    }
    log_i("Message: [%s]", result.c_str());

    const char *_message = "";

    // Check if the message is for the current topic
    if (strcmp(topic, hassmqtt._pumpTopic) == 0)
    {

        switch (pump.CheckState(result.c_str()))
        {
        case 0:
            log_d("Pump is in an undefined state.");
            _message = "[ERROR:] - UNKNOWN";
            break;
        case 1:
            log_d("Pump is not running.");
            _message = "[INFO:] - PUMP IS NOT RUNNING";
            break;
        case 2:
            log_d("Pump is running.");
            _message = "[INFO:] - PUMP IS RUNNING";
            break;
        case 3:
            log_d("Pump is in Manual.");
            _message = "[INFO:] - PUMP IS IN MANUAL";
            break;
        case 4:
            log_d("Pump is in Automatic.");
            _message = "[INFO:] - PUMP IS IN AUTOMATIC";
            break;
        case 5:
            log_d("Generating Serial Report: , ", pump.serialReport());
            _message = "[INFO:] - PUMP IS IN AUTOMATIC";
            break;
        default:
            break;
        }
        mqtt.publish(hassmqtt._infoTopic, _message);
    }
    else if (strcmp(topic, hassmqtt._menuTopic) == 0)
    {
        switch (buttons.CheckState(result.c_str()))
        {
        case 1:
            log_d("Litter Box has been turned off.");
            _message = "OFF";
            break;
        case 2:
            log_d("Litter Box has been turned on.");
            _message = "ON";
            break;
        case 3:
            log_d("Plus button has been pressed");
            _message = "PlUS";
            break;
        case 4:
            log_d("Minus button has been pressed");
            _message = "MINUS";
            break;
        default:
            break;
        }
    }
    mqtt.publish(hassmqtt._infoTopic, _message);
    mqtt.publish("greenhouse_tower_greeting", "Hello from the Greenhouse Tower!");
}

void onMqttConnected()
{
    // You can subscribe to custom topic if you need
    log_i("Connected to the broker!");
    log_i("Subscribing to the topic: %s", hassmqtt._pumpTopic);
    log_i("Subscribing to the topic: %s", hassmqtt._menuTopic);
    mqtt.subscribe(hassmqtt._pumpTopic);
    mqtt.subscribe(hassmqtt._menuTopic);
}

void onMqttConnectionFailed()
{
    log_e("Failed to connect to the broker!");
}

void onBeforeStateChanged(bool state, HASwitch *s)
{
    // this callback will be called before publishing new state to HA
    // in some cases there may be delay before onStateChanged is call relay due to network latency
}

void onRelayStateChanged(bool state, HASwitch *s)
{
    // Relay Control
    pump.setPump(state);
    log_i("switching state of pin: %s\n", state ? "HIGH" : "LOW");
    pump._pump_state_t = state ? pump.PUMP_ON : pump.PUMP_OFF;
}

void onManAutStateChanged(bool state, HASwitch *s)
{
    log_i("switching state of ManAut: %s\n", state ? "HIGH" : "LOW");
    pump._pump_state_t = state ? pump.PUMP_MANUAL : pump.PUMP_AUTOMATIC;
}

void onPowerStateChanged(bool state, HASwitch *s)
{
    log_i("switching state of Power: %s\n", state ? "HIGH" : "LOW");
    buttons._buttons_state_t = state ? buttons.Buttons_ON : buttons.Buttons_OFF;
}

void onPlusStateChanged(bool state, HASwitch *s)
{
    log_i("switching state of Plus: %s\n", state ? "HIGH" : "LOW");
    buttons._buttons_state_t = buttons.Buttons_PLUS;
}

void onMinusStateChanged(bool state, HASwitch *s)
{
    log_i("switching state of Minus: %s\n", state ? "HIGH" : "LOW");
    buttons._buttons_state_t = buttons.Buttons_MINUS;
}

/**
 * @brief Check if the current hostname is the same as the one in the config file
 * Call in the Setup BEFORE the WiFi.begin()
 * @param None
 * @return None
 */
void HASSMQTT::loadMQTTConfig()
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

void HASSMQTT::mqttLoop()
{
    mqtt.loop();
    if ((millis() - lastReadAt) > 30)
    { // read in 30ms interval
        // library produces MQTT message if a new state is different than the previous one
        relay.setState(digitalRead(pump._pump_relay_pin));
        lastInputState = relay.getState();
        lastReadAt = millis();
    }

    if ((millis() - lastAvailabilityToggleAt) > 5000)
    {
        device.setAvailability(!device.isOnline());
        lastAvailabilityToggleAt = millis();
    }

    if ((millis() - lastSentAt) >= 5000)
    {
        lastSentAt = millis();
        water_level_sensor.setValue(accumulatedata.config.water_level);
        motion_sensor.setValue(accumulatedata.config.motion_sensor);

        // Supported data types:
        // uint32_t (uint16_t, uint8_t)
        // int32_t (int16_t, int8_t)
        // double
        // float
        // const char*
    }
}

HASSMQTT hassmqtt;