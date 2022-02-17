
#include "purrmqtt.hpp"

PurrMqtt::PurrMqtt(void)
{
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
    topic_data = MQTTCreateHostName(cfg.config.MQTTTopic, json); // append string two to the result.
    mqttClient.publish(topic_data, JSONmessage, n, true);
    SERIAL_DEBUG_LNF("Sending MQTT package: %s", Doc.as<String>().c_str());
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
  if (WiFi.status() != WL_CONNECTED)
  {
    int numberOfAttempts = 0;

    while (WiFi.status() != WL_CONNECTED)
    {
      numberOfAttempts++;
      delay(200);
      SERIAL_DEBUG_ADD(".");
      if (numberOfAttempts > 100)
      {
        return false;
        break;
      }
    }
  }
  else
  {
    // Loop until we're reconnected
    while (!mqttClient.connected())
    {
      Serial.print("Attempting MQTT connection...");
      // Attempt to connect
      if (mqttClient.connect(cfg.config.MQTTDeviceName))
      {
        Serial.println("connected");
        // Subscribe
        char *topic_sub;
        const char *led = "/LED";
        topic_sub = MQTTCreateHostName(cfg.config.MQTTTopic, led); // append string two to the result.
        mqttClient.subscribe(topic_sub);

        // Publish
        char *topic_pub;
        const char *test = "/test";
        topic_pub = MQTTCreateHostName(cfg.config.MQTTTopic, test); // append string two to the result.
        mqttClient.publish(topic_pub, "hello world");
        return 1;
      }
      else
      {
        Serial.print("failed, rc=");
        Serial.print(mqttClient.state());
        Serial.println(" trying again in 5 seconds");
        // Wait 5 seconds before retrying
        return 0;
      }
    }
  }
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

  bool PurrMqtt::MQTTSetup()
  {
    // MQTT client
    if (WiFi.status() != WL_CONNECTED)
    {
      Serial.println("WiFi not connected");
      SERIAL_DEBUG_LN("MQTT setup failed! Please connect your device to a WiFi network.");
      return false;
    }
    else
    {
      auto getPort = [](bool secure)
      {
        return secure ? MQTT_PORT_SECURE : MQTT_PORT;
      };
      auto port = getPort(cfg.config.MQTTSecureState);
      auto host = cfg.config.MQTTBroker;
      mqttClient.setServer(host, port);
      mqttClient.setCallback(callback);
      return true;
    }
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

    if (!mqttClient.connected() && cfg.config.MQTTEnabled != 0)
    {
      mqttConnected = false;
      SERIAL_DEBUG_LN("MQTT not connected, attempting to reconnect...")
      //MQTTLoop();
    }
    if (mqttClient.connected() && cfg.config.MQTTEnabled != 0)
    {
      SERIAL_DEBUG_BOL
      SERIAL_DEBUG_ADD("Connecting to MQTT...")
      mqttConnected = true;
      cfg.config.MQTTConnectedState = 1;
      cfg.setConfigChanged();
      SERIAL_DEBUG_LN("MQTT connected!")

      if (MQTTLoop())
      {
        SERIAL_DEBUG_ADD("connected\n")

        SERIAL_DEBUG_LN("Subscribing to MQTT Topics")

        char *mqttSetTopicS = NULL;
        heapStr(&mqttSetTopicS, "~");
        char *setTopic = NULL;
        heapStr(&setTopic, MQTTCreateHostName(mqttSetTopicS, cfg.config.MQTTTopic));
        DynamicJsonDocument JSONencoder(4096);
        JSONencoder["~"] = cfg.config.MQTTTopic,
        JSONencoder["name"] = cfg.config.MQTTDeviceName,
        JSONencoder["dev"]["ids"] = MQTT_UNIQUE_IDENTIFIER,
        JSONencoder["dev"]["mf"] = "PurrBright",
        JSONencoder["dev"]["mdl"] = VERSION,
        JSONencoder["dev"]["name"] = cfg.config.MQTTDeviceName,
        JSONencoder["stat_t"] = "~",
        JSONencoder["cmd_t"] = setTopic,
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
        char *mqttConfigTopic = NULL;
        heapStr(&mqttConfigTopic, MQTTCreateHostName("/config", cfg.config.MQTTTopic));
        bool publish = mqttClient.beginPublish(mqttConfigTopic, n, true);
        if (publish == true)
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
  }

  PurrMqtt purrMqtt;