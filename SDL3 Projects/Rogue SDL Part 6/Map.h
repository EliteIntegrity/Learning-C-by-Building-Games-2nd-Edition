#pragma once
#include <SDL3/SDL.h>
#include <vector>   // std::vector, for the tiles
#include "Common.h"

class GlyphCache;

// What a cell of the map is made of
enum class Terrain
{
    Wall,
    Floor,
    StairsDown
};

// One cell of the map
struct Tile
{
    Terrain terrain = Terrain::Wall;
    bool explored = false;   // the player has seen it, at least once
    bool visible = false;    // the player can see it right now
};

// The dungeon: a grid of tiles, MAP_W across and MAP_H down, kept in one
// vector, a row at a time
class Map
{
public:
    Map();

    Tile& at(Point cell);
    const Tile& at(Point cell) const;
    bool isInside(Point cell) const;
    bool isBlocked(Point cell) const;
    bool isNextToOpen(Point cell) const;

    void fill(Terrain terrain);
    void clearVisible();
    void exploreAll();
    void draw(SDL_Renderer* renderer, const GlyphCache& glyphs) const;

private:
    std::vector<Tile> tiles_;
};
