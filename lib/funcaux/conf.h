#pragma once

#define DEFAULT_SSID        "xxxxxxxx"
#define DEFAULT_PASSWD      "xxxxxxxx"

#define EEPROMSIZE (1024)

#define ENABLE_BME280   true                // True if the BME280 is connected, false otherwise.
#define BME280ADDRESS   0x76                // i2s address of the BME280
#define SEALEVELPRESSURE_HPA (1013.25)


#define DEFAULT_OPENWEATHERMAP_UPDATE 5            // Update time from openweathermap in minutes
#define DEFAULT_OPENWEATHERMAP_APIKEY       "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"


#define DEFAULT_LANG            "ES"                // Language, only supports ES or EN
#define DEFAULT_LAT             39.46993048810744   // Latidude
#define DEFAULT_LON             -0.3746582960577954 // Longitude
#define DEFAULT_UNIT_TEMP       "C"                 // F for Farenheit, C for Celsius

// NTP
#define NTPSERVER               "pool.ntp.org"      // Server NTP
#define DEFAULT_TIMEZONE        +1                  // Time zone
#define UPDATENTP               5                   // Time update interval from NTP server (minutes)
#define DEFAULT_MONTH_START_DS  3                   // Default Daylight saving start month (last sunday of the month)               
#define DEFAULT_MONTH_END_DS    10                  // Default Daylight saving end month (last sunday of the month) 
#define DEFAULT_DAYLIGTHSAVING  true                // Default True to enable daylight saving time, otherwise false

// Webserver
#define WEBSERVER_PORT	80
//OTA
#ifndef mDNSName
    #define mDNSName wstation
#endif
#define PASSOTA         "Pass4OTA"  //Password for OTA authentication. It should be the same as in platformio.ini [env:release_ota]

#define BUTTON_PIN      4   // Touch button

