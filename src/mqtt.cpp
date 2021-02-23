#include "ArduinoJson.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <mqtt.h>

// Creates a partially initialised mqttclient instance.
WiFiClient wifi_client;
PubSubClient mqtt_client(wifi_client);

void mqtt_setup(const char* mqtt_host, const unsigned int mqtt_port, mqtt_callback f) {
  printf("[mqtt_setup]  begin %p\n",(void*)&mqtt_client);
  // Create MQTT connection
  Serial.print("[mqtt_setup] mqtt_host: ");
  Serial.println(mqtt_host);
  Serial.print("[mqtt_setup] mqtt_port: ");
  Serial.println(mqtt_port);
  mqtt_client.setServer(mqtt_host, mqtt_port);
  mqtt_client.setCallback(f);
  printf("[mqtt_setup] end %p\n",(void*)&mqtt_client);
}

// Should be MQTT reconnection function
void mqtt_connect(const char* mqtt_user, const char* mqtt_pass, const char* mqtt_sub_topic) {
  printf("[mqtt_connect] begin %p\n",(void*)&mqtt_client);
  Serial.print("[mqtt_connect] mqtt_user: ");
  Serial.println(mqtt_user);
  Serial.print("[mqtt_connect] mqtt_pass: ");
  Serial.println(mqtt_pass);
  Serial.print("[mqtt_connect] mqtt_sub_topic: ");
  Serial.println(mqtt_sub_topic);
  // Loop until we're reconnected
  while (!mqtt_client.connected()) {
    Serial.println("[MQTT] connecting..");
    // Create a random client ID
    String mqtt_client_id = "pixelframe-";
    mqtt_client_id += String(random(0xffff), HEX);
    // Attempt to connect
    Serial.print("[mqtt_connect] mqtt_client_id: ");
    Serial.println(mqtt_client_id);
    printf("[mqtt_connect] middle %p\n",(void*)&mqtt_client);
    if (mqtt_client.connect(mqtt_client_id.c_str(), mqtt_user, mqtt_pass)) {
      Serial.println("[MQTT] connected!");
      mqtt_client.subscribe(mqtt_sub_topic);
    } else {
      Serial.println((String)"[MQTT] failed, rc=" + mqtt_client.state() + " retry in 5s.");
      delay(5000); // Wait 5 seconds before retrying
    }
  }
}

bool mqtt_connected() {
  return mqtt_client.connected();
}

void mqtt_loop() {
  mqtt_client.loop();
}
