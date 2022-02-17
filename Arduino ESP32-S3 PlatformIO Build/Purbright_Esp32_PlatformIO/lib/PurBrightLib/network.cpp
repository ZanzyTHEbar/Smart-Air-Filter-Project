
#include "network.hpp"

IPAddress mqttServer;
AsyncWebServer server(80);
IPAddress localIP;

// Set your Gateway IP address
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 0, 0);

const char *PARAM_INPUT_1 = "ssid";
const char *PARAM_INPUT_2 = "password";
const char *mdnsPath = "/mdns.txt";
const char *dhcpcheckPath = "/dhcpcheck.txt";
const char *ssidPath = "/ssid.txt";
const char *passPath = "/pass.txt";
const char *ntptimePath = "/ntptime.txt";
const char *ntptimeoffsetPath = "/ntptimeoffset.txt";
String mdnsdotlocalurl = "Purbright";

// char* create_mDNS_hostname = StringtoChar(MQTTCreateHostName(MQTT_HOSTNAME, ".local"));

String SSID;
String PASS;
String ntptime;
String ntptimeoffset;
String mdns;
String dhcpcheck;

String ledState;

// Timer variables
unsigned long previousMillis = 0;
const long interval = 10000; // interval to wait for Wi-Fi connection (milliseconds)

Network::Network()
{
}

Network::~Network()
{
}

// Read File from SPIFFS
String Network::readFile(fs::FS &fs, const char *path)
{
  Serial.printf("Reading file: %s\r\n", path);

  File file = fs.open(path);
  if (!file || file.isDirectory())
  {
    Serial.println("- failed to open file for reading");
    return String();
  }

  String fileContent;
  while (file.available())
  {
    fileContent = file.readStringUntil('\n');
    break;
  }
  return fileContent;
}

// Write file to SPIFFS
void Network::writeFile(fs::FS &fs, const char *path, const char *message)
{
  Serial.printf("Writing file: %s\r\n", path);
  delay(10);

  File file = fs.open(path, FILE_WRITE);
  if (!file)
  {
    Serial.println("- failed to open file for writing");
    return;
  }
  if (file.print(message))
  {
    Serial.println("- file written");
  }
  else
  {
    Serial.println("- file write failed");
  }
}

// Replaces placeholder with LED state value
String processor(const String &var)
{
  if (var == "STATE")
  {
    if (digitalRead(LED_BUILTIN == HIGH))
    {
      ledState = "ON";
    }
    else
    {
      ledState = "OFF";
    }
    return ledState;
  }
  return String();
}

bool Network::SetupNetworkStack()
{
  if (!cfg.loadConfig())
  {
    Serial.println("Failed to load config");
  }
  else
  {
    Serial.println("Config loaded");
  }

  // Load values saved in SPIFFS

  SSID = readFile(SPIFFS, ssidPath);
  Serial.println(SSID);
  PASS = readFile(SPIFFS, passPath);
  Serial.println(PASS);

  // Save loaded values to config struct

  heapStr(&cfg.config.WIFISSID, StringtoChar(SSID));
  heapStr(&cfg.config.WIFIPASS, StringtoChar(PASS));

  if (cfg.config.WIFISSID[0] == '\0' || cfg.config.WIFIPASS[0] == '\0')
  {
    Serial.println("No SSID or password has been set.");
    Serial.println("Please configure the Wifi Manager by scanning the QR code on your device.");
    Serial.println("");
    return false;
  }
  else
  {
    Serial.println("Configured SSID: ");
    Serial.println(cfg.config.WIFISSID);
    Serial.println("");
    WiFi.mode(WIFI_STA);
    if (dhcpcheck == "on")
    {
      Serial.println("DHCP Check is on");
      localIP.fromString(cfg.config.clientIP);

      if (!WiFi.config(localIP, gateway, subnet))
      {
        Serial.println("STA Failed to configure");
        return false;
      }

      WiFi.begin(cfg.config.WIFISSID, cfg.config.WIFIPASS);

      unsigned long currentMillis = millis();
      previousMillis = currentMillis;

      while (WiFi.status() != WL_CONNECTED)
      {
        currentMillis = millis();
        if (currentMillis - previousMillis >= interval)
        {
          Serial.println("WiFi connection timed out.");
          return false;
        }
      }

      Serial.println("Connected to WiFi.");
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());
      return true;
    }
  }
  return false;
}

