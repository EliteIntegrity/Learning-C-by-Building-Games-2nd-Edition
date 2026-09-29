#pragma once

// Anything that changes from frame to frame
class IUpdatable
{
public:
    virtual ~IUpdatable() = default;

    virtual void update(float delta) = 0;
};
