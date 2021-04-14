/*Benötigte Libraries
- Adafruit NeoPixel
*/
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <Constants.h>

Adafruit_NeoPixel bottleStrip(BOTTLE_STRIP_COUNT, BOTTLE_STRIP_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel glassStrip(GLASS_STRIP_COUNT, GLASS_STRIP_PIN, NEO_GRB + NEO_KHZ800);

void initPINS(){
	pinMode(FLOW_SENSOR_PIN, INPUT);
	pinMode(PRESSURE_SENSOR_PIN, INPUT);
	pinMode(SPEAKER_PIN, OUTPUT);
	pinMode(WATER_PUMP_PIN, OUTPUT);
}
void initObjects(){
	bottleStrip.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
	bottleStrip.show(); // Turn OFF all pixels ASAP
	bottleStrip.setBrightness(LED_BRIGHTNESS); // Set BRIGHTNESS to about 1/5 (max = 255)
	glassStrip.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
	glassStrip.show(); // Turn OFF all pixels ASAP
	glassStrip.setBrightness(LED_BRIGHTNESS); // Set BRIGHTNESS to about 1/5 (max = 255)
}

void beep(){
		tone(SPEAKER_PIN, TONE_FREQUENCY, TONE_DURATION);
}
void LED_loading_animation(Adafruit_NeoPixel &strip, uint32_t color, int wait){
	for(int i = 0; i < strip.numPixels(); i++){
		strip.clear();
		strip.setPixelColor(i, color);
		strip.show();
		delay(wait);
	}
	for(int i = strip.numPixels()-2; i > 0; i--){
		strip.clear();
		strip.setPixelColor(i, color);
		strip.show();
		delay(wait);
	}
}

void setup() {
	Serial.begin(9600);
	initPINS();
	initObjects();
}


// Thread option: https://create.arduino.cc/projecthub/reanimationxp/how-to-multithread-an-arduino-protothreading-tutorial-dd2c37
void loop() {
	Serial.println(1);
	delay(200);

}
