#include <Arduino.h>

#include "settings.h"
#include "network.h"
#include "mqtt.h"
#include <TemperatureSensor.h>
#include <ThermostatControl.h>

#define NESP_UPDATE_INTERVAL 15000
#define EPin 255 // emergency heat
#define AuxPin 26 // 2nd stage heat
#define GPin 27 // fan 
#define OBPin 32 // reversing valve, set direction below
#define YPin 33 // compressor
#define LPin 255

// change to false if your reversing valve is powered for heating (B wire)
#define REVERSING_VALVE_POWERED_FOR_COOLING  true // O wire

// give sensors time to settle
unsigned long lastUpdate = millis(); 
//float currentTemp;
float targetTemp;
uint8_t mode;

ThermostatControl control(EPin, AuxPin, GPin, OBPin, YPin, true);

void setup() {
  Serial.begin(115200);
  
  delay(1000);
  
  prefs.begin(NESP_SETTINGS_NAMESPACE, false);

  loadSettings();
  configNet();

  if(configMQTT()) {
    Serial.println("MQTT configured");
  }

  configTempSensor();

  control.useFahrenheit(settings.useFahreheit);
  control.setMinMaxTargetTemp(settings.minTemperature, settings.maxTemperature);
  control.changeMode(mode);
  control.setTargetTemp(targetTemp);
}

void loop() {
  portal.handleClient();
  handleMQTT();

  if(lastUpdate + NESP_UPDATE_INTERVAL < millis()) {

    if(settings.sensor > 0) {
      float currentTemp = getReadings();

      control.updateCurrentTemp(currentTemp); 

      float updatedTargetTemp = control.getTargetTemp();

      if(targetTemp != updatedTargetTemp) {
        targetTemp = updatedTargetTemp;
        saveTargetTemp();
      }

      uint8_t updatedMode = control.getCurrentMainMode();

      if(mode != updatedMode) {
        mode = updatedMode;
        saveMode();
      }

      if(settings.mqtt.enabled) {
        publishMQTT(currentTemp, targetTemp, control.getCurrentMainModeName(), control.getCurrentActiveModeName());
      }
    }

    lastUpdate = millis();
  }
}

