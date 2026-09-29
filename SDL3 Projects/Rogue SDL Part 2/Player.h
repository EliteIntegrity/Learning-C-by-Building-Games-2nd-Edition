#pragma once
#include "Entity.h"

class Map;

// You: the @
class Player : public Entity
{
public:
    Player();

    bool tryMove(int dx, int dy, const Map& map);
};
