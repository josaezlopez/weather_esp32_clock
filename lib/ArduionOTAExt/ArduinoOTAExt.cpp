#include <ArduinoOTAExt.h>

ArduinoOTAExt::ArduinoOTAExt(Adafruit_ILI9341Ext& _tft,const char* _pass,const char* _nombre)   
  : TaskParent(OTATASK_NAME, OTATASK_HEAP,OTATASK_PRIORITY, OTATASK_CORE){
  tft = &_tft; 
  strcpy(password,_pass);
  strcpy(nombre,_nombre);
}

void ArduinoOTAExt::loop(){
    delay(1000);
    initOTA();
    while(true){
        ArduinoOTA.handle();
        vPortYield();
    }
}

void ArduinoOTAExt::initOTA(){
    ArduinoOTA.onStart([&]() {
        String type;
        if (ArduinoOTA.getCommand() == U_FLASH)
          type = "sketch";
        else // U_SPIFFS
          type = "filesystem";
        setUpload();
        tft->suspend();
        delay(100);
        tft->fillScreen(ILI9341_BLACK);
        tft->setTextColor(ILI9341_YELLOW,ILI9341_BLACK);
        tft->setTextSize(3);
        tft->setCursor(0,0);
        tft->println("Uploading");
      })
    .onEnd([&]() {
        // Reinicio por vencimiento del wdt, necesario para que reinicie al finalizar el upload
        esp_task_wdt_init(1, true);
        esp_task_wdt_add(NULL);
        while(true);  
      })
    .onProgress([&](unsigned int progress, unsigned int total) {
        esp_task_wdt_reset();     // Para que no temporize el TWDT
        tft->setTextSize(5);
        tft->setCursor(75,100);
        tft->printf("%u%%\r", (progress / (total / 100)));
      })
    .onError([&](ota_error_t error) {
        tft->fillScreen(ILI9341_BLACK);
        tft->setTextColor(ILI9341_YELLOW,ILI9341_BLACK);
        tft->setTextSize(2);
        tft->setCursor(0,-20);
        tft->printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) tft->print("Fallo de autenticacion");
        else 
          if (error == OTA_BEGIN_ERROR) tft->print("Begin Failed");
        else 
          if (error == OTA_CONNECT_ERROR) tft->print("Connect Failed");
        else 
          if (error == OTA_RECEIVE_ERROR) tft->print("Receive Failed");
        else 
          if (error == OTA_END_ERROR) tft->print("End Failed");
      });

    if (!MDNS.begin(nombre)) {
        Serial.println("Error setting up MDNS responder!");
    }
 
    ArduinoOTA.setMdnsEnabled(true);
    ArduinoOTA.setRebootOnSuccess(true);
    ArduinoOTA.setPassword(password);
    ArduinoOTA.setHostname(nombre);
    ArduinoOTA.begin();
}