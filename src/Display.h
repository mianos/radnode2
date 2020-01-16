#pragma once

#include "SSD1306.h"


#define OLED_I2C_ADDR 0x3C
#define OLED_RESET 16
#define OLED_SDA 4
#define OLED_SCL 15


class Display {
    SSD1306 tft;
public:
    Display() : tft(OLED_I2C_ADDR, OLED_SDA, OLED_SCL) {
    }

    void begin() {
        pinMode(OLED_RESET,OUTPUT);
        digitalWrite(OLED_RESET, LOW);
        delay(50);
        digitalWrite(OLED_RESET, HIGH);
        tft.init();
//        tft.flipScreenVertically();
        tft.setFont(ArialMT_Plain_10);
    }
    void show_10(int cpm, int count, double uS_h) {
        char buffer[80];
        sprintf(buffer, "CPM %d %d %.6f\n", cpm, count, uS_h);
        tft.clear();
        tft.drawString(0, 0, buffer);
        tft.display();
    }

    void show_360(int cpm, int count, double uS_h) {
        char buffer[80];
        sprintf(buffer, "CPM %d %d %.6f\n", cpm, count, uS_h);
        tft.clear();
        tft.drawString(12, 0, buffer);
    }

    void    output(int cpm, int period, int count, double uS_h) {
        switch (period) {
        case 10:
            show_10(cpm, count, uS_h);
            break;
        case 360:
            show_360(cpm, count, uS_h);
            break;
       }
    }
};
