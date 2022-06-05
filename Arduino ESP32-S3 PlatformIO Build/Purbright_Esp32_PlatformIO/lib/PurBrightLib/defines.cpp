#include <defines.hpp>

Adafruit_NeoPixel strip = Adafruit_NeoPixel(12, NeoPixelPIN, NEO_GRB + NEO_KHZ800);

// Globally available functions
char *StringtoChar(String inputString)
{
  char *outputString;
  outputString = NULL;
  resizeBuff(inputString.length() + 1, &outputString);
  strcpy(outputString, inputString.c_str());
  return outputString;
}

char *appendChartoChar(const char *first, const char *second)
{
  int numBytes = strlen(first) + strlen(second) + 1; // +1 for the null terminator | allocate a buffer of the required size
  char *first_str = NULL;
  resizeBuff(numBytes, &first_str);
  strcpy(first_str, first);
  strcat(first_str, second); // append second to first
  return first_str;
}

void my_delay(volatile long delay_time)
{
  delay_time = delay_time * 1e6L;
  for (volatile long count = delay_time; count > 0L; count--)
    ;
}

// a function to generate the device ID and called generateDeviceID()
String generateDeviceID()
{
  uint32_t chipId = 0;
  for (int i = 0; i < 17; i = i + 8)
  {
    chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
  }

  log_i("ESP32 Chip model = %s Rev %d\n", ESP.getChipModel(), ESP.getChipRevision());
  log_i("This chip has %d cores\n", ESP.getChipCores());
  log_i("Chip ID: %d", chipId);
  String deviceID = String(chipId);
  return deviceID;
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

unsigned long time_now = 0;

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
