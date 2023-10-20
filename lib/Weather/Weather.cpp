#include <Weather.h>

extern conf setting;
extern NTPClientExt* timeClient;
extern SemaphoreHandle_t xSemaphoreData;

// Constructor
OpenWeatherMap::OpenWeatherMap(): 
    TaskParent(WEATHERTASK_NAME,WEATHERTASK_HEAP, WEATHERTASK_PRIORITY, WEATHERTASK_CORE){
}

// true if the data is valid
bool OpenWeatherMap::isValidData(){
    
    return !updatingCurrentCast && !updatingForeCast && !updatingPollutionData &&
            validDataCurrent && validDataForeCast && validDataPollution;
}

// Returns the structure with the current data
Weather* OpenWeatherMap::getCurrentData(){
    return &(this->currentData);
}

// Returns the structure with the pollution data
polucion* OpenWeatherMap::getPollution(){
    return &(this->pollution);
}

// Return the list of forecast data
std::list<foreCast>* OpenWeatherMap::getForecastList(){ 
    return &foreCastList; 
    }

void OpenWeatherMap::forceUpdate(){
    flagForceUpdate = true; 
    }


// Loop task 
void OpenWeatherMap::loop(){
    uint32_t ultActualizacion;
    while(WiFi.status()!=WL_CONNECTED){
        taskYIELD();
        }
    update();
    flagForceUpdate = false;
    lastUpdate = millis();
    while(true){
        if( (millis() - lastUpdate > (setting.owm_tupdate * 60 * 1000)) || flagForceUpdate){
            log_d("Updating.\r\n");
            update();
            log_d("Updated.\r\n");
            flagForceUpdate = false;
            lastUpdate = millis();
            }
        delay(500);
        }
    }

/// Update all data
void OpenWeatherMap::update(){
    // Wait for semaphoreData
    while( xSemaphoreTake( xSemaphoreData, ( TickType_t ) 500 ) != pdTRUE ){
         delay(1); 
         };
    bool errCD = dlCurrentData();
    log_d("Updating current data\r\n");
    bool errPD = dlPollutionData();
    log_d("Updating polucion data\r\n");
    bool errFD = dlForecastData();
    log_d("Updating forecast data\r\n");
    flagForceUpdate = false;
    allUpdated = errCD && errPD && errFD;
    xSemaphoreGive( xSemaphoreData );
}

// Download the current data
bool OpenWeatherMap::dlCurrentData(){
    HTTPClient http;
    DynamicJsonDocument doc(1100);

    validDataCurrent = false;
    updating = updatingCurrentCast = true;

    sprintf(urlBuffer,UrlCurrentData,setting.home.lat,setting.home.lon,setting.owm_apikey,setting.lang);
    http.begin(urlBuffer);
    int httpResponseCode = http.GET();
    
    if (httpResponseCode>0) {
        String payLoad = http.getString();
        DeserializationError err = deserializeJson(doc, payLoad);
        if (err) {
            log_e("deserializeJson() failed with code %s\r\n",err.c_str());
            validDataCurrent = false;
            updating = updatingCurrentCast = false;
            return false;
            }
        currentData.cod = doc["cod"];
        if(currentData.cod!=200){
            strncpy(currentData.errMsg,doc["message"],sizeof(currentData.errMsg)-1);
            log_d("Cod %d: %s\r\n",currentData.cod, currentData.errMsg);
            validDataCurrent = false;
            updating = updatingCurrentCast = false;
            return true;
        }
        //Coord
        currentData._coord.lon = doc["coord"]["lon"];  
        currentData._coord.lat = doc["coord"]["lat"]; 
        // Weather
        JsonObject weather = doc["weather"][0];
        currentData._weather.id = weather["id"];
        strcpy(currentData._weather.main , weather["main"]);
        strcpy(currentData._weather.description ,  weather["description"]);
        currentData._weather.description[0] = toupper(currentData._weather.description[0]);
        strcpy(currentData._weather.icon,  weather["icon"]);
        //base
        strcpy(currentData._base , doc["base"]);
        //main
        JsonObject main = doc["main"];
        currentData._weather_main.temp = main["temp"];
        currentData._weather_main.feels_like = main["feels_like"];
        currentData._weather_main.temp_min =  main["temp_min"];
        currentData._weather_main.temp_max = main["temp_max"];
        currentData._weather_main.pressure =  main["pressure"];
        currentData._weather_main.humidity  = main["humidity"];
        //visibility
        currentData.visibility = doc["visibility"];
        // Wind
        currentData._wind.speed = doc["wind"]["speed"];
        currentData._wind.deg = doc["wind"]["deg"];
        getNameWind((float)doc["wind"]["deg"],&currentData._wind);
        //Clouds
        currentData._clouds.all = doc["clouds"]["all"];
        //dt
        currentData.dt = doc["dt"];
        //sys
        JsonObject sys = doc["sys"];
        currentData._sys.type = sys["type"];
        currentData._sys.id = sys["id"];
        strcpy(currentData._sys.country , sys["country"]);
        currentData._sys.sunrise = sys["sunrise"];
        currentData._sys.sunset = sys["sunset"];
        currentData._sys.timezone = doc["timezone"];
        currentData.id = doc["id"];
        strcpy(currentData.name , doc["name"]);
        currentData.cod = doc["cod"];
        }
    else{
        validDataCurrent = false;
        updating = updatingCurrentCast = false;
        return false;
        }
    http.end();
    validDataCurrent = true;
    updating = updatingCurrentCast = false;
    return true;
}



