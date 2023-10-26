#pragma once

#include <Arduino.h>
#include <threadesp32.h>
#include <NTPClientExt.hpp>
#include <Adafruit_ILI9341.h>
#include <Weather.h>
#include <Adafruit_BME280.h>
#include <list>


#define CLOCKTASK_NAME "WCLOCK"
#define CLOCKTASK_HEAP (5*1024)
#define CLOCKTASK_PRIORITY    TaskParent::th_LOWEST_PRIORITY
#define CLOCKTASK_CORE    0


#define STMPE_CS 32
#define TFT_CS   5
#define TFT_DC   2
#define SD_CS    14

class Adafruit_ILI9341Ext : public Adafruit_ILI9341, public TaskParent {
    public:
        Adafruit_ILI9341Ext():Adafruit_ILI9341(TFT_CS,TFT_DC),TaskParent(CLOCKTASK_NAME,CLOCKTASK_HEAP,CLOCKTASK_PRIORITY,CLOCKTASK_CORE)
        {
        }
        void printCurrent(Weather* CurrentData,bool clear,Adafruit_BME280* bme280);
        void printAirQuality(polucion* pollution,bool clear);
        void printWeatherData(Weather* currentData,bool clear);
        bool printForecast(OpenWeatherMap* Location,bool clear,int dia);
        void begin();
        void clear(uint16_t backgroundColor);
        void printIconPba(char* icono);



    protected:
        void printUntil(char* cad,int nCars);
        void printV(const char *str,uint16_t color,uint16_t backgroundColor,uint16_t colorBorde,bool force = false);
        void printTime(int x,int y,uint16_t colorFecha,uint16_t colorReloj,uint16_t backgroundColor);
        void printWind(int x,int y,Weather* currentData,uint16_t textColor,uint16_t backgroundColor);
        void printSun(int x,int y,Weather* currentData,uint16_t textColor,uint16_t colorDatos,uint16_t backgroundColor);
        void printTemp(Weather* currentData,int  x,int y);
        void printIcon(Weather* currentData,int x,int y,bool force = false);
        void loop();
        void printFillRight(char* cad,int maxcar,uint16_t textColor = ILI9341_WHITE,uint16_t bgColor=ILI9341_BLACK);
        int getNumLang();

        Adafruit_BME280* bme280 = nullptr;
        char lastIcon[5];
        uint16_t colorPollution(int aqi);
        bool isBegin = false;

};