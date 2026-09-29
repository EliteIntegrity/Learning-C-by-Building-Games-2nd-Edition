#pragma once
#include <SDL3/SDL.h>
#include <vector>   // std::vector, for the flashes
#include "Common.h"

// How long a flash takes to fade away, in milliseconds
constexpr Uint64 FLASH_MS = 200;

// A cell lit up in a color for a moment
struct Flash
{
    Point cell;
    SDL_Color color;
    Uint64 startMs;   // when it began, from SDL_GetTicks
};

// Short flashes of color over cells: red where something is hit, green
// where the player heals. Each one fades away over FLASH_MS
class FlashEffects
{
public:
    void add(Point cell, SDL_Color color);
    bool isEmpty() const;
    void removeFinished();
    void draw(SDL_Renderer* renderer) const;

private:
    std::vector<Flash> flashes_;
};
