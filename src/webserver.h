#ifndef webserver_h
#define webserver_h
#include <Arduino.h>

extern char* mdnsName;

void setup_webserver();
void webserver_loop();
#endif // webserver_h
