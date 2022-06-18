#ifndef __OCTOPRINT_HPP__
#define __OCTOPRINT_HPP__

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

namespace pliskin
{

typedef struct octotemp
{
    float setpoint = 0;
    float measured = 0;
} octotemp;

typedef struct octostatus
{
    String state = "<none>";
    String job = "<none>";
    float progress = 0; // 0.0 .. 100.0f
    int elapsedSeconds = 0;
    int estimatedSecondsLeft = 0;
    octotemp toolTemp;
    octotemp bedTemp;
} octostatus;

class octoprint 
{
    private:
        StaticJsonDocument<1024> _doc;      // should be enough, since the filter ignores most values anyway
        WiFiClient _client;
        String _url, _apiKey;

    public:
        octoprint(const String &apiKey, const String &octoAddress = "http://octopi.local") : _url(octoAddress), _apiKey(apiKey) {};
        octostatus getStatus (void);
};

};

#endif