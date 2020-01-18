#pragma once

#include "SSD1306.h"

#include "Periods.h"

const double alpha = 53.032; // cpm = uSv x alpha
// const double CONV_FACTOR = 1.0 / alpha;
// // #define CONV_FACTOR 0.00812


#define OLED_I2C_ADDR 0x3C
#define OLED_RESET 16
#define OLED_SDA 4
#define OLED_SCL 15

const int pages = 2;

class Display {
    SSD1306 tft;
    int page;
public:
    Display() : tft(OLED_I2C_ADDR, OLED_SDA, OLED_SCL) {
    }

    int decimals(double val) {
        if (val > 99.9) {
            return 0;
        } else if (val > .99) {
            return 1;
        } else if (val > 0.1) {
            return 2;
        } else {
            return 3;
        }
    }

    void tgraph(int32_t *values, int vsize, int height, int max_size, int bwidth=3, int off_x=0, int off_y=0) {
        tft.setColor(BLACK);
        tft.fillRect(off_x, off_y, max_size * bwidth, height);
        tft.setColor(WHITE);
        tft.drawRect(off_x, off_y, max_size * bwidth, height);
#if 1
        int current_x = 0;
        for (int ii = 0; ii < vsize; ii++) {
            // tft.fillRect(current_x, 0, (current_x + 1) * bwidth, values[ii], WHITE);
            if (values[ii]) {
                tft.fillRect(current_x + off_x + 1, height - values[ii] + off_y - 1, bwidth, values[ii]);
                // printf("Out %d val %d\n", ii, values[ii]);
            } 
            current_x += bwidth;
        }
#endif
    }

    void display(int rcl, Periods& periods) {
#if 1
        show_tline(rcl * 6, rcl, (float)rcl * 6 / alpha, 0);
        auto rs60_avg = periods.rs60s.r_avg;
        show_tline(rs60_avg * 6, rcl, (float)rs60_avg * 6 / alpha, 16);
#endif
#if 1
        const int gheight = 16;
        int32_t remapped[periods.rs60s.len];
        int out_size = periods.rs60s.scale(remapped, gheight - 2);
        tgraph(remapped, out_size,
                gheight /* height */, 6 /* max_size */, 2 /* line width */, 0 /* off_x */, 40 /* off_y */);


        int32_t minrem[periods.rs60mins.len];
        out_size = periods.rs60mins.scale(minrem, gheight - 2);
        tgraph(minrem, out_size,
                gheight /* height */, 60 /* max_size */, 1 /* line width */, 20 /* off_x */, 40 /* off_y */);

#endif
        tft.display();

    }
    void next_page() {
        if (++page == pages) {
            page = 0;
        }
        printf("Change page to %d\n", page);
    }
    void begin() {
        pinMode(OLED_RESET,OUTPUT);
        digitalWrite(OLED_RESET, LOW);
        delay(50);
        digitalWrite(OLED_RESET, HIGH);
        tft.init();
//        tft.flipScreenVertically();
        tft.setFont(ArialMT_Plain_16);
    }
    int psym(int y, int x, const char *text) {
        tft.setFont(ArialMT_Plain_10);
        tft.drawString(y, x, text);
        int width = tft.getStringWidth(text);
        tft.setFont(ArialMT_Plain_16);
        return width;
    }
    void show_tline(int cpm, int count, double uS_h, int row) {
        char buffer[80];
        tft.setColor(BLACK);
        tft.fillRect(0, row, 128, row +18);
        tft.setColor(WHITE);
        sprintf(buffer, "%5.*f", decimals(uS_h), uS_h);
        int us_width = 40; // uS/h is right hand justfied
        tft.setTextAlignment(TEXT_ALIGN_RIGHT);
        tft.drawString(us_width, row, buffer);
        tft.setTextAlignment(TEXT_ALIGN_LEFT);
        us_width += psym(us_width, row, "uS/h");
        us_width += 2;
        sprintf(buffer, "%d", cpm);
        tft.drawString(us_width, row, buffer);
        us_width += tft.getStringWidth(buffer);
        if (cpm < 100000) {
            us_width += psym(us_width, row, "cpm");
        }
    }
};
