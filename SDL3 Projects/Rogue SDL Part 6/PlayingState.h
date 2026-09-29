#pragma once
#include "GameState.h"

// Playing: moving, fighting, going downstairs, and saving and loading
class PlayingState : public GameState
{
public:
    StateChange handleKey(Game& game, SDL_Keycode key) override;
};
