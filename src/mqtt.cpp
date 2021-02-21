#include "ArduinoJson.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Creates a partially initialised mqttclient instance.
WiFiClient wifi_client;
PubSubClient mqtt_client(wifi_client);

// mqtt subscription callback. This function is called when new messages arrive at the client.
void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  StaticJsonDocument<256> doc;
  deserializeJson(doc, payload, length);
  String pattern = doc["pattern"]; // e.g. "blink_slowly"
  JsonArray color = doc["color"];
  int R = color[0]; // e.g. 255
  int G = color[1]; // e.g. 255
  int B = color[2]; // e.g. 255
  int duration = doc["duration"]; // e.g. 10

  // Print received MQTT message. TODO: format JSON
  Serial.print("[MQTT] message on (");
  Serial.print(topic);
  Serial.println("): ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
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

void mqtt_setup(const char* mqtt_host, const unsigned int mqtt_port) {
  // Create MQTT connection
  mqtt_client.setServer(mqtt_host, mqtt_port);
  mqtt_client.setCallback(mqtt_callback);
}
