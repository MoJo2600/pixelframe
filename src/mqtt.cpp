#include "ArduinoJson.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <mqtt.h>

WiFiClient wifi_client;
PubSubClient mqtt_client(wifi_client);
long lastReconnectAttempt = 0;

void mqtt_setup(const char* mqtt_host, const unsigned int mqtt_port, mqtt_callback f) {
  mqtt_client.setServer(mqtt_host, mqtt_port);
  mqtt_client.setCallback(f);
}

void mqtt_connect(const char* mqtt_user, const char* mqtt_pass, const char* mqtt_sub_topic) {

  Serial.println(F("[MQTT] connecting.."));

  if (millis() - lastReconnectAttempt > 5000) {
    lastReconnectAttempt = millis();
    mqtt_client.connect("pixelframe", mqtt_user, mqtt_pass);
    if (mqtt_client.connected()) {
      lastReconnectAttempt = 0;
      Serial.println(F("[MQTT] connected!"));
      mqtt_client.subscribe(mqtt_sub_topic);
    } else {
      Serial.print(F("[MQTT] failed, rc="));
      Serial.println(mqtt_client.state());
    } 
  }
}

bool mqtt_connected() {
  return mqtt_client.connected();
}

void mqtt_loop() {
  mqtt_client.loop();
}
