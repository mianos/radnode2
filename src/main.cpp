#include <Arduino.h>

#include <NeoPixelBus.h>

#include <esp_wifi.h>
#include <WiFi.h>
#include <Button2.h>

#define ESP_getChipId()   ((uint32_t)ESP.getEfuseMac())
// SSID and PW for Config Portal
String ssid = "radnode_" + String(ESP_getChipId(), HEX);
const char* password = "portal";
//
// SSID and PW for your Router
String Router_SSID;
String Router_Pass;

#include <ESP_WiFiManager.h>
#define ESP_DRD_USE_EEPROM      true
#define ESP_DRD_USE_SPIFFS      false
#define DOUBLERESETDETECTOR_DEBUG       true  //false
#include <ESP_DoubleResetDetector.h>


#include "SecMilli.h"
#include "ntp.h"
#include "Sender.h"


#include "maq.h"
#include "Display.h"

const int PixelCount = 4;

const int signPin = 25;
const int noisePin = 33; //17;
const int beepPin = 27; 
const int PixelPin = 26;

const int noise_hold_time = 200;    // 200mS

MiniNtp *mntp;

NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> strip(PixelCount, PixelPin);

#define ADC_EN          14
#define ADC_PIN         34
#define BUTTON_1        35
#define BUTTON_2        0

Button2 btn1(BUTTON_1);
Button2 btn2(BUTTON_2);

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
  digitalWrite(beepPin, LOW);
  beep_end = ev_at + 5;
  events++;

  events_acked = false;
}

int nc = 0;

void noise_event() {
  noise_hold_until = millis() + noise_hold_time;
  nc++;
}

Display *display;
Sender *sender;
// Number of seconds after reset during which a 
// subseqent reset will be considered a double reset.
#define DRD_TIMEOUT 10

// RTC Memory Address for the DoubleResetDetector to use
#define DRD_ADDRESS 0

bool initialConfig = false;

//DoubleResetDetector drd(DRD_TIMEOUT, DRD_ADDRESS);
DoubleResetDetector* drd;


