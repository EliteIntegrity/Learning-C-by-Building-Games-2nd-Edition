#pragma once
#include <SDL3/SDL.h>
#include "IDrawable.h"
#include "Texture.h"

// The landscape behind everything: four pictures, each filling the window,
// drawn from the back to the front. It never changes, so it's drawn, but
// never updated
class Scenery : public IDrawable
{
public:
    Scenery(SDL_Renderer* renderer);

    bool isLoaded() const;
    void draw(SDL_Renderer* renderer) const override;

private:
    Texture sky_;
    Texture farHills_;
    Texture nearHills_;
    Texture ground_;
};
