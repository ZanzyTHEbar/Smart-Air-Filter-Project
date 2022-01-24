/*
 defines.cpp - Purbright library
 Copyright (c) 2021 Zacariah Austin Heim.
*/
#include "defines.hpp"

WiFiClient espClient;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(12, NeoPixelPIN, NEO_GRB + NEO_KHZ800);
StaticJsonDocument<1000> Doc;
IPAddress mqttServer;
AsyncWebServer webServer(80);
#if !(USING_ESP32_S2 || USING_ESP32_C3)
DNSServer dnsServer;
#endif

const char *itchy = "itchy:d=8,o=6,b=160:c,a5,4p";

/*######################## MQTT Configuration ########################*/
#ifdef ENABLE_MQTT_SUPPORT
// MQTT includes
#include <PubSubClient.h>
PubSubClient mqttClient(espClient);

// Variables for MQTT
const char *MQTT_TOPIC = "purr/data";
const String HOMEASSISTANT_MQTT_HOSTNAME = "homeassistant.local";
const String MQTT_HOSTNAME = "PURBRIGHT_LITTER_BOX";
const String MQTT_USER = "MyUserName";
const String MQTT_PASS = "";
const String MQTT_HOMEASSISTANT_TOPIC_SET = "/set";                // MQTT Topic to subscribe to for changes(Home Assistant)
const String MQTT_HOMEASSISTANT_TOPIC = "homeassistant/HBAT/data"; // MQTT Topic to Publish to for state and config (Home Assistant);
String MQTT_DEVICE_NAME = "HBAT_HMS" + MQTT_UNIQUE_IDENTIFIER;     // MQTT Topic to Publish to for state and config (Any MQTT Broker)
bool mqttProcessing = false;

#endif



// Global Functions

char *StringtoChar(String inputString)
{
  char *outputString = new char[inputString.length() + 1];
  strcpy(outputString, inputString.c_str());
  return outputString;
}

// Variables
unsigned long lastMillis;
// IO
const int TouchOnOff = 32;
bool TouchOnOffOld = false;
bool TouchOnOffPN = false;
const int TouchManAut = 34;
bool TouchManAutOld = false;
bool TouchManAutPN = false;
const int TouchPlus = 33;
bool TouchPlusOld = false;
bool TouchPlusPN = false;
const int TouchMinus = 25;
bool TouchMinusOld = false;
bool TouchMinusPN = false;
const int FlowSensor = 6;
const int MotionSensor = 21;
const int PumpRelay = 14;

// Wifi Variables
//  Set these to your desired credentials.
char *ssid = StringtoChar("PURR");
char *password = StringtoChar("purrpurr");
bool wifiMangerPortalRunning = false;
bool wifiConnected = false;

const char *mqtt_mDNS_clientId = StringtoChar(MQTT_DEVICE_NAME);
char *mDNS_hostname = StringtoChar(MQTT_HOSTNAME);

int mqttPort;

int period = 500;
unsigned long time_now = 0;
bool Charge_State;

// Device States
bool S_OnOff = false;
int Step_Manual = 10;
int Step_Automatic = 20;
bool S_ManAut = false;
int Step_Menu = 30;
bool S_Menu = false;
int Step_Error = 40;
bool S_Error = false;

// Alarms
bool NoWaterFlow = false;

// Timers
unsigned long DelayStartPump = 0;
bool DelayRunningPump = false;
unsigned long DelayStartSettings = 0;
bool DelayRunningSettings = false;

// Settings
int PumpSetting = 2;

