/*
 HMSMqtt.h - ESP32GreenHouseDIY MQTT library
 Copyright (c) 2021 ZanzyTHEbar
 */
#pragma once
#ifndef HAMQTT_HPP
#define HAMQTT_HPP
#include <defines.hpp>
#include <ArduinoHA.h>

class HASSMQTT
{
public:
  // Constructor
  HASSMQTT();
  virtual ~HASSMQTT();

  void loadMQTTConfig();
  void begin();
  void mqttLoop();

  // Friends
  friend class LDR;
  friend void onMqttMessage(const char *topic, const uint8_t *payload, uint16_t length);
  friend void onMqttConnected();

private:
  // Private functions

  // Private variable
  char *_pumpTopic;
  char *_pumpInTopic;
  char *_menuTopic;
  char *_speakerTopic;
  char *_infoTopic;

  unsigned long lastReadAt;
  unsigned long lastAvailabilityToggleAt;
  bool lastInputState;
  unsigned long lastSentAt;
};

extern HASSMQTT hassmqtt;
#endif // HAMQTT_HPP