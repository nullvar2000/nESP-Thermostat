#ifndef DISPLAY_ILI9341_H
    #define DISPLAY_ILI9341_H

    #include "FS.h"
    #include <TFT_eSPI.h>

    #include "userConfig.h"
    #include "ThermostatControl.h"

    #define CALIBRATION_FILE "/TouchCalData2"
    #define REPEAT_CAL false

    class Display : public BaseDisplay {
        public:
            Display(ThermostatControl* thermostatControl) {
                _control = thermostatControl;
                _display.init();
                _display.setRotation(1);

                //touch_calibrate();

                
            }
        
            void loop() {
                _display.fillScreen(TFT_BLACK);
                _display.setCursor(0,0);
                _display.setTextColor(TFT_CYAN);
                _display.setTextSize(2);

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
            //TFT_eSPI_Button _modeButton;
            //TFT_eSPI_Button _upButton;
            //TFT_eSPI_Button _downButton;

            /*void touch_calibrate() {
                uint16_t calData[5];
                uint8_t calDataOK = 0;

                // check file system exists
                if (!SPIFFS.begin()) {
                    Serial.println("Formating file system");
                    SPIFFS.format();
                    SPIFFS.begin();
                }

                // check if calibration file exists and size is correct
                if (SPIFFS.exists(CALIBRATION_FILE)) {
                    if (REPEAT_CAL) {
                        // Delete if we want to re-calibrate
                        SPIFFS.remove(CALIBRATION_FILE);
                    } else {
                        File f = SPIFFS.open(CALIBRATION_FILE, "r");

                        if (f) {
                            if (f.readBytes((char *)calData, 14) == 14)
                                calDataOK = 1;
                            f.close();
                        }
                    }
                }

                if (calDataOK && !REPEAT_CAL) {
                    // calibration data valid
                    _display.setTouch(calData);
                } else {
                    // data not valid so recalibrate
                    _display.fillScreen(TFT_BLACK);
                    _display.setCursor(20, 0);
                    _display.setTextFont(2);
                    _display.setTextSize(1);
                    _display.setTextColor(TFT_WHITE, TFT_BLACK);

                    _display.println("Touch corners as indicated");

                    _display.setTextFont(1);
                    _display.println();

                    if (REPEAT_CAL) {
                        _display.setTextColor(TFT_RED, TFT_BLACK);
                        _display.println("Set REPEAT_CAL to false to stop this running again!");
                    }

                    _display.calibrateTouch(calData, TFT_MAGENTA, TFT_BLACK, 15);

                    _display.setTextColor(TFT_GREEN, TFT_BLACK);
                    _display.println("Calibration complete!");

                    // store data
                    File f = SPIFFS.open(CALIBRATION_FILE, "w");
                    
                    if (f) {
                        f.write((const unsigned char *)calData, 14);
                        f.close();
                    }
                }
            }*/
    };

#endif
