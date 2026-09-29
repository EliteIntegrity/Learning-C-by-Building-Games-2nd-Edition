#include "Entity.h"

// The runner's sprite sheet
const float FRAME_W     = 112.0f;   // runner.png is six frames, each
const float FRAME_H     = 180.0f;   // 112 by 180
const int   RUN_FRAMES  = 6;
const int   JUMP_FRAME  = 2;        // her longest stride, as in Chapter 17
const int   STAND_FRAME = 4;        // the frame with her feet closest
const float FRAME_TIME  = 0.08f;    // seconds per frame
const float FEET_GAP    = 2.0f;     // empty pixels below her feet
const float HIPS_X      = 74.0f;    // her middle, from the frame's left edge

// Running and jumping
const float RUN_SPEED  = 360.0f;    // pixels per second, at a pace of 1
const float JUMP_SPEED = -820.0f;   // pixels per second at take-off, upward
const float GRAVITY    = 2200.0f;   // pixels per second, per second

Entity::Entity(const Texture& sheet, float x, float groundY, float pace,
               SDL_Color tint)
    : x_(x),
      sheet_(sheet),
      animator_(RUN_FRAMES, FRAME_TIME),
      groundTop_(groundY + FEET_GAP - FRAME_H),
      y_(groundTop_),
      pace_(pace),
      tint_(tint)
{
}

// Which way to run: -1 for left, 1 for right, or 0 to stand still
void Entity::run(int direction)
{
    direction_ = direction;
    if (direction != 0)
        facingLeft_ = direction < 0;
}

void Entity::jump()
{
    if (!jumping_)
    {
        jumping_ = true;
        velY_ = JUMP_SPEED;
    }
}

void Entity::update(float delta)
{
    // Run at her own pace, with her legs keeping up
    x_ += direction_ * RUN_SPEED * pace_ * delta;
    if (direction_ != 0)
        animator_.update(delta * pace_);

    // Rise, fall, and land, just as in Chapter 17
    if (jumping_)
    {
        velY_ += GRAVITY * delta;
        y_ += velY_ * delta;
        if (y_ >= groundTop_)
        {
            y_ = groundTop_;   // she has landed
            velY_ = 0.0f;
            jumping_ = false;
        }
    }
}

void Entity::draw(SDL_Renderer* renderer) const
{
    // In the air, running, or standing still
    int frame = STAND_FRAME;
    if (jumping_)
        frame = JUMP_FRAME;
    else if (direction_ != 0)
        frame = animator_.getFrame();

    // Her middle is HIPS_X from the frame's left edge, or from its right
    // edge when the frame is mirrored, so the frame moves to keep her
    // middle at x_
    float left = x_ - HIPS_X;
    SDL_FlipMode flip = SDL_FLIP_NONE;
    if (facingLeft_)
    {
        left = x_ - (FRAME_W - HIPS_X);
        flip = SDL_FLIP_HORIZONTAL;
    }

    SDL_FRect src = { frame * FRAME_W, 0.0f, FRAME_W, FRAME_H };
    SDL_FRect dst = { left, y_, FRAME_W, FRAME_H };
    sheet_.draw(renderer, &src, &dst, flip, tint_);
}
