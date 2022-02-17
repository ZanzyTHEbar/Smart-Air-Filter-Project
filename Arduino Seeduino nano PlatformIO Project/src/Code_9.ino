#include <anyrtttl.h>
#include <binrtttl.h>
#include <pitches.h>
#include <Adafruit_NeoPixel.h>
#include <arduino-timer.h>

#ifdef __AVR__
  #include <avr/power.h>
#endif
#define NeoPixelPIN 12
Adafruit_NeoPixel strip = Adafruit_NeoPixel(12, NeoPixelPIN, NEO_GRB + NEO_KHZ800);
// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across

#define BUZZER_PIN 10
const char * itchy = "itchy:d=8,o=6,b=160:c,a5,4p";
// This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
#if defined (__AVR_ATtiny85__)
  if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
#endif

// IO
const int TouchOnOff = 2; bool TouchOnOffOld = false; bool TouchOnOffPN = false;
const int TouchManAut = 3; bool TouchManAutOld = false; bool TouchManAutPN = false;
const int TouchPlus = 4; bool TouchPlusOld = false; bool TouchPlusPN = false;
const int TouchMinus = 5; bool TouchMinusOld = false; bool TouchMinusPN = false;
const int FlowSensor = 6;
const int MotionSensor = 7;
const int PumpRelay = 9;

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

void setup() {
  pinMode(BUZZER_PIN, OUTPUT);
  Serial.begin(9600);
  pinMode(TouchOnOff, INPUT);
  pinMode(TouchManAut, INPUT);
  pinMode(TouchPlus, INPUT);
  pinMode(TouchMinus, INPUT);
  pinMode(FlowSensor, INPUT);
  pinMode(MotionSensor, INPUT);
  pinMode(PumpRelay, OUTPUT);

  strip.begin();
  strip.setBrightness(50);
  strip.show(); // Initialize all pixels to 'off'
  
}

void loop() {
  //Button Check
  //if (digitalRead(TouchOnOff)==1) {Serial.write("ON ");} 
  //if (digitalRead(TouchManAut)==1) {Serial.write("MAN ");}
  //if (digitalRead(TouchPlus)==1) {Serial.write("PLUS ");}
  //if (digitalRead(TouchMinus)==1) {Serial.write("MINUS ");}
  //if (digitalRead(MotionSensor)==1) {Serial.write("MOTION ");}
  //if (S_Menu==false) {Serial.write(S_Menu ? "ON " : "OFF ");}
  
  // On Off
  if ((digitalRead(TouchOnOff)==0) && (TouchOnOffOld==1)) 
    {TouchOnOffPN = 1;}
  else {TouchOnOffPN = 0;}
  TouchOnOffOld = digitalRead(TouchOnOff);

  if (S_OnOff==true) {
  // Manual
  if ((digitalRead(TouchManAut)==0) && (TouchManAutOld==1)) 
    {TouchManAutPN = 1;}
  else {TouchManAutPN = 0;}
  TouchManAutOld = digitalRead(TouchManAut);
  
  // Plus
  if ((digitalRead(TouchPlus)==0) && (TouchPlusOld==1)) 
    {TouchPlusPN = 1;}
  else {TouchPlusPN = 0;}
  TouchPlusOld = digitalRead(TouchPlus);
  
  // Minus
  if ((digitalRead(TouchMinus)==0) && (TouchMinusOld==1)) 
    {TouchMinusPN = 1;}
  else {TouchMinusPN = 0;}
  TouchMinusOld = digitalRead(TouchMinus);
  }
  else {
  //Powerled
    strip.setPixelColor(0, strip.Color(255, 225, 255));
    strip.show();
  }

  // BUTTONS
  if (TouchOnOffPN==1) {OnOff();}
  if ((S_Menu==false) && (TouchManAutPN==1)) {Man_Aut();}
  if ((S_Menu==false) && (TouchPlusPN==1)) {Settings();}
  if ((S_Menu==true) && (TouchPlusPN==1)) {Plus();}
  if ((S_Menu==false) && (TouchMinusPN==1)) {Settings();}
  if ((S_Menu==true) && (TouchMinusPN==1)) {Minus();}

  // Automatic Functionality
  if ((S_OnOff==true) && (S_Menu == false) && (S_ManAut == true)) {Automatic();}

  // Manual Functionality
  if ((S_OnOff==true) && (S_Menu == false) && (S_ManAut == false)) {Manual();}
  
  checkTurnOffPump();
  checkTurnOffSettings();

  // Motion indication
  if ((digitalRead(MotionSensor)==true) && (S_OnOff==true)){
    strip.setPixelColor(0, strip.Color(0,255,0));
    strip.show();
  }
  else if (S_OnOff==true) {
    if (S_ManAut==0) {
      strip.setPixelColor(0, strip.Color(233, 225, 85));
      strip.show();
    }
    else {
      strip.setPixelColor(0, strip.Color(0,0,127));
      strip.show();
    }
  }
  
  // Pump indication
  if (digitalRead(PumpRelay)==true) {
    strip.setPixelColor(1, strip.Color(0,255,0));
    strip.show();
  }
  else if (S_OnOff==true) {
    if (S_ManAut==0) {
      strip.setPixelColor(1, strip.Color(233, 225, 85));
      strip.show();
    }
    else {
      strip.setPixelColor(1, strip.Color(0,0,127));
      strip.show();
    }
  }
}
//----------------------------------------------------------

