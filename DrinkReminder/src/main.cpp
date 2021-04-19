/*
Required Libraries
	- Adafruit NeoPixel
*/
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <Constants.h>

Adafruit_NeoPixel bottleStrip(BOTTLE_STRIP_COUNT, BOTTLE_STRIP_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel glassStrip(GLASS_STRIP_COUNT, GLASS_STRIP_PIN, NEO_GRB + NEO_KHZ800);

int deviceState; // state of the device
int waterAmount_required; // the amount of water (in ml) the users should have drunk so far
int waterAmount_drunk; // the amount of water (in ml) that has been drunk so far
volatile double waterFlow; // amount of water (in ml) pumped in the current filling process
long lastMillis; // the past milliseconds when you were reminded to drink the last time
long lastSnoozeMillis; // the past milliseconds when the snooze remind was active the last time
bool fillingInterrupted; // was the last filling process interrupted
bool wasFilledUp; // was the glass filled up at least once

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
void initValues(){
	deviceState = STANDARD;
	lastMillis = millis();
	waterAmount_required = 0;
	waterAmount_drunk = 0;
	waterFlow = 0;
	fillingInterrupted = false;
	wasFilledUp = false;
}

// method for sound
void beep(){
		tone(SPEAKER_PIN, TONE_FREQUENCY, TONE_DURATION);
}
// pixel runs from left to right of the strip
void runningPixel(Adafruit_NeoPixel &strip, uint32_t color, int wait){
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
// blink LED for 1 second
void blink(Adafruit_NeoPixel &strip, uint32_t color){
	strip.clear();
	for(int i = 0;  i < strip.numPixels(); i++){
		strip.setPixelColor(i, color);
	}
	strip.show();
	delay(1000);
	strip.clear();
	strip.show();
}

void remindToDrink(){
	Serial.println("Remind to drink");
	beep();
	blink(glassStrip, glassStrip.Color(255, 0, 0));
}

// returns state of the glass: -1 if glass not on plate, 0 if glass on plate and empty, 1 if glass filled with water
int getGlassState() {
	int val = analogRead(PRESSURE_SENSOR_PIN);
	if(val <= THRESHOLD_NO_GLASS){
		return -1;
	}
	else if(val > THRESHOLD_NO_GLASS && val <= THRESHOLD_EMPTY_GLASS){
		return 0;
	}
	else{
		return 1;
	}
}

bool bottleEmpty(){
	// read value from flow sensor
	if (true) {
		return true;
	}
	return false;
}

void pulse() {  // measure the quantity of square waves
	waterFlow += 1.0 / 5880.0; // 5880 is the number of Pulses required for one liter
}




void setup() {
	Serial.begin(9600);
	delay(2000);
	Serial.println("Start");
	initPINS();
	initObjects();
	initValues();
}

void loop() {
	// remind timers work independent of device status
	if (millis() - lastMillis >= REMIND_INTERVAL) { // time to remind the user to drink
		Serial.println("Alarm Alarm Alarm Alarm!!!!");
		waterAmount_required += WATER_AMOUNT;
		remindToDrink();
		lastMillis = millis();
		lastSnoozeMillis = millis();
	}
	else if (waterAmount_drunk < waterAmount_required && millis()  - lastSnoozeMillis >= SNOOZE_INTERVAL ) { // user has not drunk enough so the snooze-remind start every two minutes (until enough water was drunk)
		Serial.println("Snooze Snooze Snooze Snooze!!!!");
		remindToDrink();
		lastSnoozeMillis = millis();
	}
	int glassState = getGlassState();

	switch (deviceState){
		case STANDARD:
			Serial.println("STANDARD");
			if( glassState ==  -1 ){ // no glass on plate
				deviceState = NO_GLASS;
			}
			else if ( glassState == 0 ) { // glass empty
				attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN), pulse, RISING);
				if(wasFilledUp){
					waterAmount_drunk += WATER_AMOUNT;
				}
				wasFilledUp = true;
				digitalWrite(WATER_PUMP_PIN, HIGH); // start water pump
				deviceState = RECHARGING;
			}
			break;

		case RECHARGING:
			Serial.println("RECHARGING");
			if ( glassState ==  -1 ){
				fillingInterrupted = true;
				digitalWrite(WATER_PUMP_PIN, LOW); // stop water pump
				deviceState = NO_GLASS;
			}
			 while ( bottleEmpty() ) { // wait until the bottle is filled up
				runningPixel(bottleStrip, bottleStrip.Color(255, 0, 0), 20);
			}
			if ( waterFlow >= WATER_AMOUNT ){ // 80 ml were filled in the glass
				detachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN)) ;
				digitalWrite(WATER_PUMP_PIN, LOW); // stop water pump
				waterFlow = 0;
				deviceState = STANDARD;
			}
			break;

		case NO_GLASS:
			Serial.println("NO_GLASS");
			if ( glassState !=  -1 ) {
				if(fillingInterrupted){ // if filling was interrupted, the program jumps back to the previous state
					fillingInterrupted = false;
					digitalWrite(WATER_PUMP_PIN, HIGH); // restart water pump
					deviceState = RECHARGING;
				}
				else{
					deviceState = STANDARD;
				}
			}
			//delay(1000); // delay achieves that no random values are measured when putting down glass
			break;
	}
	//delay(5);
}
