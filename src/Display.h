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
        sprintf(buffer, "10 CPM %d %d %.6f\n", cpm, count, uS_h);
        tft.setColor(BLACK);
        tft.fillRect(0, 0, 128, 10);
        tft.setColor(WHITE);
        tft.drawString(0, 0, buffer);
        tft.display();
    }

    void show_60(int cpm, int count, double uS_h) {
        char buffer[80];
        sprintf(buffer, "60 CPM %d %d %.6f\n", cpm, count, uS_h);
        tft.setColor(BLACK);
        tft.fillRect(0, 12, 128, 10);
        tft.setColor(WHITE);
        tft.drawString(0, 12, buffer);
        tft.display();
    }

    void    output(int cpm, int period, int count, double uS_h) {
        switch (period) {
        case 10:
            show_10(cpm, count, uS_h);
            break;
        case 60:
            show_60(cpm, count, uS_h);
            break;
       }
    }
};