// OnOff Button Code
void OnOff () {
  if (S_OnOff==0) {
    S_OnOff = 1;
    colorWipeAll(strip.Color(0, 0, 127), 50); // Soft Blue
    S_ManAut = true;
  }
  else {
    S_OnOff = false;
    All_Off ();
  }
}

// All Off
void All_Off (){
  digitalWrite(PumpRelay, false);
  S_ManAut = false;
  Step_Manual = 10;
  Step_Automatic = 20;
  S_Menu = false;
  Step_Menu = 40;
  colorWipeAll(strip.Color(0, 0, 0), 50); // Off
}

// Manual-Automatic
void Man_Aut (){
   if ((S_OnOff==1) && (S_Menu==0)) {
    if (S_ManAut==1) 
      {S_ManAut = 0; Manual ();}
    else {S_ManAut = 1;digitalWrite(PumpRelay, false);}
  }
}

// Manual Code
void Manual (){
  colorWipe(strip.Color(233, 225, 85), 50);
  digitalWrite(PumpRelay, true);
}

// Automatic Code
void Automatic (){
  colorWipe(strip.Color(0, 0, 127), 70);
  if (digitalRead(MotionSensor)==1) {
    DelayStartPump = millis();
    DelayRunningPump = true;
    digitalWrite(PumpRelay, true);
    }
}

// Settings Code
void Settings (){
  for(int i=2;i<strip.numPixels();i++){
    strip.setPixelColor(i, strip.Color(0,0,0));
    strip.show();
  }
  for(int i=2;i<PumpSetting;i++){
    strip.setPixelColor(i, strip.Color(255,255,255));
    strip.show();
  }
  DelayStartSettings = millis();
  DelayRunningSettings = true;
  S_Menu = true;
}
void Plus (){
  if (PumpSetting < 12) {
    PumpSetting = PumpSetting + 1;
    DelayStartSettings = millis();
    for(int i=2;i<PumpSetting;i++) {
      strip.setPixelColor(i, strip.Color(255,255,255));
      strip.show();
    }
  }
}
void Minus (){
  if (PumpSetting > 3) {
    PumpSetting = PumpSetting - 1;
    DelayStartSettings = millis();
    for(int i=PumpSetting;i<strip.numPixels();i++) {
      strip.setPixelColor(i, strip.Color(0,0,0));
      strip.show();
    }
  }
}

// Timer delay Pump
void checkTurnOffPump() {
  // check if delay has timed out
  if (DelayRunningPump && ((millis() - DelayStartPump) >= ((PumpSetting-1) * 120000))) { // 2min per led
    DelayRunningPump = false;
    digitalWrite(PumpRelay, false);
    strip.setPixelColor(0, strip.Color(0,0,0));
    strip.show();
  }
}

// Timer delay Settings
void checkTurnOffSettings() {
  // check if delay has timed out
  if (DelayRunningSettings && ((millis() - DelayStartSettings) >= 4000)) {
    DelayRunningSettings = false;
    S_Menu = false;
  }
}

//NeoPixel Codes-------------------------------------------------
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=2; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}
void colorWipeAll(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}
