#include "oled.hpp"

namespace pliskin
{

enum printerState {Idle, Preheating, Printing, WarningOrError};

static printerState getPrinterState (const octostatus &status)
{
    if (status.state.compareTo("Operational") == 0)
        return Idle;

    if (status.state.compareTo("Printing") == 0)
    {
        if ((status.progress < 1) && 
            ((status.toolTemp.setpoint - status.toolTemp.measured) > 10))
        {
            return Preheating;
        }

        return Printing;
    }

    return WarningOrError;
}

void statusDisplay::setup (void)
{
    Wire.begin();
    begin(SSD1306_SWITCHCAPVCC, 0x3C);
    setTextColor(SSD1306_WHITE, SSD1306_BLACK);
    setTextWrap(false);
    clearDisplay();
    setCursor(0,0);
    centerText("3D Printer Monitor");
    display();
}

void statusDisplay::showStartupScreen (void)
{
    clearDisplay();
    setCursor(0,0);
    print("Status Monitor started");
    display();
}

void statusDisplay::setStyle (const statusStyle style)
{
    _style = style;
}

void statusDisplay::updateContent (const octostatus &status)
{
    clearDisplay();
    setCursor(0,0);

    switch (_style)
    {
        case progressBar:   styleProgressBar(status);   break;
        default:            styleLargePercent(status);  break;
    }

    display();
}

void statusDisplay::printDone (const octostatus &status)
{
    char tempStr[40];
    setTextSize(2);
    snprintf_P(tempStr, sizeof(tempStr), PSTR("Bed: %.1fC"), status.bedTemp.measured);
    centerText(tempStr);
    setTextSize(1);
    setCursor(0, 2 * 8);
    snprintf_P(tempStr, sizeof(tempStr), PSTR("Tool: %.1fC"), status.toolTemp.measured);
    centerText(tempStr);

    if (_toggle)
    {
        setTextColor(SSD1306_BLACK, SSD1306_WHITE);
        fillRect(0, 30, width(), height() - 30, SSD1306_WHITE);
    }

    setCursor(10, 4 * 8);
    setTextSize(4);
    print("DONE!");
    setTextSize(1);
    _toggle ^= true;

    setTextColor(SSD1306_WHITE, SSD1306_BLACK);
}

void statusDisplay::styleLargePercent (const octostatus &status)
{
    println(status.state);
    println(status.job);
    println("Progress:\n");
    setTextSize(4);
    printf_P(PSTR("%.1f%%"), status.progress);
    setTextSize(1);
}

void statusDisplay::styleProgressBar (const octostatus &status)
{
    const auto printer = getPrinterState(status);
    bool showTempBars = false;

    switch (printer)
    {
        case WarningOrError:
        {
            setTextSize(2); // big yellow text in line 0 and 1
            print(status.state);
            setTextSize(1);

            showTempBars = true;
        }
        break;

        case Printing:
        {
            // not yet done    
            printf_P(PSTR("\n\n%s (%d|%d)"), status.state.c_str(), (int) status.toolTemp.measured, (int) status.bedTemp.measured);  // blue starts at line 2
            setCursor(0, 28);
            println(status.job);

            
            setCursor(0, 36);   // between line 4 and 5
            char text[10];
            if (status.estimatedSecondsLeft > 0)
            {
                div_t qr = div(status.estimatedSecondsLeft, 60);
                const auto seconds = qr.rem;
                qr = div(qr.quot, 60);
                snprintf_P(text, sizeof(text), PSTR("%02d:%02d:%02d"), qr.quot, qr.rem, seconds);
            }
            else
                snprintf_P(text, sizeof(text), PSTR("%.2f %%"), status.progress);

            //printf_P(PSTR("Progress: %.1f%%"), status.progress);
            printBarAt(6 * 8, status.progress, text);
        }
        break;

        case Preheating:
        {
            setCursor(0, 2 * 8);
            centerText(status.state.c_str());
            showTempBars = true;
        }
        break;

        case Idle:
        {
            if (status.progress >= 100)
            {
                printDone(status);
                break;
            }

            setCursor(0, 2 * 8);
            centerText(status.state.c_str());
            showTempBars = true;
        }
        break;
    }    

    if (showTempBars)
    {
        printTempBarAt(28, bed, status);
        printTempBarAt(48, tool, status);
    }
}

void statusDisplay::printTempBarAt (const uint16_t y, const temperatureProbe probe, const octostatus &status)
{
    float progress = 0;
    char text[40];
    float &startTemp = ((probe == bed) ? _startTempBed : _startTempTool);
    const octotemp &temperature = ((probe == bed) ? status.bedTemp : status.toolTemp);
    int textPos = snprintf_P(text, sizeof(text), PSTR("%s: %.1f"), (probe == bed) ? "Bed":"Tool", temperature.measured);

    if (temperature.setpoint <= 0)
        startTemp = 0;
    else if (startTemp <= 0)
        startTemp = temperature.measured;

    if (startTemp > 0)
    {
        textPos += snprintf_P(text + textPos, sizeof(text) - textPos, PSTR("/%.1f"), temperature.setpoint);
        progress = 100 * (temperature.measured - startTemp) / (temperature.setpoint - startTemp);
        if (progress > 100)
            progress = 100;
    }

    strncpy_P(text + textPos, PSTR(" C"), sizeof(text) - textPos);
    printBarAt(y, progress, text);
}

void statusDisplay::printBarAt (const int y, const float percent, const char * const text, const int height)
{
    const int16_t 
    frameWitdh  = 2, 
    maxWidth    = width() - 2 * frameWitdh, 
    barLength   = maxWidth * (percent / 100.0f),
    barXEnd     = frameWitdh + barLength,
    textYPos    = y + height - 2 * frameWitdh - 8;  // size of text at 1 is 8 pixels

    // print the frame
    drawRect(0, y, width(), height, SSD1306_WHITE);

    // fill it
    fillRect(2, y + frameWitdh, barLength, height - 2 * frameWitdh, SSD1306_WHITE);

    if (text)
    {
        setCursor(0, textYPos);
        // char text[10];
        // itoa((int) percent, text, 10);
        const int16_t textXStart = centerText(text);

        const int16_t textXEnd = getCursorX();
        if (textXStart < barXEnd)
        {
            // reverse the part of the text which should not yet be black on white
            for (int16_t invertY = 0; invertY < 8; invertY++)
            {
                const int16_t 
                nextYPos = textYPos + invertY, 
                endXPos = ((textXEnd < barXEnd) ? textXEnd : barXEnd);

                for (int16_t nextXPos = textXStart; nextXPos < endXPos; nextXPos++)
                {
                    bool pixelIsOn = getPixel(nextXPos, nextYPos);
                    drawPixel(nextXPos, nextYPos, (pixelIsOn ? SSD1306_BLACK : SSD1306_WHITE));
                }
            }
        }
    }

    return;
}

int16_t statusDisplay::centerText (const char * const text)
{
    int16_t xPos = width(), x1, y1;
    const int16_t yPos = getCursorY();
    uint16_t width, height;
    getTextBounds(text, 0, yPos, &x1, &y1, &width, &height);
    xPos -= width;
    xPos /= 2;

    if (xPos < 0)
        xPos = 0;
    
    setCursor(xPos, yPos);
    print(text);

    return xPos;
}

};