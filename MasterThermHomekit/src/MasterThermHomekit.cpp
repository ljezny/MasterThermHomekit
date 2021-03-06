/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#include "application.h"
#line 1 "/Users/ljezny/Projects/MasterThermHomekit/MasterThermHomekit/src/MasterThermHomekit.ino"
/*
 * Project MasterThermHomekit
 * Description:
 * Author:
 * Date:
 */
#include "HKServer.h"
#include "HKLog.h"

#include "MasterThermAccessory.h"

int restart(String extra);
int resetAll(String extra);
int setUsername(String extra);
int setPassword(String extra);
void progress(Progress_t progress);
void setup();
void loop();
#line 12 "/Users/ljezny/Projects/MasterThermHomekit/MasterThermHomekit/src/MasterThermHomekit.ino"
SerialLogHandler logHandler(LOG_LEVEL_ALL);

HKServer *hkServer = NULL;
MasterThermAccessory *acc = new MasterThermAccessory();

// Cloud functions must return int and take one String
int restart(String extra) {
  System.reset();
  return 0;
}

int resetAll(String extra) {
  HKPersistor().resetAll();
  System.reset();
  return 0;
}

int setUsername(String extra) {
  acc->setUsername(extra);
  return 0;
}

int setPassword(String extra) {
  acc->setPassword(extra);
  return 0;
}

void progress(Progress_t progress) {
    hkLog.info("Homekit progress callback: %d",progress);
}

// setup() runs once, when the device is first turned on.
void setup() {
  // Put initialization like pinMode and begin functions here.
	randomSeed(Time.now());//we need to somehow init random seed, so device identity will be unique
  Serial.begin();

  //

  hkServer = new HKServer(acc->getDeviceType(),"MasterTherm Thermostat","523-12-643",progress);

  acc->initAccessorySet();

  hkServer->start();

  Particle.function("restart", restart);
  Particle.function("resetAll", resetAll);
  Particle.function("username", setUsername);
  Particle.function("password", setPassword);
}

// loop() runs over and over again, as quickly as it can execute.
void loop() {
  bool didAnything = false; //!hkServer->hasConnections();
  didAnything |= hkServer->handle(); //handle connections, did anything (i.e processed some requests etc.)
  didAnything |= acc->handle(); //handle accessory, did anything (i.e read some sensors)
  if(didAnything) {
     
  }
}