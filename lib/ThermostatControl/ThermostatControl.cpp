#include "Arduino.h"
#include "ThermostatControl.h"

ThermostatControl::ThermostatControl(uint8_t e, uint8_t aux, uint8_t g, uint8_t ob, uint8_t y, bool reversingValveForCooling) {

  activeModes[OFF_MODE].ePinActivate = OFF;
  activeModes[OFF_MODE].auxPinActivate = OFF;
  activeModes[OFF_MODE].gPinActivate = OFF;
  activeModes[OFF_MODE].obPinActivate = OFF;
  activeModes[OFF_MODE].yPinActivate = OFF;
  strlcpy(activeModes[OFF_MODE].name,"off",sizeof(activeModes[OFF_MODE].name));
  strlcpy(activeModes[OFF_MODE].display,"Off",sizeof(activeModes[OFF_MODE].display));

  activeModes[COOL_MODE].ePinActivate = OFF;
  activeModes[COOL_MODE].auxPinActivate = OFF;
  activeModes[COOL_MODE].gPinActivate = ON;
  if(reversingValveForCooling) {
    activeModes[COOL_MODE].obPinActivate = ON;
  } else {
    activeModes[COOL_MODE].obPinActivate = OFF;
  }
  activeModes[COOL_MODE].yPinActivate = ON;
  strlcpy(activeModes[COOL_MODE].name,"cool",sizeof(activeModes[COOL_MODE].name));
  strlcpy(activeModes[COOL_MODE].display,"Cool",sizeof(activeModes[COOL_MODE].display));

  activeModes[HEAT_MODE].ePinActivate = OFF;
  activeModes[HEAT_MODE].auxPinActivate = OFF;
  activeModes[HEAT_MODE].gPinActivate = ON;
  if(!reversingValveForCooling) {
    activeModes[HEAT_MODE].obPinActivate = ON;
  } else {
    activeModes[HEAT_MODE].obPinActivate = OFF;
  }
  activeModes[HEAT_MODE].yPinActivate = ON;
  strlcpy(activeModes[HEAT_MODE].name,"heat",sizeof(activeModes[HEAT_MODE].name));
  strlcpy(activeModes[HEAT_MODE].display,"Heat",sizeof(activeModes[HEAT_MODE].display));

  activeModes[EHEAT_MODE].ePinActivate = ON;
  activeModes[EHEAT_MODE].auxPinActivate = OFF;
  activeModes[EHEAT_MODE].gPinActivate = ON;
  activeModes[EHEAT_MODE].obPinActivate = OFF;
  activeModes[EHEAT_MODE].yPinActivate = OFF;
  strlcpy(activeModes[EHEAT_MODE].name,"eheat",sizeof(activeModes[EHEAT_MODE].name));
  strlcpy(activeModes[EHEAT_MODE].display,"EM Heat",sizeof(activeModes[EHEAT_MODE].display));

  activeModes[SS_HEAT_MODE].ePinActivate = OFF;
  activeModes[SS_HEAT_MODE].auxPinActivate = ON;
  activeModes[SS_HEAT_MODE].gPinActivate = ON;
  if(!reversingValveForCooling) {
    activeModes[SS_HEAT_MODE].obPinActivate = ON;
  } else {
    activeModes[SS_HEAT_MODE].obPinActivate = OFF;
  }
  activeModes[SS_HEAT_MODE].yPinActivate = ON;
  strlcpy(activeModes[SS_HEAT_MODE].name,"heat",sizeof(activeModes[SS_HEAT_MODE].name));
  strlcpy(activeModes[SS_HEAT_MODE].display,"Heat x2",sizeof(activeModes[SS_HEAT_MODE].display));

  activeModes[FAN_MODE].ePinActivate = OFF;
  activeModes[FAN_MODE].auxPinActivate = OFF;
  activeModes[FAN_MODE].gPinActivate = ON;
  activeModes[FAN_MODE].obPinActivate = OFF;
  activeModes[FAN_MODE].yPinActivate = OFF;
  strlcpy(activeModes[FAN_MODE].name,"fan_only",sizeof(activeModes[FAN_MODE].name));
  strlcpy(activeModes[FAN_MODE].display,"Fan",sizeof(activeModes[FAN_MODE].display));

  mainModes[MAIN_OFF].enabled = true;
  strlcpy(mainModes[MAIN_OFF].name,"off",sizeof(mainModes[MAIN_OFF].name));
  strlcpy(mainModes[MAIN_OFF].display,"Off",sizeof(mainModes[MAIN_OFF].display));

  mainModes[MAIN_AUTO].enabled = true;
  strlcpy(mainModes[MAIN_AUTO].name,"auto",sizeof(mainModes[MAIN_AUTO].name));
  strlcpy(mainModes[MAIN_AUTO].display,"Auto",sizeof(mainModes[MAIN_AUTO].display));

  mainModes[MAIN_COOL].enabled = true;
  strlcpy(mainModes[MAIN_COOL].name,"cool",sizeof(mainModes[MAIN_COOL].name));
  strlcpy(mainModes[MAIN_COOL].display,"Cool",sizeof(mainModes[MAIN_COOL].display));

  mainModes[MAIN_HEAT].enabled = true;
  strlcpy(mainModes[MAIN_HEAT].name,"heat",sizeof(mainModes[MAIN_HEAT].name));
  strlcpy(mainModes[MAIN_HEAT].display,"Heat",sizeof(mainModes[MAIN_HEAT].display));

  mainModes[MAIN_EHEAT].enabled = false;
  strlcpy(mainModes[MAIN_EHEAT].name,"eheat",sizeof(mainModes[MAIN_EHEAT].name));
  strlcpy(mainModes[MAIN_EHEAT].display,"Heat",sizeof(mainModes[MAIN_EHEAT].display));

  mainModes[MAIN_FAN].enabled = true;
  strlcpy(mainModes[MAIN_FAN].name,"fan_only",sizeof(mainModes[MAIN_FAN].name));
  strlcpy(mainModes[MAIN_FAN].display,"Fan",sizeof(mainModes[MAIN_FAN].display));

  _useFahrenheit = false;
  _minTargetTemp = 10;
  _maxTargetTemp = 30;
  
  ePin = e; // emergency heat
  auxPin = aux; // 2nd stage heat
  gPin = g; // fan
  obPin = ob; // reversing valve
  yPin = y; // compressor
  
  currentMainMode = OFF_MODE;
  currentActiveMode = OFF_MODE;
  lastOffTime = 0;
  currentTemp = targetTemp;
  swing = DEFAULT_SWING;
  presenceDetected = true;
  heatLedPin = DISABLED_LED;
  coolLedPin = DISABLED_LED;
  fanLedPin = DISABLED_LED;

  // set pins to high to keep relays off at boot up
  digitalWrite(ePin, HIGH);
  digitalWrite(auxPin, HIGH);
  digitalWrite(gPin, HIGH);
  digitalWrite(obPin, HIGH);
  digitalWrite(yPin, HIGH);

  pinMode(ePin, OUTPUT);
  pinMode(auxPin, OUTPUT);
  pinMode(gPin, OUTPUT);
  pinMode(obPin, OUTPUT);
  pinMode(yPin, OUTPUT);
}

