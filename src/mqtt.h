#ifndef mqtt_h
#define mqtt_h
#include <PubSubClient.h>

PubSubClient mqtt_client;

void mqtt_setup(const char* mqtt_host, const unsigned int mqtt_port);
void mqtt_connect(const  char* mqtt_user, const char* mqtt_pass, const char* mqtt_sub_topic);

// void mqtt_loop();
#endif // mqtt_h
