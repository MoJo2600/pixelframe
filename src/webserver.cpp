#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <Wire.h>
#include <LittleFS.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include <FastLED.h>

ESP8266WebServer server(80);       // Create a webserver object that listens for HTTP request on port 80
WebSocketsServer webSocket(81);    // create a websocket server on port 81

const char* mdnsName = "pixelframe"; // Domain name for the mDNS responder

String getContentType(String filename) { // determine the filetype of a given filename, based on the extension
  if (filename.endsWith(".html")) return "text/html";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".js")) return "application/javascript";
  else if (filename.endsWith(".ico")) return "image/x-icon";
  else if (filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}

bool handleFileRead(String path) { // send the right file to the client (if it exists)
  Serial.println("handleFileRead: " + path);
  if (path.endsWith("/")) path += "index.html";          // If a folder is requested, send the index file
  String contentType = getContentType(path);             // Get the MIME type
  String pathWithGz = path + ".gz";
  if (LittleFS.exists(pathWithGz) || LittleFS.exists(path)) { // If the file exists, either as a compressed archive, or normal
    if (LittleFS.exists(pathWithGz))                         // If there's a compressed version available
      path += ".gz";                                         // Use the compressed verion
    File file = LittleFS.open(path, "r");                    // Open the file
    size_t sent = server.streamFile(file, contentType);    // Send it to the client
    file.close();                                          // Close the file again
    Serial.println(String("\tSent file: ") + path);
    return true;
  }
  Serial.println(String("\tFile Not Found: ") + path);   // If the file doesn't exist, return false
  return false;
}

void handleNotFound(){ // if the requested file or page doesn't exist, return a 404 not found error
  if(!handleFileRead(server.uri())){          // check if the file exists in the flash memory (LittleFS), if so, send it
    server.send(404, "text/plain", "404: File Not Found");
  }
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght) { // When a WebSocket message is received
  switch (type) {
    case WStype_DISCONNECTED:             // if the websocket is disconnected
      Serial.printf("[%u] Disconnected!\n", num);
      break;
    case WStype_CONNECTED: {              // if a new websocket connection is established
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
      }
      break;
    case WStype_TEXT:                     // if new text data is received
      Serial.printf("[%u] get Text: %s\n", num, payload);

      StaticJsonDocument<512> received;
      // Deserialize the JSON document
      DeserializationError error = deserializeJson(received, payload);
      if (error) {
        Serial.println(F("Failed to read payload"));
        return;
      }

      // Got brightness?
      int brightness = received["brightness"];
      if (brightness) {
        Serial.println("Setting brightness");
        Serial.println(brightness);
        FastLED.setBrightness(brightness);
      }
      break;
  }
}

void startWebSocket() { // Start a WebSocket server
  webSocket.begin();                          // start the websocket server
  webSocket.onEvent(webSocketEvent);          // if there's an incomming websocket message, go to function 'webSocketEvent'
  Serial.println("WebSocket server started.");
}

void startMDNS() { // Start the mDNS responder
  // start the multicast domain name server
  if (!MDNS.begin(mdnsName)) {
    Serial.print("Could not start MDNS service!");
  }
  else
  {
    Serial.print("mDNS responder started: http://");
    Serial.print(mdnsName);
    Serial.println(".local");
  }
}

void startServer() { // Start a HTTP server with a file read handler and an upload handler
  server.onNotFound(handleNotFound);          // if someone requests any other file or page, go to function 'handleNotFound'
                                              // and check if the file exists

  server.begin();                             // start the HTTP server
  Serial.println("HTTP server started.");
}


void setup_webserver() {
  // delay(10);

  // Serial.println("\r\n");

  // startWiFi();                 // Start a Wi-Fi access point, and try to connect to some given access points. Then wait for either an AP or STA connection
  
  // startLittleFS();               // Start the LittleFS and list all contents

  startWebSocket();            // Start a WebSocket server
  
  startMDNS();                 // Start the mDNS responder

  startServer();               // Start a HTTP server with a file read handler and an upload handler
}

void webserver_loop() {

  // unsigned long now = millis();

  webSocket.loop();                           // constantly check for websocket events
  server.handleClient();                      // run the server
  MDNS.update();

  // unsigned long currentMillis = millis();

  // if (webSocket.connectedClients() > 0) {
  //   // if (currentMillis - previousMillis >= 3000)
  //   // {
  //   //   digitalWrite(PIN_LED, LOW);
  //   //   previousMillis = currentMillis;
  //   //   // creat JSON message for Socket.IO (event)
  //   //   DynamicJsonDocument doc(1024);

  //   //   doc["temperature"] = getTemperature();

  //   //   batteryReading = getBattery(DEFAULT_BATTERY_EMPTY_ADC_READING, DEFAULT_BATTERY_FULL_ADC_READING);
  //   //   doc["battery_raw"] = batteryReading.raw;
  //   //   doc["battery"] = batteryReading.adjusted;
  //   //   doc["battery_percent"] = batteryReading.percent;

  //   //   moistureReading = getMoisture(batteryReading.adjusted, DEFAULT_MOIST_DRY_READING_AT_3V, DEFAULT_MOIST_WET_READING_AT_3V, DEFAULT_BATTERY_FULL_ADC_READING);
  //   //   doc["moisture_raw"] = moistureReading.raw;
  //   //   doc["moisture"] = moistureReading.adjusted;
  //   //   doc["moisture_percent"] = moistureReading.percent;

  //   //   // JSON to String (serializion)
  //   //   String output;
  //   //   serializeJson(doc, output);

  //   //   webSocket.broadcastTXT(output);
  //   //   now = millis();
  //   //   digitalWrite(PIN_LED, HIGH);
  //   // }
  // }
}
