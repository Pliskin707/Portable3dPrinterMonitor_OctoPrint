#include "oled.hpp"

namespace pliskin
{

void statusDisplay::setup (void)
{
    Wire.begin();
    begin(SSD1306_SWITCHCAPVCC, 0x3C);
    setTextColor(SSD1306_WHITE, SSD1306_BLACK);
    setTextWrap(false);
    clearDisplay();
    setCursor(0,0);
    print("Hello World");
    display();
}

void statusDisplay::showStartupScreen (void)
{
    this->clearDisplay();
    this->setCursor(0,0);
    this->print("Status Monitor started");
    this->display();
}

};