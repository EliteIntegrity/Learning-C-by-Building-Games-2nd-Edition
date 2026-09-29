#include "Player.h"
#include "Map.h"

Player::Player()
    : Entity({ 0, 0 }, '@', Palette::PLAYER)
{
}

// Steps one cell, unless the way is blocked. Returns true if the player
// moved
bool Player::tryMove(int dx, int dy, const Map& map)
{
    Point next = { getPosition().x + dx, getPosition().y + dy };
    if (map.isBlocked(next))
        return false;

    setPosition(next);
    return true;
}
