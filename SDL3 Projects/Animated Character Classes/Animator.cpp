#include "Animator.h"

Animator::Animator(int frameCount, float frameTime)
    : frameCount_(frameCount), frameTime_(frameTime)
{
}

// Chapter 17's accumulator: add up the time, and move on one frame for
// every frameTime_ collected
void Animator::update(float delta)
{
    timer_ += delta;
    while (timer_ >= frameTime_)
    {
        timer_ -= frameTime_;
        frame_ = (frame_ + 1) % frameCount_;
    }
}

int Animator::getFrame() const
{
    return frame_;
}
