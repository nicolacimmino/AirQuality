
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

void printReport(float dustDensity, float humidity, float temperature, float dewpoint, uint32_t co2, uint32_t tvoc, bool airQValid)
{
    VT100.setCursor(10, 2);
    VT100.setTextColor(VT_YELLOW);
    Serial.print("PM2.5: ");
    VT100.setTextColor(VT_WHITE);
    Serial.print(dustDensity);
    Serial.println(" mg/m3");

    VT100.setCursor(12, 2);
    VT100.setTextColor(VT_YELLOW);
    Serial.print("Humidity: ");
    VT100.setTextColor(VT_WHITE);
    Serial.print(humidity);
    Serial.println(" %RH");

    VT100.setCursor(14, 2);
    VT100.setTextColor(VT_YELLOW);
    Serial.print("Temp: ");
    VT100.setTextColor(VT_WHITE);
    Serial.print(temperature);
    Serial.println(" C");

    VT100.setCursor(16, 2);
    VT100.setTextColor(VT_YELLOW);
    Serial.print("Dewpoint: ");
    VT100.setTextColor(VT_WHITE);
    Serial.print(dewpoint);
    Serial.println(" C");

    VT100.setCursor(18, 2);
    VT100.setTextColor(VT_YELLOW);
    Serial.print("CO2: ");
    VT100.setTextColor(VT_WHITE);
    if (airQValid)
    {
        Serial.print(co2);
    }
    else
    {
        Serial.print("---");
    }
    Serial.println(" ppm");

    VT100.setCursor(20, 2);
    VT100.setTextColor(VT_YELLOW);
    Serial.print("TVOC: ");
    VT100.setTextColor(VT_WHITE);
    if (airQValid)
    {
        Serial.print(tvoc);
    }
    else
    {
        Serial.print("---");
    }
    Serial.println(" ppb");

    VT100.cursorOff();
}

void printStatusBar(bool airQValid, bool runIn, bool error, bool busy)
{
    VT100.setBackgroundColor(VT_BLACK);
    VT100.clearScreen();

    printBanner();

    VT100.setCursor(1, 1);
    VT100.setBackgroundColor(VT_YELLOW);
    VT100.setTextColor(VT_BLACK);
    Serial.print(" AirQ V1.0 ");

    if (airQValid)
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