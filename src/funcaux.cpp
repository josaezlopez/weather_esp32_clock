#include <funcaux.h>

extern conf setting;

void clearEEPROM(){
    EEPROM.begin(EEPROMSIZE);
    for (int address = 0; address < EEPROMSIZE ; address++){
      EEPROM.write(address,0);
  }
    EEPROM.commit();
  
}

// Return true if the crc is ok
bool checkEEPROM(){
  uint32_t EEPROMcrc;
  uint32_t crcCal;
  CRC32 crc;

    // Check eeprom
  for (int address = 0; address < EEPROMSIZE - sizeof(uint32_t); address++){
      crc.update(EEPROM.read(address));
  }
  crcCal = crc.finalize();
  EEPROMcrc = EEPROM.get(EEPROMSIZE - sizeof(uint32_t), EEPROMcrc);
  return crcCal==EEPROMcrc;

}

// update crc
void updateCRC(){
  CRC32 crc;
    for (int address = 0; address < EEPROMSIZE - sizeof(uint32_t); address++){
      crc.update(EEPROM.read(address));
      }

    EEPROM.put(EEPROMSIZE - sizeof(uint32_t), crc.finalize());
    EEPROM.commit();

}

// Return the setting
conf getConfig(){
  conf setting;

  EEPROM.begin(EEPROMSIZE);

  if(!checkEEPROM()){
    Serial.println("Actualizando EEPROM");
    for(int address=0 ; address<EEPROMSIZE ; address++){
      EEPROM.write(address,0x0);
      }

    // Coordendas iniciales  
    strcpy(setting.lang,DEFAULT_LANG);
    strcpy(setting.unitTemp,DEFAULT_UNIT_TEMP); 
    setting.home.lat = DEFAULT_LAT;
    setting.home.lon = DEFAULT_LON;
    setting.timeZone = DEFAULT_TIMEZONE;
    setting.dayligthSaving = DEFAULT_DAYLIGTHSAVING;
    setting.monthStartDS = DEFAULT_MONTH_START_DS;
    setting.monthEndDS = DEFAULT_MONTH_END_DS;
    strcpy(setting.ssid,DEFAULT_SSID);
    strcpy(setting.password,DEFAULT_PASSWD);
    strcpy(setting.owm_apikey,DEFAULT_OPENWEATHERMAP_APIKEY);
    setting.owm_tupdate = DEFAULT_OPENWEATHERMAP_UPDATE;

    EEPROM.put(0x0,setting);
    updateCRC();

  }

  else{
    EEPROM.get(0x0,setting);
  }

  return setting;
}

void debugSettings(){
  Serial.println("---------------------------------------");
  Serial.printf("Enable DS: %d\r\n",setting.dayligthSaving);
  Serial.printf("Enable Lat: %f\r\n",setting.home.lat);
  Serial.printf("Enable Lat: %f\r\n",setting.home.lon);
  Serial.printf("Language: %s\r\n",setting.lang);
  Serial.printf("TimeZone: %d\r\n",setting.timeZone);
  Serial.printf("monthEndDS: %d\r\n",setting.monthEndDS);
  Serial.printf("monthStartDS: %d\r\n",setting.monthStartDS);
  Serial.printf("SSID: %S\r\n",setting.ssid);
  Serial.printf("Password: %s\r\n",setting.password);
  Serial.printf("T_Update: %d\r\n",setting.owm_tupdate);
  Serial.printf("ApiKey: %s\r\n",setting.owm_apikey);
  Serial.println("--------------------------------------");
}