void setup_wifi() {
    strip.SetPixelColor(1, RgbColor(50, 0, 0));	// RED led 1 config
  drd = new DoubleResetDetector(DRD_TIMEOUT, DRD_ADDRESS);
  //Local intialization. Once its business is done, there is no need to keep it around
  // Use this to default DHCP hostname to ESP8266-XXXXXX or ESP32-XXXXXX
  //ESP_WiFiManager ESP_wifiManager;
  // Use this to personalize DHCP hostname (RFC952 conformed)
  ESP_WiFiManager ESP_wifiManager("ConfigOnDoubleReset");
  
  ESP_wifiManager.setMinimumSignalQuality(-1);
  // Set static IP, Gateway, Subnetmask, DNS1 and DNS2. New in v1.0.5
  // ESP_wifiManager.setSTAStaticIPConfig(IPAddress(192,168,2,114), IPAddress(192,168,2,1), IPAddress(255,255,255,0), 
//                                        IPAddress(192,168,2,1), IPAddress(8,8,8,8));
  
  // We can't use WiFi.SSID() in ESP32 as it's only valid after connected. 
  // SSID and Password stored in ESP32 wifi_ap_record_t and wifi_config_t are also cleared in reboot
  // Have to create a new function to store in EEPROM/SPIFFS for this purpose
  Router_SSID = ESP_wifiManager.WiFi_SSID();
  Router_Pass = ESP_wifiManager.WiFi_Pass();
  
  //Remove this line if you do not want to see WiFi password printed
  printf("Stored: SSID = %s pass = %s\n",  Router_SSID.c_str(), Router_Pass.c_str());

  // SSID to uppercase 
  ssid.toUpperCase();
  
  if (Router_SSID != "") {
    ESP_wifiManager.setConfigPortalTimeout(60); //If no access point name has been previously entered disable timeout.
    printf("%s\n", "Got stored Credentials. Timeout 60s");
  } else {
    printf("%s\n", "No stored Credentials. No timeout");
    initialConfig = true;
  }
    
  if (drd->detectDoubleReset()) {
    printf("%s\n", "Double Reset Detected");
    initialConfig = true;
  }
  
  if (initialConfig) {
    printf("%s\n", "Starting configuration portal.");

    //sets timeout in seconds until configuration portal gets turned off.
    //If not specified device will remain in configuration mode until
    //switched off via webserver or device is restarted.
    //ESP_wifiManager.setConfigPortalTimeout(600);

    //it starts an access point 
    //and goes into a blocking loop awaiting configuration
    if (!ESP_wifiManager.startConfigPortal((const char *) ssid.c_str(), password)) 
      printf("%s\n", "Not connected to WiFi but continuing anyway.");
    else 
      printf("%s\n", "WiFi connected...yeey :)");    
  }

	strip.SetPixelColor(1, RgbColor(0, 10, 0));

#define WIFI_CONNECT_TIMEOUT        30000L
#define WHILE_LOOP_DELAY            200L
#define WHILE_LOOP_STEPS            (WIFI_CONNECT_TIMEOUT / ( 3 * WHILE_LOOP_DELAY ))
  
	unsigned long startedAt = millis();
  
	while ((WiFi.status() != WL_CONNECTED) && (millis() - startedAt < WIFI_CONNECT_TIMEOUT)) {   
		WiFi.mode(WIFI_STA);
		WiFi.persistent (true);
		// We start by connecting to a WiFi network
  
		printf("%s ","Connecting to ");
		printf("%s\n", Router_SSID.c_str());
  
		WiFi.begin(Router_SSID.c_str(), Router_Pass.c_str());

		int i = 0;
		while((!WiFi.status() || WiFi.status() >= WL_DISCONNECTED) && i++ < WHILE_LOOP_STEPS) {
		  delay(WHILE_LOOP_DELAY);
		}    
	}

	printf("After waiting ");
	printf("%lu", (millis()- startedAt) / 1000);
	printf(" secs more in setup(), connection result is ");

    if (WiFi.status() == WL_CONNECTED) {
        printf("connected. Local IP: ");
        printf("%s\n", WiFi.localIP().toString().c_str());
    } else {
        printf("%s\n", ESP_wifiManager.getStatus(WiFi.status()));
    }
}


void setup() {
	Serial.begin(9600); //9600bps
    // TODO: get this from the config file
    setenv("TZ", "AEST-10AEDT,M10.1.0,M4.1.0/3", 1);
    tzset();
    if (!SPIFFS.begin()) {
        printf("SPIFFS initialisation failed!");
        while (1) yield(); // Stay here twiddling thumbs waiting
    }
	strip.Begin();
	strip.Show();
	setup_wifi();
    mntp = new MiniNtp{"131.84.1.1", [](){ printf("time good\n"); }};

	pinMode(signPin,INPUT_PULLUP); //PIN setting for Radiation Pulse
	attachInterrupt(digitalPinToInterrupt(signPin), count_event, FALLING);
	pinMode(noisePin,INPUT_PULLUP); //PIN setting for Noise Pulse
	attachInterrupt(digitalPinToInterrupt(noisePin), noise_event, RISING);
	pinMode(beepPin, OUTPUT); 
	digitalWrite(beepPin, HIGH);
	display = new Display(mntp);
    btn1.setPressedHandler([](Button2& bb) {
        display->next_page(periods);
    });
    sender = new Sender([]() {
            printf("---------------- connected");
            });
}

int count_10s = 0;

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
    display->display_time();

    if (since > 8000) {
        if (since > 10000) {
            /* get the current count and subtract how much we are logging from it so we can put a mutex around this */
            int rcl = rad_count;
            rad_count -= rcl;
            last = now_millis;
            periods.rs60s.add(rcl);

            if (!(++count_10s % 6)) {
               periods.rs60mins.add(periods.rs60s.running_sum);
               sender->PublishRadValue(mntp, periods);
            }
            display->display(periods);
        } else {
            // greater than 8, less than 10, don't do anything below in case it takes too long
            return;
        }
    }
	drd->loop();
	mntp->run();
    btn1.loop();
    btn1.loop();
    sender->loop();
}
