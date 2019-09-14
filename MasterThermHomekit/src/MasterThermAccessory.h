#ifndef MasterThermAccessory_hpp
#define MasterThermAccessory_hpp

#include "HKAccessory.h"
#include "HAPAccessoryDescriptor.h"

typedef struct MasterThermCredentials {
  char username[24];
  char password[24];
};

class MasterThermAccessory: public HAPAccessoryDescriptor {
private:
  int TIME_PERIOD_MS = 20000;
  String sessionId = "";
  int moduleId = 0;
  void identify(bool oldValue, bool newValue, HKConnection *sender);
  long lastUpdateMS = 0;
  void setTargetHeatingCoolingState (bool oldValue, bool newValue, HKConnection *sender);
  void setTargetTemperature (float oldValue, float newValue, HKConnection *sender);

  MasterThermCredentials credentials;

  intCharacteristics *currentHeatingState;

  bool login();
  bool refreshPassiveData();
public:
  MasterThermAccessory() {
  }

  virtual void initAccessorySet();

  virtual int getDeviceType(){
      return deviceType_thermostat;
  }
  virtual bool handle();

  void setUsername(String userName) {
    memset(credentials.username,'\0',24);
    strcpy(credentials.username,userName.c_str());
    EEPROM.put(0,credentials);
  }

  void setPassword(String password) {
    memset(credentials.password,'\0',24);
    strcpy(credentials.password,password.c_str());
    EEPROM.put(0,credentials);
  }
};

#endif