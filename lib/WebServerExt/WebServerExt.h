#pragma once

#include <WebServer.h>
#include <threadesp32.h>

#include "Weather.h"

#define WEBTASK_NAME        "WEBSERVER"
#define WEBTASK_HEAP        (5 * 1024)
#define WEBTASK_PRIORITY    TaskParent::th_LOWEST_PRIORITY  
#define WEBTASK_CORE        1

String selectUnitTemperature(char* value);
String selectLanguage(char* value);
String selectMonth(const char* name,int value);
String selectDay(bool value);
String optionSelectTimezone(int value,int tz);
String selectTimezone(int value);
void update();
void handleRoot();


class WebServerExt : public WebServer, public TaskParent{
    public:
        WebServerExt(int port);
    protected:
        void loop();
    
};

