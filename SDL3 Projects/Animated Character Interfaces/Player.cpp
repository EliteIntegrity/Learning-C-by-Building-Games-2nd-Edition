#include "Player.h"

const float EDGE = 40.0f;   // how close her middle can get to a side

// She starts in the middle of the window, at the usual pace
Player::Player(const Texture& sheet, float groundY, float windowWidth)
    : Entity(sheet, windowWidth / 2.0f, groundY),
      maxX_(windowWidth - EDGE)
{
}

// Everything an Entity does, and then keep her in the window
void Player::update(float delta)
{
    Entity::update(delta);

    if (x_ < EDGE)
        x_ = EDGE;
    if (x_ > maxX_)
        x_ = maxX_;
}
