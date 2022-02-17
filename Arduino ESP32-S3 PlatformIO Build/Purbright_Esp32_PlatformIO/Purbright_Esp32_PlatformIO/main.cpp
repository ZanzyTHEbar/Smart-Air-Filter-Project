#include <defines.hpp>

// define Task variables
/* TaskHandle_t runserver;
TaskHandle_t accumulatedata; */

/******************************************************************************
 * Function: Task to Run Network Stack
 * Description: This function assigns the network stack loop to a task in freeRTOS.
 * Parameters: void *parameters
 * Return: None
 ******************************************************************************/
/* void TasktoRunNetworkStack(void *parameter)
{
  Serial.print("Networking Tasks running on core ");
  Serial.println(xPortGetCoreID());
  for (;;)
  {
    // run the network stack
    timedTasks.Run_NetworkCheck_Background_every_10_Seconds();
    //timedTasks.Run_Check_Pump_every_10_Seconds();
    timedTasks.Run_Check_PIR_every_10_Seconds();
    timedTasks.Run_Check_Neopixel_every_10_Seconds();
    timedTasks.Run_UpdateConfigData_Background_every_5_Seconds();
    timedTasks.Run_Pump_Turn_Off_Check_5s();
    timedTasks.checkTurnOffSettings();
    timedTasks.CheckTurnOffSettings_10();

    if (millis() - lastMillis >= 5000)
    {
      network.updateCurrentData(); // Update the config file with the current data
      network.CheckNetworkLoop();  // Check the network state and connect to the AP if needed
      lastMillis = millis();       // get ready for the next iteration
    }
    // network.SetupmDNSLoop(); // Setup mDNS loop
    purrMqtt.RunMqttService(); // Run the MQTT service loop
  }
}
 */
/******************************************************************************
 * Function: Task to accumulate sensor data run application logic
 * Description: This function assigns the application loop to a task in freeRTOS.
 * Parameters: void *pvPparameters
 * Return: None
 ******************************************************************************/
/* void TasktoAccumulateSensorData(void *pvParameters)
{
  Serial.print("Application Tasks running on core ");
  Serial.println(xPortGetCoreID());

  for (;;)
  {
    timedTasks.Run_Check_DataJSON_5();
    if (millis() - lastMillis >= 5000)
    {
      lastMillis = millis(); // get ready for the next iteration
      pump.checkTurnOffPump();
      timedTasks.checkTurnOffSettings();
      accumulateData.InitAccumulateDataJson();
    }
    buttons.ButtonLoop(); // Setup the main loop for the buttons

    // Automatic Functionality
    if ((S_OnOff == true) && (S_Menu == false) && (S_ManAut == true))
    {
      pump.Automatic();
    }

    // Manual Functionality
    if ((S_OnOff == true) && (S_Menu == false) && (S_ManAut == false))
    {
      pump.Manual();
    }
  }
}
  */

void setup()
{
  Serial.begin(BAUD_RATE);
  while (!Serial)
    delay(10); // will pause until serial console opens
/* #if PURR_DEBUG != 0
 
#endif */
  // Set GPIO 2 as an OUTPUT
  cfg.CreateDefaultConfig();
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  //timedTasks.hookup();
  timedTasks.SetupTimers();
  Serial.println("Booting up");
  Serial.println("Version: " + String(VERSION) + " " + String(VERSION_DATE));
  Serial.println("");
  Serial.println("Starting...");

  // Setup the network stack
  // Setup the Wifi Manager
  Serial.println("Reading Config File");
  Serial.println("Config File Read");
  Serial.println("Setting up WiFi");
  SERIAL_DEBUG_LN(F("Starting Webserver"))
  network.SetupmDNSServer(); // setup the mDNS server for the future web-front-end
  //network.SetupWebServer();// Setup the server
  network.loadConfig();
  network.DiscovermDNSBroker(); // discover the mDNS broker for mqtt

  network.SetupServer();
  pir.SetupPIR();
  neopixel.SetupNeopixel();
  buttons.SetupButtons();
  
  neopixel.SetupNeopixel();
  Serial.println('\n');
  pump.SetupPump();
  purrMqtt.MQTTSetup();

  //time_now = millis();

  // Cores where the task should run
  /* xTaskCreatePinnedToCore(
      TasktoRunNetworkStack, // Function to implement the task
      "Task1",               // Name of the task
      10000,                 // Stack size in words
      NULL,                  // Task input parameter
      2,                     // Priority of the task
      &runserver,            // Task handle.
      0); */

  
  /* 
    // create a task that will be executed in the Task2code() function, with priority 1 and executed on core 1
    xTaskCreatePinnedToCore(
        TasktoAccumulateSensorData, // Task function. 
        "Task2",                    // name of task. 
        10000,                      // Stack size of task
        NULL,                       // parameter of the task
        1,                          // priority of the task
        &accumulatedata,            // Task handle to keep track of created task
        1);
                                 // pin task to core 1
                                  */

  SERIAL_DEBUG_LN(F("Setup Complete"));
}

void loop()
{
  // Automatic Functionality
  if ((S_OnOff == true) && (S_Menu == false) && (S_ManAut == true))
  {
    pump.Automatic();
  }

  // Manual Functionality
  if ((S_OnOff == true) && (S_Menu == false) && (S_ManAut == false))
  {
    pump.Manual();
  }

  pump.PumpLoop(); // Setup the main loop for the pump
  pump.checkTurnOffPump();
  pir.SetupMainLoop();  // Setup the main loop for the PIR sensor
  buttons.ButtonLoop(); // Setup the main loop for the buttons
  network.SetupmDNSLoop(); // Setup mDNS loop
  timedTasks.checkTurnOffSettings();
  timedTasks.Run_Check_DataJSON_5();
  delay(100);
  timedTasks.Run_NetworkCheck_Background_every_10_Seconds();
  //purrMqtt.RunMqttService(); // Run the MQTT service loop
}