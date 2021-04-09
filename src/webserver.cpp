#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <uri/UriBraces.h>
#include <uri/UriRegex.h>
#include <ESP8266mDNS.h>
#include <Wire.h>
#define FS_NO_GLOBALS
#include <LittleFS.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include "config.hpp"
#include "webserver.h"
#include "components/orchestrator.hpp"
#include "frames/clockframe.hpp"
#include "frames/gifframe.hpp"
#include "frames/visualsframe.hpp"
#include "frames/off.hpp"

const char* fsName = "LittleFS";
FS* fileSystem = &LittleFS;
LittleFSConfig fileSystemConfig = LittleFSConfig();

ESP8266WebServer server(80);       // Create a webserver object that listens for HTTP request on port 80
// WebSocketsServer webSocket(81);    // create a websocket server on port 81

char* mdnsName = "pixelframe"; // Domain name for the mDNS responder

static const char TEXT_PLAIN[] PROGMEM = "text/plain";
static const char APPLICATION_JSON[] PROGMEM = "application/json";
static const char FS_INIT_ERROR[] PROGMEM = "FS INIT ERROR";
static const char FILE_NOT_FOUND[] PROGMEM = "FileNotFound";

////////////////////////////////
// Utils to return HTTP codes, and determine content-type

void replyOK() {
  server.send(200, FPSTR(TEXT_PLAIN), "");
}

void replyOKWithMsg(String msg) {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, FPSTR(TEXT_PLAIN), msg);
}

void replyOKWithJson(String serializedJson) {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, FPSTR(APPLICATION_JSON), serializedJson);
}

void replyNotFound(String msg) {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(404, FPSTR(TEXT_PLAIN), msg);
}

void replyBadRequest(String msg) {
  Serial.println(msg);
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(400, FPSTR(TEXT_PLAIN), msg + "\r\n");
}

void replyServerError(String msg) {
  Serial.println(msg);
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(500, FPSTR(TEXT_PLAIN), msg + "\r\n");
}

String getContentType(String filename) { // determine the filetype of a given filename, based on the extension
  if (filename.endsWith(".html")) return "text/html";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".js")) return "application/javascript";
  else if (filename.endsWith(".json")) return "application/json";
  else if (filename.endsWith(".ico")) return "image/x-icon";
  else if (filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}

bool handleFileRead(String path) { // send the right file to the client (if it exists)
  Serial.println("[WEBSERVER] handleFileRead: " + path);
  if (path.endsWith("/")) path += "index.html";          // If a folder is requested, send the index file
  String contentType = getContentType(path);             // Get the MIME type
  String pathWithGz = path + ".gz";
  if (LittleFS.exists(pathWithGz) || LittleFS.exists(path)) { // If the file exists, either as a compressed archive, or normal
    if (LittleFS.exists(pathWithGz))                          // If there's a compressed version available
      path += ".gz";                                          // Use the compressed verion
    File file = LittleFS.open(path, "r");                     // Open the file
    server.streamFile(file, contentType);                     // Send it to the client
    file.close();                                             // Close the file again
    Serial.println(String("\t[WEBSERVER] Sent file: ") + path);
    return true;
  }
  Serial.println(String("\t[WEBSERVER]  File Not Found: ") + path);   // If the file doesn't exist, return false
  return false;
}

