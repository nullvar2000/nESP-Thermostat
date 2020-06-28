#include <Adafruit_Sensor.h>
#include <DHT.h>

#include "Config.h"

class TemperatureSensor : public BaseTemperatureSensor {
    public:
        TemperatureSensor(uint8_t pin, float offset) : sensor(pin, DHT11) {
            _useFahrenheit = false;
            _offset = offset;
            
            sensor.begin();
            delay(5000);
        }
    
        float readTemperature() {
            if(_useFahrenheit) {
                return calculateTemperature(sensor.readTemperature(true) + _offset);
            } else {
                return calculateTemperature(sensor.readTemperature() + _offset);
            }
        }

    private:
        DHT sensor;
};

