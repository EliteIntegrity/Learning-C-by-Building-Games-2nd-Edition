#include "Shadow.h"

const float GAP = 90.0f;                            // how far behind she keeps
const SDL_Color SHADOW_TINT = { 40, 40, 70, 150 };  // dark, and see-through

Shadow::Shadow(const Texture& sheet, const Entity& leader, float x,
               float groundY)
    : Entity(sheet, x, groundY, 1.0f, SHADOW_TINT),
      leader_(leader)
{
}

void Shadow::update(float delta)
{
    // Run after her leader until she's close behind, then wait
    float distance = leader_.getX() - x_;
    if (distance > GAP)
        run(1);
    else if (distance < -GAP)
        run(-1);
    else
        run(0);

    // Jump when she jumps
    if (leader_.isJumping())
        jump();

    Entity::update(delta);
}
