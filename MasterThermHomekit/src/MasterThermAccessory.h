#ifndef MasterThermAccessory_hpp
#define MasterThermAccessory_hpp

#include "HKAccessory.h"
#include "HAPAccessoryDescriptor.h"
#include <limits.h>

typedef struct MasterThermCredentials {
  char username[24];
  char password[24];
};

class MasterThermAccessory: public HAPAccessoryDescriptor {
private:
  const int REFRESH_PERIOD_MS = 30000;
  const int LOGIN_PERIOD_MS = 60000;
  const int MIN_TEMPERATURE = 15;
  const int MAX_TEMPERATURE = 30;

  long lastRefreshPassiveDataMS = -REFRESH_PERIOD_MS;
  long lastLoginMS = -LOGIN_PERIOD_MS;

  String sessionId = "";
  int moduleId = 0;
  void identify(bool oldValue, bool newValue, HKConnection *sender);

  void setTargetHeatingCoolingState (bool oldValue, bool newValue, HKConnection *sender);
  void setTargetTemperature (float oldValue, float newValue, HKConnection *sender);

  MasterThermCredentials credentials;

  intCharacteristics *currentHeatingState;
  floatCharacteristics *currentTemperature;
  floatCharacteristics *targetTemperature;
  intCharacteristics *targetHeatingState;

  void checkLogin();
  void checkPassiveData();
  bool performLogin();
  bool performRefreshPassiveData();
  bool performSetActiveData(String variableId, String variableValue);

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
