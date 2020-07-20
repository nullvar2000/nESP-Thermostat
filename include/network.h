#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <AutoConnect.h>

#include "settings.h"
#include <ThermostatControl.h>

#define NESP_AP_PSK "nESP123"

WebServer server;
AutoConnect portal(server);
AutoConnectConfig config;

extern Settings settings;
//extern float targetTemp;
//extern uint8_t mode;
//extern float currentTemp;
extern ThermostatControl control;

// define components of settings page
ACText(settingsHeader, "<h1>Settings</h1>");
ACText(thermHeader, "<h2>Thermostat Settings</h2>");
ACCheckbox(useFahrenheit, "fahrenheit", "Use Fahrenheit", false);
//ACSelect(sensor, {"DHT11", "BMP280", "DS18B20"}, "Sensor");
ACText(hostHeader, "<h2>Host Settings</h2>");
ACInput(hostname, "", "Hostname");
ACText(mqttHeader, "<h2>MQTT Settings</h2>");
ACCheckbox(mqttEnabled, "mqttEnabled", "MQTT Enabled", false);
ACInput(mqttServer, "", "MQTT Server", "^(([a-zA-Z0-9]|[a-zA-Z0-9][a-zA-Z0-9\\-]*[a-zA-Z0-9])\\.)*([A-Za-z0-9]|[A-Za-z0-9][A-Za-z0-9\\-]*[A-Za-z0-9])$", "MQTT broker server");
ACInput(mqttPort, "1883", "MQTT Port");
ACInput(mqttUsername, "", "MQTT Username");
ACInput(mqttPassword, "", "MQTT Password");
ACInput(mqttTopicPrefix, NESP_DEFAULT_MQTT_TOPIC_PREFIX, "MQTT Topic Prefix");
ACCheckbox(mqttHaDiscovery, "haDiscovery", "Home Assistant Discovery", false);
ACElement(newLine, "<hr>");
ACSubmit(save, "Save", "/settings_save");
ACSubmit(cancel, "Cancel", "/_ac");

// define the settings page
AutoConnectAux settings_page("/settings", "Settings", true, {
  settingsHeader,
  thermHeader,
  useFahrenheit,
  //sensor,
  newLine,
  hostHeader,
  hostname,
  newLine,
  mqttHeader,
  mqttEnabled,
  mqttServer,
  mqttPort,
  mqttUsername,
  mqttPassword,
  mqttTopicPrefix,
  mqttHaDiscovery,
  newLine,
  save,
  cancel
});

// define the components of the settings_save page
ACText(saveCaption, "<h4>Saved!</h4>", "text-align:center;color:#2f4f4f;padding:10px;");

// define the settings_save page
AutoConnectAux settings_save("/settings_save", "Save Settings", false, {
  saveCaption
});

void handleRoot() {

  String minTempStr = String(settings.minTemperature);
  String maxTempStr = String(settings.maxTemperature);
  String targetTempStr = String(control.getTargetTemp());
  String content = R"(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8" name="viewport" content="width=device-width, initial-scale=1">
  <title>nESP Thermostat</title>
  <script>
    function submitTemp(temp) { window.location.href = "submitTemp?target=" + temp; }
    function submitMode(mode) { window.location.href = "submitMode?mode=" + mode; }
  </script>
</head>
<body>
  <div class=\"menu\">__AC_BAR__</div>
  <div class=\"slidecontainer\">
    <p>
      <center>
        Current Temperature: <span id="current_temp" style="color:green;font-weight:bold;">)";

  content += String(control.getCurrentTemp()) + String(F("</span><br>Target Temperature: ")) + targetTempStr + String(F("<br>"));
  content += minTempStr + String(F("<input type=\"range\" name=\"target\" min=\"")) + minTempStr;
  content += String(F("\" max=\"")) + maxTempStr + String(F("\" value=\"")) + targetTempStr;
  content += String(F("\" class=\"slider\" onchange=\"submitTemp(this.value);\">")) + maxTempStr;
  content += String(F("<br><select onchange=\"submitMode(this.value);\">"));
  content += String(F("<option value=\"0\"")) + (mode == 0 ? String(F(" selected")) : "") + String(F(">Off</option>"));
  content += String(F("<option value=\"1\"")) + (mode == 1 ? String(F(" selected")) : "") + String(F(">Auto</option>"));
  content += String(F("<option value=\"2\"")) + (mode == 2 ? String(F(" selected")) : "") + String(F(">Cool</option>"));
  content += String(F("<option value=\"3\"")) + (mode == 3 ? String(F(" selected")) : "") + String(F(">Heat</option>"));
  content += String(F("<option value=\"4\"")) + (mode == 4 ? String(F(" selected")) : "") + String(F(">EHeat</option>"));
  content += String(F("<option value=\"5\"")) + (mode == 5 ? String(F(" selected")) : "") + String(F(">Fan</option>"));
  content += String(F("</select></center></body></html>"));

  content.replace("__AC_BAR__", String(AUTOCONNECT_LINK(BAR_32)));
  server.send(200, "text/html", content);
}

