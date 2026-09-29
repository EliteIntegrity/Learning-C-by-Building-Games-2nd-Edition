#pragma once
#include <SDL3/SDL.h>
#include "Animator.h"
#include "Texture.h"

// Anyone drawn from the runner's sprite sheet: she runs left and right at
// her own pace, turns to face the way she's going, and jumps
class Entity
{
public:
    Entity(const Texture& sheet, float x, float groundY, float pace = 1.0f,
           SDL_Color tint = { 255, 255, 255, 255 });

    void run(int direction);
    void jump();
    void update(float delta);
    void draw(SDL_Renderer* renderer) const;

protected:
    float x_;                   // the middle of her body, across the window

private:
    const Texture& sheet_;      // the runner's sprite sheet, used but not owned
    Animator animator_;         // counts through her running frames
    float groundTop_;           // the top of her frame when she's standing
    float y_;                   // the top of her frame
    float pace_;                // 1 for the usual speed, 2 for twice as fast
    SDL_Color tint_;            // her color, and how see-through she is
    float velY_ = 0.0f;         // pixels per second, and negative is up
    int direction_ = 0;         // -1 for left, 1 for right, 0 for standing
    bool facingLeft_ = false;
    bool jumping_ = false;
};
