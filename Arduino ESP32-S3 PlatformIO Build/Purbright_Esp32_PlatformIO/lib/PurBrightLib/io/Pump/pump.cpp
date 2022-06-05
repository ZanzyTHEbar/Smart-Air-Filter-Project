#include "pump.hpp"

/**
 * Schedule of all settings
 */
int pumpSchedule[3][3] = {
    {0, 10, 1}, // Spray the roots at a set interval and duration (Default)
    {1, 10, 1}, // Run the pump (Default)
    {0, 10, 1}  // Night setting (Default)
};
// Day setting set (0:00-23:00)
int pumpScheduleIndex[1][24] = {
    {2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2} // Pump at 12:00
};

PUMP::PUMP() : _runInterval(1),
               _pumpMaxRunTime(0),
               _pumpOn(0),
               _pumpInterval(10),
               _pumpDuration(1),
               _runProgram(1),
               _oneReport(0),
               _t_(0)
{
}

PUMP::~PUMP()
{
}

void PUMP::SetupPumpClass()
{
    _tDelay = _runInterval;
    _pump_relay_pin = PUMP_RELAY_PIN;
    _MotionSensorPin = MOTION_SENSOR_PIN;
    _pumpTopic = PUMP_TOPIC;
    // Setup the pump relay pin
    log_i("Setting up the pump...");
    pinMode(_pump_relay_pin, OUTPUT);
    Relay.RelayOnOff(_pump_relay_pin, false);

    pinMode(_MotionSensorPin, INPUT);
    /* pinMode(WaterLevelSensor, INPUT); */
    // Set the time
    setTime(12, 0, 0, networkntp.getDay().toInt(), networkntp.getMonth().toInt(), networkntp.getYear().toInt()); // hour,min,sec,day,month,year
}

void PUMP::PumpLoopAdvanced()
{

    _t_ = now();
    // Keep working as long as data is in the buffer
    while (Serial.available() > 0)
    {
        serialControl();
    }
    if (_t_ < _tDelay)
    {
        return;
    }

    if (_runProgram == 1)
    {
        scheduleFromUser();

        /**
         * The scheduling is per hour, but running the pump for that long is
         * unnecessary so have a maximum run time that is less than 60 min.
         */
        if (_pumpOn == 1 && minute(_t_) > _pumpMaxRunTime)
        {
            _pumpOn = 0;
        }
        setPump(true);
    }
    if (_pumpInterval - 1 == minute(_t_) % _pumpInterval)
    {
        if (_pumpDuration > second(_t_))
        {
            Relay.RelayOnOff(_pump_relay_pin, true);
            _oneReport = 1;
        }
        if (_pumpDuration <= second(_t_))
        {
            Relay.RelayOnOff(_pump_relay_pin, false);
            if (_oneReport == 1)
            {
                _oneReport = 0;
                serialReport();
            }
        }
    }

    _tDelay = _t_ + _runInterval;
}

void PUMP::PumpLoop()
{
    /* if (S_OnOff)
    {
        // Pump indication
        if (digitalRead(_pump_relay_pin))
        {
            strip.setPixelColor(1, strip.Color(0, 255, 0));
            strip.show();
        }

        if (!S_ManAut)
        {
            strip.setPixelColor(1, strip.Color(233, 225, 85));
            strip.show();
        }
        else
        {
            strip.setPixelColor(1, strip.Color(0, 0, 127));
            strip.show();
        }
    } */

    // Pump indication
    if (S_OnOff && digitalRead(_pump_relay_pin))
    {
        strip.setPixelColor(1, strip.Color(0, 225, 0));
        strip.show();
    }
    else
    {
        strip.setPixelColor(1, strip.Color(0, 0, 0));
        strip.show();
    }
}

void PUMP::scheduleFromUser()
{
    // Change every hour
    int x = hour(_t_); // 0-23
    int z = pumpScheduleIndex[0][x];

    _pumpOn = pumpSchedule[z][0];
    _pumpInterval = pumpSchedule[z][1];
    _pumpDuration = pumpSchedule[z][2];
}

