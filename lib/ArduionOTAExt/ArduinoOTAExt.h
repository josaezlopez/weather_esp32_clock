#pragma once

#include <ArduinoOTA.h>
#include <threadesp32.h>
#include <esp_task_wdt.h>
#include <ESPmDNS.h>

#include "Adafruit_ILI9341Ext.h"

#define OTATASK_NAME        "OTAEXT"
#define OTATASK_HEAP        (4* 1024)
#define OTATASK_PRIORITY    TaskParent::th_LOWEST_PRIORITY
#define OTATASK_CORE        1

class ArduinoOTAExt :  public TaskParent{

    public:
        ArduinoOTAExt(Adafruit_ILI9341Ext& _tft,const char* _password,const char* __name);
        void loop();
        Adafruit_ILI9341Ext* tft;
        void setUpload() { upload = true;} 
        bool getUpload() { return upload;}       
    protected:
        void initOTA();
        bool upload = false;
        char password[11];
        char name[15];

};


