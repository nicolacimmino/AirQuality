
#include <Arduino.h>
#include <Wire.h>
#include "iAQCoreTwoWire.h"
#include <FastLED.h>
#include "VT100.h"

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
		while (1);
	}
}

void loop()
{ 
	delay(1000);

	//read the sensor to determine if it holds a valid value. 
	//hasValue MUST be called to update sensor values. 
	Serial.print("sensor has valid value: "); Serial.println(iaq.hasValue() ? "true" : "false");

	if (iaq.isRunin()) {
		Serial.println("Sensor in Run-in state");
    led[0] = CHSV(200, 255, 128);
    FastLED.show();
	}
 
	else if (iaq.isBusy()) {
		Serial.println("Sensor busy");
	}
	else if (iaq.isValid())
	{
    uint32_t co2 = iaq.getCO2();
    uint32_t tvoc = iaq.getTVOC();
    
    VT100.setBackgroundColor(VT_BLACK);  
    VT100.clearScreen();

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
 
	
    led[0] = CHSV((128*(2000-min(2000,co2)))/2000, 255, 128);
    FastLED.show();
	}

	else if (iaq.isError())
		Serial.println("Sensor error");
}
