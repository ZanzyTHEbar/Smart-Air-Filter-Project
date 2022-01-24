#include <defines.hpp>

// define Task variables
TaskHandle_t runserver;
TaskHandle_t accumulatedata;

// Timer delay Settings
void checkTurnOffSettings()
{
  // check if delay has timed out
  if (DelayRunningSettings && ((millis() - DelayStartSettings) >= 4000))
  {
    DelayRunningSettings = false;
    S_Menu = false;
  }
}

/******************************************************************************
 * Function: Task to Run Network Stack
 * Description: This function assigns the network stack loop to a task in freeRTOS.
 * Parameters: void *parameters
 * Return: None
 ******************************************************************************/
void TasktoRunNetworkStack(void *parameter)
{
  Network *network = new Network;
  PurrMqtt *mqtt = new PurrMqtt;
  Serial.print("Networking Tasks running on core ");
  Serial.println(xPortGetCoreID());
  for (;;)
  {
    if (millis() - lastMillis >= 5000)
    {
      lastMillis = millis();        // get ready for the next iteration
      network->updateCurrentData(); // Update the config file with the current data
      network->CheckNetworkLoop();  // Check the network state and connect to the AP if needed
    }
    // network.SetupmDNSLoop(); // Setup mDNS loop
    mqtt->RunMqttService(); // Run the MQTT service loop
  }
  delete network;
  delete mqtt;
}

/******************************************************************************
 * Function: Task to accumulate sensor data run application logic
 * Description: This function assigns the application loop to a task in freeRTOS.
 * Parameters: void *pvPparameters
 * Return: None
 ******************************************************************************/
void TasktoAccumulateSensorData(void *pvParameters)
{
  PIR *pir = new PIR;
  AccumulateData *accumulatedData = new AccumulateData;
  Pump *pump = new Pump;
  Buttons *buttons = new Buttons;

  Serial.print("Application Tasks running on core ");
  Serial.println(xPortGetCoreID());

  for (;;)
  {
    if (millis() - lastMillis >= 5000)
    {
      lastMillis = millis(); // get ready for the next iteration
      pump->checkTurnOffPump();
      checkTurnOffSettings();
      accumulatedData->InitAccumulateDataJson();
    }
    pir->SetupMainLoop();  // Setup the main loop for the PIR sensor
    pump->SetupMainLoop(); // Setup the main loop for the pump
    buttons->ButtonLoop(); // Setup the main loop for the buttons

    // Automatic Functionality
    if ((S_OnOff == true) && (S_Menu == false) && (S_ManAut == true))
    {
      pump->Automatic();
    }

    // Manual Functionality
    if ((S_OnOff == true) && (S_Menu == false) && (S_ManAut == false))
    {
      pump->Manual();
    }
  }
  delete pir;
  delete pump;
  delete buttons;
  delete accumulatedData;
}

void setup()
{
  Neopixel *neopixel = new Neopixel;

  neopixel->SetupNeopixel();

  // Setup the Wifi Manager
  Network *network = new Network;

  network->SetupServer(); // Setup the serial communication and network stack

  // ######################## server setup #########################
  SERIAL_DEBUG_ADD("\nStarting " + String(MQTT_DEVICE_NAME) + " on " + String(ARDUINO_BOARD));
  SERIAL_DEBUG_LN(ESP_ASYNC_WIFIMANAGER_VERSION);
#if (USING_ESP32_S2 || USING_ESP32_C3)
  ESPAsync_WiFiManager *ESPAsync_wifiManager = new ESPAsync_WiFiManager(&webServer, NULL, "PURRBRIGHT_LITTER_BOX");
#else
  ESPAsync_WiFiManager *ESPAsync_wifimanager = new ESPAsync_WiFiManager(&webServer, &dnsServer, "PURRBRIGHT_LITTER_BOX");
#endif

  // ESPAsync_wifiManager.resetSettings();   //reset saved settings
  // ESPAsync_wifiManager.setAPStaticIPConfig(IPAddress(192, 168, 132, 1), IPAddress(192, 168, 132, 1), IPAddress(255, 255, 255, 0));
  ESPAsync_wifimanager->autoConnect(cfg.WIFISSID);
  if (WiFi.status() == WL_CONNECTED)
  {
    SERIAL_DEBUG_ADD(F("Connected. Local IP: "));
    SERIAL_DEBUG_LN(WiFi.localIP());
  }
  else
  {
    SERIAL_DEBUG_LN(ESPAsync_wifimanager->getStatus(WiFi.status()));
    network->CheckWifiState();
  }

  webServer.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
               { request->send(200, "text/plain", "Hi! I am MyESP32."); });

  AsyncElegantOtaClass *asyncElegantOTA = new AsyncElegantOtaClass;

  asyncElegantOTA->begin(&webServer); // Start ElegantOTA
  webServer.begin();
  SERIAL_DEBUG_LN("HTTP server started"); // initialise the network stack and generate web-portal to configure the network
  network->DiscovermDNSBroker();          // discover the mDNS broker for mqtt

  // network.SetupmDNSServer(); // setup the mDNS server for the future web-front-end

  time_now = millis();

  /* Cores where the task should run */
  xTaskCreatePinnedToCore(
      TasktoRunNetworkStack, /* Function to implement the task */
      "Task1",               /* Name of the task */
      10000,                 /* Stack size in words */
      NULL,                  /* Task input parameter */
      2,                     /* Priority of the task */
      &runserver,            /* Task handle. */
      0);

  if (millis() < time_now + period)
  {
    // create a task that will be executed in the Task2code() function, with priority 1 and executed on core 1
    xTaskCreatePinnedToCore(
        TasktoAccumulateSensorData, /* Task function. */
        "Task2",                    /* name of task. */
        10000,                      /* Stack size of task */
        NULL,                       /* parameter of the task */
        1,                          /* priority of the task */
        &accumulatedata,            /* Task handle to keep track of created task */
        1);                         /* pin task to core 1 */
  }
  SERIAL_DEBUG_LN(F("Setup Complete"));
  delete neopixel;
  delete network;
}

void ScanI2CBus()
{
  Scanner *scanner = new Scanner;
  scanner->SetupScan();
  scanner->BeginScan();
  delete scanner;
}

void loop()
{
}