
#ifndef __AIRQ_TERMINAL_H__
#define __AIRQ_TERMINAL_H__

#include "VT100.h"
#include "messages.h"

#define TERMINAL_WIDTH 80

void printBanner()
{
    VT100.setCursor(2, 1);
    printMessage(0);
}

void printReport(uint32_t co2, uint32_t tvoc)
{
    VT100.setCursor(10, 2);
    VT100.setTextColor(VT_YELLOW);
    Serial.print("CO2: ");
    VT100.setTextColor(VT_WHITE);
    Serial.print(co2);
    Serial.println("ppm");

    VT100.setCursor(12, 2);
    VT100.setTextColor(VT_YELLOW);
    Serial.print("TVOC: ");
    VT100.setTextColor(VT_WHITE);
    Serial.print(tvoc);
    Serial.println("ppb");
    VT100.cursorOff();
}

void printStatusBar(bool valid, bool runIn, bool error, bool busy)
{
    VT100.setBackgroundColor(VT_BLACK);
    VT100.clearScreen();

    printBanner();

    VT100.setCursor(1, 1);
    VT100.setBackgroundColor(VT_YELLOW);
    VT100.setTextColor(VT_BLACK);
    Serial.print(" AirQ V1.0 ");

    if (valid)
    {
        Serial.print(" VLD ");
    }
    else
    {
        Serial.print("     ");
    }

    if (runIn)
    {
        Serial.print(" RIN ");
    }
    else
    {
        Serial.print("     ");
    }

    if (error)
    {
        Serial.print(" ERR ");
    }
    else
    {
        Serial.print("     ");
    }

    if (busy)
    {
        Serial.print(" BSY ");
    }
    else
    {
        Serial.print("     ");
    }

    VT100.clearLineAfter();
    VT100.setCursor(1, TERMINAL_WIDTH + 1);
    VT100.setBackgroundColor(VT_BLACK);
    VT100.setTextColor(VT_WHITE);
    VT100.clearLineAfter();
}

#endif