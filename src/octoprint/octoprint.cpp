#include "octoprint.hpp"
#include "projutils/projutils.hpp"

namespace pliskin
{
octostatus octoprint::getStatus (void)
{
    octostatus receivedStatus;
    int result;
    HTTPClient http;

    // client settings
    http.setTimeout(3000);
    http.useHTTP10(true);   // this is required, so "deserializeJson()" can read the stream directly
    // http.setAuthorization(_apiKey.c_str());

    // get "state" data
    // http.begin(this->_client, this->_url + "/api/printer");
    http.begin(_client, _url + "/api/printer?exclude=sd,state");
    http.addHeader("X-Api-Key", _apiKey);
    result = http.GET();
    dprintf("GET state result: %d\n", result);
    if (result == 200)  // == OK
    {
        DeserializationError error = deserializeJson(_doc, http.getStream(), DeserializationOption::Filter(_filter));
        
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
                    receivedStatus.bedTemp = temperature;
                else if (strcmp_P(temperature_item_key, PSTR("tool0")) == 0)
                    receivedStatus.toolTemp = temperature;
            }
        }
    }

    http.begin(_client, _url + "/api/job");
    http.addHeader("X-Api-Key", _apiKey);
    result = http.GET();
    dprintf("GET state result: %d\n", result);
    if (result == 200)  // == OK
    {
        DeserializationError error = deserializeJson(_doc, http.getStream());
        
        if (error)
        {
            dprintf("Deserialization failed: %s\n", error.c_str());
        }
        else
        {
            JsonObject job = _doc["job"];

            JsonObject job_file = job["file"];
            const char* job_file_name = job_file["name"]; // "CFFFP_Example.gcode"

            receivedStatus.job = String(job_file_name);

            // ignore the file extension -> find the last dot
            const auto dotIndex = receivedStatus.job.lastIndexOf('.');

            // only if found
            if (dotIndex > 0)
                receivedStatus.job = receivedStatus.job.substring(0, dotIndex - 1);

            // const char* job_user = job["user"];

            JsonObject progress = _doc["progress"];
            receivedStatus.progress = progress["completion"];
            receivedStatus.elapsedSeconds = progress["printTime"];
            receivedStatus.estimatedSecondsLeft = progress["printTimeLeft"];

            const char* state = _doc["state"]; // "Printing"
            receivedStatus.state = String(state);
        }
    }

    http.end();

    return receivedStatus;
}
};