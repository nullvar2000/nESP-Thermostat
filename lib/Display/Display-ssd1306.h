#include <U8g2lib.h>

#include "userConfig.h"
#include "Logger.h"

U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, I2C_SCL, I2C_SDA, U8X8_PIN_NONE);

void initDisplay() {
    u8g2.begin();
    u8g2.setFont(u8g2_font_profont12_mr);
}

void updateDisplay(float ctemp, float ttemp, char* cmode, char* amode) {
    char tempStr[8];
    char line[24];
    
    u8g2.clearBuffer();
    
    dtostrf(ctemp, 5, 1, tempStr);
    strcpy(line, "Current Temp: ");
    strcat(line, tempStr);
    u8g2.drawStr(0, 12, line);
    
    dtostrf(ttemp, 5, 1, tempStr);
    strcpy(line, "Target Temp: ");
    strcat(line, tempStr);
    u8g2.drawStr(0, 24, line);
    
    strcpy(line, "Current mode: ");
    strcat(line, cmode);
    u8g2.drawStr(0, 36, line);
    
    strcpy(line, "Active mode: ");
    strcat(line, amode);
    u8g2.drawStr(0, 48, line);
    
    u8g2.sendBuffer();
}

