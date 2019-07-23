#ifndef DISPLAY_H
  #define DISPLAY_H

  #include "userConfig.h"

  class BaseDisplay {
      public:
        virtual void loop() = 0;

      protected:
        ThermostatControl* _control;
    };

  #ifdef USE_SSD1306
    #include "Display-ssd1306.h"
  #endif

  #ifdef USE_TFT_eSPI
    #include "Display-ili9341.h"
  #endif

#endif
