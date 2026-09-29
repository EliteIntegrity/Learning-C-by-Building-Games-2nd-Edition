#include "MapGenerator.h"
#include <algorithm>   // std::min and std::max
#include <cstdlib>   // std::abs
#include "Map.h"

// An area is cut in two only if its longer side is at least twice this, so
// both pieces are at least this long
constexpr int MIN_AREA = 10;
// and after this many cuts, it isn't cut again, however big it is
constexpr int MAX_CUTS = 6;
// The smallest room, in cells
constexpr int MIN_ROOM_W = 4;
constexpr int MIN_ROOM_H = 3;

namespace
{
    // A random whole number from low to high, including both
    int randomBetween(int low, int high)
    {
        if (high <= low)
            return low;
        return low + SDL_rand(high - low + 1);
    }

    Point centerOf(SDL_Rect room)
    {
        return { room.x + room.w / 2, room.y + room.h / 2 };
    }
}

MapGenerator::MapGenerator(Map& map)
    : map_(map)
{
}

// Builds the level, and returns the cell where the player starts
Point MapGenerator::generate()
{
    map_.fill(Terrain::Wall);
    rooms_.clear();
    split({ 0, 0, MAP_W, MAP_H }, 0);

    // Join each room to the one made after it, so that every room can be
    // reached from every other
    for (size_t i = 1; i < rooms_.size(); ++i)
        carveCorridor(centerOf(rooms_[i - 1]), centerOf(rooms_[i]));

    // Start in the middle of a room chosen at random
    int roomCount = static_cast<int>(rooms_.size());
    Point start = centerOf(rooms_[SDL_rand(roomCount)]);

    // and put the stairs in the middle of the room farthest away from it
    Point stairs = start;
    int farthest = 0;
    for (const SDL_Rect& room : rooms_)
    {
        Point center = centerOf(room);
        int distance = std::abs(center.x - start.x) +
                       std::abs(center.y - start.y);
        if (distance > farthest)
        {
            farthest = distance;
            stairs = center;
        }
    }
    map_.at(stairs).terrain = Terrain::StairsDown;

    return start;
}

// Cuts an area in two, then cuts each piece in two, and so on. An area
// that's too small to cut, or has been cut out by MAX_CUTS cuts, gets a
// room instead
void MapGenerator::split(SDL_Rect area, int cuts)
{
    // Cut across the longer side, so that the pieces don't get too thin
    bool cutAcross = area.h > area.w;
    int length = cutAcross ? area.h : area.w;
    if (cuts == MAX_CUTS || length < MIN_AREA * 2)
    {
        addRoom(area);
        return;
    }

    int cut = randomBetween(MIN_AREA, length - MIN_AREA);
    if (cutAcross)
    {
        split({ area.x, area.y, area.w, cut }, cuts + 1);
        split({ area.x, area.y + cut, area.w, area.h - cut }, cuts + 1);
    }
    else
    {
        split({ area.x, area.y, cut, area.h }, cuts + 1);
        split({ area.x + cut, area.y, area.w - cut, area.h }, cuts + 1);
    }
}

// Carves a room of a random size somewhere inside an area, with at least
// one cell of wall between it and the area's edges
void MapGenerator::addRoom(SDL_Rect area)
{
    SDL_Rect room;
    room.w = randomBetween(MIN_ROOM_W, area.w - 2);
    room.h = randomBetween(MIN_ROOM_H, area.h - 2);
    room.x = randomBetween(area.x + 1, area.x + area.w - room.w - 1);
    room.y = randomBetween(area.y + 1, area.y + area.h - room.h - 1);

    carve({ room.x, room.y }, { room.x + room.w - 1, room.y + room.h - 1 });
    rooms_.push_back(room);
}

// Turns every cell in the box with corners a and b into floor. When a and
// b are in the same row, or the same column, the box is a straight line
void MapGenerator::carve(Point a, Point b)
{
    for (int y = std::min(a.y, b.y); y <= std::max(a.y, b.y); ++y)
    {
        for (int x = std::min(a.x, b.x); x <= std::max(a.x, b.x); ++x)
            map_.at({ x, y }).terrain = Terrain::Floor;
    }
}

// Carves an L-shaped corridor between two cells, turning its corner at
// one end or the other, chosen at random
void MapGenerator::carveCorridor(Point from, Point to)
{
    Point corner = { to.x, from.y };
    if (SDL_rand(2) == 0)
        corner = { from.x, to.y };

    carve(from, corner);
    carve(corner, to);
}
