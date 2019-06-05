#ifndef DISPLAY_ILI9341_H
    #define DISPLAY_ILI9341_H

    #include <TFT_eSPI.h>

    #include "userConfig.h"
    #include "ThermostatControl.h"

    class Display : public BaseDisplay {
        public:
            Display(ThermostatControl* thermostatControl) {
                _control = thermostatControl;
                _display.init();
                _display.setRotation(3);
                _display.setTextColor(TFT_CYAN);
                _display.setTextSize(1);
            }
        
            void loop() {
                _display.fillScreen(TFT_DARKGREY);
                _display.setCursor(0,0);

                _display.print("Current Temp: ");
                _display.println(_control->getCurrentTemp());
                _display.print("Target Temp: ");
                _display.println(_control->getTargetTemp());
                _display.print("Current Mode: ");
                _display.println(_control->getCurrentMainModeDisplay());
                _display.print("Active Mode: ");
                _display.println(_control->getCurrentActiveModeDisplay());
            }

        private:
            TFT_eSPI _display = TFT_eSPI();
    };

#endif
