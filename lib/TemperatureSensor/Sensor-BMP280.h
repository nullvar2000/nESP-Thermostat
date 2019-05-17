#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

#include "userConfig.h"

class TemperatureSensor : public BaseTemperatureSensor {
    public:
        TemperatureSensor(uint8_t pin, bool useFahrenheit, float offset) : sensor(pin) {
            _useFahrenheit = useFahrenheit;
            _offset = offset;
            
            sensor.begin();
            delay(5000);
        }
    
        float readTemperature() {
            if(_useFahrenheit) {
                return (calculateTemperature(sensor.readTemperature(true) * 9 / 5) + 32 + _offset);
            } else {
                return calculateTemperature(sensor.readTemperature() + _offset);
            }
        }

    private:
        Adafruit_BMP280 sensor;
};