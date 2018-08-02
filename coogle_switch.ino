/*
  +----------------------------------------------------------------------+
  | CoogleSwitch for ESP8266                                             |
  +----------------------------------------------------------------------+
  | Copyright (c) 2017-2018 John Coggeshall                              |
  +----------------------------------------------------------------------+
  | Licensed under the Apache License, Version 2.0 (the "License");      |
  | you may not use this file except in compliance with the License. You |
  | may obtain a copy of the License at:                                 |
  |                                                                      |
  | http://www.apache.org/licenses/LICENSE-2.0                           |
  |                                                                      |
  | Unless required by applicable law or agreed to in writing, software  |
  | distributed under the License is distributed on an "AS IS" BASIS,    |
  | WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or      |
  | implied. See the License for the specific language governing         |
  | permissions and limitations under the License.                       |
  +----------------------------------------------------------------------+
  | Authors: John Coggeshall <john@thissmarthouse.com>                   |
  +----------------------------------------------------------------------+
*/

#include <CoogleIOT.h>

#include "config.h"

#ifndef STATUS_LED
#define STATUS_LED LED_BUILTIN
#endif

#ifndef SERIAL_BAUD
#define SERIAL_BAUD 115200
#endif

#ifndef NUM_SWITCHES
#define NUM_SWITCHES 4
#endif

#ifndef TOPIC_ID
#define TOPIC_ID "coogle-switch"
#endif

#ifndef SWITCH_BASE_TOPIC
#define SWITCH_BASE_TOPIC ""
#endif

#define SWITCH_BASE_TOPIC "/" TOPIC_ID "/switch/"

CoogleIOT *iot;
PubSubClient *mqtt;

char msg[150];

void setup() {

  iot = new CoogleIOT(STATUS_LED);

  iot->enableSerial(SERIAL_BAUD);
  iot->initialize();

  iot->info("CoogleSwitch Initializing...");
  iot->info("-=-=-=-=--=--=-=-=-=-=-=-=-=-=-=-=-=-");
  iot->logPrintf(INFO, "Number of Switches: %d", NUM_SWITCHES);
  iot->logPrintf(INFO, "MQTT Topic ID: %s", TOPIC_ID);

  iot->info("-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-");
  iot->info("Switches");
  iot->info("-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-");

  for(int i = 0; i < sizeof(switches) / sizeof(int); i++) {
	pinMode(switches[i], OUTPUT);
	iot->logPrintf(INFO, "  Switch %d on pin %d", i+1, switches[i]);
  }

  iot->info("");

#ifdef HAS_BUTTON
  for(int i = 0; i < sizeof(buttons) / sizeof(int); i++) {
	  pinMode(buttons[i], INPUT);
  }
#endif

  if(!iot->mqttActive()) {
	iot->error("Initialization failure, invalid MQTT Server connection.");
    return;
  }

  mqtt = iot->getMQTTClient();

  mqtt->setCallback(mqttCallback);

  for(int i = 0; i < sizeof(switches) / sizeof(int); i++) {
	  snprintf(msg, 150, SWITCH_BASE_TOPIC "%d/state", i+1);
	  mqtt->publish(msg, "0", true);

	  iot->logPrintf(DEBUG, "Publishing to state topic '%s'", msg);

	  snprintf(msg, 150, SWITCH_BASE_TOPIC "%d", i+1);
	  mqtt->subscribe(msg);

	  iot->logPrintf(DEBUG, "Subscribed to action topic '%s'", msg);
  }

  iot->info("Coogle Smart Switch Initialized!");
}

void mqttCallback(char *topic, byte *payload, unsigned int length)
{
    unsigned int switchPin = 0;
    unsigned int switchId = 0;
    unsigned int switchState = 0;

    iot->logPrintf(DEBUG, "MQTT Callback Triggered. Topic: %s\n", topic);

    for(int i = 0; i < sizeof(switches) / sizeof(int); i++) {
    	  snprintf(msg, 150, SWITCH_BASE_TOPIC "%d", i + 1);

    	  if(strcmp(topic, msg) == 0) {
    		  switchPin = switches[i];
    		  switchId = i + 1;
    	  }
    }

    if(switchPin <= 0) {
      return;
    }

    iot->logPrintf(DEBUG, "Processing request for switch %d on pin %d\n", switchId, switchPin);

    if((char)payload[0] == '1') {
      digitalWrite(switchPin, HIGH);
      switchState = HIGH;
    } else if((char)payload[0] == '0') {
      digitalWrite(switchPin, LOW);
      switchState = LOW;
    }

    snprintf(msg, 150, SWITCH_BASE_TOPIC "%d/state", switchId);

    mqtt->publish(msg, switchState == HIGH ? "1" : "0", true);
}

void loop() {
  iot->loop();

#ifdef HAS_BUTTON

  for(int i = 0; i < sizeof(buttons) / sizeof(int); i++) {

	int currentReading;

	currentReading = digitalRead(buttons[i]);

	if(currentReading == LOW) {

		if(buttonReadings[i] == HIGH) {

			if((millis() - lastToggleTimes[i]) > DEBOUNCE_TIME) {

				snprintf(msg, 150, SWITCH_BASE_TOPIC "%d/state", i + 1);

				if(digitalRead(switches[i]) == HIGH) {
					digitalWrite(switches[i], LOW);
					if(iot->mqttActive()) {
						mqtt->publish(msg, "0", true);
					}
				} else {
					digitalWrite(switches[i], HIGH);
					if(iot->mqttActive()) {
						mqtt->publish(msg, "1", true);
					}
				}

				lastToggleTimes[i] = millis();

				iot->flashStatus(200);
			}
		}
    }

    buttonReadings[i] = currentReading;

  }

#endif

}
