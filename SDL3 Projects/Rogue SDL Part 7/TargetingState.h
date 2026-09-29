#pragma once
#include "GameState.h"

// Aiming a scroll of fireball: the arrow keys move a cursor, Enter sends
// the fireball there, and Escape puts the scroll away unread
class TargetingState : public GameState
{
public:
    TargetingState(int slot, Point start);

    StateChange handleKey(Game& game, SDL_Keycode key) override;
    void draw(SDL_Renderer* renderer, const Game& game) const override;

private:
    int slot_;       // the scroll's slot in the inventory
    Point cursor_;   // the cell the fireball will burst on
};
