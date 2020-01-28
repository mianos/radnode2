#pragma once

#include <WiFiClient.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const char* mqtt_server = "mqtt2.mianos.com";


class Sender {
    WiFiClient espClient;
    PubSubClient client;
    int last_connect_time;
    const char *mqtt_name;
    String mqtt_base = String("tele/radnode/");
public:
    Sender(const char *mqtt_name="radnode3") 
        : client(espClient), last_connect_time(0), mqtt_name(mqtt_name) {
      client.setServer(mqtt_server, 1883);
      client.setCallback([this](char* topic, byte* payload, unsigned int length) {
              callback(topic, payload, length);
          });
    }
    void	PubJson(const JsonDocument& data, const String& topic=String("tele")) {
        client.publish((const char *)(mqtt_base + topic).c_str(), (const char *)data.as<String>().c_str());
    }
    void    PublishStatus(const char * status) {
        StaticJsonDocument<100> json_state;
        json_state["status"] = status;
        PubJson(json_state);
    }

    void callback(char* topic, byte* payload, unsigned int length) {
        printf("Callback\n");
    }


    void reconnect() {
      auto now = millis();
      if (!last_connect_time || now > last_connect_time + 1000) {
        if (client.connect(mqtt_name)) {
            printf("mqtt Connected\n");
            PublishStatus("startup");
          // ... and resubscribe
          //client.subscribe("cmnd/doorbell/tone");
        } else {
            printf("mqtt NOT Connected\n");
        }
        last_connect_time = now;
      }
    }

    void loop() {
        if (!client.connected()) {
            reconnect();
        }
        client.loop();
    }
};
