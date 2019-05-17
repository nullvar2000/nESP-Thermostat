#include "userConfig.h"

#define READINGS_TO_COMPUTE_MEAN 6

class BaseTemperatureSensor {
  public:
    virtual float readTemperature() = 0;

    void setOffset(float offset) {
      _offset = offset;
    }
    float getOffset() {
      return _offset;
    }

    void setUseFahrenheit(bool useFahrenheit) {
      _useFahrenheit = useFahrenheit;
    }
    bool getUseFahrenheit() {
      return _useFahrenheit;
    }

  protected:
    bool _useFahrenheit;
    float _offset;
    float _readings[READINGS_TO_COMPUTE_MEAN];
    uint8_t _previousIndex = 255;

    float calculateTemperature(float currentReading) {
      // first reading only
      if(_previousIndex == 255) {
        for(uint8_t i = 0; i < READINGS_TO_COMPUTE_MEAN; i++) {
          _readings[i] = currentReading;
        }

        _previousIndex = 0;

        return currentReading;
      }

      float sum = 0.0;

      if(++_previousIndex >= READINGS_TO_COMPUTE_MEAN) {
        _previousIndex = 0;
      }

      for(uint8_t i = 0; i < READINGS_TO_COMPUTE_MEAN; i++) {
        sum += _readings[i];
      }

      return sum / READINGS_TO_COMPUTE_MEAN;
    }
};

#ifdef USE_BMP280
  #include "Sensor-BMP280.h"
#endif

#ifdef USE_DHT11
  #include "Sensor-DHT.h"

#endif

