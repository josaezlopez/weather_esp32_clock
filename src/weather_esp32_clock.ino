#include <Arduino.h>
#include <WiFi.h>
#include <Adafruit_BME280.h>
#include <NTPClientExt.hpp>

#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

#include "Adafruit_ILI9341Ext.h"
#include "ArduinoOTAExt.h"
#include "touchEsp32.h"
#include "Weather.h"
#include "WebServerExt.h"
#include "funcaux.h"
#include "conf.h"

#include <list>




#define HALT while(true) taskYIELD()
#define ST(A) #A
#define STR(A) ST(A)



#if ENABLE_BME280
  Adafruit_BME280* bme280;
#else
  Adafruit_BME280* bme280 = nullptr;
#endif


SemaphoreHandle_t semaphoreData = NULL;
SemaphoreHandle_t semaphoreTFT = NULL;
WebServerExt* httpServer;
NTPClientExt* timeClient;
Adafruit_ILI9341Ext tft = Adafruit_ILI9341Ext();  // Start screen task
OpenWeatherMap* Location;
touchEsp32 button(BUTTON_PIN);                    // Start button task 
ArduinoOTAExt* OTAExt;

conf setting;
std::list<ssidInfo> SSIDList;



void setup() {
  bool isSet;
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
  Serial.begin(115200);
  setting = getConfig(isSet);
  semaphoreData = xSemaphoreCreateMutex();
  semaphoreTFT = xSemaphoreCreateMutex();


  #if ENABLE_BME280
   bme280 = new Adafruit_BME280;
   bme280->begin(BME280ADDRESS);  
  #endif

  tft.begin();

  if(!connect(setting.ssid,setting.password)){
        getSSIDList(&SSIDList);
        // Change to AP mode
        WiFi.mode(WIFI_AP);
        WiFi.softAP(STR(mDNSName),NULL,1,0);
        WiFi.begin();
        tft.println("");
        tft.print("Cannot connect to ");
        tft.println(setting.ssid);
        tft.print("Associate to SSID ");
        tft.println(WiFi.softAPSSID());
        tft.println("and then, from the browser, open ");
        tft.print("http://");
        tft.print(WiFi.softAPIP());
        tft.println(" to configure the WIFI.");
        HALT;
    }

   OTAExt = new ArduinoOTAExt(tft,PASSOTA,STR(mDNSName));  // Start OTA and  MDNS task


  tft.printf("ok\r\n");
  tft.setTextSize(1);
  tft.printf("http://%s.local\r\n", STR(mDNSName));
  tft.println("\r\nOR\r\n");
  tft.print("\r\nhttp://");
  tft.println(WiFi.localIP());
  
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 1);
  // Start WEB server task
  httpServer = new WebServerExt(WEBSERVER_PORT);
  
  // Start the NTP client task
  timeClient = new NTPClientExt(NTPSERVER,setting.timeZone,UPDATENTP,setting.dayligthSaving);
  timeClient->setLang(setting.lang);
  debugSettings();
  // Start the OpenWeatherMap task
  Location = new OpenWeatherMap();

  }


void loop(void) {
  static uint32_t pressDuration = 0;
  Weather* currentData;
  uint32_t currentDatadt;
  int nPrev;  
  // Wait for data 
  
  while(Location->getCurrentDataResult()<0 || 
        Location->getAirPollutionDataResult()<0 ||  
        Location->getForecastDataResult()<0){
    tft.printf("Update current data.\r\n");
    delay(1000);
    }


  currentData = Location->getCurrentData();
  currentDatadt = currentData->dt;


  while(true){
    if(Location->getCurrentDataResult()==200)
      tft.printCurrent(currentData,true,nullptr);  
    tft.resume();                                 

    log_d("Current data screen.\r\n");
    while(xQueueReceive(button.getQueue(),&pressDuration,1) != pdPASS){    
      if(currentDatadt != currentData->dt){
        if(Location->getCurrentDataResult()==200)
          tft.printCurrent(currentData,true,nullptr);   

        currentDatadt = currentData->dt;
        }
      vPortYield();
      }


    #if ENABLE_BME280
      log_d("Current data and local temperature display\r\n");
      tft.printCurrent(currentData,false,bme280);    
      while(xQueueReceive(button.getQueue(),&pressDuration,1) != pdPASS){    
        if(currentDatadt != currentData->dt){
          tft.printCurrent(currentData,false,bme280);
          currentDatadt = currentData->dt;
          }
        vPortYield();
      }
    #endif

    log_d("Air quality display");
    // It cannot be suspended if the traffic light is taken
    while(xSemaphoreTake(semaphoreTFT,(TickType_t) 1) != pdTRUE){ vPortYield(); } 
    tft.suspend();
    xSemaphoreGive(semaphoreTFT);
    if(Location->getAirPollutionDataResult()==200)
      tft.printAirQuality(Location->getPollution(),true);     //sCREEN2
    while(xQueueReceive(button.getQueue(),&pressDuration,1) != pdPASS){    // while not push button
        if(currentDatadt != currentData->dt){
          if(Location->getAirPollutionDataResult()==200)
            tft.printAirQuality(Location->getPollution(),true);    // Print Screen2
          currentDatadt = currentData->dt;
          }
        vPortYield();
        }

    log_d("Current data screen, detail\r\n");
    
    tft.suspend();
    if(Location->getCurrentDataResult()==200)
      tft.printWeatherData(Location->getCurrentData(),true);    //sCREEN3
    while(xQueueReceive(button.getQueue(),&pressDuration,1) != pdPASS){    // while not push button
        if(currentDatadt != currentData->dt){
          if(Location->getCurrentDataResult()==200)
            tft.printWeatherData(Location->getCurrentData(),true);    // Print Screen2
          currentDatadt = currentData->dt;
          }
        vPortYield();
        }

      if(pressDuration > 500){
        nPrev = 0;
        while(true){
            if(Location->getForecastDataResult()==200){
              if(!tft.printForecast(Location,true,nPrev++)){
                nPrev = 0;
                break;
                }
              log_d("Forecast %d\r\n",nPrev);
              while(xQueueReceive(button.getQueue(),&pressDuration,1) != pdTRUE)  vPortYield();
            }
            else{
              break;
              }
          }
        }
    }
 
}

