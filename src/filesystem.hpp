#ifndef FILESYSTEM_HPP_INCLUDED
#define FILESYSTEM_HPP_INCLUDED

#include <vector>

using std::vector;

vector<String> gifs_vec;

bool fsOK;

String formatBytes(size_t bytes) { // convert sizes in bytes to KB and MB
  if (bytes < 1024) {
    return String(bytes) + "B";
  } else if (bytes < (1024 * 1024)) {
    return String(bytes / 1024.0) + "KB";
  } else if (bytes < (1024 * 1024 * 1024)) {
    return String(bytes / 1024.0 / 1024.0) + "MB";
  }
}

void startLittleFS() { // Start the LittleFS and list all contents
  Serial.println(F("Inizializing FS..."));
  fsOK = LittleFS.begin();
  if (fsOK){
      Serial.println(F("done."));
  }else{
      Serial.println(F("fail."));
  }
  
  FSInfo fs_info;
  LittleFS.info(fs_info);

  Serial.println("File sistem info.");

  Serial.print("Total space:      ");
  Serial.print(fs_info.totalBytes);
  Serial.println("byte");

  Serial.print("Total space used: ");
  Serial.print(fs_info.usedBytes);
  Serial.println("byte");

  Serial.print("Block size:       ");
  Serial.print(fs_info.blockSize);
  Serial.println("byte");

  Serial.print("Page size:        ");
  Serial.print(fs_info.totalBytes);
  Serial.println("byte");

  Serial.print("Max open files:   ");
  Serial.println(fs_info.maxOpenFiles);

  Serial.print("Max path length:  ");
  Serial.println(fs_info.maxPathLength);

  Serial.println();
  
  Serial.println("LittleFS started. Contents:");
  {
    Dir dir = LittleFS.openDir("/gifs");
    while (dir.next()) {                      // List the file system contents
      String fileName = dir.fileName();
      gifs_vec.push_back("/gifs/"+fileName);
      size_t fileSize = dir.fileSize();
      Serial.printf("\tFS File: %s, size: %s\r\n", fileName.c_str(), formatBytes(fileSize).c_str());
    }
    Serial.printf("\n");
  }
}
#endif
