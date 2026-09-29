#pragma once
#include "InputComponent.h"

// The player at the keyboard: the arrow keys, or A and D, to run, and Space,
// W, or the up arrow to jump
class KeyboardInput : public InputComponent
{
public:
    void update(GameObject& owner, float delta) override;

private:
    bool jumpWasDown_ = false;  // whether a jump key was down last frame
};
