#include "FOV.h"
#include <algorithm>   // std::max
#include <cmath>   // std::round
#include <cstdlib>   // std::abs
#include "Map.h"

namespace
{
    // Walks a straight line from one cell toward another, a cell at a
    // time, and marks each cell seen, until the line leaves the circle of
    // sight or meets a wall. The wall is seen, but nothing behind it
    void castRay(Map& map, Point from, Point to, int radius)
    {
        int dx = to.x - from.x;
        int dy = to.y - from.y;
        int steps = std::max(std::abs(dx), std::abs(dy));

        for (int step = 1; step <= steps; ++step)
        {
            // This far along the line, rounded to the nearest cell
            float t = static_cast<float>(step) / steps;
            int offsetX = static_cast<int>(std::round(dx * t));
            int offsetY = static_cast<int>(std::round(dy * t));
            Point cell = { from.x + offsetX, from.y + offsetY };

            bool inCircle =
                offsetX * offsetX + offsetY * offsetY <= radius * radius;
            if (!inCircle || !map.isInside(cell))
                return;

            Tile& tile = map.at(cell);
            tile.visible = true;
            tile.explored = true;
            if (tile.terrain == Terrain::Wall)
                return;
        }
    }
}

namespace FOV
{
    // Marks every cell the player can see as visible, and explored, and
    // every other cell as not visible
    void compute(Map& map, Point from, int radius)
    {
        map.clearVisible();
        map.at(from).visible = true;
        map.at(from).explored = true;

        // A ray to every cell around the edge of the square that the
        // circle of sight fits in
        for (int i = -radius; i <= radius; ++i)
        {
            castRay(map, from, { from.x + i, from.y - radius }, radius);
            castRay(map, from, { from.x + i, from.y + radius }, radius);
            castRay(map, from, { from.x - radius, from.y + i }, radius);
            castRay(map, from, { from.x + radius, from.y + i }, radius);
        }
    }
}
