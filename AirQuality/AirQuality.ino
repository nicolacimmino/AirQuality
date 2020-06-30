
#include <Arduino.h>
#include <Wire.h>
#include "iAQCoreTwoWire.h"
#include <FastLED.h>
#include "terminal.h"

CRGB led[1];

iAQCoreTwoWire iaq(&Wire);

void setup()
{
	Serial.begin(115200);
	VT100.begin(Serial);
	VT100.cursorOff();

	FastLED.addLeds<WS2812B, 4, GRB>(led, 2);
	FastLED.setBrightness(255);

	Wire.begin();

	//iAQ-Core can operate at a maximum of 100kHz clock speed
	Wire.setClock(80000L);

	if (!iaq.begin())
	{
		Serial.println("Cannot initialise sensor.");
		while (1)
			;
	}
}

void loop()
{
	delay(1000);

	bool valid = iaq.hasValue();
	bool runIn = iaq.isRunin();
	bool error = iaq.isError();
	bool busy = iaq.isBusy();

	printStatusBar(valid, runIn, error, busy);
	
	if (valid)
	{
		uint32_t co2 = iaq.getCO2();
		uint32_t tvoc = iaq.getTVOC();

		printReport(co2, tvoc);

		led[0] = CHSV((128 * (2000 - min(2000, co2))) / 2000, 255, 128);
		FastLED.show();
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

		FastLED.show();
	}
}