String handleSettingsPage(AutoConnectAux& aux, PageArgument& args) {
  useFahrenheit.checked = settings.useFahreheit;
  //sensor.selected = settings.sensor;
  hostname.value = String(settings.hostname);
  mqttEnabled.checked = settings.mqtt.enabled;
  mqttServer.value = String(settings.mqtt.mqttServer);
  mqttPort.value = String(settings.mqtt.mqttPort);
  mqttUsername.value = String(settings.mqtt.mqttUsername);
  mqttPassword.value = String(settings.mqtt.mqttPassword);
  mqttTopicPrefix.value = String(settings.mqtt.mqttTopicPrefix);
  mqttHaDiscovery.checked = settings.mqtt.haDiscovery;

  return String("");
}

String handleSettingsSave(AutoConnectAux& aux, PageArgument& args) {
  settings.useFahreheit = useFahrenheit.checked;
  //settings.sensor = sensor.selected;
  hostname.value.toCharArray(settings.hostname, NESP_STRING_LENGTH);
  settings.mqtt.enabled = mqttEnabled.checked;
  mqttServer.value.toCharArray(settings.mqtt.mqttServer, NESP_STRING_LENGTH);
  mqttPort.value.toCharArray(settings.mqtt.mqttPort, NESP_STRING_LENGTH);
  mqttUsername.value.toCharArray(settings.mqtt.mqttUsername, NESP_STRING_LENGTH);
  mqttPassword.value.toCharArray(settings.mqtt.mqttPassword, NESP_STRING_LENGTH);
  mqttTopicPrefix.value.toCharArray(settings.mqtt.mqttTopicPrefix, NESP_STRING_LENGTH);
  settings.mqtt.haDiscovery = mqttHaDiscovery.checked;
  
  saveSettings();

  return String("");
}

void sendRedirect() {
  server.sendHeader("Location", "/", true);
  server.send(302, "text/plain", "");
  server.client().stop();
}

void handleSubmitTemp() {
  control.setTargetTemp(server.arg("target").toFloat());

  sendRedirect();
}

void handleSubmitMode() {
  control.changeMode(server.arg("mode").toInt());

  sendRedirect();
}

void configNet() {

  server.on("/", handleRoot);
  server.on("/submitTemp", handleSubmitTemp);
  server.on("/submitMode", handleSubmitMode);

  config.apid = "nESP-setup";
  config.psk = NESP_AP_PSK;
  config.title = "nESP Thermostat";
  config.hostName = String(settings.hostname);
  config.ticker = false;
  //config.tickerPort = LED_BUILTIN;
  config.tickerOn = HIGH;
  config.ota = AC_OTA_BUILTIN;
  config.autoReconnect = true;

  //config.menuItems = AC_MENUITEM_CONFIGNEW | AC_MENUITEM_DISCONNECT | AC_MENUITEM_RESET | AC_MENUITEM_HOME;
  portal.config(config);
  portal.join({ settings_page, settings_save });
  portal.on("/settings", handleSettingsPage);
  portal.on("/settings_save", handleSettingsSave);

  Serial.print("WiFi ");
  if (portal.begin()) {
    config.bootUri = AC_ONBOOTURI_HOME;
    Serial.println("connected: " + WiFi.SSID());
    Serial.println("IP: " + WiFi.localIP().toString());
  }

  if (MDNS.begin(settings.hostname)) {
    MDNS.addService("http", "tcp", 80);
  }
}
