#ifndef mqtt_h
#define mqtt_h

typedef void (*mqtt_callback)(char* topic, byte* payload, unsigned int length);

void mqtt_setup(const char* mqtt_host, const unsigned int mqtt_port, mqtt_callback f);
void mqtt_connect(const  char* mqtt_user, const char* mqtt_pass, const char* mqtt_sub_topic);
bool mqtt_connected();
void mqtt_loop();

// void mqtt_loop();
#endif // mqtt_h
