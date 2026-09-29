#pragma once
#include <SDL3/SDL.h>
#include "GlyphCache.h"
#include "HUD.h"
#include "Map.h"
#include "Player.h"

// The whole game. It owns the glyphs, the map, the player, and the HUD,
// and runs the loop that waits for a key, acts on it, and draws what
// happened
class Game
{
public:
    Game(SDL_Renderer* renderer);

    bool isLoaded() const;
    void run();

private:
    void newLevel();
    void handleKey(SDL_Keycode key);
    void takeStairs();
    void draw() const;

    SDL_Renderer* renderer_;
    GlyphCache glyphs_;
    Map map_;
    Player player_;
    HUD hud_;
    int depth_ = 1;          // how many levels down the player is
    bool running_ = true;
    bool dirty_ = true;      // true when the window needs drawing again
};
