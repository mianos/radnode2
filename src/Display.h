#pragma once

#include <TFT_eSPI.h>

#include "Periods.h"

const double alpha = 53.032; // cpm = uSv x alpha
// const double CONV_FACTOR = 1.0 / alpha;
// // #define CONV_FACTOR 0.00812


const int pages = 2;

class Display {
    TFT_eSPI tft;
    int page;
public:
    Display() : tft(135, 240) {
        tft.init();
        tft.setRotation(1);
        tft.fillScreen(TFT_BLACK);
        tft.setTextSize(2);
        tft.setTextColor(TFT_WHITE);
        tft.setCursor(0, 0);
        tft.setTextDatum(MC_DATUM);
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
        tft.fillRect(off_x, off_y, max_size * bwidth, height, TFT_BLACK);
        tft.drawRect(off_x, off_y, max_size * bwidth, height, TFT_WHITE);
#if 1
        int current_x = 0;
        for (int ii = 0; ii < vsize; ii++) {
            // tft.fillRect(current_x, 0, (current_x + 1) * bwidth, values[ii], WHITE);
            if (values[ii]) {
                tft.fillRect(current_x + off_x + 1, height - values[ii] + off_y - 1, bwidth, values[ii], TFT_WHITE);
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

    }
    void next_page() {
        if (++page == pages) {
            page = 0;
        }
        printf("Change page to %d\n", page);
    }
    void begin() {
        tft.init();
//        tft.flipScreenVertically();
//        tft.setFont(ArialMT_Plain_16);
    }
    int psym(int y, int x, const char *text) {
        tft.setTextColor(TFT_WHITE);
        tft.setTextFont(10);
        tft.setCursor(y, x);
        tft.print(text);
//        int width = tft.getStringWidth(text);
//        tft.setFont(ArialMT_Plain_16);
        return 40;
    }
    void show_tline(int cpm, int count, double uS_h, int row) {
        char buffer[80];
        tft.fillRect(0, row, 128, row + 18, TFT_BLACK);
        tft.setTextColor(TFT_WHITE);
        sprintf(buffer, "%5.*f", decimals(uS_h), uS_h);
        int us_width = 40; // uS/h is right hand justfied
        //tft.setTextAlignment(TEXT_ALIGN_RIGHT);
        tft.setCursor(30, row);
        tft.print(buffer);
        //tft.setTextAlignment(TEXT_ALIGN_LEFT);
        //us_width += psym(us_width, row, "uS/h");
        //us_width += 2;
        //sprintf(buffer, "%d", cpm);
        //tft.drawString(us_width, row, buffer);
        //us_width += tft.getStringWidth(buffer);
        //if (cpm < 100000) {
            us_width += psym(us_width, row, "cpm");
       // }
    }
};
