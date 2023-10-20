#include <touchEsp32.h>


touchEsp32::touchEsp32(int _pin):TaskParent(TOUCHTASK_NAME,TOUCHTASK_HEAP,TOUCHTASK_PRIORITY, TOUCHTASK_CORE){
    pin = _pin;
    calibracion();
    queue = xQueueCreate(5,sizeof(uint32_t));
    
}

// Retorna true si ha pulsado, pone en ton el tiempo que ha estado pulsado.
bool touchEsp32::getPulsacion(uint32_t* ton){
    if(pulsado){
        pulsado = false;
        *ton = tPulsado;
        return true;
    }
    return false;
}

// Retorna true si se ha pulsado.
bool touchEsp32::getPulsacion(){
    if(pulsado){
        pulsado = false;
        return true;
    }
    return false;
}

void touchEsp32::loop(){
    delay(1000);
    calibracion();
    queue = xQueueCreate(5,sizeof(uint32_t));
    if(queue == NULL){
        log_e("Error al crear la pila del boton\r\n");
        suspend();
        }
    uint32_t ton;
    uint32_t toff;
    pulsado = false;

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
            tPulsado = toff - ton;
            pulsado = true;
            xQueueSend( queue,(void *) &tPulsado,(TickType_t )0);
        }
        vPortYield();
    }
}

// Toma en cal la media de NCALTOUCH medidas
int touchEsp32::calibracion(){
    cal = 0L;
    for(int n=0; n < NCALTOUCH; n++){
        cal += touchRead(pin);
        }
    cal = cal / NCALTOUCH;
    return cal;
}

// Lee el pin
bool touchEsp32::read(){
    int a = touchRead(pin);;
    if( a < (cal-10))
        return true;
    else
        return false;
}
