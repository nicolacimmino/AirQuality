/* 
Upload Source Code
*/
#include <Arduino.h>
#include <Wire.h>
#include <iAQCoreTwoWire.h>
#include <Sodaq_SHT2x.h>
#include <FastLED.h>
#include "VT100.h"
#include "messages.h"

#define TERMINAL_WIDTH 80
#define TERMINAL_KEY_ESC 0x1B

#define DUST_MEASURE_PIN A0
#define DUST_LED_PWR_PIN 5
#define DUST_SAMPLING_TIME 280
#define DUST_DELTA_TIME 40
#define DUST_SLEEP_TIME 9680

float dustDensity;
float humidity;
float temperature;
float dewpoint;
uint32_t co2;
uint32_t tvoc;
bool airQValid;
float dustDensityMax;
float humidityMax;
float temperatureMax;
float dewpointMax;
uint32_t co2Max;
uint32_t tvocMax;
float dustDensityMin;
float humidityMin;
float temperatureMin;
float dewpointMin;
uint32_t co2Min;
uint32_t tvocMin;

iAQCoreTwoWire iaq(&Wire);

CRGB led[1];

void printBanner()
{
  VT100.setCursor(2, 1);
  printMessage(0);
}

void reportValue(uint8_t row, float value, float valueMin, float valueMax)
{
  VT100.setCursor(row, 19);
  VT100.setTextColor(VT_WHITE);
  Serial.print(value);
  Serial.print(" ");

  VT100.setCursor(row, 28);
  VT100.setTextColor(VT_WHITE);
  Serial.print(valueMin);
  Serial.print(" ");

  VT100.setCursor(row, 37);
  VT100.setTextColor(VT_WHITE);
  Serial.print(valueMax);
  Serial.print(" ");
}

void reportValue(uint8_t row, uint32_t value, uint32_t valueMin, uint32_t valueMax)
{
  VT100.setCursor(row, 19);
  VT100.setTextColor(VT_WHITE);
  Serial.print(value);
  Serial.print(" ");

  VT100.setCursor(row, 28);
  VT100.setTextColor(VT_WHITE);
  Serial.print(valueMin);
  Serial.print(" ");

  VT100.setCursor(row, 37);
  VT100.setTextColor(VT_WHITE);
  Serial.print(valueMax);
  Serial.print(" ");
}

