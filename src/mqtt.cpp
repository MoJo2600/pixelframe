#include "ArduinoJson.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <mqtt.h>

// Creates a partially initialised mqttclient instance.
WiFiClient wifi_client;
PubSubClient mqtt_client(wifi_client);

bool mqtt_connected() {
  return mqtt_client.connected();
}

void mqtt_loop() {
  mqtt_client.loop();
}

// Should be MQTT reconnection function
void mqtt_connect(const  char* mqtt_user, const char* mqtt_pass, const char* mqtt_sub_topic) {
  // Loop until we're reconnected
  while (!mqtt_client.connected()) {
    Serial.print("[MQTT] reconnecting..");
    // Create a random client ID
    String mqtt_client_id = "pixelframe-";
    mqtt_client_id += String(random(0xffff), HEX);
    // Attempt to connect
    if (mqtt_client.connect(mqtt_client_id.c_str(), mqtt_user, mqtt_pass)) {
      Serial.println("[MQTT] reconnected!");
      mqtt_client.subscribe(mqtt_sub_topic);
    } else {
      Serial.print("[MQTT] failed, rc=");
      Serial.print(mqtt_client.state());
      Serial.println(" retry in 5s.");
      delay(5000); // Wait 5 seconds before retrying
    }
  }
}

void mqtt_setup(const char* mqtt_host, const unsigned int mqtt_port, mqtt_callback f) {
  // Create MQTT connection
  mqtt_client.setServer(mqtt_host, mqtt_port);
  mqtt_client.setCallback(f);
}
