#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

#include "TemperatureSensor.h"

class SensorBMP280 : public TemperatureSensor {
    public:
        #if defined USE_BMP280_I2C
            SensorBMP280(uint8_t pin) {
                // pin is not used, only included for conformity
                
                sensor.begin();
            }
        #endif

        #if defined USE_BMP280_HSPI
            SensorBMP280(uint8_t pin) : sensor(TEMP_SENSOR_PIN, HSPI_MOSI, HSPI_MISO, HSPI_SCLK) {
                // pin is not used, only included for conformity
                
                sensor.begin();
            }
        #endif

        #if defined USE_BMP280_VSPI
            SensorBMP280(uint8_t pin) : sensor(TEMP_SENSOR_PIN, VSPI_MOSI, VSPI_MISO, VSPI_SCLK) {
                // pin is not used, only included for conformity
                
                sensor.begin();
            }
        #endif

        float getReadings() {
            if(settings.useFahreheit) {
                return ((calculateTemperature(sensor.readTemperature()) * 9 / 5) + 32);
            } else {
                return calculateTemperature(sensor.readTemperature());
            }
        }

    private:
        Adafruit_BMP280 sensor;
};