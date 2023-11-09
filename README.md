# weather_esp32_clock
[En español](LEEME.md)  
Weather station with desktop clock based on api.openweathermap.org and BME280 for esp32 on Arduino core.


![Alt text](images/en_screen1.png)
![Alt text](images/es_screen1.png)
![Alt text](images/en_air_quality.png)
![Alt text](images/en_screen2.png)
![Alt text](images/es_forecast_screen.png)


#### Features:
  
    NTP based clock with daylight saving time support.
    Weather data from openweathermaps.org.
    Local temperature, humidity and pressure data from a BME280 sensor.
    Web server with WIFI in AP mode to configure WIFI.
    Web server for configuration.
    Data displayed.
        Date and Time.
        Wind speed and direction, name of the wind.
        Sunrise time.
        Sunset time.
        Weather description and icon.
        Temperature.
        Maximum temperature.
        Minimum temperature.
        Feels like.
        Atmospheric pressure.
        Relative humidity.
        Location name.
        Air quality, rating and data (CO,NO,NO2,O3,NH3,SO2,PM2-5 and PM10)
        Five days of forecast with day name, time, temperature, probability of precipitation and description.

#### D1 Mini ESP32 ESP-32:

![Alt text](images/wemo_d1_mini_A.png) ![Alt text](images/wemo_d1_mini_B.png)

#### Digital Temperature Humidity Sensor Barometric Pressure Module BME280 (opcional):

![Alt text](/images/bme280.png)

## Compilation
#### Arduino IDE

    You need to have support for esp32 boards, look at https://espressif-docs.readthedocs-hosted.com/projects/arduino-esp32/en/latest/installing.html how to install.

    -Install Git:
      https://git-scm.com/book/en/v2/Getting-Started-Installing-Git
    -Clone the repository
      From the Arduino directory run: git clone https://github.com/josaezlopez/weather_esp32_clock.git
    -Install the libraries
      Change to the directory created by git: cd weather_esp32_clock
        -On Linux run the make_arduino_ide.sh script: sh make_arduino_ide.sh
        -On Windows run the make_arduino_ide.bat script from cmd.
            - Install the following libraries from the IDE library manager.
                -ArduinoJson@^6.20.1
                -Adafruit BME280 Library@^2.2.2
                -Adafruit Unified Sensor@^1.1.7
                -CRC32@^2.0.0
                -SPIFFS ImageReader Library@^1.0.0
                -Adafruit ILI9341@^1.5.14

    -Run the arduino IDE and open the weather_esp32_clock.ino sketch.
    -Configure the esp32 board used and the port it is connected to, choose the partition scheme and debug level as shown in the image:  
![Alt text](/images/web.png)

#### PlatformIO
    Download or clone the repository, as in the previous case, to the Platformio projects directory.

## Setting

#### conf.h
If you do not have the BME280 sensor installed change the **ENABLE_BME280** directive to false.
Optionally, you can enter the SSID, password and Apikey of openweathermaps or enter it later from the access point that you will create if you cannot connect.
The free openweathermaps apikey can be obtained from here: https://openweathermap.org/price.

Compile and upload the program to the board.

  
There are three environments in platformio.ini; release, release_ota and debug. The release_ota environment uploads to the board via WIFI. The first time the program is sent it must be done by cable, using the release environment.

If the correct SSID and password of your WIFI were specified, the device will boot with the default configuration, you can change it by accessing **http://wstation.local**
The coordinates can be pasted from Google Maps.
## Functioning:

After booting it shows:

Screen number 1:

    • Temperature
    • Pressure
    • RH
    • Description and icon.
    • Wind direction, name (in the Mediterranean) and speed
    • Sunrise and sunset time:
    • Clock with date and time (can be set to summer/winter time depending on settings)

With a press on the touch button it shows:

    • Temperature read by the BME280
    • Pressure read by the BME280
    • Humidity read by the BME280
    • Description and icon
    • Wind direction, name (in the Mediterranean) and speed
    • Sunrise and sunset time:
    • Clock with date and time.

With a press on the touch button it starts showing the air quality:

Screen number 2:

    • Air quality classification
    • CO (carbon monoxide)
    • NO (nitrogen monoxide)
    • NO2 (nitrogen dioxide)
    • O3 (ozone)
    • NH3 Ammonia
    • SO2 (sulfur dioxide)
    • PM 2-5 (particles between 2 and 5 uM
    • PM 10 (particles larger than 10uM


By pressing the touch button, it will display the weather general data.  

Screen number 3:

    • Temperature
    • Maximum temperature (last 24 hours)
    • Minimum temperature (last 24 hours)
    • Thermal sensation.
    • Humidity
    • Pressure
    • Wind direction, name and speed.
    • Climate description.

From this screen, a short press returns to Screen 1 and a long press shows the weather forecast for five days.

Dependencies:

 bblanchon/ArduinoJson@^6.20.1  
 adafruit/Adafruit BME280 Library@^2.2.2  
 adafruit/Adafruit Unified Sensor@^1.1.7  
 josaezlopez/threadesp32@^1.0  
 josaezlopez/NTPClientExt@^1.0.0  
 bakercp/CRC32@^2.0.0  
 lucadentella/SPIFFS ImageReader Library@^1.0.0  
 adafruit/Adafruit ILI9341@^1.5.14  

stl for print box:
https://cults3d.com/es/modelo-3d/artilugios/caja-para-esp32-esp8266-y-tft-2-2-pulgadas-ili9341-josaezlopez

