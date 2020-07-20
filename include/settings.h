#ifndef SETTINGS_H
#define SETTINGS_H

#include <Preferences.h>

#define NESP_SETTINGS_NAMESPACE "nesp-settings"
#define NESP_TARGET_TEMP_KEY "target-temp"
#define NESP_MODE_KEY "mode"
#define NESP_USE_FAHRENHEIT_KEY "use-fahrenheit"
#define NESP_SENSOR "sensor"
#define NESP_HOSTNAME_KEY "hostname"
#define NESP_MQTT_SETTINGS_KEY "mqtt-settings"
#define NESP_DEFAULT_HOSTNAME "nesp"
#define NESP_DEFAULT_MQTT_PORT "1883"
#define NESP_DEFAULT_MQTT_TOPIC_PREFIX "thermostat/"
#define NESP_STRING_LENGTH 32
#define NESP_DEFAULT_TARGET_TEMP_FAHRENHEIT 72
#define NESP_MIN_TEMP_FAHRENHEIT 60
#define NESP_MAX_TEMP_FAHRENHEIT 80
#define NESP_DEFAULT_TARGET_TEMP_CELSIUS 22
#define NESP_MIN_TEMP_CELSIUS 15
#define NESP_MAX_TEMP_CELSIUS 27

//extern Preferences prefs;
Preferences prefs;
extern float targetTemp;
extern uint8_t mode;

typedef struct mqttSettings_t {
    bool enabled;
    char mqttServer[NESP_STRING_LENGTH];
    char mqttPort[NESP_STRING_LENGTH];
    char mqttUsername[NESP_STRING_LENGTH];
    char mqttPassword[NESP_STRING_LENGTH];
    char mqttTopicPrefix[NESP_STRING_LENGTH];
    bool haDiscovery;
} MqttSettings;

typedef struct settings_t {
    bool useFahreheit;
    uint8_t minTemperature;
    uint8_t maxTemperature;
    uint8_t sensor;
    char hostname[NESP_STRING_LENGTH];
    MqttSettings mqtt;
} Settings;

Settings settings;

void displaySettings() {
    Serial.print("useFahrenheit: ");
    Serial.println(settings.useFahreheit);
    Serial.print("sensor: ");
    Serial.println(settings.sensor);
    Serial.print("hostname: ");
    Serial.println(settings.hostname);
    Serial.print("enableMQTT: ");
    Serial.println(settings.mqtt.enabled);
    Serial.print("mqttServer: ");
    Serial.println(settings.mqtt.mqttServer);
    Serial.print("mqttPort: ");
    Serial.println(settings.mqtt.mqttPort);
    Serial.print("mqttUsername: ");
    Serial.println(settings.mqtt.mqttUsername);
    Serial.print("mqttPassword: ");
    Serial.println(settings.mqtt.mqttPassword);
    Serial.print("mqttTopicPrefix: ");
    Serial.println(settings.mqtt.mqttTopicPrefix);
    Serial.print("haDiscovery: ");
    Serial.println(settings.mqtt.haDiscovery);
    Serial.print("targetTemp: ");
    Serial.println(targetTemp);
    Serial.print("mode: ");
    Serial.println(mode);
    Serial.println("");
}

void loadSettings() {
    Serial.println("Loading settings");

    settings.useFahreheit = prefs.getBool(NESP_USE_FAHRENHEIT_KEY, false);

    if(settings.useFahreheit) {
        settings.minTemperature = NESP_MIN_TEMP_FAHRENHEIT;
        settings.maxTemperature = NESP_MAX_TEMP_FAHRENHEIT;
    } else {
        settings.minTemperature = NESP_MIN_TEMP_CELSIUS;
        settings.maxTemperature = NESP_MAX_TEMP_CELSIUS;
    }

    settings.sensor = prefs.getUInt(NESP_SENSOR);

    size_t hostnameLength = prefs.getString(NESP_HOSTNAME_KEY, settings.hostname, NESP_STRING_LENGTH);

    if(hostnameLength <= 2) {
        strncpy(settings.hostname, NESP_DEFAULT_HOSTNAME, sizeof(NESP_DEFAULT_HOSTNAME));
    }

    size_t mqttLength = prefs.getBytesLength(NESP_MQTT_SETTINGS_KEY);

    if(mqttLength > 0) {
        char buf[mqttLength];

        prefs.getBytes(NESP_MQTT_SETTINGS_KEY, buf, mqttLength);
        memcpy(&settings.mqtt, buf, mqttLength);
    }

    if(settings.mqtt.mqttPort[0] == '\0') {
        //settings.mqtt.mqttPort = (uint8_t)NESP_DEFAULT_MQTT_PORT;
        strncpy(settings.mqtt.mqttPort, NESP_DEFAULT_MQTT_PORT, sizeof(NESP_DEFAULT_MQTT_PORT));
    }

    if(settings.mqtt.mqttTopicPrefix[0] == '\0') {
        strncpy(settings.mqtt.mqttTopicPrefix, NESP_DEFAULT_MQTT_TOPIC_PREFIX, sizeof(NESP_DEFAULT_MQTT_TOPIC_PREFIX));
    }

    targetTemp = prefs.getFloat(NESP_TARGET_TEMP_KEY, (settings.useFahreheit) ? NESP_DEFAULT_TARGET_TEMP_FAHRENHEIT : NESP_DEFAULT_TARGET_TEMP_CELSIUS);
    mode = prefs.getUInt(NESP_MODE_KEY, 0);

    displaySettings();
}

void saveSettings() {
    Serial.println("Saving settings");

    prefs.putBool(NESP_USE_FAHRENHEIT_KEY, settings.useFahreheit);
    prefs.putUInt(NESP_SENSOR, settings.sensor);
    prefs.putString(NESP_HOSTNAME_KEY, settings.hostname);
    prefs.putBytes(NESP_MQTT_SETTINGS_KEY, &settings.mqtt, sizeof(settings.mqtt));

    //displaySettings();
    ESP.restart();
}

void saveTargetTemp() {
    Serial.println("Saving target temp");
    prefs.putFloat(NESP_TARGET_TEMP_KEY, targetTemp);
}

void saveMode() {
    Serial.println("Saving mode");
    prefs.putUInt(NESP_MODE_KEY, mode);
}

#endif
