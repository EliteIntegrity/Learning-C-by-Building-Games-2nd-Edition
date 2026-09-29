#include "Ghost.h"

const float OFFSCREEN   = 120.0f;   // how far past an edge she goes, unseen
const float JUMP_CHANCE = 0.5f;     // jumps a second on the ground, on average

Ghost::Ghost(const Texture& sheet, float x, float groundY, int direction,
             float pace, SDL_Color tint, float windowWidth)
    : Entity(sheet, x, groundY, pace, tint),
      windowWidth_(windowWidth)
{
    run(direction);   // she never stops
}

void Ghost::update(float delta)
{
    // A jump now and then, at random
    if (SDL_randf() < JUMP_CHANCE * delta)
        jump();

    Entity::update(delta);

    // Off one side of the window, and back on at the other
    if (x_ > windowWidth_ + OFFSCREEN)
        x_ = -OFFSCREEN;
    else if (x_ < -OFFSCREEN)
        x_ = windowWidth_ + OFFSCREEN;
}
