#ifndef USERCONFIG_H

    #define USERCONFIG_H

    /* ============================================================================
    * Enable wifi and mqtt
    * ==========================================================================*/
    #define ENABLE_WIFI
    #define THERMOSTAT_NAME "thermostat"
    #define WIFI_INITIAL_PASSWORD "Pa55word!2"
    //#define RESET_PASSWORD_BUTTON 16

    #define ENABLE_MQTT

    /* ============================================================================
    * Select the HVAC type and associated pins
    * ==========================================================================*/
    #define HEATPUMP_HVAC_TYPE
    //#define CONVENTIONAL_HVAC_TYPE

    #ifdef HEATPUMP_HVAC_TYPE
        #define EPin 32 // emergency heat
        #define AuxPin 33 // 2nd stage heat
        #define GPin 25 // fan 
        #define OBPin 26 // reversing valve, off for cool, on for heat
        #define YPin 27 // compressor
        #define LPin -1
    #endif

    #ifdef CONVENTIONAL_HVAC_TYPE
        #define EPin 32 // emergency heat
        #define WPin 33 // heat
        #define W2Pin 34 // 2nd stage heat
        #define GPin 25 // fan
        #define Y2Pin 26 // 2nd stage compressor
        #define YPin 27 // compressor
    #endif


    /* ============================================================================
    * Disable HVAC modes
    * Uncomment to disable
    * ==========================================================================*/
    //#define DISABLE_OFF 0
    //#define DISABLE_AUTO 1
    //#define DISABLE_COOL 2
    //#define DISABLE_HEAT 3
    //#define DISABLE_EHEAT 4
    //#define DISABLE_FAN 5

    /* ============================================================================
    * Fahrenheit or celsius
    * ==========================================================================*/
    #define USE_FAHRENHEIT
    //#define USE_CELSIUS

    /* ============================================================================
    * Select the temperature sensor and pin
    * ==========================================================================*/
    //#define USE_BMP280
    #define USE_DHT11

    #define TEMP_SENSOR_PIN 4

    /* ============================================================================
    * Enable buttons and set pins
    * ==========================================================================*/
    #define ENABLE_BUTTONS

    #define MODE_BUTTON_PIN 17
    #define UP_BUTTON_PIN 18
    #define DOWN_BUTTON_PIN 19

    /* ============================================================================
    * Enable LEDs and set pins
    * ==========================================================================*/
    #define ENABLE_LEDS

    #define STATUS_LED_PIN 12
    #define COOL_LED_PIN 13 
    #define HEAT_LED_PIN 14

    /* ============================================================================
    * Enable display 
    * Select display type and set pins
    * ==========================================================================*/
    #define ENABLE_DISPLAY
    #define USE_SSD1306

    #define I2C_SDA 21
    #define I2C_SCL 22

    /* ============================================================================
    * Various optional settings
    * ==========================================================================*/
    #define LOG_LEVEL 0 

    #define UPDATE_INTERVAL 10000

    #define BAUD_RATE 9600

#endif
