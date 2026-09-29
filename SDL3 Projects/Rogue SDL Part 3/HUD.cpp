#include "HUD.h"
#include "Common.h"
#include "GlyphCache.h"
#include "Player.h"

// The first row is how things stand, and the second is the keys, so the
// messages get the rows left over
constexpr int MESSAGE_ROWS = HUD_ROWS - 2;

// Adds a message at the bottom, and drops the oldest from the top when
// there are too many to show
void HUD::addMessage(const std::string& message)
{
    messages_.push_back(message);
    if (messages_.size() > MESSAGE_ROWS)
        messages_.pop_front();
}

void HUD::clear()
{
    messages_.clear();
}

void HUD::draw(SDL_Renderer* renderer, const GlyphCache& glyphs,
               const Player& player, int depth, bool gameOver) const
{
    int row = MAP_H;   // the first row below the map
    std::string status =
        "HP " + std::to_string(player.getHp()) + "/" +
        std::to_string(PLAYER_MAX_HP) +
        "   Gold " + std::to_string(player.getGold()) +
        "   Potions " + std::to_string(player.getPotions()) +
        "   Depth " + std::to_string(depth);
    glyphs.drawText(renderer, status, { 1, row }, Palette::TEXT);

    // The keys, or once the player has died, what to do about it
    std::string help = "Arrows or WASD: move and attack   H: drink a potion"
                       "   .: take the stairs down   Esc: quit";
    SDL_Color helpColor = Palette::TEXT_DIM;
    if (gameOver)
    {
        help = "You have died. Press R to play again, or Esc to quit.";
        helpColor = Palette::TEXT_BAD;
    }
    glyphs.drawText(renderer, help, { 1, row + 1 }, helpColor);

    // The newest message is bright, and the older ones are dim
    for (size_t i = 0; i < messages_.size(); ++i)
    {
        bool newest = i + 1 == messages_.size();
        glyphs.drawText(renderer, messages_[i],
                        { 1, row + 2 + static_cast<int>(i) },
                        newest ? Palette::TEXT : Palette::TEXT_DIM);
    }
}
