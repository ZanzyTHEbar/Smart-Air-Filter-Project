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
HASensor water_temp("water_temp");
#if USE_DHT_SENSOR
HASensor tower_humidity("tower_humidity");
HASensor tower_humidity_temp("tower_humidity_temp");
#endif // USE_DHT_SENSOR
HASensor light("light");

#if USE_SHT31_SENSOR
HASensor sht31_humidity("tower_humidity_sht31");
HASensor sht31_humidity_temp("tower_humidity_temp_sht31");
HASensor sht31_humidity_2("tower_humidity_sht31");
HASensor sht31_humidity_temp_2("tower_humidity_temp_sht31");
#endif // USE_SHT31_SENSOR

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

HASSMQTT::HASSMQTT() : lastReadAt(millis()), lastAvailabilityToggleAt(millis()), lastInputState(digitalRead(pump._pump_relay_pin)), lastSentAt(millis())
{
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
#if USE_DHT_SENSOR
    tower_humidity_temp.setUnitOfMeasurement("°C");
    tower_humidity_temp.setDeviceClass("temperature");
    tower_humidity_temp.setIcon("mdi:thermometer-lines");
    tower_humidity_temp.setName("Tower temperature");

    tower_humidity.setUnitOfMeasurement("%");
    tower_humidity.setDeviceClass("humidity");
    tower_humidity.setIcon("mdi:water-percent");
    tower_humidity.setName("Tower Humidity");
#endif // USE_DHT_SENSOR

    water_temp.setUnitOfMeasurement("°C");
    water_temp.setDeviceClass("temperature");
    water_temp.setIcon("mdi:coolant-temperature");
    water_temp.setName("Tower water temperature");

#if USE_SHT31_SENSOR
    sht31_humidity.setUnitOfMeasurement("%");
    sht31_humidity.setDeviceClass("humidity");
    sht31_humidity.setIcon("mdi:water-percent");
    sht31_humidity.setName("Tower Humidity");

    sht31_humidity_temp.setUnitOfMeasurement("°C");
    sht31_humidity_temp.setDeviceClass("temperature");
    sht31_humidity_temp.setIcon("mdi:thermometer-lines");
    sht31_humidity_temp.setName("Tower Temperature");

    sht31_humidity_2.setUnitOfMeasurement("%");
    sht31_humidity_2.setDeviceClass("humidity");
    sht31_humidity_2.setIcon("mdi:water-percent");
    sht31_humidity_2.setName("Tower Humidity Sensor 2");

    sht31_humidity_temp_2.setUnitOfMeasurement("°C");
    sht31_humidity_temp_2.setDeviceClass("humidity");
    sht31_humidity_temp_2.setIcon("mdi:thermometer-lines");
    sht31_humidity_temp_2.setName("Tower Temperature Sensor 2");
#endif // USE_SHT31_SENSOR

    light.setUnitOfMeasurement("lx");
    light.setDeviceClass("illuminance");
    light.setIcon("mdi:lightbulb");
    light.setName("Light");
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

HASSMQTT::~HASSMQTT()
{
    // Destructor
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

    // Check if the message is for the current device
    if (strcmp(topic, pump._pumpTopic) == 0)
    {
        if (strcmp(result.c_str(), "ON") == 0)
        {
            log_i("Turning on the pump");
            Relay.RelayOnOff(pump._pump_relay_pin, true);
        }
        else if (strcmp(result.c_str(), "OFF") == 0)
        {
            log_i("Turning off the pump");
            Relay.RelayOnOff(pump._pump_relay_pin, false);
        }
    }
#if ENABLE_PH_SUPPORT
    else if (strcmp(topic, phsensor._pHTopic) == 0)
    {
        log_i("Setting pH level to: [%s]", result.c_str());
        phsensor.eventListener(topic, payload, length);
    }
#endif // ENABLE_PH_SUPPORT

    mqtt.publish("greenhouse_tower_info", "Hello from the Greenhouse Tower!");
}

void onMqttConnected()
{
    // You can subscribe to custom topic if you need
    log_i("Connected to the broker!");
    log_i("Subscribing to the topic: %s", pump._pumpTopic);
    mqtt.subscribe(pump._pumpTopic);

    log_i("Subscribing to the topic: %s", phsensor._pHTopic);
    mqtt.subscribe(phsensor._pHTopic);
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
    bool relay = state;
    for (int i = 0; i < sizeof(cfg.config.relays_pin) / sizeof(cfg.config.relays_pin[0]); i++)
    {
        log_i("switching state of pin: %s\n", relay ? "HIGH" : "LOW");
        cfg.config.relays[i] = (cfg.config.relays[i]) ? false : true;
    }
}

#if ENABLE_PH_SUPPORT
void onPHStateChanged(bool state, HASwitch *s)
{
    // PH Control
    bool ph = state;
}
#endif // ENABLE_PH_SUPPORT

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
        tower_humidity_temp.setValue(accumulatedata.config.humidity_temp);
        tower_humidity.setValue(accumulatedata.config.humidity);
        water_temp.setValue(accumulatedata.config.temp_sensors[0]);
        light.setValue(ldr.getLux());
#if USE_SHT31_SENSOR
        sht31_humidity.setValue(lastValue);
        sht31_humidity_temp.setValue(lastValue);
        sht31_humidity_2.setValue(lastValue);
        sht31_humidity_temp_2.setValue(lastValue);
#endif // USE_SHT31_SENSOR

        // Supported data types:
        // uint32_t (uint16_t, uint8_t)
        // int32_t (int16_t, int8_t)
        // double
        // float
        // const char*
    }
}

HASSMQTT hassmqtt;