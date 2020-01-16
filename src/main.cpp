#include <Arduino.h>

#include <NeoPixelBus.h>

#include "maq.h"
#include "Display.h"

const int PixelPin = 17;
const int PixelCount = 4;

const int signPin = 2; // 2 36Radiation Pulse (Yellow)
const int noisePin = 23; // 5; //Vibration Noise Pulse (White)
const int beepPin = 22; // 3;

const double alpha = 53.032; // cpm = uSv x alpha
// const double CONV_FACTOR = 1.0 / alpha;
// // #define CONV_FACTOR 0.00812


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

void setup() {
  Serial.begin(9600); //9600bps

  pinMode(signPin,INPUT_PULLUP); //PIN setting for Radiation Pulse
  attachInterrupt(digitalPinToInterrupt(signPin), count_event, FALLING);
  pinMode(noisePin,INPUT_PULLUP); //PIN setting for Noise Pulse
  attachInterrupt(digitalPinToInterrupt(noisePin), noise_event, RISING);
  pinMode(beepPin, OUTPUT); //PIN setting for Noise Pulse
  digitalWrite(beepPin, HIGH);
  strip.Begin();
  strip.Show();
  display.begin();
}

int count_10s = 0;
Rcs rs60s  =  Rcs(60, 6);
Rcs rs60mins = Rcs(360, 60);

void loop() {
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
            display.output(rcl * 6, 10, rcl, (float)rcl * 6 / alpha);
            double rs60 = rs60s.add(rcl);
            //output(rs60 * 6, 60, rcl, (float)rs60  * 6 / alpha);
			int32_t remapped[rs60s.len];
			int out_size = rs60s.scale(remapped, 16);
			//tgraph(remapped, out_size, 15, 6, 2);
           rs60s.vq->display();
            if (!(++count_10s % 6)) {
               double rs60min = rs60mins.add(rs60s.running_sum);
               // output(rs60min, 360, rcl, (float)rs60min / alpha);
    		   int32_t rem60[rs60mins.len];
    		   int out_size = rs60mins.scale(rem60, 16);
               rs60mins.vq->display();
			   // tgraph(rem60, out_size, 16, 60, 1, 18, 0);
            }
        } else {
            // greater than 8, less than 10, don't do anything below in case it takes too long
            return;
        }
    }
}
