// Connected components
#define RF_TRANSMITTER_PIN 12 // NOTE: Needs to be connected to an interrupt pin (eg. pin 2)
#define RF_RECIEVER_INTERRUPT 0 // interrupt 0 = pin 2
#define LIGHT_BTN_LED 3
#define LIGHT_BTN_PIN 4
#define STATUS_LED_PIN 5
#define DEBUG_LED 13

// Raspberry Pi Connection Details
#define DOG_SIGNATURE "Dog"
#define DOG_DEBUG_PREFIX "DOG"
#define BITRATE 9600

// Wireless communication details
#define LIGHTS_SIGNAL_REPETITIONS 2
#define LIGHTS_PERIOD_DURATION 260 // microseconds
#define LIGHT_1_CODE   432974
#define LIGHT_2_CODE   432956
#define LIGHT_3_CODE   432950
#define LIGHT_12_CODE  432983
#define LIGHT_13_CODE  432977
#define LIGHT_23_CODE  432959
#define LIGHT_123_CODE 432986
#define MAX_TIME_BEFORE_REPEAT_PULSE 1000 // milliseconds

#define BIT_ON(var, mask) ((var & mask) == mask ? "on" : "off")
