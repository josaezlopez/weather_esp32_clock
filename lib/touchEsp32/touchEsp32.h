#pragma once

#include <Arduino.h>
#include <threadesp32.h>

#define TOUCHTASK_NAME        "BUTTON"
#define TOUCHTASK_HEAP        1024
#define TOUCHTASK_PRIORITY    TaskParent::th_LOWEST_PRIORITY
#define TOUCHTASK_CORE        1
#define NCALTOUCH 1000

class touchEsp32 : public TaskParent {
  public:
    touchEsp32(int _pin);
    void loop();
    QueueHandle_t getQueue() { return queue;}

  protected:
    int setCalibration();
    QueueHandle_t queue;
    int pin;
    int32_t calibration;
    uint32_t tPressed;
    bool pressed = false;
    bool read();
};
