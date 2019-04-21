#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

#include "userConfig.h"

Adafruit_BMP280 bmp(TEMP_SENSOR_PIN); // VSPI
float tempOffset = 0.0;

bool initSensor() {
    if(!bmp.begin()) {
        return false;
    }
    return true;
}

float readTemperature() {
    #ifdef USE_FAHRENHEIT
        return (bmp.readTemperature() * 9 / 5) + 32 + tempOffset;
    #else
        return bmp.readTemperature() + tempOffset;
    #endif
}

void setOffset(float offset) {
    tempOffset = offset;
}

