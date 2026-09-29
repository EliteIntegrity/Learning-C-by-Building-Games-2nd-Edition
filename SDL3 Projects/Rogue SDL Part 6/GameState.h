#pragma once
#include <SDL3/SDL.h>
#include <memory>   // std::unique_ptr, for the state to open
#include "Common.h"

class Game;
class GameState;

// What a state asks to have done to the stack, once it has dealt with a
// key: to be closed, taken off the top, or to have another state opened
// on top, or both, or neither
struct StateChange
{
    bool close = false;
    std::unique_ptr<GameState> open;
};

// A mode the game can be in, such as playing, or looking through the
// inventory. The game keeps a stack of them. Every key goes to the state
// on top, and every state draws, from the bottom up, over the dungeon
class GameState
{
public:
    virtual ~GameState() = default;

    virtual StateChange handleKey(Game& game, SDL_Keycode key) = 0;

    // Draws whatever the state adds to the picture: nothing, unless a
    // state says otherwise
    virtual void draw(SDL_Renderer*, const Game&) const {}

protected:
    static Point directionOf(SDL_Keycode key);
};
