copy lib\Adafruit_ILI9341Ext\*.cpp .
copy lib\Adafruit_ILI9341Ext\*.h .
copy lib\ArduinoOTAExt\*.cpp .
copy lib\ArduinoOTAExt\*.h .
copy lib\funcaux\*.cpp .
copy lib\funcaux\*.h .
copy lib\touchEsp32\*.cpp .
copy lib\touchEsp32\*.h .
copy lib\Weather\*.cpp .
copy lib\Weather\*.h .
copy lib\WebServerExt\*.cpp .
copy lib\WebServerExt\*.h .
copy src\*.h .
copy src\*.cpp .
copy src\*.ino .
cd ..
cd libraries
git clone https://github.com/josaezlopez/threadesp32.git
git clone https://github.com/josaezlopez/NTPClientExt.git
cd ..
cd weather_esp32_clock