void printReport(bool reprintTable)
{
  if (reprintTable)
  {
    VT100.setCursor(9, 1);
    Serial.println(F(" \e[2m\e[32m\e(0\x6c\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x77\x71\x71\x71\x71\x71\x71\x71\x71\x77\x71\x71\x71\x71\x71\x71\x71\x71\x77\x71\x71\x71\x71\x71\x71\x71\x71\x77\x71\x71\x71\x71\x71\x71\x71\x71\x77\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x6b\e(B"));
    Serial.println(F(" \e(0\x78 \e(BParameter\e(0    \x78 \e(BValue\e(0  \x78 \e(BMin\e(0    \x78 \e(BMax\e(0    \x78 \e(BUnit\e(0   \x78 \e(BNotes\e(0                  \x78\e(B"));
    Serial.println(F(" \e(0\x74\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x6E\x71\x71\x71\x71\x71\x71\x71\x71\x6E\x71\x71\x71\x71\x71\x71\x71\x71\x6E\x71\x71\x71\x71\x71\x71\x71\x71\x6E\x71\x71\x71\x71\x71\x71\x71\x71\x6E\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x75\e(B"));
    Serial.println(F(" \e(0\x78 \e(B\e[0m\e[33mPM2.5\e[2m\e[32m\e(0        \x78        \x78        \x78        \x78 \e(B\e[0m\e[33mg/m3\e[2m\e[32m\e(0   \x78                        \x78\e(B"));
    Serial.println(F(" \e(0\x74\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x6E\x71\x71\x71\x71\x71\x71\x71\x71\x6E\x71\x71\x71\x71\x71\x71\x71\x71\x6E\x71\x71\x71\x71\x71\x71\x71\x71\x6E\x71\x71\x71\x71\x71\x71\x71\x71\x6E\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x75\e(B"));
    Serial.println(F(" \e(0\x78 \e(B\e[0m\e[33mHumidity\e[2m\e[32m\e(0     \x78        \x78        \x78        \x78 \e(B\e[0m\e[33m%RH\e[2m\e[32m\e(0    \x78                        \x78\e(B"));
    Serial.println(F(" \e(0\x74\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x6E\x71\x71\x71\x71\x71\x71\x71\x71\x6E\x71\x71\x71\x71\x71\x71\x71\x71\x6E\x71\x71\x71\x71\x71\x71\x71\x71\x6E\x71\x71\x71\x71\x71\x71\x71\x71\x6E\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x75\e(B"));
    Serial.println(F(" \e(0\x78 \e(B\e[0m\e[33mTemperature\e[2m\e[32m\e(0  \x78        \x78        \x78        \x78 \e(B\e[0m\e[33mC\e[2m\e[32m\e(0      \x78                        \x78\e(B"));
    Serial.println(F(" \e(0\x74\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x6E\x71\x71\x71\x71\x71\x71\x71\x71\x6E\x71\x71\x71\x71\x71\x71\x71\x71\x6E\x71\x71\x71\x71\x71\x71\x71\x71\x6E\x71\x71\x71\x71\x71\x71\x71\x71\x6E\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x75\e(B"));
    Serial.println(F(" \e(0\x78 \e(B\e[0m\e[33mDew Point\e[2m\e[32m\e(0    \x78        \x78        \x78        \x78 \e(B\e[0m\e[33mC\e[2m\e[32m\e(0      \x78                        \x78\e(B"));
    Serial.println(F(" \e(0\x74\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x6E\x71\x71\x71\x71\x71\x71\x71\x71\x6E\x71\x71\x71\x71\x71\x71\x71\x71\x6E\x71\x71\x71\x71\x71\x71\x71\x71\x6E\x71\x71\x71\x71\x71\x71\x71\x71\x6E\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x75\e(B"));
    Serial.println(F(" \e(0\x78 \e(B\e[0m\e[33mCO2\e[2m\e[32m\e(0          \x78        \x78        \x78        \x78 \e(B\e[0m\e[33mppm\e[2m\e[32m\e(0    \x78                        \x78\e(B"));
    Serial.println(F(" \e(0\x74\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x6E\x71\x71\x71\x71\x71\x71\x71\x71\x6E\x71\x71\x71\x71\x71\x71\x71\x71\x6E\x71\x71\x71\x71\x71\x71\x71\x71\x6E\x71\x71\x71\x71\x71\x71\x71\x71\x6E\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x75\e(B"));
    Serial.println(F(" \e(0\x78 \e(B\e[0m\e[33mTVOC\e[2m\e[32m\e(0         \x78        \x78        \x78        \x78 \e(B\e[0m\e[33mppb\e[2m\e[32m\e(0    \x78                        \x78\e(B"));
    Serial.println(F(" \e(0\x6d\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x76\x71\x71\x71\x71\x71\x71\x71\x71\x76\x71\x71\x71\x71\x71\x71\x71\x71\x76\x71\x71\x71\x71\x71\x71\x71\x71\x76\x71\x71\x71\x71\x71\x71\x71\x71\x76\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x6a\e(B\e[0m"));
  }

  reportValue(12, dustDensity, dustDensityMin, dustDensityMax);
  reportValue(14, humidity, humidityMin, humidityMax);
  reportValue(16, temperature, temperatureMin, temperatureMax);
  reportValue(18, dewpoint, dewpointMin, dewpointMax);
  reportValue(20, co2, co2Min, co2Max);
  reportValue(22, tvoc, tvocMin, tvocMax);

  VT100.cursorOff();
}

