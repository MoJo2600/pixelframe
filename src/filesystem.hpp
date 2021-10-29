#ifndef FILESYSTEM_HPP_INCLUDED
#define FILESYSTEM_HPP_INCLUDED

// #include <LittleFS.h>
#include "LITTLEFS.h"
#include <vector>

using std::vector;

extern vector<String> gifs_vec;

extern bool fsOK;

String formatBytes(size_t bytes);

void startLittleFS();
#endif
