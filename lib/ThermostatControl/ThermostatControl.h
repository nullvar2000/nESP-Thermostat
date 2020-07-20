#ifndef THERMOSTATCONTROL_H
#define THERMOSTATCONTROL_H

#include <Arduino.h>

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

#define ON 0
#define OFF 1

#define NUMBER_OF_MAIN_MODES 6
#define NUMBER_OF_ACTIVE_MODES 6

#define COMPRESSOR_REST 180000
#define SECOND_STAGE_DIFFERENCE 2.0
#define FAN_TRAIL 45000
#define DEFAULT_SWING 1.0
#define MAX_SWING 1.5
#define MIN_SWING 0.5
#define AUTO_SWING 1.5
#define PRESENCE_SWING 2.0
#define DISABLED_LED 255

typedef struct activeModes_t {
  char name[10];
  char display[10];
  uint8_t ePinActivate;
  uint8_t auxPinActivate;
  uint8_t gPinActivate;
  uint8_t obPinActivate;
  uint8_t yPinActivate;
} ActiveModes;

typedef struct mainModes_t {
  char name[10];
  char display[10];
  bool enabled;
} MainModes;

class ThermostatControl {
    public:
        ThermostatControl(uint8_t e, uint8_t aux, uint8_t g, uint8_t ob, uint8_t y, bool reversingValveForCooling);

        void useFahrenheit(bool useF);
        void setMinMaxTargetTemp(float minTarget, float maxTarget);
        float getTargetTemp();
        void setTargetTemp(float target);
        float getSwing();
        void setSwing(float newSwing);
        bool getPresence();
        void setPresence(bool presence);
        float getCurrentTemp();

        char* updateCurrentTemp(float current);
        
        void getEnabledModeNames(char * names);
        char* getCurrentMainModeName();
        char* getCurrentActiveModeName();
        char* getCurrentMainModeDisplay();
        char* getCurrentActiveModeDisplay();
        uint8_t getCurrentMainMode();

        char* rotateMode();
        char* changeMode(const char *modeName);
        char* changeMode(uint8_t mode);
        void disableMode(uint8_t modeIndex);

        void setHeatLedPin(uint8_t pin);
        void setCoolLedPin(uint8_t pin);
        void setFanLedPin(uint8_t pin);

    private:
        ActiveModes activeModes[NUMBER_OF_ACTIVE_MODES];
        MainModes mainModes[NUMBER_OF_MAIN_MODES];

        uint8_t currentMainMode; // mode thermostat is in
        uint8_t currentActiveMode; // mode that is currently active

        uint8_t ePin;
        uint8_t auxPin;
        uint8_t gPin;
        uint8_t obPin;
        uint8_t yPin;
        uint8_t heatLedPin;
        uint8_t coolLedPin;
        uint8_t fanLedPin;

        bool _useFahrenheit;
        uint8_t _minTargetTemp;
        uint8_t _maxTargetTemp;
        float targetTemp;
        float currentTemp;
        float swing;
        bool presenceDetected;

        unsigned long lastOffTime;

        float calculateSwing();
        uint8_t activate(uint8_t mode);
        uint8_t deactivate();
};

#endif
