#include "Map.h"
#include "GlyphCache.h"

namespace
{
    // How each kind of terrain looks: its character, its color when it's
    // in sight, and its color when it's only remembered
    struct TerrainLook
    {
        char glyph;
        SDL_Color inSight;
        SDL_Color remembered;
    };

    // One for each kind of terrain, in the same order as the enum
    constexpr TerrainLook TERRAIN_LOOKS[] = {
        { '#', Palette::WALL, Palette::WALL_REMEMBERED },
        { '.', Palette::FLOOR, Palette::FLOOR_REMEMBERED },
        { '>', Palette::STAIRS, Palette::STAIRS_REMEMBERED }
    };
}

Map::Map()
    : tiles_(MAP_W * MAP_H)
{
}

// The tile at a cell. Row y starts y whole rows into the vector
Tile& Map::at(Point cell)
{
    return tiles_[cell.y * MAP_W + cell.x];
}

const Tile& Map::at(Point cell) const
{
    return tiles_[cell.y * MAP_W + cell.x];
}

bool Map::isInside(Point cell) const
{
    return cell.x >= 0 && cell.x < MAP_W && cell.y >= 0 && cell.y < MAP_H;
}

// Walls block the way, and so does everything outside the map
bool Map::isBlocked(Point cell) const
{
    return !isInside(cell) || at(cell).terrain == Terrain::Wall;
}

// Whether any of the eight cells around this one is open ground, rather
// than wall
bool Map::isNextToOpen(Point cell) const
{
    for (int dy = -1; dy <= 1; ++dy)
    {
        for (int dx = -1; dx <= 1; ++dx)
        {
            Point next = { cell.x + dx, cell.y + dy };
            if (isInside(next) && at(next).terrain != Terrain::Wall)
                return true;
        }
    }
    return false;
}

// Replaces every tile with a new one, made of the given terrain
void Map::fill(Terrain terrain)
{
    for (Tile& tile : tiles_)
        tile = { terrain };
}

void Map::clearVisible()
{
    for (Tile& tile : tiles_)
        tile.visible = false;
}

// Marks every tile explored, as if the player had seen the whole level.
// The rock still isn't drawn, since there's no open ground beside it
void Map::exploreAll()
{
    for (Tile& tile : tiles_)
        tile.explored = true;
}

// Draws every tile the player has explored: brightly if it's in sight, and
// dimly if it's only remembered. Solid rock, with no open ground beside
// it, isn't drawn at all
void Map::draw(SDL_Renderer* renderer, const GlyphCache& glyphs) const
{
    for (int y = 0; y < MAP_H; ++y)
    {
        for (int x = 0; x < MAP_W; ++x)
        {
            Point cell = { x, y };
            const Tile& tile = at(cell);
            bool isRock = tile.terrain == Terrain::Wall && !isNextToOpen(cell);
            if (!tile.explored || isRock)
                continue;

            int kind = static_cast<int>(tile.terrain);
            const TerrainLook& look = TERRAIN_LOOKS[kind];
            glyphs.draw(renderer, look.glyph, cell,
                        tile.visible ? look.inSight : look.remembered);
        }
    }
}
