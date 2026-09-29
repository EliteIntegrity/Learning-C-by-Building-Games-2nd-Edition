#pragma once
#include "Common.h"

class Map;

// Field of view: which cells the player can see from where they stand
namespace FOV
{
    void compute(Map& map, Point from, int radius);
}
