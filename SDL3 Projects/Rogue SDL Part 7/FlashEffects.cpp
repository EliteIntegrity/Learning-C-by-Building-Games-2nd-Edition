#include "FlashEffects.h"

// Starts a flash on a cell, now
void FlashEffects::add(Point cell, SDL_Color color)
{
    flashes_.push_back({ cell, color, SDL_GetTicks() });
}

bool FlashEffects::isEmpty() const
{
    return flashes_.empty();
}

// Forgets every flash that has faded away completely
void FlashEffects::removeFinished()
{
    Uint64 now = SDL_GetTicks();
    std::erase_if(flashes_, [now](const Flash& flash)
    {
        return now - flash.startMs >= FLASH_MS;
    });
}

// Fills each flashing cell with its color, see-through, and more so the
// older the flash is, until it's gone
void FlashEffects::draw(SDL_Renderer* renderer) const
{
    Uint64 now = SDL_GetTicks();
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    for (const Flash& flash : flashes_)
    {
        Uint64 age = now - flash.startMs;
        if (age >= FLASH_MS)
            continue;

        // From the color's own alpha when it starts, down to 0 at the end
        float left = 1.0f - static_cast<float>(age) / FLASH_MS;
        Uint8 alpha = static_cast<Uint8>(flash.color.a * left);
        SDL_SetRenderDrawColor(renderer, flash.color.r, flash.color.g,
                               flash.color.b, alpha);

        SDL_FRect box = { static_cast<float>(flash.cell.x * CELL_PX),
                          static_cast<float>(flash.cell.y * CELL_PX),
                          CELL_PX, CELL_PX };
        SDL_RenderFillRect(renderer, &box);
    }
}
