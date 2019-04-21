#ifndef IOTWC_H
    #define IOTWC_H

    #include "userConfig.h"
    #include "ThermostatControl.h"
    #include <IotWebConf.h>
    #include <ArduinoJson.h>

    #ifdef ENABLE_MQTT
        #include <MQTT.h>

        #define STRING_LEN 64

        char mqttServerValue[STRING_LEN];
        char mqttUserNameValue[STRING_LEN];
        char mqttUserPasswordValue[STRING_LEN];
        unsigned long lastReport = 0;
        unsigned long lastMqttConnectionAttempt = 0;
        bool needMqttConnect = false;
    
        bool connectMqtt();
        bool connectMqttOptions();
        void mqttMessageReceived(String &topic, String &payload);

        WiFiClient net;
        MQTTClient mqttClient;
    #endif

    #define CONFIG_VERSION "iotwc0.1"

    bool needReset = false;
    void wifiConnected();
    void configSaved();
    bool formValidator();
    void handleRoot();
    
    DNSServer dnsServer;
    WebServer server(80);
    HTTPUpdateServer httpUpdater;

    IotWebConf iotWebConf(THERMOSTAT_NAME, &dnsServer, &server, WIFI_INITIAL_PASSWORD, CONFIG_VERSION);

    ThermostatControl* _control;

    #ifdef ENABLE_MQTT
        IotWebConfParameter mqttServerParam = IotWebConfParameter("MQTT server", "mqttServer", mqttServerValue, STRING_LEN);
        IotWebConfParameter mqttUserNameParam = IotWebConfParameter("MQTT user", "mqttUser", mqttUserNameValue, STRING_LEN);
        IotWebConfParameter mqttUserPasswordParam = IotWebConfParameter("MQTT password", "mqttPass", mqttUserPasswordValue, STRING_LEN, "password");
    #endif

    void initIoTWC(ThermostatControl* thermostatControl) {
        _control = thermostatControl;

        #ifdef RESET_PASSWORD_BUTTON
            iotWebConf.setConfigPin(RESET_PASSWORD_BUTTON);
        #endif
        
        #ifdef ENABLE_LEDS
            iotWebConf.setStatusPin(STATUS_LED_PIN);
        #endif

        #ifdef ENABLE_MQTT
            iotWebConf.addParameter(&mqttServerParam);
            iotWebConf.addParameter(&mqttUserNameParam);
            iotWebConf.addParameter(&mqttUserPasswordParam);
        #endif

        iotWebConf.setConfigSavedCallback(&configSaved);
        iotWebConf.setFormValidator(&formValidator);
        iotWebConf.setWifiConnectionCallback(&wifiConnected);
        iotWebConf.setupUpdateServer(&httpUpdater);

        bool validConfig = iotWebConf.init();
        
        if (!validConfig) {
            mqttServerValue[0] = '\0';
            mqttUserNameValue[0] = '\0';
            mqttUserPasswordValue[0] = '\0';
        }

        server.on("/", handleRoot);
        server.on("/config", []{ iotWebConf.handleConfig(); });
        server.onNotFound([](){ iotWebConf.handleNotFound(); });

        #ifdef ENABLE_MQTT
            mqttClient.begin(mqttServerValue, net);
            mqttClient.onMessage(mqttMessageReceived);
        #endif
    }

    void loopIoTWC() {
        iotWebConf.doLoop();

        #ifdef ENABLE_MQTT
            mqttClient.loop();

            if(needMqttConnect) {
                if(connectMqtt()) {
                    needMqttConnect = false;
                }
            } else if((iotWebConf.getState() == IOTWEBCONF_STATE_ONLINE) && (!mqttClient.connected())) {
                connectMqtt();
            }
        #endif

        if(needReset) {
            Serial.println("Rebooting after 1 second.");
            iotWebConf.delay(1000);
            ESP.restart();
        }
    }

    void handleRoot()
    {
        if (iotWebConf.handleCaptivePortal()) {
            return;
        }

        String s = "<!DOCTYPE html><html lang=\"en\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"/>";
        s += "<title>IotWebConf 01 Minimal</title></head><body>Hello world!";
        s += "Go to <a href='config'>configure page</a> to change settings.";
        s += "</body></html>\n";

        server.send(200, "text/html", s);
    }
    
    void wifiConnected() {
        Serial.println("Wifi connected");
        needMqttConnect = true;
    }

    void configSaved() {
        Serial.println("Config saved");
        needReset = true;
    }

    bool formValidator() {
        Serial.println("Validating form.");
        bool valid = true;

        int l = server.arg(mqttServerParam.getId()).length();
        if (l < 3)
        {
            mqttServerParam.errorMessage = "Please provide at least 3 characters!";
            valid = false;
        }

        return valid;
    }

    bool connectMqtt() {
        unsigned long now = millis();
        if (1000 > now - lastMqttConnectionAttempt) {
            return false;
        }
        Serial.println("Connecting to MQTT server...");
        if (!connectMqttOptions()) {
            lastMqttConnectionAttempt = now;
            return false;
        }
        Serial.println("Connected!");

        mqttClient.subscribe("thermostat/action");
        return true;
    }

    bool connectMqttOptions()
    {
        bool result;
        if (mqttUserPasswordValue[0] != '\0') {
            result = mqttClient.connect(iotWebConf.getThingName(), mqttUserNameValue, mqttUserPasswordValue);
        } else if (mqttUserNameValue[0] != '\0') {
            result = mqttClient.connect(iotWebConf.getThingName(), mqttUserNameValue);
        } else {
            result = mqttClient.connect(iotWebConf.getThingName());
        }
        return result;
    }

    void updateMqtt(float ctemp, float ttemp, char* cmode, char* amode) {
        const size_t capacity = JSON_OBJECT_SIZE(4) + 70;
        DynamicJsonDocument doc(capacity);

        doc["currentTemp"] = ctemp;
        doc["targetTemp"] = ttemp;
        doc["currentMode"] = cmode;
        doc["activeMode"] = amode;

        int cap = measureJson(doc) + 1;
        char buf[cap];
        
        serializeJson(doc, buf, sizeof(buf));
        
        mqttClient.publish("thermostat/status", buf);
    }

    void mqttMessageReceived(String &topic, String &payload) {
        const int capacity = JSON_OBJECT_SIZE(2) + 40;
        StaticJsonDocument<capacity> doc;

        deserializeJson(doc, payload);

        const char* command = doc["command"];
        
        if(strcmp("setTargetTemp",command) == 0) {
            Serial.println("Setting target temperature");
            _control->setTargetTemp(doc["value"]);
        } else if(strcmp("setMode",command) == 0) {
            Serial.println("Setting mode");
            _control->changeMode(doc["value"]);
        } else {
            Serial.println("Unknown command");
        }
    }
#endif
