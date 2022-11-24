#ifndef __OLED_HPP__
#define __OLED_HPP__

#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include "octoprint/octoprint.hpp"

namespace pliskin
{

typedef enum statusStyle
{
    progressBar = 0,
    largePercent
} statusStyle;

typedef enum temperatureProbe
{
    bed = 0,
    tool
} temperatureProbe;

class statusDisplay: public Adafruit_SSD1306
{
    using Adafruit_SSD1306::Adafruit_SSD1306;   // "forward" all constructors

    private:
        statusStyle _style = largePercent;
        bool _toggle = false;
        float _startTempBed = 0;
        float _startTempTool = 0;
        void printDone (const octostatus &status);
        void printError (const octostatus &status);
        void printBarAt (const int y, const float percent, const char * const text = nullptr, const int height = 16);
        void styleProgressBar (const octostatus &status);
        void styleLargePercent (const octostatus &status);
        void printTempBarAt(const uint16_t y, const temperatureProbe probe, const octostatus &status);

    public:
        void setup (void);
        void showStartupScreen (void);
        void setStyle (const statusStyle style);
        void updateContent (const octostatus &status);
        int16_t centerText (const char * const text);   // returns the start X-position of the text
        void showWarning (const char * const text = nullptr);
};

};

#endif