#ifndef THERMOSTATCONTROL_H
  #define THERMOSTATCONTROL_H

  #include <elapsedMillis.h>

  #define OFF_MODE 0
  #define COOL_MODE 1
  #define HEAT_MODE 2
  #define EHEAT_MODE 3
  #define SS_HEAT_MODE 4
  #define FAN_MODE 5

  #define MAIN_OFF 0
  #define MAIN_AUTO 1
  #define MAIN_COOL 2
  #define MAIN_HEAT 3
  #define MAIN_EHEAT 4
  #define MAIN_FAN 5

  #define NUMBER_OF_MAIN_MODES 6
  #define NUMBER_OF_ACTIVE_MODES 6

  #define COMPRESSOR_REST 20000
  #define MAX_TARGET_TEMP 80.0
  #define MIN_TARGET_TEMP 60.0
  #define DEFAULT_TARGET_TEMP 72.0
  #define SECOND_STAGE_DIFFERENCE 1.5
  #define FAN_TRAIL 20000
  #define DEFAULT_SWING 0.50
  #define MAX_SWING 1.4
  #define MIN_SWING 0.5
  #define AUTO_SWING 2.0
  #define PRESENCE_SWING 2.0

  struct ActiveMode {
    char name[8];
    uint8_t ePinActivate;
    uint8_t auxPinActivate;
    uint8_t gPinActivate;
    uint8_t obPinActivate;
    uint8_t yPinActivate;
  };

  struct MainMode {
    char name[8];
    bool enabled;
  };

  class ThermostatControl {
    public:
      ThermostatControl(uint8_t e, uint8_t aux, uint8_t g, uint8_t ob, uint8_t y);

      float getTargetTemp();
      void setTargetTemp(float target);
      float getSwing();
      void setSwing(float newSwing);
      bool getPresence();
      void setPresence(bool presence);
      char* getCurrentMainModeName();
      char* getCurrentActiveModeName();

      char* rotateMode();
      char* changeMode(const char *modeName);
      char* updateCurrentTemp(float current);

      void disableMode(uint8_t modeIndex);

    private:
      ActiveMode activeModes[NUMBER_OF_ACTIVE_MODES];
      MainMode mainModes[NUMBER_OF_MAIN_MODES];

      uint8_t currentMainMode; // mode thermostat is in 
      uint8_t currentActiveMode; // mode that is currently active
      
      uint8_t ePin;
      uint8_t auxPin;
      uint8_t gPin;
      uint8_t obPin;
      uint8_t yPin;

      float targetTemp;
      float currentTemp;
      float swing;
      bool presenceDetected;

      elapsedMillis lastOffTime;

      float calculateSwing();
      uint8_t activate(uint8_t mode);
      uint8_t deactivate();
  };

#endif
