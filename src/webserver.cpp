#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <Wire.h>
#define FS_NO_GLOBALS
#include <LittleFS.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include <FastLED.h>

const char* fsName = "LittleFS";
FS* fileSystem = &LittleFS;
LittleFSConfig fileSystemConfig = LittleFSConfig();

ESP8266WebServer server(80);       // Create a webserver object that listens for HTTP request on port 80
WebSocketsServer webSocket(81);    // create a websocket server on port 81

const char* mdnsName = "pixelframe"; // Domain name for the mDNS responder

static const char TEXT_PLAIN[] PROGMEM = "text/plain";
static const char FS_INIT_ERROR[] PROGMEM = "FS INIT ERROR";
static const char FILE_NOT_FOUND[] PROGMEM = "FileNotFound";

////////////////////////////////
// Utils to return HTTP codes, and determine content-type

void replyOK() {
  server.send(200, FPSTR(TEXT_PLAIN), "");
}

void replyOKWithMsg(String msg) {
  server.send(200, FPSTR(TEXT_PLAIN), msg);
}

void replyNotFound(String msg) {
  server.send(404, FPSTR(TEXT_PLAIN), msg);
}

void replyBadRequest(String msg) {
  Serial.println(msg);
  server.send(400, FPSTR(TEXT_PLAIN), msg + "\r\n");
}

void replyServerError(String msg) {
  Serial.println(msg);
  server.send(500, FPSTR(TEXT_PLAIN), msg + "\r\n");
}

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

/*
   Return the list of files in the directory specified by the "dir" query string parameter.
   Also demonstrates the use of chuncked responses.
*/
void handleFileList() {
  // if (!fsOK) {
  //   return replyServerError(FPSTR(FS_INIT_ERROR));
  // }

  if (!server.hasArg("dir")) {
    return replyBadRequest(F("DIR ARG MISSING"));
  }

  String path = server.arg("dir");
  if (path != "/" && !fileSystem->exists(path)) {
    return replyBadRequest("BAD PATH");
  }

  Serial.println(String("handleFileList: ") + path);
  Dir dir = fileSystem->openDir(path);
  path.clear();

  server.sendHeader("Access-Control-Allow-Origin", "*");
  // use HTTP/1.1 Chunked response to avoid building a huge temporary string
  if (!server.chunkedResponseModeStart(200, "application/json")) {
    server.send(505, F("text/html"), F("HTTP1.1 required"));
    return;
  }

  // use the same string for every line
  String output;
  output.reserve(64);
  while (dir.next()) {

    if (output.length()) {
      // send string from previous iteration
      // as an HTTP chunk
      server.sendContent(output);
      output = ',';
    } else {
      output = '[';
    }

    output += "{\"type\":\"";
    if (dir.isDirectory()) {
      output += "dir";
    } else {
      output += F("file\",\"size\":\"");
      output += dir.fileSize();
    }

    output += F("\",\"name\":\"");
    // Always return names without leading "/"
    if (dir.fileName()[0] == '/') {
      output += &(dir.fileName()[1]);
    } else {
      output += dir.fileName();
    }

    output += "\"}";
  }

  // send last string
  output += "]";
  server.sendContent(output);
  server.chunkedResponseFinalize();
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

      const char* action = received["action"];
      if (action) {
        Serial.print("Got action: ");
        Serial.println(action);
      }

      const char* timezone = received["timezone"];
      if (timezone) {
        Serial.println("Setting timezone");
        Serial.println(timezone);
      }


      // TODO: If 'save settings'

      // // get wifi config from homie
      // File file = SPIFFS.open("/system/config.json", "r");               // Open the file
      // DynamicJsonDocument doc(1024);
      // // Deserialize the JSON document
      // error = deserializeJson(doc, file);
      // if (error) {
      //   Serial.println(F("Failed to read file, using default configuration"));
      //   return;
      // }

      // doc["timezone"] = received["dry"];
      // doc["settings"]["wetReadingAt3V"] = received["wet"];
      // doc["settings"]["batteryFull"] = received["battery"];
      // doc["settings"]["startCalibration"] = false;

      // file.close();                                                     // Close the file again

      // file = SPIFFS.open("/system/config.json", "w");                    // Open the file
      // serializeJson(doc, file);
      // file.close();

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

  // List directory
  server.on("/list", HTTP_GET, handleFileList);

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
