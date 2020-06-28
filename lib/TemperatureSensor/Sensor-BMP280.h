#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

#include "Config.h"

class TemperatureSensor : public BaseTemperatureSensor {
    public:
        #if defined USE_BMP280_I2C
            TemperatureSensor(uint8_t pin, float offset) {
                // pin is not used, only included for conformity
                _useFahrenheit = false;
                _offset = offset;
                
                sensor.begin();
                delay(2000);
            }
        #endif

        #if defined USE_BMP280_HSPI
            TemperatureSensor(uint8_t pin, float offset) : sensor(TEMP_SENSOR_PIN, HSPI_MOSI, HSPI_MISO, HSPI_SCLK) {
                // pin is not used, only included for conformity
                _useFahrenheit = false;
                _offset = offset;
                
                sensor.begin();
                delay(2000);
            }
        #endif

        #if defined USE_BMP280_VSPI
            TemperatureSensor(uint8_t pin, float offset) : sensor(TEMP_SENSOR_PIN, VSPI_MOSI, VSPI_MISO, VSPI_SCLK) {
                // pin is not used, only included for conformity
                _useFahrenheit = false;
                _offset = offset;
                
                sensor.begin();
                delay(2000);
            }
        #endif

        float readTemperature() {
            if(_useFahrenheit) {
                return ((calculateTemperature(sensor.readTemperature()) * 9 / 5) + 32 + _offset);
            } else {
                return calculateTemperature(sensor.readTemperature() + _offset);
            }
        }

    private:
        Adafruit_BMP280 sensor;
};