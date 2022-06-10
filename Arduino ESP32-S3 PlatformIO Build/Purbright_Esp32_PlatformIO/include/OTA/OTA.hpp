#ifndef OTA_HPP
#define OTA_HPP

#include "storage.hpp"
#include <ArduinoOTA.h>
#include <HTTPClient.h>

namespace AirFilter
{
    class OTA
    {
    private:
        unsigned long bootTimestamp = 0;
        bool isOtaEnabled = true;

    public:
        void SetupOTA(AirFilter::Configuration trackerConfig)
        {
            Serial.println("Setting up OTA updates");
            auto deviceConfig = trackerConfig.getDeviceConfig();

            if (strcmp(deviceConfig->OTAPassword, "") == 0)
            {
                Serial.println("THE PASSWORD IS REQUIRED, [[ABORTING]]");
                return;
            }
            ArduinoOTA.setPort(deviceConfig->OTAPort);

            ArduinoOTA
                .onStart([]()
                         {
                String type;
                if (ArduinoOTA.getCommand() == U_FLASH)
                    type = "sketch";
                else // U_SPIFFS
                    type = "filesystem"; })
                .onEnd([]()
                       { Serial.println("OTA updated finished successfully!"); })
                .onProgress([](unsigned int progress, unsigned int total)
                            { Serial.printf("Progress: %u%%\r", (progress / (total / 100))); })
                .onError([](ota_error_t error)
                         {
                Serial.printf("Error[%u]: ", error);
                if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
                else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
                else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
                else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
                else if (error == OTA_END_ERROR) Serial.println("End Failed"); });

            Serial.println("Starting up basic OTA server");
            Serial.println("OTA will be live for 30s, after which it will be disabled until restart");
            ArduinoOTA.begin();
            bootTimestamp = millis();
        }

        void HandleOTAUpdate()
        {
            if (isOtaEnabled)
            {
                if (bootTimestamp + 30000 < millis())
                {
                    // we're disabling ota after first 30sec so that nothing bad happens during playtime
                    isOtaEnabled = false;
                    Serial.println("From now on, OTA is disabled");
                    return;
                }
                ArduinoOTA.handle();
            }
        }
    };
}
#endif // OTA_HPP