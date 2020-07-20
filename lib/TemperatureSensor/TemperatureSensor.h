#ifndef TEMPERATURESENSOR_H
#define TEMPERATURESENSOR_H

#include <OneWire.h>
#include <DallasTemperature.h>

#include "settings.h"

OneWire oneWire(13);
DallasTemperature tempSensor(&oneWire);

void configTempSensor() {
  tempSensor.begin();
}

float getReadings() {
  tempSensor.requestTemperatures();

  if(settings.useFahreheit) {
    return tempSensor.getTempFByIndex(0);
  } else {
    return tempSensor.getTempCByIndex(0);
  }
}

#endif