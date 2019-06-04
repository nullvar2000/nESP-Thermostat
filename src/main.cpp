#include "userConfig.h"

#include "ThermostatControl.h"
#include "TemperatureSensor.h"
#include "Logger.h"

#define VERSION 0.1

#ifdef ENABLE_WIFI
  #include "IoTWC.h"
#endif

#ifdef ENABLE_BUTTONS
  #include <OneButton.h>
#endif

#ifdef ENABLE_DISPLAY
  #include "Display.h"
#endif

#ifdef HEATPUMP_HVAC_TYPE
  ThermostatControl control(HEATPUMP_HVAC_TYPE, USE_FAHRENHEIT, REVERSING_VALVE_POWERED_FOR_COOLING, EPin, AuxPin, GPin, OBPin, YPin);
#elif CONVENTIONAL_HVAC_TYPE
  ThermostatControl control(CONTENTIONAL_HVAC_TYPE, USE_FAHRENHEIT, REVERSING_VALVE_POWERED_FOR_COOLING, EPin, AuxPin, GPin, OBPin, YPin);
#endif

#ifdef ENABLE_BUTTONS
  OneButton modeButton(MODE_BUTTON_PIN, true);
  OneButton upButton(UP_BUTTON_PIN, true);
  OneButton downButton(DOWN_BUTTON_PIN, true);

  void modeButtonPress() {
    control.rotateMode();
  }

  void upButtonPress() {
    float targetTemp = control.getTargetTemp();
    targetTemp += INCREMENT_TEMP_STEP;
    control.setTargetTemp(targetTemp);
  }

  void downButtonPress() {
    float targetTemp = control.getTargetTemp();
    targetTemp -= INCREMENT_TEMP_STEP;
    control.setTargetTemp(targetTemp);
  }
#endif

TemperatureSensor tempSensor(TEMP_SENSOR_ONE_WIRE, USE_FAHRENHEIT, 0.0);
float currentTemp = 0.0;
unsigned long nextUpdate = 0;

#ifdef ENABLE_PRESENCE_DETECTION
  uint8_t currentPresenceState = LOW;
  uint8_t previousPresenceState = LOW;
  unsigned long presenceCooldown = 0;
#endif

void setup() {
  if(LOG_LEVEL == 0) {
    Serial.begin(BAUD_RATE);
  }
  
  logln("Setting up");
  
  #ifdef ENABLE_WIFI
    initIoTWC(&control);
  #endif

  #ifdef ENABLE_BUTTONS
    logln("Initializing buttons");
    modeButton.attachClick(modeButtonPress);
    upButton.attachClick(upButtonPress);
    downButton.attachClick(downButtonPress);
    logln("Complete");
  #endif

  #ifdef ENABLE_DISPLAY
    initDisplay();
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

  logln("Setup complete");
}

void loop() {

  #ifdef ENABLE_WIFI
    loopIoTWC();
  #endif

  #ifdef ENABLE_BUTTONS
    modeButton.tick();
    upButton.tick();
    downButton.tick();
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

    control.updateCurrentTemp(currentTemp);

    #ifdef ENABLE_MQTT
      updateMqtt(currentTemp, control.getTargetTemp(), control.getCurrentMainModeName(), control.getCurrentActiveModeName());
    #endif

    nextUpdate = millis() + UPDATE_INTERVAL;
  }

  #ifdef ENABLE_DISPLAY
    updateDisplay(currentTemp, control.getTargetTemp(), control.getCurrentMainModeDisplay(), control.getCurrentActiveModeDisplay());
  #endif
}



