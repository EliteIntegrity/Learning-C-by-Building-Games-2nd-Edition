#pragma once
#include <SDL3/SDL.h>

// Anything that can draw itself
class IDrawable
{
public:
    virtual ~IDrawable() = default;

    virtual void draw(SDL_Renderer* renderer) const = 0;
};
