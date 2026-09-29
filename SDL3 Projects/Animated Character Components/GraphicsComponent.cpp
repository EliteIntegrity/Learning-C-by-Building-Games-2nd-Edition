#include "GraphicsComponent.h"
#include "GameObject.h"

// The runner's sprite sheet
const float FRAME_W     = 112.0f;   // runner.png is six frames, each
const float FRAME_H     = 180.0f;   // 112 by 180
const int   RUN_FRAMES  = 6;
const int   JUMP_FRAME  = 2;        // her longest stride, as in Chapter 17
const int   STAND_FRAME = 4;        // the frame with her feet closest
const float FRAME_TIME  = 0.08f;    // seconds per frame
const float FEET_GAP    = 2.0f;     // empty pixels below her feet
const float HIPS_X      = 74.0f;    // her middle, from the frame's left edge

GraphicsComponent::GraphicsComponent(const Texture& sheet, SDL_Color tint)
    : sheet_(sheet),
      animator_(RUN_FRAMES, FRAME_TIME),
      tint_(tint)
{
}

// Keep her legs in step with her running, and face the way she's going
void GraphicsComponent::update(const GameObject& owner, float delta)
{
    const PhysicsComponent& physics = owner.getPhysics();
    int direction = physics.getDirection();
    if (direction != 0)
    {
        animator_.update(delta * physics.getPace());
        facingLeft_ = direction < 0;
    }
}

void GraphicsComponent::draw(SDL_Renderer* renderer,
                             const GameObject& owner) const
{
    // In the air, running, or standing still
    const PhysicsComponent& physics = owner.getPhysics();
    int frame = STAND_FRAME;
    if (physics.isJumping())
        frame = JUMP_FRAME;
    else if (physics.getDirection() != 0)
        frame = animator_.getFrame();

    // Her middle is HIPS_X from the frame's left edge, or from its right
    // edge when the frame is mirrored, so the frame moves to keep her
    // middle at x
    float left = owner.x - HIPS_X;
    SDL_FlipMode flip = SDL_FLIP_NONE;
    if (facingLeft_)
    {
        left = owner.x - (FRAME_W - HIPS_X);
        flip = SDL_FLIP_HORIZONTAL;
    }

    // Her feet are at y, so the frame's top is its height above them
    float top = owner.y + FEET_GAP - FRAME_H;

    SDL_FRect src = { frame * FRAME_W, 0.0f, FRAME_W, FRAME_H };
    SDL_FRect dst = { left, top, FRAME_W, FRAME_H };
    sheet_.draw(renderer, &src, &dst, flip, tint_);
}
