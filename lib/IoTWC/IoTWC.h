#ifndef IOTWC_H
    #define IOTWC_H

    #include "userConfig.h"
    #include "ThermostatControl.h"
    #include "TemperatureSensor.h"
    #include <IotWebConf.h>
    #include <ArduinoJson.h>

    #ifdef ENABLE_MQTT
        #include <MQTT.h>

        #define STRING_LEN 64
        #define AVAILABILITY_STATE_TOPIC MQTT_PREFIX THERMOSTAT_NAME "/availability/state"
        #define CURRENT_TEMP_STATE_TOPIC MQTT_PREFIX THERMOSTAT_NAME "/temperature/state"
        #define TARGET_TEMP_STATE_TOPIC MQTT_PREFIX THERMOSTAT_NAME "/targettemp/state"
        #define TARGET_TEMP_COMMAND_TOPIC MQTT_PREFIX THERMOSTAT_NAME "/targettemp/set"
        #define ACTIVE_STATE_TOPIC MQTT_PREFIX THERMOSTAT_NAME "/active/state"
        #define MODE_STATE_TOPIC MQTT_PREFIX THERMOSTAT_NAME "/mode/state"
        #define MODE_COMMAND_TOPIC MQTT_PREFIX THERMOSTAT_NAME "/mode/set"
        #define SWING_STATE_TOPIC MQTT_PREFIX THERMOSTAT_NAME "/swing/state"
        #define SWING_COMMAND_TOPIC MQTT_PREFIX THERMOSTAT_NAME "/swing/set"
        #define OFFSET_STATE_TOPIC MQTT_PREFIX THERMOSTAT_NAME "/offset/state"
        #define OFFSET_COMMAND_TOPIC MQTT_PREFIX THERMOSTAT_NAME "/offset/set"
        #define PRESENCE_STATE_TOPIC MQTT_PREFIX THERMOSTAT_NAME "/presence/state"
        #define PRESENCE_COMMAND_TOPIC MQTT_PREFIX THERMOSTAT_NAME "/presence/set"
        #define SUBSCRIPTION_TOPIC MQTT_PREFIX THERMOSTAT_NAME "/+/set"
        #define HA_DISCOVERY_TOPIC "homeassistant/climate/" THERMOSTAT_NAME "/config"

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
        MQTTClient mqttClient(512);
    #endif

    #define CONFIG_VERSION "iotwc0.2"

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
    TemperatureSensor* _tempSensor;

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
            mqttClient.setWill(AVAILABILITY_STATE_TOPIC, "offline", true, 0);
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

        Serial.print("Subscribing to ");
        Serial.println(SUBSCRIPTION_TOPIC);
        mqttClient.subscribe(SUBSCRIPTION_TOPIC);

        #ifdef ENABLE_HOME_ASSISTANT_DISCOVERY
            const size_t capacity = JSON_ARRAY_SIZE(6) + JSON_OBJECT_SIZE(10);
            
            DynamicJsonDocument doc(capacity);
            
            doc["name"] = THERMOSTAT_NAME;

            JsonArray modes = doc.createNestedArray("modes");
            #ifndef DISABLE_OFF
                modes.add("off");
            #endif
            #ifndef DISABLE_AUTO
                modes.add("auto");
            #endif
            #ifndef DISABLE_COOL
                modes.add("cool");
            #endif
            #ifndef DISABLE_HEAT
                modes.add("heat");
            #endif
            #ifndef DISABLE_EHEAT
                modes.add("eheat");
            #endif
            #ifndef DISABLE_FAN_ONLY
                modes.add("fan_only");
            #endif

            if(USE_FAHRENHEIT) {
                doc["min_temp"] = MIN_TARGET_TEMP_F;
                doc["max_temp"] = MAX_TARGET_TEMP_F;
            } else {
                doc["min_temp"] = MIN_TARGET_TEMP_C;
                doc["max_temp"] = MAX_TARGET_TEMP_C;
            }
            doc["avty_t"] = AVAILABILITY_STATE_TOPIC;
            doc["curr_temp_t"] = CURRENT_TEMP_STATE_TOPIC;
            doc["temp_cmd_t"] = TARGET_TEMP_COMMAND_TOPIC;
            doc["temp_stat_t"] = TARGET_TEMP_STATE_TOPIC;
            doc["mode_cmd_t"] = MODE_COMMAND_TOPIC;
            doc["mode_stat_t"] = MODE_STATE_TOPIC;

            int cap = measureJson(doc) + 1;
            char buf[cap];
            
            serializeJson(doc, buf, sizeof(buf));

            mqttClient.publish(HA_DISCOVERY_TOPIC, buf, true, 1);
        #endif

        mqttClient.publish(AVAILABILITY_STATE_TOPIC, "online", true, 0);

        return true;
    }

    bool connectMqttOptions() {
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
        char buf[8];
        dtostrf(ctemp, 4, 1, buf);
        mqttClient.publish(CURRENT_TEMP_STATE_TOPIC, buf);
        dtostrf(ttemp, 4, 1, buf);
        mqttClient.publish(TARGET_TEMP_STATE_TOPIC, buf);
        mqttClient.publish(ACTIVE_STATE_TOPIC, amode);
        mqttClient.publish(MODE_STATE_TOPIC, cmode);

        #ifdef ENABLE_PRESENCE_DETECTION
            if(_control->getPresence()) {
                mqttClient.publish(PRESENCE_STATE_TOPIC, "true");
            } else {
                mqttClient.publish(PRESENCE_STATE_TOPIC, "false");
            }
        #endif
        
    }

    void mqttMessageReceived(String &topic, String &payload) {
        if(topic.equals(MODE_COMMAND_TOPIC)) {
            char buf[10];
            payload.toCharArray(buf, 10);        
            char *cmode = _control->changeMode(buf);
            mqttClient.publish(MODE_STATE_TOPIC, cmode);
        } else if(topic.equals(TARGET_TEMP_COMMAND_TOPIC)) {
            _control->setTargetTemp(payload.toFloat());
            char buf[8];
    		dtostrf(_control->getTargetTemp(), 4, 1, buf);
	    	mqttClient.publish(TARGET_TEMP_STATE_TOPIC, buf);
        } else if(topic.equals(PRESENCE_COMMAND_TOPIC)) {
            if(payload.equals("true")) {
                _control->setPresence(true);
                mqttClient.publish(PRESENCE_STATE_TOPIC, "true");
            } else {
                _control->setPresence(false);
                mqttClient.publish(PRESENCE_STATE_TOPIC, "false");
            }
        }  else if(topic.equals(SWING_COMMAND_TOPIC)) {
            _control->setSwing(payload.toFloat());
            char buf[8];
    		dtostrf(_control->getSwing(), 4, 1, buf);
	    	mqttClient.publish(SWING_STATE_TOPIC, buf);
        }  else if(topic.equals(OFFSET_COMMAND_TOPIC)) {
            _tempSensor->setOffset(payload.toFloat());
            char buf[8];
    		dtostrf(_tempSensor->getOffset(), 4, 1, buf);
	    	mqttClient.publish(OFFSET_STATE_TOPIC, buf);
        }
    }
#endif
