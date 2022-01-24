#include "purrmqtt.hpp"

PurrMqtt::PurrMqtt(void)
{
  MQTTSetup();
}

PurrMqtt::~PurrMqtt(void)
{
}

//############################## MQTT HELPER FUNCTIONS ##############################

void mqttSendStatus()
{
  uint8_t JSONmessage[1000];
  size_t n = serializeJson(Doc, JSONmessage);
  if (!mqttProcessing)
  {
    char *topic_data;
    const char *json = "/json_data";
    topic_data = (char *)calloc(strlen(cfg.MQTTTopic) + strlen(json) + 1, sizeof(char));
    strcpy(topic_data, cfg.MQTTTopic);
    strcat(topic_data, json); // append string two to the result.
    mqttClient.publish(topic_data, JSONmessage, n, true);
    SERIAL_DEBUG_LNF("Sending MQTT package: %s", Doc.as<String>().c_str())
    free(topic_data);
  }
}

void callback(char *topic, byte *message, unsigned int length)
{
  mqttProcessing = true;
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  char *messageTemp;

  for (int i = 0; i < length; i++)
  {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  DynamicJsonDocument Docdynamic(1024);
  deserializeJson(Docdynamic, messageTemp, length);

  SERIAL_DEBUG_LNF("Received MQTT package: %s", Docdynamic.as<String>().c_str())

  JsonObject obj = Docdynamic.as<JsonObject>();
  for (JsonPair p : obj)
  {
    const char *key = p.key().c_str();
    JsonVariant v = p.value();

    if (strcmp(key, "state") == 0)
    {
      String val = v.as<String>();
      val.toLowerCase();
      if (val == String("on"))
      {
        // setPower(1);
      }
      else if (val == String("off"))
      {
        // setPower(0);
      }
      else if (val == String("toggle"))
      {
        // setPower((power == 1) ? 0 : 1);
      }
    }
  }
  mqttProcessing = false;
  mqttSendStatus();
}
//############################## MQTT HELPER FUNCTIONS END ##############################//

int PurrMqtt::ReConnect() // TODO:Call this in the Loop pinned to task for core 0
{
  Network *network = new Network();
  int wifi_status = network->CheckWifiState();
  if (wifi_status == 1)
  {
    // Loop until we're reconnected
    while (!mqttClient.connected())
    {
      Serial.print("Attempting MQTT connection...");
      // Attempt to connect
      if (mqttClient.connect(cfg.MQTTDeviceName))
      {
        Serial.println("connected");
        // Subscribe
        char *topic_sub;
        const char *led = "/LED";
        topic_sub = (char *)calloc(strlen(cfg.MQTTTopic) + strlen(led) + 1, sizeof(led && cfg.MQTTTopic));
        strcpy(topic_sub, cfg.MQTTTopic);
        strcat(topic_sub, led); // append string two to the result.
        mqttClient.subscribe(topic_sub);
        free(topic_sub);

        // Publish
        char *topic_pub;
        const char *test = "/test";
        topic_pub = (char *)calloc(strlen(cfg.MQTTTopic) + strlen(test) + 1, sizeof(test && cfg.MQTTTopic));
        strcpy(topic_pub, cfg.MQTTTopic);
        strcat(topic_pub, test); // append string two to the result.
        mqttClient.publish(topic_pub, "hello world");
        free(topic_pub);
        return 1;
      }
      else
      {
        Serial.print("failed, rc=");
        Serial.print(mqttClient.state());
        Serial.println(" try again in 5 seconds");
        // Wait 5 seconds before retrying
        return 0;
      }
    }
  }
  else
  {
    return 0;
  }
  delete network;
  return 0;
}

void PurrMqtt::MessageReceived(char topic[], char payload[])
{
  SERIAL_DEBUG_LN("incoming: " + String(topic) + " - " + String(payload));

  // Note: Do not use the mqtt in the callback to publish, subscribe or
  // unsubscribe as it may cause deadlocks when other things arrive while
  // sending and receiving acknowledgments. Instead, change a global variable,
  // or push to a queue and handle it in the loop after calling `mqtt.loop()`.
}

void PurrMqtt::MQTTSetup()
{
  Network *network = new Network();
  // MQTT client
  if (network->CheckWifiState() != 0)
  {
    auto getPort = [](bool secure)
    {
      return secure ? MQTT_PORT_SECURE : MQTT_PORT;
    };
    auto port = getPort(cfg.MQTTSecureState);
    char host = cfg.MQTTBroker;
    mqttClient.setServer(host, port);
    mqttClient.setCallback(callback);
  }
  else
  {
    SERIAL_DEBUG_LN("MQTT setup failed! Please connect your device to a WiFi network.");
  }
  delete network;
}

void PurrMqtt::MQTTPublish(char topic[], char payload[])
{
  if (MQTTLoop() == 1)
  {
    unsigned int len = strlen(payload);
    bool publish = mqttClient.publish(topic, payload, len);
    if (publish != false)
    {
      SERIAL_DEBUG_LN("MQTT publish success!");
    }
    else
    {
      SERIAL_DEBUG_LN("MQTT publish failed!");
    }
  }
  else
  {
    if (Serial.available() > 0)
    {
      SERIAL_DEBUG_LN("MQTT not connected, not publishing. Attempting to reconnect...");
    }
  }
}

int PurrMqtt::MQTTLoop()
{
  mqttClient.loop();
  if (ReConnect() != 1)
  {
    // reconnect if connection dropped
    ReConnect();
    return 0;
  }
  else
  {
    return 1;
  }
}

void PurrMqtt::RunMqttService()
{
  static bool mqttConnected = false;

  if (!mqttClient.connected() && cfg.MQTTEnabled != 0)
  {
    mqttConnected = false;
    SERIAL_DEBUG_LN("MQTT not connected, attempting to reconnect...")
    MQTTLoop();
  }
  if (mqttClient.connected() && cfg.MQTTEnabled != 0)
  {
    SERIAL_DEBUG_BOL
    SERIAL_DEBUG_ADD("Connecting to MQTT...")
    mqttConnected = true;
    cfg.MQTTConnectedState = 1;
    setConfigChanged();
    SERIAL_DEBUG_LN("MQTT connected!")

    if (MQTTLoop())
    {
      SERIAL_DEBUG_ADD("connected\n")

      SERIAL_DEBUG_LN("Subscribing to MQTT Topics")
      char mqttSetTopicC[129];
      strlcpy(mqttSetTopicC, MQTT_TOPIC, sizeof(mqttSetTopicC));
      strlcat(mqttSetTopicC, cfg.MQTTSetTopic, sizeof(mqttSetTopicC));
      mqttClient.subscribe(mqttSetTopicC);

      char mqttSetTopicS[66];
      strcpy(mqttSetTopicS, "~");
      strlcat(mqttSetTopicS, cfg.MQTTSetTopic, sizeof(mqttSetTopicS));

      DynamicJsonDocument JSONencoder(4096);
      JSONencoder["~"] = cfg.MQTTTopic,
      JSONencoder["name"] = cfg.MQTTDeviceName,
      JSONencoder["dev"]["ids"] = MQTT_UNIQUE_IDENTIFIER,
      JSONencoder["dev"]["mf"] = "PurrBright",
      JSONencoder["dev"]["mdl"] = VERSION,
      JSONencoder["dev"]["name"] = cfg.MQTTDeviceName,
      JSONencoder["stat_t"] = "~",
      JSONencoder["cmd_t"] = mqttSetTopicS,
      JSONencoder[""] = true,
      JSONencoder[""] = true,
      JSONencoder[""] = true,
      JSONencoder["uniq_id"] = MQTT_UNIQUE_IDENTIFIER,
      JSONencoder["schema"] = "json";

      /* JsonArray sensors_list = JSONencoder.createNestedArray("sensors_list");
      for (uint8_t i = 0; i < sensorCount; i++)
      {
        sensors_list.add(sensors[i].name);
      } */

      size_t n = measureJson(JSONencoder);
      char mqttConfigTopic[85];
      strlcat(mqttConfigTopic, cfg.MQTTTopic, sizeof(mqttConfigTopic));
      strcat(mqttConfigTopic, "/config");
      if (mqttClient.beginPublish(mqttConfigTopic, n, true) == true)
      {
        SERIAL_DEBUG_LN("Configuration Publishing Begun")
        if (serializeJson(JSONencoder, mqttClient) == n)
        {
          SERIAL_DEBUG_LN("Configuration Sent")
        }
        if (mqttClient.endPublish() == true)
        {
          SERIAL_DEBUG_LN("Configuration Publishing Finished")
          mqttSendStatus();
          SERIAL_DEBUG_LN("Sending Initial Status")
        }
      }
      else
      {
        SERIAL_DEBUG_LN("Error sending Configuration");
      }
    }
    else
    {
      SERIAL_DEBUG_ADDF("failed with state %s\n", mqttClient.state());
    }
  }
  mqttSendStatus();
}
