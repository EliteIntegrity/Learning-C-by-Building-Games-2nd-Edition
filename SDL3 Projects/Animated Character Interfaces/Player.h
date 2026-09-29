#pragma once
#include "Entity.h"

// The runner you control: an Entity who can't leave the window
class Player : public Entity
{
public:
    Player(const Texture& sheet, float groundY, float windowWidth);

    void update(float delta) override;

private:
    float maxX_;                // the furthest right her middle can go
};
