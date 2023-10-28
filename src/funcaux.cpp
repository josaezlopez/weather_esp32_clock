#include "funcaux.h"

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
  log_d("Enable DS: %d\r\nLat: %f\r\nLon: %f\r\nLanguage: %s\r\nTimeZone: %d\r\nmonthEndDS: %d\r\nmonthStartDS: %d\r\nSSID: %S\r\nsPassword: %s\r\nT_Update: %d\r\nApiKey: %s\r\n",
        setting.dayligthSaving,
        setting.home.lat,
        setting.home.lon,
        setting.lang,
        setting.timeZone,
        setting.monthEndDS,
        setting.monthStartDS,
        setting.ssid,
        setting.password,
        setting.owm_tupdate,
        setting.owm_apikey);

}