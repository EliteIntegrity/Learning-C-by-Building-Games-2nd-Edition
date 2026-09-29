#pragma once
#include <SDL3/SDL.h>
#include "Animator.h"
#include "Texture.h"

// The runner: she runs left and right, turns to face the way she's
// going, and jumps
class Player
{
public:
    Player(const Texture& sheet, float groundY, float windowWidth);

    void run(int direction);
    void jump();
    void update(float delta);
    void draw(SDL_Renderer* renderer) const;

private:
    const Texture& sheet_;      // her sprite sheet, used but not owned
    Animator animator_;         // counts through her running frames
    float groundTop_;           // the top of her frame when she's standing
    float maxX_;                // the furthest right her middle can go
    float x_;                   // the middle of her body, across the window
    float y_;                   // the top of her frame
    float velY_ = 0.0f;         // pixels per second, and negative is up
    int direction_ = 0;         // -1 for left, 1 for right, 0 for standing
    bool facingLeft_ = false;
    bool jumping_ = false;
};