void Network::SetupWebServer()
{
  if (SetupNetworkStack())
  {
    // Route for root / web page
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/index.html", "text/html", false, processor); });
    server.serveStatic("/", SPIFFS, "/");

    // Route to set GPIO state to HIGH
    server.on("/on", HTTP_GET, [](AsyncWebServerRequest *request)
              {
      digitalWrite(LED_BUILTIN, HIGH);
      request->send(SPIFFS, "/index.html", "text/html", false, processor); });

    // Route to set GPIO state to LOW
    server.on("/off", HTTP_GET, [](AsyncWebServerRequest *request)
              {
      digitalWrite(LED_BUILTIN, LOW);
      request->send(SPIFFS, "/index.html", "text/html", false, processor); });
    server.begin();
  }
  else
  {

    // TODO: Route for root to  "Please Scan QR code" - Route for Wifi Manager /purbrightwifi page 
    // TODO: There should be a reset mode that will reset the device to factory settings and restart the device.
    // TODO: Should be a physical reset button on the PCB itself - not a touch button - hold for 5 seconds to reset. Flash LED to indicate reset per second.
    // Connect to Wi-Fi network with SSID and password
    Serial.println("Setting Access Point...");
    // NULL sets an open Access Point
    WiFi.softAP("PURBRIGHT-WIFI", "purpur"); //TODO: Change Password to Mac Address of device - This is the password that will be used to connect to the device.

    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(IP);

    // Web Server Root URL
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/wifimanager.html", "text/html"); });

    server.serveStatic("/", SPIFFS, "/");

    server.on("/", HTTP_POST, [&](AsyncWebServerRequest *request)
              {
      int params = request->params();
      for(int i=0;i<params;i++){
        AsyncWebParameter* p = request->getParam(i);
        if(p->isPost()){
          // HTTP POST ssid value
          if (p->name() == PARAM_INPUT_1) {
            String ssID; 
            ssID = p->value().c_str();
            Serial.print("SSID set to: ");
            Serial.println(ssID);
            // Write file to save value
            writeFile(SPIFFS, ssidPath, ssID.c_str());
          }
          // HTTP POST pass value
          if (p->name() == PARAM_INPUT_2) {
            String passWord; 
            passWord = p->value().c_str();
            Serial.print("Password set to: ");
            Serial.println(passWord);
            // Write file to save value
            writeFile(SPIFFS, passPath, passWord.c_str());
          }
          Serial.printf("POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
        }
      }
      request->send(200, "text/plain", "Done. ESP will restart, connect to your router and go to IP address: " + String(cfg.config.clientIP));
      delay(3000);
      ESP.restart(); });
    server.begin();
  }
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
  if (WiFi.status() != WL_CONNECTED)
  {
    wifiConnected = false;
    SERIAL_DEBUG_LN(F("Wifi is not connected"));
  }
  else
  {
    wifiConnected = true;
    SERIAL_DEBUG_LN(F("Wifi is connected"));
    Serial.print("[INFO]: WiFi Connected! Open http://");
    Serial.print(WiFi.localIP());
    SERIAL_DEBUG_LN(" in your browser");
  }
}

int Network::DiscovermDNSBroker()
{
  // check if there is a WiFi connection
  if (WiFi.status() == WL_CONNECTED)
  {
    SERIAL_DEBUG_LN("\nconnected!");

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
        heapStr(&(cfg.config.MQTTBroker), mqttServer.toString().c_str());
        int number = mqttPort;
        char charValue = number + '0';
        Serial.printf("The port is:%c", charValue);
        heapStr(&(cfg.config.MQTTPort), &charValue);
        Serial.print("MQTT broker found at: ");
        Serial.print(mqttServer);
        Serial.print(cfg.config.MQTTBroker);
        Serial.print(":");
        Serial.println(mqttPort);
        return 1;
      }
    }
    return 1;
  }
  return 0;
}

// ######################## Call this OR SetupWebServer - not both #########################
void Network::SetupmDNSServer()
{
  // ######################## Multicast DNS #########################
  Serial.print("Setting up mDNS: ");
  // Set up mDNS responder:
  // - first argument is the domain name, in this example
  //   the fully-qualified domain name is "esp32.local"
  // - second argument is the IP address to advertise
  //   we send our IP address on the WiFi network
  if (!MDNS.begin(mdnsdotlocalurl.c_str()))
  {
    Serial.println("Error setting up MDNS responder!");
    while (1)
    {
      delay(1000);
    }
  }
  Serial.println("mDNS responder started");

  // Start TCP (HTTP) server
  SetupWebServer();
  Serial.println("TCP server started");

  // Add service to MDNS-SD
  MDNS.addService("http", "tcp", 80);
}

