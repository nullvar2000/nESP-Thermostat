#ifndef DISPLAY_ILI9341_H
    #define DISPLAY_ILI9341_H

    #include <TFT_eSPI.h>

    #include "userConfig.h"

    class Display : public BaseDisplay {
        public:
            Display() {
                display.init();
                display.setRotation(3);
                display.setTextColor(TFT_CYAN);
                display.setTextSize(1);
            }
        
            void loop(float ctemp, float ttemp, char* cmode, char* amode) {
                display.fillScreen(TFT_DARKGREY);
                display.setCursor(0,0);

                display.print("Current Temp: ");
                display.println(ctemp);
                display.print("Target Temp: ");
                display.println(ttemp);
                display.print("Current Mode: ");
                display.println(cmode);
                display.print("Active Mode: ");
                display.println(amode);
            }

        private:
            TFT_eSPI display = TFT_eSPI();
    };

#endif
