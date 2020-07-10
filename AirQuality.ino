/* 
Upload Source Code
*/
#include <Arduino.h>
#include <Wire.h>
#include <iAQCoreTwoWire.h>
#include <Sodaq_SHT2x.h>
#include <FastLED.h>
#include "terminal.h"

#define DUST_MEASURE_PIN A0
#define DUST_LED_PWR_PIN 5

unsigned int samplingTime = 280;
unsigned int deltaTime = 40;
unsigned int sleepTime = 9680;

float voMeasured = 0;
float calcVoltage = 0;
float dustDensity = 0;

iAQCoreTwoWire iaq(&Wire);

CRGB led[1];

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

void loop()
{

  digitalWrite(DUST_LED_PWR_PIN, LOW);
  delayMicroseconds(samplingTime);

  voMeasured = analogRead(DUST_MEASURE_PIN);

  delayMicroseconds(deltaTime);
  digitalWrite(DUST_LED_PWR_PIN, HIGH);
  delayMicroseconds(sleepTime);

  calcVoltage = voMeasured * (5.0 / 1024);
  dustDensity = max(0, 0.17 * calcVoltage - 0.1);

  float humidity = SHT2x.GetHumidity();
  float temperature = SHT2x.GetTemperature();
  float dewpoint = SHT2x.GetDewPoint();

  bool airQValid = iaq.hasValue();
  bool runIn = iaq.isRunin();
  bool error = iaq.isError();
  bool busy = iaq.isBusy();
  uint32_t co2 = 0;
  uint32_t tvoc = 0;

  if (airQValid)
  {
    co2 = iaq.getCO2();
    tvoc = iaq.getTVOC();

    led[0] = CHSV((128 * (2000 - min(2000, co2))) / 2000, 255, 128);
  }
  else
  {
    if (runIn)
    {
      led[0] = CHSV(200, 255, 128);
    }

    if (error)
    {
      led[0] = CHSV(0, 255, 128);
    }

    if (busy)
    {
      led[0] = CHSV(160, 255, 128);
    }

    if (((millis() / 1000) % 2) == 0)
    {
      led[0].fadeToBlackBy(240);
    }
  }

  FastLED.show();
  printStatusBar(airQValid, runIn, error, busy);
  printReport(dustDensity, humidity, temperature, dewpoint, co2, tvoc, airQValid);
  delay(1000);
}
