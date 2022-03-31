#ifndef pixel_webserver_h
#define pixel_webserver_h
#include <Arduino.h>

extern char* mdnsName;

void setup_webserver();
void webserver_loop();
#endif // pixel_webserver_h
