#include "GameState.h"

// The step an arrow key, or W, A, S, or D, stands for, and no step at all,
// { 0, 0 }, for any other key
Point GameState::directionOf(SDL_Keycode key)
{
    switch (key)
    {
    case SDLK_UP:
    case SDLK_W:
        return { 0, -1 };
    case SDLK_DOWN:
    case SDLK_S:
        return { 0, 1 };
    case SDLK_LEFT:
    case SDLK_A:
        return { -1, 0 };
    case SDLK_RIGHT:
    case SDLK_D:
        return { 1, 0 };
    default:
        return { 0, 0 };
    }
}
