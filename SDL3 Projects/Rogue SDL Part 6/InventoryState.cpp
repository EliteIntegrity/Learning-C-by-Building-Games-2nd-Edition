#include "InventoryState.h"
#include <string>   // std::string, for each line
#include <vector>   // std::vector, for the inventory
#include "Game.h"

// The box the inventory is shown in, in cells
constexpr int BOX_X = 22;
constexpr int BOX_Y = 6;
constexpr int BOX_W = 36;
constexpr int BOX_H = INVENTORY_SIZE + 6;

// A letter uses the item in the slot it stands for, and closes the
// inventory. Escape closes it without using anything
StateChange InventoryState::handleKey(Game& game, SDL_Keycode key)
{
    StateChange change;
    if (key == SDLK_ESCAPE)
    {
        change.close = true;
        return change;
    }

    // SDL's keycodes for the letters are in order, like the letters, so a
    // is slot 0, b is slot 1, and so on
    if (key < SDLK_A || key > SDLK_Z)
        return change;
    int slot = static_cast<int>(key - SDLK_A);
    if (slot >= static_cast<int>(game.getPlayer().getInventory().size()))
        return change;

    game.useItem(slot);
    change.close = true;
    return change;
}

// Dims the dungeon, and lists the inventory in a box over it
void InventoryState::draw(SDL_Renderer* renderer, const Game& game) const
{
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, Palette::SHADE.r, Palette::SHADE.g,
                           Palette::SHADE.b, Palette::SHADE.a);
    SDL_RenderFillRect(renderer, nullptr);

    SDL_FRect box = { BOX_X * CELL_PX, BOX_Y * CELL_PX, BOX_W * CELL_PX,
                      BOX_H * CELL_PX };
    SDL_SetRenderDrawColor(renderer, Palette::BACKGROUND.r,
                           Palette::BACKGROUND.g, Palette::BACKGROUND.b, 255);
    SDL_RenderFillRect(renderer, &box);
    SDL_SetRenderDrawColor(renderer, Palette::TEXT_DIM.r, Palette::TEXT_DIM.g,
                           Palette::TEXT_DIM.b, 255);
    SDL_RenderRect(renderer, &box);

    const GlyphCache& glyphs = game.getGlyphs();
    glyphs.drawText(renderer, "Inventory", { BOX_X + 2, BOX_Y + 1 },
                    Palette::TEXT);

    // Every item on a line of its own, after its letter
    const std::vector<ItemKind>& items = game.getPlayer().getInventory();
    if (items.empty())
    {
        glyphs.drawText(renderer, "Nothing yet.", { BOX_X + 2, BOX_Y + 3 },
                        Palette::TEXT_DIM);
    }
    for (size_t i = 0; i < items.size(); ++i)
    {
        char letter = static_cast<char>('a' + i);
        std::string line = std::string(1, letter) + ")  " +
                           statsOf(items[i]).name;
        glyphs.drawText(renderer, line,
                        { BOX_X + 2, BOX_Y + 3 + static_cast<int>(i) },
                        Palette::TEXT);
    }

    glyphs.drawText(renderer, "Press a letter to use an item, or Esc.",
                    { BOX_X + 2, BOX_Y + BOX_H - 2 }, Palette::TEXT_DIM);
}
