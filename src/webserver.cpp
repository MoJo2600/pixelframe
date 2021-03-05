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
#include <FastLED.h>
// #include <pixelframe.hpp>

const char* fsName = "LittleFS";
FS* fileSystem = &LittleFS;
LittleFSConfig fileSystemConfig = LittleFSConfig();

ESP8266WebServer server(80);       // Create a webserver object that listens for HTTP request on port 80
// WebSocketsServer webSocket(81);    // create a websocket server on port 81

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
    if (LittleFS.exists(pathWithGz))                         // If there's a compressed version available
      path += ".gz";                                         // Use the compressed verion
    File file = LittleFS.open(path, "r");                    // Open the file
    size_t sent = server.streamFile(file, contentType);    // Send it to the client
    file.close();                                          // Close the file again
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

  // Gif handling
  server.on(UriBraces("/api/show/{}"), HTTP_GET, []() {
    String name = server.pathArg(0);
    // if (name == "toggle") {
    //   ToggleEvent event;
    //   fsm_handle::dispatch(event);
    // }
    replyOKWithMsg(F("Everyday I'm toggeling"));
  });

  // Gif handling
  server.on("/api/play", HTTP_GET, []() {
    if (!server.hasArg("image")) {
      return replyBadRequest(F("IMAGE ARG MISSING"));
    }
    String path = server.arg("image");

    // PlayGifEvent playGif;
    // playGif.file = String("/gifs/")+ path;

    // ???
    // fsm_handle::dispatch(playGif);

    replyOKWithMsg(F("Playing file"));
  });

  server.on(UriBraces("/api/images/{}"), []() {
    String name = server.pathArg(0);
    handleFileRead("gifs/"+ name);
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
