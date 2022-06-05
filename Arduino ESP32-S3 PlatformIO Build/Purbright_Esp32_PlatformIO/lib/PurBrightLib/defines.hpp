/*
 defines.hpp - ESP32GreenHouseDIY library
 Copyright (c) 2021 ZanzyTHEbar
 */
#ifndef DEFINES_hpp
#define DEFINES_hpp
#if !(defined(ESP32))
#error This code is intended to run on the ESP32 platform! Please check your Board setting.
#endif

// IO
#define LED_BUILTIN 2

#include <Arduino.h>
#include <stdio.h>  /* printf, NULL */
#include <stdlib.h> /* strtoul */
#include <ArduinoJson.h>
#include <strTools.h>
#include <Adafruit_NeoPixel.h>

// File System
#include <SPIFFS.h>

// data Struct
#include "data/config.hpp"

// IO
#include <Wire.h>
#include <io/Neopixel/neopixel.hpp>
#include <io/speaker/speaker.hpp>

#include <sensors/pir/pir.hpp>

#if ENABLE_I2C_SCANNER
#include "io/i2cScanner/i2cscan.hpp"
#endif // ENABLE_I2C_SCANNER

// Network definitions
#include "network/network.hpp"
#include "network/ntp.hpp"

/*######################## MQTT Configuration ########################*/
// MQTT includes
/* #if ENABLE_MDNS_SUPPORT
#include "mqtt/mDNS_Discovery/mDNS.hpp"
#endif // ENABLE_MDNS_SUPPORT
#if ENABLE_HASS
#include "mqtt/HASSIO/hassmqtt.hpp"
#else
#include "mqtt/BASIC/basicmqtt.hpp"
#endif // ENABLE_HASS */
/*###################### MQTT Configuration END ######################*/

// Relays and other IO
#include "io/Relays/Relays.hpp"
#include "io/Pump/pump.hpp"
#include <io/Buttons/buttons.hpp>

// Accumulate Data
#include "data/accumulatedata.hpp"

// Timed tasks
#include "data/timedtasks.hpp"

/**
 * @brief The below Macros print data to the terminal during compilation.
 * !TODO("");
 * !TODO(Variable);
 * !Message("");
 * !Reminder("");
 * !Feature("");
 */
#define Stringize(L) #L
#define MakeString(M, L) M(L)
#define $Line MakeString(Stringize, __LINE__)

#define Reminder __FILE__ "(" $Line ") : Reminder:: "
#define Feature __FILE__ "(" $Line ") : Feature:: "

#define _STR(x) #x
#define STR(x) _STR(x)
#define TODO(x) _Pragma(STR(message("TODO: " STR(x) "::" __FILE__ "@" \
                                                    "(" $Line ")")))
#define Message(desc) _Pragma(STR(message(__FILE__ "(" $Line "):" #desc)))

/**
 * To be used for the Serial Monitor in Release Mode.
 *
 * @brief ANSI color codes
 * Windows users can use these to color the terminal output.
 * @note This is not supported on Linux.
 *
 * Green : \e[32m -or- \e[1;32m
 * Red : \e[31m -or- \e[1;31m
 * Yellow : \e[33m -or- \e[1;33m
 * Blue : \e[34m -or- \e[1;34m
 * Magenta : \e[35m -or- \e[1;35m
 * Cyan : \e[36m -or- \e[1;36m
 * Reset : \e[0m -or- \e[m
 * End : \e[37m -or- \e[1;37m
 *
 * Linux users can use these to color the terminal output.
 * @note This is not supported on Windows.
 *
 * Green : \033[32m -or- \033[1;32m
 * Red : \033[31m -or- \033[1;31m
 * Yellow : \033[33m -or- \033[1;33m
 * Blue : \033[34m -or- \033[1;34m
 * Magenta : \033[35m -or- \033[1;35m
 * Cyan : \033[36m -or- \033[1;36m
 * Reset : \033[0m -or- \033[m
 * End : \033[37m -or- \033[1;37m
 *
 * @see https://en.wikipedia.org/wiki/ANSI_escape_code
 */

// Globally available functions
char *StringtoChar(String inputString);
char *appendChartoChar(const char *hostname, const char *def_host);
void my_delay(volatile long delay_time);
String generateDeviceID();

#define NeoPixelPIN 12
extern Adafruit_NeoPixel strip;

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

extern unsigned long time_now;

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

#endif // DEFINES_hpp
