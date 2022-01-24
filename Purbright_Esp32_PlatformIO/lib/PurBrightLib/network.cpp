#include "Network.hpp"

Network::Network()
{
}

Network::~Network()
{
}

// ######################## server functions #########################
int Network::CheckWifiState()
{
  // check if there is a WiFi connection
  int wifiStateCounter = 0;
  SERIAL_DEBUG_ADD("checking wifi...");
  while (WiFi.status() != WL_CONNECTED)
  {
    SERIAL_DEBUG_ADD(". ");
    delay(1000);
    wifiStateCounter++;
    if (wifiStateCounter > 100)
      SERIAL_DEBUG_LN("WiFi not connected");
    return 0;
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    SERIAL_DEBUG_LN("\nconnected!");
    return 1;
  }
  return 0;
}

/******************************************************************************
 * Function: Check Network Connection Loop
 * Description: This function checks the network connection and reconnects if necessary - is called in the loop() function every 5 seconds
 * Parameters: None
 * Return: None
 ******************************************************************************/
void Network::CheckNetworkLoop()
{
  // run current function every 5 seconds
  
  int currentWifiStatus = CheckWifiState();

  if (currentWifiStatus != 1)
  {
    wifiConnected = false;
    SERIAL_DEBUG_LN(F("Wifi is not connected"));
  }
  else
  {
    SERIAL_DEBUG_LN(F("Wifi is connected and not connected to a network"));
  }
  if (currentWifiStatus == WL_CONNECTED && !wifiConnected)
  {
    wifiConnected = true;
    Serial.print("INFO: WiFi Connected! Open http://");
    Serial.print(WiFi.localIP());
    SERIAL_DEBUG_LN(" in your browser");
  }
}

int Network::DiscovermDNSBroker()
{
  // ######################## Multicast DNS #########################
  Serial.print("Setting up mDNS: ");
  if (!MDNS.begin(mqtt_mDNS_clientId))
  {
    Serial.println("[Fail]");
  }
  else
  {
    Serial.println("[OK]");
    Serial.print("Querying MQTT broker: ");

    int n = MDNS.queryService("mqtt", "tcp");

    if (n == 0)
    {
      // No service found
      Serial.println("[Fail]");
      return 0;
    }
    else
    {
      // Found one or more MQTT service - use the first one.
      Serial.println("[OK]");
      mqttServer = MDNS.IP(0);
      mqttPort = MDNS.port(0);
      cfg.MQTTBroker = mqttServer;
      cfg.MQTTPort = mqttPort;
      Serial.print("MQTT broker found at: ");
      Serial.print(mqttServer);
      Serial.print(":");
      Serial.println(mqttPort);
      return 1;
    }
  }
  return 0;
}

void Network::SetupmDNSServer()
{
  // ######################## Multicast DNS #########################
  Serial.print("Setting up mDNS: ");
  // Set up mDNS responder:
  // - first argument is the domain name, in this example
  //   the fully-qualified domain name is "esp32.local"
  // - second argument is the IP address to advertise
  //   we send our IP address on the WiFi network
  if (!MDNS.begin(mDNS_hostname))
  {
    Serial.println("Error setting up MDNS responder!");
    while (1)
    {
      delay(1000);
    }
  }
  Serial.println("mDNS responder started");

  // Start TCP (HTTP) server
  webServer.begin();
  Serial.println("TCP server started");

  // Add service to MDNS-SD
  MDNS.addService("http", "tcp", 80);
}

void Network::SetupmDNSLoop()
{
  // ######################## Multicast DNS #########################
  // Check if a new espClient has connected
  int wifi_status = CheckWifiState();
  if (wifi_status == 1)
  {
    if (!espClient)
    {
      return;
    }
    // Wait for data from espClient to become available
    while (!espClient.available())
    {
      delay(1);
    }

    // Read the first line of HTTP request
    String req = espClient.readStringUntil('\r');

    // First line of HTTP request looks like "GET /path HTTP/1.1"
    // Retrieve the "/path" part by finding the spaces
    int addr_start = req.indexOf(' ');
    int addr_end = req.indexOf(' ', addr_start + 1);
    if (addr_start == -1 || addr_end == -1)
    {
      Serial.print("Invalid request: ");
      Serial.println(req);
      return;
    }
    req = req.substring(addr_start + 1, addr_end);
    Serial.print("Request: ");
    Serial.println(req);

    String s;
    if (req == "/")
    {
      IPAddress ip = WiFi.localIP();
      String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
      s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>Hello from ESP32 at ";
      s += ipStr;
      s += "</html>\r\n\r\n";
      Serial.println("Sending 200");
    }
    else
    {
      s = "HTTP/1.1 404 Not Found\r\n\r\n";
      Serial.println("Sending 404");
    }
    espClient.print(s);

    espClient.stop();
    Serial.println("Done with espClient");
  }
  else
  {
    return;
  }
}

