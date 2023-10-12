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
  bool connected = true;
  xSemaphoreData = xSemaphoreCreateMutex();
  xSemaphoreTFT = xSemaphoreCreateMutex();

  Serial.begin(115200);

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
  static uint32_t durPul;
  int nPrev;    

  // Wait for data 
  for(;!Location->isValidData();){
    taskYIELD();
    }
  
  
  
  while(true){
    tft.resume();
    if(!OTAExt.getUpload()){ 
      tft.printCurrent(Location->getCurrentData(),true,nullptr);
      }
    tft.resume();
    while(!button.getPulsacion(&durPul)){
      if(!OTAExt.getUpload() && Location->isValidData()){ 
        tft.printCurrent(Location->getCurrentData(),false,nullptr);
        }
      }

    #if ENABLE_BME280      
    while(!button.getPulsacion(&durPul)){
      if(!OTAExt.getUpload() && Location->isValidData()){ 
        tft.printCurrent(Location->getCurrentData(),false,bme280);
        }
      }
    #endif
    delay(100);
    tft.suspend();
    if(!OTAExt.getUpload() && Location->isValidData()) tft.printAirQuality(Location->getPollution(),true);
    while(!button.getPulsacion(&durPul)){
      if(!OTAExt.getUpload() && Location->isValidData()) tft.printAirQuality(Location->getPollution(),false);
      }

    if(!OTAExt.getUpload()) tft.printWeatherData(Location->getCurrentData(),true);
    while(!button.getPulsacion(&durPul)){
      if(Location->isValidData()){
        if(!OTAExt.getUpload()) tft.printWeatherData(Location->getCurrentData(),false);
        }
      }
    if(durPul>500){
      nPrev = 0;
      while(true){
        if(!OTAExt.getUpload()) 
          if(!tft.printForecast(Location,true,nPrev++)){
            nPrev = 0;
            break;
            }
        while(!button.getPulsacion());
        }

      }
  esp_task_wdt_reset();
  }
   
}

