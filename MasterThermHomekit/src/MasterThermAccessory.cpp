#include "MasterThermAccessory.h"

#include "HKConnection.h"

#include <set>
#include <regex>
#include <Particle.h>
#include "HKLog.h"

#include "HttpClient.h"
#include "sha1.h"

#include <regex.h>        
regex_t regex;



const char* BASE_URL = "mastertherm.vip-it.cz";
const char* PARAMS_BASE_URL = "mastertherm.vip-it.cz:8091/mt";

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


void passwordSHA1(char* password, char *hexresult) {
  char result[21];
  size_t offset;

  xSHA1( result, password, strlen(password) );
  
  /* format the hash for comparison */
  for( offset = 0; offset < 20; offset++) {
    sprintf( ( hexresult + (2*offset)), "%02x", result[offset]&0xff);
  }
}

bool MasterThermAccessory::login() {
  HttpClient http;
  char hashPassword[41];
  memset(hashPassword,'\0',41);

  passwordSHA1(credentials.password,hashPassword);

  Serial.printf("MasterTherm login: %s password hash: %s\n",credentials.username,hashPassword);

  http_request_t request;
  http_response_t response;
  http_header_t headers[] = {
      //  { "Content-Type", "application/json" },
      //  { "Accept" , "application/json" },
      { "Content-Type" , "application/x-www-form-urlencoded"},
      { NULL, NULL } // NOTE: Always terminate headers will NULL
  };

  request.hostname = BASE_URL;
  request.port = 80;
  request.path = "/plugins/mastertherm_login/client_login.php";
  request.body = "uname=ljezny%40gmail.com&login=login&upwd=hash";

  http.post(request, response, headers);
  
  char sessionId[64];
  
  Serial.printf("HEADERS SENT: %s\n",response.headers.c_str());
  
  char* token = strtok((char *) response.headers.c_str(), "\r\n");
   
   /* walk through other tokens */
   while( token != NULL ) {
      memset(sessionId,0,64);
      int results = sscanf(token,"Set-Cookie: PHPSESSID=%[^;]; path=/",sessionId);
      if(results > 0) {
        Serial.printf("sessionId: %s\n",sessionId);
        break;
      }
      token = strtok(NULL, "\n");
   }
  
  return true;
}

bool MasterThermAccessory::handle() {
    if((millis() - lastUpdateMS) > TIME_PERIOD_MS) {
        lastUpdateMS = millis();

        //TODO: refresh data from cloud
        login();
    }
}

void MasterThermAccessory::initAccessorySet() {
  //Particle.variable("username", &this->on, INT);



  EEPROM.get(0,credentials);


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

  floatCharacteristics *currentTemperature = new floatCharacteristics(charType_currentTemperature,premission_read|premission_notify,0,40,0.1,unit_celsius);
  currentTemperature->characteristics::setValue("0");
  thermostatAcc->addCharacteristics(thermostatService,currentTemperature);

  floatCharacteristics *targetTemperature = new floatCharacteristics(charType_targetTemperature,premission_read|premission_write|premission_notify,0,40,0.1,unit_celsius);
  targetTemperature->characteristics::setValue("0");
  targetTemperature->valueChangeFunctionCall = std::bind(&MasterThermAccessory::setTargetTemperature, this, std::placeholders::_1, std::placeholders::_2,std::placeholders::_3);
  thermostatAcc->addCharacteristics(thermostatService,targetTemperature);

  intCharacteristics *temperatureDisplayUnits = new intCharacteristics(charType_temperatureUnit,premission_read|premission_write|premission_notify,0,3,1,unit_none); 
  temperatureDisplayUnits->characteristics::setValue("0");
  thermostatAcc->addCharacteristics(thermostatService,temperatureDisplayUnits);
}

