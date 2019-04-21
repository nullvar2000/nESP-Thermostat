#include<Arduino.h>

void log(char* msg) {
    Serial.print(msg);
}

void logln(char* msg) {
    Serial.println(msg);
}

void log(float number) {
    Serial.print(number);
}

void logln(float number) {
    Serial.println(number);
}

void log(const char* msg) {
    Serial.print(msg);
}

void logln(const char* msg) {
    Serial.println(msg);
}