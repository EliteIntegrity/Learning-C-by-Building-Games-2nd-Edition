#include "TargetingState.h"
#include "Game.h"

TargetingState::TargetingState(int slot, Point start)
    : slot_(slot), cursor_(start)
{
}

// The arrows, or W, A, S, and D, move the cursor. Enter casts the fireball
// on it, and Escape puts the scroll away, unread
StateChange TargetingState::handleKey(Game& game, SDL_Keycode key)
{
    StateChange change;

    // A key that stands for a step moves the cursor, but never off the map
    Point step = directionOf(key);
    Point next = { cursor_.x + step.x, cursor_.y + step.y };
    if (game.getMap().isInside(next))
        cursor_ = next;

    if (key == SDLK_RETURN || key == SDLK_KP_ENTER)
        change.close = game.castFireball(slot_, cursor_);
    else if (key == SDLK_ESCAPE)
        change.close = true;
    return change;
}

// Shows where the fireball would burn, and a box around the cursor, with
// the keys in a banner along the top of the window
void TargetingState::draw(SDL_Renderer* renderer, const Game& game) const
{
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, Palette::BLAST.r, Palette::BLAST.g,
                           Palette::BLAST.b, Palette::BLAST.a);
    for (int dy = -FIREBALL_RADIUS; dy <= FIREBALL_RADIUS; ++dy)
    {
        for (int dx = -FIREBALL_RADIUS; dx <= FIREBALL_RADIUS; ++dx)
        {
            Point cell = { cursor_.x + dx, cursor_.y + dy };
            if (!game.canBurn(cell, cursor_))
                continue;

            SDL_FRect box = { static_cast<float>(cell.x * CELL_PX),
                              static_cast<float>(cell.y * CELL_PX),
                              CELL_PX, CELL_PX };
            SDL_RenderFillRect(renderer, &box);
        }
    }

    SDL_FRect cursor = { static_cast<float>(cursor_.x * CELL_PX),
                         static_cast<float>(cursor_.y * CELL_PX),
                         CELL_PX, CELL_PX };
    SDL_SetRenderDrawColor(renderer, Palette::TARGET.r, Palette::TARGET.g,
                           Palette::TARGET.b, 255);
    SDL_RenderRect(renderer, &cursor);

    SDL_FRect banner = { 0, 0, WINDOW_W, 2 * CELL_PX };
    SDL_SetRenderDrawColor(renderer, Palette::BACKGROUND.r,
                           Palette::BACKGROUND.g, Palette::BACKGROUND.b, 255);
    SDL_RenderFillRect(renderer, &banner);
    game.getGlyphs().drawText(renderer,
                              "Aim with the arrows or WASD   Enter: cast the "
                              "fireball   Esc: put the scroll away",
                              { 1, 1 }, Palette::TARGET);
}
