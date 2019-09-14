#include "MasterThermAccessory.h"

#include "HKConnection.h"

#include <set>
#include <regex>
#include <Particle.h>
#include "HKLog.h"

#include "HttpClient.h"
#include "sha1.h"


const char* BASE_URL = "mastertherm.vip-it.cz";
const char* PARAMS_BASE_URL = "mastertherm.vip-it.cz";

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

static int messageId = 0;
bool MasterThermAccessory::refreshPassiveData() {
  bool result = false;
  TCPClient client;
  if(client.connect(PARAMS_BASE_URL,8091)) {
    Serial.println("PassiveData: connected.");
    String body = String::format("moduleId=%d&fullRange=true&messageId=1",this->moduleId);

    client.print("POST /mt/PassiveVizualizationServlet HTTP/1.0\r\n");
    client.print("Connection: close\r\n");
    client.print("Content-Type: application/x-www-form-urlencoded\r\n");
    client.printf("Content-Length: %d\r\n",body.length());
    client.printf("Cookie: PHPSESSID=%s; path=/\r\n",sessionId.c_str());
    client.print("\r\n");
    client.print(body);
    client.flush();
    
    const int BUFFER_SIZE = 1024;
    char line_buffer[BUFFER_SIZE] = {'\0'};
    char sessionId[64];
    int pos = 0;
    while(client.connected()) {
      while(client.available()) {
        char x = client.read();
        //Serial.print(x);
        line_buffer[pos++] = x;
        if(x == '\n' || x == ',') {
            //Serial.println(line_buffer);
            int paramId = 0;
            int intValue = 0;
            int floatValue = 0;
            if(sscanf(line_buffer,"\"I_%d\":\"%d\"",&paramId,&intValue)) { 
                
            } else if(sscanf(line_buffer,"\"A_%d\":\"%f\"",&paramId,&floatValue)) { 
            
            } else if(sscanf(line_buffer,"\"D_%d\":\"%d\"",&paramId,&intValue)) { 
              if(paramId == 3) { //HeatPump mode (on/off)
                Serial.printlnf("HeatPump on: %d",intValue);
                currentHeatingState->characteristics::setValue(intValue ? "1":"0");
                currentHeatingState->notify(NULL);
              }
            }

            pos = 0;
            memset(line_buffer,0,BUFFER_SIZE);
        }
      }
    }
  }
  return result;
}

bool MasterThermAccessory::login() {
  bool result = false;
  char hashPassword[41];
  memset(hashPassword,0,41);

  passwordSHA1(credentials.password,hashPassword);

  TCPClient client;
  if(client.connect(BASE_URL,80)) {
    Serial.println("Login: connected.");
    String body = String::format("uname=%s&login=login&upwd=%s",credentials.username,hashPassword);
    Serial.println(body);
    client.print("POST /plugins/mastertherm_login/client_login.php HTTP/1.0\r\n");
    client.print("Connection: close\r\n");
    client.print("Content-Type: application/x-www-form-urlencoded\r\n");
    client.printf("Content-Length: %d\r\n",body.length());
    client.print("\r\n");
    client.print(body);
    
    client.flush();

    const int BUFFER_SIZE = 1024;
    char line_buffer[BUFFER_SIZE] = {'\0'};
    char sessionId[64];
    int pos = 0;
    while(client.connected()) {
      while(client.available()) {
        char x = client.read();
        line_buffer[pos++] = x;
        if(x == '\n' || x == ',') {
            if(sscanf(line_buffer,"Set-Cookie: PHPSESSID=%[^;]; path=/\r\n",sessionId)) { 
                this->sessionId = String(sessionId);
                Serial.printf("sessionId: %s\n",sessionId);
            }
            int moduleId = 0;
            if(sscanf(line_buffer,"\"modules\":[{\"id\":\"%d\",",&moduleId)) { 
                this->moduleId = moduleId;
                Serial.printf("moduleId: %d\n",moduleId);
            }

            pos = 0;
            memset(line_buffer,0,BUFFER_SIZE);
        } 
      }
    }
    client.stop();
  }
  return result;
}


bool MasterThermAccessory::handle() {
    if((millis() - lastUpdateMS) > TIME_PERIOD_MS) {
        lastUpdateMS = millis();

        if(sessionId.length() == 0) {
          login();
        }
        if(sessionId.length() > 0) {
          refreshPassiveData();
        }
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

  currentHeatingState = new intCharacteristics(charType_currentHeatCoolMode,premission_read|premission_notify,0,1,1,unit_none);
  currentHeatingState->characteristics::setValue("0");
  thermostatAcc->addCharacteristics(thermostatService,currentHeatingState);

  intCharacteristics *targetHeatingState = new intCharacteristics(charType_targetHeatCoolMode,premission_read|premission_write|premission_notify,0,1,1,unit_none);
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