// BACKUP LEGACY FUNCTION
/******************************************************************************
 * Function: Connect to Wifi with fail to AP mode if no connection
 * Description: This connection will attempt to create a WiFi connection with the given SSID and password. If the connection fails, it will attempt to connect to a default Wifi AP.
 * Parameters: None
 * Return: None
 ******************************************************************************/
void Network::connectToApWithFailToStation()
{
  WiFi.persistent(true);
  SERIAL_DEBUG_LN("Configuring access point...");
  SERIAL_DEBUG_ADD("cfg.WIFISSID:");
  SERIAL_DEBUG_LN(cfg.WIFISSID);
  SERIAL_DEBUG_ADD("cfg.WIFIPASS:");
  SERIAL_DEBUG_LN(cfg.WIFIPASS);

  WiFi.mode(WIFI_STA);
  if (cfg.WIFISSID == " ")
  {
    WiFi.reconnect();
  }
  else
  {
    WiFi.begin(cfg.WIFISSID, cfg.WIFIPASS);
  }

  int numberOfAttempts = 0;

  while (WiFi.status() != WL_CONNECTED)
  {
    numberOfAttempts++;
    delay(200);
    SERIAL_DEBUG_ADD(".");
    if (numberOfAttempts > 100)
    {
      WiFi.mode(WIFI_AP);
      // You can remove the password parameter if you want the AP to be open.
      WiFi.softAP(cfg.WIFISSID, cfg.WIFIPASS);
      SERIAL_DEBUG_ADD("Wifi Connect Failed. \r\nStarting AP. \r\nAP IP address: ");
      SERIAL_DEBUG_LN(WiFi.softAPIP());
      return;
      break;
    }
  }
  SERIAL_DEBUG_LN("");
  SERIAL_DEBUG_LN("Connected! IP address: ");
  SERIAL_DEBUG_LN(WiFi.localIP());
}

// ############## functions to update current server settings ###################
void Network::loadConfig()
{
  SERIAL_DEBUG_LN(F("Loading config"));
  // Loads configuration from EEPROM into RAM
  EEPROM.begin(4095);
  EEPROM.get(0, cfg);
  EEPROM.end();

  if (!isValidHostname(cfg.hostname, sizeof(cfg.hostname)))
  {
    strncpy(cfg.hostname, DEFAULT_HOSTNAME, sizeof(cfg.hostname));
    setConfigChanged();
  }

#ifdef ENABLE_MQTT_SUPPORT
  // fall back to default settings if hostname is invalid
  char *mqtt_host = (char *)cfg.MQTTBroker;
  if (!isValidHostname(mqtt_host, sizeof(cfg.MQTTBroker)))
  {
    cfg.MQTTEnabled = MQTT_ENABLED;
    // Convert String data of global definitions into Char pointers for use in strncpy
    char *MQTT_BROKER = StringtoChar(MQTT_HOSTNAME);
    char *mqtt_user = StringtoChar(MQTT_USER);
    char *mqtt_pass = StringtoChar(MQTT_PASS);
    char *mqtt_topic = StringtoChar(MQTT_TOPIC);
    char *mqtt_topic_set = StringtoChar(MQTT_HOMEASSISTANT_TOPIC_SET);
    char *mqtt_device_name = StringtoChar(MQTT_DEVICE_NAME);

    if (MQTT_SECURE_ENABLED != 0)
    {
      cfg.MQTTPort = uint16_t(MQTT_PORT_SECURE);
    }
    else
    {
      cfg.MQTTPort = uint16_t(MQTT_PORT);
    }

    strncpy(mqtt_host, MQTT_BROKER, sizeof(cfg.MQTTBroker));
    strncpy(cfg.MQTTUser, mqtt_user, sizeof(cfg.MQTTUser));
    strncpy(cfg.MQTTPass, mqtt_pass, sizeof(cfg.MQTTPass));
    strncpy(cfg.MQTTTopic, mqtt_topic, sizeof(cfg.MQTTTopic));
    strncpy(cfg.MQTTSetTopic, mqtt_topic_set, sizeof(cfg.MQTTSetTopic));
    strncpy(cfg.MQTTDeviceName, mqtt_device_name, sizeof(cfg.MQTTDeviceName));
    setConfigChanged();
  }
#endif

  SERIAL_DEBUG_LNF("Loaded config: hostname %s, MQTT enabled %s, MQTT host %s, MQTT port %d, MQTT user %s, MQTT pass %s, MQTT topic %s, MQTT set topic %s, MQTT device name %s",
                   cfg.hostname,
                   (cfg.MQTTEnabled == MQTT_ENABLED) ? "true" : "false",
                   cfg.MQTTBroker,
                   cfg.MQTTPort,
                   cfg.MQTTUser,
                   cfg.MQTTPass,
                   cfg.MQTTTopic,
                   cfg.MQTTSetTopic,
                   cfg.MQTTDeviceName)
}

