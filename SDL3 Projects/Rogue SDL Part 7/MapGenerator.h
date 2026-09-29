#pragma once
#include <SDL3/SDL.h>
#include <vector>   // std::vector, for the rooms
#include "Common.h"

class Enemy;
class Item;
class Map;

// Builds a new level on a map: it splits the map into areas, puts a room
// in each one, joins the rooms with corridors, puts stairs down in one of
// them, and fills the rooms with monsters and treasure
class MapGenerator
{
public:
    MapGenerator(Map& map);

    Point generate(int depth, std::vector<Enemy>& enemies,
                   std::vector<Item>& items);

private:
    void split(SDL_Rect area, int cuts);
    void addRoom(SDL_Rect area);
    void carve(Point a, Point b);
    void carveCorridor(Point from, Point to);
    Point takeFreeSpot(bool awayFromStart);
    void addMonsters(int depth, std::vector<Enemy>& enemies);
    void addTreasure(int depth, std::vector<Item>& items);

    Map& map_;                      // the map being built, not owned
    std::vector<SDL_Rect> rooms_;   // every room, in the order it was made
    int startRoom_ = 0;             // the room the player starts in
    std::vector<Point> taken_;      // cells that already have something
};
