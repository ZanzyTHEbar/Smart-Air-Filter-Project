#include <defines.hpp>

void setup()
{
  Serial.begin(115200);

  // Set GPIO 2 as an OUTPUT
  Serial.println("This is a test print");
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  Serial.println(F("Booting up"));
  Serial.println("Version: " + String(VERSION) + " " + String(__DATE__));
  Serial.println("");
  Serial.println(F("Starting..."));

  // Setup the network stack
  // Setup the Wifi Manager
  Serial.println(F("Reading Config File"));
  Serial.println(F("Config File Read"));
  Serial.println(F("Setting up WiFi"));
  Serial.println(F("Starting Webserver"));
  timedTasks.SetupTimers();
  Relay.SetupRelays();
  // network.SetupmDNSServer(); // setup the mDNS server for the future web-front-end
  network.SetupServer();
  network.SetupWebServer(); // Setup the server
  // network.DiscovermDNSBroker(); // discover the mDNS broker for mqtt

  Serial.println('\n');
  neopixel.begin();
  networkntp.NTPSetup();
  pump.SetupPumpClass();
  buttons.SetupButtons();
  pir.SetupPIR();
  // purrMqtt.MQTTSetup();

  Serial.println(F("Setup Complete"));
}

void loop()
{
  /* delay(1000);
  digitalWrite(14, HIGH);
  delay(1000);
  digitalWrite(14, LOW); */

  pump.PumpLoop(); // Setup the main loop for the pump
  pir.run();       // Setup the main loop for the PIR sensor
  buttons.ButtonLoop(); // Setup the main loop for the buttons
  pump.checkTurnOffPump();
  // Automatic Functionality
  if (S_OnOff && !S_Menu && S_ManAut)
  {
    pump.Automatic();
  }

  // Manual Functionality
  if (S_OnOff && !S_Menu && !S_ManAut)
  {
    pump.Manual();
  }
  
  // network.SetupmDNSLoop(); // Setup mDNS loop
  timedTasks.checkTurnOffSettings();
  timedTasks.Run_Check_DataJSON_5();
  delay(100);
  timedTasks.checkNetwork();
  // purrMqtt.RunMqttService(); // Run the MQTT service loop
}