void printStatusBar()
{
  VT100.setBackgroundColor(VT_BLACK);

  printBanner();

  VT100.setCursor(1, 1);
  VT100.setBackgroundColor(VT_YELLOW);
  VT100.setTextColor(VT_BLACK);
  Serial.print(" AirQ V1.0 ");
  VT100.clearLineAfter();
  VT100.setCursor(1, TERMINAL_WIDTH + 1);
  VT100.setBackgroundColor(VT_BLACK);
  VT100.setTextColor(VT_WHITE);
  VT100.clearLineAfter();
}

void setup()
{
  Serial.begin(115200);
  VT100.begin(Serial);
  VT100.cursorOff();

  pinMode(DUST_LED_PWR_PIN, OUTPUT);
  pinMode(DUST_MEASURE_PIN, INPUT);

  FastLED.addLeds<WS2812B, 4, GRB>(led, 2);
  FastLED.setBrightness(255);

  //iAQ-Core can operate at a maximum of 100kHz clock speed
  Wire.setClock(80000L);

  Wire.begin();

  if (!iaq.begin())
  {
    Serial.println("Cannot initialise sensor.");
    while (1)
      ;
  }
}

void resetMaxMin()
{
  dustDensityMax = 0;
  humidityMax = 0;
  temperatureMax = 0;
  dewpointMax = 0;
  co2Max = 0;
  tvocMax = 0;
  dustDensityMin = 0;
  humidityMin = 0;
  temperatureMin = 0;
  dewpointMin = 0;
  co2Min = 0;
  tvocMin = 0;
}

void loop()
{
  digitalWrite(DUST_LED_PWR_PIN, LOW);
  delayMicroseconds(DUST_SAMPLING_TIME);

  float voMeasured = analogRead(DUST_MEASURE_PIN);

  delayMicroseconds(DUST_DELTA_TIME);
  digitalWrite(DUST_LED_PWR_PIN, HIGH);
  delayMicroseconds(DUST_SLEEP_TIME);

  dustDensity = max(0, 0.17 * (voMeasured * (5.0 / 1024)) - 0.1);
  dustDensityMax = max(dustDensity, dustDensityMax);
  dustDensityMin = dustDensityMin == 0 ? dustDensity : min(dustDensity, dustDensityMin);

  humidity = SHT2x.GetHumidity();
  humidityMax = max(humidity, humidityMax);
  humidityMin = humidityMin == 0 ? humidity : min(humidity, humidityMin);

  temperature = SHT2x.GetTemperature();
  temperatureMax = max(temperature, temperatureMax);
  temperatureMin = temperatureMin == 0 ? temperature : min(temperature, temperatureMin);

  dewpoint = SHT2x.GetDewPoint();
  dewpointMax = max(dewpoint, dewpointMax);
  dewpointMin = dewpointMin == 0 ? dewpoint : min(dewpoint, dewpointMin);

  airQValid = iaq.hasValue();

  if (airQValid)
  {
    co2 = iaq.getCO2();
    co2Max = max(co2, co2Max);
    co2Min = co2Min == 0 ? co2 : min(co2, co2Min);

    tvoc = iaq.getTVOC();
    tvocMax = max(tvoc, tvocMax);
    tvocMin = tvocMin == 0 ? tvoc : min(tvoc, tvocMin);

    led[0] = CHSV((128 * (2000 - min(2000, co2))) / 2000, 255, 128);
  }
  else
  {
    if (iaq.isRunin())
    {
      led[0] = CHSV(200, 255, 128);
    }

    if (iaq.isError())
    {
      led[0] = CHSV(0, 255, 128);
    }

    if (iaq.isBusy())
    {
      led[0] = CHSV(160, 255, 128);
    }

    if (((millis() / 1000) % 2) == 0)
    {
      led[0].fadeToBlackBy(240);
    }
  }

  int keyStroke = Serial.read();

  if (keyStroke == 'c')
  {
    resetMaxMin();
  }

  bool reprintReportTable = false;
  if (keyStroke == TERMINAL_KEY_ESC)
  {
    reprintReportTable = true;
  }

  FastLED.show();
  printStatusBar();
  printReport(reprintReportTable);
  //delay(1000);
}