bool Network::SetupmDNSLoop()
{
  // ######################## Multicast DNS #########################
  // Check if a new espClient has connected
  if (WiFi.status() == WL_CONNECTED)
  {
    if (!espClient)
    {
      return false;
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
      return false;
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
  return false;
}

// BACKUP LEGACY FUNCTION
/******************************************************************************
 * Function: Connect to Wifi with fail to AP mode if no connection
 * Description: This connection will attempt to create a WiFi connection with the given SSID and password. If the connection fails, it will attempt to connect to a default Wifi AP.
 * Parameters: None
 * Return: None
 ******************************************************************************/
bool Network::connectToApWithFailToStation()
{
  WiFi.persistent(true);
  SERIAL_DEBUG_LN("Configuring access point...");
  SERIAL_DEBUG_ADD("SSID:");
  SERIAL_DEBUG_LN(cfg.config.WIFISSID);
  SERIAL_DEBUG_ADD("PASS:");
  SERIAL_DEBUG_LN(cfg.config.WIFIPASS);

  WiFi.mode(WIFI_STA);
  if (cfg.config.WIFISSID == "")
  {
    WiFi.reconnect();
  }
  else
  {
    WiFi.begin(cfg.config.WIFISSID, cfg.config.WIFIPASS);
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
      SERIAL_DEBUG_ADD("Wifi Connect Failed. \r\nStarting AP. \r\nAP IP address: ");
      WiFi.softAP("Purbright", "purpur");
      SERIAL_DEBUG_LN(WiFi.softAPIP());
      return false;
      break;
    }
  }
  SERIAL_DEBUG_LN("");
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());
  return true;
}

// ############## functions to update current server settings ###################
void Network::loadConfig()
{
  SERIAL_DEBUG_LN(F("Checking if hostname is set and valid"));
  size_t size = sizeof(cfg.config.hostname);
  if (!cfg.isValidHostname(cfg.config.hostname, size - 1))
  {
    strncpy(cfg.config.hostname, DEFAULT_HOSTNAME, size - 1);
    cfg.setConfigChanged();
  }

  if (cfg.config.MQTTEnabled)
  {
    SERIAL_DEBUG_LN(F("MQTT Enabled"));
    SERIAL_DEBUG_LN(F("MQTT Broker: "));
    SERIAL_DEBUG_LN(cfg.config.MQTTBroker);
    SERIAL_DEBUG_LN(F("MQTT Port: "));
    SERIAL_DEBUG_LN(cfg.config.MQTTPort);
    SERIAL_DEBUG_LN(F("MQTT User: "));
    SERIAL_DEBUG_LN(cfg.config.MQTTUser);
    SERIAL_DEBUG_LN(F("MQTT Pass: "));
    SERIAL_DEBUG_LN(cfg.config.MQTTPass);
    SERIAL_DEBUG_LN(F("MQTT Topic: "));
    SERIAL_DEBUG_LN(cfg.config.MQTTTopic);
    SERIAL_DEBUG_LN(F("MQTT Set Topic: "));
    SERIAL_DEBUG_LN(cfg.config.MQTTSetTopic);
    SERIAL_DEBUG_LN(F("MQTT Device Name: "));
    SERIAL_DEBUG_LN(cfg.config.MQTTDeviceName);
  }
  else
  {
    SERIAL_DEBUG_LN(F("MQTT Disabled"));
  }
  SERIAL_DEBUG_LNF("Loaded config: hostname %s", cfg.config.hostname);
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
  SERIAL_DEBUG_EOL;
  SERIAL_DEBUG_LN(F("System Information:"));
  SERIAL_DEBUG_LNF("Version: %s (%s)", VERSION, VERSION_DATE);
  SERIAL_DEBUG_LNF("Heap: %d", system_get_free_heap_size());
  SERIAL_DEBUG_LNF("SDK: %s", system_get_sdk_version());
  SERIAL_DEBUG_LNF("MAC address: %s", WiFi.macAddress().c_str());
  SERIAL_DEBUG_LNF("CPU Speed: %d MHz", ESP.getCpuFreqMHz());
  SERIAL_DEBUG_LNF("Flash Size: %dKB", ESP.getFlashChipSize());
  SERIAL_DEBUG_LN("System Information Sent");
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

Network network;
