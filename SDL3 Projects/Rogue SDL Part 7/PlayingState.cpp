#include "PlayingState.h"
#include <memory>   // std::make_unique
#include "Game.h"
#include "InventoryState.h"

// Every key press is one action, or none
StateChange PlayingState::handleKey(Game& game, SDL_Keycode key)
{
    StateChange change;

    // Once the player has died, only two keys do anything
    if (game.isGameOver())
    {
        if (key == SDLK_R)
            game.newGame();
        else if (key == SDLK_ESCAPE)
            game.quit();
        return change;
    }

    // A key that stands for a step moves the player, or attacks
    Point step = directionOf(key);
    if (step != Point{ 0, 0 })
    {
        game.moveOrAttack(step.x, step.y);
        return change;
    }

    switch (key)
    {
    case SDLK_I:
        change.open = std::make_unique<InventoryState>();
        break;
    case SDLK_PERIOD:
        game.takeStairs();
        break;
    case SDLK_F5:
        game.saveGame();
        break;
    case SDLK_F9:
        game.loadGame();
        break;
    case SDLK_ESCAPE:
        game.quit();
        break;
    }
    return change;
}
