#ifndef MasterThermAccessory_hpp
#define MasterThermAccessory_hpp

#include "HKAccessory.h"
#include "HAPAccessoryDescriptor.h"

class MasterThermAccessory: public HAPAccessoryDescriptor {
private:
  int TIME_PERIOD_MS = 1000;
  void identify(bool oldValue, bool newValue, HKConnection *sender);
  long lastUpdateMS = 0;
  void setTargetHeatingCoolingState (bool oldValue, bool newValue, HKConnection *sender);
  void setTargetTemperature (float oldValue, float newValue, HKConnection *sender);
public:
  MasterThermAccessory() {
  }

  virtual void initAccessorySet();

  virtual int getDeviceType(){
      return deviceType_thermostat;
  }
  virtual bool handle();
};

#endif