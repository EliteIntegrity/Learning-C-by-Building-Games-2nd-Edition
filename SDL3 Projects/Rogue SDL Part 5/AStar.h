#pragma once
#include <vector>   // std::vector, for the path
#include "Common.h"

class Map;

// A*, the shortest way from one cell to another, around the walls
namespace AStar
{
    std::vector<Point> findPath(const Map& map, Point from, Point to);
}
