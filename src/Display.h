#pragma once

#include <TFT_eSPI.h>

#include "Free_Fonts.h" 

#include "Periods.h"
#include "DeltaText.h"

const double alpha = 53.032; // cpm = uSv x alpha
// const double CONV_FACTOR = 1.0 / alpha;
// // #define CONV_FACTOR 0.00812


const int pages = 2;
const int tft_width = TFT_HEIGHT;
const int tft_height = TFT_WIDTH;

class Display {
    TFT_eSPI tft;
    int page = 0;
    MiniNtp *mntp;
    DeltaText dtx;
public:
    Display(MiniNtp *mntp) : tft(tft_height, tft_width), mntp(mntp), dtx(tft, "--:--:--", 120, 0, 4) {
        tft.init();
        tft.setRotation(1);
        tft.fillScreen(TFT_BLACK);

        dtx.Refresh();
    }

    void display_time() {
        if (mntp->is_good()) {
            char buffer[40];
            auto nn = mntp->now();
            nn.local(buffer, sizeof (buffer), 10, true);
            buffer[8] = 0;
            dtx.Draw(buffer);
        }
    }

    int decimals(double val) {
        if (val == 0) {
            return 0;
        } else if (val > 99.9) {
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

    void display(Periods& periods) {

#if 1
        if (page == 0) {
            show_page_title("10S");
            auto projected_60s_count = periods.rs60s.recent() * 6;
            show_tline(projected_60s_count, (float)projected_60s_count / alpha, 1);
        } else {
            show_page_title("60S");
            auto rs60_avg = periods.rs60s.r_avg;
            show_tline(rs60_avg * 6, (float)rs60_avg * 6 / alpha, 1);
        }
#endif
#if 1
        const int gheight = 32;
        int32_t remapped[periods.rs60s.len];
        int out_size = periods.rs60s.scale(remapped, gheight - 2);
        tgraph(remapped, out_size,
                gheight /* height */, 6 /* max_size */, 16 /* line width */, 0 /* off_x */, 70 /* off_y */);


//        int32_t minrem[periods.rs60mins.len];
//        out_size = periods.rs60mins.scale(minrem, gheight - 2);
//        tgraph(minrem, out_size,
//                gheight /* height */, 60 /* max_size */, 1 /* line width */, 20 /* off_x */, 70 /* off_y */);

#endif

    }
    void next_page(Periods &periods) {
        if (++page == pages) {
            page = 0;
        }
        printf("Change page to %d\n", page);
        display(periods);
    }
    
    int psym(int x, int y, const char *text) {
        tft.setTextColor(TFT_WHITE);
        tft.setFreeFont(TT1);
        tft.drawString(text, x, y);
        int width = tft.textWidth(text);
        return width;
    }
    void show_page_title(const char *title) {
        tft.setTextFont(4);    
        tft.setTextColor(TFT_WHITE, TFT_RED);
        tft.setTextDatum(TL_DATUM);
        auto width = tft.textWidth("M");
        tft.setTextPadding(width);
        tft.drawString(title, 0, 0);
    }
    void show_tline(int cpm, double uS_h, int row) {
        char buffer[80];

        tft.setTextFont(4);    
        auto width = tft.textWidth("M");
        auto font_height = tft.fontHeight();
        auto y_pos = row * font_height;
        sprintf(buffer, "%5.*f", decimals(uS_h), uS_h);

        auto us_width =  tft.textWidth("000.0"); // uS/h is right hand justfied
        tft.setTextColor(TFT_WHITE, TFT_RED);
        tft.setTextDatum(TR_DATUM);
        tft.setTextPadding(width);
        tft.drawString(buffer, us_width, y_pos);
        tft.setTextDatum(TL_DATUM);
        us_width += 2;

        us_width += psym(us_width, y_pos,  "uSv/h");

        sprintf(buffer, "%d", cpm);
        tft.setTextFont(4);    
        tft.setTextColor(TFT_WHITE, TFT_RED);
        tft.setTextPadding(width);
        tft.drawString(buffer, us_width, y_pos);
        us_width += tft.textWidth(buffer);
        us_width += 2;

        if (cpm < 10000) {
            psym(us_width, y_pos,  "cpm");
       }
    }
};
