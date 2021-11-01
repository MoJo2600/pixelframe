#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
// #include <ESP8266mDNS.h>
#include <Wire.h>
#define FS_NO_GLOBALS
#include <LITTLEFS.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include "config.hpp"
#include "webserver.h"
#include "components/orchestrator.hpp"
#include "frames/clockframe.hpp"
#include "frames/gifframe.hpp"
#include "frames/visualsframe.hpp"
#include "frames/off.hpp"
#include "AsyncJson.h"

const char *fsName = "LittleFS";
FS *fileSystem = &LITTLEFS;
// LittleFSConfig fileSystemConfig = LittleFSConfig();

AsyncWebServer server(80);  // Create a webserver object that listens for HTTP request on port 80

char *mdnsName = "pixelframe"; // Domain name for the mDNS responder

static const char TEXT_PLAIN[] PROGMEM = "text/plain";
static const char APPLICATION_JSON[] PROGMEM = "application/json";
static const char FS_INIT_ERROR[] PROGMEM = "FS INIT ERROR";
static const char FILE_NOT_FOUND[] PROGMEM = "FileNotFound";


// Utils to return HTTP codes, and determine content-type
void replyOK(AsyncWebServerRequest *request)
{
  AsyncWebServerResponse *response = request->beginResponse(200, FPSTR(TEXT_PLAIN), "");
  request->send(response);
}

void replyOKWithMsg(AsyncWebServerRequest *request, String msg)
{
  AsyncWebServerResponse *response = request->beginResponse(200, FPSTR(TEXT_PLAIN), msg);
  request->send(response);
}

void replyOKWithJson(AsyncWebServerRequest *request, String serializedJson)
{
  AsyncWebServerResponse *response = request->beginResponse(200, FPSTR(APPLICATION_JSON), serializedJson);
  request->send(response);
}

void replyNotFound(AsyncWebServerRequest *request, String msg)
{
  AsyncWebServerResponse *response = request->beginResponse(404, FPSTR(TEXT_PLAIN), msg);
  request->send(response);
}

void replyBadRequest(AsyncWebServerRequest *request, String msg)
{
  AsyncWebServerResponse *response = request->beginResponse(400, FPSTR(TEXT_PLAIN), msg + "\r\n");
  request->send(response);
}

void replyServerError(AsyncWebServerRequest *request, String msg)
{
  AsyncWebServerResponse *response = request->beginResponse(500, FPSTR(TEXT_PLAIN), msg + "\r\n");
  request->send(response);
}

String getContentType(String filename)
{ // determine the filetype of a given filename, based on the extension
  if (filename.endsWith(".html"))
    return "text/html";
  else if (filename.endsWith(".css"))
    return "text/css";
  else if (filename.endsWith(".gif"))
    return "image/gif";
  else if (filename.endsWith(".js"))
    return "application/javascript";
  else if (filename.endsWith(".json"))
    return "application/json";
  else if (filename.endsWith(".ico"))
    return "image/x-icon";
  else if (filename.endsWith(".gz"))
    return "application/x-gzip";
  return "text/plain";
}

void handleGetFiles(AsyncWebServerRequest * request, String directory)
{
  Serial.println(String("[WEBSERVER] handleFileList: ") + directory);

  String path = "/gifs";
  File dir = fileSystem->open(path);

  // Another approach with lambda... i did not get this working: https://stackoverflow.com/questions/61559745/espasyncwebserver-serve-large-array-from-ram
  AsyncResponseStream *response = request->beginResponseStream("application/json");
  response->print('[');
  bool firstFile = true;

  while (true)
  {
    File entry = dir.openNextFile();
    if (!firstFile) {
      response->print(",");
    }
    response->print("{\"type\":\"");
    if (dir.isDirectory())
    {
      response->print("dir");
    }
    else
    {
      response->print(F("file\",\"size\":\""));
      response->print(dir.size());
    }
    response->print(F("\",\"name\":\""));
    // Always return names without leading "/"
    if (dir.name()[0] == '/')
    {
      response->print(&(dir.name()[1]));
    }
    else
    {
      response->print(dir.name());
    }
    response->print("\"}");
    firstFile = false;
  }
  // send last string
  response->print("]");
  request->send(response);
}

// void handleFileRead(AsyncWebServerRequest *request, String path)
// { // send the right file to the client (if it exists)
//   // String path = String(request->url());
//   Serial.println("[WEBSERVER] handleFileRead: " + path);
//   if (path.endsWith("/"))
//     path += "index.html";                    // If a folder is requested, send the index file
//   String contentType = getContentType(path); // Get the MIME type
//   request->send(LittleFS, path, contentType);
//   Serial.println(String("[WEBSERVER] Sent file: ") + path);
// }

