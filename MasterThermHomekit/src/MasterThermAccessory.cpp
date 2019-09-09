#include "MasterThermAccessory.h"

#include "HKConnection.h"

#include <set>

#include <Particle.h>
#include "HKLog.h"


void MasterThermAccessory::setTargetHeatingCoolingState (bool oldValue, bool newValue, HKConnection *sender){
    //Particle.publish("nixie/power", newValue ? "on" : "off", PUBLIC);
    //on = newValue ? 1 : 0;
}

void MasterThermAccessory::setTargetTemperature (float oldValue, float newValue, HKConnection *sender){
    //Particle.publish("nixie/power", newValue ? "on" : "off", PUBLIC);
    //on = newValue ? 1 : 0;
}

void MasterThermAccessory::identify(bool oldValue, bool newValue, HKConnection *sender) {
    
}

bool MasterThermAccessory::handle() {
    if((millis() - lastUpdateMS) > TIME_PERIOD_MS) {
        lastUpdateMS = millis();

        //TODO: refresh data from cloud
    }
}

void MasterThermAccessory::initAccessorySet() {
  //Particle.variable("username", &this->on, INT);
  
  Accessory *thermostatAcc = new Accessory();

  //Add Thermostat
  AccessorySet *accSet = &AccessorySet::getInstance();
  addInfoServiceToAccessory(thermostatAcc, "MasterTherm HeatPump", "MasterTherm", "MasterTherm", "1","1.0.0", std::bind(&MasterThermAccessory::identify, this, std::placeholders::_1, std::placeholders::_2,std::placeholders::_3));
  accSet->addAccessory(thermostatAcc);

  Service *thermostatService = new Service(serviceType_thermostat);
  thermostatAcc->addService(thermostatService);

  intCharacteristics *currentHeatingState = new intCharacteristics(charType_currentHeatCoolMode,premission_read|premission_notify,0,3,1,unit_none);
  currentHeatingState->characteristics::setValue("0");
  thermostatAcc->addCharacteristics(thermostatService,currentHeatingState);

  intCharacteristics *targetHeatingState = new intCharacteristics(charType_targetHeatCoolMode,premission_read|premission_write|premission_notify,0,3,1,unit_none);
  targetHeatingState->characteristics::setValue("0");
  targetHeatingState->valueChangeFunctionCall = std::bind(&MasterThermAccessory::setTargetHeatingCoolingState, this, std::placeholders::_1, std::placeholders::_2,std::placeholders::_3);
  thermostatAcc->addCharacteristics(thermostatService,targetHeatingState);

  floatCharacteristics *currentTemperature = new floatCharacteristics(charType_currentTemperature,premission_read|premission_notify,0,0,40,unit_celsius);
  currentTemperature->characteristics::setValue("0");
  thermostatAcc->addCharacteristics(thermostatService,currentTemperature);

  floatCharacteristics *targetTemperature = new floatCharacteristics(charType_targetTemperature,premission_read|premission_write|premission_notify,0,0,40,unit_celsius);
  targetTemperature->characteristics::setValue("0");
  targetTemperature->valueChangeFunctionCall = std::bind(&MasterThermAccessory::setTargetTemperature, this, std::placeholders::_1, std::placeholders::_2,std::placeholders::_3);
  thermostatAcc->addCharacteristics(thermostatService,targetTemperature);

  intCharacteristics *temperatureDisplayUnits = new intCharacteristics(charType_temperatureUnit,premission_read|premission_write|premission_notify,0,3,1,unit_none); 
  temperatureDisplayUnits->characteristics::setValue("0");
  thermostatAcc->addCharacteristics(thermostatService,temperatureDisplayUnits);
}