/**
 * Turn the pump on or off depending on the value of the global variable _pumpOn.
 */
void PUMP::setPump(bool state)
{
    Relay.RelayOnOff(_pump_relay_pin, state);
}

String PUMP::timeReport()
{
    String timeString;

    timeString = "Time(dd:hh:mm:ss):      ";
    timeString = timeString + day(_t_);
    timeString = timeString + ":";
    timeString = timeString + hour(_t_);
    timeString = timeString + ":";
    timeString = timeString + minute(_t_);
    timeString = timeString + ":";
    timeString = timeString + second(_t_);
    timeString = timeString + "\n";

    return timeString;
}

void PUMP::serialReport()
{
    String reportValues;

    reportValues = "Settings:\n";
    reportValues = "Pump: " + _pumpOn ? "On" : "Off";
    reportValues = reportValues + "\nMinutes between sprays: ";
    reportValues = reportValues + String(_pumpInterval);
    reportValues = reportValues + "\nSeconds of spray: ";
    reportValues = reportValues + String(_pumpDuration);

    reportValues = "-- Report ----\n" + timeReport() + reportValues;
    Serial.println(reportValues);
}

/*
 * Messages consist of the letter S followed by
 * ,int(0-1),int(0-59),int(0-59)
 * S,0,20,5 (pump,_pumpInterval,_pumpDuration)
 */
void PUMP::serialControl()
{
    char c = Serial.read();
    switch (c)
    {
    case 'S':
    {
        _pumpOn = Serial.parseInt();
        _pumpInterval = Serial.parseInt();
        _pumpDuration = Serial.parseInt();

        setPump(true);
        break;
    }
    case 'R':
    {
        serialReport();
        break;
    }
    case 'Q':
    {
        if (_runProgram == 1)
        {
            _runProgram = 0;
            break;
        }
        if (_runProgram == 0)
        {
            _runProgram = 1;
            break;
        }
        break;
    }
    case 'T':
    {
        int h = Serial.parseInt();                                                        // First valid integer
        int m = Serial.parseInt();                                                        // Second valid integer
        int d = Serial.parseInt();                                                        // Third valid integer
        setTime(h, m, 0, d, networkntp.getMonth().toInt(), networkntp.getYear().toInt()); // hour,min,sec,day,month,year
        _t_ = now();
        _tDelay = _runInterval;
        break;
    }
    }
}

// Timer delay Pump
void PUMP::checkTurnOffPump()
{
    // check if delay has timed out
    if (DelayRunningPump && ((millis() - DelayStartPump) >= ((PumpSetting - 1) * 1200)))
    { // 2min per led
        DelayRunningPump = false;
        digitalWrite(_pump_relay_pin, false);
        strip.setPixelColor(0, strip.Color(0, 0, 0));
        strip.show();
    }
}

// TODO: Save settings to SPIFFS config file to have persistent settings across reboots
//  Manual-Automatic
void PUMP::Man_Aut()
{
    if (S_OnOff && !S_Menu)
    {
        if (S_ManAut)
        {
            S_ManAut = 0;
            Manual();
            log_i("Automatic: %d", S_ManAut);
        }
        else
        {
            S_ManAut = 1;
            setPump(false);
        }
    }
}

// TODO: Save settings to SPIFFS config file to have persistent settings across reboots
//  Manual Code
void PUMP::Manual()
{
    neopixel.colorWipe(strip.Color(233, 225, 85), 50);
    digitalWrite(_pump_relay_pin, HIGH);
    log_i("Relay: %d", digitalRead(_pump_relay_pin));
}

// TODO: Save settings to SPIFFS config file to have persistent settings across reboots
//  Automatic Code
void PUMP::Automatic()
{
    neopixel.colorWipe(strip.Color(0, 0, 127), 70);
    if (digitalRead(_MotionSensorPin))
    {
        DelayStartPump = millis();
        DelayRunningPump = true;
        digitalWrite(_pump_relay_pin, HIGH);
        log_i("Relay: %d", digitalRead(_pump_relay_pin));
    }
}

PUMP pump;