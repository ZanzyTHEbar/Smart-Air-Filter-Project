/*
 defines.h - Purbright library
 Copyright (c) 2021 Zacariah Austin Heim.
 */
#pragma once
#ifndef DEFINES_hpp
#define DEFINES_hpp

#define ESP32_RTOS
#define VERSION "0.0.1"
#define VERSION_DATE "2021-12-17"

// Arduino
#include <Arduino.h>
#include "debug.hpp"
#include "timedtasks.hpp"

// FreeRTOS and File System

#include "config.hpp" /* software data Struct */
#include "accumulatedata.hpp"
#include <SPIFFS.h>
#include <stdio.h>  /* prextern intf, NULL */
#include <stdlib.h> /* strtoul */
#include <ArduinoJson.h>
#include "purrmqtt.hpp"

// IO
#include <Wire.h>
#include "PIR.hpp"
#include "i2cscan.hpp"
#include "resizeBuff.h"

// wifi definition
#include <WiFiClient.h>

#include "network.hpp"

// Software Stack
#include <Adafruit_NeoPixel.h>
#include <arduino-timer.h>
//#include <speaker.h>
#include "neopixel.hpp"
#include "pump.hpp"
#include "buttons.hpp"
#define NeoPixelPIN 12
extern Adafruit_NeoPixel strip;

extern WiFiClient espClient;
extern StaticJsonDocument<1000> Doc;
// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
#define BAUD_RATE 115200
#define BUZZER_PIN 10
extern const char *itchy;

// define EEPROM settings
// https://www.kriwanek.de/index.php/de/homeautomation/esp8266/364-eeprom-für-parameter-verwenden
// define debugging MACROS
#define DEFAULT_HOSTNAME "PURBRIGHT_PURFECT" // default hostname
#define ENABLE_MULTICAST_DNS                 // allows to access the UI via "http://<HOSTNAME>.local/"
#define ENABLE_MQTT_SUPPORT                  // allows extern integration in homeassistant/googlehome/mqtt
/*######################## MQTT Configuration ########################*/
#ifdef ENABLE_MQTT_SUPPORT
// these are deafault settings which can be changed in the web extern interface "settings" page
#define MQTT_ENABLED 1
#define MQTT_SECURE_ENABLED 0
#define MQTT_PORT 1883
#define MQTT_PORT_SECURE 8883
#define MQTT_MAX_PACKET_SIZE 1024
#define MQTT_MAX_TRANSFER_SIZE 1024
#define MQTT_HOSTNAME "PURBRIGHT_PURFECT"
#define HOMEASSISTANT_MQTT_HOSTNAME "homeassistant.local"

#define MQTT_USER "MyUserName"
#define MQTT_PASS ""
#define MQTT_HOMEASSISTANT_TOPIC_SET "/set"                // MQTT Topic to subscribe to for changes(Home Assistant)
#define MQTT_HOMEASSISTANT_TOPIC "homeassistant/HBAT/data" // MQTT Topic to Publish to for state and config (Home Assistant);

// MQTT includes
#include <PubSubClient.h>

extern PubSubClient mqttClient;

// Variables for MQTT
extern char *MQTT_TOPIC; // MQTT Topic to Publish to for state and config (Any MQTT Broker)
extern bool mqttProcessing;

#endif

char *MQTTCreateHostName(const char *hostname, const char *def_host);
char *StringtoChar(String inputString);

extern char *MQTT_UNIQUE_IDENTIFIER;
extern char *Mqtt_Device_Name;

// Variables
extern unsigned long lastMillis;
// IO
extern char *MQTT_UNIQUE_IDENTIFIER; // A Unique Identifier for the device in Homeassistant (MAC Address used by default)
extern const int TouchOnOff;
extern bool TouchOnOffOld;
extern bool TouchOnOffPN;
extern const int TouchManAut;
extern bool TouchManAutOld;
extern bool TouchManAutPN;
extern const int TouchPlus;
extern bool TouchPlusOld;
extern bool TouchPlusPN;
extern const int TouchMinus;
extern bool TouchMinusOld;
extern bool TouchMinusPN;
// Wifi Variables
//  Set these to your desired credentials.
extern char *ssid;
extern char *password;
extern char *ip;
extern bool wifiMangerPortalRunning;
extern bool wifiConnected;

extern const char *mqtt_mDNS_clientId;

extern int mqttPort;

extern int period;
extern unsigned long time_now;
extern bool Charge_State;

// Device States
extern bool S_OnOff;
extern int Step_Manual;
extern int Step_Automatic;
extern bool S_ManAut;
extern int Step_Menu;
extern bool S_Menu;
extern int Step_Error;
extern bool S_Error;

// Alarms
extern bool NoWaterFlow;

// Timers
extern unsigned long DelayStartPump;
extern bool DelayRunningPump;
extern unsigned long DelayStartSettings;
extern bool DelayRunningSettings;

// Settings
extern int PumpSetting;

// IO
#define LED_BUILTIN 2

#endif
