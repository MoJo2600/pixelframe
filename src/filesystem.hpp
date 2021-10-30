#ifndef FILESYSTEM_HPP_INCLUDED
#define FILESYSTEM_HPP_INCLUDED

// Good tutorial https://www.mischianti.org/2021/04/01/esp32-integrated-littlefs-filesystem-5/

#include "FS.h"
#include <LITTLEFS.h>
#include <vector>

using std::vector;

extern vector<String> gifs_vec;

extern bool fsOK;

String formatBytes(size_t bytes);

void startLittleFS();
#endif
