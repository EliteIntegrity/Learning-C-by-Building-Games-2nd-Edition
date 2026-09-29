#pragma once
#include "Entity.h"

// A dark, see-through runner who follows another wherever she goes, and
// jumps whenever she jumps
class Shadow : public Entity
{
public:
    Shadow(const Texture& sheet, const Entity& leader, float x,
           float groundY);

    void update(float delta) override;

private:
    const Entity& leader_;      // the runner she follows, used but not owned
};
