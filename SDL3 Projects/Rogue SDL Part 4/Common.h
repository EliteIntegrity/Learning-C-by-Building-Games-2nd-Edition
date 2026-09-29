#pragma once
#include <SDL3/SDL.h>
#include <functional>   // std::hash

// A place on the map, counted in cells, not pixels
struct Point
{
    int x = 0;
    int y = 0;

    bool operator==(const Point& other) const = default;
};

// The map is a grid of square cells, CELL_PX pixels across, MAP_W cells
// wide and MAP_H cells tall. Below it are HUD_ROWS rows of text, and the
// window is exactly the size of both: 1280 by 720 pixels
constexpr int CELL_PX = 16;
constexpr int MAP_W = 80;
constexpr int MAP_H = 40;
constexpr int HUD_ROWS = 5;
constexpr int WINDOW_W = MAP_W * CELL_PX;
constexpr int WINDOW_H = (MAP_H + HUD_ROWS) * CELL_PX;

// How to hash a Point, so that it can be a key in an unordered_map. Each
// cell gets its own number: the same number as its tile's index in the
// Map's vector
template <>
struct std::hash<Point>
{
    size_t operator()(const Point& point) const
    {
        return point.y * MAP_W + point.x;
    }
};

// How far the player can see, in cells
constexpr int SIGHT_RADIUS = 8;

// Every color in the game, in one place
namespace Palette
{
    constexpr SDL_Color BACKGROUND = { 10, 10, 16, 255 };
    constexpr SDL_Color WALL = { 180, 160, 110, 255 };
    constexpr SDL_Color FLOOR = { 110, 110, 130, 255 };
    constexpr SDL_Color PLAYER = { 255, 255, 255, 255 };

    // The stairs down
    constexpr SDL_Color STAIRS = { 240, 220, 80, 255 };

    // The colors of things remembered, but out of sight
    constexpr SDL_Color WALL_REMEMBERED = { 60, 55, 40, 255 };
    constexpr SDL_Color FLOOR_REMEMBERED = { 40, 40, 55, 255 };
    constexpr SDL_Color STAIRS_REMEMBERED = { 110, 100, 40, 255 };

    // The HUD's text
    constexpr SDL_Color TEXT = { 200, 200, 210, 255 };
    constexpr SDL_Color TEXT_DIM = { 100, 100, 110, 255 };
    constexpr SDL_Color TEXT_BAD = { 240, 80, 80, 255 };

    // Monsters and treasure
    constexpr SDL_Color RAT = { 180, 180, 100, 255 };
    constexpr SDL_Color GOBLIN = { 100, 220, 100, 255 };
    constexpr SDL_Color ORC = { 220, 100, 100, 255 };
    constexpr SDL_Color POTION = { 220, 80, 220, 255 };
    constexpr SDL_Color GOLD = { 240, 220, 80, 255 };
}
