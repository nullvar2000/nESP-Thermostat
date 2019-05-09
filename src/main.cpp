#include "userConfig.h"

#include "ThermostatControl.h"
#include "TemperatureSensor.h"
#include "Logger.h"

#include <elapsedMillis.h>

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
  ThermostatControl control(HEATPUMP_HVAC_TYPE, USE_FAHRENHEIT, EPin, AuxPin, GPin, OBPin, YPin);
#elif CONVENTIONAL_HVAC_TYPE
  ThermostatControl control(CONTENTIONAL_HVAC_TYPE, USE_FAHRENHEIT, EPin, AuxPin, GPin, OBPin, YPin);
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

float targetTemp = INITIAL_TARGET_TEMP;
float currentTemp = 0.0;
elapsedMillis timeElapsed = 0;

void setup() {
  if(LOG_LEVEL == 0) {
    Serial.begin(BAUD_RATE);
  }
  
  logln("Setting up");

  initSensor();
  
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

  if (timeElapsed > UPDATE_INTERVAL) {
    currentTemp = readTemperature();

    control.updateCurrentTemp(currentTemp);

    #ifdef ENABLE_MQTT
      updateMqtt(currentTemp, control.getTargetTemp(), control.getCurrentMainModeName(), control.getCurrentMainModeName());
    #endif

    timeElapsed = 0;
  }

  #ifdef ENABLE_DISPLAY
    updateDisplay(currentTemp, control.getTargetTemp(), control.getCurrentMainModeDisplay(), control.getCurrentActiveModeDisplay());
  #endif
}



