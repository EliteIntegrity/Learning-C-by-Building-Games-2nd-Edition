#pragma once
#include <SDL3/SDL.h>
#include <deque>   // std::deque, for the messages
#include <string>   // std::string, for the messages

class GlyphCache;
class Player;

// The rows of text below the map: how things stand, which keys do what,
// and the last few things that happened, with the newest at the bottom
class HUD
{
public:
    void addMessage(const std::string& message);
    void clear();
    void draw(SDL_Renderer* renderer, const GlyphCache& glyphs,
              const Player& player, int depth, bool gameOver) const;

private:
    std::deque<std::string> messages_;
};
