#pragma once

#include <Arduino.h>
#include <threadesp32.h>

#define TOUCHTASK_NAME        "PULSADOR"
#define TOUCHTASK_HEAP        1024
#define TOUCHTASK_PRIORITY    TaskParent::th_LOWEST_PRIORITY
#define TOUCHTASK_CORE        1
#define NCALTOUCH 1000

class touchEsp32 : public TaskParent {
  public:
    touchEsp32(int _pin);
    void loop();
    bool getPulsacion();
    bool getPulsacion(uint32_t* ton);
    QueueHandle_t getQueue() { return queue;}

  protected:
    int calibracion();
    QueueHandle_t queue;
    int pin;
    int32_t cal;
    uint32_t tPulsado;
    bool pulsado = false;
    bool read();
};
