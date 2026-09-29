#pragma once
#include <SDL3/SDL.h>
#include "IDrawable.h"
#include "IUpdatable.h"
#include "Texture.h"

// The frame rate, counted over each second, and drawn from a strip of
// digits in the window's top-left corner. It changes and it's drawn, like
// a runner, though it's nothing like one
class HUD : public IUpdatable, public IDrawable
{
public:
    HUD(const Texture& digits);

    void update(float delta) override;
    void draw(SDL_Renderer* renderer) const override;

private:
    void drawNumber(SDL_Renderer* renderer, int value, float x,
                    float y) const;

    const Texture& digits_;   // the strip of digits, used but not owned
    float timer_ = 0.0f;      // seconds since fps_ was last worked out
    int frames_ = 0;          // frames counted in that time
    int fps_ = 0;             // frames in the last whole second
};
