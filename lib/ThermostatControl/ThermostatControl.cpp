#include "Arduino.h"
#include "ThermostatControl.h"

ThermostatControl::ThermostatControl(uint8_t e, uint8_t aux, uint8_t g, uint8_t ob, uint8_t y) {

    activeModes[OFF_MODE].ePinActivate = 0;
    activeModes[OFF_MODE].auxPinActivate = 0;
    activeModes[OFF_MODE].gPinActivate = 0;
    activeModes[OFF_MODE].obPinActivate = 0;
    activeModes[OFF_MODE].yPinActivate = 0;
    strlcpy(activeModes[OFF_MODE].name,"Off",sizeof(activeModes[OFF_MODE].name));
    
    activeModes[COOL_MODE].ePinActivate = 0;
    activeModes[COOL_MODE].auxPinActivate = 0;
    activeModes[COOL_MODE].gPinActivate = 1;
    activeModes[COOL_MODE].obPinActivate = 0;
    activeModes[COOL_MODE].yPinActivate = 1;
    strlcpy(activeModes[COOL_MODE].name,"Cool",sizeof(activeModes[COOL_MODE].name));

    activeModes[HEAT_MODE].ePinActivate = 0;
    activeModes[HEAT_MODE].auxPinActivate = 0;
    activeModes[HEAT_MODE].gPinActivate = 1;
    activeModes[HEAT_MODE].obPinActivate = 1;
    activeModes[HEAT_MODE].yPinActivate = 1;
    strlcpy(activeModes[HEAT_MODE].name,"Heat",sizeof(activeModes[HEAT_MODE].name));

    activeModes[EHEAT_MODE].ePinActivate = 1;
    activeModes[EHEAT_MODE].auxPinActivate = 1;
    activeModes[EHEAT_MODE].gPinActivate = 1;
    activeModes[EHEAT_MODE].obPinActivate = 1;
    activeModes[EHEAT_MODE].yPinActivate = 0;
    strlcpy(activeModes[EHEAT_MODE].name,"EHeat",sizeof(activeModes[EHEAT_MODE].name));

    activeModes[SS_HEAT_MODE].ePinActivate = 0;
    activeModes[SS_HEAT_MODE].auxPinActivate = 1;
    activeModes[SS_HEAT_MODE].gPinActivate = 1;
    activeModes[SS_HEAT_MODE].obPinActivate = 1;
    activeModes[SS_HEAT_MODE].yPinActivate = 1;
    strlcpy(activeModes[SS_HEAT_MODE].name,"Heat",sizeof(activeModes[SS_HEAT_MODE].name));

    activeModes[FAN_MODE].ePinActivate = 0;
    activeModes[FAN_MODE].auxPinActivate = 0;
    activeModes[FAN_MODE].gPinActivate = 1;
    activeModes[FAN_MODE].obPinActivate = 0;
    activeModes[FAN_MODE].yPinActivate = 0;
    strlcpy(activeModes[FAN_MODE].name,"Fan",sizeof(activeModes[FAN_MODE].name));

    mainModes[MAIN_OFF].enabled = true;
    strlcpy(mainModes[MAIN_OFF].name,"Off",sizeof(mainModes[MAIN_OFF].name));

    mainModes[MAIN_AUTO].enabled = true;
    strlcpy(mainModes[MAIN_AUTO].name,"Auto",sizeof(mainModes[MAIN_AUTO].name));

    mainModes[MAIN_COOL].enabled = true;
    strlcpy(mainModes[MAIN_COOL].name,"Cool",sizeof(mainModes[MAIN_COOL].name));

    mainModes[MAIN_HEAT].enabled = true;
    strlcpy(mainModes[MAIN_HEAT].name,"Heat",sizeof(mainModes[MAIN_HEAT].name));

    mainModes[MAIN_EHEAT].enabled = true;
    strlcpy(mainModes[MAIN_EHEAT].name,"EHeat",sizeof(mainModes[MAIN_EHEAT].name));

    mainModes[MAIN_FAN].enabled = true;
    strlcpy(mainModes[MAIN_FAN].name,"Fan",sizeof(mainModes[MAIN_FAN].name));

    ePin = e; // emergency heat
    auxPin = aux; // 2nd stage heat
    gPin = g; // fan
    obPin = ob; // reversing valve, on for cool, off for heat
    yPin = y; // compressor
    
    currentMainMode = OFF_MODE;
    currentActiveMode = OFF_MODE;
    lastOffTime = 0;
    targetTemp = DEFAULT_TARGET_TEMP;
    currentTemp = DEFAULT_TARGET_TEMP;
    swing = DEFAULT_SWING;
    presenceDetected = true;

    pinMode(ePin, OUTPUT);
    pinMode(auxPin, OUTPUT);
    pinMode(gPin, OUTPUT);
    pinMode(obPin, OUTPUT);
    pinMode(yPin, OUTPUT);
}

float ThermostatControl::getTargetTemp() {
  return targetTemp;
}

