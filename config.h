#ifndef COOGLESWITCH_H
#define COOGLESWITCH_H

#define NUM_SWITCHES 2
#define TOPIC_ID "hallway-power"
#define SWITCH_BASE_TOPIC ""

// List of pins, in order, that we will be switching on/off
static int switches[NUM_SWITCHES] = {
	4, 5
};

#endif
