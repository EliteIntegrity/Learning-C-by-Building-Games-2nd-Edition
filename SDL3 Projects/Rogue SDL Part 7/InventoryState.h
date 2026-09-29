#pragma once
#include "GameState.h"

// Looking through the inventory. Each item has a letter, and pressing it
// uses the item. Escape closes the inventory
class InventoryState : public GameState
{
public:
    StateChange handleKey(Game& game, SDL_Keycode key) override;
    void draw(SDL_Renderer* renderer, const Game& game) const override;
};
