#ifndef BUTTONS_H
    #define BUTTONS_H

    #include <OneButton.h>

    #include "userConfig.h"
    #include "ThermostatControl.h"

    class Buttons {

        public:
            Buttons(ThermostatControl* control) {
                _control = control;

                modeButton.attachClick(modeButtonPress);
                upButton.attachClick(upButtonPress);
                downButton.attachClick(downButtonPress);
            }

            void loop() {
                modeButton.tick();
                upButton.tick();
                downButton.tick();
            }

        private:

            ThermostatControl* _control;
            
            OneButton modeButton(MODE_BUTTON_PIN, true);
            OneButton upButton(UP_BUTTON_PIN, true);
            OneButton downButton(DOWN_BUTTON_PIN, true);

            void modeButtonPress() {
                _control->rotateMode();
            }

            void upButtonPress() {
                float targetTemp = _control->getTargetTemp();
                targetTemp += INCREMENT_TEMP_STEP;
                _control->setTargetTemp(targetTemp);
            }

            void downButtonPress() {
                float targetTemp = _control->getTargetTemp();
                targetTemp -= INCREMENT_TEMP_STEP;
                _control->setTargetTemp(targetTemp);
            }
    };

#endif
