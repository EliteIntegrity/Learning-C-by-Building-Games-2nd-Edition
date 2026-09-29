#pragma once
#include <SDL3/SDL.h>
#include <vector>   // std::vector, for the rooms
#include "Common.h"

class Map;

// Builds a new level on a map: it splits the map into areas, puts a room
// in each one, joins the rooms with corridors, and puts stairs down in one
// of them
class MapGenerator
{
public:
    MapGenerator(Map& map);

    Point generate();

private:
    void split(SDL_Rect area, int cuts);
    void addRoom(SDL_Rect area);
    void carve(Point a, Point b);
    void carveCorridor(Point from, Point to);

    Map& map_;                      // the map being built, not owned
    std::vector<SDL_Rect> rooms_;   // every room, in the order it was made
};
