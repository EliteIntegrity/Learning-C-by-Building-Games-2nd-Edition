#pragma once
#include <SDL3/SDL.h>
#include "GlyphCache.h"
#include "Map.h"
#include "Player.h"

// The whole game. It owns the glyphs, the map, and the player, and runs
// the loop that waits for a key, acts on it, and draws what happened
class Game
{
public:
    Game(SDL_Renderer* renderer);

    bool isLoaded() const;
    void run();

private:
    void makeTestRoom();
    void handleKey(SDL_Keycode key);
    void draw() const;

    SDL_Renderer* renderer_;
    GlyphCache glyphs_;
    Map map_;
    Player player_;
    bool running_ = true;
    bool dirty_ = true;      // true when the window needs drawing again
};
