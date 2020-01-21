# Remote radiation monitoring node V1
* This version is largely superceded by the new version.

This code runs on an ESP8266 or ESP32 with a radionwatch.org solid state radiation counter.

The radiation count and gamma dose factor are displayed on the local OLED + sent to the local MQTT server.

This project also includes a mS accurate NTP client that partially implements from scratch the UDP tracking NTP protocol.
