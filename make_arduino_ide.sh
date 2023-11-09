#!/bin/bash

mv lib/*/* .
mv src/*.h .
mv src/*.cpp .
mv src/*.ino .
rm -rf lib
rm -rf src

dlLibrarie(){
	curl -L https://github.com/$1/$2/archive/$3.zip --output $3.zip
	mv $3.zip ../libraries/$3.zip
	cd ../libraries	
	unzip -o $3.zip
	rm -f $3.zip
}


dlLibrarie josaezlopez threadesp32 main
dlLibrarie josaezlopez NTPClientExt main
dlLibrarie arduino-libraries NTPClient master
dlLibrarie adafruit Adafruit_BME280 master
dlLibrarie adafruit Adafruit_BusIO master
dlLibrarie adafruit Adafruit_Sensor master
dlLibrarie adafruit Adafruit_ILI9341 master
dlLibrarie adafruit Adafruit-GFX-Library master
dlLibrarie adafruit Adafruit-GFX-Library master
dlLibrarie bblanchon ArduinoJson 6.x
dlLibrarie bakercp CRC32 master

