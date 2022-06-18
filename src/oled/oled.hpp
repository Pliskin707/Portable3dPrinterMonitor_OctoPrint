#ifndef __OLED_HPP__
#define __OLED_HPP__

#include <Adafruit_SSD1306.h>
#include <Wire.h>

namespace pliskin
{

class statusDisplay: public Adafruit_SSD1306
{
    using Adafruit_SSD1306::Adafruit_SSD1306;   // "forward" all constructors

    public:
        void setup (void);
        void showStartupScreen (void);
};

};

#endif