#pragma once

#include <TFT_eSPI.h>

struct DeltaText {
    class TFT_eSPI &tft;
    int width;
    char *pre_value;
    int len;
    int base_x;
    int base_y;
    int font;
    DeltaText(class TFT_eSPI &tft, const char *shape, int base_x, int base_y, int font) :
            tft(tft), base_x(base_x), base_y(base_y), font(font) {
        len = strlen(shape);
        tft.setTextFont(font);
        width = tft.textWidth("8");
        pre_value = new char[len + 1];
        strcpy(pre_value, shape);
    }
    void Refresh() {
        tft.setTextFont(font);
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        tft.setTextDatum(TL_DATUM);
        tft.setTextPadding(width);
        for (int ii = 0; ii < len; ii++) {
            tft.drawChar(pre_value[ii], base_x + width * ii, base_y);
        }
    }
    int  Draw(const char *text) {
        if (strcmp(text, pre_value) == 0) {
            return 0;
        }
        tft.setTextFont(font);
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        tft.setTextDatum(TL_DATUM);
        tft.setTextPadding(width);
        int drawn = 0;
        for (int ii = 0; ii < len; ii++) {
            if (text[ii] != pre_value[ii]) {
                pre_value[ii] = text[ii];
                tft.fillRect(base_x + width * ii, base_y, width, 16, TFT_BLACK);
                tft.drawChar(text[ii], base_x + width * ii, base_y);
                drawn++;
            }
        }
        tft.setTextPadding(0);
        return drawn;
    }
};

