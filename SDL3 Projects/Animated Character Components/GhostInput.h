#pragma once
#include "InputComponent.h"

// A ghost's own whims: she runs one way forever, with a jump now and then
class GhostInput : public InputComponent
{
public:
    GhostInput(int direction);

    void update(GameObject& owner, float delta) override;

private:
    int direction_;             // -1 for left, 1 for right
};
