#include <touchEsp32.h>


touchEsp32::touchEsp32(int _pin):TaskParent(TOUCHTASK_NAME,TOUCHTASK_HEAP,TOUCHTASK_PRIORITY, TOUCHTASK_CORE){
    pin = _pin;
    setCalibration();
    queue = xQueueCreate(5,sizeof(uint32_t));
    
}

void touchEsp32::loop(){
    delay(1000);
    setCalibration();
    queue = xQueueCreate(5,sizeof(uint32_t));
    if(queue == NULL){
        log_e("Error al crear la pila del boton\r\n");
        suspend();
        }
    uint32_t ton;
    uint32_t toff;
    pressed = false;

    while(true){
        if(read()){
            ton = millis();
            delay(10);
            if(!read())
                continue;
            while(read()){
                vPortYield();
                }
            toff = millis();
            tPressed = toff - ton;
            pressed = true;
            xQueueSend( queue,(void *) &tPressed,(TickType_t )0);
        }
        vPortYield();
    }
}

// Toma en cal la media de NCALTOUCH medidas
int touchEsp32::setCalibration(){
    calibration = 0L;
    for(int n=0; n < NCALTOUCH; n++){
        calibration += touchRead(pin);
        }
    calibration = calibration / NCALTOUCH;
    return calibration;
}

// Lee el pin
bool touchEsp32::read(){
    int a = touchRead(pin);;
    if( a < (calibration-10))
        return true;
    else
        return false;
}
