#pragma once
#include "Entity.h"

// A rival runner, tinted and see-through, who runs on her own: round and
// round the window, with a jump now and then
class Ghost : public Entity
{
public:
    Ghost(const Texture& sheet, float x, float groundY, int direction,
          float pace, SDL_Color tint, float windowWidth);

    void update(float delta);

private:
    float windowWidth_;         // where she wraps around
};
