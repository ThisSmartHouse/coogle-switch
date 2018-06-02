#ifndef COOGLESWITCH_H
#define COOGLESWITCH_H

#define NUM_SWITCHES 1
#define TOPIC_ID "bathroom-switch"
#define SWITCH_BASE_TOPIC ""

#define STATUS_LED 13

#define HAS_BUTTON

// List of pins, in order, that we will be switching on/off
static int switches[NUM_SWITCHES] = {
	12
};

#ifdef HAS_BUTTON

#define DEBOUNCE_TIME 200 // Debounce time in MS

int buttonReadings[NUM_SWITCHES] = {
	LOW
};

static int buttons[NUM_SWITCHES] = {
	0
};

long lastToggleTimes[NUM_SWITCHES] = {
	0
};

int currentButtonReading = LOW;

#endif

#endif
