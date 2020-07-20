#include <PubSubClient.h>
#include <ArduinoJson.h>

#include "settings.h"
#include <ThermostatControl.h>

#define NESP_MQTT_CONNECTION_INTERVAL 5000
#define NESP_MQTT_TOPIC_LENGTH 80
#define NESP_MQTT_BUFFER_SIZE 512

extern ThermostatControl control;

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

typedef struct mqttTopics_t {
    char availablity[NESP_MQTT_TOPIC_LENGTH];
    char temperature[NESP_MQTT_TOPIC_LENGTH];
    char targetTemp[NESP_MQTT_TOPIC_LENGTH];
    char targetTempSet[NESP_MQTT_TOPIC_LENGTH];
    char mode[NESP_MQTT_TOPIC_LENGTH];
    char modeSet[NESP_MQTT_TOPIC_LENGTH];
    char active[NESP_MQTT_TOPIC_LENGTH];
    char subscription[NESP_MQTT_TOPIC_LENGTH];
    char haDiscovery[NESP_MQTT_TOPIC_LENGTH];
} MqttTopics;

MqttTopics mqttTopic;

unsigned long lastConnectionAttempt = 0;

void recieveMessage(char* topic, byte* payload, unsigned int length) {

    payload[length] = '\0';

    if(strcmp(topic, mqttTopic.modeSet) == 0) { 
        control.changeMode((char *)payload);
        mqttClient.publish(mqttTopic.mode, control.getCurrentMainModeName());
    } else if(strcmp(topic, mqttTopic.targetTempSet) == 0) {
        float temp = (float)atof((char *)payload);
        control.setTargetTemp(temp);

        char buf[8];
        dtostrf(control.getTargetTemp(), 4, 1, buf);
        mqttClient.publish(mqttTopic.targetTemp, buf);
    }
}

bool connectMQTT() {
    if(mqttClient.connected()) {
        return true;
    }

    if(lastConnectionAttempt + NESP_MQTT_CONNECTION_INTERVAL > millis()) {
        return false;
    }

    Serial.println("Connecting to MQTT");

    if(settings.mqtt.mqttUsername[0] == '\0') {
        mqttClient.connect(settings.mqtt.mqttServer, mqttTopic.availablity, 0, true, "offline");
    } else {
        mqttClient.connect(settings.mqtt.mqttServer,settings.mqtt.mqttUsername, settings.mqtt.mqttPassword, mqttTopic.availablity, 0, true, "offline");
    }

    if(!mqttClient.connected()) {
        Serial.print("Connection failed: ");
        Serial.println(mqttClient.state());

        lastConnectionAttempt = millis();

        return false;
    }

    Serial.println("Connected!");

    mqttClient.publish(mqttTopic.availablity, "online", true);
    mqttClient.subscribe(mqttTopic.subscription);

    if(settings.mqtt.haDiscovery) {
        const size_t capacity = JSON_ARRAY_SIZE(5) + JSON_OBJECT_SIZE(10) + 342;
        DynamicJsonDocument doc(capacity);

        doc["name"] = settings.hostname;

        JsonArray modes = doc.createNestedArray("modes");
        modes.add("off");
        modes.add("auto");
        modes.add("cool");
        modes.add("heat");
        modes.add("fan_only");

        if(settings.useFahreheit) {
            doc["min_temp"] = NESP_MIN_TEMP_FAHRENHEIT;
            doc["max_temp"] = NESP_MAX_TEMP_FAHRENHEIT;
        } else {
            doc["min_temp"] = NESP_MIN_TEMP_CELSIUS;
            doc["max_temp"] = NESP_MAX_TEMP_CELSIUS;
        }

        doc["avty_t"] = mqttTopic.availablity;
        doc["curr_temp_t"] = mqttTopic.temperature;
        doc["temp_cmd_t"] = mqttTopic.targetTempSet;
        doc["temp_stat_t"] = mqttTopic.targetTemp;
        doc["mode_cmd_t"] = mqttTopic.modeSet; 
        doc["mode_stat_t"] = mqttTopic.mode;

        int cap = measureJson(doc) + 1;
        char buf[cap];
        
        serializeJson(doc, buf, cap);

        mqttClient.publish(mqttTopic.haDiscovery, buf, true);
    } else {
        mqttClient.publish(mqttTopic.haDiscovery, NULL, true);
    }

    return true;
}

bool configMQTT() {
    if(!settings.mqtt.enabled) {
        return false;
    }

    snprintf(mqttTopic.availablity, NESP_MQTT_TOPIC_LENGTH, "%s%s%s", settings.mqtt.mqttTopicPrefix, settings.hostname, "/availability");
    snprintf(mqttTopic.temperature, NESP_MQTT_TOPIC_LENGTH, "%s%s%s", settings.mqtt.mqttTopicPrefix, settings.hostname, "/temperature");
    snprintf(mqttTopic.targetTemp, NESP_MQTT_TOPIC_LENGTH, "%s%s%s", settings.mqtt.mqttTopicPrefix, settings.hostname, "/targettemp");
    snprintf(mqttTopic.targetTempSet, NESP_MQTT_TOPIC_LENGTH, "%s%s%s", settings.mqtt.mqttTopicPrefix, settings.hostname, "/targettemp/set");
    snprintf(mqttTopic.mode, NESP_MQTT_TOPIC_LENGTH, "%s%s%s", settings.mqtt.mqttTopicPrefix, settings.hostname, "/mode");
    snprintf(mqttTopic.modeSet, NESP_MQTT_TOPIC_LENGTH, "%s%s%s", settings.mqtt.mqttTopicPrefix, settings.hostname, "/mode/set");
    snprintf(mqttTopic.active, NESP_MQTT_TOPIC_LENGTH, "%s%s%s", settings.mqtt.mqttTopicPrefix, settings.hostname, "/active");
    snprintf(mqttTopic.subscription, NESP_MQTT_TOPIC_LENGTH, "%s%s%s", settings.mqtt.mqttTopicPrefix, settings.hostname, "/+/set");
    snprintf(mqttTopic.haDiscovery, NESP_MQTT_TOPIC_LENGTH, "%s%s%s", "homeassistant/climate/", settings.hostname, "/config");

    mqttClient.setBufferSize(NESP_MQTT_BUFFER_SIZE);
    mqttClient.setServer(settings.mqtt.mqttServer, atoi(settings.mqtt.mqttPort));
    mqttClient.setCallback(recieveMessage);

    return true;
}

bool handleMQTT() {
    if(!settings.mqtt.enabled) {
        return false;
    }

    if(!mqttClient.connected()) {
        connectMQTT();
        return false;
    }

    mqttClient.loop();

    return true;
}

void publishMQTT(float cTemp, float ttemp, char* cmode, char* amode) {
    char buf[8];
    dtostrf(cTemp, 4, 1, buf);
    mqttClient.publish(mqttTopic.temperature, buf);
    dtostrf(ttemp, 4, 1, buf);
    mqttClient.publish(mqttTopic.targetTemp, buf);
    mqttClient.publish(mqttTopic.active, amode);
    mqttClient.publish(mqttTopic.mode, cmode);
}

