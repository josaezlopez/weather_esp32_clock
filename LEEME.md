# weather_esp32_clock  

Estacion meteorológica con reloj de escritorio basada en api.openweathermap.org y BME280 para esp32 en Arduino core.

![Alt text](images/en_screen1.png)
![Alt text](images/es_screen1.png)
![Alt text](images/en_air_quality.png)
![Alt text](images/en_screen2.png)
![Alt text](images/es_forecast_screen.png)


#### Características:

	Reloj basado en NTP.  
	Datos del clima desde openweathermaps.org.  
	Datos de temperatura, humedad y presión locales desde un sensor BME280.  
	Servidor web para la configuración.
	Previsión del tiempo con Probabilidad de precipitacion, temperatura y descripción.  

#### D1 Mini ESP32 ESP-32:

![Alt text](images/wemo_d1_mini_A.png) ![Alt text](images/wemo_d1_mini_B.png)

#### Sensor Digital de temperatura, humedad, módulo de presión barométrica, 	BME280 (opcional):

![Alt text](/images/bme280.png)

#### Módulo LCD SPI de 2,2 pulgadas, 240x320, ILI9341:

![Alt text](/images/TFT_2_2_ILI9341.png)

#### Esquema

![Alt text](/images/schematic.png)

## Compilación:

  #### Preparación del sistema de ficheros SPIFFS
  
Antes de compilar modifique **conf.h** para poner el ssid password de su WIFI y la APIKEY de openweathermaps.
Si no tiene instalado el sensor BME280 cambie la directiva **ENABLE_BME280** a false. 
Se usa el apikey gratuito, desde aquí se puede conseguir el apikey: https://openweathermap.org/price.
  
Si se usa platformio se resolverán correctamente todas las dependencias de las librerías.
Existen dos entornos en platformio.ini: release y release_ota. release_ota permite programar el dispositivo por OTA.
Después de compilar y enviar al esp32. Arrancará con la configuración por defecto, para cambiarla puede acceder a **http://wstation.local** 
Las coordenadas se pueden pegar desde google maps.

![Alt text](/images/web.png)


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
