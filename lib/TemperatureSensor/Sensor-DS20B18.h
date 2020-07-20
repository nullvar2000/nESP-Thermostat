#include <OneWire.h>
#include <DallasTemperature.h>

#include "TemperatureSensor.h"

class SensorDS20B18 : public TemperatureSensor {
    public:
        SensorDS20B18(uint8_t pin) : oneWire(pin), sensor(&oneWire) {
            sensor.begin();
        }
    
        float getReadings() {
            sensor.requestTemperatures();
            
            if(settings.useFahreheit) {
                return calculateTemperature((sensor.getTempCByIndex(0) * 9) / 5 + 32.0);
            } else {
                return calculateTemperature(sensor.getTempCByIndex(0));
            }
        }

    private:
        //DHT sensor;
        OneWire oneWire;
        DallasTemperature sensor;
};

