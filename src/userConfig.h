#ifndef USERCONFIG_H

    #define USERCONFIG_H

    /* ============================================================================
    * Enable wifi and mqtt
    * ==========================================================================*/
    #define ENABLE_WIFI
    #define THERMOSTAT_NAME "nESP" // must be unique if you have more than one on your network
    #define WIFI_INITIAL_PASSWORD "Pa55word!2"
    #define RESET_PASSWORD_BUTTON 15

    #define ENABLE_MQTT
    #define MQTT_PREFIX "home/thermostat/" 
    
    #define ENABLE_HOME_ASSISTANT_DISCOVERY
    
    /* ============================================================================
    * Select the HVAC type and associated pins
    * ==========================================================================*/
    #define HEATPUMP_HVAC_TYPE 0 
    //#define CONVENTIONAL_HVAC_TYPE 1

    #ifdef HEATPUMP_HVAC_TYPE
        #define EPin 255 // emergency heat
        #define AuxPin 26 // 2nd stage heat
        #define GPin 27 // fan 
        #define OBPin 32 // reversing valve, set direction below
        #define YPin 33 // compressor
        #define LPin 255

        // change to false if your reversing valve is powered for heating (B wire)
        #define REVERSING_VALVE_POWERED_FOR_COOLING  true// O wire
    #endif

    // not working yet
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
    #define DISABLE_EHEAT 4
    //#define DISABLE_FAN_ONLY 5

    /* ============================================================================
    * Fahrenheit or celsius
    * Set USE_FAHRENHEIT to false to use celsius
    * Adjust others as needed
    * ==========================================================================*/
    #define USE_FAHRENHEIT true

    #define INCREMENT_TEMP_STEP 1.0

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

    #define MODE_BUTTON_PIN 16
    #define UP_BUTTON_PIN 17
    #define DOWN_BUTTON_PIN 18

    /* ============================================================================
    * Enable LEDs and set pins
    * ==========================================================================*/
    #define ENABLE_LEDS

    #define STATUS_LED_PIN 19
    #define FAN_LED_PIN 19
    #define COOL_LED_PIN 20 
    #define HEAT_LED_PIN 23

    /* ============================================================================
    * Enable display 
    * Select display type and set pins
    * ==========================================================================*/
    #define ENABLE_DISPLAY
    #define USE_SSD1306

    #define I2C_SDA 21
    #define I2C_SCL 22

    #define HSPI_SCLK 14
    #define HSPI_MISO 12
    #define HSPI_MOSI 13 // SDA
    #define HSPI_CS 15

    #define VSPI_SCLK 18
    #define VSPI_MISO 19
    #define VSPI_MOSI 23 // SDA
    #define VSPI_CS 5

    /* ============================================================================
    * Enable presence detection 
    * Set pin
    * ==========================================================================*/
    #define ENABLE_PRESENCE_DETECTION
    
    #define PRESENCE_PIN 5
    #define PRESENCE_COOLDOWN 1800000

    /* ============================================================================
    * Various optional settings
    * ==========================================================================*/
    #define LOG_LEVEL 0 

    #define UPDATE_INTERVAL 10000

    #define BAUD_RATE 9600

#endif
