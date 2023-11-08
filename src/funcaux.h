#pragma once

#include <Arduino.h>
#include <EEPROM.h>
#include <CRC32.h>
#include "Weather.h"
#include "conf.h"
#include <list>
#include "Adafruit_ILI9341Ext.h"

struct ssidInfo{
  char ssid[MAX_SSID_LEN + 1];
  int32_t rssi;
  wifi_auth_mode_t encription;
};

void clearEEPROM();
bool checkEEPROM();
void updateCRC();
conf getConfig(bool& isSet);
void debugSettings();
void getSSIDList(std::list<ssidInfo>* ssidList);
bool connect(const char* ssid ,const char* password);
