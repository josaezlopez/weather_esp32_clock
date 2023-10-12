#pragma once

#include <Arduino.h>
#include <threadesp32.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <NTPClientExt.hpp>
#include <list>

#define WEATHERTASK_NAME "WEATHER"
#define WEATHERTASK_HEAP (9*1024)
#define WEATHERTASK_PRIORITY    TaskParent::th_LOWEST_PRIORITY
#define WEATHERTASK_CORE    0


struct weatherId{
    char main[50];
    char descripcion[20];
};

const float ZeroAbs = -273.15;


#define UrlCurrentData  "http://api.openweathermap.org/data/2.5/weather?lat=%f&lon=%f&appid=%s&lang=%s"
#define UrlForecastData "https://api.openweathermap.org/data/2.5/forecast?lat=%f&lon=%f&appid=%s&lang=%s"
#define UrlPollution    "http://api.openweathermap.org/data/2.5/air_pollution?lat=%f&lon=%f&appid=%s"

struct coord{
   double lat;
   double lon;
};

// Data structure in EEPROM
struct conf{
    coord home;
    char lang[3];
    int timeZone;
    char unitTemp[2];
    int dayligthSaving;
    int monthStartDS;
    int monthEndDS;
    char ssid[33];
    char password[65];
    char owm_apikey[65];
    int  owm_tupdate;
};

struct weather{
    int id;
    char main[20];
    char description[20];
    char icon[4];

};

struct weather_main{
    float temp;
    float feels_like;
    float temp_min;
    float temp_max;
    int pressure;
    int humidity;
};

struct wind{
    float speed;
    int deg;
    float gust;
    char dirViento[12];
    char NombreViento[12];
    char pcAbv[3];
};

struct clouds{
    int all;
};

struct sys{
    int type;
    unsigned long id;
    char country[4];
    uint32_t sunrise;
    uint32_t sunset;
    int32_t timezone;
};


// Structure for currentadata
struct Weather{
    coord _coord;
    weather _weather;
    wind _wind;
    weather_main _weather_main;
    clouds _clouds;    
    sys _sys;
    unsigned long dt;
    char _base[10];
    int visibility;
    unsigned long id;
    char name[50];
    int cod;
    char errMsg[101];
};

//Structure for forecast
struct foreCast{
    unsigned long dt;
    weather_main _SWeather_main;
    weather _Sweather;
    wind _Swind;
    clouds _Sclouds;
    char pod[3];
    char dt_txt[20];
    char nombreDia[10];
    char hora[6];
};

// Structure for pollution
struct polucion{
    coord _coord;
    int n_aqi;
    char aqi[15];
    float co;
    float no;
    float no2;
    float o3;
    float so2;
    float pm2_5;
    float pm10;
    float nh3;
    uint32_t dt;
};

// Class task 
class OpenWeatherMap : public  TaskParent{
   public:
        OpenWeatherMap();
        bool isValidData();
        Weather* getCurrentData();
        polucion* getPollution();
        std::list<foreCast>* getForecastList();
        void forceUpdate();

   protected:
      void loop();
      void update();
      bool dlCurrentData();
      bool dlForecastData();
      bool dlPollutionData();
      void getNameWind(float deg,wind* wind);

      char urlBuffer[256];
      Weather currentData;
      std::list<foreCast> foreCastList;
      polucion pollution;

      bool updatingCurrentCast = false;
      bool updatingForeCast = false;
      bool updatingPollutionData = false;
      bool validDataCurrent = false;
      bool validDataForeCast = false;
      bool validDataPollution = false;
      bool allUpdated = false;
      bool updating = false;
      bool flagForceUpdate = false;
      uint32_t lastUpdate;

      const char pcAbv[8][3] = {"N ","NE","E ","SE","S ","SO","O ","NO"};
      const char dirViento[2][8][10] = {"Norte","Noreste","Este","Sudeste","Sur","Sudoeste","Oeste","Noroeste",
                                        "North","Northeast","East","Southeast","South","Southwest","West","Northwest"};
      const char nombreViento[8][15] = {    "Tramontana ","Gregal     ","Levante    ","Siroco     ",
                                            "Ostro      ","Garbino    ","Poniente   ","Mistral    "};
      const char aqiPollution[2][5][20] = {"Buena","Suficiente","Moderada","Mala","Muy mala",
                                           "Good","Fair","Moderate","Poor","Very poor"};

};



