#pragma once
#include <SDL3/SDL.h>

// A place on the map, counted in cells, not pixels
struct Point
{
    int x = 0;
    int y = 0;
};

// The map is a grid of square cells, CELL_PX pixels across, MAP_W cells
// wide and MAP_H cells tall, and the window is exactly its size: 1280 by
// 720 pixels
constexpr int CELL_PX = 16;
constexpr int MAP_W = 80;
constexpr int MAP_H = 45;
constexpr int WINDOW_W = MAP_W * CELL_PX;
constexpr int WINDOW_H = MAP_H * CELL_PX;

// Every color in the game, in one place
namespace Palette
{
    constexpr SDL_Color BACKGROUND = { 10, 10, 16, 255 };
    constexpr SDL_Color WALL = { 180, 160, 110, 255 };
    constexpr SDL_Color FLOOR = { 110, 110, 130, 255 };
    constexpr SDL_Color PLAYER = { 255, 255, 255, 255 };
}
