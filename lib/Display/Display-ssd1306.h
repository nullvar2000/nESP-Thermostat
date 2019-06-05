#ifndef DISPLAY_SSD1306_H
    #define DISPLAY_SSD1306_H

    #include <U8g2lib.h>

    #include "userConfig.h"

    class Display : public BaseDisplay {
        public:
            Display(ThermostatControl* thermostatControl) {
                _control = thermostatControl;
                _display.begin();
                _display.setFont(u8g2_font_profont12_mr);
            }

            void loop() {
                char tempStr[8];
                char line[24];
                
                _display.clearBuffer();
                
                dtostrf(_control->getCurrentTemp(), 5, 1, tempStr);
                strcpy(line, "Current Temp: ");
                strcat(line, tempStr);
                _display.drawStr(0, 12, line);
                
                dtostrf(_control->getTargetTemp(), 5, 1, tempStr);
                strcpy(line, "Target Temp: ");
                strcat(line, tempStr);
                _display.drawStr(0, 24, line);
                
                strcpy(line, "Current mode: ");
                strcat(line, _control->getCurrentMainModeDisplay());
                _display.drawStr(0, 36, line);
                
                strcpy(line, "Active mode: ");
                strcat(line, _control->getCurrentActiveModeDisplay();
                _display.drawStr(0, 48, line);
                
                _display.sendBuffer();
            }
        
        private:
            U8G2_SSD1306_128X64_NONAME_F_SW_I2C _display(U8G2_R0, I2C_SCL, I2C_SDA, U8X8_PIN_NONE);
    }

#endif