// Download forecasta data
bool OpenWeatherMap::dlForecastData(){

    if(currentData.cod!=200){
        log_d("Cod %d: %s\r\n",currentData.cod, currentData.errMsg);
        validDataForeCast = false;
        updating = updatingForeCast = false;
        return true;
    }

    HTTPClient http;
    DynamicJsonDocument doc(24576);
    String payLoad;

    validDataForeCast = false;
    updating = updatingForeCast = true;
    
    sprintf(urlBuffer,UrlForecastData,setting.home.lat,setting.home.lon,setting.owm_apikey,setting.lang);
    http.begin(urlBuffer);
    int httpResponseCode = http.GET();
    
    if (httpResponseCode>0) {
        payLoad = http.getString();
        }
    DeserializationError err = deserializeJson(doc, payLoad);
    if (err) {
        log_e("deserializeJson() failed with code %s\r\n",err.c_str());
        validDataForeCast = false;
        updating = updatingForeCast = false;
        return false;
        }

    foreCastList.clear();
    foreCast item;
    weatherId weatherMain;
    struct tm tm;

    for (JsonObject list_item : doc["list"].as<JsonArray>()) {
        
        // main
        JsonObject list_item_main = list_item["main"];

        item._SWeather_main.temp = list_item_main["temp"];
        item._SWeather_main.feels_like = list_item_main["feels_like"]; 
        item._SWeather_main.temp_min = list_item_main["temp_min"]; 
        item._SWeather_main.temp_max = list_item_main["temp_max"]; 
        item._SWeather_main.pressure = list_item_main["pressure"]; 
        item._SWeather_main.sea_level = list_item_main["sea_level"];
        item._SWeather_main.ground_level = list_item_main["ground_level"];
        item._SWeather_main.humidity = list_item_main["humidity"]; 
        item._SWeather_main.temp_kf = list_item_main["temp_kf"]; 

        // weather
        JsonObject list_item_weather_0 = list_item["weather"][0];
        item._Sweather.id = list_item_weather_0["id"]; 
        strcpy(item._Sweather.main,list_item_weather_0["main"]);
        strcpy(item._Sweather.description,list_item_weather_0["description"]);
        item._Sweather.description[0] = toupper(item._Sweather.description[0]);
        strcpy(item._Sweather.icon,list_item_weather_0["icon"]);
        
        // clouds
        item._Sclouds.all =  list_item["clouds"]["all"]; 

        // wind
        JsonObject list_item_wind = list_item["wind"];
        item._Swind.speed = list_item_wind["speed"]; 
        item._Swind.deg = list_item_wind["deg"]; 
        item._Swind.gust = list_item_wind["gust"]; 
        getNameWind(item._Swind.deg,&item._Swind);
        item._Swind.visibility = list_item_wind["visibility"];
        item.pop = list_item["pop"];
        
        strcpy(item.pod, list_item["sys"]["pod"]); 
        strcpy(item.dt_txt, list_item["dt_txt"]); 
        // Nombre del dia
        strptime(item.dt_txt,"%Y-%m-%d %H:%M:%S", &tm);
        strcpy(item.nombreDia,timeClient->getNameOfDay(tm.tm_wday));  
        /* Solo la hora*/ 

        String sdt_txt = String(item.dt_txt);
        strcpy(item.hora,sdt_txt.substring(11,16).c_str());  
    
        foreCastList.push_back(item);
        }

    if(foreCastList.size()==0){
        validDataForeCast = false;
        updating = updatingForeCast = false;
        return false;
    }
    // city
    JsonObject city = doc["city"];

    cityData.id = city["id"]; 
    strcpy(cityData.name,city["name"]); 
    cityData.coords.lat = city["coord"]["lat"];
    cityData.coords.lon = city["coord"]["lon"];
    strcpy(cityData.country,city["country"]);
    cityData.population = city["population"];
    cityData.timezone = city["timezone"];
    cityData.sunrise = city["sunrise"];
    cityData.sunrise = city["sunset"];

    http.end();
    validDataForeCast = true;
    updating = updatingForeCast = false;
    return true;

    }

