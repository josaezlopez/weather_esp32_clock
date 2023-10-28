#include "Adafruit_ILI9341Ext.h"
#include "icons.h"

#define HALT while(true) taskYIELD()

extern conf setting;
extern SemaphoreHandle_t xSemaphoreData;
extern SemaphoreHandle_t xSemaphoreTFT;
extern OpenWeatherMap* Location;
extern NTPClientExt* timeClient;



char text_amanecer[2][1][15] =  {"Amanecer","Sunrise "};
char text_anochecer[2][1][15] = {"Puesta  ","Sunset  "};
char text_fecha[2][1][15] = {"Fecha","Date"};
char text_temperatura[2][1][15] = {"Temperatura","Temperature"};
char text_maxima[2][1][15] = {"Maxima","Maxima"};
char text_stermica[2][1][20] = {"S. termica","Feels like"};
char text_minima[2][1][15] = {"Minima","Minimal"};
char text_presion[2][1][15] = {"Presion","Pressure"};
char text_humedad[2][1][15] = {"Humedad","Humidity"};
char text_unidades[2][1][15] ={"uds. en ug/m3","units in g/m3"};
char text_aire[2][1][15] ={"Aire","Air"};

// Task loop
void Adafruit_ILI9341Ext::loop(){
  suspend();
  while(WiFi.status()!=WL_CONNECTED){
    taskYIELD();
    }
  while(true){
    if(xSemaphoreTake(xSemaphoreTFT,(TickType_t) 1 ) == pdTRUE){ 
      printTime(50,115,ILI9341_MAGENTA,color565(0xD4,0xD1,0x2F),ILI9341_BLACK);
      printTemp(Location->getCurrentData(),135,0);
      xSemaphoreGive(xSemaphoreTFT);
      }
    delay(500);
    }
  }



void Adafruit_ILI9341Ext::clear(uint16_t backgroundColor=ILI9341_BLACK){
  fillScreen(backgroundColor);
}


void Adafruit_ILI9341Ext::begin(){
    Adafruit_ILI9341::begin();
    clear();
    isBegin=true;

}

// Vertical print
void Adafruit_ILI9341Ext::printV(const char *str,uint16_t color,uint16_t backgroundColor,uint16_t borderColor,bool force){
  static char name[50];

  int16_t x1,y1;
  uint16_t ancho, alto, cx=0, cy=0, textSize=3;
  
  if(strcmp(name,str) == 0 && !force){
    return;
  }
  
  if(strlen(str)>9) textSize = 2;
  fillRoundRect(0,0,35,320,5,backgroundColor);
  drawRoundRect(0,0,35,320,5,borderColor);
  cx+=6;
  cy+=10;

  setTextSize(textSize);
  setTextColor(color);

  for(int n=0;n < strlen(str) || n==9;n++){
    String car = String(str[n]);
    getTextBounds(car,cx,cy,&x1,&y1,&ancho,&alto);
    setCursor(cx+4,cy);
    print(str[n]);
    cy += alto+10;

  }

  strcpy(name,str);
}

