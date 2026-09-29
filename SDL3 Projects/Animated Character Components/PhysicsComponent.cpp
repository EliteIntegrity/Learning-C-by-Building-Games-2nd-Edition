#include "PhysicsComponent.h"
#include "GameObject.h"

// Running and jumping
const float RUN_SPEED  = 360.0f;    // pixels per second, at a pace of 1
const float JUMP_SPEED = -820.0f;   // pixels per second at take-off, upward
const float GRAVITY    = 2200.0f;   // pixels per second, per second

// The window's sides
const float EDGE      = 40.0f;      // how close to a side Stop lets her get
const float OFFSCREEN = 120.0f;     // how far past a side Wrap lets her go

PhysicsComponent::PhysicsComponent(float groundY, float windowWidth,
                                   Edges edges, float pace)
    : groundY_(groundY),
      windowWidth_(windowWidth),
      edges_(edges),
      pace_(pace)
{
}

// Which way to run: -1 for left, 1 for right, or 0 to stand still
void PhysicsComponent::run(int direction)
{
    direction_ = direction;
}

void PhysicsComponent::jump()
{
    if (!jumping_)
    {
        jumping_ = true;
        velY_ = JUMP_SPEED;
    }
}

int PhysicsComponent::getDirection() const
{
    return direction_;
}

bool PhysicsComponent::isJumping() const
{
    return jumping_;
}

float PhysicsComponent::getPace() const
{
    return pace_;
}

void PhysicsComponent::update(GameObject& owner, float delta)
{
    // Run at her own pace
    owner.x += direction_ * RUN_SPEED * pace_ * delta;

    // Rise, fall, and land, just as in Chapter 17
    if (jumping_)
    {
        velY_ += GRAVITY * delta;
        owner.y += velY_ * delta;
        if (owner.y >= groundY_)
        {
            owner.y = groundY_;   // she has landed
            velY_ = 0.0f;
            jumping_ = false;
        }
    }

    keepToEdges(owner);
}

// Stop at the sides, wrap around them, or pay them no attention
void PhysicsComponent::keepToEdges(GameObject& owner) const
{
    switch (edges_)
    {
    case Edges::Stop:
        if (owner.x < EDGE)
            owner.x = EDGE;
        if (owner.x > windowWidth_ - EDGE)
            owner.x = windowWidth_ - EDGE;
        break;

    case Edges::Wrap:
        if (owner.x > windowWidth_ + OFFSCREEN)
            owner.x = -OFFSCREEN;
        else if (owner.x < -OFFSCREEN)
            owner.x = windowWidth_ + OFFSCREEN;
        break;

    case Edges::Free:
        break;
    }
}