// Download pollution data
bool  OpenWeatherMap::dlPollutionData(){

    if(currentData.cod!=200){
        log_d("Cod %d: %s\r\n",currentData.cod, currentData.errMsg);
        validDataPollution = false;
        updating = updatingPollutionData = false;
        return true;
    }

    HTTPClient http;
    DynamicJsonDocument doc(1100);

    validDataPollution = false;
    updating = updatingPollutionData = true;

    
    sprintf(urlBuffer,UrlPollution,setting.home.lat,setting.home.lon,setting.owm_apikey,setting.lang);
    http.begin(urlBuffer);
    int httpResponseCode = http.GET();
    
    if (httpResponseCode>0) {
        String payLoad = http.getString();
        DeserializationError err = deserializeJson(doc, payLoad);
        if (err) {
            log_e("deserializeJson() failed with code %s\r\n",err.c_str());
            validDataPollution = false;
            updating = updatingPollutionData = false;
            return false;
            }

        //Coord
        pollution._coord.lon = doc["coord"]["lon"]; 
        pollution._coord.lat = doc["coord"]["lat"]; 
        JsonObject list_0 = doc["list"][0];
        pollution.n_aqi = list_0["main"]["aqi"];
        pollution.n_aqi--;

        if(strcmp("ES",setting.lang)==0)
            strcpy(pollution.aqi,aqiPollution[0][pollution.n_aqi]);
        else
            strcpy(pollution.aqi,aqiPollution[1][pollution.n_aqi]);

        JsonObject list_0_components = list_0["components"];
        pollution.co =  list_0_components["co"]; 
        pollution.no = list_0_components["no"]; 
        pollution.no2 = list_0_components["no2"]; 
        pollution.o3 = list_0_components["o3"]; 
        pollution.so2 = list_0_components["so2"]; 
        pollution.pm2_5 = list_0_components["pm2_5"]; 
        pollution.pm10 = list_0_components["pm10"]; 
        pollution.nh3 = list_0_components["nh3"]; 
        pollution.dt = list_0["dt"]; 
        }
    else{
        validDataPollution = false;
        updating = updatingPollutionData = false;
        return false;
    }

    http.end();
    validDataPollution = true;
    updating = updatingPollutionData = false;
    return true;

}

// Nombres de los vientos en mediterraneo...
// Viento del norte o Tramontana (N): de 337,5° a 22,5°
// Viento del noreste o Gregal (NE): de 22,5° a 67,6°
// Viento del este o Levante (E): de 67,5° a 112,5°
// Viento del sureste o Siroco (SE): de 112,5° a 157,5°
// Viento del sur u Ostro (S): de 157,5° a 202,5°
// Viento del suroeste: Lebeche o Garbino (SW): de 202,5° a 247,5°
// Viento del oeste o Poniente (W): de 247,5° a 292,5°
// Viento del noroeste: Mistral (NW): de 292,5° a 337,5°

void OpenWeatherMap::getNameWind(float deg,wind* wind){
    int indice = 0;
    int m=0;
    if(strcmp("ES",setting.lang)==0)
        m=0;
    else
        m=1;
    if(deg<0 || deg>=360){
        indice = -1;
        }
    if((deg >= 0 && deg<=22.5) || (deg>=337.5 && deg<=359)){
        indice =0;
    }
    if(deg>=22.5 && deg<= 67.5){
        indice =1;
    }
    if(deg>=67.5 && deg<= 112.5){
        indice =2;
    }
    if(deg>=112.5 && deg<= 157.5){
        indice =3;
    }
    if(deg>=157.5 && deg<= 202.5){
        indice =4;
    }
    if(deg>=202.5 && deg<= 247.5){
        indice =5;
    }
    if(deg>=247.5 && deg<= 292.5){
        indice =6;
    }
    if(deg>=292.5 && deg<= 337.5){
        indice =7;
    }

    if(indice == -1){
        strcpy(wind->NombreViento,"");
        strcpy(wind->pcAbv,"");
        strcpy(wind->dirViento,"");

    }
    else{
        strcpy(wind->NombreViento,nombreViento[indice]);
        strcpy(wind->pcAbv,pcAbv[indice]);
        strcpy(wind->dirViento,dirViento[m][indice]);
    }
}
