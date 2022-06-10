#include "accumulatedata.hpp"

AccumulateData::AccumulateData(float seconds) : exeClass(), _pir_sensor(NULL)
{
    float ms = seconds * 1000;
    Timer_5s.setTime(ms);
}

AccumulateData::AccumulateData(void)
{
}

AccumulateData::~AccumulateData()
{
}

void AccumulateData::begin(PIR *pir_sensor)
{
    _pir_sensor = pir_sensor;
}

void AccumulateData::execute()
{
    if (Timer_5s.ding())
    {
        InitAccumulateDataJson();
        Timer_5s.start();
    }
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

String AccumulateData::InitAccumulateDataJson()
{
    // Send the data to the server - Use ArduinoJson library
    TODO("Implement FEATURE");
    StaticJsonDocument<1024> Doc;
    JsonObject json = Doc.to<JsonObject>();
    const byte PumpRelay = 14;
    const byte MotionSensor = 21;

    // Stack Data to send
    bool PIR_state;
    if (digitalRead(MotionSensor) ? PIR_state : !PIR_state)
        ;
    Doc["Motion_Sensor_state"] = PIR_state;
    // Doc["MotionSensor_Last_Updated"] = millis(); //TODO: Implement proper time stamping for PIR sensor activation
    bool Pump_state;
    if (digitalRead(PumpRelay) ? Pump_state : !Pump_state)
        ;
    Doc["Pump_State"] = Pump_state;
    // Doc["PUR_PUMP_FLOWRATE"] = ;
    // Doc["PUR_WATER_LEVEL"] = ;

    // Speakers
    JsonArray Speakers = Doc.createNestedArray("PUR_SPEAKERS");
    bool speakers[2] = {1, 2};
    for (int i = 0; i < 2; i++)
    {
        Doc["Speaker"][i] = Speakers.add(speakers[i]);
    }

    // Speakers
    /* JsonArray SpeakersState = Doc.createNestedArray("PUR_SPEAKERS_State");
    for (int i = 0; i < 2; i++)
    {
        SpeakersState.add(speaker.speakersstate[i]);
        SpeakersState.add("Relay" + String(i) + "State: ");
        if (digitalRead(speakers[i]) ? speaker.speakersstate[i] : speaker.speakersstate[i] = false)
            ;
    } */

    JsonArray PumpData = Doc.createNestedArray("PUR_PUMP_DATA");
    int *pumpPointer = (int *)&pump.pumpdata;
    for (int i = 0; i < sizeof(pumpPointer) / sizeof(pumpPointer[0]); i++)
    {
        PumpData.add(pumpPointer[i]);
    }

    String jsonString;
    auto error = serializeJson(Doc, jsonString);
    if (error)
    {
        log_e("Failed to serialize MQTT JSON File");
    }
    else
    {
        log_d("MQTT Json: %s", jsonString.c_str());
    }
    return jsonString;
}

AccumulateData accumulatedata;
