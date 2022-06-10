#ifndef WEBSERVERHANDLER_HPP
#define WEBSERVERHANDLER_HPP
#include <AsyncTCP.h>
#include "ArduinoJson.h"
#include "storage.hpp"
#include <ESPAsyncWebServer.h>

namespace AirFilter
{
    class HTTPDHandler
    {
    private:
        void config_update_handler(AsyncWebServerRequest *request);
        void stream_handler(AsyncWebServerRequest *request);
        Configuration *filterConfig;
        AsyncWebServer *server;

    public:
        HTTPDHandler(Configuration *TrackerConfig);
        void startStreamServer();
    };
}
#endif // WEBSERVERHANDLER_HPP