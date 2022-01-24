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

// FreeRTOS and File System
#include <EEPROM.h>
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

// wifi definition
#include <AsyncElegantOTA.h>
#include <ESPAsync_WiFiManager.h>
#include <ESPmDNS.h>
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
extern IPAddress mqttServer;
extern AsyncWebServer webServer;
#if !(USING_ESP32_S2 || USING_ESP32_C3)
extern DNSServer dnsServer;
#endif

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
#define BAUD_RATE 115200
#define BUZZER_PIN 10
extern const char *itchy;

// define EEPROM settings
// https://www.kriwanek.de/index.php/de/homeautomation/esp8266/364-eeprom-für-parameter-verwenden
// define debugging MACROS
#define DEFAULT_HOSTNAME "PURBRIGHT_LITTER_BOX" // default hostname
#define ENABLE_MULTICAST_DNS                    // allows to access the UI via "http://<HOSTNAME>.local/"
#define ENABLE_MQTT_SUPPORT                     // allows extern integration in homeassistant/googlehome/mqtt
/*######################## MQTT Configuration ########################*/
#ifdef ENABLE_MQTT_SUPPORT
// these are deafault settings which can be changed in the web extern interface "settings" page
#define MQTT_ENABLED 1
#define MQTT_SECURE_ENABLED 0
#define MQTT_PORT 1883
#define MQTT_PORT_SECURE 8883
#define MQTT_UNIQUE_IDENTIFIER WiFi.macAddress() // A Unique Identifier for the device in Homeassistant (MAC Address used by default)
#define MQTT_MAX_PACKET_SIZE 1024
#define MQTT_MAX_TRANSFER_SIZE 1024
// MQTT includes
#include <PubSubClient.h>

extern PubSubClient mqttClient;

// Variables for MQTT
extern const char *MQTT_TOPIC;
extern const String HOMEASSISTANT_MQTT_HOSTNAME;
extern const String MQTT_HOSTNAME;
extern const String MQTT_USER;
extern const String MQTT_PASS;
extern const String MQTT_HOMEASSISTANT_TOPIC_SET;                // MQTT Topic to subscribe to for changes(Home Assistant)
extern const String MQTT_HOMEASSISTANT_TOPIC; // MQTT Topic to Publish to for state and config (Home Assistant);
extern String MQTT_DEVICE_NAME;     // MQTT Topic to Publish to for state and config (Any MQTT Broker)
extern bool mqttProcessing;

#endif

char *StringtoChar(String inputString);


// Variables
extern unsigned long lastMillis;
// IO
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
extern const int FlowSensor;
extern const int MotionSensor;
extern const int PumpRelay;

// Wifi Variables
//  Set these to your desired credentials.
extern char *ssid;
extern char *password;
extern bool wifiMangerPortalRunning;
extern bool wifiConnected;

extern const char *mqtt_mDNS_clientId;
extern char *mDNS_hostname;

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