void ThermostatControl::setTargetTemp(float target) {
  if(target < MIN_TARGET_TEMP) {
    targetTemp = MIN_TARGET_TEMP;
  } else if(target > MAX_TARGET_TEMP) {
    targetTemp = MAX_TARGET_TEMP;
  } else {
    targetTemp = target;
  }
}

float ThermostatControl::getSwing() {
  return swing;
}

void ThermostatControl::setSwing(float newSwing) {
  if(newSwing < MIN_SWING) {
    swing = MIN_SWING;
  } else if(newSwing > MAX_SWING) {
    swing = MAX_SWING;
  } else {
    swing = newSwing;
  }
}

bool ThermostatControl::getPresence() {
  return presenceDetected;
}

void ThermostatControl::setPresence(bool presence) {
  presenceDetected = presence;
}

char* ThermostatControl::getCurrentMainModeName() {
  return mainModes[currentMainMode].name;
}

char* ThermostatControl::getCurrentActiveModeName() {
  return activeModes[currentActiveMode].name;
}

char* ThermostatControl::rotateMode() {
  do {
    currentMainMode++;
    if(currentMainMode > NUMBER_OF_MAIN_MODES) {
      currentMainMode = 0;
    }
  } while (!mainModes[currentMainMode].enabled);

  currentActiveMode = deactivate();

  return mainModes[currentMainMode].name;
}

char* ThermostatControl::changeMode(const char *modeName) {
  for(int i = 0; i < NUMBER_OF_MAIN_MODES; i++) {
    if(strcmp(mainModes[i].name, modeName) == 0 && mainModes[i].enabled) {
      currentMainMode = i;
      break;
    }
  }

  currentActiveMode = deactivate();

  return mainModes[currentMainMode].name;
}

char* ThermostatControl::updateCurrentTemp(float current) {

  currentTemp = current;
  
  if(currentMainMode == MAIN_OFF) {
    activate(OFF_MODE);
  } else if(currentMainMode == MAIN_FAN) {
    activate(FAN_MODE);
  } else if(currentMainMode == MAIN_AUTO) {
    if(currentTemp < targetTemp - AUTO_SWING) {
      activate(HEAT_MODE);
    } else if(currentTemp > targetTemp + AUTO_SWING) {
      activate(COOL_MODE);
    } else {
      deactivate();
    }
  } else if(currentMainMode == MAIN_COOL) {
    if(currentTemp > targetTemp + calculateSwing()) activate(COOL_MODE);
    if(currentTemp < targetTemp - calculateSwing()) deactivate();
  } else if(currentMainMode == MAIN_HEAT) {
    if(currentTemp < targetTemp - SECOND_STAGE_DIFFERENCE) activate(SS_HEAT_MODE);
    if(currentTemp < targetTemp - calculateSwing()) activate(HEAT_MODE);
    if(currentTemp > targetTemp + calculateSwing()) deactivate();
  } else if(currentMainMode == MAIN_EHEAT) {
    if(currentTemp < targetTemp - calculateSwing()) activate(EHEAT_MODE);
    if(currentTemp > targetTemp + calculateSwing()) deactivate();
  }
  
  // turn off fan after trailing time
  if(currentMainMode != MAIN_FAN && currentActiveMode == FAN_MODE && lastOffTime > FAN_TRAIL) {
    digitalWrite(gPin, LOW);
    currentActiveMode = OFF_MODE;
  }

  return activeModes[currentActiveMode].name;
}

void ThermostatControl::disableMode(uint8_t modeIndex) {
  if(modeIndex >= 0 && modeIndex < 6) {
    mainModes[modeIndex].enabled = false;
  }
}

float ThermostatControl::calculateSwing() {
  if(presenceDetected) {
    return swing;
  } else {
    return swing + PRESENCE_SWING;
  }
}

uint8_t ThermostatControl::activate(uint8_t mode) {

  if(mode != currentActiveMode || mode == OFF_MODE) {
    deactivate();
  }
  
  // compressor has not had enough rest
  if(lastOffTime < COMPRESSOR_REST) { 
    return -1;
  }

  digitalWrite(ePin, activeModes[mode].ePinActivate);
  digitalWrite(auxPin, activeModes[mode].auxPinActivate);
  digitalWrite(gPin, activeModes[mode].gPinActivate);
  digitalWrite(obPin, activeModes[mode].obPinActivate);
  digitalWrite(yPin, activeModes[mode].yPinActivate);
  
  currentActiveMode = mode;

  return currentActiveMode;
}

uint8_t ThermostatControl::deactivate() {

  if(currentActiveMode == HEAT_MODE || currentActiveMode == EHEAT_MODE || currentActiveMode == COOL_MODE) {
    lastOffTime = 0;
    currentActiveMode = FAN_MODE;
  } else {
    currentActiveMode = OFF_MODE;
    digitalWrite(gPin, LOW); // fan
  }

  digitalWrite(ePin, LOW); // emergency heat
  digitalWrite(auxPin, LOW); // 2nd stage heat
  // leave fan on for trailing time
  //digitalWrite(gPin, HIGH); // fan
  digitalWrite(obPin, LOW); // reversing valve, high for cool
  digitalWrite(yPin, LOW); // compressor

  return currentActiveMode;
}
