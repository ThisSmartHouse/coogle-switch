#include <CoogleIOT.h>

#define SERIAL_BAUD 115200

#define NUM_SWITCHES 4
#define CLIENT_ID "freshwater-power"
#define SWITCH_BASE_TOPIC "/" CLIENT_ID "/switch/"

CoogleIOT *iot;
PubSubClient *mqtt;

char msg[150];

// List of pins, in order, that we will be switching on/off
static int switches[NUM_SWITCHES] = {
	4, 5, 12, 14
};

void setup() {

  iot = new CoogleIOT(LED_BUILTIN);

  iot->enableSerial(115200);
  iot->initialize();

  if(!iot->mqttActive()) {
	iot->error("Initialization failure, invalid MQTT Server connection.");
    return;
  }

  mqtt = iot->getMQTTClient();

  iot->logPrintf(INFO, "Initializing with %d switch(es)", NUM_SWITCHES);

  for(int i = 0; i < sizeof(switches) / sizeof(int); i++) {
	pinMode(switches[i], OUTPUT);
  }

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
}
