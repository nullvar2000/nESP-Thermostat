#include <Adafruit_Sensor.h>
#include <DHT.h>

#include "userConfig.h"
#include "Logger.h"

#ifdef USE_DHT11
    DHT dht(TEMP_SENSOR_PIN, DHT11);
#elif USE_DHT21
    DHT dht(TEMP_SENSOR_PIN, DHT21);
#elif USE_DHT22
    DHT dht(TEMP_SENSOR_PIN, DHT22);
#endif

float tempOffset = 0.0;

void initSensor() {
    logln("Initializing DHT sensor");
    dht.begin();
    delay(5000);
    logln("Complete");
}

float readTemperature() {
    #ifdef USE_FAHRENHEIT
        return dht.readTemperature(true) + tempOffset;
    #else
        return dht.readTemperature() + tempOffset;
    #endif
}

void setOffset(float offset) {
    tempOffset = offset;
}

