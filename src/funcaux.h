#pragma once

#include <Arduino.h>
#include <EEPROM.h>
#include <CRC32.h>
#include "Weather.h"
#include "conf.h"

void clearEEPROM();
bool checkEEPROM();
void updateCRC();
conf getConfig();
void debugSettings();
