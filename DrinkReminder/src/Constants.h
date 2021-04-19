//Input PINs
#define FLOW_SENSOR_PIN 7 // digital input
#define PRESSURE_SENSOR_PIN A2 // analog input
//Output PINs
#define WATER_PUMP_PIN 2 // digital output
#define GLASS_STRIP_PIN 3 // digital output
#define SPEAKER_PIN 4 // digital output
#define BOTTLE_STRIP_PIN 5 // digital output

//LED strips
#define BOTTLE_STRIP_COUNT 9
#define GLASS_STRIP_COUNT 10
#define LED_BRIGHTNESS 100

//Speaker
#define TONE_FREQUENCY 300
#define TONE_DURATION 1000 // duration in ms

//States
#define STANDARD 0
#define RECHARGING 1
#define BOTTLE_EMPTY 2
#define NO_GLASS 3

//Glass pressure values
#define THRESHOLD_NO_GLASS 3 // until this threshold the glass status is defined as no glass
#define THRESHOLD_EMPTY_GLASS 10 // until this threshold the glass status is defined as empty; if higher than this value, then theres still water in the glass


//Time Intervals and water amount
//#define REMIND_INTERVAL 1800000  // 30 mins * 60 * 1000 (= milliseconds)
//#define SNOOZE_INTERVAL 120000 // 2 mins * 60 * 1000 // not exactly 2 mins to avoid overlaps with REMIND_INTERVAL
#define WATER_AMOUNT 80 // Water amound (in ml) that has to be consumed within the REMIND_INTERVAL

//
#define REMIND_INTERVAL 20000
#define SNOOZE_INTERVAL 5010