// Data screen
void Adafruit_ILI9341Ext::printWeatherData(Weather* currentData,bool clear=true){

  int incremento = 20;
  uint16_t colorTit = ILI9341_BLUE;
  uint16_t colorDatos = ILI9341_GREEN;
  uint16_t backgroundColor = ILI9341_BLACK;

  while(xSemaphoreTake(xSemaphoreTFT,(TickType_t) 1) != pdTRUE){ 
    delay(1000);
    }

  if(clear){
    fillScreen(backgroundColor);
    setTextSize(2);
    printV(currentData->name,ILI9341_WHITE,ILI9341_DARKGREY,ILI9341_CYAN);
    }

  float temp = currentData->_weather_main.temp + ZeroAbs;
  if(setting.unitTemp[0]=='F'){
    temp = temp * (9/5) + 32;
  }

  setTextSize(2);
  setCursor(50,0);
  setTextColor(colorTit,backgroundColor);
  println(text_temperatura[0][getNumLang()]);



  setTextSize(3);
  setTextColor(colorDatos,backgroundColor);
  setCursor(50,incremento);
  printf(" % 2.1f %s",temp,setting.unitTemp);      
    
  // Max temperature
  temp = currentData->_weather_main.temp_max + ZeroAbs;
  if(setting.unitTemp[0]=='F'){
    temp = temp * (9/5) + 32;
  }
  
  setTextColor(colorTit,backgroundColor);
  setTextSize(2);
  setCursor(50,25+incremento);
  println(text_maxima[0][getNumLang()]);
  setTextSize(3);
  setTextColor(colorDatos,backgroundColor);
  setCursor(50,25+incremento*2);
  printf(" % 2.1f %s",temp,setting.unitTemp);      
  
  // Minimal temperature
  temp = currentData->_weather_main.temp_min + ZeroAbs;
  if(setting.unitTemp[0]=='F'){
    temp = temp * (9/5) + 32;
  }



  setTextColor(colorTit,backgroundColor);
  setTextSize(2);
  setCursor(50,30+incremento*3);
  println(text_minima[0][getNumLang()]);
  setTextSize(3);
  setCursor(50,50+incremento*3);
  setTextColor(colorDatos,backgroundColor);
  printf(" % 2.1f %s",temp,setting.unitTemp);      
  

  // It feels like
  temp = currentData->_weather_main.feels_like + ZeroAbs;
  if(setting.unitTemp[0]=='F'){
    temp = temp * (9/5) + 32;
  }

  setTextColor(colorTit,backgroundColor);
  setTextSize(2);
  setCursor(50,55+incremento*4);
  println(text_stermica[0][getNumLang()]);
  setTextSize(3);
  setCursor(50,75+incremento*4);
  setTextColor(colorDatos,backgroundColor);
  printf(" % 2.1f %s",temp,setting.unitTemp);      
  


  // Humedity
  setTextColor(colorTit,backgroundColor);
  setTextSize(2);
  setCursor(50,55+25+incremento*5);
  println(text_humedad[0][getNumLang()]);
  setTextSize(3);
  setCursor(50,100+incremento*5);
  setTextColor(colorDatos,backgroundColor);
  printf(" % 2d ",currentData->_weather_main.humidity);      
     

  // Pressure
  setTextColor(colorTit,backgroundColor);
  setTextSize(2);
  setCursor(50,55+25+25+incremento*6);
  println(text_presion[0][getNumLang()]);
  setTextSize(3);
  setCursor(50,125+incremento*6);
  setTextColor(colorDatos,backgroundColor);
  printf(" % 4d hPa",currentData->_weather_main.pressure);      

  setTextSize(1);
  setTextColor(ILI9341_CYAN,backgroundColor);
  setCursor(50,275);
  if(getNumLang()==0)
    printf("%s (%s) %2.1fm/s",currentData->_wind.directionOfWind,currentData->_wind.nameOfWind,currentData->_wind.speed);
  else
    printf("%s wind %2.1fm/s",currentData->_wind.directionOfWind,currentData->_wind.speed);
  

  setTextSize(2);
  setCursor(50,290);
  printFillRight(currentData->_weather.description,14,ILI9341_CYAN,ILI9341_BLACK);
  xSemaphoreGive(xSemaphoreTFT);

}

// Print clock
void Adafruit_ILI9341Ext::printTime(int x,int y,uint16_t dateColor,uint16_t clockColor,uint16_t backgroundColor){
 
  fillRoundRect(x,y,190,5,5,ILI9341_CYAN);
  
  setTextSize(2);
  setTextColor(dateColor,backgroundColor);
  setCursor(x+25,y+10);
 
  printf("%d %s %d",
        timeClient->getMDay(),
        timeClient->getNameOfMonth(timeClient->getMonth()-1),
        timeClient->getYear());
  // Hora 
  timeClient->getEpochTime();
  setTextSize(6);
  setCursor(x,y+33);
  setTextColor(clockColor,backgroundColor);
  printf("%02d:%02d",timeClient->getHours(),timeClient->getMinutes());
  setTextSize(3);
  setCursor(x+140,y+83);
  printf("%02d",timeClient->getSeconds());

  fillRoundRect(x,y+115,185,5,5,ILI9341_CYAN);

  }

// Wind
void Adafruit_ILI9341Ext::printWind(int x,int y,Weather* currentData,uint16_t textColor,uint16_t backgroundColor){
  char buf[25];

  setTextSize(2);
  setTextColor(textColor,backgroundColor);
  setCursor(x,y);
  if(!getNumLang())
    sprintf(buf,"%s %s",currentData->_wind.pcAbv,currentData->_wind.nameOfWind);
  else
    sprintf(buf,"%s wind",currentData->_wind.directionOfWind);
  printFillRight(buf,15,textColor,backgroundColor);
  setCursor(x,y+15);
  sprintf(buf,"%2.1f m/s",currentData->_wind.speed);
  printFillRight(buf,15,textColor,backgroundColor);

  }

