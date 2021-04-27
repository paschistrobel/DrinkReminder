//Input PINs
#define FLOW_SENSOR_PIN 7 // digital input
#define PRESSURE_SENSOR_PIN A2 // analog input

//Output PINs
#define WATER_PUMP_PIN 2 // digital output
#define GLASS_STRIP_PIN 3 // digital output
#define SPEAKER_PIN 4 // digital output
#define BOTTLE_STRIP_PIN 5 // digital output

//LED strips
#define BOTTLE_STRIP_COUNT 11
#define GLASS_STRIP_COUNT 12
#define LED_BRIGHTNESS 100

//Speaker
#define TONE_FREQUENCY 300
#define TONE_DURATION 1000 // duration in ms

// Water flow sensor
#define PULSES_PER_LITRE 5880.0 // Pulses that are needed for flow sensor to measure one litre

//States
#define STANDARD 0
#define RECHARGING 1
#define BOTTLE_EMPTY 2
#define NO_GLASS 3

//Glass pressure values
#define THRESHOLD_NO_GLASS 15 // if below this value, no glass is on plate
#define THRESHOLD_EMPTY_GLASS 31 // if this or above this value, glass still contains

//Time Intervals and water amount
#define REMIND_INTERVAL 1800000  // 30 minutes // set to 60000 (1 minute) for testing
#define SNOOZE_INTERVAL 300000 // 5 minutes // set to 5000 (5 seconds) for testing
#define WATER_AMOUNT 0.08 // Water amound (in l) that has to be consumed within the REMIND_INTERVAL --> 80 ml
#define DELAY 100
#define RECHARGE_TIMEOUT 10000
