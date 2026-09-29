#include "GhostInput.h"
#include <SDL3/SDL.h>
#include "GameObject.h"

const float JUMP_CHANCE = 0.5f;   // jumps a second on the ground, on average

GhostInput::GhostInput(int direction) : direction_(direction)
{
}

void GhostInput::update(GameObject& owner, float delta)
{
    // She never stops, and now and then, at random, she jumps
    owner.getPhysics().run(direction_);
    if (SDL_randf() < JUMP_CHANCE * delta)
        owner.getPhysics().jump();
}
