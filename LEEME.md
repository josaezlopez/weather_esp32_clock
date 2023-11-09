# weather_esp32_clock  

Estación meteorológica con reloj de escritorio basada en api.openweathermap.org y BME280 para esp32 en Arduino core.  

![Alt text](images/en_screen1.png)
![Alt text](images/es_screen1.png)
![Alt text](images/en_air_quality.png)
![Alt text](images/en_screen2.png)
![Alt text](images/es_forecast_screen.png)


#### Características:

	Reloj basado en NTP con soporte de horario de verano. 
	Datos del clima desde openweathermaps.org.  
	Datos de temperatura, humedad y presión locales desde un sensor BME280.
    Servidor web con WIFI en modo AP para configurar la WIFI.  
	Servidor web para la configuración.
    Datos mostrados.
        Fecha y hora.
        Velocidad y direccion del viento, nombre del viento.
        Hora de amanecer.
        Hora de anochecer.
        Descripción del clima e icono.
        Temperatura.
        Temperatura máxima.
        Temperatura mínima.
        Sensación térmica.
        Presión atmosférica.
        Humedad relativa.
        Nombre de la localización.
        Calificación de la calidad del aire y datos (CO,NO,NO2,O3,NH3,SO2,PM2-5 y PM10)
        Cinco días de previsión con nombre del día, hora, temperatura, probabilidad de precipitación y descripción.



#### D1 Mini ESP32 ESP-32:

![Alt text](images/wemo_d1_mini_A.png) ![Alt text](images/wemo_d1_mini_B.png)

#### Sensor Digital de temperatura, humedad, módulo de presión barométrica, 	BME280 (opcional):

![Alt text](/images/bme280.png)

#### Módulo LCD SPI de 2,2 pulgadas, 240x320, ILI9341:

![Alt text](/images/TFT_2_2_ILI9341.png)

#### Esquema

![Alt text](/images/schematic.png)

## Compilación
#### Arduino IDE

    Necesita tener soporte para las placas esp32, mire en https://espressif-docs.readthedocs-hosted.com/projects/arduino-esp32/en/latest/installing.html como realizar la instalación.  

    -Instalar Git:  
      https://git-scm.com/book/en/v2/Getting-Started-Installing-Git
    -Clonar el repositorio
      Desde el directorio de Arduino ejecutad: git clone https://github.com/josaezlopez/weather_esp32_clock.git
    -Instalar las librerias
      Cambiese al directorio que creó git: cd weather_esp32_clock
        -En linux ejecutar el script make_arduino_ide.sh: sh make_arduino_ide.sh
        -En windows ejecutar el script make_arduino_ide.bat desde el cmd.
            - Instalar las siguientes librerías desde el gestor de librerías del IDE.  
                -ArduinoJson@^6.20.1
	            -Adafruit BME280 Library@^2.2.2
	            -Adafruit Unified Sensor@^1.1.7
                -CRC32@^2.0.0
	            -SPIFFS ImageReader Library@^1.0.0
	            -Adafruit ILI9341@^1.5.14

    -Ejecute el IDE de arduino y abra el sketch weather_esp32_clock.ino.
    -Configure la placa esp32 utilizada y el puerto al que está conectado, elija el esquema de particiones y el nivel de debug como se muestra en la image:  
![Alt text](/images/arduino_ide_settings.png) 

#### PlaformIO
    Descarge o clone el repositorio, como en el caso anterior, sobre el directorio de projectos de Platformio.

## Configuración

#### conf.h
Si no tiene instalado el sensor BME280 cambie la directiva **ENABLE_BME280** a false. 
Opcionalmente, puede poner el SSID, password y Apikey de openweathermaps o bien ponerlo después desde el punto de acesso que creará si no puede conectar.
El apikey gratuito de openweathermaps se puede conseguir desde aquí: https://openweathermap.org/price.

Compile y suba el programa a la placa. 

  
Existen tres entornos en platformio.ini; release, release_ota y debug. El entorno release_ota hace el upload a la placa por WIFI. La primera vez que se envíe el programa hay que hacerlo por cable, usando el entorna release.

Si se especificó el SSID y password correctos de su WIFI el dispositivo arrancará con la configuración por defecto, puede cambiarla accediendo a **http://wstation.local**
Las coordenadas se pueden pegar desde google maps.

![Alt text](/images/web.png)

Si no especificó un SSID y Password correctos el dispositivo arrancará en modo AP.
Busque la WIFI 'wstation' y asociese a ella, despues, desde el navegador web vaya a http://192.168.4.1 , actualice el SSID y password, se reiniciará y podrá acabar la configuración como antes, en **http://wstation.local**

## Funcionamiento
Después de arrancar muestra:

Pantalla numero 1:

    • Temperatura
    • Presión
    • Humedad relativa
    • Descripción e icono.
    • Dirección del viento, nombre (en el mediterráneo) y velocidad
    • Hora del amanecer y anochecer:
    • Reloj con fecha y hora (puede ajustarse a horario de verano/invierno según la configuración)

Con una pulsación en el botón touch pasa a mostrar:

    • Temperatura leída  por el BME280
    • Presión leída  por el BME280
    • Humedad leída por el BME280
    • Descripción e icono
    • Dirección del viento, nombre (en el mediterráneo) y velocidad
    • Hora del amanecer y anochecer:
    • Reloj con fecha y hora.

Con una pulsación en el botón touch pasa a mostrar la calidad del aire:

Pantalla numero 2:

    • Clasificación de la calidad del aire
    • CO (monóxido de carbono)
    • NO (monóxido de nitrógeno)
    • NO2 (dióxido de nitrogeno)
    • O3 (ozono)
    • NH3 Amoniaco
    • SO2 (dióxido de azufre)
    • PM 2-5 (partículas entre 2 y 5 uM
    • PM 10 (partículas de mas de 10uM

Con una pulsación en el botón touch pasa a mostrar los datos generales del tiempo.

Pantalla numero 3:

    • Temperatura
    • Temperatura máxima (ultima 24 horas)
    • Temperatura mínima (ultimas 24 horas)
    • Sensación térmica.
    • Humedad
    • Presión
    • Dirección viento, nombre y velocidad.
    • Descripción clima.

Desde esta pantalla una pulsación corta vuelve a la Pantalla 1 y una larga muestra la previsión del clima en cinco dias.

Dependencias:

	bblanchon/ArduinoJson@^6.20.1
	adafruit/Adafruit BME280 Library@^2.2.2
	adafruit/Adafruit Unified Sensor@^1.1.7
	josaezlopez/threadesp32@^1.0
	josaezlopez/NTPClientExt@^1.0.0
	bakercp/CRC32@^2.0.0
	lucadentella/SPIFFS ImageReader Library@^1.0.0
	adafruit/Adafruit ILI9341@^1.5.14

stl para imprimir la caja
https://cults3d.com/es/modelo-3d/artilugios/caja-para-esp32-esp8266-y-tft-2-2-pulgadas-ili9341-josaezlopez
