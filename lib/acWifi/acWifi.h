#ifndef ACWIFI_H
#define ACWIFI_H

#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <AutoConnect.h>

#include "ThermostatControl.h"

extern ThermostatControl control;

extern uint8_t minTemp;
extern uint8_t maxTemp;
extern uint8_t tempStep;
extern float currentTemp;

String style = PSTR(
    ".slidecontainer {"
        "color: black"
    "}"
    ".radiocontainer input[type=\"radio\"] {"
        "opacity: 0;"
        "position: fixed;"
        "width: 0;"
    "}"
    ".radiocontainer label {"
        "display: inline-block;"
        "background-color: #ddd;"
        "padding: 10px 20px;"
        "font-family: sans-serif, Arial;"
        "font-size: 16px;"
        "border: 2px solid #444;"
        "border-radius: 4px;"
    "}"
    ".radiocontainer input[type=\"radio\"]:checked + label {"
        "background-color:#bfb;"
        "border-color: #4c4;"
    "}"
    ".radiocontainer input[type=\"radio\"]:focus + label {"
        "border: 2px dashed #444;"
    "}"
    ".radiocontainer label:hover {"
        "background-color: #dfd;"
    "}"
);

WebServer server;
AutoConnect portal(server);
AutoConnectConfig config;

// define / uri
//ACText(header, "nESP");
//ACText(temp, "Temperature");

//AutoConnectAux home("/", "nESP Thermostat", false, {header, temp});
//AutoConnectAux home("/mqtt_config", "MQTT Config");

void handleRoot() {
  String page = PSTR (
    "<html>"
    "<head>"
    "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"
    "<title>nESP Thermostat</title>"
    "<script>function submitTemp(temp) { window.location.href = \"submitTemp?target=\"+temp; }"
    "function submitMode(mode) { window.location.href = \"submitMode?mode=\"+mode; }</script>"
    "<style>"
  );
  page += style;
  page += PSTR (
    "</style>"
    "</head>"
    "<body>"
    "<div class=\"menu\">" AUTOCONNECT_LINK(BAR_32) "</div>"
    "<center>"
    "<div class=\"slidecontainer\">"
    "Current Temperature: <span id=\"slider_value\" style=\"color:green;font-weight:bold;\">"
  );
  page += String(currentTemp) + String(F("</span><br>"));
  page += String(F("Target Temperature: ")) + String(control.getTargetTemp()) + String(F("<br>"));
  page += String(minTemp) + String(F("<input type=\"range\" name=\"target\" min=\"")) + String(minTemp);
  page += String(F("\" max=\"")) + String(maxTemp);
  page += String(F("\" value=\"")) + String(control.getTargetTemp());
  page += String(F("\" class=\"slider\" onchange=\"submitTemp(this.value);\">")) + String(maxTemp);
  page += String(F("<br></div>"));
  page += String(F("<div class=\"radiocontainer\">"));
  page += String(F("<input type=\"radio\" name=\"mode\" value=\"off\" id=\"off\" onchange=\"submitMode(this.value);\" ")) + (control.getCurrentMainMode() == MAIN_OFF ? String(F(" checked>")) : ">");
  page += String(F("<label for=\"off\">Off</label>"));
  page += String(F("<input type=\"radio\" name=\"mode\" value=\"auto\" id=\"auto\" onchange=\"submitMode(this.value);\" ")) + (control.getCurrentMainMode() == MAIN_AUTO ? String(F(" checked>")) : ">");
  page += String(F("<label for=\"auto\">Auto</label>"));
  page += String(F("<input type=\"radio\" name=\"mode\" value=\"cool\" id=\"cool\" onchange=\"submitMode(this.value);\" ")) + (control.getCurrentMainMode() == MAIN_COOL ? String(F(" checked>")) : ">");
  page += String(F("<label for=\"cool\">Cool</label>"));
  page += String(F("<input type=\"radio\" name=\"mode\" value=\"heat\" id=\"heat\" onchange=\"submitMode(this.value);\" ")) + (control.getCurrentMainMode() == MAIN_HEAT ? String(F(" checked>")) : ">");
  page += String(F("<label for=\"heat\">Heat</label>"));
  page += String(F("<input type=\"radio\" name=\"mode\" value=\"eheat\" id=\"eheat\" onchange=\"submitMode(this.value);\" ")) + (control.getCurrentMainMode() == MAIN_EHEAT ? String(F(" checked>")) : ">");
  page += String(F("<label for=\"eheat\">EHeat</label>"));
  page += String(F("<input type=\"radio\" name=\"mode\" value=\"fan_only\" id=\"fan_only\" onchange=\"submitMode(this.value);\" ")) + (control.getCurrentMainMode() == MAIN_FAN ? String(F(" checked>")) : ">");
  page += String(F("<label for=\"fan_only\">Fan</label>"));
  page += String(F("</select></div></center>"));
  page += String(F("</body></html>"));
  
  server.send(200, "text/html", page);
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

void setupWifi() {

  config.hostName = "nespy";
  config.autoReconnect = true;
  config.title = "nESP";
  config.apid = "nesp-" + String((uint32_t)(ESP.getEfuseMac() >> 32), HEX);
  config.psk = "nesp123";
  
  portal.config(config);
  //portal.join({home});

  server.on("/", handleRoot);
  server.on("/submitTemp", handleSubmitTemp);
  server.on("/submitMode", handleSubmitMode);

  if(portal.begin()) {
    Serial.println("WiFi connected: " + WiFi.localIP().toString());

    if(MDNS.begin("nespy")) {
        MDNS.addService("http", "tcp", 80);
    }
  }
}

void loopWifi() {
    portal.handleClient();
}

#endif