void Network::updateCurrentData()
{
    // call to save config if config has changed
    saveConfig();
    SERIAL_DEBUG_LNF("Heap: %d", system_get_free_heap_size());
    SERIAL_DEBUG_LN("Updating current data");
    // update current data
}

// These are old functions from previous websocket implementation
// but we keep then as this could be still used in the future
/* void broadcastInt(String name, uint8_t value)
{
// String json = "{\"name\":\"" + name + "\",\"value\":" + String(value) + "}";
#ifdef ENABLE_MQTT_SUPPORT
  MqttData.mqttSendStatus();
#endif
}

void broadcastString(String name, String value)
{
// String json = "{\"name\":\"" + name + "\",\"value\":\"" + String(value) + "\"}";
#ifdef ENABLE_MQTT_SUPPORT
  MqttData.mqttSendStatus();
#endif
}
 */

void Network::SetupServer()
{
  Serial.begin(BAUD_RATE);
#if PURR_DEBUG != 0
  while (!Serial)
    delay(10); // will pause until serial console opens
#endif
  Serial.println("Booting up");
  Serial.println("Version: " + String(VERSION) + " " + String(VERSION_DATE));
  Serial.println("");
  SERIAL_DEBUG_LN(F("Starting Webserver"))

  // starting file system
  if (!SPIFFS.begin())
  {
    SERIAL_DEBUG_LN(F("An Error has occurred while mounting SPIFFS"));
    return;
  }
  loadConfig();

  SERIAL_DEBUG_EOL;
  SERIAL_DEBUG_LN(F("System Information:"));
  SERIAL_DEBUG_LNF("Version: %s (%s)", VERSION, VERSION_DATE);
  SERIAL_DEBUG_LNF("Heap: %d", system_get_free_heap_size());
  SERIAL_DEBUG_LNF("SDK: %s", system_get_sdk_version());
  SERIAL_DEBUG_LNF("MAC address: %s", WiFi.macAddress().c_str());
  SERIAL_DEBUG_LNF("CPU Speed: %d MHz", ESP.getCpuFreqMHz());
  SERIAL_DEBUG_LNF("Flash Size: %dKB", ESP.getFlashChipSize());
  SERIAL_DEBUG_EOL("System Information Sent");
  SERIAL_DEBUG_EOL;

// FS debug information
// THIS NEEDS TO BE PAST THE WIFI SETUP!! OTHERWISE WIFI SETUP WILL BE DELAYED
#if PURR_DEBUG != 0
  SERIAL_DEBUG_LN(F("SPIFFS contents:"));
#ifdef ESP32
  File root = SPIFFS.open("/");
  File file = root.openNextFile();
  while (file)
  {
    SERIAL_DEBUG_LNF("FS File: %s, size: %lu", file.name(), file.size());
    file = root.openNextFile();
  }
  SERIAL_DEBUG_EOL;
  unsigned int totalBytes = SPIFFS.totalBytes();
  unsigned int usedBytes = SPIFFS.usedBytes();
#endif
  if (usedBytes == 0)
  {
    SERIAL_DEBUG_LN(F("NO WEB SERVER FILES PRESENT! SEE: \n"));
  }
  SERIAL_DEBUG_LNF("FS Size: %luKB, used: %luKB, %0.2f%%",
                   totalBytes, usedBytes,
                   (float)100 / totalBytes * usedBytes);
  SERIAL_DEBUG_EOL;
#endif
}