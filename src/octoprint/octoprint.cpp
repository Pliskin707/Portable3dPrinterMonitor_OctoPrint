#include "octoprint.hpp"
#include "projutils/projutils.hpp"

namespace pliskin
{
bool octoprint::getStatus (octostatus &status)
{
    bool success = false;
    int result;
    HTTPClient http;
    DeserializationError error;

    // client settings
    http.setTimeout(3000);
    http.useHTTP10(true);   // this is required, so "deserializeJson()" can read the stream directly
    // http.setAuthorization(_apiKey.c_str());

    do
    {
        http.begin(_client, _url + "/api/job");
        http.addHeader("X-Api-Key", _apiKey);
        result = http.GET();
        dprintf("GET state result: %d\n", result);

        if (result != 200)  // == OK
            break;

        error = deserializeJson(_doc, http.getStream());
        
        if (error)
        {
            dprintf("Deserialization failed: %s\n", error.c_str());
        }
        else
        {
            JsonObject job = _doc["job"];

            JsonObject job_file = job["file"];
            const char* job_file_name = job_file["name"]; // "CFFFP_Example.gcode"

            status.job = String(job_file_name);

            // ignore the file extension -> find the last dot
            const auto dotIndex = status.job.lastIndexOf('.');

            // only if found
            if (dotIndex > 0)
                status.job = status.job.substring(0, dotIndex);

            // ignore the "CFFFP_" part prepended by cura
            if (status.job.startsWith(F("CFFFP_")))
                status.job = status.job.substring(6);

            // const char* job_user = job["user"];

            JsonObject progress = _doc["progress"];
            status.progress = progress["completion"];
            status.elapsedSeconds = progress["printTime"];
            status.estimatedSecondsLeft = progress["printTimeLeft"];

            const char* state = _doc["state"]; // "Printing"
            status.state = String(state);
        }

        // get "state" data
        // http.begin(this->_client, this->_url + "/api/printer");
        http.begin(_client, _url + "/api/printer?exclude=sd,state");    // http://octopi.local/api/printer?exclude=sd,state
        http.addHeader("X-Api-Key", _apiKey);
        result = http.GET();
        dprintf("GET state result: %d\n", result);

        if (result != 200)  // == OK
            break;
        
        error = deserializeJson(_doc, http.getStream());
        
        if (error)
        {
            dprintf("Deserialization failed: %s\n", error.c_str());
        }
        else
        {
            for (JsonPair temperature_item : _doc["temperature"].as<JsonObject>()) 
            {
                const char* temperature_item_key = temperature_item.key().c_str(); // "W", "bed", "tool0"
                octotemp temperature;

                temperature.measured = temperature_item.value()["actual"];
                temperature.setpoint = temperature_item.value()["target"];

                if (strcmp_P(temperature_item_key, PSTR("bed")) == 0)
                    status.bedTemp = temperature;
                else if (strcmp_P(temperature_item_key, PSTR("tool0")) == 0)
                    status.toolTemp = temperature;
            }
        }

        success = true;
    } while (false);

    http.end();
    return success;
}

int octoprint::reconnectServerToPrinter (void)
{
    int result;
    HTTPClient http;

    // client settings
    http.setTimeout(3000);
    http.useHTTP10(true);   // this is required, so "deserializeJson()" can read the stream directly

    http.begin(_client, _url + "/api/connection");    // http://octopi.local/api/connection
    http.addHeader("X-Api-Key", _apiKey);
    http.addHeader("Content-Type", "application/json");
    result = http.POST("{\"command\":\"connect\"}");
    dprintf("POST state result: %d\n", result);
    http.end();

    return result;
}

};