void ThermostatControl::useFahrenheit(bool useF) {
    _useFahrenheit = useF;
}

void ThermostatControl::setMinMaxTargetTemp(float minTarget, float maxTarget) {
    _minTargetTemp = minTarget;
    _maxTargetTemp = maxTarget;
}

float ThermostatControl::getTargetTemp() {
  return targetTemp;
}

void ThermostatControl::setTargetTemp(float target) {
  if(target < _minTargetTemp) {
    targetTemp = _minTargetTemp;
  } else if(target > _maxTargetTemp) {
    targetTemp = _maxTargetTemp;
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

float ThermostatControl::getCurrentTemp() {
  return currentTemp;
}

void ThermostatControl::getEnabledModeNames(char * names) {
  for(int i = 0; i < NUMBER_OF_MAIN_MODES; i++) {
    if(!mainModes[i].enabled) {
      continue;
    }

    if(i == 0) {
      sprintf(names, "'%s'", mainModes[i].name);
    } else {
      sprintf(names, "%s,'%s'", names, mainModes[i].name);
    }
  }
  sprintf(names, "%s", names);
}

char* ThermostatControl::getCurrentMainModeName() {
  return mainModes[currentMainMode].name;
}

char* ThermostatControl::getCurrentActiveModeName() {
  return activeModes[currentActiveMode].name;
}

char* ThermostatControl::getCurrentMainModeDisplay() {
  return mainModes[currentMainMode].display;
}

char* ThermostatControl::getCurrentActiveModeDisplay() {
  return activeModes[currentActiveMode].display;
}

uint8_t ThermostatControl::getCurrentMainMode() {
  return currentMainMode;
}

char* ThermostatControl::rotateMode() {
  do {
    currentMainMode++;
    if(currentMainMode >= NUMBER_OF_MAIN_MODES) {
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

char* ThermostatControl::changeMode(uint8_t mode) {
  if(mode >= 0 && mode < NUMBER_OF_MAIN_MODES && mainModes[mode].enabled) {
    currentMainMode = mode;
    currentActiveMode = deactivate();
  }

  return mainModes[currentMainMode].name;
}

char* ThermostatControl::updateCurrentTemp(float current) {
  currentTemp = current;
  
  if(currentMainMode == MAIN_OFF) {
    activate(OFF_MODE);
  } else if(currentMainMode == MAIN_FAN) {
    activate(FAN_MODE);
  } else if(currentMainMode == MAIN_AUTO) {
    if(currentTemp < targetTemp - calculateSwing()) {
      if(currentTemp < targetTemp - SECOND_STAGE_DIFFERENCE) {
        activate(SS_HEAT_MODE);
      } else {
        activate(HEAT_MODE);
      }
    } else if(currentTemp > targetTemp + calculateSwing()) {
      activate(COOL_MODE);
    } else if(currentActiveMode == COOL_MODE && currentTemp < targetTemp) {
      deactivate();
    } else if((currentActiveMode == HEAT_MODE || currentActiveMode == SS_HEAT_MODE) && currentTemp > targetTemp) {
      deactivate();
    }
  } else if(currentMainMode == MAIN_COOL) {
    if(currentTemp > targetTemp + calculateSwing()) activate(COOL_MODE);
    if(currentTemp < targetTemp - calculateSwing()) deactivate();
  } else if(currentMainMode == MAIN_HEAT) {
    if(currentTemp < targetTemp - SECOND_STAGE_DIFFERENCE) {
      activate(SS_HEAT_MODE);
    } else if(currentTemp < targetTemp - calculateSwing()) { 
      activate(HEAT_MODE);
    }
    if(currentTemp > targetTemp + calculateSwing()) deactivate();
  } else if(currentMainMode == MAIN_EHEAT) {
    if(currentTemp < targetTemp - calculateSwing()) activate(EHEAT_MODE);
    if(currentTemp > targetTemp + calculateSwing()) deactivate();
  }
  
  // turn off fan after trailing time
  if(currentMainMode != MAIN_FAN && currentActiveMode == FAN_MODE && millis() > lastOffTime + FAN_TRAIL) {
    digitalWrite(gPin, OFF);
    currentActiveMode = OFF_MODE;

    if(fanLedPin != DISABLED_LED) {
      digitalWrite(fanLedPin, LOW);
    }
  }

  return activeModes[currentActiveMode].name;
}

void ThermostatControl::disableMode(uint8_t modeIndex) {
  if(modeIndex >= 0 && modeIndex < 6) {
    mainModes[modeIndex].enabled = false;
  }
}

void ThermostatControl::setHeatLedPin(uint8_t pin) {
  heatLedPin = pin;
  pinMode(heatLedPin, OUTPUT);
}

void ThermostatControl::setCoolLedPin(uint8_t pin) {
  coolLedPin = pin;
  pinMode(coolLedPin, OUTPUT);
}

void ThermostatControl::setFanLedPin(uint8_t pin) {
  fanLedPin = pin;
  pinMode(fanLedPin, OUTPUT);
}

float ThermostatControl::calculateSwing() {
  uint8_t additionalSwing = 0.0;

  if(!presenceDetected) {
    additionalSwing = PRESENCE_SWING;
  }

  if(currentMainMode == MAIN_AUTO) {
    return AUTO_SWING + additionalSwing;
  } else {
    return swing + additionalSwing;
  }
}

uint8_t ThermostatControl::activate(uint8_t mode) {
  if(mode != currentActiveMode || mode == OFF_MODE) {
    deactivate();
  }
  
  // compressor has not had enough rest
  if(millis() < lastOffTime + COMPRESSOR_REST) { 
    return -1;
  }

  digitalWrite(ePin, activeModes[mode].ePinActivate);
  digitalWrite(auxPin, activeModes[mode].auxPinActivate);
  digitalWrite(gPin, activeModes[mode].gPinActivate);
  digitalWrite(obPin, activeModes[mode].obPinActivate);
  digitalWrite(yPin, activeModes[mode].yPinActivate);
  
  if(mode == COOL_MODE && coolLedPin != DISABLED_LED) {
    digitalWrite(coolLedPin, HIGH);
  }
  
  if((mode == HEAT_MODE || mode == EHEAT_MODE || mode == SS_HEAT_MODE) && heatLedPin != DISABLED_LED) {
    digitalWrite(heatLedPin, HIGH);
  }

  if(mode == FAN_MODE && fanLedPin != DISABLED_LED) {
    digitalWrite(fanLedPin, HIGH);
  }

  currentActiveMode = mode;

  return currentActiveMode;
}

uint8_t ThermostatControl::deactivate() {
  if(currentActiveMode == HEAT_MODE || currentActiveMode == EHEAT_MODE || currentActiveMode == SS_HEAT_MODE || currentActiveMode == COOL_MODE) {
    lastOffTime = millis();
    currentActiveMode = FAN_MODE;

    if(coolLedPin != DISABLED_LED) {
      digitalWrite(coolLedPin, LOW);
    }

    if(heatLedPin != DISABLED_LED) {
      digitalWrite(heatLedPin, LOW);
    }

    if(fanLedPin != DISABLED_LED) {
      digitalWrite(fanLedPin, HIGH);
    }

  } else {
    currentActiveMode = OFF_MODE;
    digitalWrite(gPin, OFF); // fan

    if(fanLedPin != DISABLED_LED) {
      digitalWrite(fanLedPin, LOW);
    }
  }

  digitalWrite(ePin, OFF); // emergency heat
  digitalWrite(auxPin, OFF); // 2nd stage heat
  // leave fan on for trailing time
  //digitalWrite(gPin, LOW); // fan
  digitalWrite(obPin, OFF); // reversing valve, high for cool
  digitalWrite(yPin, OFF); // compressor

  return currentActiveMode;
}
