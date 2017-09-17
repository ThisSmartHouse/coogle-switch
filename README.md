# CoogleSwitch

A generic multi-purpose sketch built on top of the CoogleIOT platform to control an arbitrary number
of switches via GPIO pins on an ESP8266 using MQTT. 

## Usage

Once you've downloaded the sketch, you'll need to add the CoogleIOT library to your project. You will also need to
configure the settings by editing the `config.h` file. An example of this file is shown below:

```
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
```

`NUM_SWITCHES` The total number of switches being controlled
`TOPIC_ID` The MQTT topic ID being used (see below)
`SWITCH_BASE_TOPIC` The Base Topic (see below)
`switches[NUM_SWITCHES]` An array of pin numbers for your device, where the first pin is Switch #1, second Switch #2, etc.

### Controlling the Switches

The switches will be controlled via MQTT by publishing a 1 or a 0 to `SWITCH_BASE_TOPIC`/`TOPIC_ID`/switch/`SWITCH NUMBER`. Since by default
`SWITCH_BASE_TOPIC` is empty you can turn on/off the switches using the configuration described above by publishing a 1 or a 0 to:

```
/hallway-power/switch/1
```

The state of each switch can also be determined by subscribing to the same with an additional `state` appended:

```
/hallway-power/switch/1/state
```
