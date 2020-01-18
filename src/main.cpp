#include <Arduino.h>

#include <NeoPixelBus.h>

#include "maq.h"
#include "Display.h"

const int PixelPin = 17;
const int PixelCount = 4;

const int signPin = 2; // 2 36Radiation Pulse (Yellow)
const int noisePin = 23; // 5; //Vibration Noise Pulse (White)
const int beepPin = 22; // 3;



NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> strip(PixelCount, PixelPin);

int rad_count = 0;
int held = 0;
unsigned long noise_hold_until = 0;
unsigned long beep_end = 0;

int events = 0;
bool events_acked = false;

void count_event() {
  auto ev_at = millis();
  if (ev_at < noise_hold_until) {
    held++;
    return;
  } 
  rad_count++;
//  int sign = digitalRead(signPin);
  digitalWrite(beepPin, LOW);
  beep_end = ev_at + 5;
  events++;

  events_acked = false;
}

int nc = 0;

void noise_event() {
  noise_hold_until = millis() + 100;
  nc++;
}

Display display;

struct Dbc {
    int bc = 0;
    int last_bc = -1;
    unsigned long last_bc_time = 0;
    bool action() {
        if (bc && bc != last_bc) {
            if (last_bc_time && (last_bc_time + 300 > millis())) {
                last_bc = bc;
                return false;
            } else {
                last_bc_time = millis();
            }
            last_bc = bc;
            return true;
        }
        return false;
    }
} dbc;

void button_event() {
    dbc.bc++;
}

void setup() {
  Serial.begin(9600); //9600bps

  pinMode(signPin,INPUT_PULLUP); //PIN setting for Radiation Pulse
  attachInterrupt(digitalPinToInterrupt(signPin), count_event, FALLING);
  pinMode(noisePin,INPUT_PULLUP); //PIN setting for Noise Pulse
  attachInterrupt(digitalPinToInterrupt(noisePin), noise_event, RISING);
  pinMode(beepPin, OUTPUT); 
  digitalWrite(beepPin, HIGH);
  pinMode(0, INPUT_PULLUP); // PRG button
  attachInterrupt(digitalPinToInterrupt(0), button_event, FALLING);
  strip.Begin();
  strip.Show();
  display.begin();
}

int count_10s = 0;

void loop() {
    if (dbc.action()) {
        display.next_page();
    }
    if (events && !events_acked) {
        strip.SetPixelColor(0, RgbColor(0, 30, 0));
        strip.Show();
        events = 0;
        events_acked = true;
    }
    if (beep_end && millis() > beep_end) {
        digitalWrite(beepPin, HIGH);
        strip.SetPixelColor(0, RgbColor(0, 0, 0));
        strip.Show();
        beep_end = 0;
    }
    static unsigned  last = 0;
    int now_millis = millis();
    int since = now_millis - last;
    if (since > 8000) {
        if (since > 10000) {
            /* get the current count and subtract how much we are logging from it so we can put a mutex around this */
            int rcl = rad_count;
            rad_count -= rcl;
            last = now_millis;
            periods.rs60s.add(rcl);
            if (!(++count_10s % 6)) {
               periods.rs60mins.add(periods.rs60s.running_sum);
            }
            display.display(rcl, periods);
        } else {
            // greater than 8, less than 10, don't do anything below in case it takes too long
            return;
        }
    }
}
