#include "frames/visuals/visual.hpp"
#include <iostream>

Visual::~Visual() {}

VisualFactory::map_type * VisualFactory::map = NULL;