/*
 PurrMqtt.h - HBAT MQTT library
 Copyright (c) 2021 Zacariah Austin Heim.
 */
#pragma once
#ifndef PURRMQTT_hpp
#define PURRMQTT_hpp
#include "defines.hpp"

class PurrMqtt
{
public:
  // Constructor
  PurrMqtt(void);
  virtual ~PurrMqtt(void);
  // Initialize the library
  //void mqttSendStatus();
  void mqttCallback(char *topic, byte *payload, unsigned int length);
  void MQTTSetup();
  int MQTTLoop();
  void RunMqttService();
  void MessageReceived(char topic[], char payload[]);
  void MQTTPublish(char topic[], char payload[]);
  int CheckWifiState();
  int ReConnect();
  /* void mqttSendStatus();
  void callback(char *topic, byte *message, unsigned int length); */

private:
};
#endif