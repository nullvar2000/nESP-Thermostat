#include "userConfig.h"

#include "ThermostatControl.h"
#include "TemperatureSensor.h"
#include "Logger.h"

#define VERSION 0.1

#ifdef ENABLE_WIFI
  #include "IoTWC.h"
#endif

#ifdef HEATPUMP_HVAC_TYPE
  ThermostatControl control(HEATPUMP_HVAC_TYPE, USE_FAHRENHEIT, REVERSING_VALVE_POWERED_FOR_COOLING, EPin, AuxPin, GPin, OBPin, YPin);
#elif CONVENTIONAL_HVAC_TYPE
  ThermostatControl control(CONTENTIONAL_HVAC_TYPE, USE_FAHRENHEIT, REVERSING_VALVE_POWERED_FOR_COOLING, EPin, AuxPin, GPin, OBPin, YPin);
#endif

#ifdef ENABLE_BUTTONS
  #include "Buttons.h"
  Buttons buttons(&control);
#endif

#ifdef ENABLE_DISPLAY
  #include "Display.h"
  Display display(&control);
#endif

#ifdef ENABLE_PRESENCE_DETECTION
  uint8_t currentPresenceState = LOW;
  uint8_t previousPresenceState = LOW;
  unsigned long presenceCooldown = 0;
#endif

TemperatureSensor tempSensor(TEMP_SENSOR_PIN, 0.0);
float currentTemp = 0.0;
unsigned long nextUpdate = 0;

void setup() {
  if(LOG_LEVEL == 0) {
    Serial.begin(BAUD_RATE);
  }
  
  logln("Setting up");
  
  #ifdef ENABLE_WIFI
    initIoTWC(&control);
  #endif

  #ifdef DISABLE_OFF
    control.disableMode(DISABLE_OFF);
  #endif
  #ifdef DISABLE_AUTO
    control.disableMode(DISABLE_AUTO);
  #endif
  #ifdef DISABLE_COOL
    control.disableMode(DISABLE_COOL);
  #endif
  #ifdef DISABLE_HEAT
    control.disableMode(DISABLE_HEAT);
  #endif
  #ifdef DISABLE_EHEAT
    control.disableMode(DISABLE_EHEAT);
  #endif
  #ifdef DISABLE_FAN_ONLY
    control.disableMode(DISABLE_FAN_ONLY);
  #endif

  #ifdef ENABLE_LEDS
    control.setCoolLedPin(COOL_LED_PIN);
    control.setHeatLedPin(HEAT_LED_PIN);
    control.setFanLedPin(FAN_LED_PIN);
  #endif

  #ifdef ENABLE_PRESENCE_DETECTION
    pinMode(PRESENCE_PIN, INPUT);
  #endif

  tempSensor.setUseFahrenheit(USE_FAHRENHEIT);
  
  logln("Setup complete");
}

void loop() {

  #ifdef ENABLE_WIFI
    loopIoTWC();
  #endif

  #ifdef ENABLE_BUTTONS
    buttons.loop();
  #endif

  #ifdef ENABLE_PRESENCE_DETECTION
    currentPresenceState = digitalRead(PRESENCE_PIN);

    if(currentPresenceState == HIGH) {
      presenceCooldown = millis() + PRESENCE_COOLDOWN;
      if(previousPresenceState == LOW) {
        control.setPresence(true);
      }
    } else if(millis() > presenceCooldown && previousPresenceState == HIGH) {
      control.setPresence(false);
    }

    previousPresenceState = currentPresenceState;
  #endif

  if(millis() > nextUpdate) {
    currentTemp = tempSensor.readTemperature();

    Serial.println(currentTemp);

    control.updateCurrentTemp(currentTemp);

    #ifdef ENABLE_MQTT
      updateMqtt(currentTemp, control.getTargetTemp(), control.getCurrentMainModeName(), control.getCurrentActiveModeName());
    #endif

    nextUpdate = millis() + UPDATE_INTERVAL;

    #ifdef ENABLE_DISPLAY
      display.loop();
    #endif
  }
}