// Sun 
void Adafruit_ILI9341Ext::printSun(int x,int y,Weather* currentData, uint16_t textColor,uint16_t dataColor,uint16_t backgroundColor){

  // SunRise
  setTextColor(ILI9341_WHITE,backgroundColor);
  setTextSize(2);
  setCursor(x,y);
  print(text_amanecer[0][getNumLang()]);
  setTextColor(dataColor,backgroundColor);
  struct tm* tmTemp;
  tmTemp = timeClient->epoch2tm(currentData->_sys.sunrise + currentData->_sys.timezone );
  printf(": %02d:%02d",tmTemp->tm_hour,tmTemp->tm_min);

  // Sunset
  setTextColor(ILI9341_WHITE,backgroundColor);
  setTextSize(2);
  setCursor(x,y+18);
  print(text_anochecer[0][getNumLang()]);
  setTextColor(dataColor,backgroundColor);
  tmTemp = timeClient->epoch2tm(currentData->_sys.sunset + currentData->_sys.timezone );
  printf(": %02d:%02d",tmTemp->tm_hour,tmTemp->tm_min);
}


// Main screen
void Adafruit_ILI9341Ext::printCurrent(Weather* currentData,bool clear,Adafruit_BME280* _bme280=nullptr){
  
  uint16_t backgroundColor = ILI9341_BLACK;
  bme280 = _bme280;

  if(xSemaphoreTake(xSemaphoreTFT,(TickType_t) 1)!=pdTRUE){ 
    return;
    }

  
  if(clear){ 
    fillScreen(ILI9341_BLACK);
    printV(currentData->name,ILI9341_WHITE,ILI9341_DARKGREY,ILI9341_CYAN,true);
    printIcon(currentData,35,0,true);
    }
  else{
    printV(currentData->name,ILI9341_WHITE,ILI9341_DARKGREY,ILI9341_CYAN);
    printIcon(currentData,35,0);
    }

  printWind(50,240,currentData,ILI9341_CYAN,ILI9341_BLACK);
  printSun(50,280,currentData,ILI9341_WHITE,ILI9341_GREEN,ILI9341_BLACK);
  xSemaphoreGive(xSemaphoreTFT);

  
}
// Print the temperature in current if bme280 != null the temperature is from bme280
void Adafruit_ILI9341Ext::printTemp(Weather* currentData,int x,int y){

  float temperature;
  int pressure;
  int humidity;
  char buffer[10];


  if(!bme280){
    temperature = currentData->_weather_main.temp + ZeroAbs;
    if(setting.unitTemp[0]=='F'){
      temperature = temperature * (9/5) + 32;
    }

    pressure = currentData->_weather_main.pressure;
    humidity = currentData->_weather_main.humidity;
    }
  else{
    temperature = bme280->readTemperature();
    if(setting.unitTemp[0]=='F'){
      temperature = temperature * (9/5) + 32;
    }
    pressure = bme280->readPressure()/100;
    humidity = bme280->readHumidity();
  }

  setCursor(x,y+10);
  setTextSize(1);
  setTextColor(ILI9341_RED,ILI9341_BLACK); 

  if(bme280)
    print("local");
   else
    print("     ");
   
  // Description
  setTextColor(ILI9341_CYAN,ILI9341_BLACK);
  setCursor(x-80,y+90);
  setTextSize(2);
  printFillRight(currentData->_weather.description,15,ILI9341_CYAN,ILI9341_BLACK);

  // Temperature
  setCursor(x,y+20);
  setTextSize(3);
  setTextColor(ILI9341_GREEN,ILI9341_BLACK);
  int t = round(temperature);
  sprintf(buffer,"%d %s ",t,setting.unitTemp);
  printFillRight(buffer,5,ILI9341_GREEN,ILI9341_BLACK); 

  // Pressure
  setCursor(x,y+45);
  setTextSize(2);
  setTextColor(ILI9341_GREEN,ILI9341_BLACK);
  sprintf(buffer,"%d hPa",pressure);
  printFillRight(buffer,8,ILI9341_GREEN,ILI9341_BLACK);
  
  
  // Humidity
  setCursor(x,y+60);
  setTextSize(2);
  sprintf(buffer,"Hum:%d%s",humidity,"%");
  printFillRight(buffer,8,ILI9341_GREEN,ILI9341_BLACK);

}

