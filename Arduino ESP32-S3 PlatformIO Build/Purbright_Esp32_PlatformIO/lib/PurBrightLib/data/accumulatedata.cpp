#include "accumulatedata.hpp"

AccumulateData::AccumulateData()
{
}

AccumulateData::~AccumulateData()
{
}

/******************************************************************************
 * Function: Accumulate Data to send from sensors and store in json
 * Description: This function accumulates all sensor data and stores it in the main data structure.
 * Parameters: None
 * Return: None
 ******************************************************************************/
void AccumulateData::InitAccumulateData()
{
    // Relays
    for (int i = 0; i < sizeof(cfg.config.relays_pin) / sizeof(cfg.config.relays_pin[0]); i++)
    {
        Relay.RelayOnOff(cfg.config.relays_pin[i], cfg.config.relays[i]);
        log_d("Relay on pin: %d is %s", cfg.config.relays_pin[i], cfg.config.relays[i] ? "on" : "off");
    }
}

bool AccumulateData::SendData()
{
    // Send the data to the server - Use ArduinoJson library
    return false;
    TODO("Implement FEATURE");
}

void AccumulateData::InitAccumulateDataJson()
{
    // Send the data to the server - Use ArduinoJson library
    TODO("Implement FEATURE");
    StaticJsonDocument<1024> Doc;
    JsonObject json = Doc.to<JsonObject>();
    const byte PumpRelay = 14;
    const byte MotionSensor = 21;

    // Stack Data to send
    bool PIR_state;
    if (digitalRead(MotionSensor) ? PIR_state = "true" : PIR_state = "false")
        ;
    Doc["Motion_Sensor_state"] = PIR_state;
    // Doc["MotionSensor_Last_Updated"] = millis(); //TODO: Implement proper time stamping for PIR sensor activation
    bool Pump_state;
    if (digitalRead(PumpRelay) ? Pump_state = "true" : Pump_state = "false")
        ;
    Doc["Pump_State"] = Pump_state;
    // Doc["PUR_PUMP_FLOWRATE"] = ;
    // Doc["PUR_WATER_LEVEL"] = ;

    // Speakers
    JsonArray Speakers = Doc.createNestedArray("PUR_SPEAKERS");
    bool speakers[2] = {1, 2};
    for (int i = 0; i < 2; i++)
    {
        Speakers.add(speakers[i]);
    }

    // Speakers
    JsonArray SpeakersState = Doc.createNestedArray("PUR_SPEAKERS_State");
    bool speakersstate[2] = {1, 2};
    for (int i = 0; i < 2; i++)
    {
        SpeakersState.add(speakersstate[i]);
        bool speakers_state[2] = {1, 2};
        SpeakersState.add("Relay" + String(i) + "State: ");
        if (digitalRead(speakers[i]) ? speakers_state[i] = true : speakers_state[i] = false)
            ;
    }
}

AccumulateData accumulatedata;
