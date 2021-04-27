/*Required Libraries: Adafruit NeoPixel*/
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <Constants.h>

Adafruit_NeoPixel bottleStrip(BOTTLE_STRIP_COUNT, BOTTLE_STRIP_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel glassStrip(GLASS_STRIP_COUNT, GLASS_STRIP_PIN, NEO_GRB + NEO_KHZ800);

int deviceState; // state of the device
double waterAmount_required; // the amount of water (in ml) the users should have drunk so far
double waterAmount_drunk; // the amount of water (in ml) that has been drunk so far
volatile double waterFlow; // amount of water (in ml) pumped in the current filling process
long lastMillis; // the past milliseconds when you were reminded to drink the last time
long lastSnoozeMillis; // the past milliseconds when the snooze remind was active the last time
bool fillingInterrupted; // was the last filling process interrupted
bool wasFilledUp; // was the glass filled up at least once
long rechargingStart;

void initPINS(){
	pinMode(FLOW_SENSOR_PIN, INPUT);
	pinMode(PRESSURE_SENSOR_PIN, INPUT);
	pinMode(SPEAKER_PIN, OUTPUT);
	pinMode(WATER_PUMP_PIN, OUTPUT);
	pinMode(BOTTLE_STRIP_PIN, OUTPUT);
	pinMode(GLASS_STRIP_PIN, OUTPUT);
}

// method for sound
void beep(){
		tone(SPEAKER_PIN, TONE_FREQUENCY, TONE_DURATION);
}
// pixel runs from left to right of the strip
// has delay of wait*number of LEDs on strip
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
	strip.clear();
	strip.show();
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
void pulse() {  // measure the quantity of square waves
	waterFlow += 1.0 / PULSES_PER_LITRE; // 5880 is the number of Pulses required for one liter
}

void initObjects(){
	bottleStrip.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
	bottleStrip.clear();
	bottleStrip.show(); // Turn OFF all pixels ASAP
	bottleStrip.setBrightness(LED_BRIGHTNESS); // Set BRIGHTNESS to about 1/5 (max = 255)
	glassStrip.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
	glassStrip.clear();
	glassStrip.show(); // Turn OFF all pixels ASAP
	glassStrip.setBrightness(LED_BRIGHTNESS); // Set BRIGHTNESS to about 1/5 (max = 255)
}
void initValues(){
	deviceState = STANDARD;
	lastMillis = millis();
	waterAmount_required = 0.0;
	waterAmount_drunk = 0.0;
	waterFlow = 0;
	fillingInterrupted = false;
	wasFilledUp = false;
}
void setup() {
	Serial.begin(9600);
	initPINS();
	initObjects();
	initValues();
	delay(3000);
}

void loop() {
	// remind timers work independent of device status
	if (millis() - lastMillis >= REMIND_INTERVAL) { // time to remind the user to drink
		waterAmount_required += WATER_AMOUNT;
		if(waterAmount_drunk < waterAmount_required){
			remindToDrink();
		}
		lastMillis = millis();
		lastSnoozeMillis = millis();
	}
	else if (waterAmount_drunk < waterAmount_required && millis()  - lastSnoozeMillis >= SNOOZE_INTERVAL ) { // user has not drunk enough so the snooze-remind start every two minutes (until enough water was drunk)
		remindToDrink();
		lastSnoozeMillis = millis();
	}
	int glassState = getGlassState();

	switch (deviceState){
		case STANDARD:
			if( glassState ==  -1 ){ // no glass on plate
				deviceState = NO_GLASS;
			}
			else if ( glassState == 0 ) { // glass empty
				attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN), pulse, RISING); // flow sensor starts measuring
				if(wasFilledUp){
					waterAmount_drunk += WATER_AMOUNT;
				}
				wasFilledUp = true;
				digitalWrite(WATER_PUMP_PIN, HIGH); // start water pump
				deviceState = RECHARGING;
				rechargingStart = millis() + DELAY;
			}
			break;

		case RECHARGING:
			if ( glassState ==  -1 ){
				fillingInterrupted = true;
				digitalWrite(WATER_PUMP_PIN, LOW); // stop water pump
				deviceState = NO_GLASS;
			}
			if ( waterFlow >= WATER_AMOUNT ){ // 80 ml were filled in the glass
				digitalWrite(WATER_PUMP_PIN, LOW); // stop water pump
				detachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN)) ; // flow sensor stops measuring
				waterFlow = 0;
				deviceState = STANDARD;
			}
			if(millis() -rechargingStart >= RECHARGE_TIMEOUT ){ // bottle seems to be empty
				runningPixel(bottleStrip, bottleStrip.Color(255, 0,0), 50);
				runningPixel(bottleStrip, bottleStrip.Color(255, 0,0), 50);
				rechargingStart = millis() + DELAY;
			}
			break;

		case NO_GLASS:
			if ( glassState !=  -1 ) {
				if(fillingInterrupted){ // if filling was interrupted, the program jumps back to the previous state
					fillingInterrupted = false;
					digitalWrite(WATER_PUMP_PIN, HIGH); // restart water pump
					deviceState = RECHARGING;
					rechargingStart = millis() + DELAY;
				}
				else{
					deviceState = STANDARD;
				}
			}
			delay(1500); // delay achieves that no random values are measured when putting down glass
			break;
	}
	delay(DELAY);
}