bool handleStaticFile(AsyncWebServerRequest *request, String path) {
  
  if (path.endsWith("/")) path += F("index.html");
  
  Serial.println(String("[WEBSERVER] handleFileRead: ") + path);

  String contentType = getContentType(path);
  String pathWithGz = path + ".gz";
  
  if (fileSystem->exists(pathWithGz) || fileSystem->exists(path)) {
    bool gzipped = false;
    if (fileSystem->exists(pathWithGz)) {
        gzipped = true;
        path += ".gz";
    }
      
    Serial.println(String("[WEBSERVER] opening file: ") + path);
    File file = fileSystem->open(path, "r"); // e.g. SPIFFS.open(path, "r"); 

    AsyncWebServerResponse *response = request->beginResponse(
      contentType,
      file.size(),
      [file](uint8_t *buffer, size_t maxLen, size_t total) mutable -> size_t {
        int bytes = file.read(buffer, maxLen);
          
        // close file at the end
        if (bytes + total == file.size()) file.close();

        return max(0, bytes); // return 0 even when no bytes were loaded
      }
    );

    if (gzipped) {
      response->addHeader(F("Content-Encoding"), F("gzip"));
    }

    request->send(response);
    Serial.println(String("[WEBSERVER] sent file: ") + path);
    
    return true;
  }
  
  return false;
}

/*
   Return the list of files in the directory specified by the "dir" query string parameter.
   Also demonstrates the use of chuncked responses.
*/
void handleFileList(AsyncWebServerRequest *request)
{
  if (!request->hasParam("dir"))
  {
    replyBadRequest(request, F("DIR ARG MISSING"));
    return;
  }

  String path = request->getParam("dir")->value();
  handleGetFiles(request, path);
}

void startMDNS()
{ // Start the mDNS responder
  // start the multicast domain name server
  // TODO
  // if (!MDNS.begin(mdnsName))
  // {
  //   Serial.print("[WEBSERVER] Could not start MDNS service!");
  // }
  // else
  // {
  //   Serial.print("[WEBSERVER] mDNS responder started: http://");
  //   Serial.print(mdnsName);
  //   Serial.println(".local");
  // }
}

