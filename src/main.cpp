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
  ThermostatControl control(EPin, AuxPin, GPin, OBPin, YPin);
#endif

#ifdef ENABLE_BUTTONS
  OneButton modeButton(MODE_BUTTON_PIN, true);
  OneButton upButton(UP_BUTTON_PIN, true);
  OneButton downButton(DOWN_BUTTON_PIN, true);

  void modeButtonPress() {
      log("Changed mode to ");
      logln(control.rotateMode());
  }

  void upButtonPress() {
    float targetTemp = control.getTargetTemp();
    targetTemp += 1.0;
    log("Setting targetTemp to ");
    logln(targetTemp);
    control.setTargetTemp(targetTemp);
  }

  void downButtonPress() {
    float targetTemp = control.getTargetTemp();
    targetTemp -= 1.0;
    log("Setting targetTemp to ");
    logln(targetTemp);
    control.setTargetTemp(targetTemp);
  }
#endif

float targetTemp = 70.0;
float currentTemp = 0.0;
char* activeMode;
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

  control.changeMode("Off");
  activeMode = control.getCurrentActiveModeName();
  
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
    //log("Temperature: ");
    currentTemp = readTemperature();
    //logln(currentTemp);

    activeMode = control.updateCurrentTemp(currentTemp);
    /*log("Current mode: ");
    logln(control.getCurrentMainModeName());
    log("Target Temperature: ");
    logln(control.getTargetTemp());
    log("Currently active mode: ");
    logln(activeMode);*/

    #ifdef ENABLE_MQTT
      updateMqtt(currentTemp, control.getTargetTemp(), control.getCurrentMainModeName(), activeMode);
    #endif

    timeElapsed = 0;
  }

  #ifdef ENABLE_DISPLAY
    updateDisplay(currentTemp, control.getTargetTemp(), control.getCurrentMainModeName(), activeMode);
  #endif
}



