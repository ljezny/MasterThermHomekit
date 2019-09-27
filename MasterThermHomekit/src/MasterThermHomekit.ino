/*
 * Project MasterThermHomekit
 * Description:
 * Author:
 * Date:
 */
#include "HKServer.h"
#include "HKLog.h"

#include "MasterThermAccessory.h"

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
  hkServer->handle(); //handle connections, did anything (i.e processed some requests etc.)
  if(hkServer->isPaired()) {
    acc->handle();
  }

}