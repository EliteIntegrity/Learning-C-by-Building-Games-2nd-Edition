#include "HUD.h"
#include <string>   // std::string and std::to_string

const float DIGIT_W    = 26.0f;   // digits.png is ten digits, each
const float DIGIT_H    = 40.0f;   // 26 by 40, from 0 to 9
const float HUD_MARGIN = 16.0f;   // pixels from the window's edges

HUD::HUD(const Texture& digits) : digits_(digits)
{
}

// Count the frames, and each time a whole second has passed, show the count
void HUD::update(float delta)
{
    frames_++;
    timer_ += delta;
    if (timer_ >= 1.0f)
    {
        fps_ = frames_;
        frames_ = 0;
        timer_ -= 1.0f;
    }
}

void HUD::draw(SDL_Renderer* renderer) const
{
    drawNumber(renderer, fps_, HUD_MARGIN, HUD_MARGIN);
}

// Chapter 17's drawNumber, as a private member function: a whole number,
// digit by digit, with its top-left corner at (x, y)
void HUD::drawNumber(SDL_Renderer* renderer, int value, float x,
                     float y) const
{
    std::string text = std::to_string(value);
    for (char c : text)
    {
        int digit = c - '0';   // the characters '0' to '9' become 0 to 9
        SDL_FRect src = { digit * DIGIT_W, 0.0f, DIGIT_W, DIGIT_H };
        SDL_FRect dst = { x, y, DIGIT_W, DIGIT_H };
        digits_.draw(renderer, &src, &dst);
        x += DIGIT_W;
    }
}
