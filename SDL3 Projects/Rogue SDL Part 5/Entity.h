#pragma once
#include <SDL3/SDL.h>
#include "Common.h"

class GlyphCache;

// Anything that stands on the map, drawn as one character in one color
class Entity
{
public:
    Entity(Point position, char glyph, SDL_Color color);

    Point getPosition() const;
    void setPosition(Point position);
    void draw(SDL_Renderer* renderer, const GlyphCache& glyphs) const;

private:
    Point position_;
    char glyph_;
    SDL_Color color_;
};