void handleGetFiles(String path) {
  if (path != "/" && !fileSystem->exists(path)) {
    return replyBadRequest("BAD PATH");
  }

  Serial.println(String("[WEBSERVER] handleFileList: ") + path);
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

/*
   Return the list of files in the directory specified by the "dir" query string parameter.
   Also demonstrates the use of chuncked responses.
*/
void handleFileList() {
  if (!server.hasArg("dir")) {
    return replyBadRequest(F("DIR ARG MISSING"));
  }

  String path = server.arg("dir");

  handleGetFiles(path);
}

void handleNotFound(){ // if the requested file or page doesn't exist, return a 404 not found error
  if(!handleFileRead(server.uri())){          // check if the file exists in the flash memory (LittleFS), if so, send it
    server.send(404, "text/plain", "404: File Not Found");
  }
}

void startMDNS() { // Start the mDNS responder
  // start the multicast domain name server
  if (!MDNS.begin(mdnsName)) {
    Serial.print("[WEBSERVER] Could not start MDNS service!");
  }
  else
  {
    Serial.print("[WEBSERVER] mDNS responder started: http://");
    Serial.print(mdnsName);
    Serial.println(".local");
  }
}

void startServer() { // Start a HTTP server with a file read handler and an upload handler

  // List directory
  server.on("/api/files", HTTP_GET, handleFileList);

  server.on(UriBraces("/api/show/clock"), HTTP_GET, []() {
    Serial.println("[WEBSERVER] Receive command - switch to clock");

    auto ev = new ClockFrameEvent();
    Orchestrator::Instance()->react(ev);
    
    replyOKWithMsg(F("Switching to clock"));
  });


  server.on(UriBraces("/api/show/off"), HTTP_GET, []() {
    Serial.println("[WEBSERVER] Receive command - switch to off");

    auto ev = new OffEvent();
    Orchestrator::Instance()->react(ev);

    replyOKWithMsg(F("Switching to off"));
  });


  server.on(UriBraces("/api/show/gif"), HTTP_GET, []() {
    if (server.hasArg("image")) {
      String filename = server.arg("image");
      Serial.print("[WEBSERVER] Receive command - switch to ");
      Serial.println(filename);

      uint8_t duration = 10; // TODO: Put default gif duration somewhere more central?

      if (server.hasArg("duration")) {
        duration = server.arg("duration").toInt();
      }

      auto ev = new SingleGifFrameEvent(duration);

      ev->filename = std::string(filename.c_str());
      Orchestrator::Instance()->react(ev);
    } else {
      Serial.println("[WEBSERVER] Receive command - switch to random gif");

      auto ev = new RandomGifFrameEvent();
      Orchestrator::Instance()->react(ev);
    }

    replyOKWithMsg(F("Switching to gif"));
  });

  server.on(UriBraces("/api/show/visuals"), HTTP_GET, []() {
    Serial.println("[WEBSERVER] Receive command - switch to visuals frame");

    auto ev = new VisualsFrameEvent();
    Orchestrator::Instance()->react(ev);

    replyOKWithMsg(F("Switching to visual frame"));
  });

  server.on(UriBraces("/api/images/{}"), []() {
    String name = server.pathArg(0);
    handleFileRead("gifs/"+ name);
  });

  server.on(UriBraces("/api/configuration/basic"), HTTP_GET, []() {
    Serial.println("[WEBSERVER] GET /configuration/basic");

    StaticJsonDocument<200> config;
    config["brightness"] = matrix_brightness;
    config["timezone"] = "Europe/Berlin";

    char json_string[200];
    serializeJson(config, json_string);

    replyOKWithJson(String(json_string));
  });

  server.on(UriBraces("/api/configuration/basic"), HTTP_PATCH, []() {
    Serial.println("[WEBSERVER] PATCH /configuration/basic");

    StaticJsonDocument<200> config;

    DeserializationError error = deserializeJson(config, server.arg("plain"));

    if (error) {
      replyBadRequest(F("Unable to parse body"));
      return;
    }

    if (config["brightness"] != nullptr) {
      set_brightness(config["brightness"]);
    }

    replyOKWithMsg(F("Updating basic configuration"));
  });

  server.on(UriBraces("/api/configuration/wifi"), HTTP_GET, []() {
    Serial.println("[WEBSERVER] GET api/configuration/wifi");

    StaticJsonDocument<200> config;
    config["ssid"] = wifi_ssid;

    char json_string[200];
    serializeJson(config, json_string);

    replyOKWithJson(String(json_string));
  });

  server.on(UriBraces("/api/configuration/wifi"), HTTP_PUT, []() {
    Serial.println("[WEBSERVER] PUT /configuration/wifi");

    StaticJsonDocument<200> config;

    DeserializationError error = deserializeJson(config, server.arg("plain"));

    if (error) {
      replyBadRequest(F("Unable to parse body"));
      return;
    }

    if (config["ssid"] == nullptr || config["password"] == nullptr) {
      replyBadRequest("Body must contain SSID and password");
      return;
    }

    set_wifi(strdup(config["ssid"]), strdup(config["password"]));

    replyOKWithMsg(F("Updating wifi configuration"));
  });

  server.on(UriBraces("/api/environment/wifis"), HTTP_GET, []() {
    Serial.println("[WEBSERVER] GET api/environment/wifis");

    int numberOfNetworks = WiFi.scanNetworks();

    StaticJsonDocument<512> config;

    for(int i = 0; i < numberOfNetworks; i++) {
      config[i]["ssid"] = WiFi.SSID(i);
      config[i]["signalStrength"] = WiFi.RSSI(i);
    }

    char json_string[512];
    serializeJson(config, json_string);

    replyOKWithJson(String(json_string));
  });

  server.on("/api/images", HTTP_GET, []() {
    handleGetFiles("/gifs");
  });

  server.onNotFound(handleNotFound);          // if someone requests any other file or page, go to function 'handleNotFound'
                                              // and check if the file exists

  server.begin();                             // start the HTTP server
  Serial.println("[WEBSERVER] HTTP server started.");
}


void setup_webserver() {
  startMDNS();                 // Start the mDNS responder
  startServer();               // Start a HTTP server with a file read handler and an upload handler
}

void webserver_loop() {
  server.handleClient();                      // run the server
  MDNS.update();
}