// Forecast screen
bool Adafruit_ILI9341Ext::printForecast(OpenWeatherMap* Location,bool clear,int dia){
  std::list<foreCast> *forecastlist = Location->getForecastList();

  char dayName[10];
  int nDias = 0;
  for (foreCast x : *forecastlist) {
    if(strcmp(dayName, x.dayName)!=0){
      nDias++;
      strcpy(dayName,x.dayName);
    }
	}
  
  if(dia<0 || dia>nDias-1) return false;
  int *forecastDay = new int[nDias];
  for(int n=0;n<nDias;n++) forecastDay[n] = 0;

  int n=0;
  strcpy(dayName,forecastlist->begin()->dayName);
  for (foreCast x : *forecastlist) { 
    if(strcmp(dayName, x.dayName)==0){
      forecastDay[n]++;
    }
    else{
      n++;
    }
    strcpy(dayName,x.dayName);
  }
  
  for(int n=0;n<nDias;n++){
    if(n!=0)
      forecastDay[n]++;
  }

  for(int n=0;n<nDias;n++){
    forecastDay[n]++;
  }

  int increase = 40;
  uint16_t dataColor = ILI9341_WHITE;
  uint16_t backgroundColor = ILI9341_BLACK;

  int saltar = 0;
  for(int n=0 ; n<dia; n++){
    saltar += forecastDay[n]-1;
  }


  auto it = forecastlist->begin();
  float temp;

  for(int n=0;n<saltar;n++){
    it++;
  }

  if(clear){
    fillScreen(backgroundColor);
    setTextSize(2);
    printV(it->dayName,ILI9341_WHITE,ILI9341_DARKGREY,ILI9341_CYAN);
    }

  for(int n=0;n<forecastDay[dia]-1 ; n++){
    setTextColor(dataColor,backgroundColor);
    setTextSize(2);
    setCursor(50,n*increase);
    setTextColor(ILI9341_GREEN,backgroundColor);
    temp = it->_SWeather_main.temp +ZeroAbs;
    if(setting.unitTemp[0]=='F'){
      temp = temp * (9/5) + 32;
    }

    printf("%s %0.f%s %0.f%s",it->hour,temp,setting.unitTemp,it->pop*100.0,"%");
    setCursor(50,n*increase+15);
    setTextColor(ILI9341_WHITE,backgroundColor);
    printUntil(it->_Sweather.description,15);
    drawFastHLine(50,n*increase+33,200,ILI9341_YELLOW);
    it++;
    }


  delete forecastDay;
  return true;

}

// Air quality display
void Adafruit_ILI9341Ext::printAirQuality(polucion* pollution,bool clear=true){

  uint16_t backgroundColor = ILI9341_BLACK;
  int yPos = 0;
  int increase = 20;

  if(xSemaphoreTake(xSemaphoreTFT,(TickType_t) 1)!=pdTRUE){ 
    return;
  }


  if(clear){
    fillScreen(backgroundColor);
    setTextSize(2);
    printV(text_aire[0][getNumLang()],ILI9341_WHITE,ILI9341_DARKGREY,ILI9341_CYAN);
    }

  
  // Air quality classification
  setTextSize(3);
  setTextColor(colorPollution(pollution->n_aqi),backgroundColor);
  setCursor(50,yPos);
  printf("%s", pollution->aqi);      
    
  // Carbon monoxide
  setTextColor(ILI9341_WHITE,backgroundColor);
  setTextSize(2);
  yPos+=50;
  setCursor(50,yPos);
  printf("CO:    %2.1f",pollution->co);


  // Nitrogen oxides
  setTextColor(ILI9341_WHITE,backgroundColor);
  yPos+=increase;
  setCursor(50,yPos);
  printf("NO:    %2.1f",pollution->no);

  // Nitrogen dioxide
  setTextColor(ILI9341_WHITE,backgroundColor);
  yPos+=increase;
  setCursor(50,yPos);
  printf("NO2:   %2.1f",pollution->no2);


  // Ozone
  yPos+=increase;
  setCursor(50,yPos);
  setTextColor(ILI9341_WHITE,backgroundColor);
  printf("O3:    %2.1f",pollution->o3);      
     
  // Ammonia
  yPos+=increase;
  setCursor(50,yPos);
  setTextColor(ILI9341_WHITE,backgroundColor);
  printf("NH3:   %2.1f",pollution->nh3);      

  // Sulfur dioxide
  yPos+=increase;
  setCursor(50,yPos);
  setTextColor(ILI9341_WHITE,backgroundColor);
  printf("SO2:   %2.1f",pollution->so2); 

  // Particles diameter 2.5uM
  yPos+=increase;
  setCursor(50,yPos);
  setTextColor(ILI9341_WHITE,backgroundColor);
  printf("PM2-5: %2.1f",pollution->pm2_5); 

  // Particle diameter 10uM
  yPos+=increase;
  setCursor(50,yPos);
  setTextColor(ILI9341_WHITE,backgroundColor);
  printf("PM10:  %2.1f",pollution->pm10); 

  // Units
  yPos+=increase*2;
  setCursor(50,yPos);
  setTextColor(ILI9341_WHITE,backgroundColor);
  printf(text_unidades[0][getNumLang()]); 
  xSemaphoreGive(xSemaphoreTFT);
}

