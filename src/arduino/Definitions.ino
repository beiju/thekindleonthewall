// Connected components
#define RF_TRANSMITTER_PIN 12 // NOTE: Needs to be connected to an interrupt pin (eg. pin 2) [is this correct?]
#define RF_RECIEVER_INTERRUPT 0 // interrupt 0 = pin 2
#define LIGHT_BTN_LED 3
#define LIGHT_BTN_PIN 4
#define RED_LED_PIN 9
#define GREEN_LED_PIN 10
#define BLUE_LED_PIN 11
#define DEBUG_LED 13
#define ENABLED_LIGHTS 0b001

// Raspberry Pi Connection Details
#define DOG_SIGNATURE "Dog"
#define DOG_DEBUG_PREFIX "DOG"
#define BITRATE 115200

// Wireless communication details
#define LIGHTS_SIGNAL_REPETITIONS 2 ///! currently not used in transmission(?)
#define LIGHTS_PERIOD_DURATION 260 // microseconds
#define LIGHTS_SIGNATURE 21022222
#define CUSTOM_SIGNATURE 2,1,0,2,2,2,1,2
#define MAX_TIME_BEFORE_REPEAT_PULSE 1000 // milliseconds

#define BIT_ON(var, mask) ((var & mask) == mask ? "on" : "off")
#define BITWISE_STATUS(var) BIT_ON(var, 0b001), BIT_ON(var, 0b010), BIT_ON(var, 0b100)
