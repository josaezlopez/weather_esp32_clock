#include <Arduino.h>
#include <WiFi.h>
#include <Adafruit_ILI9341Ext.h>
#include <ArduinoOTAExt.h>
#include <Adafruit_BME280.h>
#include <touchEsp32.h>
#include <Weather.h>
#include <WebServerExt.h>
#include <NTPClientExt.hpp>
#include <funcaux.h>
#include <conf.h>

#define HALT while(true) taskYIELD()
#define ST(A) #A
#define STR(A) ST(A)



#if ENABLE_BME280
  Adafruit_BME280* bme280;
#else
  Adafruit_BME280* bme280 = nullptr;
#endif


SemaphoreHandle_t xSemaphoreData = NULL;
SemaphoreHandle_t xSemaphoreTFT = NULL;
WebServerExt* httpServer;
NTPClientExt* timeClient;
Adafruit_ILI9341Ext tft = Adafruit_ILI9341Ext();  // Start screen task
OpenWeatherMap* Location;
touchEsp32 button(BUTTON_PIN);                    // Start button task
ArduinoOTAExt OTAExt(tft,PASSOTA,STR(mDNSName));  // Start OTA and  MDNS task
conf setting;



void setup() {
  Serial.begin(115200);

  bool connected = true;
  xSemaphoreData = xSemaphoreCreateMutex();
  xSemaphoreTFT = xSemaphoreCreateMutex();


  #if ENABLE_BME280
   bme280 = new Adafruit_BME280;
   bme280->begin(BME280ADDRESS);  
  #endif

  setting = getConfig();
  tft.begin();

  WiFi.begin(setting.ssid,setting.password);
  tft.print("Connecting");
  int contadorConexion = 0;
  while(WiFi.status() != WL_CONNECTED) {
    delay(1000);
    tft.print(".");
    contadorConexion++;
    if(contadorConexion > 30){
      connected = false;
      break;
      }
    }

  if(!connected){
    tft.printf("\r\nNo connection to %s\r\n",setting.ssid);
    tft.printf("\r\nErase flash memory and recompile\r\n");
    HALT;
    }


  tft.printf("ok\r\n");
  tft.setTextSize(1);
  tft.printf("http://%s.local\r\n", STR(mDNSName));
  tft.println("\r\nOR\r\n");
  tft.print("\r\nhttp://");
  tft.println(WiFi.localIP());
  
  
  // Start WEB server task
  httpServer = new WebServerExt(80);
  
  // Start the NTP client task
  timeClient = new NTPClientExt(NTPSERVER,setting.timeZone,UPDATENTP,setting.dayligthSaving);
  timeClient->setLang(setting.lang);
  debugSettings();
  // Start the OpenWeatherMap task
  Location = new OpenWeatherMap();
  }

void loop(void) {
  static uint32_t durPul=0;
  Weather* currentData;
  uint32_t currentDatadt;
  int nPrev;  

  // Wait for data 
  for(;!Location->isValidData();){
    tft.printf("Update data...\r\n");
    delay(1000);
    }
  currentData = Location->getCurrentData();
  currentDatadt = currentData->dt;


  while(true){
    tft.printCurrent(currentData,true,nullptr);  
    tft.resume();                                 

    log_d("Current data screen.\r\n");
    while(xQueueReceive(button.getQueue(),&durPul,1) != pdPASS){    
      if(currentDatadt != currentData->dt){
        tft.printCurrent(currentData,true,nullptr);   
        Serial.printf("Actualizando datos");
        currentDatadt = currentData->dt;
        }
      vPortYield();
      }


    #if ENABLE_BME280
      log_d("Current data and local temperature display\r\n");
      tft.printCurrent(currentData,false,bme280);    
      while(xQueueReceive(button.getQueue(),&durPul,1) != pdPASS){    
        if(currentDatadt != currentData->dt){
          tft.printCurrent(currentData,false,bme280);
          currentDatadt = currentData->dt;
          }
        vPortYield();
      }
    #endif

    log_d("Air quality display");
    // It cannot be suspended if the traffic light is taken
    while(xSemaphoreTake(xSemaphoreTFT,(TickType_t) 1) != pdTRUE){ vPortYield(); } 
    tft.suspend();
    xSemaphoreGive(xSemaphoreTFT);
    
    tft.printAirQuality(Location->getPollution(),true);     //sCREEN2
    while(xQueueReceive(button.getQueue(),&durPul,1) != pdPASS){    // while not push button
        if(currentDatadt != currentData->dt){
          tft.printAirQuality(Location->getPollution(),true);    // Print Screen2
          currentDatadt = currentData->dt;
          }
        vPortYield();
        }

    log_d("Current data screen, detail\r\n");
    
    tft.suspend();
    tft.printWeatherData(Location->getCurrentData(),true);    //sCREEN3
    while(xQueueReceive(button.getQueue(),&durPul,1) != pdPASS){    // while not push button
        if(currentDatadt != currentData->dt){
          tft.printWeatherData(Location->getCurrentData(),true);    // Print Screen2
          currentDatadt = currentData->dt;
          }
        vPortYield();
        }

      if(durPul>500){
        nPrev = 0;
        while(true){
            if(!tft.printForecast(Location,true,nPrev++)){
              nPrev = 0;
              break;
              }
            log_d("Forecast %d\r\n",nPrev);
            while(xQueueReceive(button.getQueue(),&durPul,1) != pdTRUE)  vPortYield();
          }
        }
    }
 
}