uint16_t Adafruit_ILI9341Ext::colorPollution(int aqi){
  log_d("----------------------_> aqi:%d\r\n",aqi);
  switch(aqi){
    case 0:
      return ILI9341_CYAN;
    case 1:
      return ILI9341_BLUE;
    case 2:
      return ILI9341_GREEN;
    case 3:
      return ILI9341_YELLOW;
    case 4:
      return ILI9341_RED;
    default:
      return ILI9341_WHITE;

  }
}

// Get numerical languge
int Adafruit_ILI9341Ext::getNumLang(){
  if(strcmp(setting.lang,"ES")==0)
    return 0;
  else
    return 1;
}


// Print icon
void Adafruit_ILI9341Ext::printIcon(Weather* currentData,int _x,int _y,bool force){
  byte pixel[4];
  char* icon;
  char* iconWeather = currentData->_weather.icon;
  if(strcmp(lastIcon,iconWeather)==0 && !force){
    return;
    } 

  while(xSemaphoreTake(xSemaphoreData,(TickType_t) 1)!=pdTRUE){ 
    delay(100);
    }

  if(strcmp(iconWeather,"01d")==0)         icon = icon_01d;
  else if(strcmp(iconWeather,"01n")==0)    icon = icon_01n;
  else if(strcmp(iconWeather,"02d")==0)    icon = icon_02d;
  else if(strcmp(iconWeather,"02n")==0)    icon = icon_02n;
  else if(strcmp(iconWeather,"03d")==0)    icon = icon_03d;
  else if(strcmp(iconWeather,"03n")==0)    icon = icon_03n;
  else if(strcmp(iconWeather,"04d")==0)    icon = icon_04d;
  else if(strcmp(iconWeather,"04n")==0)    icon = icon_04n;
  else if(strcmp(iconWeather,"09d")==0)    icon = icon_09d;
  else if(strcmp(iconWeather,"09n")==0)    icon = icon_09n;
  else if(strcmp(iconWeather,"10d")==0)    icon = icon_10d;
  else if(strcmp(iconWeather,"10n")==0)    icon = icon_10n;
  else if(strcmp(iconWeather,"11d")==0)    icon = icon_11d;
  else if(strcmp(iconWeather,"11n")==0)    icon = icon_11n;
  else if(strcmp(iconWeather,"13d")==0)    icon = icon_13d;
  else if(strcmp(iconWeather,"13n")==0)    icon = icon_13n;
  else if(strcmp(iconWeather,"50d")==0)    icon = icon_50d;
  else if(strcmp(iconWeather,"50n")==0)    icon = icon_50n;

  for(int y=_y; y < 100 +_y  ; y++){
    for(int x=_x; x < 100 + _x ; x++){
      HEADER_PIXEL(icon,pixel);
      drawPixel(x,y,color565(pixel[0],pixel[1],pixel[2]));
    }
  }

  strcpy(lastIcon,iconWeather);
  log_d("The icon is redrawn\r\n");

  xSemaphoreGive(xSemaphoreData);

}

// Print with spaces on the right  
void Adafruit_ILI9341Ext::printFillRight(char* cad,int maxcar,uint16_t textColor,uint16_t bgColor){

  int lt = strlen(cad);
  setTextColor(textcolor,bgColor);
  print(cad);
  for(int n=lt;n<maxcar;n++){
    print(" ");
  }

}

// Print the first nCars from cad.
void Adafruit_ILI9341Ext::printUntil(char* cad,int nCars){
  if(strlen(cad)>nCars){
    for(int n=0 ; n<nCars ; n++){
      print(cad[n]);

    }
  }
  else
    print(cad);
}

