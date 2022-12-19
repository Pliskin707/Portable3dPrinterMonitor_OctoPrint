#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include "ota/ota.hpp"
#include "projutils/projutils.hpp"
#include "config.hpp"
#include "oled/oled.hpp"
#include "octoprint/octoprint.hpp"

using namespace pliskin;

/** Wifi authentication **
 * 
 * this file needs to be created with the following content (and is obviously not included in version control):
 * 
 * #pragma once
 * 
 * const char* ssid = "<YourSSIDhere>";
 * const char* password = "<YourPasswordHere>";
 * const char* octoapi = "<YourOctoprintApiKey>";
 */
#include "../wifiauth2.h"

static bool mDNS_init_ok = false;
WiFiClient client;
statusDisplay display(128, 64, &Wire, -1);
octoprint octopi(octoapi);

void setup() {
  #ifndef DEBUG_PRINT
  pinMode(LEDPIN, OUTPUT);
  #else
  Serial.begin(115200);
  #endif

  // status display
  display.setup();
  display.setStyle(progressBar);

  // Wifi
  IPAddress local_IP(192, 168, 0, 50);
  IPAddress gateway(192, 168, 0, 1);
  IPAddress subnet(255, 255, 255, 0);
  IPAddress dns(1, 1, 1, 1);
  WiFi.hostname(DEVICENAME);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  wifi_set_sleep_type(NONE_SLEEP_T);

  wl_status_t wstat;
  while (true)
  {
    delay(500);
    wstat = WiFi.status();
    if (wstat == WL_CONNECTED)
      break;

    #ifndef DEBUG_PRINT
    digitalWrite(LEDPIN, !digitalRead(LEDPIN));
    #endif
    dprintf("Connecting (%d) ...\n", wstat);
  };

  // mDNS
  mDNS_init_ok = MDNS.begin(DEVICENAME);

  // OTA
  ota::begin(DEVICENAME);
}

void loop() {
  const uint32_t time = millis();
  static uint32_t next = 0;

  // Wifi status
  const bool connected = WiFi.isConnected();
  #ifndef DEBUG_PRINT
  digitalWrite(LEDPIN, !connected);
  #endif

  // mDNS
  if (mDNS_init_ok)
    MDNS.update();

  // OTA
  ota::handle();

  // program logic
  if (time >= next)
  {
    next = time + 5000;
    //dprintf("Systime: %lu ms; WLAN: %sconnected (as %s)\n", time, (connected ? "":"dis"), WiFi.localIP().toString().c_str());
    octostatus printerStatus;
    
    if (octopi.getStatus(printerStatus))
      display.updateContent(printerStatus);

    if (connected)
    {
        if (printerStatus.state.compareTo("Offline after error") == 0)
        {
          display.fillRect(0, 16, display.width(), display.height() - 16, SSD1306_BLACK);
          display.setCursor(0, 16);
          display.print("Trying to reconnect\nserver to printer...\n");
          display.printf_P(PSTR("Result: %d"), octopi.reconnectServerToPrinter());
          display.display();
        }
    }
    else
      WiFi.reconnect();
  }

  yield();
}