void startServer()
{
  // Start a HTTP server
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "*");
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Methods", "POST, GET, OPTIONS, PATCH, PUT");

  // List directory
  server.on("/api/files", HTTP_GET, handleFileList);

  // server.on("/api/show/clock", HTTP_GET, [](AsyncWebServerRequest *request) {
  //   Serial.println("[WEBSERVER] Receive command - switch to clock");

  //   auto ev = new ClockFrameEvent();
  //   Orchestrator::Instance()->react(ev);

  //   replyOKWithMsg(request, F("Switching to clock"));
  // });

  // server.on("/api/show/off", HTTP_GET, [](AsyncWebServerRequest *request) {
  //   Serial.println("[WEBSERVER] Receive command - switch to off");

  //   auto ev = new OffEvent();
  //   Orchestrator::Instance()->react(ev);

  //   replyOKWithMsg(request, F("Switching to off"));
  // });

  server.on("/api/show/gif", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->hasParam("image"))
    {
      String filename = request->getParam("image")->value();
      Serial.print("[WEBSERVER] Receive command - switch to ");
      Serial.println(filename);

      uint8_t duration = 10; // TODO: Put default gif duration somewhere more central?

      if (request->hasParam("duration"))
      {
        duration = request->getParam("duration")->value().toInt();
      }

      auto ev = new SingleGifFrameEvent(duration);

      ev->filename = std::string(filename.c_str());
      Orchestrator::Instance()->react(ev);
    }
    else
    {
      Serial.println("[WEBSERVER] Receive command - switch to random gif");

      auto ev = new RandomGifFrameEvent();
      Orchestrator::Instance()->react(ev);
    }

    replyOKWithMsg(request, F("Switching to gif"));
  });

  // server.on("/api/show/visuals", HTTP_GET, [](AsyncWebServerRequest *request) {
  //   if (request->hasParam("v"))
  //   {
  //     String visualArg = request->getParam("v")->value();
  //     Serial.print("[WEBSERVER] Receive command - switch to visual ");
  //     Serial.println(visualArg);

  //     auto ev = new VisualsFrameEvent();
  //     auto visual = std::string(visualArg.c_str());
  //     ev->visual = visual;
  //     Orchestrator::Instance()->react(ev);

  //     replyOKWithMsg(request, F("Switching to visual frame"));
  //   }
  //   else
  //   {
  //     Serial.println("[WEBSERVER] Receive command - switch to visuals frame");

  //     auto ev = new VisualsFrameEvent();
  //     ev->visual = "random";
  //     Orchestrator::Instance()->react(ev);

  //     replyOKWithMsg(request, F("Switching to random visual frame"));
  //   }
  // });

  server.on("/api/images", [](AsyncWebServerRequest *request) {
    Serial.println("[WEBSERVER] GET /api/images");
    if (request->hasParam("f")) {
      AsyncWebParameter* p = request->getParam("f");
      String name = p->value();
      String fullPath = "/gifs/" + name;
      // handleFileRead(request, fullPath);
      handleStaticFile(request, fullPath);
    } else {
      handleGetFiles(request, "/gifs");
    }
  });

  // server.on("/api/configuration/basic", HTTP_GET, [](AsyncWebServerRequest *request) {
  //   Serial.println("[WEBSERVER] GET /configuration/basic");

  //   // TODO: EspAsyncWebserver has a native implementation for JSON, so maybe we should this?
  //   StaticJsonDocument<200> config;
  //   config["brightness"] = matrix_brightness;
  //   config["timezone"] = "Europe/Berlin";

  //   char json_string[200];
  //   serializeJson(config, json_string);

  //   replyOKWithJson(request, String(json_string));
  // });

  // AsyncCallbackJsonWebHandler* configHandler = new AsyncCallbackJsonWebHandler("/api/configuration/basic", [](AsyncWebServerRequest *request, JsonVariant &config) {
  //   JsonObject jsonObj = config.as<JsonObject>();

  //   if (jsonObj["brightness"] != nullptr) {
  //     set_brightness(jsonObj["brightness"]);
  //   }

  //   replyOKWithMsg(request, F("Updating basic configuration"));
  // });
  // server.addHandler(configHandler);

  // server.on("/api/configuration/wifi", HTTP_GET, [](AsyncWebServerRequest *request) {
  //   Serial.println("[WEBSERVER] GET api/configuration/wifi");

  //   StaticJsonDocument<200> config;
  //   config["ssid"] = wifi_ssid;

  //   char json_string[200];
  //   serializeJson(config, json_string);

  //   replyOKWithJson(request, String(json_string));
  // });

  // AsyncCallbackJsonWebHandler* wifiConfigHandler = new AsyncCallbackJsonWebHandler("/api/configuration/wifi", [](AsyncWebServerRequest *request, JsonVariant &config) {
  //   JsonObject jsonObj = config.as<JsonObject>();

  //   if (jsonObj["ssid"] == nullptr || jsonObj["password"] == nullptr) {
  //     replyBadRequest(request, "Body must contain SSID and password");
  //     return;
  //   }

  //   replyOKWithMsg(request, F("Updating wifi configuration"));

  //   set_wifi(strdup(config["ssid"]), strdup(config["password"]));
  // });
  // server.addHandler(wifiConfigHandler);

  // server.on("/api/environment/wifis", HTTP_GET, [](AsyncWebServerRequest *request) {
  //   Serial.println("[WEBSERVER] GET api/environment/wifis");

  //   int numberOfNetworks = WiFi.scanNetworks();

  //   StaticJsonDocument<512> config;

  //   for(int i = 0; i < numberOfNetworks; i++) {
  //     config[i]["ssid"] = WiFi.SSID(i);
  //     config[i]["signalStrength"] = WiFi.RSSI(i);
  //   }

  //   char json_string[512];
  //   serializeJson(config, json_string);

  //   replyOKWithJson(request, String(json_string));
  // });

  server.onNotFound([](AsyncWebServerRequest *request) {
    if (request->method() == HTTP_OPTIONS) {
      request->send(200);
    } else if (request->method() == HTTP_GET) {
      if (!handleStaticFile(request, request->url())) {
        request->send(404);
      }
    } else {
      request->send(404);
    }
  });

  // server.onNotFound(handleNotFound); // if someone requests any other file or page, go to function 'handleNotFound'
  //                                    // and check if the file exists

  server.begin(); // start the HTTP server
  Serial.println("[WEBSERVER] HTTP server started.");
}

void setup_webserver()
{

  // TODO: Add to a IFDEF DEBUG somewhere for webserver debugging
  // int params = request->params();
  // for(int i=0;i<params;i++){
  //   AsyncWebParameter* p = request->getParam(i);
  //   if(p->isFile()){ //p->isPost() is also true
  //     Serial.printf("FILE[%s]: %s, size: %u\n", p->name().c_str(), p->value().c_str(), p->size());
  //   } else if(p->isPost()){
  //     Serial.printf("POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
  //   } else {
  //     Serial.printf("GET[%s]: %s\n", p->name().c_str(), p->value().c_str());
  //   }
  // }

  startMDNS();   // Start the mDNS responder
  //AsyncElegantOTA.begin(&server);    // Start ElegantOTA
  startServer(); // Start a HTTP server with a file read handler and an upload handler
}

void webserver_loop()
{
  //AsyncElegantOTA.loop();
  // TODO
  // MDNS.update();
}