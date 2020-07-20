#include <Adafruit_Sensor.h>
#include <DHT.h>

#include "TemperatureSensor.h"

class SensorDHT : public TemperatureSensor {
    public:
        SensorDHT(uint8_t pin) : sensor(pin, DHT11) {
            sensor.begin();
        }
    
        float getReadings() {
            if(settings.useFahreheit) {
                return calculateTemperature(sensor.readTemperature(true));
            } else {
                return calculateTemperature(sensor.readTemperature());
            }
        }

    private:
        DHT sensor;
};

