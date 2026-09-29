#pragma once

// Counts through the frames of an animation at a steady rate, however
// quickly or slowly the game's own frames come
class Animator
{
public:
    Animator(int frameCount, float frameTime);

    void update(float delta);
    int getFrame() const;

private:
    int frameCount_;
    float frameTime_;      // seconds to show each frame for
    int frame_ = 0;        // which frame to show now
    float timer_ = 0.0f;   // seconds spent on that frame so far
};
