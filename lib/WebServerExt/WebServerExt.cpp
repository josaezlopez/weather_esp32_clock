#include <WebServerExt.h>
#include <EEPROM.h>

extern void debugSettings();
extern WebServerExt* httpServer;
extern OpenWeatherMap* Location;
extern NTPClientExt* timeClient;
extern conf setting;
extern void updateCRC();

const char* homePage =
    "<html>\
        <head>\
            <meta  name='viewport' content='width=device-width, initial-scale=1'>\
            <title>ESP32 WStation</title>\
            <style>\
                body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
            </style>\
        </head>\
        <style> .div-1 {background: #EBEBEB;}\
            div { height: 100px;margin: 0px;border: 2px solid;background-color: #FBD603;}\
        </style>\
        <body>\
            <h1>Wstation ESP32</h1>\
            <form action='/update/' method='post'>\
                SSID    : <input type='text' disabled name='ssid' value='%s'><br>\
                <div class='div-1'>\
                    Language : %s<br>\
                    APIKey  : <input type='text' name='apikey' value='%s' size='36'><br>\
                    Coordinates (lat,lon): <input type='text' name='coord' size='64' value='%.16f,%.16f'> Set time from coordinates <input type='checkbox' name='fromCoord' value='1' /><br>\
                    Refresh time: <input type='text' name='tupdate' value='%d'><br>\
                    Temperature unit: %s<br>\
                </div>\
                <div class='div-1'>\
                    Time zone: %s<br>\
                    Timetable summer: %s<br>\
                    Summer time start month (last Sunday of the month): %s<br>\
                    Month of end of summer time (last Sunday of the month:: %s<br>\
                </div>\
                <button type='submit' name='b1' value='update'>Update</button>\
            </form>\
        </body>\
        <script>\
            function enDisMonth(value) {\
                if(value=='Yes'){\
                    document.getElementById('startds').disabled = false;\
                    document.getElementById('endds').disabled = false;\
                    }\
                else{\
                    document.getElementById('startds').disabled = true;\
                    document.getElementById('endds').disabled = true;\
                }\
            }\
        </script>\
        </html>";

const char* toRoot =
    "<html>\
        <head>\
            <meta  name='viewport' content='width=device-width, initial-scale=1'>\
            <title>ESP32 WStation</title>\
            <style>\
                body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
            </style>\
        </head>\
        <body>\
            <h1>Wstation ESP32</h1>\
            <form action='/' method='post'>\
                <button type='submit'>Back</button>\
            </form>\
        </body>\
        </html>";

String selectUnitTemperature(char* value){
    String ret =  String("<select name='unittemp'>");
    ret += "<option value='C'";
    if(strcmp(value,"C")==0)
        ret += " selected";
    ret += ">Celsius</option>";

    ret += "<option value='F'";
    if(strcmp(value,"F")==0)
        ret += " selected";
    ret += ">Farenheit</option>";
    ret += "</select>";
    return ret;
}


String selectLanguage(char* value){
    String ret =  String("<select name='language'>");
    ret += "<option value='ES'";
    if(strcmp(value,"ES")==0)
        ret += " selected";
    ret += ">ES</option>";

    ret += "<option value='EN'";
    if(strcmp(value,"EN")==0)
        ret += " selected";
    ret += ">EN</option>";
    ret += "</select>";
    return ret;
}


String selectMonth(const char* name,int value){
    String ret =  String("<select name='");
    ret += name;
    ret += "' ";
    ret += "id='";
    ret += name;
    ret += "'";
    if(!setting.dayligthSaving)
        ret += " disabled";
    ret +=">";
    

    for(int n=1; n<13; n++){
        ret += String("<option value='") + n + "' ";
        if(value==n)
            ret += " selected";
        ret += String(">") + timeClient->getNameOfMonth(n-1) + "</option>";
        }
        
    ret += "</select>";
    return ret;
}


String selectDay(bool value){
    String ret =  String("<select onchange='enDisMonth(this.value)' name='dayligthsaving'>");
    ret += "<option value='0'";
    if(!value)
        ret += " selected";
    ret += ">No</option>";
    ret += "<option value='1'";
    if(value)
        ret += " selected";
    ret += ">Yes</option>";
    ret += "</select>";
    return ret;
}

String optionSelectTimezone(int value,int tz){
    String ret;

    ret = String("<option value='");
    ret += tz;
    ret += "'";
    if(value==tz) ret += " selected";
    ret += ">UTC(";
    if(tz > 0) ret += "+";
    ret += tz;
    ret += ")</option>";
    return ret;
}

String selectTimezone(int value){
    String ret =  String("<select name='timezone'>");
 
    for(int n = 12; n > -13; n--){
        ret += optionSelectTimezone(value,n);
    }
    ret += "</select>";
    return ret;
}

WebServerExt::WebServerExt(int port)
    :WebServer(port),TaskParent(WEBTASK_NAME,WEBTASK_HEAP,WEBTASK_PRIORITY, WEBTASK_CORE){
    on("/", handleRoot);
    on("/update/", update);
    }


void WebServerExt::loop(){
    delay(1000);
    begin();
    while(true){
        handleClient();
        delay(1);
        }
    }


void update(){

    String unitTemp = httpServer->arg("unittemp");
    String language = httpServer->arg("language");
    String coordText = httpServer->arg("coord");
    String timeZone = httpServer->arg("timezone");
    String daylightSaving = httpServer->arg("dayligthsaving");
    String apikey = httpServer->arg("apikey");
    String tupdate = httpServer->arg("tupdate");
    int fromCoord = httpServer->arg("fromCoord").toInt();
    int  startDS = String(httpServer->arg("startds")).toInt();
    int  endDS = String(httpServer->arg("endds")).toInt();

    int nComma = 0;
    int nPeriod = 0;

    for(int n = 0; n < coordText.length(); n++){
        if(coordText.charAt(n)=='.'){
            nPeriod++;
        }
        if(coordText.charAt(n)==','){
            nPeriod++;
        }
    }

    if(nComma==0 || nComma==2 || nComma >3){
        httpServer->send(200,"text/html",String("Invalid coordinates."));
        return;
        }

    int c,c1,c2,c3;
    if(nComma==3){
        c1 = coordText.indexOf(',');        // c1 = pos primera coma
        c2 = coordText.indexOf(',',c1+1);   // c2 = pos segunda coma
        c3 = coordText.indexOf(',',c2+1);   // c3 = pos tercera coma
        coordText[c1] = coordText[c3] = '.';    // Se cambia primera y ultima coma por '.' 
        }

    c = coordText.indexOf(',');     // Se vuelve a busca la coma, ahora solo hay una.
    setting.home.lat =  atof(coordText.substring(0,c).c_str()); 
    setting.home.lon = atof(coordText.substring(c+1).c_str());
    strcpy(setting.lang,language.c_str());
    timeClient->setLang(setting.lang);
    
    Location->forceUpdate(); 

    if(fromCoord){
        setting.timeZone = Location->getCurrentData()->_sys.timezone / 3600; 
        setting.dayligthSaving = 0;
        }
    else{
        setting.timeZone = timeZone.toInt();
        setting.dayligthSaving = daylightSaving.toInt();
        }


    timeClient->setTimeZone(setting.timeZone);
    setting.monthStartDS = startDS==0 ? setting.monthStartDS : startDS;
    setting.monthEndDS = endDS==0 ? setting.monthEndDS : endDS;

    strcpy(setting.owm_apikey,apikey.c_str());
    setting.owm_tupdate = tupdate.toInt();
    
    strcpy(setting.unitTemp,unitTemp.c_str());

    if(setting.dayligthSaving){
        timeClient->setDaylightSaving(setting.monthStartDS,setting.monthEndDS);
        }
    else{
        timeClient->unsetDaylightSaving();
        }

    EEPROM.put(0x0,setting);
    updateCRC();
    debugSettings();
    httpServer->send(200,"text/html",toRoot);
}

void handleRoot(){
   
    static char buffer[4096];
 
    sprintf(buffer,homePage,
        setting.ssid,
        selectLanguage(setting.lang).c_str(),
        setting.owm_apikey,
        setting.home.lat,
        setting.home.lon,
        setting.owm_tupdate,
        selectUnitTemperature(setting.unitTemp).c_str(),    
        selectTimezone(setting.timeZone).c_str(),
        selectDay(setting.dayligthSaving).c_str(),
        selectMonth("startds",setting.monthStartDS).c_str(),    
        selectMonth("endds",setting.monthEndDS).c_str());
   
    httpServer->send(200,"text/html",buffer);

}



