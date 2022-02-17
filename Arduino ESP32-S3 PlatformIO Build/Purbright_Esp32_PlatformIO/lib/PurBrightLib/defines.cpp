/*
 defines.cpp - Purbright library
 Copyright (c) 2021 Zacariah Austin Heim.
*/
#include "defines.hpp"

WiFiClient espClient;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(12, NeoPixelPIN, NEO_GRB + NEO_KHZ800);
StaticJsonDocument<1000> Doc;

const char *itchy = "itchy:d=8,o=6,b=160:c,a5,4p";

/*######################## MQTT Configuration ########################*/
#ifdef ENABLE_MQTT_SUPPORT

// MQTT includes
#include <PubSubClient.h>
PubSubClient mqttClient(espClient);

// Variables for MQTT
char *MQTT_TOPIC = "pur/data";

bool mqttProcessing = false;

#endif

// Global Functions

//Code allocates memory - remember to free it using resizeBuff(0, &thing_allocated)

char* MQTTCreateHostName(const char* hostname, const char* def_host)
{
  // create hostname
  int numBytes = strlen(hostname ) + strlen(def_host) + 1;
  char* hostname_str = NULL;
  char* def_host_str = NULL;
  resizeBuff(numBytes, &hostname_str);
  strcpy(hostname_str, hostname);
  strcat(hostname_str, def_host_str);
  return hostname_str;
}

char *StringtoChar(String inputString)
{
  char *outputString;
  outputString = NULL;
  resizeBuff(inputString.length() + 1, &outputString);
  strcpy(outputString, inputString.c_str());
  return outputString;
}

// Variables
unsigned long lastMillis;
// IO
const int TouchOnOff = 32;
bool TouchOnOffOld = false;
bool TouchOnOffPN = false;
const int TouchManAut = 35;
bool TouchManAutOld = false;
bool TouchManAutPN = false;
const int TouchPlus = 33;
bool TouchPlusOld = false;
bool TouchPlusPN = false;                                                                                                    
const int TouchMinus = 27;
bool TouchMinusOld = false;
bool TouchMinusPN = false;

// Wifi Variables
//  Set these to your desired credentials.

char* ssid = "";
char* password = "";
char* ip = "";
char* MQTT_UNIQUE_IDENTIFIER = StringtoChar(WiFi.macAddress());
char* Mqtt_Device_Name = MQTTCreateHostName(MQTT_UNIQUE_IDENTIFIER, DEFAULT_HOSTNAME);   // MQTT Topic to Publish to for state and config (Any MQTT Broker)
bool wifiMangerPortalRunning = false;
bool wifiConnected = false;

const char* mqtt_mDNS_clientId = Mqtt_Device_Name;

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

