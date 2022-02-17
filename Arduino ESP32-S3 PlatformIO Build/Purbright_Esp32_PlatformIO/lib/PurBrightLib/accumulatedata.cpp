#include "AccumulateData.hpp"

AccumulateData::AccumulateData()
{
}

AccumulateData::~AccumulateData(void)
{
}

/******************************************************************************
 * Function: Setup Main Loop
 * Description: This is the setup function for the main loop of the whole program. Use this to setup the main loop.
 * Parameters: None
 * Return: None
 ******************************************************************************/
void AccumulateData::SetupMainLoop()
{
  //MqttData.MQTTSetup();
  // debug("freeMemory()="+freeMemory());
}

/******************************************************************************
 * Function: Accumulate Data to send from sensors and store in json
 * Description: This function accumualtes all sensor data and stores it in the main json data structure.
 * Parameters: None
 * Return: None
 ******************************************************************************/
void AccumulateData::InitAccumulateDataJson()
{
  const uint8_t PumpRelay = 14;
  const uint8_t MotionSensor = 21;

  // Stack Data to send
  bool PIR_state;
  if (digitalRead(MotionSensor) ?  PIR_state = "true": PIR_state = "false");
  Doc["Motion_Sensor_state"] = PIR_state;
  //Doc["MotionSensor_Last_Updated"] = millis(); //TODO: Implement proper time stamping for PIR sensor activation
  bool Pump_state;
  if (digitalRead(PumpRelay) ?  Pump_state = "true": Pump_state = "false");
  Doc["Pump_State"] = Pump_state;
  //Doc["PUR_PUMP_FLOWRATE"] = ;
  //Doc["PUR_WATER_LEVEL"] = ;

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
    if (digitalRead(speakers[i]) ?  speakers_state[i] = true: speakers_state[i] = false);
  }

  // Flow Rate


  //example of how to add json array to json object
  /* // Add arrays for Cell level Data.
  JsonArray Cell_Voltage = Doc.createNestedArray("HMS_Cell_Voltage"); // from 0 - 10 in increasing order
  float *cell_voltage = HMSmain.readSensAndCondition();
  // loop through and store per cell voltage
  for (int i = 0; i < numSensors; i++)
  {
    Cell_Voltage.add(cell_voltage[i]);
  } 

  free(cell_voltage); // free the memory*/

  /* serializeJson(Doc, Serial);
  String json = Doc.as<String>(); */
  //mqtt.MQTTPublish("/PUR/json_live", StringtoChar(json)); 
}

AccumulateData accumulateData;
