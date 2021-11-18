#include "filesystem.hpp"

using std::vector;

vector<String> gifs_vec = {};

bool fsOK = false;

String formatBytes(size_t bytes) { // convert sizes in bytes to KB and MB
  if (bytes < 1024) {
    return String(bytes) + "B";
  } else if (bytes < (1024 * 1024)) {
    return String(bytes / 1024.0) + "KB";
  } else if (bytes < (1024 * 1024 * 1024)) {
    return String(bytes / 1024.0 / 1024.0) + "MB";
  }
  return "0B";
}

void printDirectory(File dir, int numTabs) {
  while (true) {
 
    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++) {
      Serial.print('\t');
    }
    Serial.print(entry.name());
    if (entry.isDirectory()) {
      Serial.println("/");
      printDirectory(entry, numTabs + 1);
    } else {
      // files have sizes, directories do not
      Serial.print("\t\t");
      Serial.println(entry.size(), DEC);
    }
    entry.close();
  }
}

void startLittleFS() { // Start the LittleFS and list all contents
  Serial.println(F("Inizializing FS..."));
  fsOK = LITTLEFS.begin();
  if (fsOK){
      Serial.println(F("done."));
  }else{
      Serial.println(F("fail."));
  }

  unsigned int totalBytes = LITTLEFS.totalBytes();
  unsigned int usedBytes = LITTLEFS.usedBytes();

  Serial.println("File sistem info.");

  Serial.print("Total space:      ");
  Serial.print(totalBytes);
  Serial.println("byte");

  Serial.print("Total space used: ");
  Serial.print(usedBytes);
  Serial.println("byte");

  Serial.println();
  
  Serial.println("LittleFS started. Contents:");
  {
    File dir = LITTLEFS.open("/gifs");
    while (true) {
      File entry =  dir.openNextFile();
      if (! entry) {
        // no more files
        break;
      }

      String fileName = entry.name();
      if (entry.isDirectory()) {
        Serial.print("Directory found: ");
        Serial.println(fileName);
      } else {
        gifs_vec.push_back(fileName);
        size_t fileSize =  entry.size();
        Serial.printf("\tFS File: %s, size: %s\r\n", fileName.c_str(), formatBytes(fileSize).c_str());
      }
      entry.close();
    }
    Serial.printf("\n");
  }
}
